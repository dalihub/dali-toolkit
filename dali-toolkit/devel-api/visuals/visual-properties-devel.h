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

  ANIMATED_GRADIENT = ANIMATED_IMAGE + 1,  ///< Renders an animated gradient.
};

} // namespace DevelVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_VISUALS_VISUAL_PROPERTIES_DEVEL_H
