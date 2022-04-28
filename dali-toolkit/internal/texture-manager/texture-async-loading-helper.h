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
#include <dali-toolkit/internal/image-loader/async-image-loader-impl.h>
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
  /**
   * Struct to hold information about a requested Async load.
   * This is used to look up a TextureManager::TextureId from the returned AsyncLoad Id.
   */
  struct AsyncLoadingInfo
  {
    AsyncLoadingInfo(TextureManager::TextureId textureId)
    : textureId(textureId),
      loadId(0)
    {
    }

    TextureManager::TextureId textureId; ///< The external Texture Id assigned to this load
    std::uint32_t             loadId;    ///< The load Id used by the async loader to reference this load
  };

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
   */
  void LoadAnimatedImage(const TextureManager::TextureId& textureId,
                         Dali::AnimatedImageLoading       animatedImageLoading,
                         const std::uint32_t&             frameIndex);

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
   */
  void Load(const TextureManager::TextureId&                textureId,
            const VisualUrl&                                url,
            const Dali::ImageDimensions&                    desiredSize,
            const Dali::FittingMode::Type&                  fittingMode,
            const Dali::SamplingMode::Type&                 samplingMode,
            const bool&                                     orientationCorrection,
            const DevelAsyncImageLoader::PreMultiplyOnLoad& preMultiplyOnLoad);

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

  TextureAsyncLoadingHelper(TextureAsyncLoadingHelper&& rhs);
  TextureAsyncLoadingHelper& operator=(TextureAsyncLoadingHelper&& rhs) = delete;

private:                                                              // Private typedefs:
  typedef std::deque<AsyncLoadingInfo> AsyncLoadingInfoContainerType; ///< The container type used to manage Asynchronous loads in progress

private:
  /**
   * @brief Main constructor that used by all other constructors
   */
  TextureAsyncLoadingHelper(Toolkit::AsyncImageLoader       loader,
                            TextureManager&                 textureManager,
                            AsyncLoadingInfoContainerType&& loadingInfoContainer);

  /**
   * @brief Callback to be called when texture loading is complete, it passes the pixel buffer list on to texture manager.
   * @param[in] id           Loader id
   * @param[in] pixelBuffers Image data
   */
  void AsyncLoadComplete(std::uint32_t id, std::vector<Devel::PixelBuffer>& pixelBuffers);

private: // Member Variables:
  Toolkit::AsyncImageLoader     mLoader;
  TextureManager&               mTextureManager;
  AsyncLoadingInfoContainerType mLoadingInfoContainer;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXTURE_ASYNC_LOADING_HELPER_H
