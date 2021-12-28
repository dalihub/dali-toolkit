#ifndef DALI_TOOLKIT_TEXTURE_ASYNC_LOADING_HELPER_H
#define DALI_TOOLKIT_TEXTURE_ASYNC_LOADING_HELPER_H

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
#include <dali/public-api/signals/connection-tracker.h>
#include <deque>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/image-loader/loading-task.h>
#include <dali-toolkit/internal/texture-manager/texture-manager-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * @brief Helper class to keep the relation between AsyncImageLoader and corresponding LoadingInfo container
 */
class TextureAsyncLoadingHelper : public ConnectionTracker
{
public:
  /**
   * @brief Create an TextureAsyncLoadingHelper.
   * @param[in] textureManager Reference to the texture manager
   */
  TextureAsyncLoadingHelper(TextureManager& textureManager);

  /**
   * @brief Load a new frame of animated image
   * @param[in] textureId             TextureId to reference the texture that will be loaded
   * @param[in] animatedImageLoading  The AnimatedImageLoading to load animated image
   * @param[in] frameIndex            The frame index of a frame to be loaded frame
   * @param[in] desiredSize           The size the image is likely to appear at.
   *                                  This can be set to 0,0 for automatic
   * @param[in] fittingMode           The FittingMode to use
   * @param[in] samplingMode          The SamplingMode to use
   * @param[in] preMultiplyOnLoad     if the image's color should be multiplied by it's alpha. Set to OFF if there is no alpha or if the image need to be applied alpha mask.
   */
  void LoadAnimatedImage(const TextureManager::TextureId&                textureId,
                         Dali::AnimatedImageLoading                      animatedImageLoading,
                         const std::uint32_t&                            frameIndex,
                         const Dali::ImageDimensions&                    desiredSize,
                         const Dali::FittingMode::Type&                  fittingMode,
                         const Dali::SamplingMode::Type&                 samplingMode,
                         const DevelAsyncImageLoader::PreMultiplyOnLoad& preMultiplyOnLoad);

  /**
   * @brief Load a new texture.
   * @param[in] textureId             TextureId to reference the texture that will be loaded
   * @param[in] url                   The URL of the image to load
   * @param[in] desiredSize           The size the image is likely to appear at.
   *                                  This can be set to 0,0 for automatic
   * @param[in] fittingMode           The FittingMode to use
   * @param[in] samplingMode          The SamplingMode to use
   * @param[in] orientationCorrection Whether to use image metadata to rotate or flip the image,
   *                                  e.g., from portrait to landscape
   * @param[in] preMultiplyOnLoad     if the image's color should be multiplied by it's alpha. Set to OFF if there is no alpha or if the image need to be applied alpha mask.
   * @param[in] loadYuvPlanes         True if the image should be loaded as yuv planes
   */
  void Load(const TextureManager::TextureId&                textureId,
            const VisualUrl&                                url,
            const Dali::ImageDimensions&                    desiredSize,
            const Dali::FittingMode::Type&                  fittingMode,
            const Dali::SamplingMode::Type&                 samplingMode,
            const bool&                                     orientationCorrection,
            const DevelAsyncImageLoader::PreMultiplyOnLoad& preMultiplyOnLoad,
            const bool&                                     loadYuvPlanes);

  /**
   * @brief Apply mask
   * @param [in] textureId of the texture
   * @param [in] pixelBuffer of the to be masked image
   * @param [in] maskPixelBuffer of the mask image
   * @param [in] contentScale The factor to scale the content
   * @param [in] cropToMask Whether to crop the content to the mask size
   * @param [in] preMultiplyOnLoad if the image's color should be multiplied by it's alpha. Set to OFF if there is no alpha.
   */
  void ApplyMask(const TextureManager::TextureId&                textureId,
                 Devel::PixelBuffer                              pixelBuffer,
                 Devel::PixelBuffer                              maskPixelBuffer,
                 const float&                                    contentScale,
                 const bool&                                     cropToMask,
                 const DevelAsyncImageLoader::PreMultiplyOnLoad& preMultiplyOnLoad);

public:
  TextureAsyncLoadingHelper(const TextureAsyncLoadingHelper&) = delete;
  TextureAsyncLoadingHelper& operator=(const TextureAsyncLoadingHelper&) = delete;

private:
  /**
   * @brief Callback to be called when texture loading is complete, it passes the pixel buffer list on to texture manager.
   * @param[in] task LoadingTaskPtr
   */
  void AsyncLoadComplete(LoadingTaskPtr task);

private: // Member Variables:
  TextureManager& mTextureManager;
  uint32_t        mLoadTaskId;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXTURE_ASYNC_LOADING_HELPER_H
