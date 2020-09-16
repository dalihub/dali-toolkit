#ifndef DALI_TOOLKIT_NPATCH_UTILITIES_H
#define DALI_TOOLKIT_NPATCH_UTILITIES_H

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
 *
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/uint-16-pair.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
namespace Toolkit
{
namespace NPatchUtility
{
/**
 * The list that includes stretch pixel ranges
 */
using StretchRanges = Dali::Vector<Uint16Pair>;

/**
 * @brief Get the offset of the red channel for the format.
 *
 * @param[in] pixelFormat The pixel format
 * @param[out] byteOffset The byte offset of the red channel.
 * @param[out] bitMask The bit mask of the red channel.
 */
DALI_TOOLKIT_API void GetRedOffsetAndMask(Dali::Pixel::Format pixelFormat, int& byteOffset, int& bitMask);

/**
 * @brief Read the borders of the buffer and determine the child area and stretch borders.
 *
 * @param[in] pixelBuffer The npatch image buffer.
 * @param[out] stretchPixelsX The horizontal stretchable pixels in the cropped image space.
 * @param[out] stretchPixelsY The vertical stretchable pixels in the cropped image space.
 */
DALI_TOOLKIT_API void ParseBorders(Devel::PixelBuffer& pixelBuffer, StretchRanges& stretchPixelsX, StretchRanges& stretchPixelsY);

/**
 * @brief Helper method to determine if the filename indicates that the image has a 9 patch or n patch border.
 *
 * @param [in] url The URL of the image file.
 * @return true if it is a 9 patch or n patch image
 */
DALI_TOOLKIT_API bool IsNinePatchUrl(const std::string& url);

} // namespace NPatchUtility

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_NPATCH_UTILITIES_H
