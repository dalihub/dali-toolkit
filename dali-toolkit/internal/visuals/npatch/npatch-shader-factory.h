#ifndef DALI_TOOLKIT_NPATCH_SHADER_FACTORY_H
#define DALI_TOOLKIT_NPATCH_SHADER_FACTORY_H

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

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/shader-precompiler.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-shader-factory-interface.h>
#include <string_view>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * NpatchShaderFactory is an object that provides custom shader
 */
class NpatchShaderFactory : public VisualShaderFactoryInterface
{
public:
  /**
   * @brief Constructor
   */
  NpatchShaderFactory();

  /**
   * @brief Destructor
   */
  ~NpatchShaderFactory() override;

public: // Implementation of VisualShaderFactoryInterface
  /**
   * @copydoc Dali::Toolkit::VisualShaderFactoryInterface::AddPrecompiledShader
   */
  bool AddPrecompiledShader(PrecompileShaderOption& option) override;

  /**
   * @copydoc Dali::Toolkit::VisualShaderFactoryInterface::GetPreCompiledShader
   */
  void GetPreCompiledShader(ShaderPreCompiler::RawShaderData& shaders) override;

private:
  /**
   * @brief Get the NPatch vertex shader. this is used for generating pre-compiled shader.
   */
  void GetVertexShader(std::string& vertexShader) const;

  /**
   * @brief Get the NPatch fragment shader. this is used for generating pre-compiled shader
   */
  void GetFragmentShader(std::string& fragmentShader) const;

  /**
   * @brief Save the npatch shader
   */
  bool SavePrecompileShader(VisualFactoryCache::ShaderType shader, std::string&& vertexPrefix, std::string&& fragmentPrefix);

protected:
  /**
   * Undefined copy constructor.
   */
  NpatchShaderFactory(const NpatchShaderFactory&) = delete;

  /**
   * Undefined assignment operator.
   */
  NpatchShaderFactory& operator=(const NpatchShaderFactory& rhs) = delete;

private:
  // For Npatch
  uint32_t mNpatchXStretchCount;
  uint32_t mNpatchYStretchCount;
  bool     mNpatchMaskingEnable;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_NPATCH_SHADER_FACTORY_H
