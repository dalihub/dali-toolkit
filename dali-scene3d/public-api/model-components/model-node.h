#ifndef DALI_SCENE3D_MODEL_COMPONENTS_MODEL_NODE_H
#define DALI_SCENE3D_MODEL_COMPONENTS_MODEL_NODE_H

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
#include <dali/public-api/actors/custom-actor.h>
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/loader/blend-shape-details.h> ///< For Loader::BlendShapes::Index
#include <dali-scene3d/public-api/model-components/model-primitive.h>

namespace Dali
{
namespace Scene3D
{
// Forward declarations.

namespace Internal
{
class ModelNode;
}
/**
 * @addtogroup dali_scene3d_model_components_model_node
 * @{
 */

/**
 * @brief ModelNode is a class for representing the Node of Model in Scene3D.
 * ModelNode contains multiple ModelPrimitives and allows easy access and modification of Material information that ModelPrimitive has.
 * If a 3D format file is loaded by Model, ModelNode is created internally to construct the model.
 * In addition, you can create a Custom ModelNode using ModelPrimitive and Material directly and add it to Model.
 *
 * @SINCE_2_2.99
 *
 * @code
 * ModelNode modelNode = ModelNode::New();
 * ModelPrimitive modelPrimitive = ModelPrimitive::New();
 * modelNode.AddModelPrimitive(modelPrimitive);
 *
 * Material material = Material::New();
 * modelPrimitive.SetMaterial(material);
 * material.SetProperty(PropertyIndex, PropertyValue);
 * @endcode
 */
class DALI_SCENE3D_API ModelNode : public Dali::CustomActor
{
public:
  /**
   * @brief Create an initialized ModelNode.
   *
   * @SINCE_2_2.99
   * @return A handle to a newly allocated Dali resource
   */
  static ModelNode New();

  /**
   * @brief Creates an uninitialized ModelNode.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   *
   * @SINCE_2_2.99
   */
  ModelNode();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   *
   * @SINCE_2_2.99
   */
  ~ModelNode();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_2_2.99
   * @param[in] modelNode Handle to an object
   */
  ModelNode(const ModelNode& modelNode);

  /**
   * @brief Move constructor
   *
   * @SINCE_2_2.99
   * @param[in] rhs A reference to the moved handle
   */
  ModelNode(ModelNode&& rhs) noexcept;

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_2.99
   * @param[in] modelNode Handle to an object
   * @return reference to this
   */
  ModelNode& operator=(const ModelNode& modelNode);

  /**
   * @brief Move assignment
   *
   * @SINCE_2_2.99
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  ModelNode& operator=(ModelNode&& rhs) noexcept;

  /**
   * @brief Downcasts an Object handle to ModelNode.
   *
   * If handle points to a ModelNode, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_2_2.99
   * @param[in] handle Handle to an object
   * @return Handle to a ModelNode or an uninitialized handle
   */
  static ModelNode DownCast(BaseHandle handle);

public: // Public Method
  /**
   * @brief Gets the number of ModelPrimitives this node has.
   *
   * @SINCE_2_2.99
   * @return The number of ModelPrimitives this node has.
   */
  uint32_t GetModelPrimitiveCount() const;

  /**
   * @brief Appends a ModelPrimitive to this node.
   *
   * @SINCE_2_2.99
   * @param[in] modelPrimitive The ModelPrimitive to append.
   */
  void AddModelPrimitive(ModelPrimitive modelPrimitive);

  /**
   * @brief Removes a ModelPrimitive from this node.
   *
   * @SINCE_2_2.99
   * @param[in] modelPrimitive The ModelPrimitive to remove.
   */
  void RemoveModelPrimitive(ModelPrimitive modelPrimitive);

  /**
   * @brief Removes a ModelPrimitive from this node by index.
   *
   * @SINCE_2_2.99
   * @param[in] index The index of the ModelPrimitive to remove.
   */
  void RemoveModelPrimitive(uint32_t index);

  /**
   * @brief Gets a ModelPrimitive by index.
   *
   * @SINCE_2_2.99
   * @param[in] index The index of the ModelPrimitive to get.
   * @return The ModelPrimitive at the given index, or an empty handle if the index is out of range.
   */
  ModelPrimitive GetModelPrimitive(uint32_t index) const;

  /**
   * @brief Returns a child ModelNode object with a name that matches nodeName.
   *
   * @SINCE_2_2.99
   * @param[in] nodeName The name of the child ModelNode object you want to find.
   * @return Returns a child ModelNode object with a name that matches nodeName. If there is no corresponding child ModelNode object, it returns an empty ModelNode object.
   */
  ModelNode FindChildModelNodeByName(std::string_view nodeName);

  /**
   * @brief Retrieve the list of blendshape name that current ModelNode hold.
   * The name will be appended end of input list.
   *
   * @SINCE_2_2.99
   * @param[in, out] blendShapeNames The name of blendShape list collected.
   */
  void RetrieveBlendShapeNames(std::vector<std::string>& blendShapeNames) const;

  /**
   * @brief Get the index of blend shape by given name.
   *
   * @SINCE_2_2.99
   * @param[in] blendShapeName The name of blendshape that is not empty.
   * @return Index of blendshape, or return invalid if there is no blendshape with given name.
   */
  Loader::BlendShapes::Index GetBlendShapeIndexByName(std::string_view blendShapeName) const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Scene3D::Internal implementation.
   *
   * @param[in] implementation The ModelNodel implementation
   */
  DALI_INTERNAL ModelNode(Internal::ModelNode& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  DALI_INTERNAL ModelNode(Dali::Internal::CustomActor* internal);
  /// @endcond
};

/**
 * @}
 */
} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_COMPONENTS_MODEL_NODE_H
