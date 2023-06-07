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
#include <dali-scene3d/public-api/model-motion/motion-index/motion-index.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-motion/motion-index/motion-index-impl.h>

namespace Dali
{
namespace Scene3D
{
MotionIndex::MotionIndex()
{
}

MotionIndex::MotionIndex(const MotionIndex& motionIndex) = default;

MotionIndex::MotionIndex(MotionIndex&& rhs) noexcept = default;

MotionIndex::~MotionIndex()
{
}

MotionIndex& MotionIndex::operator=(const MotionIndex& handle) = default;

MotionIndex& MotionIndex::operator=(MotionIndex&& rhs) noexcept = default;

MotionIndex MotionIndex::DownCast(BaseHandle handle)
{
  return MotionIndex(dynamic_cast<Dali::Scene3D::Internal::MotionIndex*>(handle.GetObjectPtr()));
}

MotionIndex::MotionIndex(Dali::Scene3D::Internal::MotionIndex* internal)
: BaseHandle(internal)
{
}

// Public Method

void MotionIndex::SetModelNodeId(Property::Key modelNodeId)
{
  GetImplementation(*this).SetModelNodeId(modelNodeId);
}

Property::Key MotionIndex::GetModelNodeId() const
{
  return GetImplementation(*this).GetModelNodeId();
}

std::string MotionIndex::GetPropertyName(ModelNode node)
{
  return GetImplementation(*this).GetPropertyName(node);
}

Property::Index MotionIndex::GetPropertyIndex(ModelNode node)
{
  return GetImplementation(*this).GetPropertyIndex(node);
}

} // namespace Scene3D

} // namespace Dali