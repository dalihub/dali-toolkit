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
#include <dali-physics/internal/physics-actor-impl.h>
#include <dali-physics/internal/physics-adaptor-impl.h>

#include <chipmunk/chipmunk.h>

namespace
{
inline cpVect fromVec3(Dali::Vector3 vec3)
{
  return cpv(vec3.x, vec3.y);
}

inline Dali::Vector3 toVec3(cpVect vec)
{
  return Dali::Vector3(vec.x, vec.y, 0.0f);
}

} //Anonymous namespace

namespace Dali::Toolkit::Physics::Internal
{
PhysicsActorPtr PhysicsActor::New(Dali::Actor actor, Dali::Any body, Dali::Toolkit::Physics::Internal::PhysicsAdaptor& adaptor)
{
  PhysicsActorPtr physicsActor(new Internal::PhysicsActor(actor, body, adaptor));
  physicsActor->Initialize();
  return physicsActor;
}

PhysicsActor::PhysicsActor(Dali::Actor actor, Dali::Any body, PhysicsAdaptor& adaptor)
: mAdaptor(adaptor),
  mActorId(actor.GetProperty<int>(Dali::Actor::Property::ID)),
  mBody(body)
{
}

PhysicsActor::~PhysicsActor() = default;

void PhysicsActor::Initialize(void)
{
  cpBodySetUserData2(mBody.Get<cpBody*>(), this);

  // RegisterObject?
}

void PhysicsActor::AsyncSetPhysicsPosition(Dali::Vector3 actorPosition)
{
  // Queue task
  cpBody* body = mBody.Get<cpBody*>();
  cpVect  pos  = fromVec3(mAdaptor.TranslateToPhysicsSpace(actorPosition));
  mAdaptor.Queue([body, pos] { cpBodySetPosition(body, pos); });
}

void PhysicsActor::AsyncSetPhysicsRotation(Dali::Quaternion rotation)
{
  // Queue task
  cpBody* body = mBody.Get<cpBody*>();
  auto    q    = mAdaptor.TranslateToPhysicsSpace(rotation);
  Vector3 axis;
  Radian  angle;
  q.ToAxisAngle(axis, angle);
  mAdaptor.Queue([body, angle]() { cpBodySetAngle(body, angle); });
}

Dali::Vector3 PhysicsActor::GetPhysicsPosition() const
{
  cpBody* body = mBody.Get<cpBody*>();
  return toVec3(cpBodyGetPosition(body));
}

Dali::Quaternion PhysicsActor::GetPhysicsRotation() const
{
  cpBody* body  = mBody.Get<cpBody*>();
  cpFloat angle = cpBodyGetAngle(body);
  return Quaternion(Radian(angle), Vector3::ZAXIS);
}

Dali::Vector3 PhysicsActor::GetActorPosition() const
{
  cpBody* body       = mBody.Get<cpBody*>();
  cpVect  cpPosition = cpBodyGetPosition(body);
  return mAdaptor.TranslateFromPhysicsSpace(Vector3(cpPosition.x, cpPosition.y, 0.0f));
}

Dali::Quaternion PhysicsActor::GetActorRotation() const
{
  cpBody* body  = mBody.Get<cpBody*>();
  cpFloat angle = cpBodyGetAngle(body);
  return mAdaptor.TranslateFromPhysicsSpace(Quaternion(Radian(angle), Vector3::ZAXIS));
}

} // namespace Dali::Toolkit::Physics::Internal
