#ifndef DALI_TOOLKIT_GRADIENT_VISUAL_PROPERTIES_H
#define DALI_TOOLKIT_GRADIENT_VISUAL_PROPERTIES_H

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
 *
 */

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/toolkit-property-index-ranges.h>

namespace Dali
{

namespace Toolkit
{

namespace GradientVisual
{

namespace Property
{

enum
{
  /**
   * @brief The start position of a linear gradient.
   * @details Name "startPosition", type Property::VECTOR2.
   * @SINCE_1_1.45
   * @note Mandatory for Linear.
   */
  START_POSITION = VISUAL_PROPERTY_START_INDEX,

  /**
   * @brief The end position of a linear gradient.
   * @details Name "endPosition", type Property::VECTOR2.
   * @SINCE_1_1.45
   * @note Mandatory for Linear.
   */
  END_POSITION,

  /**
   * @brief The center point of a radial gradient.
   * @details Name "center", type Property::VECTOR2.
   * @SINCE_1_1.45
   * @note Mandatory for Radial.
   */
  CENTER,

  /**
   * @brief The size of the radius of a radial gradient.
   * @details Name "radius", type Property::FLOAT.
   * @SINCE_1_1.45
   * @note Mandatory for Radial.
   */
  RADIUS,

  /**
   * @brief All the stop offsets.
   * @details Name "stopOffset", type Property::ARRAY of Property::FLOAT.
   * @SINCE_1_1.45
   * @note Optional. If not supplied, default is 0.0 and 1.0.
   */
  STOP_OFFSET,

  /**
   * @brief The color at the stop offsets.
   * @details Name "stopColor", type Property::ARRAY of Property::VECTOR4.
   * @SINCE_1_1.45
   * @note Mandatory. At least 2 values required to show a gradient.
   */
  STOP_COLOR,

  /**
   * @brief Defines the coordinate system for certain attributes of the points in a gradient.
   * @details Name "units", type Units::Type (Property::INTEGER) or Property::STRING.
   * @SINCE_1_1.45
   * @note Optional. If not supplied, default is Units::OBJECT_BOUNDING_BOX.
   * @see Units::Type
   */
  UNITS,

  /**
   * @brief Indicates what happens if the gradient starts or ends inside the bounds of the target rectangle.
   * @details Name "spreadMethod", type SpreadMethod::Type (Property::INTEGER) or Property::STRING.
   * @SINCE_1_1.45
   * @note Optional. If not supplied, default is SpreadMethod::PAD.
   * @see SpreadMethod::Type
   */
  SPREAD_METHOD
};

} // namespace Property

namespace Units
{

/**
 * @brief The type of coordinate system for certain attributes of the points in a gradient.
 *
 * This applies to the:
 * - Start (x1, y1) and End (x2 and y2) points of a line if using a linear gradient.
 * - Center point (cx, cy) and radius (r) of a circle if using a radial gradient.
 */
enum Type
{
  OBJECT_BOUNDING_BOX, ///< Uses the normals for the start, end & center points, i.e. top-left is (-0.5, -0.5) and bottom-right is (0.5, 0.5). @SINCE_1_1.45
  USER_SPACE ///< Uses the user coordinates for the start, end & center points, i.e. in a 200 by 200 control, top-left is (0, 0) and bottom-right is (200, 200). @SINCE_1_1.45
};

} // namespace Units

namespace SpreadMethod
{

/**
 * @brief Policies that define what happens if the gradient starts or ends inside the bounds of the target rectangle.
 */
enum Type
{
  PAD, ///< Uses the terminal colors of the gradient to fill the remainder of the quad. @SINCE_1_1.45
  REFLECT, ///< Reflect the gradient pattern start-to-end, end-to-start, start-to-end etc. until the quad is filled. @SINCE_1_1.45
  REPEAT ///< Repeat the gradient pattern start-to-end, start-to-end, start-to-end etc. until the quad is filled. @SINCE_1_1.45
};

} // namespace SpreadMethod

} // namespace GradientVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_GRADIENT_VISUAL_PROPERTIES_H
