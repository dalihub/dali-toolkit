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
#include <dali-physics/internal/bullet-impl/bullet-physics-adaptor-impl.h>

// External Headers
#include <btBulletDynamicsCommon.h>
#include <utility>

// Internal Headers
#include <dali-physics/internal/bullet-impl/bullet-physics-world-impl.h>
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
PhysicsAdaptorPtr CreateNewPhysicsAdaptor(const Dali::Matrix& transform, Uint16Pair worldSize)
{
  PhysicsAdaptorPtr adaptor(new BulletPhysicsAdaptor());
  adaptor->Initialize(transform, worldSize);
  return adaptor;
}

BulletPhysicsAdaptor::BulletPhysicsAdaptor()
: PhysicsAdaptor()
{
}

BulletPhysicsAdaptor::~BulletPhysicsAdaptor()
{
  // @todo Ensure physics bodies don't leak
}

void BulletPhysicsAdaptor::OnInitialize(const Dali::Matrix& transform, Uint16Pair worldSize)
{
  mTransform        = transform;
  mInverseTransform = transform;
  mInverseTransform.Invert();
  mSize = worldSize;

  mPhysicsWorld = BulletPhysicsWorld::New(mRootActor,
                                          Dali::MakeCallback(mSlotDelegate.GetSlot(),
                                                             &PhysicsAdaptor::OnUpdateActors));
}

Layer BulletPhysicsAdaptor::CreateDebugLayer(Dali::Window window)
{
  Layer debugLayer;

  auto renderTaskList = window.GetRenderTaskList();
  auto renderTask     = renderTaskList.GetTask(0);
  auto windowSize     = window.GetSize();

  debugLayer                                 = Layer::New();
  debugLayer[Actor::Property::NAME]          = "PhysicsDebugLayer";
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

void BulletPhysicsAdaptor::SetTransformAndSize(const Dali::Matrix& transform, Uint16Pair worldSize)
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

PhysicsActorPtr BulletPhysicsAdaptor::AddActorBody(Dali::Actor actor, Dali::Any body)
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

void BulletPhysicsAdaptor::RemoveActorBody(PhysicsActor& physicsActor)
{
  auto iter = mPhysicsActors.find(physicsActor.GetId());
  if(iter != mPhysicsActors.end())
  {
    mPhysicsActors.erase(iter);
  }
  Dali::Actor actor = mRootActor.FindChildById(physicsActor.GetId());
  if(actor)
  {
    actor.Unparent();
  }

  auto         body   = physicsActor.GetBody();
  btRigidBody* btBody = body.Get<btRigidBody*>();
  if(btBody)
  {
    btBody->setUserIndex(-1);
  }
}

PhysicsActorPtr BulletPhysicsAdaptor::GetPhysicsActor(Dali::Any body) const
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

// Convert a position from root actor local space to physics space
Vector3 BulletPhysicsAdaptor::TranslateToPhysicsSpace(Vector3 vector) const
{
  Vector4 position = mTransform * Vector4(vector.x, vector.y, vector.z, 1.0f);
  return Vector3(position);
}

// Convert a position from physics space to root actor local space
Vector3 BulletPhysicsAdaptor::TranslateFromPhysicsSpace(Vector3 vector) const
{
  Vector4 position = mInverseTransform * Vector4(vector.x, vector.y, vector.z, 1.0f);
  return Vector3(position);
}

Quaternion BulletPhysicsAdaptor::TranslateToPhysicsSpace(Quaternion orientation) const
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

Quaternion BulletPhysicsAdaptor::TranslateFromPhysicsSpace(Quaternion orientation) const
{
  // Mirroring conversion is identical in both transforms
  return TranslateToPhysicsSpace(orientation);
}

// Convert a vector from dali space to physics space
Vector3 BulletPhysicsAdaptor::ConvertVectorToPhysicsSpace(Vector3 vector) const
{
  Vector4 otherVector(mTransform * Vector4(vector.x, vector.y, vector.z, 0.0f));
  return Vector3(otherVector);
}

// Convert a vector from physics space to root actor local space
Vector3 BulletPhysicsAdaptor::ConvertVectorFromPhysicsSpace(Vector3 vector) const
{
  Vector4 otherVector(mInverseTransform * Vector4(vector.x, vector.y, vector.z, 0.0f));
  return Vector3(otherVector);
}

void BulletPhysicsAdaptor::BuildPickingRay(Vector3 origin, Vector3 direction, Dali::Vector3& rayFromWorld, Dali::Vector3& rayToWorld)
{
  rayFromWorld = TranslateToPhysicsSpace(origin);
  rayToWorld   = TranslateToPhysicsSpace(origin + direction * 10000.0f);
}

Vector3 BulletPhysicsAdaptor::ProjectPoint(Vector3 origin, Vector3 direction, float distance)
{
  Vector3 rayFromWorld = TranslateToPhysicsSpace(origin);
  Vector3 rayToWorld   = TranslateToPhysicsSpace(origin + direction * 10000.0f);

  Vector3 dir = rayToWorld - rayFromWorld;
  dir.Normalize();
  dir *= distance;
  return (rayFromWorld + dir);
}

} // namespace Dali::Toolkit::Physics::Internal
