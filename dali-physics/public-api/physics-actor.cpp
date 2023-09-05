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

// Class Header
#include <dali-physics/public-api/physics-actor.h>

// Internal headers
#include <dali-physics/internal/physics-actor-impl.h>
#include <dali-physics/internal/physics-adaptor-impl.h>

namespace Dali::Toolkit::Physics
{
PhysicsActor::PhysicsActor()                            = default;
PhysicsActor::~PhysicsActor()                           = default;
PhysicsActor::PhysicsActor(const PhysicsActor& handle)  = default;
PhysicsActor::PhysicsActor(PhysicsActor&& rhs) noexcept = default;
PhysicsActor& PhysicsActor::operator=(const PhysicsActor& handle) = default;
PhysicsActor& PhysicsActor::operator=(PhysicsActor&& handle) noexcept = default;

PhysicsActor PhysicsActor::New(Dali::Actor actor, Dali::Any body, PhysicsAdaptor adaptor)
{
  Dali::Toolkit::Physics::Internal::PhysicsActorPtr internal = Internal::PhysicsActor::New(actor, body, GetImplementation(adaptor));
  return PhysicsActor(internal.Get());
}

PhysicsActor PhysicsActor::DownCast(BaseHandle handle)
{
  return PhysicsActor(dynamic_cast<Dali::Toolkit::Physics::Internal::PhysicsActor*>(handle.GetObjectPtr()));
}

uint32_t PhysicsActor::GetId() const
{
  return GetImplementation(*this).GetId();
}

Dali::Any PhysicsActor::GetBody() const
{
  return GetImplementation(*this).GetBody();
}

void PhysicsActor::AsyncSetPhysicsPosition(Dali::Vector3 actorPosition)
{
  GetImplementation(*this).AsyncSetPhysicsPosition(actorPosition);
}

void PhysicsActor::AsyncSetPhysicsRotation(Dali::Quaternion actorRotation)
{
  GetImplementation(*this).AsyncSetPhysicsRotation(actorRotation);
}

Dali::Vector3 PhysicsActor::GetPhysicsPosition() const
{
  return GetImplementation(*this).GetPhysicsPosition();
}

Dali::Quaternion PhysicsActor::GetPhysicsRotation() const
{
  return GetImplementation(*this).GetPhysicsRotation();
}

Dali::Vector3 PhysicsActor::GetActorPosition() const
{
  return GetImplementation(*this).GetActorPosition();
}

Dali::Quaternion PhysicsActor::GetActorRotation() const
{
  return GetImplementation(*this).GetActorRotation();
}

PhysicsActor::PhysicsActor(Internal::PhysicsActor* impl)
: BaseHandle(impl)
{
}

} // namespace Dali::Toolkit::Physics
