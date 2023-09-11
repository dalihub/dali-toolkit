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
#include <dali-physics/internal/physics-adaptor-impl.h>

// External Headers
#include <memory>
#include <utility>

// Internal Headers
#include <dali-physics/internal/physics-world-impl.h>
#include <dali/dali.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/drawable-actor.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, false, "LOG_PHYSICS");
#endif

} // namespace

namespace Dali::Toolkit::Physics::Internal
{
PhysicsAdaptor::PhysicsAdaptor()
: mSlotDelegate(this)
{
}

PhysicsAdaptor::~PhysicsAdaptor()
{
}

void PhysicsAdaptor::Initialize(const Dali::Matrix& transform, Uint16Pair worldSize)
{
  // Create an actor that can handle mouse events.
  // @todo Enable this to be fully configured / provided
  mRootActor                                 = Layer::New();
  mRootActor[Actor::Property::NAME]          = "PhysicsRootLayer";
  mRootActor[Layer::Property::BEHAVIOR]      = Layer::LAYER_3D;
  mRootActor[Layer::Property::DEPTH_TEST]    = true;
  mRootActor[Actor::Property::SIZE]          = Vector2(worldSize.GetWidth(), worldSize.GetHeight());
  mRootActor[Actor::Property::ANCHOR_POINT]  = Dali::AnchorPoint::CENTER;
  mRootActor[Actor::Property::PARENT_ORIGIN] = Dali::ParentOrigin::CENTER;

  // Initialize derived adaptor (and world)
  OnInitialize(transform, worldSize);
}

void PhysicsAdaptor::SetTimestep(float timestep)
{
  mPhysicsWorld->SetTimestep(timestep);
}

float PhysicsAdaptor::GetTimestep() const
{
  return mPhysicsWorld->GetTimestep();
}

Physics::PhysicsAdaptor::ScopedPhysicsAccessorPtr PhysicsAdaptor::GetPhysicsAccessor()
{
  return std::unique_ptr<Physics::PhysicsAdaptor::ScopedPhysicsAccessor>(new Physics::PhysicsAdaptor::ScopedPhysicsAccessor(*mPhysicsWorld.get()));
}

void PhysicsAdaptor::SetIntegrationState(Physics::PhysicsAdaptor::IntegrationState state)
{
  mPhysicsWorld->SetIntegrationState(state);
}

Physics::PhysicsAdaptor::IntegrationState PhysicsAdaptor::GetIntegrationState() const
{
  return mPhysicsWorld->GetIntegrationState();
}

void PhysicsAdaptor::SetDebugState(Physics::PhysicsAdaptor::DebugState state)
{
  mPhysicsWorld->SetDebugState(state);
}

Physics::PhysicsAdaptor::DebugState PhysicsAdaptor::GetDebugState() const
{
  return mPhysicsWorld->GetDebugState();
}

Dali::Actor PhysicsAdaptor::GetRootActor() const
{
  return mRootActor;
}

void PhysicsAdaptor::OnUpdateActors(Dali::UpdateProxy* updateProxy)
{
  for(auto&& actor : mPhysicsActors)
  {
    // Get position, orientation from physics world.
    Vector3 position = actor.second->GetActorPosition();
    updateProxy->BakePosition(actor.first, position);
    Quaternion rotation = actor.second->GetActorRotation();
    updateProxy->BakeOrientation(actor.first, rotation);
  }
}

void PhysicsAdaptor::Queue(std::function<void()> function)
{
  mPhysicsWorld->Queue(function);
}

void PhysicsAdaptor::CreateSyncPoint()
{
  mPhysicsWorld->CreateSyncPoint();
}

std::unique_ptr<PhysicsWorld>& PhysicsAdaptor::GetPhysicsWorld()
{
  return mPhysicsWorld;
}

} // namespace Dali::Toolkit::Physics::Internal
