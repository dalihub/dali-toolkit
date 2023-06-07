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
#include <dali-scene3d/internal/model-motion/motion-index/motion-index-impl.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
namespace
{
} // unnamed namespace

MotionIndex::MotionIndex()
{
}

MotionIndex::~MotionIndex()
{
}

void MotionIndex::SetModelNodeId(Property::Key modelNodeId)
{
  mModelNodeId = modelNodeId;
}

Property::Key MotionIndex::GetModelNodeId() const
{
  return mModelNodeId;
}

} // namespace Internal

} // namespace Scene3D

} // namespace Dali