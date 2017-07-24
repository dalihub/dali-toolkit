#ifndef DALI_TOOLKIT_DEVEL_API_VISUALS_IMAGE_VISUAL_PROPERTIES_DEVEL_H
#define DALI_TOOLKIT_DEVEL_API_VISUALS_IMAGE_VISUAL_PROPERTIES_DEVEL_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>

namespace Dali
{

namespace Toolkit
{

namespace DevelImageVisual
{

namespace Property
{

enum Type
{
  URL                 = Dali::Toolkit::ImageVisual::Property::URL,
  FITTING_MODE        = Dali::Toolkit::ImageVisual::Property::FITTING_MODE,
  SAMPLING_MODE       = Dali::Toolkit::ImageVisual::Property::SAMPLING_MODE,
  DESIRED_WIDTH       = Dali::Toolkit::ImageVisual::Property::DESIRED_WIDTH,
  DESIRED_HEIGHT      = Dali::Toolkit::ImageVisual::Property::DESIRED_HEIGHT,
  SYNCHRONOUS_LOADING = Dali::Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING,
  BORDER_ONLY         = Dali::Toolkit::ImageVisual::Property::BORDER_ONLY,
  PIXEL_AREA          = Dali::Toolkit::ImageVisual::Property::PIXEL_AREA,
  WRAP_MODE_U         = Dali::Toolkit::ImageVisual::Property::WRAP_MODE_U,
  WRAP_MODE_V         = Dali::Toolkit::ImageVisual::Property::WRAP_MODE_V,

  /**
   * @brief The border of the image
   * @details Name "border", type Property::RECTANGLE or Property::VECTOR4
   *          The border of the image in the order: left, right, bottom, top.
   *
   * @note Optional.
   * @note For N-Patch images only.
   */
  BORDER = WRAP_MODE_V + 1,

  /**
   * @brief Whether to use the texture atlas
   * @details Name "atlasing", type Property::BOOLEAN, true to enable texture atlas
   *
   * @note Optional. By default atlasing is off.
   */

  ATLASING = WRAP_MODE_V + 2,

  /**
   * @brief URL of a masking image
   * @details Name "alphaMaskUrl", type Property::STRING, URL of image to apply as
   * a mask after image loading. If set after the main URL has finished loading, this
   * may necessitate a re-load of the main image. The alpha mask image will be scaled
   * on load to match the size of the main image, then applied to the pixel data
   * before uploading to GL.
   * @note Optional.
   */

  ALPHA_MASK_URL = WRAP_MODE_V + 3,

  /**
   * @brief Defines the batch size for pre-loading images in the AnimatedImageVisual
   * @details Name "batchSize", type Property::INTEGER, number of images to pre-load
   * before starting to play. Default value: 1
   */
  BATCH_SIZE = WRAP_MODE_V + 4,

  /**
   * @brief Defines the cache size for loading images in the AnimatedImageVisual
   * @details Name "cacheSize", type Property::INTEGER, number of images to keep
   * cached ahead during playback. Default value: 1
   *
   * @note, cacheSize should be >= batchSize.
   * If it isn't, then the cache will automatically be changed to batchSize.
   * @note, because of the defaults, it is expected that the application developer
   * tune the batch and cache sizes to their particular use case.
   */
  CACHE_SIZE = WRAP_MODE_V + 5,

  /**
   * @brief The number of milliseconds between each frame in the AnimatedImageVisual
   * @details Name "frameDelay", type Property::INTEGER, The number of milliseconds between each frame. Note, this is only used with the URLS property above.
   */
  FRAME_DELAY = WRAP_MODE_V + 6,

  /**
   * @brief The scale factor to apply to the content image before masking
   * @details Name "maskContentScale", type Property::FLOAT, The scale factor
   * to apply to the content before masking. Note, scaled images are cropped to
   * the same size as the alpha mask.
   */
  MASK_CONTENT_SCALE = WRAP_MODE_V + 7,

  /**
   * @brief Whether to crop image to mask or scale mask to fit image
   * @details Name "cropToMask", type Property::BOOLEAN, True if the image should
   * be cropped to match the mask size, or false if the image should remain the same size.
   * Note, if this is false, then the mask is scaled to fit the image before being applied.
   */
  CROP_TO_MASK = WRAP_MODE_V + 8,

};

} //namespace Property

} // namespace DevelImageVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_VISUALS_IMAGE_VISUAL_PROPERTIES_DEVEL_H
