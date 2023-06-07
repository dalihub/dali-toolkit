#ifndef DALI_SCENE3D_MODEL_MOTION_MOTION_PROPERTY_INDEX_IMPL_H
#define DALI_SCENE3D_MODEL_MOTION_MOTION_PROPERTY_INDEX_IMPL_H

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
#include <dali-scene3d/public-api/model-motion/motion-index/motion-property-index.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
using MotionPropertyIndexPtr = IntrusivePtr<MotionPropertyIndex>;

/**
 * @brief Internal data for Scene3D::MotionPropertyIndex.
 */
class MotionPropertyIndex : public MotionIndex
{
public: // Creation & Destruction
  /**
   * @brief Create a new MotionPropertyIndex object.
   * @return A smart-pointer to the newly allocated MotionPropertyIndex.
   */
  static MotionPropertyIndexPtr New();

protected:
  /**
   * @brief Construct a new MotionPropertyIndex.
   */
  MotionPropertyIndex();

  /**
   * @brief Second-phase constructor.
   */
  void Initialize();

  /**
   * @brief Virtual destructor.
   */
  virtual ~MotionPropertyIndex();

public: // Public Method
  /**
   * @copydoc Dali::Scene3D::MotionPropertyIndex::SetPropertyId()
   */
  void SetPropertyId(Property::Key propertyId);

  /**
   * @copydoc Dali::Scene3D::MotionPropertyIndex::GetPropertyId()
   */
  Property::Key GetPropertyId() const;

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
  Property::Key mPropertyId{Property::INVALID_KEY}; ///< The id of blend shape
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::MotionPropertyIndex& GetImplementation(Dali::Scene3D::MotionPropertyIndex& motionPropertyIndex)
{
  DALI_ASSERT_ALWAYS(motionPropertyIndex && "MotionPropertyIndex handle is empty");

  BaseObject& handle = motionPropertyIndex.GetBaseObject();

  return static_cast<Internal::MotionPropertyIndex&>(handle);
}

inline const Internal::MotionPropertyIndex& GetImplementation(const Dali::Scene3D::MotionPropertyIndex& motionPropertyIndex)
{
  DALI_ASSERT_ALWAYS(motionPropertyIndex && "MotionPropertyIndex handle is empty");

  const BaseObject& handle = motionPropertyIndex.GetBaseObject();

  return static_cast<const Internal::MotionPropertyIndex&>(handle);
}

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_MOTION_MOTION_PROPERTY_INDEX_IMPL_H