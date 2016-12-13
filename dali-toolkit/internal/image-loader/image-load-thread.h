#ifndef __DALI_TOOLKIT_IMAGE_LOAD_THREAD_H__
#define __DALI_TOOLKIT_IMAGE_LOAD_THREAD_H__

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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/threading/thread.h>
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
   * @param [in] id of the task
   * @param [in] url The URL of the image file to load.
   * @param [in] size The width and height to fit the loaded image to, 0.0 means whole image
   * @param [in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter.
   * @param [in] samplingMode The filtering method used when sampling pixels from the input image while fitting it to desired size.
   * @param [in] orientationCorrection Reorient the image to respect any orientation metadata in its header.
   */
  LoadingTask( uint32_t id, const std::string& url, ImageDimensions dimensions,
               FittingMode::Type fittingMode, SamplingMode::Type samplingMode, bool orientationCorrection );

  /**
   * Load the image
   */
  void Load();

private:

  // Undefined
  LoadingTask( const LoadingTask& queue );

  // Undefined
  LoadingTask& operator=( const LoadingTask& queue );

public:

  PixelData pixelData;             ///< pixelData handle after successfull load
  std::string url;                 ///< url of the image to load
  uint32_t     id;                 ///< The unique id associated with this task.
  ImageDimensions dimensions;      ///< dimensions to load
  FittingMode::Type fittingMode;   ///< fitting options
  SamplingMode::Type samplingMode; ///< sampling options
  bool orientationCorrection:1;    ///< if orientation correction is needed

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
   * @param[in] mTrigger The trigger to wake up the main thread.
   */
  ImageLoadThread( EventThreadCallback* mTrigger );

  /**
   * Destructor.
   */
  virtual ~ImageLoadThread();

  /**
   * Add a task in to the loading queue
   *
   * @param[in] task The task added to the queue.
   *
   * @note This class takes ownership of the task object
   */
  void AddTask( LoadingTask* task );

  /**
   * Pop the next task out from the completed queue.
   *
   * @return The next task to be processed.
   */
  LoadingTask* NextCompletedTask();

  /**
   * Remove the loading task from the waiting queue.
   */
  bool CancelTask( uint32_t loadingTaskId );

  /**
   * Remove all the loading tasks in the waiting queue.
   */
  void CancelAll();

private:

  /**
   * Pop the next loading task out from the queue to process.
   *
   * @return The next task to be processed.
   */
  LoadingTask* NextTaskToProcess();

  /**
   * Add a task in to the loading queue
   *
   * @param[in] task The task added to the queue.
   */
  void AddCompletedTask( LoadingTask* task );

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

  Vector< LoadingTask* > mLoadQueue;     ///<The task queue with images for loading.
  Vector< LoadingTask* > mCompleteQueue; ///<The task queue with images loaded.

  ConditionalWait        mConditionalWait;
  Dali::Mutex            mMutex;
  EventThreadCallback*   mTrigger;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_IMAGE_LOAD_THREAD_H__ */
