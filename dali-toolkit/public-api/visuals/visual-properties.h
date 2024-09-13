#ifndef DALI_TOOLKIT_VISUAL_PROPERTIES_H
#define DALI_TOOLKIT_VISUAL_PROPERTIES_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
 * @brief All the visual types.
 * @SINCE_1_1.45
 */
namespace Visual
{
/**
 * @brief All the visual types.
 * @SINCE_1_1.45
 */
enum Type
{
  BORDER,         ///< Renders a solid color as an internal border to the control's quad. @SINCE_1_1.45
  COLOR,          ///< Renders a solid color to the control's quad. @SINCE_1_1.45
  GRADIENT,       ///< Renders a smooth transition of colors to the control's quad. @SINCE_1_1.45
  IMAGE,          ///< Renders an image into the control's quad. @SINCE_1_1.45
  MESH,           ///< Renders a mesh using an "obj" file, optionally with textures provided by an "mtl" file. @SINCE_1_1.45
  PRIMITIVE,      ///< Renders a simple 3D shape, such as a cube or sphere. @SINCE_1_1.45
  WIREFRAME,      ///< Renders a simple wire-frame outlining a quad. @SINCE_1_2_2
  TEXT,           ///< Renders text @SINCE_1_2.60
  N_PATCH,        ///< Renders an n-patch image. @SINCE_1_2.60
  SVG,            ///< Renders an SVG image. @SINCE_1_2.60
  ANIMATED_IMAGE, ///< Renders a animated image. @SINCE_1_2.60
};

/**
 * @brief Visual Property.
 * @SINCE_1_1.45
 */
namespace Property
{
/**
 * @brief Enumeration for the instance of properties belonging to the Visual Property.
 * @SINCE_1_1.45
 */
enum
{
  /**
   * @brief The index for the visual type.
   * @details Name "visualType", type [Type](Dali::Toolkit::Visual::Type) (Property::INTEGER) or Property::STRING.
   * @SINCE_1_1.45
   * @note Mandatory.
   * @see Type
   */
  TYPE = VISUAL_PROPERTY_BASE_START_INDEX,

  /**
   * @brief The shader to use in the visual.
   * @details Name "shader", type Property::MAP.
   * @SINCE_1_1.45
   * @note Optional.
   * @note Will override the existing shaders.
   * @see Shader::Property
   */
  SHADER,

  /**
   * @brief The transform used by the visual.
   * @details Name "transform", type Property::MAP.
   * @SINCE_1_2.60
   * @note Optional.
   * @see Toolkit::Visual::Transform::Property
   */
  TRANSFORM,

  /**
   * @brief Enables/disables premultiplied alpha.
   * @details Name "premultipliedAlpha", type Property::BOOLEAN.
   * @SINCE_1_2.60
   * @note Optional.
   * @note The premultiplied alpha is false by default unless this behaviour is modified
   * by the derived Visual type.
   */
  PREMULTIPLIED_ALPHA,

  /**
   * @brief Mix color is a blend color for any visual.
   * @details Name "mixColor", type Property::VECTOR3 or Property::VECTOR4, animatable
   * @SINCE_1_2.60
   * @note Optional
   * @note Animate support for Property::VECTOR4, which OPACITY property included.
   */
  MIX_COLOR,

  /**
   * @brief Opacity is the alpha component of the mixColor, above.
   * @details Name "opacity", type Property::FLOAT, animatable
   * @SINCE_1_2.60
   * @note Optional
   */
  OPACITY,
};

} // namespace Property

/**
 * @brief Visual Transform for the offset or size.
 * @SINCE_1_2.60
 */
namespace Transform
{
/**
 * @brief Policies used by the transform for the offset or size.
 * @SINCE_1_2.60
 */
namespace Policy
{
/**
 * @brief Enumeration for the type of Transform Policy.
 * @SINCE_1_2.60
 */
enum Type
{
  RELATIVE = 0, ///< Relative to the control (percentage [0.0f to 1.0f] of the control). @SINCE_1_2.60
  ABSOLUTE = 1  ///< Absolute value in world units. @SINCE_1_2.60
};

} // namespace Policy

/**
 * @brief Visual Transform Property.
 * @SINCE_1_2.60
 */
namespace Property
{
/**
 * @brief Enumeration for the type of Transform Property.
 * @SINCE_1_2.60
 */
enum Type
{
  /**
   * @brief Offset of the visual, which can be either relative (percentage [0.0f to 1.0f] of the parent) or absolute (in world units).
   * @details Name "offset", type Property::VECTOR2, animatable.
   * @SINCE_1_2.60
   *
   * @see OFFSET_POLICY
   */
  OFFSET,

  /**
   * @brief Size of the visual, which can be either relative (percentage [0.0f to 1.0f] of the parent) or absolute (in world units).
   * @details Name "size", type Property::VECTOR2, animatable.
   * @see SIZE_POLICY
   */
  SIZE,

  /**
   * @brief The origin of the visual within its control area.
   * @details Name "origin", type Align::Type (Property::INTEGER) or Property::STRING.
   * @see Toolkit::Align
   * @SINCE_1_2.60
   * @note The default is Align::TOP_BEGIN.
   */
  ORIGIN,

  /**
   * @brief The anchor-point of the visual
   * @details Name "anchorPoint", type Align::Type (Property::INTEGER) or Property::STRING.
   * @see Toolkit::Align
   * @SINCE_1_2.60
   * @note The default is Align::TOP_BEGIN.
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
   *                                     .Add( Visual::Property::TRANSFORM,
   *                                           Property::Array().Add( Toolkit::Visual::Transform::Property::OFFSET_POLICY, Vector2( Policy::ABSOLUTE, Policy::RELATIVE ) ) )
   *                                                            .Add( Toolkit::Visual::Transform::Property::OFFSET, Vector2( 10, 1.0f ) ) );
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
   * @SINCE_1_2.60
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
   * @SINCE_1_2.60
   * @note By default, both the width and the height is RELATIVE to the control's size.
   */
  SIZE_POLICY,
};

} // namespace Property

} // namespace Transform

/**
 * @brief Shader for Visuals.
 * @SINCE_1_1.45
 */
namespace Shader
{
/**
 * @brief Shader Property.
 * @SINCE_1_1.45
 */
namespace Property
{
/**
 * @brief The type of Shader.
 * @SINCE_1_1.45
 */
enum
{
  /**
   * @brief The vertex shader.
   * @details Name "vertexShader", type Property::STRING or Property::ARRAY of Property::STRING.
   *          A Property::ARRAY of Property::STRING values can be used to split the shader string over multiple lines.
   * @SINCE_1_1.45
   * @note Optional
   * @note If not supplied, the visual's already set vertex shader is used.
   */
  VERTEX_SHADER,

  /**
   * @brief The fragment shader.
   * @details Name "fragmentShader", type Property::STRING or Property::ARRAY of Property::STRING.
   *          A Property::ARRAY of Property::STRING values can be used to split the shader string over multiple lines.
   * @SINCE_1_1.45
   * @note Optional
   * @note If not supplied, the visual's already set fragment shader is used.
   */
  FRAGMENT_SHADER,

  /**
   * @brief How to subdivide the grid along the X-Axis.
   * @details Name "subdivideGridX", type Property::INTEGER.
   * @SINCE_1_1.45
   * @note Optional
   * @note If not supplied, the default is 1.
   * @note Value should be greater than or equal to 1.
   */
  SUBDIVIDE_GRID_X,

  /**
   * @brief How to subdivide the grid along the Y-Axis.
   * @details Name "subdivideGridY", type Property::INTEGER.
   * @SINCE_1_1.45
   * @note Optional
   * @note If not supplied, the default is 1.
   * @note Value should be greater than or equal to 1.
   */
  SUBDIVIDE_GRID_Y,

  /**
   * @brief Hints for rendering.
   * @details Name "hints", type Dali::Shader::Hint (Property::INTEGER), Property::STRING or Property::ARRAY of Property::STRING.
   * @SINCE_1_1.45
   * @note Optional
   * @note If not supplied, the default is Dali::Shader::Hint::NONE.
   */
  HINTS,
};

} // namespace Property

} // namespace Shader

/**
 * @brief Status of resource which is used for visual.
 * @SINCE_1_3_5
 */
enum class ResourceStatus
{
  PREPARING, /// Resource is prepared.    @SINCE_1_3_5
  READY,     /// Resource is ready.       @SINCE_1_3_5
  FAILED     /// Resource is fail to load @SINCE_1_3_5
};

} // namespace Visual

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_VISUAL_PROPERTIES_H
