/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include "dali-scene3d/public-api/loader/shader-definition.h"
#include <dali-scene3d/internal/graphics/builtin-shader-extern-gen.h>
#include "dali-scene3d/public-api/loader/utils.h"

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
namespace
{
const std::string SHADER_HINT_OUTPUT_IS_TRANSPARENT("OUTPUT_IS_TRANSPARENT"); ///< Might generate transparent alpha from opaque inputs.
const std::string SHADER_HINT_MODIFIES_GEOMETRY("MODIFIES_GEOMETRY");         ///< Might change position of vertices, this option disables any culling optimizations.

} // namespace

ShaderDefinition::ShaderDefinition(const ShaderDefinition& other)
: mRendererState(other.mRendererState),
  mVertexShaderPath(other.mVertexShaderPath),
  mFragmentShaderPath(other.mFragmentShaderPath),
  mDefines(other.mDefines),
  mHints(other.mHints),
  mUniforms(other.mUniforms),
  mUseBuiltInShader(other.mUseBuiltInShader)
{
}

void ShaderDefinition::ApplyDefine(std::string& shaderCode, const std::string& definevar)
{
  const std::string IF_1 = "#if 1";

  std::size_t found = shaderCode.find(definevar);
  while(found != std::string::npos)
  {
    // Greater then "@" character means is a letter,
    // therefore is not has the definevar we looking for.
    if((found + definevar.length()) < shaderCode.length() && shaderCode.at(found + definevar.length()) > '@')
    {
      found = shaderCode.find(definevar, found + definevar.length());
      continue;
    }
    if(found > 0u && shaderCode.at(found - 1) > '@')
    {
      found = shaderCode.find(definevar, found + definevar.length());
      continue;
    }

    std::size_t defidx     = shaderCode.rfind("#ifdef", found);
    std::size_t newlineidx = shaderCode.rfind("\n", found);
    if(newlineidx < defidx && defidx != std::string::npos)
    {
      shaderCode.replace(defidx, found - defidx + definevar.length(), IF_1);
      found = defidx + IF_1.length();
    }
    else
    {
      found += definevar.length();
    }
    found = shaderCode.find(definevar, found);
  }
}

ShaderDefinition::RawData
ShaderDefinition::LoadRaw(const std::string& shadersPath) const
{
  RawData raw;

  bool fail = false;
  if(!mUseBuiltInShader)
  {
    raw.mVertexShaderSource = LoadTextFile((shadersPath + mVertexShaderPath).c_str(), &fail);
    if(!fail)
    {
      raw.mFragmentShaderSource = LoadTextFile((shadersPath + mFragmentShaderPath).c_str(), &fail);
      if(fail)
      {
        ExceptionFlinger(ASSERT_LOCATION) << "Failed to load shader source from '" << shadersPath + mFragmentShaderPath << "'.";
      }
    }
    else
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Failed to load shader source from '" << shadersPath + mVertexShaderPath << "'.";
    }
  }
  else
  {
    raw.mVertexShaderSource   = SHADER_DEFAULT_PHYSICALLY_BASED_SHADER_VERT.data();
    raw.mFragmentShaderSource = SHADER_DEFAULT_PHYSICALLY_BASED_SHADER_FRAG.data();
  }

  if(!fail)
  {
    for(auto definevar : mDefines)
    {
      ApplyDefine(raw.mVertexShaderSource, definevar);
      ApplyDefine(raw.mFragmentShaderSource, definevar);
    }
  }

  return raw;
}

Shader ShaderDefinition::Load(RawData&& raw) const
{
  uint32_t hints = Shader::Hint::NONE;
  for(const auto& hint : mHints)
  {
    if(hint == SHADER_HINT_OUTPUT_IS_TRANSPARENT)
    {
      hints |= Shader::Hint::OUTPUT_IS_TRANSPARENT;
    }
    else if(hint == SHADER_HINT_MODIFIES_GEOMETRY)
    {
      hints |= Shader::Hint::MODIFIES_GEOMETRY;
    }
  }

  Shader shader = Shader::New(raw.mVertexShaderSource, raw.mFragmentShaderSource, static_cast<Shader::Hint::Value>(hints));
  for(Property::Map::SizeType i0 = 0, i1 = mUniforms.Count(); i0 != i1; ++i0)
  {
    auto pair = mUniforms.GetKeyValue(i0);
    DALI_ASSERT_ALWAYS(pair.first.type == Property::Key::STRING);
    shader.RegisterProperty(pair.first.stringKey, pair.second);
  }

  return shader;
}

} // namespace Loader
} // namespace Scene3D
} // namespace Dali
