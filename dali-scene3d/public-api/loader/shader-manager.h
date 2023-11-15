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
   * @return ShaderOption for the materialDefinition and meshDefinition.
   */
  ShaderOption ProduceShaderOption(const MaterialDefinition& materialDefinition, const MeshDefinition& meshDefinition);

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

  /**
   * @brief Adds new lights for each of shaders.
   * @param[in] light Light object to be newly added.
   * @return True when the new light object is added successfully.
   */
  bool AddLight(Scene3D::Light light);

  /**
   * @brief Removes light from each of shaders.
   * @param[in] light Light object to be removed.
   */
  void RemoveLight(Scene3D::Light light);

  /**
   * @brief Retrieves added light counts.
   * @return The number of added light count.
   */
  uint32_t GetLightCount() const;

  /**
   * @brief Set a shadow to this scene by input light.
   *
   * @param[in] light Light object to make shadow.
   */
  void SetShadow(Scene3D::Light light);

  /**
   * @brief Removes Shadow from this SceneView.
   */
  void RemoveShadow();

  /**
   * @brief Update uniform properties of shadow for the input light.
   * @param[in] light Light object to update shadow uniform.
   */
  void UpdateShadowUniform(Scene3D::Light light);

private:
  /**
   * @brief Sets constraint to the shaders with light of light index.
   * @param[in] lightIndex index of light that will be connected with shaders by constraint.
   */
  DALI_INTERNAL void SetLightConstraint(uint32_t lightIndex);

  /**
   * @brief Sets constraint to a shader with light of light index.
   * @param[in] lightIndex index of light that will be connected with input shader by constraint.
   * @param[in] shader Shader that the constraint will be applied.
   */
  DALI_INTERNAL void SetLightConstraintToShader(uint32_t lightIndex, Dali::Shader shader);

  /**
   * @brief Removes constraint of shaders and light of light index.
   * @param[in] lightIndex index of light that will be disconnected with shaders.
   */
  DALI_INTERNAL void RemoveLightConstraint(uint32_t lightIndex);

  /**
   * @brief Sets uniform about the shadow.
   * @param[in] shader Shader that the constraint will be applied.
   */
  DALI_INTERNAL void SetShadowUniformToShader(Dali::Shader shader);

  /**
   * @brief Sets properties and constraint to the shaders.
   */
  DALI_INTERNAL void SetShadowProperty();

  /**
   * @brief Sets constraint to a shader about shadow
   * @param[in] shader Shader that the constraint will be applied.
   */
  DALI_INTERNAL void SetShadowConstraintToShader(Dali::Shader shader);

private:
  struct Impl;
  const std::unique_ptr<Impl> mImpl;
};

} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_SHADER_MANAGER_H_
