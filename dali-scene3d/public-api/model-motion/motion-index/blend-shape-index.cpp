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
#include <dali-scene3d/public-api/model-motion/motion-index/blend-shape-index.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-motion/motion-index/blend-shape-index-impl.h>

namespace Dali
{
namespace Scene3D
{
BlendShapeIndex BlendShapeIndex::New()
{
  Internal::BlendShapeIndexPtr internal = Internal::BlendShapeIndex::New();

  return BlendShapeIndex(internal.Get());
}

BlendShapeIndex BlendShapeIndex::New(Property::Key blendShapeId)
{
  Internal::BlendShapeIndexPtr internal = Internal::BlendShapeIndex::New();

  internal->SetBlendShapeId(blendShapeId);

  return BlendShapeIndex(internal.Get());
}

BlendShapeIndex BlendShapeIndex::New(Property::Key modelNodeId, Property::Key blendShapeId)
{
  Internal::BlendShapeIndexPtr internal = Internal::BlendShapeIndex::New();

  internal->SetModelNodeId(modelNodeId);
  internal->SetBlendShapeId(blendShapeId);

  return BlendShapeIndex(internal.Get());
}

BlendShapeIndex::BlendShapeIndex()
{
}

BlendShapeIndex::BlendShapeIndex(const BlendShapeIndex& blendShapeIndex) = default;

BlendShapeIndex::BlendShapeIndex(BlendShapeIndex&& rhs) noexcept = default;

BlendShapeIndex::~BlendShapeIndex()
{
}

BlendShapeIndex& BlendShapeIndex::operator=(const BlendShapeIndex& handle) = default;

BlendShapeIndex& BlendShapeIndex::operator=(BlendShapeIndex&& rhs) noexcept = default;

BlendShapeIndex BlendShapeIndex::DownCast(BaseHandle handle)
{
  return BlendShapeIndex(dynamic_cast<Dali::Scene3D::Internal::BlendShapeIndex*>(handle.GetObjectPtr()));
}

BlendShapeIndex::BlendShapeIndex(Dali::Scene3D::Internal::BlendShapeIndex* internal)
: MotionIndex(internal)
{
}

// Public Method

void BlendShapeIndex::SetBlendShapeId(Property::Key blendShapeId)
{
  GetImplementation(*this).SetBlendShapeId(blendShapeId);
}

Property::Key BlendShapeIndex::GetBlendShapeId() const
{
  return GetImplementation(*this).GetBlendShapeId();
}

} // namespace Scene3D

} // namespace Dali