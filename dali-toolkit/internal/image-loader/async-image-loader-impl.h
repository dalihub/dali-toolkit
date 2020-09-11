#ifndef DALI_TOOLKIT_ASYNC_IMAGE_LOADER_IMPL_H
#define DALI_TOOLKIT_ASYNC_IMAGE_LOADER_IMPL_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/image-loader/async-image-loader.h>
#include <dali-toolkit/devel-api/image-loader/async-image-loader-devel.h>
#include <dali-toolkit/internal/image-loader/image-load-thread.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class AsyncImageLoader : public BaseObject
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
   * @copydoc Toolkit::AsyncImageLoader::LoadAnimatedImage( Dali::AnimatedImageLoading animatedImageLoading, uint32_t frameIndex )
   */
  uint32_t LoadAnimatedImage( Dali::AnimatedImageLoading animatedImageLoading,
                              uint32_t frameIndex );

  /**
   * @copydoc Toolkit::AsyncImageLoader::Load( const std::string&, ImageDimensions, FittingMode::Type, SamplingMode::Type, bool , DevelAsyncImageLoader::PreMultiplyOnLoad )
   */
  uint32_t Load( const VisualUrl& url,
                 ImageDimensions dimensions,
                 FittingMode::Type fittingMode,
                 SamplingMode::Type samplingMode,
                 bool orientationCorrection,
                 DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad );

  /**
   * @brief Starts an mask applying task.
   * @param[in] pixelBuffer of the to be masked image
   * @param[in] maskPixelBuffer of the mask image
   * @param[in] contentScale The factor to scale the content
   * @param[in] cropToMask Whether to crop the content to the mask size
   * @param[in] preMultiplyOnLoad ON if the image color should be multiplied by it's alpha. Set to OFF if there is no alpha.
   * @return The loading task id
   */
  uint32_t ApplyMask( Devel::PixelBuffer pixelBuffer,
                      Devel::PixelBuffer maskPixelBuffer,
                      float contentScale,
                      bool cropToMask,
                      DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad );

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
  bool Cancel( uint32_t loadingTaskId );

  /**
   * @copydoc Toolkit::AsyncImageLoader::CancelAll
   */
  void CancelAll();

  /**
   * Process the completed loading task from the worker thread.
   */
  void ProcessLoadedImage();

protected:

  /**
   * Destructor
   */
  ~AsyncImageLoader() override;

private:
  Toolkit::AsyncImageLoader::ImageLoadedSignalType mLoadedSignal;
  Toolkit::DevelAsyncImageLoader::PixelBufferLoadedSignalType mPixelBufferLoadedSignal;

  ImageLoadThread mLoadThread;
  uint32_t        mLoadTaskId;
  bool            mIsLoadThreadStarted;
};

} // namespace Internal

inline const Internal::AsyncImageLoader& GetImplementation( const Toolkit::AsyncImageLoader& handle )
{
  DALI_ASSERT_ALWAYS( handle && "AsyncImageLoader handle is empty" );

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::AsyncImageLoader&>( object );
}

inline Internal::AsyncImageLoader& GetImplementation( Toolkit::AsyncImageLoader& handle )
{
  DALI_ASSERT_ALWAYS( handle && "AsyncImageLoader handle is empty" );

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::AsyncImageLoader&>( object );
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_ASYNC_IMAGE_LOADER_IMPL_H
