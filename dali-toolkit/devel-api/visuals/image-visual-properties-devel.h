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
};

} //namespace Property

} // namespace DevelImageVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_VISUALS_IMAGE_VISUAL_PROPERTIES_DEVEL_H
