#ifndef DALI_SCENE3D_MODEL_MOTION_MOTION_INDEX_H
#define DALI_SCENE3D_MODEL_MOTION_MOTION_INDEX_H

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
#include <dali/public-api/object/property-key.h>
#include <dali/public-api/object/property.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/model-components/model-node.h>

namespace Dali
{
namespace Scene3D
{
//Forward declarations.
namespace Internal
{
class MotionIndex;
} // namespace Internal

/**
 * @addtogroup dali_scene3d_model_motion_motion_index
 * @{
 */

/**
 * @brief Key of motion data to specify what ModelNode's property will be moved.
 * MotionIndex itself is abstract classes. We need to create New from one of below classes.
 * - BlendShapeIndex : To control blendshape.
 * - MotionPropertyIndex : To control the property by Dali::Property.
 * - MotionTransfromIndex : To control the transform property. Usually be used on CSharp binded API who cannot use Dali::Property directly.
 * @todo Need to implement ModelNodeId as IndexKey. Currently, StringKey only works well.
 *
 * @SINCE_2_2.99
 */
class DALI_SCENE3D_API MotionIndex : public Dali::BaseHandle
{
public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized MotionIndex.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   *
   * @SINCE_2_2.99
   */
  MotionIndex();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   *
   * @SINCE_2_2.99
   */
  ~MotionIndex();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_2_2.99
   * @param[in] motionIndex Handle to an object
   */
  MotionIndex(const MotionIndex& motionIndex);

  /**
   * @brief Move constructor
   *
   * @SINCE_2_2.99
   * @param[in] rhs A reference to the moved handle
   */
  MotionIndex(MotionIndex&& rhs) noexcept;

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_2.99
   * @param[in] motionIndex Handle to an object
   * @return reference to this
   */
  MotionIndex& operator=(const MotionIndex& motionIndex);

  /**
   * @brief Move assignment
   *
   * @SINCE_2_2.99
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  MotionIndex& operator=(MotionIndex&& rhs) noexcept;

  /**
   * @brief Downcasts an Object handle to MotionIndex.
   *
   * If handle points to a MotionIndex, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_2_2.99
   * @param[in] handle Handle to an object
   * @return Handle to a MotionIndex or an uninitialized handle
   */
  static MotionIndex DownCast(BaseHandle handle);

public: // Public Method
  /**
   * @brief Set the id of ModelNode what this motion index looks.
   *
   * @SINCE_2_2.99
   * @param[in] modelNodeId The model node id.
   */
  void SetModelNodeId(Property::Key modelNodeId);

  /**
   * @brief Get the id of ModelNode what this motion index looks.
   *
   * @SINCE_2_2.99
   * @return The model node id.
   */
  Property::Key GetModelNodeId() const;

  /**
   * @brief Get the property name of this MotionIndex from given ModelNode.
   *
   * @SINCE_2_2.99
   * @param[in] node The model node to get the property name.
   * @return The name of current property.
   */
  std::string GetPropertyName(ModelNode node = ModelNode());

  /**
   * @brief Get the property index of this MotionIndex from given ModelNode.
   *
   * @SINCE_2_2.99
   * @param[in] node The model node to get the property index.
   * @return The index of current property.
   */
  Property::Index GetPropertyIndex(ModelNode node = ModelNode());

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Scene3D::Internal implementation.
   *
   * @param[in] implementation The MotionIndex implementation
   */
  DALI_INTERNAL MotionIndex(Dali::Scene3D::Internal::MotionIndex* implementation);
  /// @endcond
};

/**
 * @}
 */

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_MOTION_MOTION_INDEX_H