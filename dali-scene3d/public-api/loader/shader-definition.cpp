/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-scene3d/public-api/loader/shader-definition.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/shader-integ.h>
#include <dali/public-api/object/property-array.h>
#include <regex>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-scene3d/public-api/loader/utils.h>

#include <dali/integration-api/debug.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_MODEL_SHADER_DEFINITION");
#endif
} // namespace

namespace Dali::Scene3D::Loader
{
namespace
{
const char* SHADER_HINT_OUTPUT_IS_TRANSPARENT("OUTPUT_IS_TRANSPARENT"); ///< Might generate transparent alpha from opaque inputs.
const char* SHADER_HINT_MODIFIES_GEOMETRY("MODIFIES_GEOMETRY");         ///< Might change position of vertices, this option disables any culling optimizations.
} // namespace

ShaderDefinition::ShaderDefinition(const ShaderDefinition& other)
: mRendererState(other.mRendererState),
  mVertexShaderPath(other.mVertexShaderPath),
  mFragmentShaderPath(other.mFragmentShaderPath),
  mDefines(other.mDefines),
  mHints(other.mHints),
  mUniforms(other.mUniforms),
  mUniformBlocks(other.mUniformBlocks),
  mShadowOptionHash(other.mShadowOptionHash),
  mUseBuiltInShader(other.mUseBuiltInShader)
{
}

void ApplyDefine(std::string& shaderCode, const std::string& definevar)
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

void RedefineMacro(std::string& shaderCode, const std::string& macro, const std::string& value)
{
  if(!value.empty())
  {
    std::string definition = "#define " + macro;
    std::size_t found      = shaderCode.find(definition);
    if(found != std::string::npos)
    {
      std::size_t insertionPoint = found + definition.length();

      // Automatically insert line-continuation character into value
      std::regex                 re("\n");
      std::sregex_token_iterator first{value.begin(), value.end(), re, -1}, last;
      for(auto i = first; i != last; ++i)
      {
        std::string line = std::string(" \\\n") + (*i).str();
        shaderCode.insert(insertionPoint, line);
        insertionPoint += line.length();
      }
    }
  }
  else
  {
    std::size_t invocation = shaderCode.rfind(macro);
    if(invocation != std::string::npos)
    {
      std::size_t start = shaderCode.rfind("\n", invocation);
      std::size_t end   = shaderCode.find("\n", invocation);
      shaderCode.erase(start, end - start);
    }
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
    raw.mVertexShaderSource         = SHADER_DEFAULT_PHYSICALLY_BASED_SHADER_VERT.data();
    raw.mFragmentShaderSource       = SHADER_DEFAULT_PHYSICALLY_BASED_SHADER_FRAG.data();
    raw.mShadowVertexShaderSource   = SHADER_SHADOW_MAP_SHADER_VERT.data();
    raw.mShadowFragmentShaderSource = SHADER_SHADOW_MAP_SHADER_FRAG.data();
  }

  if(!fail)
  {
    for(const auto& definevar : mDefines)
    {
      ApplyDefine(raw.mVertexShaderSource, definevar);
      ApplyDefine(raw.mFragmentShaderSource, definevar);
      ApplyDefine(raw.mShadowVertexShaderSource, definevar);
      ApplyDefine(raw.mShadowFragmentShaderSource, definevar);
    }
    for(const auto& macroDef : mMacros)
    {
      RedefineMacro(raw.mVertexShaderSource, macroDef.macro, macroDef.definition);
      RedefineMacro(raw.mFragmentShaderSource, macroDef.macro, macroDef.definition);
      RedefineMacro(raw.mShadowVertexShaderSource, macroDef.macro, macroDef.definition);
      RedefineMacro(raw.mShadowFragmentShaderSource, macroDef.macro, macroDef.definition);
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

  Property::Map map[2];
  map[0]["vertex"]        = raw.mVertexShaderSource;
  map[0]["fragment"]      = raw.mFragmentShaderSource;
  map[0]["renderPassTag"] = 0;
  map[0]["hints"]         = static_cast<Shader::Hint::Value>(hints);

  map[1]["vertex"]        = raw.mShadowVertexShaderSource;
  map[1]["fragment"]      = raw.mShadowFragmentShaderSource;
  map[1]["renderPassTag"] = 10;

  if(mUseBuiltInShader)
  {
    std::ostringstream oss;
    oss << "_0x" << std::hex << mShadowOptionHash;
    map[0]["name"] = std::string("SCENE3D_PBR") + oss.str();
    map[1]["name"] = std::string("SCENE3D_SHADOW_MAP") + oss.str();
  }
  else
  {
    map[0]["name"] = "SCENE3D_CUSTOM";
    map[1]["name"] = "SCENE3D_CUSTOM_SHADOW";
  }

  Property::Array array;
  array.PushBack(map[0]);
  array.PushBack(map[1]);

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Vert Shader src: \n%s\n", raw.mVertexShaderSource.c_str());

  // Create strong-connected ubo shader since shader definition could be removed after scene load completed.
  Shader shader = Dali::Integration::ShaderNewWithUniformBlock(array, mUniformBlocks, true);
  for(Property::Map::SizeType i0 = 0, i1 = mUniforms.Count(); i0 != i1; ++i0)
  {
    auto pair = mUniforms.GetKeyValue(i0);
    DALI_ASSERT_ALWAYS(pair.first.type == Property::Key::STRING);
    shader.RegisterProperty(pair.first.stringKey, pair.second);
  }

  return shader;
}

} // namespace Dali::Scene3D::Loader
