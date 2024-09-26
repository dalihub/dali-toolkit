/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/npatch/npatch-shader-factory.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
NpatchShaderFactory::NpatchShaderFactory()
: mNpatchXStretchCount(0),
  mNpatchYStretchCount(0),
  mNpatchMaskingEnable(false)
{
}

NpatchShaderFactory::~NpatchShaderFactory()
{
}

bool NpatchShaderFactory::AddPrecompiledShader(PrecompileShaderOption& option)
{
  ShaderFlagList shaderOption = option.GetShaderOptions();

  // Find Masking flag
  for(uint32_t i = 0; i < shaderOption.size(); ++i)
  {
    if(shaderOption[i] == PrecompileShaderOption::Flag::MASKING)
    {
      mNpatchMaskingEnable = true;
    }
  }

  mNpatchXStretchCount = option.GetNpatchXStretchCount();
  mNpatchYStretchCount = option.GetNpatchYStretchCount();

  std::string vertexShader;
  std::string fragmentShader;
  GetVertexShader(vertexShader);
  GetFragmentShader(fragmentShader);

  VisualFactoryCache::ShaderType shaderType = mNpatchMaskingEnable ? VisualFactoryCache::ShaderType::NINE_PATCH_MASK_SHADER : VisualFactoryCache::ShaderType::NINE_PATCH_SHADER;
  return SavePrecompileShader(shaderType, std::move(vertexShader), std::move(fragmentShader));
}

void NpatchShaderFactory::GetPreCompiledShader(ShaderPreCompiler::RawShaderData& shaders)
{
  std::vector<std::string> vertexPrefix;
  std::vector<std::string> fragmentPrefix;
  std::vector<std::string> shaderName;

  uint32_t shaderCount = 0;

  shaders.shaderCount = 0;

  // precompile requested shader first
  for(uint32_t i = 0; i < mRequestedPrecompileShader.size(); i++)
  {
    vertexPrefix.push_back(std::move(mRequestedPrecompileShader[i].vertexPrefix));
    fragmentPrefix.push_back(std::move(mRequestedPrecompileShader[i].fragmentPrefix));
    shaderName.push_back(std::move(mRequestedPrecompileShader[i].name));
    shaderCount++;
  }

  // Clean up requested precompile shader list
  mRequestedPrecompileShader.clear();

  shaders.vertexPrefix   = std::move(vertexPrefix);
  shaders.fragmentPrefix = std::move(fragmentPrefix);
  shaders.shaderName     = std::move(shaderName);
  shaders.vertexShader   = ""; // Custom shader use prefix shader only. No need to set vertexShader and fragmentShader.
  shaders.fragmentShader = ""; // Custom shader use prefix shader only. No need to set vertexShader and fragmentShader.
  shaders.shaderCount    = shaderCount;
  shaders.custom         = true; ///< Note that npatch shader is kind of custom shader.
}

void NpatchShaderFactory::GetVertexShader(std::string& vertexShader) const
{
  if(DALI_LIKELY((mNpatchXStretchCount == 1 && mNpatchYStretchCount == 1) ||
                 (mNpatchXStretchCount == 0 && mNpatchYStretchCount == 0)))
  {
    vertexShader += SHADER_NPATCH_VISUAL_3X3_SHADER_VERT;
  }
  else if(mNpatchXStretchCount > 0 || mNpatchYStretchCount > 0)
  {
    std::stringstream vertextShaderStream;
    vertextShaderStream << "#define FACTOR_SIZE_X " << mNpatchXStretchCount + 2 << "\n"
                        << "#define FACTOR_SIZE_Y " << mNpatchYStretchCount + 2 << "\n"
                        << SHADER_NPATCH_VISUAL_SHADER_VERT;
    vertexShader += vertextShaderStream.str();
  }
}

void NpatchShaderFactory::GetFragmentShader(std::string& fragmentShader) const
{
  fragmentShader += (mNpatchMaskingEnable ? SHADER_NPATCH_VISUAL_MASK_SHADER_FRAG : SHADER_NPATCH_VISUAL_SHADER_FRAG);
}

bool NpatchShaderFactory::SavePrecompileShader(VisualFactoryCache::ShaderType shader, std::string&& vertexShader, std::string&& fragmentShader)
{
  for(uint32_t i = 0u; i < mRequestedPrecompileShader.size(); i++)
  {
    if(mRequestedPrecompileShader[i].type == shader)
    {
      DALI_LOG_WARNING("This shader already requsted(%s).", Scripting::GetLinearEnumerationName<VisualFactoryCache::ShaderType>(mRequestedPrecompileShader[i].type, VISUAL_SHADER_TYPE_TABLE, VISUAL_SHADER_TYPE_TABLE_COUNT));
      return false;
    }
  }

  std::string shaderName = Scripting::GetLinearEnumerationName<VisualFactoryCache::ShaderType>(shader, VISUAL_SHADER_TYPE_TABLE, VISUAL_SHADER_TYPE_TABLE_COUNT);
  if(!((mNpatchXStretchCount == 1 && mNpatchYStretchCount == 1) || (mNpatchXStretchCount == 0 && mNpatchYStretchCount == 0)))
  {
    if(mNpatchXStretchCount > 0 || mNpatchYStretchCount > 0)
    {
      std::stringstream shaderNameStream;
      shaderNameStream << "NINE_PATCH_SHADER_" << mNpatchXStretchCount << "x" << mNpatchYStretchCount;
      shaderName = shaderNameStream.str();
    }
  }

  DALI_LOG_RELEASE_INFO("Add precompile shader success!!(%s)", shaderName.c_str());

  RequestShaderInfo info;
  info.type           = shader;
  info.name           = std::move(shaderName);
  info.vertexPrefix   = std::move(vertexShader);
  info.fragmentPrefix = std::move(fragmentShader);
  mRequestedPrecompileShader.emplace_back(std::move(info));
  return true;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
