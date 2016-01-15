#ifndef __DALI_TOOLKIT_IMAGE_LOAD_THREAD_H__
#define __DALI_TOOLKIT_IMAGE_LOAD_THREAD_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/devel-api/adaptor-framework/bitmap-loader.h>
#include <dali/devel-api/adaptor-framework/event-thread-callback.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * The task of loading and packing an image into the atlas.
 */
struct LoadingTask
{
  /**
   * Constructor.
   */
  LoadingTask( BitmapLoader loader, uint32_t packPositionX, uint32_t packPositionY, uint32_t width, uint32_t height  );

private:

  // Undefined
  LoadingTask( const LoadingTask& queue );

  // Undefined
  LoadingTask& operator=( const LoadingTask& queue );

public:

  BitmapLoader   loader;    ///< The loader used to load the bitmap from URL
  Rect<uint32_t> packRect;  ///< The x coordinate of the position to pack the image.

};

/**
 * The queue of the tasks waiting to load the bitmap from the URL in the worker thread/
 */
class LoadQueue //: public TaskQueue
{
public:

  /**
   * Constructor
   */
  LoadQueue();

  /**
   * Destructor.
   */
  ~LoadQueue();

  /**
   * Pop the next task out from the queue.
   *
   * @return The next task to be processed.
   */
  LoadingTask* NextTask();

  /**
   * Add a task in to the queue
   *
   * @param[in] task The task added to the queue.
   */
  void AddTask( LoadingTask* task );

private:

  // Undefined
  LoadQueue( const LoadQueue& queue );

  // Undefined
  LoadQueue& operator=( const LoadQueue& queue );

private:

  Vector< LoadingTask* > mTasks;
  ConditionalWait mConditionalWait;
};

/**
 * The queue of the tasks, with the image loaded, waiting for the main thread to upload the bitmap.
 */
class CompleteQueue //: public TaskQueue
{
public:

  /**
   * Constructor
   *
   * @param[in] mTrigger The trigger to wake up the main thread.
   */
  CompleteQueue( EventThreadCallback* mTrigger );

  /**
   * Destructor.
   */
  ~CompleteQueue();

  /**
   * Pop the next task out from the queue.
   *
   * @return The next task to be processed.
   */
  LoadingTask* NextTask();

  /**
   * Add a task in to the queue
   *
   * @param[in] task The task added to the queue.
   */
  void AddTask( LoadingTask* task );

private:

  // Undefined
  CompleteQueue( const CompleteQueue& queue );

  // Undefined
  CompleteQueue& operator=( const CompleteQueue& queue );

private:

  Vector< LoadingTask* > mTasks;
  Dali::Mutex            mMutex;
  EventThreadCallback*   mTrigger;
};

/**
 * The worker thread for image loading.
 */
class ImageLoadThread : public Thread
{
public:

  /**
   * Constructor.
   *
   * @param[in] loadQueue The task queue with images for loading.
   * @param[in] completeQurue The task queue with images loaded.
   */
  ImageLoadThread( LoadQueue& loadQueue, CompleteQueue& completeQueue );

  /**
   * Destructor.
   */
  virtual ~ImageLoadThread();

protected:

  /**
   * The entry function of the worker thread.
   * It fetches loading task from the loadQueue, loads the image and adds to the completeQueue.
   */
  virtual void Run();

private:

  // Undefined
  ImageLoadThread( const ImageLoadThread& thread );

  // Undefined
  ImageLoadThread& operator=( const ImageLoadThread& thread );

private:

  LoadQueue& mLoadQueue;          ///<The task queue with images for loading.
  CompleteQueue& mCompleteQueue;  ///<The task queue with images loaded.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_IMAGE_LOAD_THREAD_H__ */
