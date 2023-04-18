#ifndef DALI_TOOLKIT_IMAGE_H
#define DALI_TOOLKIT_IMAGE_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/adaptor-framework/native-image-source.h>
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/rendering/frame-buffer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali-toolkit/public-api/image-loader/image-url.h>

namespace Dali
{
namespace Toolkit
{
/**
 * API to interface with the toolkit image
 * Allows developers to add FrameBuffer, PixelData and NativeImageSource to toolkit so that visuals can use them to render
 */
namespace Image
{
/**
 * @brief Generate a Url from frame buffer.
 * This Url can be used in visuals to render the frame buffer.
 * @note Any color textures already attached in this freme buffer are not converted to the Url by this method.
 * This method does not check for duplicates, If same frame buffer is entered multiple times, a different URL is returned each time.
 * @param[in] frameBuffer the frame buffer to converted to Url
 * @param[in] pixelFormat the pixel format for this frame buffer
 * @param[in] width the width for this frame buffer
 * @param[in] height the height for this frame buffer
 * @return the ImageUrl representing this frame buffer
 */
DALI_TOOLKIT_API Dali::Toolkit::ImageUrl GenerateUrl(const Dali::FrameBuffer frameBuffer, Pixel::Format pixelFormat, uint32_t width, uint32_t height);

/**
 * @brief Generate a Url from frame buffer.
 * This Url can be used in visuals to render the frame buffer.
 * @note Only an color texture already attached in this frame buffer can be convert to Url by this method.
 * This method does not check for duplicates, If same frame buffer is entered multiple times, a different URL is returned each time.
 * @param[in] frameBuffer the frame buffer to converted to Url
 * @param[in] index the index of the attached color texture.
 * @return the ImageUrl representing this frame buffer
 */
DALI_TOOLKIT_API Dali::Toolkit::ImageUrl GenerateUrl(const Dali::FrameBuffer frameBuffer, uint8_t index);

/**
 * @brief Generate a Url from Pixel data.
 * This Url can be used in visuals to render the pixel data.
 * @note This method does not check for duplicates, If same pixel data is entered multiple times, a different URL is returned each time.
 * @param[in] pixelData the pixel data to converted to Url
 * @param[in] preMultiplied Whether this pixel data preMultiplied or not. Default as false.
 * @return the ImageUrl representing this pixel data
 */
DALI_TOOLKIT_API Dali::Toolkit::ImageUrl GenerateUrl(const Dali::PixelData pixelData, bool preMultiplied = false);

/**
 * @brief Generate a Url from native image source.
 * This Url can be used in visuals to render the native image source.
 * @note This method does not check for duplicates, If same native image source is entered multiple times, a different URL is returned each time.
 * @param[in] nativeImageSource the native image source to converted to Url
 * @param[in] preMultiplied Whether this native image source preMultiplied or not. Default as false.
 * @return the ImageUrl representing this native image source
 */
DALI_TOOLKIT_API Dali::Toolkit::ImageUrl GenerateUrl(const Dali::NativeImageSourcePtr nativeImageSource, bool preMultiplied = false);

/**
 * @brief Generate a Url from encoded image buffer.
 * This Url can be used in visuals to render the image.
 * @note This method does not check for duplicates, If same encoded image buffer is entered multiple times, a different URL is returned each time.
 * @param[in] encodedImageBuffer the encoded image buffer to converted to Url
 * @return the ImageUrl representing this encoded image buffer
 */
DALI_TOOLKIT_API Dali::Toolkit::ImageUrl GenerateUrl(const Dali::EncodedImageBuffer encodedImageBuffer);

} // namespace Image

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_IMAGE_H
