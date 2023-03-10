#ifndef DALI_SCENE3D_MODEL_COMPONENTS_MODEL_NODE_DATA_IMPL_H
#define DALI_SCENE3D_MODEL_COMPONENTS_MODEL_NODE_DATA_IMPL_H

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
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-components/model-node-impl.h>
#include <dali-scene3d/internal/model-components/model-primitive-modify-observer.h>
#include <dali-scene3d/public-api/loader/skinning-details.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
/**
 * @brief Holds the Implementation for the internal model node class
 */
class ModelNode::Impl : public ModelPrimitiveModifyObserver
{
public:
  using ModelPrimitiveContainer = std::vector<Scene3D::ModelPrimitive>;
  using BoneDataContainer       = std::vector<Dali::Scene3D::Loader::Skinning::BoneData>;

  /**
   * @brief Constructor.
   * @param[in] modelNodeImpl The control which owns this implementation
   */
  Impl(ModelNode& modelNodeImpl);

  /**
   * @brief Destructor.
   */
  ~Impl();

public:
  /**
   * @copydoc Dali::Scene3D::Internal::ModelNode::OnSceneConnection()
   */
  void OnSceneConnection(int depth);

  /**
   * @copydoc Dali::Scene3D::Internal::ModelNode::OnSceneConnection()
   */
  void OnSceneDisconnection();

public: // Public Method
  /**
   * @copydoc Dali::Scene3D::ModelNode::GetModelPrimitiveCount()
   */
  inline uint32_t GetModelPrimitiveCount() const
  {
    return static_cast<uint32_t>(mModelPrimitiveContainer.size());
  }

  /**
   * @copydoc Dali::Scene3D::ModelNode::AddModelPrimitive()
   */
  void AddModelPrimitive(Scene3D::ModelPrimitive modelPrimitive);

  /**
   * @copydoc Dali::Scene3D::ModelNode::RemoveModelPrimitive()
   */
  void RemoveModelPrimitive(Scene3D::ModelPrimitive modelPrimitive);

  /**
   * @copydoc Dali::Scene3D::ModelNode::RemoveModelPrimitive()
   */
  void RemoveModelPrimitive(uint32_t index);

  /**
   * @copydoc Dali::Scene3D::ModelNode::GetModelPrimitive()
   */
  Scene3D::ModelPrimitive GetModelPrimitive(uint32_t index) const;

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
  ModelNode&              mModelNodeImpl;           ///< Owner of this data
  ModelPrimitiveContainer mModelPrimitiveContainer; ///< List of model primitives
  BoneDataContainer       mBoneDataContainer;
  Dali::Texture           mSpecularTexture;
  Dali::Texture           mDiffuseTexture;
  float                   mIblScaleFactor{1.0f};
  uint32_t                mSpecularMipmapLevels{1u};
};

} // namespace Internal

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_COMPONENTS_MODEL_NODE_DATA_IMPL_H
