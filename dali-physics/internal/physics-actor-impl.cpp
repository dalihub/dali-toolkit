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

namespace Dali::Toolkit::Physics::Internal
{
namespace
{
inline btVector3 fromVec3(Dali::Vector3 vec3)
{
  return btVector3(vec3.x, vec3.y, vec3.z);
}
inline Dali::Vector3 toVec3(btVector3 vec3)
{
  return Dali::Vector3(vec3.x(), vec3.y(), vec3.z());
}

inline btQuaternion fromQuat(Dali::Quaternion q)
{
  return btQuaternion(q.mVector.x, q.mVector.y, q.mVector.z, q.mVector.w);
}
inline Dali::Quaternion toQuat(btQuaternion q)
{
  return Dali::Quaternion(q.w(), q.x(), q.y(), q.z());
}

} //Anonymous namespace

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
  // RegisterObject?
}

void PhysicsActor::AsyncSetPhysicsPosition(Dali::Vector3 actorPosition)
{
  // Queue task
  btRigidBody* body = mBody.Get<btRigidBody*>();
  auto         pos  = fromVec3(mAdaptor.TranslateToPhysicsSpace(actorPosition));
  mAdaptor.Queue([body, pos] { body->getWorldTransform().setOrigin(pos); });
}

void PhysicsActor::AsyncSetPhysicsRotation(Dali::Quaternion rotation)
{
  // Queue task
  btRigidBody* body = mBody.Get<btRigidBody*>();
  auto         q    = fromQuat(mAdaptor.TranslateToPhysicsSpace(rotation));
  mAdaptor.Queue([body, q]() { body->getWorldTransform().setRotation(q); });
}

Dali::Vector3 PhysicsActor::GetPhysicsPosition() const
{
  btRigidBody* body = mBody.Get<btRigidBody*>();
  return toVec3(body->getWorldTransform().getOrigin());
}

Dali::Quaternion PhysicsActor::GetPhysicsRotation() const
{
  btRigidBody* body = mBody.Get<btRigidBody*>();
  return toQuat(body->getWorldTransform().getRotation());
}

Dali::Vector3 PhysicsActor::GetActorPosition() const
{
  btRigidBody*       body      = mBody.Get<btRigidBody*>();
  const btTransform& transform = body->getWorldTransform();
  const btVector3&   position  = transform.getOrigin();
  return mAdaptor.TranslateFromPhysicsSpace(Vector3(position.getX(), position.getY(), position.getZ()));
}

Dali::Quaternion PhysicsActor::GetActorRotation() const
{
  btRigidBody*       body      = mBody.Get<btRigidBody*>();
  const btTransform& transform = body->getWorldTransform();
  btQuaternion       q         = transform.getRotation();
  return mAdaptor.TranslateFromPhysicsSpace(Quaternion(q.w(), q.x(), q.y(), q.z()));

}

} // namespace Dali::Toolkit::Physics::Internal
