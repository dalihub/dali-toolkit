#ifndef DALI_SCENE3D_MODEL_MOTION_MOTION_VALUE_IMPL_H
#define DALI_SCENE3D_MODEL_MOTION_MOTION_VALUE_IMPL_H

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
#include <dali-scene3d/public-api/model-motion/motion-value.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
using MotionValuePtr = IntrusivePtr<MotionValue>;

/**
 * @brief Internal data for Scene3D::MotionValue.
 */
class MotionValue : public BaseObject
{
public: // Creation & Destruction
  /**
   * @brief Create a new MotionValue object.
   * @return A smart-pointer to the newly allocated MotionValue.
   */
  static MotionValuePtr New();

protected:
  /**
   * @brief Construct a new MotionValue.
   */
  MotionValue();

  /**
   * @brief Second-phase constructor.
   */
  void Initialize();

  /**
   * @brief Virtual destructor.
   */
  virtual ~MotionValue();

public: // Public Method
  /**
   * @copydoc Dali::Scene3D::MotionValue::GetValueType()
   */
  Scene3D::MotionValue::ValueType GetValueType() const;

  /**
   * @copydoc Dali::Scene3D::MotionValue::SetValue()
   */
  void SetValue(Property::Value propertyValue);

  /**
   * @copydoc Dali::Scene3D::MotionValue::SetValue()
   */
  void SetValue(Dali::KeyFrames keyFrames);

  /**
   * @copydoc Dali::Scene3D::MotionValue::Clear()
   */
  void Clear();

  /**
   * @copydoc Dali::Scene3D::MotionValue::GetPropertyValue()
   */
  Property::Value GetPropertyValue() const;

  /**
   * @copydoc Dali::Scene3D::MotionValue::GetKeyFrames()
   */
  KeyFrames GetKeyFrames() const;

private:
  Scene3D::MotionValue::ValueType mType{Scene3D::MotionValue::ValueType::INVALID};

  Property::Value mPropertyValue{};
  Dali::KeyFrames mKeyFrames{};
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::MotionValue& GetImplementation(Dali::Scene3D::MotionValue& motionValue)
{
  DALI_ASSERT_ALWAYS(motionValue && "MotionValue handle is empty");

  BaseObject& handle = motionValue.GetBaseObject();

  return static_cast<Internal::MotionValue&>(handle);
}

inline const Internal::MotionValue& GetImplementation(const Dali::Scene3D::MotionValue& motionValue)
{
  DALI_ASSERT_ALWAYS(motionValue && "MotionValue handle is empty");

  const BaseObject& handle = motionValue.GetBaseObject();

  return static_cast<const Internal::MotionValue&>(handle);
}

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_MOTION_MOTION_VALUE_IMPL_H