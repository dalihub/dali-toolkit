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
#include <dali-toolkit/devel-api/shader-effects/distance-field-effect.h>

// EXTERNAL INCLUDES
#include <dali/public-api/rendering/shader.h>
#include <string.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Toolkit
{
Dali::Property::Map CreateDistanceFieldEffect()
{
  const char* fragmentShaderPrefix("#extension GL_OES_standard_derivatives : enable\n");

  const char* fragmentShader = SHADER_DISTANCE_FIELD_EFFECT_FRAG.data();

  Property::Map map;

  Property::Map customShader;

  std::string fragmentShaderString;
  fragmentShaderString.reserve(strlen(fragmentShaderPrefix) + strlen(fragmentShader));
  fragmentShaderString.append(fragmentShaderPrefix);
  fragmentShaderString.append(fragmentShader);

  customShader[Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShaderString;
  customShader[Visual::Shader::Property::HINTS]           = Shader::Hint::OUTPUT_IS_TRANSPARENT;

  map[Toolkit::Visual::Property::SHADER] = customShader;
  return map;
}

} // namespace Toolkit

} // namespace Dali
