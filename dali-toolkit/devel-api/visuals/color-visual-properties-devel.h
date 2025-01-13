#ifndef DALI_TOOLKIT_COLOR_VISUAL_PROPERTIES_DEVEL_H
#define DALI_TOOLKIT_COLOR_VISUAL_PROPERTIES_DEVEL_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelColorVisual
{
/**
 * @brief Additional ColorVisual Properties.
 */
namespace Property
{
/**
 * @brief Enumeration for the instance of additional properties belonging to the ColorVisual.
 */
enum
{
  MIX_COLOR = Toolkit::ColorVisual::Property::MIX_COLOR,

  /**
   * @brief The blur radius of the visual.
   * @details Name "blurRadius", type Property::FLOAT, animatable.
   *          If the value is 0, the edge is sharp. Otherwise, the larger the value, the more the edge is blurred.
   * @note Optional.
   * @note The default is 0.
   * @note The visual size increases by the blur radius.
   * @note If squareness is not zero, the width of blur radius might not equal with it's real value.
   */
  BLUR_RADIUS = MIX_COLOR + 2,

  /**
   * @brief Policy of cutout the color render result.
   * @details Name "cutoutPolicy", type Property::INTEGER.
   * @note Optional.
   * @note The default is CutoutPolicy::NONE.
   */
  CUTOUT_POLICY = MIX_COLOR + 3,
};

} // namespace Property

/**
 * @brief Enumeration for cutout policy.
 */
namespace CutoutPolicy
{
enum Type
{
  NONE,                           ///< Fully render the visual area (Default)
  CUTOUT_VIEW,                    ///< Cutout the area of the view. It will use size of view.
  CUTOUT_VIEW_WITH_CORNER_RADIUS, ///< Cutout the area of the view include visual's corner radius. It will use size of view.
};
} // namespace CutoutPolicy

} // namespace DevelColorVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_COLOR_VISUAL_PROPERTIES_DEVEL_H
