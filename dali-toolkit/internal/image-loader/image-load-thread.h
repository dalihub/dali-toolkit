#ifndef DALI_TOOLKIT_IMAGE_LOAD_THREAD_H
#define DALI_TOOLKIT_IMAGE_LOAD_THREAD_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/image-loader/async-image-loader-devel.h>
#include <dali-toolkit/internal/visuals/visual-url.h>
#include <dali/devel-api/adaptor-framework/event-thread-callback.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/integration-api/adaptor-framework/log-factory-interface.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/object/ref-object.h>

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
   * @param [in] animatedImageLoading The AnimatedImageLoading to load animated image
   * @param [in] frameIndex The frame index of a frame to be loaded frame
   */
  LoadingTask(uint32_t                   id,
              Dali::AnimatedImageLoading animatedImageLoading,
              uint32_t                   frameIndex);

  /**
   * Constructor.
   * @param [in] id of the task
   * @param [in] url The URL of the image file to load.
   * @param [in] size The width and height to fit the loaded image to, 0.0 means whole image
   * @param [in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter.
   * @param [in] samplingMode The filtering method used when sampling pixels from the input image while fitting it to desired size.
   * @param [in] orientationCorrection Reorient the image to respect any orientation metadata in its header.
   * @param [in] preMultiplyOnLoad ON if the image's color should be multiplied by it's alpha. Set to OFF if there is no alpha or if the image need to be applied alpha mask.
   */
  LoadingTask(uint32_t                                 id,
              const VisualUrl&                         url,
              ImageDimensions                          dimensions,
              FittingMode::Type                        fittingMode,
              SamplingMode::Type                       samplingMode,
              bool                                     orientationCorrection,
              DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad);

  /**
   * Constructor.
   * @param [in] id of the task
   * @param [in] pixelBuffer of the to be masked image
   * @param [in] maskPixelBuffer of the mask image
   * @param [in] contentScale The factor to scale the content
   * @param [in] cropToMask Whether to crop the content to the mask size
   * @param [in] preMultiplyOnLoad ON if the image's color should be multiplied by it's alpha. Set to OFF if there is no alpha.
   */
  LoadingTask(uint32_t                                 id,
              Devel::PixelBuffer                       pixelBuffer,
              Devel::PixelBuffer                       maskPixelBuffer,
              float                                    contentScale,
              bool                                     cropToMask,
              DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad);

  /**
   * Load the image
   */
  void Load();

  /**
   * Apply mask
   */
  void ApplyMask();

  /**
   * Multiply alpha
   */
  void MultiplyAlpha();

private:
  // Undefined
  LoadingTask(const LoadingTask& queue);

  // Undefined
  LoadingTask& operator=(const LoadingTask& queue);

public:
  Devel::PixelBuffer pixelBuffer;                                     ///< pixelBuffer handle after successful load
                                                                      ///< or pixelBuffer to be masked image in the mask task
  VisualUrl                                url;                       ///< url of the image to load
  uint32_t                                 id;                        ///< The unique id associated with this task.
  ImageDimensions                          dimensions;                ///< dimensions to load
  FittingMode::Type                        fittingMode;               ///< fitting options
  SamplingMode::Type                       samplingMode;              ///< sampling options
  bool                                     orientationCorrection : 1; ///< if orientation correction is needed
  DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad;         //< if the image's color should be multiplied by it's alpha

  bool                       isMaskTask;      ///< whether this task is for mask or not
  Devel::PixelBuffer         maskPixelBuffer; ///< pixelBuffer of mask image
  float                      contentScale;    ///< The factor to scale the content
  bool                       cropToMask;      ///< Whether to crop the content to the mask size
  Dali::AnimatedImageLoading animatedImageLoading;
  uint32_t                   frameIndex;
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
  ImageLoadThread(EventThreadCallback* mTrigger);

  /**
   * Destructor.
   */
  ~ImageLoadThread() override;

  /**
   * Add a task in to the loading queue
   *
   * @param[in] task The task added to the queue.
   *
   * @note This class takes ownership of the task object
   */
  void AddTask(LoadingTask* task);

  /**
   * Pop the next task out from the completed queue.
   *
   * @return The next task to be processed.
   */
  LoadingTask* NextCompletedTask();

  /**
   * Remove the loading task from the waiting queue.
   */
  bool CancelTask(uint32_t loadingTaskId);

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
  void AddCompletedTask(LoadingTask* task);

protected:
  /**
   * The entry function of the worker thread.
   * It fetches loading task from the loadQueue, loads the image and adds to the completeQueue.
   */
  void Run() override;

private:
  // Undefined
  ImageLoadThread(const ImageLoadThread& thread);

  // Undefined
  ImageLoadThread& operator=(const ImageLoadThread& thread);

private:
  Vector<LoadingTask*>             mLoadQueue;     ///<The task queue with images for loading.
  Vector<LoadingTask*>             mCompleteQueue; ///<The task queue with images loaded.
  EventThreadCallback*             mTrigger;
  const Dali::LogFactoryInterface& mLogFactory; ///< The log factory

  ConditionalWait mConditionalWait;
  Dali::Mutex     mMutex;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_IMAGE_LOAD_THREAD_H
