#ifndef DALI_TOOLKIT_ASYNC_IMAGE_LOADER_IMPL_H
#define DALI_TOOLKIT_ASYNC_IMAGE_LOADER_IMPL_H

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
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/image-loader/async-image-loader-devel.h>
#include <dali-toolkit/internal/image-loader/loading-task.h>
#include <dali-toolkit/public-api/image-loader/async-image-loader.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
using LoadingTaskPtr = IntrusivePtr<LoadingTask>;

struct AsyncImageLoadingInfo
{
  AsyncImageLoadingInfo(LoadingTaskPtr loadingTask,std::uint32_t loadId)
  : loadingTask(loadingTask),
    loadId(loadId)
  {
  }

  LoadingTaskPtr loadingTask;
  std::uint32_t  loadId;
};

class AsyncImageLoader : public BaseObject, public ConnectionTracker
{
public:
  /**
   * Constructor
   */
  AsyncImageLoader();

  /**
   * @copydoc Toolkit::AsyncImageLoader::New()
   */
  static IntrusivePtr<AsyncImageLoader> New();

  /**
   * @copydoc Toolkit::AsyncImageLoader::LoadAnimatedImage( Dali::AnimatedImageLoading animatedImageLoading, uint32_t frameIndex, DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
   */
  uint32_t LoadAnimatedImage(Dali::AnimatedImageLoading               animatedImageLoading,
                             uint32_t                                 frameIndex,
                             DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad);

  /**
   * @brief Starts an image loading task.
   * @param[in] url The URL of the image file to load
   * @param[in] dimensions The width and height to fit the loaded image to
   * @param[in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter
   * @param[in] samplingMode The filtering method used when sampling pixels from the input image while fitting it to desired size
   * @param[in] orientationCorrection Reorient the image to respect any orientation metadata in its header
   * @param[in] preMultiplyOnLoad ON if the image color should be multiplied by it's alpha. Set to OFF if there is no alpha or if the image need to be applied alpha mask.
   * @param[in] loadPlanes true to load image planes or false to load bitmap image.
   * @return The loading task id
   */
  uint32_t Load(const VisualUrl&                         url,
                ImageDimensions                          dimensions,
                FittingMode::Type                        fittingMode,
                SamplingMode::Type                       samplingMode,
                bool                                     orientationCorrection,
                DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad,
                bool                                     loadPlanes);

  /**
   * @brief Starts an image loading task by encoded image buffer.
   * @param[in] encodedImageBuffer The encoded buffer of the image to load
   * @param[in] dimensions The width and height to fit the loaded image to
   * @param[in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter
   * @param[in] samplingMode The filtering method used when sampling pixels from the input image while fitting it to desired size
   * @param[in] orientationCorrection Reorient the image to respect any orientation metadata in its header
   * @param[in] preMultiplyOnLoad ON if the image color should be multiplied by it's alpha. Set to OFF if there is no alpha.
   * @return The loading task id
   */
  uint32_t LoadEncodedImageBuffer(const EncodedImageBuffer&                encodedImageBuffer,
                                  ImageDimensions                          dimensions,
                                  FittingMode::Type                        fittingMode,
                                  SamplingMode::Type                       samplingMode,
                                  bool                                     orientationCorrection,
                                  DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad);

  /**
   * @brief Starts an mask applying task.
   * @param[in] pixelBuffer of the to be masked image
   * @param[in] maskPixelBuffer of the mask image
   * @param[in] contentScale The factor to scale the content
   * @param[in] cropToMask Whether to crop the content to the mask size
   * @param[in] preMultiplyOnLoad ON if the image color should be multiplied by it's alpha. Set to OFF if there is no alpha.
   * @return The loading task id
   */
  uint32_t ApplyMask(Devel::PixelBuffer                       pixelBuffer,
                     Devel::PixelBuffer                       maskPixelBuffer,
                     float                                    contentScale,
                     bool                                     cropToMask,
                     DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad);

  /**
   * @copydoc Toolkit::AsyncImageLoader::ImageLoadedSignal
   */
  Toolkit::AsyncImageLoader::ImageLoadedSignalType& ImageLoadedSignal();

  /**
   * @copydoc Toolkit::AsyncImageLoader::PixelBufferLoadedSignal
   */
  Toolkit::DevelAsyncImageLoader::PixelBufferLoadedSignalType& PixelBufferLoadedSignal();

  /**
   * @copydoc Toolkit::AsyncImageLoader::Cancel
   */
  bool Cancel(uint32_t loadingTaskId);

  /**
   * @copydoc Toolkit::AsyncImageLoader::CancelAll
   */
  void CancelAll();

  /**
   * Process the completed loading task from the worker thread.
   */
  void ProcessLoadedImage(LoadingTaskPtr task);

protected:
  /**
   * Destructor
   */
  ~AsyncImageLoader() override;

private:
  /**
   * Remove already completed tasks
   */
  void RemoveCompletedTask();

private:
  Toolkit::AsyncImageLoader::ImageLoadedSignalType            mLoadedSignal;
  Toolkit::DevelAsyncImageLoader::PixelBufferLoadedSignalType mPixelBufferLoadedSignal;
  std::vector<AsyncImageLoadingInfo>                          mLoadingTasks;
  std::vector<uint32_t>                                       mCompletedTaskIds;
  uint32_t                                                    mLoadTaskId;
};

} // namespace Internal

inline const Internal::AsyncImageLoader& GetImplementation(const Toolkit::AsyncImageLoader& handle)
{
  DALI_ASSERT_ALWAYS(handle && "AsyncImageLoader handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::AsyncImageLoader&>(object);
}

inline Internal::AsyncImageLoader& GetImplementation(Toolkit::AsyncImageLoader& handle)
{
  DALI_ASSERT_ALWAYS(handle && "AsyncImageLoader handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::AsyncImageLoader&>(object);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_ASYNC_IMAGE_LOADER_IMPL_H
