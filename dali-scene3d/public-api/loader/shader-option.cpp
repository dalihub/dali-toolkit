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
#include <dali-scene3d/public-api/loader/shader-option.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <ostream>
#include <sstream>
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
static const char*        ADD_EXTRA_SKINNING_ATTRIBUTES{"ADD_EXTRA_SKINNING_ATTRIBUTES"};
static const char*        ADD_EXTRA_WEIGHTS{"ADD_EXTRA_WEIGHTS"};

inline void HashString(uint64_t& hash, const char* string)
{
  char c;
  while((c = *string++))
  {
    hash = hash * 33 + c;
  }
}
} // namespace

ShaderOption::ShaderOption(const ShaderOption& rhs)
{
  mOptionHash = rhs.mOptionHash;
  for(auto& macroDef : rhs.mMacros)
  {
    mMacros.emplace_back(macroDef);
  }
}

ShaderOption& ShaderOption::operator=(const ShaderOption& rhs)
{
  if(this != &rhs)
  {
    mOptionHash = rhs.mOptionHash;
    for(auto& macroDef : rhs.mMacros)
    {
      mMacros.emplace_back(macroDef);
    }
  }
  return *this;
}

void ShaderOption::SetTransparency()
{
  mOptionHash |= (1 << NUMBER_OF_OPTIONS);
}

void ShaderOption::AddOption(Type shaderOptionType)
{
  mOptionHash |= (1 << static_cast<uint32_t>(shaderOptionType));
}

void ShaderOption::AddJointMacros(size_t numberOfJointSets)
{
  // Add options for ADD_EXTRA_SKINNING_ATTRIBUTES and ADD_EXTRA_WEIGHTS:
  if(numberOfJointSets > 1)
  {
    std::ostringstream attributes;
    std::ostringstream weights;
    for(size_t i = 1; i < numberOfJointSets; ++i)
    {
      attributes << "in vec4 aJoints" << i << ";\n";
      attributes << "in vec4 aWeights" << i << ";\n";

      weights << "bone +=\n"
              << "uBone[int(aJoints" << i << ".x)] * aWeights" << i << ".x +\n"
              << "uBone[int(aJoints" << i << ".y)] * aWeights" << i << ".y +\n"
              << "uBone[int(aJoints" << i << ".z)] * aWeights" << i << ".z +\n"
              << "uBone[int(aJoints" << i << ".w)] * aWeights" << i << ".w;\n";
    }
    AddMacroDefinition(ADD_EXTRA_SKINNING_ATTRIBUTES, attributes.str());
    AddMacroDefinition(ADD_EXTRA_WEIGHTS, weights.str());
  }
}

void ShaderOption::AddMacroDefinition(std::string macro, std::string definition)
{
  auto iter = std::find_if(mMacros.begin(), mMacros.end(), [macro](ShaderOption::MacroDefinition& md) { return md.macro == macro; });
  if(iter != mMacros.end())
  {
    iter->definition = definition;
  }
  else
  {
    mMacros.emplace_back(MacroDefinition{macro, definition});
  }
}

const std::vector<ShaderOption::MacroDefinition>& ShaderOption::GetMacroDefinitions() const
{
  return mMacros;
}

uint64_t ShaderOption::GetOptionHash() const
{
  uint64_t optionHash = mOptionHash;
  if(!mMacros.empty())
  {
    uint64_t hash = 5381;
    for(auto& macroDef : mMacros)
    {
      HashString(hash, macroDef.macro.c_str());
      HashString(hash, macroDef.definition.c_str());
    }
    optionHash |= (hash << 32 & 0xFFFFFFFF00000000);
  }
  return optionHash;
}

void ShaderOption::GetDefines(std::vector<std::string>& defines) const
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

std::string_view ShaderOption::GetDefineKeyword(Type shaderOptionType)
{
  return OPTION_KEYWORD[static_cast<uint32_t>(shaderOptionType)];
}

} // namespace Dali::Scene3D::Loader
