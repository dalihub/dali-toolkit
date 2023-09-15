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
 */

#include <dali-physics/integration-api/physics-world.h>
#include <dali-physics/internal/physics-world-impl.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali::Toolkit::Physics::Integration
{
PhysicsWorld::PhysicsWorld(Dali::Toolkit::Physics::Internal::PhysicsWorld* internalWorld)
: impl(internalWorld)
{
}

PhysicsWorld::~PhysicsWorld() = default;

Dali::Toolkit::Physics::Internal::PhysicsWorld* PhysicsWorld::GetImpl()
{
  return impl;
}

void PhysicsWorld::Lock()
{
  DALI_ASSERT_ALWAYS(impl && "Physics world proxy has null impl");
  impl->Lock();
}

void PhysicsWorld::Unlock()
{
  DALI_ASSERT_ALWAYS(impl && "Physics world proxy has null impl");
  impl->Unlock();
}

Dali::Any PhysicsWorld::GetNative()
{
  DALI_ASSERT_ALWAYS(impl && "Physics world proxy has null impl");
  return impl->GetNative();
}

Dali::Any PhysicsWorld::HitTest(Dali::Vector3 rayFromWorld, Dali::Vector3 rayToWorld, Dali::Any nativeFilter, Dali::Vector3& localPivot, float& distanceFromCamera)

{
  DALI_ASSERT_ALWAYS(impl && "Physics world proxy has null impl");
  return impl->HitTest(rayFromWorld, rayToWorld, nativeFilter, localPivot, distanceFromCamera);
}

} // namespace Dali::Toolkit::Physics::Integration
