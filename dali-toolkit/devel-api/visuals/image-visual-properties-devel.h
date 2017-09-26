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


/**
 * @brief The policy determining when a image is deleted from the cache in relation to the ImageVisual lifetime.
 * @note If the texture is being shared by another visual it persist if still required.
 */
namespace ReleasePolicy
{

/**
 * @brief The available named elements that define the ReleasePolicy.
 */
enum Type
{
  DETACHED = 0,  ///<  Image deleted from cache when ImageVisual detached from stage.
  DESTROYED,     ///<  Image deleted from cache when ImageVisual destroyed.
  NEVER          ///<  Image is never deleted, will survive the lifetime of the application.
};

} // namespace ReleasePolicy;

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
  BORDER              = Dali::Toolkit::ImageVisual::Property::BORDER,
  ATLASING            = Dali::Toolkit::ImageVisual::Property::ATLASING,
  ALPHA_MASK_URL      = Dali::Toolkit::ImageVisual::Property::ALPHA_MASK_URL,
  BATCH_SIZE          = Dali::Toolkit::ImageVisual::Property::BATCH_SIZE,
  CACHE_SIZE          = Dali::Toolkit::ImageVisual::Property::CACHE_SIZE,
  FRAME_DELAY         = Dali::Toolkit::ImageVisual::Property::FRAME_DELAY,
  MASK_CONTENT_SCALE  = Dali::Toolkit::ImageVisual::Property::MASK_CONTENT_SCALE,
  CROP_TO_MASK        = Dali::Toolkit::ImageVisual::Property::CROP_TO_MASK,

  /**
   * @brief The policy to determine when an image should no longer be cached.
   * @details Name "releasePolicy", Type ReleasePolicy::Type (Property::INTEGER) or Property::STRING
   * @note Default ReleasePolicy::DETACHED
   * @see ReleasePolicy::Type
   */
  RELEASE_POLICY = CROP_TO_MASK + 2,
};

} //namespace Property

} // namespace DevelImageVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_VISUALS_IMAGE_VISUAL_PROPERTIES_DEVEL_H
