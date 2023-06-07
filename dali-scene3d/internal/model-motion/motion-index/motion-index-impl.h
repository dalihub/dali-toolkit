#ifndef DALI_SCENE3D_MODEL_MOTION_MOTION_INDEX_IMPL_H
#define DALI_SCENE3D_MODEL_MOTION_MOTION_INDEX_IMPL_H

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
#include <dali-scene3d/public-api/model-motion/motion-index/motion-index.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
using MotionIndexPtr = IntrusivePtr<MotionIndex>;

/**
 * @brief Internal abstract data for Scene3D::MotionIndex.
 * The drived class can implemet there own GetPropertyIndex() and GetPropertyName() API.
 */
class MotionIndex : public BaseObject
{
protected:
  /**
   * @brief Construct a new MotionIndex.
   */
  MotionIndex();

  /**
   * @brief Virtual destructor.
   */
  virtual ~MotionIndex();

public: // Public Method
  /**
   * @copydoc Dali::Scene3D::MotionIndex::SetModelNodeId()
   */
  void SetModelNodeId(Property::Key modelNodeId);

  /**
   * @copydoc Dali::Scene3D::MotionIndex::GetModelNodeId()
   */
  Property::Key GetModelNodeId() const;

public: // Virtual Method
  /**
   * @copydoc Dali::Scene3D::MotionIndex::GetPropertyName()
   */
  virtual std::string GetPropertyName(Scene3D::ModelNode node = Scene3D::ModelNode()) = 0;

  /**
   * @copydoc Dali::Scene3D::MotionIndex::GetPropertyIndex()
   */
  virtual Property::Index GetPropertyIndex(Scene3D::ModelNode node = Scene3D::ModelNode()) = 0;

protected:
  Property::Key mModelNodeId{Property::INVALID_KEY}; ///< The id of model node
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::MotionIndex& GetImplementation(Dali::Scene3D::MotionIndex& motionIndex)
{
  DALI_ASSERT_ALWAYS(motionIndex && "MotionIndex handle is empty");

  BaseObject& handle = motionIndex.GetBaseObject();

  return static_cast<Internal::MotionIndex&>(handle);
}

inline const Internal::MotionIndex& GetImplementation(const Dali::Scene3D::MotionIndex& motionIndex)
{
  DALI_ASSERT_ALWAYS(motionIndex && "MotionIndex handle is empty");

  const BaseObject& handle = motionIndex.GetBaseObject();

  return static_cast<const Internal::MotionIndex&>(handle);
}

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_MOTION_MOTION_INDEX_IMPL_H