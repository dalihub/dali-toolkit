#ifndef DALI_TOOLKIT_SYNC_IMAGE_LOADER_H
#define DALI_TOOLKIT_SYNC_IMAGE_LOADER_H

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
#include <string>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/images/pixel-data.h>

namespace Dali
{

namespace Toolkit
{

namespace SyncImageLoader
{

/**
 * @brief The methods in the SyncImageLoader namespace are used to load pixel data from a URL synchronously.
 *
 * Example:
 *
 * @code
 * PixelData pixelData = Toolkit::SyncImageLoader::Load( "image_url.jpg" );
 *
 * // Check the image was loaded without error.
 * if( pixelData )
 * {
 *   // Do work...
 * }
 *
 * @endcode
 */

/**
 * @brief Load an image synchronously.
 * Note: When using this method, the following defaults will be used:
 * fittingMode = FittingMode::DEFAULT
 * samplingMode = SamplingMode::BOX_THEN_LINEAR
 * orientationCorrection = true
 *
 * @SINCE_1_2_14
 * @REMARK_INTERNET
 * @REMARK_STORAGE
 *
 * @param[in] url The URL of the image file to load.
 * @return A PixelData object containing the image, or an invalid object on failure.
 */
DALI_IMPORT_API PixelData Load( const std::string& url );

/**
 * @brief Load an image synchronously by specifying the target dimensions.
 * Note: When using this method, the following defaults will be used:
 * fittingMode = FittingMode::DEFAULT
 * samplingMode = SamplingMode::BOX_THEN_LINEAR
 * orientationCorrection = true
 *
 * @SINCE_1_2_14
 * @REMARK_INTERNET
 * @REMARK_STORAGE
 *
 * @param[in] url The URL of the image file to load.
 * @param[in] dimensions The width and height to fit the loaded image to.
 * @return A PixelData object containing the image, or an invalid object on failure.
 */
DALI_IMPORT_API PixelData Load( const std::string& url, ImageDimensions dimensions );

/**
 * @brief Load an image synchronously by specifying the target dimensions and options.
 * @SINCE_1_2_14
 * @REMARK_INTERNET
 * @REMARK_STORAGE
 *
 * @param[in] url The URL of the image file to load.
 * @param[in] dimensions The width and height to fit the loaded image to.
 * @param[in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter.
 * @param[in] samplingMode The filtering method used when sampling pixels from the input image while fitting it to desired size.
 * @param[in] orientationCorrection Reorient the image to respect any orientation metadata in its header.
 * @return A PixelData object containing the image, or an invalid object on failure.
 */
DALI_IMPORT_API PixelData Load( const std::string& url,
                ImageDimensions dimensions,
                FittingMode::Type fittingMode,
                SamplingMode::Type samplingMode,
                bool orientationCorrection );

} // namespace SyncImageLoader

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_SYNC_IMAGE_LOADER_H
