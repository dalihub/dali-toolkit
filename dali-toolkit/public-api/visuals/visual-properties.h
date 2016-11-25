#ifndef DALI_TOOLKIT_VISUAL_PROPERTIES_H
#define DALI_TOOLKIT_VISUAL_PROPERTIES_H

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

namespace Visual
{

/**
 * @brief All the visual types.
 * @SINCE_1_1.45
 */
enum Type
{
  BORDER, ///< Renders a solid color as an internal border to the control's quad. @SINCE_1_1.45
  COLOR, ///< Renders a solid color to the control's quad. @SINCE_1_1.45
  GRADIENT, ///< Renders a smooth transition of colors to the control's quad. @SINCE_1_1.45
  IMAGE, ///< Renders an image into the control's quad. @SINCE_1_1.45
  MESH, ///< Renders a mesh using an "obj" file, optionally with textures provided by an "mtl" file. @SINCE_1_1.45
  PRIMITIVE, ///< Renders a simple 3D shape, such as a cube or sphere. @SINCE_1_1.45
  WIREFRAME ///< Renders a simple wire-frame outlining a quad. @SINCE_1_2_2
};

namespace Property
{

enum
{
  /**
   * @brief The index for the visual type.
   * @details Name "visualType", type [Type](@ref Dali::Toolkit::Visual::Type) (Property::INTEGER) or Property::STRING.
   * @SINCE_1_1.45
   * @note Mandatory.
   * @see Type
   */
  TYPE = VISUAL_PROPERTY_BASE_START_INDEX,

  /**
   * @brief The shader to use in the visual.
   * @details Name "shader", type Property::Map.
   * @SINCE_1_1.45
   * @note Optional.
   * @note Will override the existing shaders.
   * @see Shader::Property
   */
  SHADER
};

} // namespace Property

namespace Shader
{

namespace Property
{

enum
{
  /**
   * @brief The vertex shader.
   * @details Name "vertexShader", type Property::STRING.
   * @SINCE_1_1.45
   * @note Optional
   * @note If not supplied, the visual's already set vertex shader is used.
   */
  VERTEX_SHADER,

  /**
   * @brief The fragment shader.
   * @details Name "fragmentShader", type Property::STRING.
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

} // namespace Visual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_VISUAL_PROPERTIES_H
