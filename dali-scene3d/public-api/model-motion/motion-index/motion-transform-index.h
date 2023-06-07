#ifndef DALI_SCENE3D_MODEL_MOTION_MOTION_TRANSFORM_INDEX_H
#define DALI_SCENE3D_MODEL_MOTION_MOTION_TRANSFORM_INDEX_H

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
class MotionTransformIndex;
} // namespace Internal

/**
 * @addtogroup dali_scene3d_model_motion_motion_transform_index
 * @{
 */

/**
 * @brief Specialized MotionIndex to control transform.
 *
 * @SINCE_2_2.99
 */
class DALI_SCENE3D_API MotionTransformIndex : public MotionIndex
{
public:
  /**
   * @brief The type of Transform
   * @SINCE_2_2.99
   */
  enum class TransformType
  {
    INVALID = -1,

    POSITION = 0, ///< The position of ModelNode. MotionValue should be Vector3.
    POSITION_X,   ///< The x position of ModelNode. MotionValue should be float.
    POSITION_Y,   ///< The y position of ModelNode. MotionValue should be float.
    POSITION_Z,   ///< The z position of ModelNode. MotionValue should be float.

    ORIENTATION, ///< The orientation of ModelNode. MotionValue should be Quaternion.

    SCALE,   ///< The scale of ModelNode. MotionValue should be Vector3.
    SCALE_X, ///< The x scale of ModelNode. MotionValue should be float.
    SCALE_Y, ///< The y scale of ModelNode. MotionValue should be float.
    SCALE_Z, ///< The z scale of ModelNode. MotionValue should be float.
  };

public: // Creation & Destruction
  /**
   * @brief Create an initialized MotionTransformIndex.
   *
   * @SINCE_2_2.99
   * @return A handle to a newly allocated Dali resource
   */
  static MotionTransformIndex New();

  /**
   * @brief Create an initialized MotionTransformIndex with values.
   *
   * @SINCE_2_2.99
   * @param[in] modelNodeId The id of model node what this motion index looks.
   * @param[in] type The type of transform what this motion index looks.
   * @return A handle to a newly allocated Dali resource
   */
  static MotionTransformIndex New(Property::Key modelNodeId, TransformType type);

  /**
   * @brief Creates an uninitialized MotionTransformIndex.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   *
   * @SINCE_2_2.99
   */
  MotionTransformIndex();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   *
   * @SINCE_2_2.99
   */
  ~MotionTransformIndex();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_2_2.99
   * @param[in] motionTransformIndex Handle to an object
   */
  MotionTransformIndex(const MotionTransformIndex& motionTransformIndex);

  /**
   * @brief Move constructor
   *
   * @SINCE_2_2.99
   * @param[in] rhs A reference to the moved handle
   */
  MotionTransformIndex(MotionTransformIndex&& rhs) noexcept;

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_2.99
   * @param[in] motionTransformIndex Handle to an object
   * @return reference to this
   */
  MotionTransformIndex& operator=(const MotionTransformIndex& motionTransformIndex);

  /**
   * @brief Move assignment
   *
   * @SINCE_2_2.99
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  MotionTransformIndex& operator=(MotionTransformIndex&& rhs) noexcept;

  /**
   * @brief Downcasts an Object handle to MotionTransformIndex.
   *
   * If handle points to a MotionTransformIndex, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_2_2.99
   * @param[in] handle Handle to an object
   * @return Handle to a MotionTransformIndex or an uninitialized handle
   */
  static MotionTransformIndex DownCast(BaseHandle handle);

public: // Public Method
  /**
   * @brief Set the type of transform what this motion index looks.
   *
   * @SINCE_2_2.99
   * @param[in] type The type of transform.
   */
  void SetTransformType(TransformType type);

  /**
   * @brief Get the type of transform what this motion index looks.
   *
   * @SINCE_2_2.99
   * @return The type of transform.
   */
  TransformType GetTransformType() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Scene3D::Internal implementation.
   *
   * @param[in] implementation The MotionTransformIndex implementation
   */
  DALI_INTERNAL MotionTransformIndex(Dali::Scene3D::Internal::MotionTransformIndex* implementation);
  /// @endcond
};

/**
 * @}
 */

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_MOTION_MOTION_TRANSFORM_INDEX_H