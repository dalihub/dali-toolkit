#ifndef DALI_TOOLKIT_SVG_RASTERIZE_THREAD_H
#define DALI_TOOLKIT_SVG_RASTERIZE_THREAD_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/event-thread-callback.h>
#include <dali/devel-api/adaptor-framework/vector-image-renderer.h>
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/integration-api/adaptor-framework/log-factory-interface.h>
#include <dali/integration-api/processor-interface.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/object/ref-object.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-url.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class SvgVisual;
typedef IntrusivePtr<SvgVisual> SvgVisualPtr;
class SvgTask;
typedef IntrusivePtr<SvgTask> SvgTaskPtr;

/**
 * The svg rasterizing tasks to be processed in the worker thread.
 *
 * Life cycle of a rasterizing task is as follows:
 * 1. Created by SvgVisual in the main thread
 * 2. Queued in the worked thread waiting to be processed.
 * 3. If this task gets its turn to do the rasterization, it triggers main thread to apply the rasterized image to material then been deleted in main thread call back
 *    Or if this task is been removed ( new image/size set to the visual or actor off stage) before its turn to be processed, it then been deleted in the worker thread.
 */
class SvgTask : public RefObject
{
public:
  /**
   * Constructor
   * @param[in] svgVisual The visual which the rasterized image to be applied.
   * @param[in] vectorRenderer The vector rasterizer.
   */
  SvgTask(SvgVisual* svgVisual, VectorImageRenderer vectorRenderer);

  /**
   * Destructor.
   */
  virtual ~SvgTask() = default;

  /**
   * Process the task
   */
  virtual void Process() = 0;

  /**
   * Whether the task has succeeded.
   * @return True if the task has succeeded.
   */
  bool HasSucceeded() const;

  /**
   * Get the svg visual
   */
  SvgVisual* GetSvgVisual() const;

  /**
   * Get the rasterization result.
   * @return The pixel data with the rasterized pixels.
   */
  virtual PixelData GetPixelData() const;

private:
  // Undefined
  SvgTask(const SvgTask& task) = delete;

  // Undefined
  SvgTask& operator=(const SvgTask& task) = delete;

protected:
  SvgVisualPtr        mSvgVisual;
  VectorImageRenderer mVectorRenderer;
  bool                mHasSucceeded;
};

class SvgLoadingTask : public SvgTask
{
public:
  /**
   * Constructor
   * @param[in] svgVisual The visual which the rasterized image to be applied.
   * @param[in] vectorRenderer The vector rasterizer.
   * @param[in] url The URL to svg resource to use.
   * @param[in] dpi The DPI of the screen.
   */
  SvgLoadingTask(SvgVisual* svgVisual, VectorImageRenderer vectorRenderer, const VisualUrl& url, float dpi);

  /**
   * Destructor.
   */
  ~SvgLoadingTask() override;

  /**
   * Process the task
   */
  void Process() override;

private:
  // Undefined
  SvgLoadingTask(const SvgLoadingTask& task) = delete;

  // Undefined
  SvgLoadingTask& operator=(const SvgLoadingTask& task) = delete;

private:
  VisualUrl mUrl;
  float     mDpi;
};

class SvgRasterizingTask : public SvgTask
{
public:
  /**
   * Constructor
   * @param[in] svgVisual The visual which the rasterized image to be applied.
   * @param[in] vectorRenderer The vector rasterizer.
   * @param[in] width The rasterization width.
   * @param[in] height The rasterization height.
   */
  SvgRasterizingTask(SvgVisual* svgVisual, VectorImageRenderer vectorRenderer, unsigned int width, unsigned int height);

  /**
   * Destructor.
   */
  ~SvgRasterizingTask() override;

  /**
   * Process the task accodring to the type
   */
  void Process() override;

  /**
   * Get the rasterization result.
   * @return The pixel data with the rasterized pixels.
   */
  PixelData GetPixelData() const override;

private:
  // Undefined
  SvgRasterizingTask(const SvgRasterizingTask& task) = delete;

  // Undefined
  SvgRasterizingTask& operator=(const SvgRasterizingTask& task) = delete;

private:
  PixelData mPixelData;
  uint32_t  mWidth;
  uint32_t  mHeight;
};

/**
 * The worker thread for SVG rasterization.
 */
class SvgRasterizeThread : public Thread, Integration::Processor
{
public:
  /**
   * Constructor.
   *
   * @param[in] trigger The trigger to wake up the main thread.
   */
  SvgRasterizeThread();

  /**
   * Terminate the svg rasterize thread, join and delete.
   */
  static void TerminateThread(SvgRasterizeThread*& thread);

  /**
   * Add a rasterization task into the waiting queue, called by main thread.
   *
   * @param[in] task The task added to the queue.
   */
  void AddTask(SvgTaskPtr task);

  /**
   * Pop the next task out from the completed queue, called by main thread.
   *
   * @return The next task in the completed queue.
   */
  SvgTaskPtr NextCompletedTask();

  /**
   * Remove the task with the given visual from the waiting queue, called by main thread.
   *
   * Typically called when the actor is put off stage, so the renderer is not needed anymore.
   *
   * @param[in] visual The visual pointer.
   */
  void RemoveTask(SvgVisual* visual);

  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  void Process(bool postProcessor) override;

private:
  /**
   * Pop the next task out from the queue.
   *
   * @return The next task to be processed.
   */
  SvgTaskPtr NextTaskToProcess();

  /**
   * Add a task in to the queue
   *
   * @param[in] task The task added to the queue.
   */
  void AddCompletedTask(SvgTaskPtr task);

  /**
   * Applies the rasterized image to material
   */
  void ApplyRasterizedSVGToSampler();

  /**
   * @brief Unregister a previously registered processor
   *
   */
  void UnregisterProcessor();

protected:
  /**
   * Destructor.
   */
  ~SvgRasterizeThread() override;

  /**
   * The entry function of the worker thread.
   * It fetches task from the Queue, rasterizes the image and apply to the renderer.
   */
  void Run() override;

private:
  // Undefined
  SvgRasterizeThread(const SvgRasterizeThread& thread);

  // Undefined
  SvgRasterizeThread& operator=(const SvgRasterizeThread& thread);

private:
  std::vector<SvgTaskPtr> mRasterizeTasks; //The queue of the tasks waiting to rasterize the SVG image
  std::vector<SvgTaskPtr> mCompletedTasks; //The queue of the tasks with the SVG rasterization completed

  ConditionalWait                      mConditionalWait;
  Dali::Mutex                          mMutex;
  std::unique_ptr<EventThreadCallback> mTrigger;
  const Dali::LogFactoryInterface&     mLogFactory;
  bool                                 mIsThreadWaiting;
  bool                                 mProcessorRegistered;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_SVG_RASTERIZE_THREAD_H
