#ifndef DALI_TOOLKIT_IMAGE_VISUAL_PROPERTIES_H
#define DALI_TOOLKIT_IMAGE_VISUAL_PROPERTIES_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/toolkit-property-index-ranges.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @addtogroup dali_toolkit_visuals
 * @{
 */

/**
 * @brief ImageVisual is to render an image into the control's quad.
 * @SINCE_1_1.45
 */
namespace ImageVisual
{

/**
 * @brief ImageVisual Property.
 * @SINCE_1_1.45
 */
namespace Property
{

/**
 * @brief Enumeration for the instance of properties belonging to the ImageVisual.
 * @SINCE_1_1.45
 */
enum
{
  /**
   * @brief The URL of the image.
   * @details Name "url", type Property::STRING or Property::ARRAY of Property::STRING.
   * @note The array form is used for generating animated image visuals.
   * @note The number of threads used for local and remote image loading can be controlled by the
   *       environment variables DALI_TEXTURE_LOCAL_THREADS and DALI_TEXTURE_REMOTE_THREADS respectively.
   *       The default values are 4 threads for local image loading and 8 threads for remote image loading.
   * @SINCE_1_1.45
   * @note Mandatory.
   */
  URL = VISUAL_PROPERTY_START_INDEX,

  /**
   * @brief Fitting options, used when resizing images to fit desired dimensions.
   * @details Name "fittingMode", type Dali::FittingMode (Property::INTEGER) or Property::STRING.
   * @SINCE_1_1.45
   * @note Optional. If not supplied, default is FittingMode::SHRINK_TO_FIT.
   * @note For Normal Quad images only.
   * @see Dali::FittingMode
   */
  FITTING_MODE,

  /**
   * @brief Filtering options, used when resizing images to sample original pixels.
   * @details Name "samplingMode", type Dali::SamplingMode (Property::INTEGER) or Property::STRING.
   * @SINCE_1_1.45
   * @note Optional. If not supplied, default is SamplingMode::BOX.
   * @note For Normal Quad images only.
   * @see Dali::SamplingMode
   */
  SAMPLING_MODE,

  /**
   * @brief The desired image width.
   * @details Name "desiredWidth", type Property::INTEGER.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the actual image width is used.
   * @note For Normal Quad images only.
   */
  DESIRED_WIDTH,

  /**
   * @brief The desired image height.
   * @details Name "desiredHeight", type Property::INTEGER.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the actual image height is used.
   * @note For Normal Quad images only.
   */
  DESIRED_HEIGHT,

  /**
   * @brief Whether to load the image synchronously.
   * @details Name "synchronousLoading", type Property::BOOLEAN.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the default is false, i.e. the image is loaded asynchronously.
   * @note For Normal Quad images only.
   */
  SYNCHRONOUS_LOADING,

  /**
   * @brief If true, only draws the borders.
   * @details Name "borderOnly", type Property::BOOLEAN.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the default is false.
   * @note For N-Patch images only.
   */
  BORDER_ONLY,

  /**
   * @brief The image area to be displayed.
   * @details Name "pixelArea", type Property::VECTOR4.
   *          It is a rectangular area.
   *          The first two elements indicate the top-left position of the area, and the last two elements are the area width and height respectively.
   * @SINCE_1_2.1
   * @note Optional. If not specified, the default value is [0.0, 0.0, 1.0, 1.0], i.e. the entire area of the image.
   * @note For Normal Quad images only.
   */
  PIXEL_AREA,

  /**
   * @brief The wrap mode for u coordinate.
   * @details Name "wrapModeU", type Dali::WrapMode::Type (Property::INTEGER) or Property::STRING.
   *          It decides how the texture should be sampled when the u coordinate exceeds the range of 0.0 to 1.0.
   * @SINCE_1_2.1
   * @note Optional. If not specified, the default is CLAMP.
   * @note For Normal QUAD image only.
   */
  WRAP_MODE_U,

  /**
   * @brief The wrap mode for v coordinate.
   * @details Name "wrapModeV", type Dali::WrapMode::Type (Property::INTEGER) or Property::STRING.
   *          it decides how the texture should be sampled when the v coordinate exceeds the range of 0.0 to 1.0.
   * @SINCE_1_2.1
   * @note Optional. If not specified, the default is CLAMP.
   * @note For Normal QUAD image only.
   */
  WRAP_MODE_V,

  /**
   * @brief The border of the image.
   * @details Name "border", type Property::RECTANGLE or Property::VECTOR4.
   *          The border of the image in the order: left, right, bottom, top.
   * @SINCE_1_2.60
   * @note Optional.
   * @note For N-Patch images only.
   */
  BORDER,

  /**
   * @brief Whether to use the texture atlas
   * @details Name "atlasing", type Property::BOOLEAN.
   * @SINCE_1_2.60
   * @note Optional. By default atlasing is off.
   */
  ATLASING,

  /**
   * @brief URL of a masking image
   * @details Name "alphaMaskUrl", type Property::STRING, URL of image to apply as
   * a mask after image loading. If set after the main URL has finished loading, this
   * may necessitate a re-load of the main image. The alpha mask image will be scaled
   * on load to match the size of the main image, then applied to the pixel data
   * before uploading to GL.
   * @SINCE_1_2.60
   * @note Optional.
   */
  ALPHA_MASK_URL,

  /**
   * @brief Defines the batch size for pre-loading images in the AnimatedImageVisual
   * @details Name "batchSize", type Property::INTEGER, number of images to pre-load
   * before starting to play. Default value: 2
   * @SINCE_1_2.60
   * @note Optional.
   * @note Minimum supported value is 2.
   */
  BATCH_SIZE,

  /**
   * @brief Defines the cache size for loading images in the AnimatedImageVisual
   * @details Name "cacheSize", type Property::INTEGER, number of images to keep
   * cached ahead during playback. Default value: 2
   *
   * @SINCE_1_2.60
   * @note Optional.
   * @note Minimum supported value is 2.
   * @note, cacheSize should be >= batchSize.
   * If it isn't, then the cache will automatically be changed to batchSize.
   * @note, because of the defaults, it is expected that the application developer
   * tune the batch and cache sizes to their particular use case.
   */
  CACHE_SIZE,

  /**
   * @brief The number of milliseconds between each frame in the AnimatedImageVisual
   * @details Name "frameDelay", type Property::INTEGER, The number of milliseconds between each frame.
   * @SINCE_1_2.60
   * @note Optional.
   * @note This is only used when multiple URLs are provided.
   */
  FRAME_DELAY,

  /**
   * @brief The scale factor to apply to the content image before masking
   * @details Name "maskContentScale", type Property::FLOAT, The scale factor
   * to apply to the content before masking. Note, scaled images are cropped to
   * the same size as the alpha mask.
   * @SINCE_1_2.60
   * @note Optional.
   */
  MASK_CONTENT_SCALE,

  /**
   * @brief Whether to crop image to mask or scale mask to fit image
   * @details Name "cropToMask", type Property::BOOLEAN, True if the image should
   * be cropped to match the mask size, or false if the image should remain the same size.
   * @SINCE_1_2.60
   * @note Optional.
   * @note If this is false, then the mask is scaled to fit the image before being applied.
   */
  CROP_TO_MASK,

  /**
   * @brief The policy to determine when an image should be loaded.
   * @details Name "loadPolicy",  Type LoadPolicy::Type (Property::INTEGER)or Property::STRING.
   * @SINCE_1_3_5
   * @note Default LoadPolicy::ATTACHED
   * @see LoadPolicy::Type
   */

  LOAD_POLICY,

  /**
   * @brief The policy to determine when an image should no longer be cached.
   * @details Name "releasePolicy", Type ReleasePolicy::Type (Property::INTEGER) or Property::STRING
   * @SINCE_1_3_5
   * @note Default ReleasePolicy::DESTROYED
   * @see ReleasePolicy::Type
   */
  RELEASE_POLICY,

  /**
   * @brief Determines if image orientation should be corrected so the image displays as it was intended.
   * @details Name "orientationCorrection", Type Property::BOOLEAN, if true the image's orientation will be corrected.
   * @SINCE_1_3_5
   * @note Default true
   */
  ORIENTATION_CORRECTION,

};

} // namespace Property

/**
 * @brief The policy determining if the image is loaded when the visual is staged or created.
 * @SINCE_1_3_5
 */
namespace LoadPolicy
{

/**
 * @brief The available named elements that define the LoadPolicy.
 * @SINCE_1_3_5
 */
enum Type
{
  IMMEDIATE = 0,  ///< The image is loaded when the ImageVisual is created.
  ATTACHED        ///< The image is loaded when the ImageVisual is attached to the stage.
};

} // namespace LoadPolicy

/**
 * @brief The policy determining when a image is deleted from the cache in relation to the ImageVisual lifetime.
 * @SINCE_1_3_5
 * @note If the texture is being shared by another visual it persist if still required.
 */
namespace ReleasePolicy
{

/**
 * @brief The available named elements that define the ReleasePolicy.
 * @SINCE_1_3_5
 */
enum Type
{
  DETACHED = 0,  ///<  Image deleted from cache when ImageVisual detached from stage.
  DESTROYED,     ///<  Image deleted from cache when ImageVisual destroyed.
  NEVER          ///<  Image is never deleted, will survive the lifetime of the application.
};

} // namespace ReleasePolicy;

} // namespace ImageVisual

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_IMAGE_VISUAL_PROPERTIES_H
