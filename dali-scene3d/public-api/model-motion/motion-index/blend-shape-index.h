#ifndef DALI_SCENE3D_MODEL_MOTION_BLEND_SHAPE_INDEX_H
#define DALI_SCENE3D_MODEL_MOTION_BLEND_SHAPE_INDEX_H

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
class BlendShapeIndex;
} // namespace Internal

/**
 * @addtogroup dali_scene3d_model_motion_blend_shape_index
 * @{
 */

/**
 * @brief Specialized MotionIndex to control blend shape.
 * We can control the blend shape by index (when we set BlendShapeId as IndexKey),
 * or by name (when we set BlendShapeId as StringKey).
 *
 * MotionValue should be float type.
 *
 * @code
 *
 * BlendShapeIndex blendShapeIndex0 = BlendShapeIndex::New("nodeName", 0u);
 * BlendShapeIndex blendShapeIndex1 = BlendShapeIndex::New("nodeName", "Target_1");
 *
 * // We can change the property later.
 * BlendShapeIndex blendShapeIndex2 = BlendShapeIndex::New();
 * blendShapeIndex2.SetModelNodeId("nodeName");
 * blendShapeIndex2.SetBlendShapeId("Target_2");
 *
 * @endcode
 *
 * Specially, if ModelNodeId is Property::INVALID_KEY and BlendShapeId is StringKey,
 * It will control all ModelNode that has the inputed BlendShape name.
 *
 * @code
 *
 * // If "node0" and "node1" has same BlendShape named "Smile",
 * // blendShapeIndexAll will control both nodes.
 * BlendShapeIndex blendShapeIndexAll = BlendShapeIndex::New("Smile");
 *
 * BlendShapeIndex blendShapeIndex0 = BlendShapeIndex::New("node0", "Smile");
 * BlendShapeIndex blendShapeIndex1 = BlendShapeIndex::New("node1", "Smile");
 *
 * @endcode
 *
 * @SINCE_2_2.34
 */
class DALI_SCENE3D_API BlendShapeIndex : public MotionIndex
{
public: // Creation & Destruction
  /**
   * @brief Create an initialized BlendShapeIndex.
   *
   * @SINCE_2_2.34
   * @return A handle to a newly allocated Dali resource
   */
  static BlendShapeIndex New();

  /**
   * @brief Create an initialized BlendShapeIndex with values. It will hold invalid ModelNodeId.
   *
   * @SINCE_2_2.34
   * @param[in] blendShapeId The id of blend shape what this motion index looks.
   * @return A handle to a newly allocated Dali resource
   */
  static BlendShapeIndex New(Property::Key blendShapeId);

  /**
   * @brief Create an initialized BlendShapeIndex with values.
   *
   * @SINCE_2_2.34
   * @param[in] modelNodeId The id of model node what this motion index looks.
   * @param[in] blendShapeId The id of blend shape what this motion index looks.
   * @return A handle to a newly allocated Dali resource
   */
  static BlendShapeIndex New(Property::Key modelNodeId, Property::Key blendShapeId);

  /**
   * @brief Creates an uninitialized BlendShapeIndex.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   *
   * @SINCE_2_2.34
   */
  BlendShapeIndex();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   *
   * @SINCE_2_2.34
   */
  ~BlendShapeIndex();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_2_2.34
   * @param[in] blendShapeIndex Handle to an object
   */
  BlendShapeIndex(const BlendShapeIndex& blendShapeIndex);

  /**
   * @brief Move constructor
   *
   * @SINCE_2_2.34
   * @param[in] rhs A reference to the moved handle
   */
  BlendShapeIndex(BlendShapeIndex&& rhs) noexcept;

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_2.34
   * @param[in] blendShapeIndex Handle to an object
   * @return reference to this
   */
  BlendShapeIndex& operator=(const BlendShapeIndex& blendShapeIndex);

  /**
   * @brief Move assignment
   *
   * @SINCE_2_2.34
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  BlendShapeIndex& operator=(BlendShapeIndex&& rhs) noexcept;

  /**
   * @brief Downcasts an Object handle to BlendShapeIndex.
   *
   * If handle points to a BlendShapeIndex, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_2_2.34
   * @param[in] handle Handle to an object
   * @return Handle to a BlendShapeIndex or an uninitialized handle
   */
  static BlendShapeIndex DownCast(BaseHandle handle);

public: // Public Method
  /**
   * @brief Set the id of BlendShape what this motion index looks.
   *
   * @SINCE_2_2.34
   * @param[in] blendShapeId The blend shape id.
   */
  void SetBlendShapeId(Property::Key blendShapeId);

  /**
   * @brief Get the id of BlendShape what this motion index looks.
   *
   * @SINCE_2_2.34
   * @return The blend shape id.
   */
  Property::Key GetBlendShapeId() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Scene3D::Internal implementation.
   *
   * @param[in] implementation The BlendShapeIndex implementation
   */
  DALI_INTERNAL BlendShapeIndex(Dali::Scene3D::Internal::BlendShapeIndex* implementation);
  /// @endcond
};

/**
 * @}
 */

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_MOTION_BLEND_SHAPE_INDEX_H