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
#include <iostream>
#include <map>
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

inline btVector3 ConvertVector(Dali::Vector3 vector)
{
  return btVector3(vector.x, vector.y, vector.z);
}

} // namespace

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

namespace Internal
{
PhysicsAdaptorPtr PhysicsAdaptor::New(const Dali::Matrix& transform, Uint16Pair worldSize)
{
  PhysicsAdaptorPtr adaptor(new PhysicsAdaptor());
  adaptor->Initialize(transform, worldSize);
  return adaptor;
}

PhysicsAdaptor::PhysicsAdaptor()
: mSlotDelegate(this)
{
}

PhysicsAdaptor::~PhysicsAdaptor()
{
  // @todo Ensure physics bodies don't leak
}

void PhysicsAdaptor::Initialize(const Dali::Matrix& transform, Uint16Pair worldSize)
{
  mTransform        = transform;
  mInverseTransform = transform;
  mInverseTransform.Invert();
  mSize = worldSize;

  // Create an actor that can handle mouse events.
  // @todo Enable this to be fully configured / provided
  mRootActor                                 = Layer::New();
  mRootActor[Layer::Property::BEHAVIOR]      = Layer::LAYER_3D;
  mRootActor[Layer::Property::DEPTH_TEST]    = true;
  mRootActor[Actor::Property::SIZE]          = Vector2(mSize.GetWidth(), mSize.GetHeight());
  mRootActor[Actor::Property::ANCHOR_POINT]  = Dali::AnchorPoint::CENTER;
  mRootActor[Actor::Property::PARENT_ORIGIN] = Dali::ParentOrigin::CENTER;

  mPhysicsWorld = PhysicsWorld::New(mRootActor,
                                    Dali::MakeCallback(mSlotDelegate.GetSlot(),
                                                       &PhysicsAdaptor::OnUpdateActors));
}

void PhysicsAdaptor::SetTimestep(float timestep)
{
  mPhysicsWorld->SetTimestep(timestep);
}

float PhysicsAdaptor::GetTimestep()
{
  return mPhysicsWorld->GetTimestep();
}

Physics::PhysicsAdaptor::ScopedPhysicsAccessorPtr PhysicsAdaptor::GetPhysicsAccessor()
{
  return std::unique_ptr<Physics::PhysicsAdaptor::ScopedPhysicsAccessor>(new Physics::PhysicsAdaptor::ScopedPhysicsAccessor(*mPhysicsWorld.get()));
}

Layer PhysicsAdaptor::CreateDebugLayer(Dali::Window window)
{
  Layer debugLayer;

  auto renderTaskList = window.GetRenderTaskList();
  auto renderTask     = renderTaskList.GetTask(0);
  auto windowSize     = window.GetSize();

  debugLayer                                 = Layer::New();
  debugLayer[Actor::Property::ANCHOR_POINT]  = Dali::AnchorPoint::CENTER;
  debugLayer[Actor::Property::PARENT_ORIGIN] = Dali::ParentOrigin::CENTER;

  Constraint positionConstraint = Constraint::New<Vector3>(debugLayer, Actor::Property::POSITION, EqualToConstraint());
  positionConstraint.AddSource(Source(mRootActor, Actor::Property::POSITION));
  positionConstraint.Apply();
  Constraint sizeConstraint = Constraint::New<Vector2>(debugLayer, Actor::Property::SIZE, EqualToConstraint());
  sizeConstraint.AddSource(Source(mRootActor, Actor::Property::SIZE));
  sizeConstraint.Apply();

  mDebugRenderer                              = PhysicsDebugRenderer::New(windowSize.GetWidth(), windowSize.GetHeight(), renderTask.GetCameraActor(), this);
  mDebugActor                                 = DrawableActor::New(*(mDebugRenderer->GetCallback().get()));
  mDebugActor[Actor::Property::ANCHOR_POINT]  = Dali::AnchorPoint::CENTER;
  mDebugActor[Actor::Property::PARENT_ORIGIN] = Dali::ParentOrigin::CENTER;

  Constraint sizeConstraint2 = Constraint::New<Vector2>(mDebugActor, Actor::Property::SIZE, EqualToConstraint());
  sizeConstraint2.AddSource(ParentSource(Actor::Property::SIZE));
  sizeConstraint2.Apply();

  debugLayer.Add(mDebugActor);

  auto bulletWorld = mPhysicsWorld->GetNative().Get<btDiscreteDynamicsWorld*>();

  bulletWorld->setDebugDrawer(mDebugRenderer.get());
  mDebugRenderer->setDebugMode(btIDebugDraw::DBG_DrawWireframe |
                               btIDebugDraw::DBG_DrawContactPoints |
                               btIDebugDraw::DBG_DrawNormals);

  window.Add(debugLayer);
  return debugLayer;
}

void PhysicsAdaptor::SetTransformAndSize(const Dali::Matrix& transform, Uint16Pair worldSize)
{
  mTransform        = transform;
  mInverseTransform = transform;
  mInverseTransform.Invert();
  mSize = worldSize;

  GetRootActor()[Actor::Property::SIZE] = Vector3(worldSize.GetWidth(), worldSize.GetHeight(), 0);

  if(mDebugRenderer)
  {
    Actor layer                  = mDebugActor.GetParent();
    layer[Actor::Property::SIZE] = Vector3(worldSize);
    mDebugRenderer->UpdateWindowSize(worldSize);
  }
}

void PhysicsAdaptor::SetIntegrationState(Physics::PhysicsAdaptor::IntegrationState state)
{
  mPhysicsWorld->SetIntegrationState(state);
}

Physics::PhysicsAdaptor::IntegrationState PhysicsAdaptor::GetIntegrationState()
{
  return mPhysicsWorld->GetIntegrationState();
}

void PhysicsAdaptor::SetDebugState(Physics::PhysicsAdaptor::DebugState state)
{
  mPhysicsWorld->SetDebugState(state);
}

Physics::PhysicsAdaptor::DebugState PhysicsAdaptor::GetDebugState()
{
  return mPhysicsWorld->GetDebugState();
}

PhysicsActorPtr PhysicsAdaptor::AddActorBody(Dali::Actor actor, Dali::Any body)
{
  uint32_t     id     = static_cast<uint32_t>(actor.GetProperty<int>(Actor::Property::ID));
  btRigidBody* btBody = body.Get<btRigidBody*>();

  btBody->setUserIndex(id);
  mPhysicsActors.insert(std::make_pair(id, PhysicsActor::New(actor, body, *this)));
  actor[Actor::Property::PARENT_ORIGIN] = Dali::ParentOrigin::CENTER;
  actor[Actor::Property::ANCHOR_POINT]  = Dali::AnchorPoint::CENTER;
  mRootActor.Add(actor);
  return mPhysicsActors.at(id);
}

PhysicsActorPtr PhysicsAdaptor::GetPhysicsActor(Dali::Any body)
{
  btRigidBody* btBody = body.Get<btRigidBody*>();
  if(btBody)
  {
    int  id   = btBody->getUserIndex();
    auto iter = mPhysicsActors.find(id);
    if(iter != mPhysicsActors.end())
    {
      return iter->second;
    }
  }
  DALI_LOG_ERROR("Body not found in physics actors");
  return nullptr;
}

Dali::Actor PhysicsAdaptor::GetRootActor()
{
  return mRootActor;
}

// Convert a position from root actor local space to physics space
Vector3 PhysicsAdaptor::TranslateToPhysicsSpace(Vector3 vector)
{
  Vector4 position = mTransform * Vector4(vector.x, vector.y, vector.z, 1.0f);
  return Vector3(position);
}

// Convert a position from physics space to root actor local space
Vector3 PhysicsAdaptor::TranslateFromPhysicsSpace(Vector3 vector)
{
  Vector4 position = mInverseTransform * Vector4(vector.x, vector.y, vector.z, 1.0f);
  return Vector3(position);
}

Quaternion PhysicsAdaptor::TranslateToPhysicsSpace(Quaternion orientation)
{
  // Naive check for speed. Should pass scale in constructor/transform setter

  if(std::signbit(mTransform.AsFloat()[0])) // mirrored in x
  {
    return Quaternion(orientation.mVector.w, orientation.mVector.x, -orientation.mVector.y, -orientation.mVector.z);
  }
  else if(std::signbit(mTransform.AsFloat()[5])) // mirrored in y
  {
    return Quaternion(orientation.mVector.w, -orientation.mVector.x, orientation.mVector.y, -orientation.mVector.z);
  }
  else if(std::signbit(mTransform.AsFloat()[10])) // mirrored in z
  {
    return Quaternion(orientation.mVector.w, -orientation.mVector.x, -orientation.mVector.y, orientation.mVector.z);
  }

  // No mirror, so rotation is invariant.
  return orientation;
}

Quaternion PhysicsAdaptor::TranslateFromPhysicsSpace(Quaternion orientation)
{
  // Mirroring conversion is identical in both transforms
  return TranslateToPhysicsSpace(orientation);
}

// Convert a vector from dali space to physics space
Vector3 PhysicsAdaptor::ConvertVectorToPhysicsSpace(Vector3 vector)
{
  Vector4 otherVector(mTransform * Vector4(vector.x, vector.y, vector.z, 0.0f));
  return Vector3(otherVector);
}

// Convert a vector from physics space to root actor local space
Vector3 PhysicsAdaptor::ConvertVectorFromPhysicsSpace(Vector3 vector)
{
  Vector4 otherVector(mInverseTransform * Vector4(vector.x, vector.y, vector.z, 0.0f));
  return Vector3(otherVector);
}

void PhysicsAdaptor::BuildPickingRay(Vector3 origin, Vector3 direction, Dali::Vector3& rayFromWorld, Dali::Vector3& rayToWorld)
{
  rayFromWorld = TranslateToPhysicsSpace(origin);
  // direction from DALI touch is normalized.
  // Multiply it up so that it goes through the visible world. (@todo use Space config?)
  rayToWorld = TranslateToPhysicsSpace(origin + direction * 10000.0f);
}

Vector3 PhysicsAdaptor::ProjectPoint(Vector3 origin, Vector3 direction, float distance)
{
  Vector3 rayFromWorld = TranslateToPhysicsSpace(origin);
  Vector3 rayToWorld   = TranslateToPhysicsSpace(origin + direction * 10000.0f);

  Vector3 dir = rayToWorld - rayFromWorld;
  dir.Normalize();
  dir *= distance; // Should compute with scale?
  return (rayFromWorld + dir);
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

} // namespace Internal
} // namespace Dali::Toolkit::Physics
