#ifndef DALI_SCENE3D_MODEL_COMPONENTS_MODEL_PRIMITIVE_IMPL_H
#define DALI_SCENE3D_MODEL_COMPONENTS_MODEL_PRIMITIVE_IMPL_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/property.h>
#include <set>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-components/material-modify-observer.h>
#include <dali-scene3d/internal/model-components/model-primitive-modify-observer.h>
#include <dali-scene3d/public-api/light/light.h>
#include <dali-scene3d/public-api/loader/blend-shape-details.h>
#include <dali-scene3d/public-api/loader/mesh-definition.h>
#include <dali-scene3d/public-api/loader/shader-manager.h>
#include <dali-scene3d/public-api/model-components/material.h>
#include <dali-scene3d/public-api/model-components/model-primitive.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
using ModelPrimitivePtr = IntrusivePtr<ModelPrimitive>;

/**
 * @brief TODO : Explain me.
 * Use Vector4 Tangent data
 * Same ModelPrimitive all shares IBL
 */
class ModelPrimitive : public BaseObject, public MaterialModifyObserver
{
private:
  using ModelPrimitiveModifyObserverContainer = std::set<ModelPrimitiveModifyObserver*>;

public:
  // Creation & Destruction
  /**
   * @brief Create a new ModelPrimitive object.
   * @return A smart-pointer to the newly allocated ModelPrimitive.
   */
  static ModelPrimitivePtr New();

protected:
  /**
   * @brief Construct a new ModelPrimitive.
   */
  ModelPrimitive();

  /**
   * @brief Second-phase constructor.
   */
  void Initialize();

  /**
   * @brief Virtual destructor.
   */
  virtual ~ModelPrimitive();

public:
  /**
   * @brief Set Renderer that is created by Scene3D::Loader internally.
   */
  void SetRenderer(Dali::Renderer renderer);

  /**
   * @copydoc Dali::Scene3D::ModelPrimitive::GetRenderer()
   */
  Dali::Renderer GetRenderer() const;

  /**
   * @copydoc Dali::Scene3D::ModelPrimitive::SetGeometry()
   */
  void SetGeometry(Dali::Geometry geometry);

  /**
   * @copydoc Dali::Scene3D::ModelPrimitive::GetGeometry()
   */
  Dali::Geometry GetGeometry() const;

  /**
   * @copydoc Dali::Scene3D::ModelPrimitive::SetMaterial()
   */
  void SetMaterial(Dali::Scene3D::Material material, bool updateRenderer = true);

  /**
   * @copydoc Dali::Scene3D::ModelPrimitive::GetMaterial()
   */
  Dali::Scene3D::Material GetMaterial() const;

  /**
   * @brief Adds a primitive observer to this model primitive.
   *
   * @param[in] observer The observer to add.
   */
  void AddPrimitiveObserver(ModelPrimitiveModifyObserver* observer);

  /**
   * @brief Removes a primitive observer from this model primitive.
   *
   * @param[in] observer The observer to remove.
   */
  void RemovePrimitiveObserver(ModelPrimitiveModifyObserver* observer);

  /**
   * @brief Sets shadow map texture for this model primitive.
   *
   * @param[in] shadowMapTexture The shadow map texture.
   */
  void SetShadowMapTexture(Dali::Texture shadowMapTexture);

  /**
   * @brief Sets the image-based lighting texture for this model primitive.
   *
   * @param[in] diffuseTexture The diffuse texture.
   * @param[in] specularTexture The specular texture.
   * @param[in] iblScaleFactor The scale factor to set for image-based lighting.
   * @param[in] specularMipmapLevels The number of mipmap levels of specular texture.
   */
  void SetImageBasedLightTexture(Dali::Texture diffuseTexture, Dali::Texture specularTexture, float iblScaleFactor, uint32_t specularMipmapLevels);

  /**
   * @brief Sets the scale factor for image-based lighting for this model primitive.
   *
   * @param[in] iblScaleFactor The scale factor to set for image-based lighting.
   */
  void SetImageBasedLightScaleFactor(float iblScaleFactor);

  /**
   * @brief Updates shaders by using current material
   *
   * @param[in] shaderManager Shader manager to create shader.
   */
  void UpdateShader(Scene3D::Loader::ShaderManagerPtr shaderManager);

  /**
   * @brief Sets the blend shape data for this model primitive.
   *
   * @param[in] data The blend shape data to set.
   */
  void SetBlendShapeData(Scene3D::Loader::BlendShapes::BlendShapeData& data);

  /**
   * @brief Sets the blend shape geometry for this model primitive.
   *
   * @param[in] blendShapeGeometry The blend shape geometry to set.
   */
  void SetBlendShapeGeometry(Dali::Texture blendShapeGeometry);

  /**
   * @brief Sets the blend shape options for this model primitive.
   *
   * @param[in] hasPositions Whether or not this model primitive has positions for blend shapes.
   * @param[in] hasNormals Whether or not this model primitive has normals for blend shapes.
   * @param[in] hasTangents Whether or not this model primitive has tangents for blend shapes.
   * @param[in] version blendShape version.
   */
  void SetBlendShapeOptions(bool hasPositions, bool hasNormals, bool hasTangents, Scene3D::Loader::BlendShapes::Version version);

  /**
   * @brief Sets whether or not this model primitive is skinned.
   *
   * @param[in] isSkinned Whether or not this model primitive is skinned.
   */
  void SetSkinned(bool isSkinned);

private: // From MaterialModifyObserver
  /**
   * @copydoc Dali::Scene3D::Internal::Material::MaterialModifyObserver::OnMaterialModified()
   */
  void OnMaterialModified(Dali::Scene3D::Material material, MaterialModifyObserver::ModifyFlag flag) override;

private:
  /**
   * @brief Apply materials data into renderer.
   */
  void ApplyMaterialToRenderer(MaterialModifyObserver::ModifyFlag flag = MaterialModifyObserver::ModifyFlag::NONE);

  /**
   * @brief Updates the uniform of renderer.
   */
  void UpdateRendererUniform();

  /**
   * @brief Creates a renderer.
   */
  void CreateRenderer();

  void UpdateShadowMapTexture();

  /**
   * @brief Updates the image-based lighting texture.
   */
  void UpdateImageBasedLightTexture();

private:
  // Delete copy & move operator
  ModelPrimitive(const ModelPrimitive&)                    = delete;
  ModelPrimitive(ModelPrimitive&&)                         = delete;
  ModelPrimitive& operator=(const ModelPrimitive& rhs)     = delete;
  ModelPrimitive& operator=(ModelPrimitive&& rhs) noexcept = delete;

private:
  ModelPrimitiveModifyObserverContainer mObservers{};

  // For Renderer
  Dali::Renderer          mRenderer;
  Dali::Geometry          mGeometry;
  Dali::Shader            mShader;
  Dali::TextureSet        mTextureSet;
  Dali::Scene3D::Material mMaterial;

  Scene3D::Loader::ShaderManagerPtr mShaderManager;

  // For Shadow
  Dali::Texture mShadowMapTexture;

  // For IBL
  Dali::Texture mSpecularTexture;
  Dali::Texture mDiffuseTexture;
  float         mIblScaleFactor{1.0f};
  uint32_t      mSpecularMipmapLevels{1u};

  // For blend shape
  Scene3D::Loader::BlendShapes::BlendShapeData mBlendShapeData;
  Dali::Texture                                mBlendShapeGeometry;
  bool                                         mHasSkinning       = false;
  bool                                         mHasPositions      = false;
  bool                                         mHasNormals        = false;
  bool                                         mHasTangents       = false;
  Scene3D::Loader::BlendShapes::Version        mBlendShapeVersion = Scene3D::Loader::BlendShapes::Version::INVALID;

  bool mIsMaterialChanged        = false;
  bool mNeedToSetRendererUniform = false;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ModelPrimitive& GetImplementation(Dali::Scene3D::ModelPrimitive& modelPrimitive)
{
  DALI_ASSERT_ALWAYS(modelPrimitive && "ModelPrimitive handle is empty");

  BaseObject& handle = modelPrimitive.GetBaseObject();

  return static_cast<Internal::ModelPrimitive&>(handle);
}

inline const Internal::ModelPrimitive& GetImplementation(const Dali::Scene3D::ModelPrimitive& modelPrimitive)
{
  DALI_ASSERT_ALWAYS(modelPrimitive && "ModelPrimitive handle is empty");

  const BaseObject& handle = modelPrimitive.GetBaseObject();

  return static_cast<const Internal::ModelPrimitive&>(handle);
}

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_COMPONENTS_MODEL_PRIMITIVE_IMPL_H
