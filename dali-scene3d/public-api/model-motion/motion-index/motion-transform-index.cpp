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
#include <dali-scene3d/public-api/model-motion/motion-index/motion-transform-index.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-motion/motion-index/motion-transform-index-impl.h>

namespace Dali
{
namespace Scene3D
{
MotionTransformIndex MotionTransformIndex::New()
{
  Internal::MotionTransformIndexPtr internal = Internal::MotionTransformIndex::New();

  return MotionTransformIndex(internal.Get());
}

MotionTransformIndex MotionTransformIndex::New(Property::Key modelNodeId, TransformType type)
{
  Internal::MotionTransformIndexPtr internal = Internal::MotionTransformIndex::New();

  internal->SetModelNodeId(modelNodeId);
  internal->SetTransformType(type);

  return MotionTransformIndex(internal.Get());
}

MotionTransformIndex::MotionTransformIndex()
{
}

MotionTransformIndex::MotionTransformIndex(const MotionTransformIndex& motionTransformIndex) = default;

MotionTransformIndex::MotionTransformIndex(MotionTransformIndex&& rhs) noexcept = default;

MotionTransformIndex::~MotionTransformIndex()
{
}

MotionTransformIndex& MotionTransformIndex::operator=(const MotionTransformIndex& handle) = default;

MotionTransformIndex& MotionTransformIndex::operator=(MotionTransformIndex&& rhs) noexcept = default;

MotionTransformIndex MotionTransformIndex::DownCast(BaseHandle handle)
{
  return MotionTransformIndex(dynamic_cast<Dali::Scene3D::Internal::MotionTransformIndex*>(handle.GetObjectPtr()));
}

MotionTransformIndex::MotionTransformIndex(Dali::Scene3D::Internal::MotionTransformIndex* internal)
: MotionIndex(internal)
{
}

// Public Method

void MotionTransformIndex::SetTransformType(MotionTransformIndex::TransformType type)
{
  GetImplementation(*this).SetTransformType(type);
}

MotionTransformIndex::TransformType MotionTransformIndex::GetTransformType() const
{
  return GetImplementation(*this).GetTransformType();
}

} // namespace Scene3D

} // namespace Dali