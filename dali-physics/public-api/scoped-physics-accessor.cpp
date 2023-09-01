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

#include <dali-physics/public-api/physics-adaptor.h>

#include <dali-physics/internal/physics-adaptor-impl.h>
#include <dali-physics/internal/physics-world-impl.h>

namespace Dali::Toolkit::Physics
{
struct PhysicsAdaptor::ScopedPhysicsAccessor::Impl
{
  Impl(Internal::PhysicsWorld& world)
  : mLock(world.GetMutex()),
    mPhysicsWorld(world)
  {
  }
  Impl(Impl&)         = delete;
  const Impl& operator=(const Impl&) = delete;

  Dali::Mutex::ScopedLock mLock;
  Internal::PhysicsWorld& mPhysicsWorld;
  friend Internal::PhysicsAdaptor;
};

PhysicsAdaptor::ScopedPhysicsAccessor::ScopedPhysicsAccessor(Internal::PhysicsWorld& world)
: mImpl(new Impl(world))
{
}

PhysicsAdaptor::ScopedPhysicsAccessor::~ScopedPhysicsAccessor()
{
  delete mImpl;
}

Dali::Any PhysicsAdaptor::ScopedPhysicsAccessor::GetNative()
{
  return mImpl->mPhysicsWorld.GetNative();
}

Dali::Any PhysicsAdaptor::ScopedPhysicsAccessor::HitTest(
  Dali::Vector3 rayFromWorld, Dali::Vector3 rayToWorld, Dali::Vector3& localPivot, float& distanceFromCamera)
{
  return mImpl->mPhysicsWorld.HitTest(rayFromWorld, rayToWorld, localPivot, distanceFromCamera);
}

} // namespace Dali::Toolkit::Physics
