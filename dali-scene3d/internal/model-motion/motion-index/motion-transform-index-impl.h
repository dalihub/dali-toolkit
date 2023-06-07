#ifndef DALI_SCENE3D_MODEL_MOTION_MOTION_TRANSFORM_INDEX_IMPL_H
#define DALI_SCENE3D_MODEL_MOTION_MOTION_TRANSFORM_INDEX_IMPL_H

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
#include <dali-scene3d/public-api/model-motion/motion-index/motion-transform-index.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
using MotionTransformIndexPtr = IntrusivePtr<MotionTransformIndex>;

/**
 * @brief Internal data for Scene3D::MotionTransformIndex.
 */
class MotionTransformIndex : public MotionIndex
{
public: // Creation & Destruction
  /**
   * @brief Create a new MotionTransformIndex object.
   * @return A smart-pointer to the newly allocated MotionTransformIndex.
   */
  static MotionTransformIndexPtr New();

protected:
  /**
   * @brief Construct a new MotionTransformIndex.
   */
  MotionTransformIndex();

  /**
   * @brief Second-phase constructor.
   */
  void Initialize();

  /**
   * @brief Virtual destructor.
   */
  virtual ~MotionTransformIndex();

public: // Public Method
  /**
   * @copydoc Dali::Scene3D::MotionTransformIndex::SetTransformType()
   */
  void SetTransformType(Scene3D::MotionTransformIndex::TransformType type);

  /**
   * @copydoc Dali::Scene3D::BlendShapeIndex::GetTransformType()
   */
  Scene3D::MotionTransformIndex::TransformType GetTransformType() const;

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
  Scene3D::MotionTransformIndex::TransformType mTransformType{Scene3D::MotionTransformIndex::TransformType::INVALID}; ///< The type of transform
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::MotionTransformIndex& GetImplementation(Dali::Scene3D::MotionTransformIndex& motionTransformIndex)
{
  DALI_ASSERT_ALWAYS(motionTransformIndex && "MotionTransformIndex handle is empty");

  BaseObject& handle = motionTransformIndex.GetBaseObject();

  return static_cast<Internal::MotionTransformIndex&>(handle);
}

inline const Internal::MotionTransformIndex& GetImplementation(const Dali::Scene3D::MotionTransformIndex& motionTransformIndex)
{
  DALI_ASSERT_ALWAYS(motionTransformIndex && "MotionTransformIndex handle is empty");

  const BaseObject& handle = motionTransformIndex.GetBaseObject();

  return static_cast<const Internal::MotionTransformIndex&>(handle);
}

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_MOTION_MOTION_TRANSFORM_INDEX_IMPL_H