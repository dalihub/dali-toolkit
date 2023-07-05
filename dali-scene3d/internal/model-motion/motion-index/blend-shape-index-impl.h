#ifndef DALI_SCENE3D_MODEL_MOTION_BLEND_SHAPE_INDEX_IMPL_H
#define DALI_SCENE3D_MODEL_MOTION_BLEND_SHAPE_INDEX_IMPL_H

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

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-motion/motion-index/motion-index-impl.h>
#include <dali-scene3d/public-api/model-motion/motion-index/blend-shape-index.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
using BlendShapeIndexPtr = IntrusivePtr<BlendShapeIndex>;

/**
 * @brief Internal data for Scene3D::BlendShapeIndex.
 */
class BlendShapeIndex : public MotionIndex
{
public: // Creation & Destruction
  /**
   * @brief Create a new BlendShapeIndex object.
   * @return A smart-pointer to the newly allocated BlendShapeIndex.
   */
  static BlendShapeIndexPtr New();

protected:
  /**
   * @brief Construct a new BlendShapeIndex.
   */
  BlendShapeIndex();

  /**
   * @brief Second-phase constructor.
   */
  void Initialize();

  /**
   * @brief Virtual destructor.
   */
  virtual ~BlendShapeIndex();

public: // Public Method
  /**
   * @copydoc Dali::Scene3D::BlendShapeIndex::SetBlendShapeId()
   */
  void SetBlendShapeId(Property::Key blendShapeId);

  /**
   * @copydoc Dali::Scene3D::BlendShapeIndex::GetBlendShapeId()
   */
  Property::Key GetBlendShapeId() const;

public: // MotionIndex Override Method
  /**
   * @copydoc Dali::Scene3D::Internal::MotionIndex::GetPropertyName()
   */
  std::string GetPropertyName(Scene3D::ModelNode node = Scene3D::ModelNode()) override;

  /**
   * @copydoc Dali::Scene3D::Internal::MotionIndex::GetPropertyIndex()
   */
  Property::Index GetPropertyIndex(Scene3D::ModelNode node = Scene3D::ModelNode()) override;

protected:
  Property::Key mBlendShapeId{Property::INVALID_KEY}; ///< The id of blend shape
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::BlendShapeIndex& GetImplementation(Dali::Scene3D::BlendShapeIndex& blendShapeIndex)
{
  DALI_ASSERT_ALWAYS(blendShapeIndex && "BlendShapeIndex handle is empty");

  BaseObject& handle = blendShapeIndex.GetBaseObject();

  return static_cast<Internal::BlendShapeIndex&>(handle);
}

inline const Internal::BlendShapeIndex& GetImplementation(const Dali::Scene3D::BlendShapeIndex& blendShapeIndex)
{
  DALI_ASSERT_ALWAYS(blendShapeIndex && "BlendShapeIndex handle is empty");

  const BaseObject& handle = blendShapeIndex.GetBaseObject();

  return static_cast<const Internal::BlendShapeIndex&>(handle);
}

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_MOTION_BLEND_SHAPE_INDEX_IMPL_H