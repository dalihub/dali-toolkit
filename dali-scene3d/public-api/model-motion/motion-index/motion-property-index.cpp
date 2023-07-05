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
#include <dali-scene3d/public-api/model-motion/motion-index/motion-property-index.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-motion/motion-index/motion-property-index-impl.h>

namespace Dali
{
namespace Scene3D
{
MotionPropertyIndex MotionPropertyIndex::New()
{
  Internal::MotionPropertyIndexPtr internal = Internal::MotionPropertyIndex::New();

  return MotionPropertyIndex(internal.Get());
}

MotionPropertyIndex MotionPropertyIndex::New(Property::Key modelNodeId, Property::Key propertyId)
{
  Internal::MotionPropertyIndexPtr internal = Internal::MotionPropertyIndex::New();

  internal->SetModelNodeId(modelNodeId);
  internal->SetPropertyId(propertyId);

  return MotionPropertyIndex(internal.Get());
}

MotionPropertyIndex::MotionPropertyIndex()
{
}

MotionPropertyIndex::MotionPropertyIndex(const MotionPropertyIndex& motionPropertyIndex) = default;

MotionPropertyIndex::MotionPropertyIndex(MotionPropertyIndex&& rhs) noexcept = default;

MotionPropertyIndex::~MotionPropertyIndex()
{
}

MotionPropertyIndex& MotionPropertyIndex::operator=(const MotionPropertyIndex& handle) = default;

MotionPropertyIndex& MotionPropertyIndex::operator=(MotionPropertyIndex&& rhs) noexcept = default;

MotionPropertyIndex MotionPropertyIndex::DownCast(BaseHandle handle)
{
  return MotionPropertyIndex(dynamic_cast<Dali::Scene3D::Internal::MotionPropertyIndex*>(handle.GetObjectPtr()));
}

MotionPropertyIndex::MotionPropertyIndex(Dali::Scene3D::Internal::MotionPropertyIndex* internal)
: MotionIndex(internal)
{
}

// Public Method

void MotionPropertyIndex::SetPropertyId(Property::Key blendShapeId)
{
  GetImplementation(*this).SetPropertyId(blendShapeId);
}

Property::Key MotionPropertyIndex::GetPropertyId() const
{
  return GetImplementation(*this).GetPropertyId();
}

} // namespace Scene3D

} // namespace Dali