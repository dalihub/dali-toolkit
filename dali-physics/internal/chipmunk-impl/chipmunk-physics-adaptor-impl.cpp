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
#include <dali-physics/internal/chipmunk-impl/chipmunk-physics-adaptor-impl.h>

// External Headers
#include <utility>

// Internal Headers
#include <dali-physics/internal/chipmunk-impl/chipmunk-physics-world-impl.h>
#include <dali/dali.h>
#include <dali/integration-api/debug.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, false, "LOG_PHYSICS");
#endif

inline cpVect ConvertVector(Dali::Vector3 vector)
{
  return cpv(vector.x, vector.y);
}

} // namespace

namespace Dali::Toolkit::Physics::Internal
{
PhysicsAdaptorPtr CreateNewPhysicsAdaptor(const Dali::Matrix& transform, Uint16Pair worldSize)
{
  PhysicsAdaptorPtr adaptor(new ChipmunkPhysicsAdaptor());
  adaptor->Initialize(transform, worldSize);
  return adaptor;
}

ChipmunkPhysicsAdaptor::ChipmunkPhysicsAdaptor()
: PhysicsAdaptor()
{
}

ChipmunkPhysicsAdaptor::~ChipmunkPhysicsAdaptor()
{
  // @todo Ensure physics bodies don't leak
}

void ChipmunkPhysicsAdaptor::OnInitialize(const Dali::Matrix& transform, Uint16Pair worldSize)
{
  mTransform        = transform;
  mInverseTransform = transform;
  mInverseTransform.Invert();
  mSize = worldSize;

  mPhysicsWorld = ChipmunkPhysicsWorld::New(mRootActor,
                                            Dali::MakeCallback(mSlotDelegate.GetSlot(),
                                                               &PhysicsAdaptor::OnUpdateActors));
}

Layer ChipmunkPhysicsAdaptor::CreateDebugLayer(Dali::Window window)
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

  auto world = static_cast<ChipmunkPhysicsWorld*>(mPhysicsWorld.get());

  std::unique_ptr<PhysicsDebugRenderer> debugRenderer = PhysicsDebugRenderer::New(windowSize.GetWidth(), windowSize.GetHeight(), renderTask.GetCameraActor(), this);

  mDebugActor = DrawableActor::New(*(debugRenderer->GetCallback().get()));
  world->SetDebugRenderer(debugRenderer.release());

  mDebugActor[Actor::Property::ANCHOR_POINT]  = Dali::AnchorPoint::CENTER;
  mDebugActor[Actor::Property::PARENT_ORIGIN] = Dali::ParentOrigin::CENTER;

  Constraint sizeConstraint2 = Constraint::New<Vector2>(mDebugActor, Actor::Property::SIZE, EqualToConstraint());
  sizeConstraint2.AddSource(ParentSource(Actor::Property::SIZE));
  sizeConstraint2.Apply();

  debugLayer.Add(mDebugActor);

  window.Add(debugLayer);
  return debugLayer;
}

void ChipmunkPhysicsAdaptor::SetTransformAndSize(const Dali::Matrix& transform, Uint16Pair worldSize)
{
  mTransform        = transform;
  mInverseTransform = transform;
  mInverseTransform.Invert();
  mSize = worldSize;

  GetRootActor()[Actor::Property::SIZE] = Vector3(worldSize.GetWidth(), worldSize.GetHeight(), 0);

  auto world = static_cast<ChipmunkPhysicsWorld*>(mPhysicsWorld.get());
  if(world->HasDebugRenderer())
  {
    Actor layer                  = mDebugActor.GetParent();
    layer[Actor::Property::SIZE] = Vector3(worldSize);
    world->GetDebugRenderer().UpdateWindowSize(worldSize);
  }
}

PhysicsActorPtr ChipmunkPhysicsAdaptor::AddActorBody(Dali::Actor actor, Dali::Any body)
{
  uint32_t id    = static_cast<uint32_t>(actor.GetProperty<int>(Actor::Property::ID));
  cpBody*  cBody = body.Get<cpBody*>();
  cpBodySetUserData(cBody, this);

  mPhysicsActors.insert(std::make_pair(id, PhysicsActor::New(actor, body, *this)));
  actor[Actor::Property::PARENT_ORIGIN] = Dali::ParentOrigin::CENTER;
  actor[Actor::Property::ANCHOR_POINT]  = Dali::AnchorPoint::CENTER;
  mRootActor.Add(actor);
  return mPhysicsActors.at(id);
}

void ChipmunkPhysicsAdaptor::RemoveActorBody(PhysicsActor& physicsActor)
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
  auto    body  = physicsActor.GetBody();
  cpBody* cBody = body.Get<cpBody*>();
  if(cBody)
  {
    cpBodySetUserData(cBody, nullptr);
  }
}

PhysicsActorPtr ChipmunkPhysicsAdaptor::GetPhysicsActor(Dali::Any body) const
{
  cpBody* cBody = body.Get<cpBody*>();
  if(cBody)
  {
    return reinterpret_cast<PhysicsActor*>(cpBodyGetUserData(cBody));
  }
  DALI_LOG_ERROR("Body not found in physics actors");
  return nullptr;
}

// Convert a position from root actor local space to physics space
Vector3 ChipmunkPhysicsAdaptor::TranslateToPhysicsSpace(Vector3 vector) const
{
  Vector4 position = mTransform * Vector4(vector.x, vector.y, vector.z, 1.0f);
  return Vector3(position);
}

// Convert a position from physics space to root actor local space
Vector3 ChipmunkPhysicsAdaptor::TranslateFromPhysicsSpace(Vector3 vector) const
{
  Vector4 position = mInverseTransform * Vector4(vector.x, vector.y, vector.z, 1.0f);
  return Vector3(position);
}

Quaternion ChipmunkPhysicsAdaptor::TranslateToPhysicsSpace(Quaternion orientation) const
{
  // Actors face outwards (+ve Z)
  // In DALi world, +ve angle about +ve Z is clockwise.
  // But, if physics is mirrored in Y axis, so +ve angle is anti-clockwise.

  // Compute angle about Z axis
  Vector3 axis;
  Radian  angle;
  orientation.ToAxisAngle(axis, angle);

  // Check if Transform matrix is mirrored in X xor Y
  if(std::signbit(mTransform.AsFloat()[0]) ^ std::signbit(mTransform.AsFloat()[5]))
  {
    return Quaternion(-angle, axis);
  }

  return Quaternion(angle, axis);
}

Quaternion ChipmunkPhysicsAdaptor::TranslateFromPhysicsSpace(Quaternion orientation) const
{
  // Mirroring conversion is identical in both transforms
  return TranslateToPhysicsSpace(orientation);
}

// Convert a vector from dali space to physics space
Vector3 ChipmunkPhysicsAdaptor::ConvertVectorToPhysicsSpace(Vector3 vector) const
{
  Vector4 otherVector(mTransform * Vector4(vector.x, vector.y, vector.z, 0.0f));
  return Vector3(otherVector);
}

// Convert a vector from physics space to root actor local space
Vector3 ChipmunkPhysicsAdaptor::ConvertVectorFromPhysicsSpace(Vector3 vector) const
{
  Vector4 otherVector(mInverseTransform * Vector4(vector.x, vector.y, vector.z, 0.0f));
  return Vector3(otherVector);
}

void ChipmunkPhysicsAdaptor::BuildPickingRay(Vector3 origin, Vector3 direction, Dali::Vector3& rayFromWorld, Dali::Vector3& rayToWorld)
{
  rayFromWorld = TranslateToPhysicsSpace(origin);
  rayToWorld   = TranslateToPhysicsSpace(origin); // rayToWorld is identical - there's no depth
}

Vector3 ChipmunkPhysicsAdaptor::ProjectPoint(Vector3 origin, Vector3 direction, float distance)
{
  // Ignore direction & distance.
  return TranslateToPhysicsSpace(origin);
}

} // namespace Dali::Toolkit::Physics::Internal
