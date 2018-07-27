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
 */

#include <dali-toolkit/devel-api/shader-effects/alpha-discard-effect.h>
#include <dali-toolkit/devel-api/builder/base64-encoding.h>
#include <dali-toolkit/devel-api/graphics/builtin-shader-extern-gen.h>

namespace Dali
{
namespace Toolkit
{

Property::Map CreateAlphaDiscardEffect()
{
  Property::Map customShader;
  Property::Value fragmentShader;
  EncodeBase64PropertyData(fragmentShader, GraphicsGetBuiltinShader("SHADER_ALPHA_DISCARD_FRAG"));
  customShader[ Visual::Shader::Property::FRAGMENT_SHADER ] = fragmentShader;

  Property::Map map;
  map[ Toolkit::Visual::Property::SHADER ] = customShader;
  return map;
}

} // namespace Toolkit
} // namespace Dali
