#ifndef DALI_SCENE3D_MODEL_MOTION_MOTION_PROPERTY_INDEX_H
#define DALI_SCENE3D_MODEL_MOTION_MOTION_PROPERTY_INDEX_H

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

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/model-motion/motion-index/motion-index.h>

namespace Dali
{
namespace Scene3D
{
//Forward declarations.
namespace Internal
{
class MotionPropertyIndex;
} // namespace Internal

/**
 * @addtogroup dali_scene3d_model_motion_motion_property_index
 * @{
 */

/**
 * @brief Basic MotionIndex to control Dali::Property.
 * It can control more general case.
 *
 * @code
 *
 * MotionPropertyIndex color = MotionPropertyIndex::New("nodeName", Dali::Actor::Property::COLOR);
 *
 * // We can change the property later.
 * MotionPropertyIndex custom = MotionPropertyIndex::New();
 * orientation.SetModelNodeId("nodeName");
 * orientation.SetPropertyId("some_custom_property");
 *
 * // Note that all cases of MotionTransformIndex can be controled by MotionPropertyIndex
 * // Both position0 and position1 can control the node's Position.
 * MotionTransformIndex position0 = MotionTransformIndex::New("nodeName", MotionTransformIndex::TransformType::POSITION);
 * MotionPropertyIndex  position1 = MotionPropertyIndex::New("nodeName", Dali::Actor::Property::POSITION);
 *
 * @endcode
 *
 * @SINCE_2_2.34
 */
class DALI_SCENE3D_API MotionPropertyIndex : public MotionIndex
{
public: // Creation & Destruction
  /**
   * @brief Create an initialized MotionPropertyIndex.
   *
   * @SINCE_2_2.34
   * @return A handle to a newly allocated Dali resource
   */
  static MotionPropertyIndex New();

  /**
   * @brief Create an initialized MotionPropertyIndex with values.
   *
   * @SINCE_2_2.34
   * @param[in] modelNodeId The id of model node what this motion index looks.
   * @param[in] propertyId The id of property what this motion index looks.
   * @return A handle to a newly allocated Dali resource
   */
  static MotionPropertyIndex New(Property::Key modelNodeId, Property::Key propertyId);

  /**
   * @brief Creates an uninitialized MotionPropertyIndex.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   *
   * @SINCE_2_2.34
   */
  MotionPropertyIndex();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   *
   * @SINCE_2_2.34
   */
  ~MotionPropertyIndex();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_2_2.34
   * @param[in] motionPropertyIndex Handle to an object
   */
  MotionPropertyIndex(const MotionPropertyIndex& motionPropertyIndex);

  /**
   * @brief Move constructor
   *
   * @SINCE_2_2.34
   * @param[in] rhs A reference to the moved handle
   */
  MotionPropertyIndex(MotionPropertyIndex&& rhs) noexcept;

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_2.34
   * @param[in] motionPropertyIndex Handle to an object
   * @return reference to this
   */
  MotionPropertyIndex& operator=(const MotionPropertyIndex& motionPropertyIndex);

  /**
   * @brief Move assignment
   *
   * @SINCE_2_2.34
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  MotionPropertyIndex& operator=(MotionPropertyIndex&& rhs) noexcept;

  /**
   * @brief Downcasts an Object handle to MotionPropertyIndex.
   *
   * If handle points to a MotionPropertyIndex, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_2_2.34
   * @param[in] handle Handle to an object
   * @return Handle to a MotionPropertyIndex or an uninitialized handle
   */
  static MotionPropertyIndex DownCast(BaseHandle handle);

public: // Public Method
  /**
   * @brief Set the id of property what this motion index looks.
   *
   * @SINCE_2_2.34
   * @param[in] propertyId The property id.
   */
  void SetPropertyId(Property::Key propertyId);

  /**
   * @brief Get the id of property what this motion index looks.
   *
   * @SINCE_2_2.34
   * @return The blend shape id.
   */
  Property::Key GetPropertyId() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Scene3D::Internal implementation.
   *
   * @param[in] implementation The MotionPropertyIndex implementation
   */
  DALI_INTERNAL MotionPropertyIndex(Dali::Scene3D::Internal::MotionPropertyIndex* implementation);
  /// @endcond
};

/**
 * @}
 */

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_MOTION_MOTION_PROPERTY_INDEX_H