#ifndef DALI_SCENE3D_LOADER_SHADER_MANAGER_H_
#define DALI_SCENE3D_LOADER_SHADER_MANAGER_H_
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

// EXTERNAL INCLUDER
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/rendering/shader.h>
#include <dali/public-api/rendering/uniform-block.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/light/light.h>
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
 *
 * This class could be used as factory class to create Dali::Shader.
 * And once created Dali::Shader is kept in this manager and will be returned when the same Dali::Shader is requested to be created.
 * @SINCE_2_2.34
 */
class DALI_SCENE3D_API ShaderManager : public RefObject
{
public:
  ShaderManager();
  ~ShaderManager();

  /**
   * @brief Produces a Dali::Shader for the input materialDefinition and meshDefinition.
   *
   * Returns a cached Dali::Shader if the requested Dali::Shader has already been created once.
   * (Although the input materialDefinition and meshDefinition are not identical to those used to create the cached Dali::Shader, they share the cached one.)
   * @SINCE_2_2.34
   * @param[in] materialDefinition MaterialDefinition that includes information of material to create Shader.
   * @param[in] meshDefinition meshDefinition that includes information of mesh to create Shader.
   * @return ShaderOption for the materialDefinition and meshDefinition.
   */
  ShaderOption ProduceShaderOption(const MaterialDefinition& materialDefinition, const MeshDefinition& meshDefinition);

  /**
   * @brief Produces a Dali::Shader for the input ShaderOption.
   *
   * Returns a cached Dali::Shader if the requested Dali::Shader has already been created once.
   * @SINCE_2_2.34
   * @param[in] shaderOption shader option to create Shader.
   * @return Dali::Shader of the shader option
   */
  Dali::Shader ProduceShader(const ShaderOption& shaderOption);

  /**
   * @brief Returns RendererState of the input materialDefinition.
   * @SINCE_2_2.34
   * @param[in] materialDefinition MaterialDefinition to get RendererState
   * @return RendererState of the materialDefinition.
   */
  RendererState::Type GetRendererState(const MaterialDefinition& materialDefinition);

  /**
   * @brief Adds new lights for each of shaders.
   * @SINCE_2_2.34
   * @param[in] light Light object to be newly added.
   * @return True when the new light object is added successfully.
   */
  bool AddLight(Scene3D::Light light);

  /**
   * @brief Removes light from each of shaders.
   * @SINCE_2_2.34
   * @param[in] light Light object to be removed.
   */
  void RemoveLight(Scene3D::Light light);

  /**
   * @brief Retrieves added light counts.
   * @SINCE_2_2.34
   * @return The number of added light count.
   */
  uint32_t GetLightCount() const;

  /**
   * @brief Set a shadow to this scene by input light.
   * @SINCE_2_2.34
   * @param[in] light Light object to make shadow.
   */
  void SetShadow(Scene3D::Light light);

  /**
   * @brief Removes Shadow from this SceneView.
   * @SINCE_2_2.34
   */
  void RemoveShadow();

  /**
   * @brief Update uniform properties of shadow for the input light.
   * @SINCE_2_2.34
   * @param[in] light Light object to update shadow uniform.
   */
  void UpdateShadowUniform(Scene3D::Light light);

private:
  /// @cond internal
  /**
   * @brief Sets constraint to the uniform block with light of light index.
   * @param[in] lightIndex index of light that will be connected with uniform block by constraint.
   */
  DALI_INTERNAL void SetLightConstraint(uint32_t lightIndex);

  /**
   * @brief Removes constraint of uniform block and light of light index.
   * @param[in] lightIndex index of light that will be disconnected with uniform block.
   */
  DALI_INTERNAL void RemoveLightConstraint(uint32_t lightIndex);

  /**
   * @brief Sets properties and constraint to the shaders.
   */
  DALI_INTERNAL void SetShadowProperty();

  /**
   * @brief Sets uniform about the shadow.
   * @param[in] uniformBlock UniformBlock that the uniform values will be applied.
   */
  DALI_INTERNAL void SetShadowUniformToUniformBlock();

  /**
   * @brief Sets constraint to a uniform block about shadow
   * @param[in] uniformBlock UniformBlock that the constraint will be applied.
   */
  DALI_INTERNAL void SetShadowConstraintToUniformBlock();

  /// @endcond
private:
  struct Impl;
  const std::unique_ptr<Impl> mImpl;
};

} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_SHADER_MANAGER_H_
