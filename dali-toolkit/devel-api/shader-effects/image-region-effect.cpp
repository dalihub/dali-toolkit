/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
 */

// CLASS HEADER
#include <dali-toolkit/devel-api/shader-effects/image-region-effect.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>

namespace Dali
{
namespace Toolkit
{

Property::Map CreateImageRegionEffect()
{
  Property::Map map;

  Property::Map customShader;
  customShader[Visual::Shader::Property::VERTEX_SHADER] = SHADER_IMAGE_REGION_EFFECT_VERT.data();

  map[Toolkit::Visual::Property::SHADER] = customShader;
  return map;
}

} // Toolkit

} // Dali
