#ifndef DALI_TOOLKIT_VISUAL_PROPERTIES_DEVEL_H
#define DALI_TOOLKIT_VISUAL_PROPERTIES_DEVEL_H

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
  BORDER = Dali::Toolkit::Visual::BORDER,
  COLOR = Dali::Toolkit::Visual::COLOR,
  GRADIENT = Dali::Toolkit::Visual::GRADIENT,
  IMAGE = Dali::Toolkit::Visual::IMAGE,
  MESH = Dali::Toolkit::Visual::MESH,
  PRIMITIVE = Dali::Toolkit::Visual::PRIMITIVE,
  WIREFRAME = Dali::Toolkit::Visual::WIREFRAME,
  TEXT = Dali::Toolkit::Visual::WIREFRAME + 1, ///< Renders text.
};

} // namespace DevelVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_VISUAL_PROPERTIES_DEVEL_H
