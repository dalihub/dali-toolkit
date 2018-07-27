#ifndef DALI_TOOLKIT_DEVEL_API_VISUALS_VISUAL_PROPERTIES_DEVEL_H
#define DALI_TOOLKIT_DEVEL_API_VISUALS_VISUAL_PROPERTIES_DEVEL_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
  BORDER         = Dali::Toolkit::Visual::BORDER,
  COLOR          = Dali::Toolkit::Visual::COLOR,
  GRADIENT       = Dali::Toolkit::Visual::GRADIENT,
  IMAGE          = Dali::Toolkit::Visual::IMAGE,
  MESH           = Dali::Toolkit::Visual::MESH,
  PRIMITIVE      = Dali::Toolkit::Visual::PRIMITIVE,
  WIREFRAME      = Dali::Toolkit::Visual::WIREFRAME,
  TEXT           = Dali::Toolkit::Visual::TEXT,
  N_PATCH        = Dali::Toolkit::Visual::N_PATCH,
  SVG            = Dali::Toolkit::Visual::SVG,
  ANIMATED_IMAGE = Dali::Toolkit::Visual::ANIMATED_IMAGE,

  ANIMATED_GRADIENT = ANIMATED_IMAGE + 1,  //< Renders an animated gradient.
};

/**
 * @brief Visual Properties
 */
namespace Property
{
enum Type
{
  TYPE                = Dali::Toolkit::Visual::Property::TYPE,
  SHADER              = Dali::Toolkit::Visual::Property::SHADER,
  TRANSFORM           = Dali::Toolkit::Visual::Property::TRANSFORM,
  PREMULTIPLIED_ALPHA = Dali::Toolkit::Visual::Property::PREMULTIPLIED_ALPHA,
  MIX_COLOR           = Dali::Toolkit::Visual::Property::MIX_COLOR,
  OPACITY             = Dali::Toolkit::Visual::Property::OPACITY,

  /**
   * @brief The fitting mode of the visual
   * @details Name "fittingMode", type FittingMode (Property::INTEGER) or Property::STRING.
   * @see DevelVisual::FittingMode
   * @note The default is defined by the type of visual (if it's suitable to be stretched or not).
   */
  VISUAL_FITTING_MODE = OPACITY + 1,
};

} // namespace Property

/**
 * @brief The values of this enum determine how the visual should be fit to the view
 */
enum FittingMode
{
  FIT_KEEP_ASPECT_RATIO,  ///< The visual should be scaled to fit, preserving aspect ratio
  FILL,                   ///< The visual should be stretched to fill, not preserving aspect ratio
};



namespace Shader
{

namespace Property
{

enum
{
  VERTEX_SHADER = Dali::Toolkit::Visual::Shader::Property::VERTEX_SHADER,
  FRAGMENT_SHADER = Dali::Toolkit::Visual::Shader::Property::FRAGMENT_SHADER,
  SUBDIVIDE_GRID_X = Dali::Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_X,
  SUBDIVIDE_GRID_Y = Dali::Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_Y,
  HINTS = Dali::Toolkit::Visual::Shader::Property::HINTS,

  /**
   * @brief The specialization constants for the custom shader
   *
   * @details Name "specializationConstants", type Property::MAP
   * @note Key value pairs are used to write specialization constants for the shader:
   * Keys are checked against shader reflection, values must be valid types
   * for the shader.
   */
  SPECIALIZATION_CONSTANTS = HINTS + 1,
};

} // namespace Property

} // namespace Shader


} // namespace DevelVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_VISUALS_VISUAL_PROPERTIES_DEVEL_H
