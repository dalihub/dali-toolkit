#ifndef DALI_SCENE3D_MODEL_COMPONENTS_MODEL_NODE_IMPL_H
#define DALI_SCENE3D_MODEL_COMPONENTS_MODEL_NODE_IMPL_H

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
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/public-api/actors/custom-actor-impl.h>
#include <dali/public-api/common/dali-common.h>
#include <memory> // for std::unique_ptr
#include <string>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-components/model-primitive-modify-observer.h>
#include <dali-scene3d/public-api/light/light.h>
#include <dali-scene3d/public-api/loader/mesh-definition.h>
#include <dali-scene3d/public-api/loader/shader-manager.h>
#include <dali-scene3d/public-api/loader/skinning-details.h>
#include <dali-scene3d/public-api/model-components/model-node.h>
#include <dali-scene3d/public-api/model-components/model-primitive.h>

namespace Dali
{
namespace Scene3D
{
/**
 * @addtogroup dali_toolkit_controls_model
 * @{
 */

namespace Internal
{
/**
 * @brief This is the internal base class for custom node of model.
 *
 * @SINCE_2_2.99
 */
class DALI_SCENE3D_API ModelNode : public CustomActorImpl, public ModelPrimitiveModifyObserver
{
public:
  using ModelPrimitiveContainer = std::vector<Scene3D::ModelPrimitive>;
  using BoneDataContainer       = std::vector<Dali::Scene3D::Loader::Skinning::BoneData>;
  using BlendShapeIndexMap      = std::map<std::string, Loader::BlendShapes::Index>;

  // Creation & Destruction
  /**
   * @brief Creates a new ModelNodeImpl instance that does not require touch by default.
   *
   * If touch is required, then the user can connect to this class' touch signal.
   * @SINCE_2_2.99
   * @return A handle to the ModelNode instance
   */
  static Scene3D::ModelNode New();

protected:
  /**
   * @brief Virtual destructor.
   * @SINCE_2_2.99
   */
  virtual ~ModelNode();

protected: // From CustomActorImpl
  /**
   * @copydoc CustomActorImpl::OnSceneConnection()
   * @note If overridden, then an up-call to ModelNode::OnSceneConnection MUST be made at the end.
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc CustomActorImpl::OnSceneDisconnection()
   * @note If overridden, then an up-call to ModelNode::OnSceneDisconnection MUST be made at the end.
   */
  void OnSceneDisconnection() override;

  /**
   * @copydoc CustomActorImpl::OnChildAdd()
   * @note If overridden, then an up-call to ModelNode::OnChildAdd MUST be made at the end.
   */
  void OnChildAdd(Actor& child) override;

  /**
   * @copydoc CustomActorImpl::OnChildRemove()
   * @note If overridden, then an up-call to ModelNode::OnChildRemove MUST be made at the end.
   */
  void OnChildRemove(Actor& child) override;

  /**
   * @copydoc CustomActorImpl::OnPropertySet()
   * @note If overridden, then an up-call to ModelNode::OnChildRemove MUST be made at the end.
   */
  void OnPropertySet(Property::Index index, const Property::Value& propertyValue) override;

  /**
   * @copydoc CustomActorImpl::OnSizeSet()
   * @note If overridden, then an up-call to ModelNode::OnSizeSet MUST be made at the end.
   */
  void OnSizeSet(const Vector3& targetSize) override;

  /**
   * @copydoc CustomActorImpl::OnSizeAnimation()
   * @note If overridden, then an up-call to ModelNode::OnSizeAnimation MUST be made at the end.
   */
  void OnSizeAnimation(Animation& animation, const Vector3& targetSize) override;

  /**
   * @copydoc CustomActorImpl::OnRelayout()
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc CustomActorImpl::OnSetResizePolicy()
   */
  void OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension) override;

  /**
   * @copydoc CustomActorImpl::GetNaturalSize()
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc CustomActorImpl::CalculateChildSize()
   */
  float CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension) override;

  /**
   * @copydoc CustomActorImpl::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

  /**
   * @copydoc CustomActorImpl::GetWidthForHeight()
   */
  float GetWidthForHeight(float height) override;

  /**
   * @copydoc CustomActorImpl::RelayoutDependentOnChildren()
   */
  bool RelayoutDependentOnChildren(Dimension::Type dimension = Dimension::ALL_DIMENSIONS) override;

  /**
   * @copydoc CustomActorImpl::OnCalculateRelayoutSize()
   */
  void OnCalculateRelayoutSize(Dimension::Type dimension) override;

  /**
   * @copydoc CustomActorImpl::OnLayoutNegotiated()
   */
  void OnLayoutNegotiated(float size, Dimension::Type dimension) override;

protected:
  // Construction

  /**
   * @brief ModelNode constructor.
   *
   * @SINCE_2_2.99
   */
  ModelNode();

  /**
   * @brief Second phase initialization.
   * @SINCE_2_2.99
   */
  void Initialize();

public: // API for derived classes to override
  // Lifecycle

  /**
   * @brief This method is called after the Node has been initialized.
   *
   * Derived classes should do any second phase initialization by overriding this method.
   * @SINCE_2_2.99
   */
  virtual void OnInitialize();

public: // Public Method
  /**
   * @copydoc Dali::Scene3D::ModelNode::GetModelPrimitiveCount()
   */
  uint32_t GetModelPrimitiveCount() const;

  /**
   * @copydoc Dali::Scene3D::ModelNode::AddModelPrimitive()
   */
  void AddModelPrimitive(Dali::Scene3D::ModelPrimitive modelPrimitive);

  /**
   * @copydoc Dali::Scene3D::ModelNode::RemoveModelPrimitive(Dali::Scene3D::ModelPrimitive modelPrimitive)
   */
  void RemoveModelPrimitive(Dali::Scene3D::ModelPrimitive modelPrimitive);

  /**
   * @copydoc Dali::Scene3D::ModelNode::RemoveModelPrimitive(uint32_t index)
   */
  void RemoveModelPrimitive(uint32_t index);

  /**
   * @copydoc Dali::Scene3D::ModelNode::GetModelPrimitive()
   */
  Dali::Scene3D::ModelPrimitive GetModelPrimitive(uint32_t index) const;

  /**
   * @copydoc Dali::Scene3D::ModelNode::FindChildModelNodeByName()
   */
  Scene3D::ModelNode FindChildModelNodeByName(std::string_view nodeName);

  /**
   * @copydoc Dali::Scene3D::ModelNode::RetrieveBlendShapeNames()
   */
  void RetrieveBlendShapeNames(std::vector<std::string>& blendShapeNames) const;

  /**
   * @copydoc Dali::Scene3D::ModelNode::GetBlendShapeIndexByName()
   */
  Loader::BlendShapes::Index GetBlendShapeIndexByName(std::string_view blendShapeName) const;

  /**
   * @brief Sets the diffuse and specular image-based lighting textures for a ModelPrimitive.
   *
   * @param[in] diffuseTexture The diffuse texture.
   * @param[in] specularTexture The specular texture.
   * @param[in] iblScaleFactor The scale factor for the image-based lighting.
   * @param[in] specularMipmapLevels The number of mipmap levels for the specular texture.
   */
  void SetImageBasedLightTexture(Dali::Texture diffuseTexture, Dali::Texture specularTexture, float iblScaleFactor, uint32_t specularMipmapLevels);

  /**
   * @brief Sets the scale factor for image-based lighting.
   *
   * @param[in] iblScaleFactor The scale factor for image-based lighting.
   */
  void SetImageBasedLightScaleFactor(float iblScaleFactor);

  /**
   * @brief Updates shaders by using current material
   *
   * @param[in] shaderManager Shader manager to create shader.
   */
  void UpdateShader(Scene3D::Loader::ShaderManagerPtr shaderManager);

  /**
   * @brief Sets the blend shape data for a ModelPrimitive.
   *
   * @param[in] data The blend shape data.
   * @param[in] primitive The ModelPrimitive to set the blend shape data for.
   */
  void SetBlendShapeData(Scene3D::Loader::BlendShapes::BlendShapeData& data, Scene3D::ModelPrimitive primitive);

  /**
   * @brief Sets the bone matrix for a ModelPrimitive and bone index.
   *
   * @param[in] inverseMatrix The inverse matrix of the bone.
   * @param[in] primitive The ModelPrimitive to set the bone matrix for.
   * @param[in] boneIndex The index of the bone to set the matrix for.
   */
  void SetBoneMatrix(const Matrix& inverseMatrix, Scene3D::ModelPrimitive primitive, Scene3D::Loader::Index& boneIndex);

  /**
   * @brief Called when a Renderer of ModelPrimitive is created.
   *
   * @param[in] renderer The Renderer that is created.
   */
  void OnRendererCreated(Renderer renderer) override;

private:
  /**
   * @brief Updates the bone matrix for a ModelPrimitive.
   *
   * @param[in] primitive The ModelPrimitive to set the bone matrix for.
   */
  void UpdateBoneMatrix(Scene3D::ModelPrimitive primitive);

private:
  /// @cond internal

  // Not copyable or movable
  DALI_INTERNAL ModelNode(const ModelNode&) = delete;            ///< Deleted copy constructor.
  DALI_INTERNAL ModelNode(ModelNode&&)      = delete;            ///< Deleted move constructor.
  DALI_INTERNAL ModelNode& operator=(const ModelNode&) = delete; ///< Deleted copy assignment operator.
  DALI_INTERNAL ModelNode& operator=(ModelNode&&) = delete;      ///< Deleted move assignment operator.

private:
  Scene3D::Loader::ShaderManagerPtr mShaderManager;
  ModelPrimitiveContainer           mModelPrimitiveContainer; ///< List of model primitives
  BoneDataContainer                 mBoneDataContainer;
  BlendShapeIndexMap                mBlendShapeIndexMap;      ///< Index of blend shape by name
  Dali::Texture                     mSpecularTexture;
  Dali::Texture                     mDiffuseTexture;
  float                             mIblScaleFactor{1.0f};
  uint32_t                          mSpecularMipmapLevels{1u};
  /// @endcond
};

/**
 * @brief Gets implementation from the handle.
 *
 * @SINCE_2_2.99
 * @param handle
 * @return Implementation
 * @pre handle is initialized and points to a node
 */
DALI_SCENE3D_API Internal::ModelNode& GetImplementation(Dali::Scene3D::ModelNode& handle);

/**
 * @brief Gets implementation from the handle.
 *
 * @SINCE_2_2.99
 * @param handle
 * @return Implementation
 * @pre Handle is initialized and points to a node.
 */
DALI_SCENE3D_API const Internal::ModelNode& GetImplementation(const Dali::Scene3D::ModelNode& handle);

} // namespace Internal

/**
 * @}
 */
} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_COMPONENTS_MODEL_NODE_IMPL_H
