#ifndef DALI_TOOLKIT_MESH_VISUAL_PROPERTIES_H
#define DALI_TOOLKIT_MESH_VISUAL_PROPERTIES_H

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

/**
 * @addtogroup dali_toolkit_visuals
 * @{
 */

/**
 * @brief MeshVisual is to render a mesh using a .obj file, optionally with textures provided by a mtl file.
 * @SINCE_1_1.45
 */
namespace MeshVisual
{

/**
 * @brief MeshVisual Property
 * @SINCE_1_1.45
 */
namespace Property
{

/**
 * @brief MeshVisual Property
 * @SINCE_1_1.45
 */
enum
{
  /**
   * @brief The location of the ".obj" file.
   * @details Name "objectUrl", type Property::STRING.
   * @SINCE_1_1.45
   * @note Mandatory.
   */
  OBJECT_URL = VISUAL_PROPERTY_START_INDEX,

  /**
   * @brief The location of the ".mtl" file.
   * @details Name "materialUrl", type Property::STRING.
   * @SINCE_1_1.45
   * @note Optional. If not specified, then a textureless object is assumed.
   */
  MATERIAL_URL,

  /**
   * @brief Path to the directory the textures (including gloss and normal) are stored in.
   * @details Name "texturesPath", type Property::STRING.
   * @SINCE_1_1.45
   * @note Mandatory if using material.
   */
  TEXTURES_PATH,

  /**
   * @brief Sets the type of shading mode that the mesh will use.
   * @details Name "shadingMode", type ShadingMode::Value (Property::INTEGER) or Property::STRING.
   * If anything the specified shading mode requires is missing, a simpler mode that can be handled with what has been supplied will be used instead.
   * @SINCE_1_1.45
   * @note Optional. If not specified, it will use the best it can support (will try ShadingMode::TEXTURED_WITH_DETAILED_SPECULAR_LIGHTING first).
   * @see ShadingMode::Value
   */
  SHADING_MODE,

  /**
   * @brief Whether to use mipmaps for textures or not.
   * @details Name "useMipmapping", type Property::BOOLEAN.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the default is true.
   */
  USE_MIPMAPPING,

  /**
   * @brief Whether to average normals at each point to smooth textures or not.
   * @details Name "useSoftNormals", type Property::BOOLEAN.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the default is true.
   */
  USE_SOFT_NORMALS,

  /**
   * @brief The position, in stage space, of the point light that applies lighting to the model.
   * @details Name "lightPosition", type Property::VECTOR3.
   * This is based off the stage's dimensions, so using the width and height of the stage halved will correspond to the center,
   * and using all zeroes will place the light at the top left corner.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the default is an offset outwards from the center of the screen.
   */
  LIGHT_POSITION,
};

} // namespace Property

/**
 * @brief The shading mode used by MeshVisual.
 * @SINCE_1_1.45
 */
namespace ShadingMode
{

/**
 * @brief The shading mode used by MeshVisual.
 * @SINCE_1_1.45
 */
enum Value
{
  TEXTURELESS_WITH_DIFFUSE_LIGHTING, ///< *Simplest*. One color that is lit by ambient and diffuse lighting. @SINCE_1_1.45
  TEXTURED_WITH_SPECULAR_LIGHTING, ///< Uses only the visual image textures provided with specular lighting in addition to ambient and diffuse lighting. @SINCE_1_1.45
  TEXTURED_WITH_DETAILED_SPECULAR_LIGHTING ///< Uses all textures provided including a gloss, normal and texture map along with specular, ambient and diffuse lighting. @SINCE_1_1.45
};

} // namespace ShadingMode

} // namespace MeshVisual

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_MESH_VISUAL_PROPERTIES_H
