#ifndef DALI_TOOLKIT_DEVEL_API_IMAGE_LOADER_ASYNC_IMAGE_LOADER_DEVEL_H
#define DALI_TOOLKIT_DEVEL_API_IMAGE_LOADER_ASYNC_IMAGE_LOADER_DEVEL_H

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

// EXTERNAL HEADER
#include <dali/devel-api/adaptor-framework/animated-image-loading.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL HEADER
#include <dali-toolkit/public-api/image-loader/async-image-loader.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelAsyncImageLoader
{
typedef Signal<void(uint32_t, std::vector<Devel::PixelBuffer>&)> PixelBufferLoadedSignalType;

/**
 * @brief Whether to multiply alpha into color channels on load
 */
enum class PreMultiplyOnLoad
{
  OFF = 0, ///< Don't modify the image
  ON       ///< Multiply alpha into color channels on load
};

/**
 * @brief Starts an animated image loading task.
 * @REMARK_INTERNET
 * @REMARK_STORAGE
 * @param[in] asyncImageLoader The ayncImageLoader
 * @param[in] animatedImageLoading The AnimatedImageLoading to load animated image
 * @param[in] frameIndex The frame index of a frame to be loaded frame
 * @return The loading task id
 */
DALI_TOOLKIT_API uint32_t LoadAnimatedImage(AsyncImageLoader           asyncImageLoader,
                                            Dali::AnimatedImageLoading animatedImageLoading,
                                            uint32_t                   frameIndex);

/**
 * @brief Starts an image loading task.
 * @REMARK_INTERNET
 * @REMARK_STORAGE
 * @param[in] asyncImageLoader The ayncImageLoader
 * @param[in] url The URL of the image file to load
 * @param[in] dimensions The width and height to fit the loaded image to
 * @param[in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter
 * @param[in] samplingMode The filtering method used when sampling pixels from the input image while fitting it to desired size
 * @param[in] orientationCorrection Reorient the image to respect any orientation metadata in its header
 * @param[in] preMultiplyOnLoad ON if the image color should be multiplied by it's alpha. Set to OFF if there is no alpha or if the image need to be applied alpha mask.
 * @return The loading task id
 */
DALI_TOOLKIT_API uint32_t Load(AsyncImageLoader                         asyncImageLoader,
                               const std::string&                       url,
                               ImageDimensions                          dimensions,
                               FittingMode::Type                        fittingMode,
                               SamplingMode::Type                       samplingMode,
                               bool                                     orientationCorrection,
                               DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad);

/**
 * @brief Starts an mask applying task.
 * @REMARK_INTERNET
 * @REMARK_STORAGE
 * @param[in] asyncImageLoader The ayncImageLoader
 * @param[in] pixelBuffer Pointer to raw pixel data to be masked
 * @param[in] maskPixelBuffer Pointer to raw masking data
 * @param[in] contentScale The factor to scale the content
 * @param[in] cropToMask Whether to crop the content to the mask size
 * @param[in] preMultiplyOnLoad ON if the image color should be multiplied by it's alpha. Set to OFF if there is no alpha.
 * @return The masking task id
 */
DALI_TOOLKIT_API uint32_t ApplyMask(AsyncImageLoader                         asyncImageLoader,
                                    Devel::PixelBuffer                       pixelBuffer,
                                    Devel::PixelBuffer                       maskPixelBuffer,
                                    float                                    contentScale,
                                    bool                                     cropToMask,
                                    DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad);

/**
 * Connect to this signal if you want to load a PixelBuffer instead of a PixelData.
 * @note Connecting to this signal prevents the emission of the ImageLoadedSignal.
 */
DALI_TOOLKIT_API PixelBufferLoadedSignalType& PixelBufferLoadedSignal(AsyncImageLoader asyncImageLoader);

} // namespace DevelAsyncImageLoader
} // namespace Toolkit
} // namespace Dali

#endif
