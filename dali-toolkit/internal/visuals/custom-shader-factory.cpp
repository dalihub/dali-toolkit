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
#include <dali-toolkit/internal/visuals/custom-shader-factory.h>

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
CustomShaderFactory::CustomShaderFactory()
{
}

CustomShaderFactory::~CustomShaderFactory()
{
}

bool CustomShaderFactory::AddPrecompiledShader(PrecompileShaderOption& option)
{
  auto shaderName     = option.GetShaderName();
  auto vertexShader   = option.GetVertexShader();
  auto fragmentShader = option.GetFragmentShader();
  return SavePrecompileShader(std::move(shaderName), std::move(vertexShader), std::move(fragmentShader));
}

void CustomShaderFactory::GetPreCompiledShader(ShaderPreCompiler::RawShaderData& shaders)
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
  shaders.custom         = true;
}

bool CustomShaderFactory::SavePrecompileShader(std::string&& shaderName, std::string&& vertexShader, std::string&& fragmentShader)
{
  DALI_LOG_RELEASE_INFO("Add precompile shader success!!(%s)", shaderName.c_str());

  RequestShaderInfo info;
  info.type           = VisualFactoryCache::SHADER_TYPE_MAX; ///< Not be used
  info.name           = std::move(shaderName);
  info.vertexPrefix   = std::move(vertexShader);
  info.fragmentPrefix = std::move(fragmentShader);
  mRequestedPrecompileShader.emplace_back(std::move(info));
  return true;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
