#ifndef __DALI_TOOLKIT_SVG_RASTERIZE_THREAD_H__
#define __DALI_TOOLKIT_SVG_RASTERIZE_THREAD_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/devel-api/rendering/texture-set.h>
#include <dali/devel-api/images/pixel-data.h>
#include <dali/public-api/images/buffer-image.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/devel-api/adaptor-framework/event-thread-callback.h>

struct NSVGimage;
struct NSVGrasterizer;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class SvgRenderer;
typedef IntrusivePtr< SvgRenderer > SvgRendererPtr;
class RasterizingTask;
typedef IntrusivePtr< RasterizingTask > RasterizingTaskPtr;

/**
 * The svg rasterizing tasks to be processed in the worker thread.
 *
 * Life cycle of a rasterizing task is as follows:
 * 1. Created by SvgRenderer in the main thread
 * 2. Queued in the worked thread waiting to be processed.
 * 3. If this task gets its turn to do the rasterization, it triggers main thread to apply the rasterized image to material then been deleted in main thread call back
 *    Or if this task is been removed ( new image/size set to the renderer or actor off stage) before its turn to be processed, it then been deleted in the worker thread.
 */
class RasterizingTask : public RefObject
{
public:
  /**
   * Constructor
   *
   * @param[in] svgRenderer The renderer which the rasterized image to be applied.
   * @param[in] parsedSvg The parsed svg for rasterizing.
   *            Note, after the task is added to the worker thread, the worker thread takes over the ownership.
   *            When the image is to be deleted, delete it in the worker thread by calling SvgRasterizeThread::DeleteImage( parsedSvg ).
   * @param[in] width The rasterization width.
   * @param[in] height The rasterization height.
   */
  RasterizingTask( SvgRenderer* svgRenderer, NSVGimage* parsedSvg, unsigned int width, unsigned int height );

  /**
   * Do the rasterization with the given rasterizer.
   *@param[in] rasterizer The rasterizer that rasterize the SVG to a buffer image
   */
  void Rasterize( NSVGrasterizer* rasterizer );

  /**
   * Get the svg renderer
   */
  SvgRenderer* GetSvgRenderer() const;

  /**
   * Get the rasterization result.
   * @return The pixel data with the rasterized pixels.
   */
  PixelDataPtr GetPixelData() const;

private:

  // Undefined
  RasterizingTask( const RasterizingTask& task );

  // Undefined
  RasterizingTask& operator=( const RasterizingTask& task );

private:
  SvgRendererPtr  mSvgRenderer;
  PixelDataPtr    mPixelData;
  NSVGimage*      mParsedSvg;
  unsigned int    mWidth;
  unsigned int    mHeight;
};


/**
 * The worker thread for SVG rasterization.
 */
class SvgRasterizeThread : public Thread
{
public:

  /**
   * Constructor.
   *
   * @param[in] trigger The trigger to wake up the main thread.
   */
  SvgRasterizeThread( EventThreadCallback* trigger );

  /**
   * Terminate the svg rasterize thread, join and delete.
   */
  static void TerminateThread( SvgRasterizeThread*& thread );

  /**
   * Add a rasterization task into the waiting queue, called by main thread.
   *
   * @param[in] task The task added to the queue.
   */
  void AddTask( RasterizingTaskPtr task );

  /**
   * Pop the next task out from the completed queue, called by main thread.
   *
   * @return The next task in the completed queue.
   */
  RasterizingTaskPtr NextCompletedTask();

  /**
   * Remove the task with the given renderer from the waiting queue, called by main thread.
   *
   * Typically called when the actor is put off stage, so the renderer is not needed anymore.
   *
   * @param[in] renderer The renderer pointer.
   */
  void RemoveTask( SvgRenderer* renderer );

  /**
   * Delete the parsed SVG image, called by main thread.
   *
   * The parsed svg should be delelted in worker thread, as the main thread does not know whether a rasterization of this svg is ongoing.
   *
   * @param[in] parsedImage The image to be deleted
   */
  void DeleteImage( NSVGimage* parsedSvg );

private:

  /**
   * Pop the next task out from the queue.
   *
   * @return The next task to be processed.
   */
  RasterizingTaskPtr NextTaskToProcess();

  /**
   * Add a task in to the queue
   *
   * @param[in] task The task added to the queue.
   */
  void AddCompletedTask( RasterizingTaskPtr task );

protected:

  /**
   * Destructor.
   */
  virtual ~SvgRasterizeThread();


  /**
   * The entry function of the worker thread.
   * It fetches task from the Queue, rasterizes the image and apply to the renderer.
   */
  virtual void Run();

private:

  // Undefined
  SvgRasterizeThread( const SvgRasterizeThread& thread );

  // Undefined
  SvgRasterizeThread& operator=( const SvgRasterizeThread& thread );

private:

  std::vector<RasterizingTaskPtr>  mRasterizeTasks;     //The queue of the tasks waiting to rasterize the SVG image
  std::vector <RasterizingTaskPtr> mCompletedTasks;     //The queue of the tasks with the SVG rasterization completed
  Vector<NSVGimage*>               mDeleteSvg;          //The images that the event thread requested to delete

  ConditionalWait            mConditionalWait;
  Dali::Mutex                mMutex;
  EventThreadCallback*       mTrigger;

  NSVGrasterizer*            mRasterizer;
  bool                       mIsThreadWaiting;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_SVG_RASTERIZE_THREAD_H__ */
