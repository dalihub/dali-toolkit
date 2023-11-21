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
#include <dali-scene3d/public-api/model-motion/motion-data.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-motion/motion-data-impl.h>

namespace Dali
{
namespace Scene3D
{
MotionData MotionData::New()
{
  Internal::MotionDataPtr internal = Internal::MotionData::New();

  return MotionData(internal.Get());
}

MotionData MotionData::New(float durationSeconds)
{
  Internal::MotionDataPtr internal = Internal::MotionData::New();

  internal->SetDuration(durationSeconds);

  return MotionData(internal.Get());
}

MotionData::MotionData()
{
}

MotionData::MotionData(const MotionData& motionData) = default;

MotionData::MotionData(MotionData&& rhs) noexcept = default;

MotionData::~MotionData()
{
}

MotionData& MotionData::operator=(const MotionData& handle) = default;

MotionData& MotionData::operator=(MotionData&& rhs) noexcept = default;

MotionData MotionData::DownCast(BaseHandle handle)
{
  return MotionData(dynamic_cast<Dali::Scene3D::Internal::MotionData*>(handle.GetObjectPtr()));
}

MotionData::MotionData(Dali::Scene3D::Internal::MotionData* internal)
: BaseHandle(internal)
{
}

// Public Method

uint32_t MotionData::GetMotionCount() const
{
  return GetImplementation(*this).GetMotionCount();
}

MotionIndex MotionData::GetIndex(uint32_t index) const
{
  return GetImplementation(*this).GetIndex(index);
}

MotionValue MotionData::GetValue(uint32_t index) const
{
  return GetImplementation(*this).GetValue(index);
}

void MotionData::Add(MotionIndex index, MotionValue value)
{
  GetImplementation(*this).Add(index, value);
}

void MotionData::Clear()
{
  GetImplementation(*this).Clear();
}

void MotionData::SetDuration(float durationSeconds)
{
  GetImplementation(*this).SetDuration(durationSeconds);
}

float MotionData::GetDuration() const
{
  return GetImplementation(*this).GetDuration();
}

void MotionData::LoadBvh(const std::string& path, const Vector3& scale, bool synchronousLoad)
{
  GetImplementation(*this).LoadBvh(path, false, scale, synchronousLoad);
}

void MotionData::LoadBvh(const std::string& path, bool useRootTranslationOnly, const Vector3& scale, bool synchronousLoad)
{
  GetImplementation(*this).LoadBvh(path, useRootTranslationOnly, scale, synchronousLoad);
}

void MotionData::LoadBvhFromBuffer(const uint8_t* rawBuffer, int rawBufferLength, const Vector3& scale, bool synchronousLoad)
{
  GetImplementation(*this).LoadBvhFromBuffer(rawBuffer, rawBufferLength, false, scale, synchronousLoad);
}

void MotionData::LoadBvhFromBuffer(const uint8_t* rawBuffer, int rawBufferLength, bool useRootTranslationOnly, const Vector3& scale, bool synchronousLoad)
{
  GetImplementation(*this).LoadBvhFromBuffer(rawBuffer, rawBufferLength, useRootTranslationOnly, scale, synchronousLoad);
}

void MotionData::LoadFacialAnimation(const std::string& url, bool synchronousLoad)
{
  GetImplementation(*this).LoadFacialAnimation(url, synchronousLoad);
}

void MotionData::LoadFacialAnimationFromBuffer(const uint8_t* rawBuffer, int rawBufferLength, bool synchronousLoad)
{
  GetImplementation(*this).LoadFacialAnimationFromBuffer(rawBuffer, rawBufferLength, synchronousLoad);
}

MotionData::LoadCompletedSignalType& MotionData::LoadCompletedSignal()
{
  return GetImplementation(*this).LoadCompletedSignal();
}

} // namespace Scene3D

} // namespace Dali