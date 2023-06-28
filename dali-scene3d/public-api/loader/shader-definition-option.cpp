/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali-scene3d/public-api/loader/shader-definition-option.h>

// EXTERNAL INCLUDES
#include <string>

namespace Dali::Scene3D::Loader
{
namespace
{
static constexpr std::string_view OPTION_KEYWORD[] =
  {
    "GLTF_CHANNELS",
    "THREE_TEX",
    "BASECOLOR_TEX",
    "METALLIC_ROUGHNESS_TEX",
    "NORMAL_TEX",
    "OCCLUSION",
    "EMISSIVE_TEXTURE",
    "ALPHA_TEST",
    "SSS",
    "MATERIAL_SPECULAR_TEXTURE",
    "MATERIAL_SPECULAR_COLOR_TEXTURE",
    "SKINNING",
    "FLIP_V",
    "COLOR_ATTRIBUTE",
    "VEC4_TANGENT",
    "MORPH_POSITION",
    "MORPH_NORMAL",
    "MORPH_TANGENT",
    "MORPH_VERSION_2_0",
};
static constexpr uint32_t NUMBER_OF_OPTIONS = sizeof(OPTION_KEYWORD) / sizeof(OPTION_KEYWORD[0]);
} // namespace

void ShaderDefinitionOption::SetTransparency()
{
  mOptionHash |= (1 << NUMBER_OF_OPTIONS);
}

void ShaderDefinitionOption::AddOption(Type shaderDefinitionOptionType)
{
  mOptionHash |= (1 << static_cast<uint32_t>(shaderDefinitionOptionType));
}

uint64_t ShaderDefinitionOption::GetOptionHash() const
{
  return mOptionHash;
}

void ShaderDefinitionOption::GetDefines(std::vector<std::string>& defines) const
{
  defines.clear();
  for(uint32_t i = 0; i < NUMBER_OF_OPTIONS; ++i)
  {
    if(mOptionHash & 1 << i)
    {
      defines.push_back(OPTION_KEYWORD[i].data());
    }
  }
}

std::string_view ShaderDefinitionOption::GetDefineKeyword(Type shaderDefinitionOptionType)
{
  return OPTION_KEYWORD[static_cast<uint32_t>(shaderDefinitionOptionType)];
}

} // namespace Dali::Scene3D::Loader
