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
#include <dali-physics/public-api/physics-adaptor.h>

// Internal headers
#include <dali-physics/internal/physics-actor-impl.h>
#include <dali-physics/internal/physics-adaptor-impl.h>
#include <dali-physics/public-api/physics-actor.h>

namespace Dali::Toolkit::Physics
{
PhysicsAdaptor::PhysicsAdaptor() = default;

PhysicsAdaptor::~PhysicsAdaptor() = default;

PhysicsAdaptor::PhysicsAdaptor(const PhysicsAdaptor& handle) = default;

PhysicsAdaptor::PhysicsAdaptor(PhysicsAdaptor&& rhs) noexcept = default;

PhysicsAdaptor& PhysicsAdaptor::operator=(const PhysicsAdaptor& handle) = default;

PhysicsAdaptor& PhysicsAdaptor::operator=(PhysicsAdaptor&& handle) noexcept = default;

PhysicsAdaptor PhysicsAdaptor::New(const Dali::Matrix& transform, Uint16Pair size)
{
  Internal::PhysicsAdaptorPtr internal = Internal::PhysicsAdaptor::New(transform, size);
  return PhysicsAdaptor(internal.Get());
}

PhysicsAdaptor PhysicsAdaptor::DownCast(BaseHandle handle)
{
  return PhysicsAdaptor(dynamic_cast<Dali::Toolkit::Physics::Internal::PhysicsAdaptor*>(handle.GetObjectPtr()));
}

void PhysicsAdaptor::SetTimestep(float timestep)
{
  GetImplementation(*this).SetTimestep(timestep);
}

float PhysicsAdaptor::GetTimestep()
{
  return GetImplementation(*this).GetTimestep();
}

PhysicsAdaptor::ScopedPhysicsAccessorPtr PhysicsAdaptor::GetPhysicsAccessor()
{
  return GetImplementation(*this).GetPhysicsAccessor();
}

Dali::Layer PhysicsAdaptor::CreateDebugLayer(Dali::Window window)
{
  return GetImplementation(*this).CreateDebugLayer(window);
}

Dali::Vector3 PhysicsAdaptor::TranslateToPhysicsSpace(Dali::Vector3 vector)
{
  return GetImplementation(*this).TranslateToPhysicsSpace(vector);
}

Dali::Quaternion PhysicsAdaptor::TranslateToPhysicsSpace(Dali::Quaternion rotation)
{
  return GetImplementation(*this).TranslateToPhysicsSpace(rotation);
}

Dali::Vector3 PhysicsAdaptor::TranslateFromPhysicsSpace(Dali::Vector3 vector)
{
  return GetImplementation(*this).TranslateFromPhysicsSpace(vector);
}

Dali::Vector3 PhysicsAdaptor::ConvertVectorToPhysicsSpace(Dali::Vector3 vector)
{
  return GetImplementation(*this).ConvertVectorToPhysicsSpace(vector);
}

Dali::Vector3 PhysicsAdaptor::ConvertVectorFromPhysicsSpace(Dali::Vector3 vector)
{
  return GetImplementation(*this).ConvertVectorFromPhysicsSpace(vector);
}

void PhysicsAdaptor::SetTransformAndSize(const Dali::Matrix& transform, Uint16Pair size)
{
  GetImplementation(*this).SetTransformAndSize(transform, size);
}

void PhysicsAdaptor::SetIntegrationState(Physics::PhysicsAdaptor::IntegrationState state)
{
  GetImplementation(*this).SetIntegrationState(state);
}

Physics::PhysicsAdaptor::IntegrationState PhysicsAdaptor::GetIntegrationState()
{
  return GetImplementation(*this).GetIntegrationState();
}

void PhysicsAdaptor::SetDebugState(Physics::PhysicsAdaptor::DebugState state)
{
  GetImplementation(*this).SetDebugState(state);
}

Physics::PhysicsAdaptor::DebugState PhysicsAdaptor::GetDebugState()
{
  return GetImplementation(*this).GetDebugState();
}

PhysicsActor PhysicsAdaptor::AddActorBody(Dali::Actor actor, Dali::Any body)
{
  Internal::PhysicsActorPtr physicsActor = GetImplementation(*this).AddActorBody(actor, body);
  return PhysicsActor(physicsActor.Get());
}

PhysicsActor PhysicsAdaptor::GetPhysicsActor(Dali::Any body)
{
  Internal::PhysicsActorPtr physicsActor = GetImplementation(*this).GetPhysicsActor(body);
  return PhysicsActor(physicsActor.Get());
}

Dali::Actor PhysicsAdaptor::GetRootActor()
{
  return GetImplementation(*this).GetRootActor();
}

void PhysicsAdaptor::BuildPickingRay(Dali::Vector3 origin, Dali::Vector3 direction, Dali::Vector3& rayFromWorld, Dali::Vector3& rayToWorld)
{
  GetImplementation(*this).BuildPickingRay(origin, direction, rayFromWorld, rayToWorld);
}

Dali::Vector3 PhysicsAdaptor::ProjectPoint(Dali::Vector3 origin, Dali::Vector3 direction, float distance)
{
  return GetImplementation(*this).ProjectPoint(origin, direction, distance);
}

void PhysicsAdaptor::Queue(std::function<void(void)> function)
{
  GetImplementation(*this).Queue(function);
}

void PhysicsAdaptor::CreateSyncPoint()
{
  GetImplementation(*this).CreateSyncPoint();
}

PhysicsAdaptor::PhysicsAdaptor(Internal::PhysicsAdaptor* impl)
: BaseHandle(impl)
{
}

} // namespace Dali::Toolkit::Physics
