#ifndef DALI_SCENE3D_LOADER_SHADER_MANAGER_H_
#define DALI_SCENE3D_LOADER_SHADER_MANAGER_H_
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

// EXTERNAL INCLUDER
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/rendering/shader.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/loader/index.h>
#include <dali-scene3d/public-api/loader/material-definition.h>
#include <dali-scene3d/public-api/loader/mesh-definition.h>
#include <dali-scene3d/public-api/loader/renderer-state.h>
#include <dali-scene3d/public-api/loader/shader-option.h>

namespace Dali::Scene3D::Loader
{
struct NodeDefinition;
class ResourceBundle;
class ShaderManager;
typedef IntrusivePtr<ShaderManager> ShaderManagerPtr;

/**
 * @brief This class is to manage Shaders.
 * This class could be used as factory class to create Dali::Shader.
 * And once created Dali::Shader is kept in this manager and will be returned when the same Dali::Shader is requested to be created.
 */
class DALI_SCENE3D_API ShaderManager : public RefObject
{
public:
  ShaderManager();
  ~ShaderManager();

  /**
   * @brief Produces a Dali::Shader for the input materialDefinition and meshDefinition.
   * Returns a cached Dali::Shader if the requested Dali::Shader has already been created once.
   * (Although the input materialDefinition and meshDefinition are not identical to those used to create the cached Dali::Shader, they share the cached one.)
   * @param[in] materialDefinition MaterialDefinition that includes information of material to create Shader.
   * @param[in] meshDefinition meshDefinition that includes information of mesh to create Shader.
   * @return Dali::Shader for the materialDefinition and meshDefinition.
   */
  Dali::Shader ProduceShader(const MaterialDefinition& materialDefinition, const MeshDefinition& meshDefinition);

  /**
   * @brief Produces a Dali::Shader for the input ShaderOption
   * Returns a cached Dali::Shader if the requested Dali::Shader has already been created once.
   * @param[in] shaderOption shader option to create Shader.
   * @return Dali::Shader of the shader option
   */
  Dali::Shader ProduceShader(const ShaderOption& shaderOption);

  /**
   * @brief Returns RendererState of the input materialDefinition.
   * @param[in] materialDefinition MaterialDefinition to get RendererState
   * @return RendererState of the materialDefinition.
   */
  RendererState::Type GetRendererState(const MaterialDefinition& materialDefinition);

private:
  struct Impl;
  const std::unique_ptr<Impl> mImpl;
};

} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_SHADER_MANAGER_H_
