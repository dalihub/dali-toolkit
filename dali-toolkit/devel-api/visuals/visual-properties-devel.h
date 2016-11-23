#ifndef DALI_TOOLKIT_DEVEL_API_VISUALS_VISUAL_PROPERTIES_DEVEL_H
#define DALI_TOOLKIT_DEVEL_API_VISUALS_VISUAL_PROPERTIES_DEVEL_H

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

   * @note Optional.
   * @see DevelVisual::Transform::Property
   */
  TRANSFORM = SHADER+1//Dali::Toolkit::Visual::Property::SHADER+1
};

} //namespace Property

namespace Transform
{

namespace Property
{

enum Type
{
  /**
   * @brief Offset of the visual. It can be either relative (percentage of the parent)
   * or absolute (in world units).
   * @details Name "offset", type Property::VECTOR2
   */
  OFFSET,

  /**
   * @brief Size of the visual. It can be either relative (percentage of the parent)
   * or absolute (in world units).
   * @details Name "size", type Property::VECTOR2
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
   * @brief Indicates which components of the offset and size are relative
   * (percentage of the parent) or absolute (in world units).
   * 0 indicates the component is relative, and 1 absolute.
   * @details Name "offsetSizeMode", type Property::VECTOR4
   */
  OFFSET_SIZE_MODE
};

} //namespace Property

} // namespace Transform

} // namespace DevelVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_VISUALS_VISUAL_PROPERTIES_DEVEL_H
