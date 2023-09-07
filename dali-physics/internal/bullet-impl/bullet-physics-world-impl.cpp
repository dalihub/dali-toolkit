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
#include <dali-physics/internal/bullet-impl/bullet-physics-world-impl.h>

// External Headers
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>
#include <btBulletCollisionCommon.h>
#include <memory>

// Internal Headers
#include <dali/dali.h>
#include <dali/devel-api/common/stage-devel.h>
#include <dali/devel-api/update/frame-callback-interface.h>

namespace Dali::Toolkit::Physics::Internal
{
std::unique_ptr<PhysicsWorld> BulletPhysicsWorld::New(Dali::Actor rootActor, Dali::CallbackBase* updateCallback)
{
  std::unique_ptr<BulletPhysicsWorld> world = std::make_unique<BulletPhysicsWorld>(rootActor, updateCallback);
  world->Initialize();
  return world;
}

BulletPhysicsWorld::BulletPhysicsWorld(Dali::Actor rootActor, Dali::CallbackBase* updateCallback)
: PhysicsWorld(rootActor, updateCallback)
{
}

void BulletPhysicsWorld::OnInitialize(/*void* dynamicsWorld*/)
{
  // @todo Should enable developer to supply their own created DynamicsWorld.

  mCollisionConfiguration = new btDefaultCollisionConfiguration();
  mDispatcher             = new btCollisionDispatcher(mCollisionConfiguration);
  mBroadphase             = new btDbvtBroadphase();
  mSolver                 = new btSequentialImpulseConstraintSolver;
  mDynamicsWorld          = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
}

BulletPhysicsWorld::~BulletPhysicsWorld()
{
  Dali::Mutex::ScopedLock lock(mMutex);

  if(mDynamicsWorld)
  {
    int i;
    for(i = mDynamicsWorld->getNumConstraints() - 1; i >= 0; i--)
    {
      mDynamicsWorld->removeConstraint(mDynamicsWorld->getConstraint(i));
    }
    for(i = mDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
      btCollisionObject* obj  = mDynamicsWorld->getCollisionObjectArray()[i];
      btRigidBody*       body = btRigidBody::upcast(obj);
      if(body && body->getMotionState())
      {
        delete body->getMotionState();
      }
      mDynamicsWorld->removeCollisionObject(obj);
      delete obj;
    }
  }

  /*
  for (int j = 0; j < m_collisionShapes.size(); j++)
  {
    btCollisionShape* shape = mCollisionShapes[j];
    delete shape;
  }
  mCollisionShapes.clear();
  */

  delete mDynamicsWorld;
  delete mSolver;
  delete mBroadphase;
  delete mDispatcher;
  delete mCollisionConfiguration;
}

Dali::Any BulletPhysicsWorld::GetNative()
{
  return mDynamicsWorld;
}

void BulletPhysicsWorld::Integrate(float timestep)
{
  if(mPhysicsIntegrateState == Physics::PhysicsAdaptor::IntegrationState::ON)
  {
    mDynamicsWorld->stepSimulation(timestep);
  }
  if(mDynamicsWorld->getDebugDrawer() && mPhysicsDebugState == Physics::PhysicsAdaptor::DebugState::ON)
  {
    mDynamicsWorld->debugDrawWorld();
  }
}

inline btVector3 ConvertVector(Dali::Vector3 vector)
{
  return btVector3(vector.x, vector.y, vector.z);
}

Dali::Any BulletPhysicsWorld::HitTest(Dali::Vector3 rayFromWorld, Dali::Vector3 rayToWorld, Dali::Any nativeFilter, Dali::Vector3& localPivot, float& distanceFromCamera)
{
  btRigidBody* hitBody{nullptr};

  btVector3                                  origin = ConvertVector(rayFromWorld);
  btVector3                                  end    = ConvertVector(rayToWorld);
  btCollisionWorld::ClosestRayResultCallback rayResultCallback(origin, end);
  rayResultCallback.m_flags |= btTriangleRaycastCallback::kF_UseGjkConvexCastRaytest;

  mDynamicsWorld->rayTest(origin, end, rayResultCallback);
  if(rayResultCallback.hasHit())
  {
    auto         pickPos = rayResultCallback.m_hitPointWorld;
    btRigidBody* body    = (btRigidBody*)btRigidBody::upcast(rayResultCallback.m_collisionObject);
    if(body)
    {
      if(!(body->isStaticObject() || body->isKinematicObject()))
      {
        hitBody            = body; // Found a dynamic body.
        distanceFromCamera = (pickPos - origin).length();

        btVector3 pivot = body->getCenterOfMassTransform().inverse() * pickPos;
        localPivot.x    = pivot.x();
        localPivot.y    = pivot.y();
        localPivot.z    = pivot.z();
      }
    }
  }
  Dali::Any bodyPtr;
  if(hitBody)
  {
    bodyPtr = hitBody;
  }

  return bodyPtr;
}

} // namespace Dali::Toolkit::Physics::Internal
