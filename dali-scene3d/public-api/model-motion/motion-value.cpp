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

// CLASS HEADER
#include <dali-scene3d/public-api/model-motion/motion-value.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-motion/motion-value-impl.h>

namespace Dali
{
namespace Scene3D
{
MotionValue MotionValue::New()
{
  Internal::MotionValuePtr internal = Internal::MotionValue::New();

  return MotionValue(internal.Get());
}

MotionValue MotionValue::New(Property::Value propertyValue)
{
  Internal::MotionValuePtr internal = Internal::MotionValue::New();

  internal->SetValue(propertyValue);

  return MotionValue(internal.Get());
}

MotionValue MotionValue::New(KeyFrames keyFrames)
{
  Internal::MotionValuePtr internal = Internal::MotionValue::New();

  internal->SetValue(keyFrames);

  return MotionValue(internal.Get());
}

MotionValue::MotionValue()
{
}

MotionValue::MotionValue(const MotionValue& motionValue) = default;

MotionValue::MotionValue(MotionValue&& rhs) noexcept = default;

MotionValue::~MotionValue()
{
}

MotionValue& MotionValue::operator=(const MotionValue& handle) = default;

MotionValue& MotionValue::operator=(MotionValue&& rhs) noexcept = default;

MotionValue MotionValue::DownCast(BaseHandle handle)
{
  return MotionValue(dynamic_cast<Dali::Scene3D::Internal::MotionValue*>(handle.GetObjectPtr()));
}

MotionValue::MotionValue(Dali::Scene3D::Internal::MotionValue* internal)
: BaseHandle(internal)
{
}

// Public Method

MotionValue::ValueType MotionValue::GetValueType() const
{
  return GetImplementation(*this).GetValueType();
}

void MotionValue::SetValue(Property::Value propertyValue)
{
  GetImplementation(*this).SetValue(propertyValue);
}

void MotionValue::SetValue(KeyFrames keyFrames)
{
  GetImplementation(*this).SetValue(keyFrames);
}

void MotionValue::Invalidate()
{
  Clear();
}

void MotionValue::Clear()
{
  GetImplementation(*this).Clear();
}

Property::Value MotionValue::GetPropertyValue() const
{
  return GetImplementation(*this).GetPropertyValue();
}

KeyFrames MotionValue::GetKeyFrames() const
{
  return GetImplementation(*this).GetKeyFrames();
}

} // namespace Scene3D

} // namespace Dali