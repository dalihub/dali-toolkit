#ifndef DALI_TOOLKIT_DEVEL_API_VISUALS_VISUAL_PROPERTIES_DEVEL_H
#define DALI_TOOLKIT_DEVEL_API_VISUALS_VISUAL_PROPERTIES_DEVEL_H

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
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{

namespace Toolkit
{

namespace DevelVisual
{

/**
 * @brief All the visual types.
 */
enum Type
{
  BORDER    = Dali::Toolkit::Visual::BORDER,
  COLOR     = Dali::Toolkit::Visual::COLOR,
  GRADIENT  = Dali::Toolkit::Visual::GRADIENT,
  IMAGE     = Dali::Toolkit::Visual::IMAGE,
  MESH      = Dali::Toolkit::Visual::MESH,
  PRIMITIVE = Dali::Toolkit::Visual::PRIMITIVE,
  WIREFRAME = Dali::Toolkit::Visual::WIREFRAME,

  TEXT      = WIREFRAME + 1, ///< Renders text.
};

namespace Property
{

enum Type
{
  TYPE   = Dali::Toolkit::Visual::Property::TYPE,
  SHADER = Dali::Toolkit::Visual::Property::SHADER,

  /**
   * @brief The transform used by the visual.
   * @details Name "transform", type Property::Map.
   *
   * @note Optional.
   * @see DevelVisual::Transform::Property
   */
  TRANSFORM = SHADER + 1,

  /**
   * @brief Enables/disables premultiplied alpha.
   * The premultiplied alpha is false by default unless this behaviour is modified
   * by the derived Visual type.
   *
   * @details Name "premultipliedAlpha", type Property::Boolean.
   *
   * @note Optional.
   */
  PREMULTIPLIED_ALPHA = SHADER + 2,

  /**
   * @brief Mix color is a blend color for any visual.
   * @details Name "mixColor", type Property::VECTOR3 or Property::VECTOR4
   *
   * @note Optional
   */
  MIX_COLOR = SHADER + 3,

  /**
   * @brief Opacity is the alpha component of the mixColor, above.
   * @details Name "opacity", type Property::FLOAT
   *
   * @note Optional
   */
  OPACITY = SHADER + 4
};

} // namespace Property

namespace Transform
{

/**
 * @brief Policies used by the transform for the offset or size.
 */
namespace Policy
{

enum Type
{
  RELATIVE = 0,   ///< Relative to the control (percentage [0.0f to 1.0f] of the control).
  ABSOLUTE = 1    ///< Absolute value in world units.
};

} // namespace Policy

namespace Property
{

enum Type
{
  /**
   * @brief Offset of the visual, which can be either relative (percentage [0.0f to 1.0f] of the parent) or absolute (in world units).
   * @details Name "offset", type Property::VECTOR2
   * @see OFFSET_POLICY
   */
  OFFSET,

  /**
   * @brief Size of the visual, which can be either relative (percentage [0.0f to 1.0f] of the parent) or absolute (in world units).
   * @details Name "size", type Property::VECTOR2
   * @see SIZE_POLICY
   */
  SIZE,

  /**
   * @brief The origin of the visual within its control area.
   * @details Name "origin", type Align::Type (Property::INTEGER) or Property::STRING.
   * @see Toolkit::Align
   */
  ORIGIN,

  /**
   * @brief The anchor-point of the visual
   * @details Name "anchorPoint", type Align::Type (Property::INTEGER) or Property::STRING.
   * @see Toolkit::Align
   */
  ANCHOR_POINT,

  /**
   * @brief Whether the x or y OFFSET values are relative (percentage [0.0f to 1.0f] of the control) or absolute (in world units).
   * @details Name "offsetPolicy", type Vector2 or Property::ARRAY of Property::STRING.
   *          If Property::ARRAY then 2 strings expected for the x and y.
   *
   * C++:
   * @code
   * control.SetProperty( ..., // Some visual based property
   *                      Property::Map().Add( ... ) // Properties to set up visual
   *                                     .Add( DevelVisual::Property::TRANSFORM,
   *                                           Property::Array().Add( DevelVisual::Transform::Property::OFFSET_POLICY, Vector2( Policy::ABSOLUTE, Policy::RELATIVE ) ) )
   *                                                            .Add( DevelVisual::Transform::Property::OFFSET, Vector2( 10, 1.0f ) ) );
   * @endcode
   *
   * JSON:
   * @code
   * {
   *   ...
   *   "transition":
   *   {
   *     "offsetPolicy" : [ "ABSOLUTE", "RELATIVE" ],
   *     "offset" : [ 10, 1.0 ]
   *   }
   *   ...
   * }
   *
   * @endcode
   * @see Policy::Type
   * @note By default, both the x and the y offset is RELATIVE.
   */
  OFFSET_POLICY,

  /**
   * @brief Whether the width or height SIZE values are relative (percentage [0.0f to 1.0f] of the control) or absolute (in world units).
   * @details Name "sizePolicy", type Vector2 or Property::ARRAY of Property::STRING.
   *          If Property::ARRAY then 2 strings expected for the width and height.
   *
   * @see Policy::Type
   * @see OFFSET_POLICY for example
   * @note By default, both the width and the height is RELATIVE to the control's size.
   */
  SIZE_POLICY,
};

} // namespace Property

} // namespace Transform

} // namespace DevelVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_VISUALS_VISUAL_PROPERTIES_DEVEL_H
