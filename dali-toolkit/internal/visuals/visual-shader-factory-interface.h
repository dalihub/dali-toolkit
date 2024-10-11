#ifndef DALI_TOOLKIT_VISUAL_SHADER_FACTORY_INTERFACE_H
#define DALI_TOOLKIT_VISUAL_SHADER_FACTORY_INTERFACE_H

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
 *
 */

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/shader-precompiler.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali-toolkit/devel-api/visual-factory/precompile-shader-option.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>

namespace Dali
{
namespace Toolkit
{

using HashType = uint64_t;
using ShaderFlagList = std::vector<PrecompileShaderOption::Flag>;

namespace Internal
{
/**
 * @brief The VisualShaderFactoryInterface class provides a factory interface for creating visual shader
 */
class VisualShaderFactoryInterface
{
public:

  /**
   * @brief Structure to request shader info from visual shader factory.
   */
  struct RequestShaderInfo
  {
    VisualFactoryCache::ShaderType type;
    std::string name;
    std::string vertexPrefix;
    std::string fragmentPrefix;
  };

  VisualShaderFactoryInterface() = default;
  virtual ~VisualShaderFactoryInterface() = default;

  /**
   * @brief Add precompiled shader for precompile
   * @param[in] option shaderList for precompile
   */
  virtual bool AddPrecompiledShader(PrecompileShaderOption& option) = 0;

  /**
   * @brief Get precompiled shader for precompile
   * @param[out] shaders shaderList for precompile
   */
  virtual void GetPreCompiledShader(RawShaderData& shaders) = 0;

protected:
  std::vector<RequestShaderInfo> mRequestedPrecompileShader;
};

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_VISUAL_SHADER_FACTORY_INTERFACE_H
