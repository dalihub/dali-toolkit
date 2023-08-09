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
#include <dali-physics/internal/physics-world-impl.h>

// External Headers
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>
#include <btBulletCollisionCommon.h>
#include <iostream>
#include <map>
#include <memory>
#include <utility>

// Internal Headers
#include <dali/dali.h>
#include <dali/devel-api/common/stage-devel.h>
#include <dali/devel-api/update/frame-callback-interface.h>

namespace Dali::Toolkit::Physics::Internal
{
/**
 * FrameCallback implementation. Will run the OnUpdate method.
 */
class FrameCallback : public Dali::FrameCallbackInterface
{
public:
  /**
   * Constructor
   */
  explicit FrameCallback(PhysicsWorld& physicsWorld)
  : mPhysicsWorld(physicsWorld)
  {
  }

private:
  /**
   * Called each frame.
   * @param[in] updateProxy Used to set world matrix and size
   * @param[in] elapsedSeconds Time since last frame
   * @return Whether we should keep rendering.
   */
  bool Update(Dali::UpdateProxy& updateProxy, float elapsedSeconds) override
  {
    return mPhysicsWorld.OnUpdate(updateProxy, elapsedSeconds);
  }

private: // Member variables
  PhysicsWorld& mPhysicsWorld;
};

using NativeWorld = btDiscreteDynamicsWorld*;

std::unique_ptr<PhysicsWorld> PhysicsWorld::New(Dali::Actor rootActor, Dali::CallbackBase* updateCallback)
{
  std::unique_ptr<PhysicsWorld> world = std::make_unique<PhysicsWorld>(rootActor, updateCallback);
  world->Initialize();
  return world;
}

PhysicsWorld::PhysicsWorld(Dali::Actor rootActor, Dali::CallbackBase* updateCallback)
: mUpdateCallback(updateCallback),
  mRootActor(rootActor)
{
  Initialize();
}

void PhysicsWorld::Initialize(/*void* dynamicsWorld*/)
{
  // @todo Should enable developer to supply their own created DynamicsWorld.

  mCollisionConfiguration = new btDefaultCollisionConfiguration();
  mDispatcher             = new btCollisionDispatcher(mCollisionConfiguration);
  mBroadphase             = new btDbvtBroadphase();
  mSolver                 = new btSequentialImpulseConstraintSolver;
  mDynamicsWorld          = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);

  // Automatically start the frame callback. This means everything should
  // be accessed with a mutex lock, which is automatically locked when
  // ScopedAccessor is used.
  mFrameCallback = std::make_unique<FrameCallback>(*this);
  Dali::DevelStage::AddFrameCallback(Dali::Stage::GetCurrent(), *mFrameCallback, mRootActor);
  Dali::Stage::GetCurrent().KeepRendering(30); // @todo Remove!
}

Dali::Any PhysicsWorld::GetNative()
{
  return mDynamicsWorld;
}

PhysicsWorld::~PhysicsWorld()
{
  Dali::DevelStage::RemoveFrameCallback(Dali::Stage::GetCurrent(), *mFrameCallback);

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

Dali::Mutex& PhysicsWorld::GetMutex()
{
  return mMutex;
}

bool PhysicsWorld::OnUpdate(Dali::UpdateProxy& updateProxy, float elapsedSeconds)
{
  Dali::Mutex::ScopedLock lock(mMutex);

  // Process command queue
  if(mNotifySyncPoint != Dali::UpdateProxy::INVALID_SYNC &&
     mNotifySyncPoint == updateProxy.PopSyncPoint())
  {
    do
    {
      commandQueue.front()(); // Execute the queued methods
      commandQueue.pop();
    } while(!commandQueue.empty());

    mNotifySyncPoint = Dali::UpdateProxy::INVALID_SYNC;
  }

  // Perform as many integration steps as needed to handle elapsed time
  static float frameTime = 0;
  frameTime += elapsedSeconds;
  do
  {
    Integrate(mPhysicsTimeStep);
    frameTime -= mPhysicsTimeStep;
  } while(frameTime > 0);

  // Update the corresponding actors to their physics spaces
  if(mUpdateCallback)
  {
    Dali::CallbackBase::Execute(*mUpdateCallback, &updateProxy); // Don't care about actor update return
  }

  // @todo Check physics world to see if everything is at rest
  return true;
}

void PhysicsWorld::Integrate(float timestep)
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

void PhysicsWorld::SetTimestep(float timeStep)
{
  mPhysicsTimeStep = timeStep;
}

float PhysicsWorld::GetTimestep()
{
  return mPhysicsTimeStep;
}

void PhysicsWorld::Queue(std::function<void(void)> function)
{
  if(!mMutex.IsLocked()) // Annoyingly, the dali mutex scoped lock doesn't prevent relocking in the same thread.
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    commandQueue.push(function);
  }
  else
  {
    commandQueue.push(function);
  }
}

void PhysicsWorld::CreateSyncPoint()
{
  mNotifySyncPoint = Dali::DevelStage::NotifyFrameCallback(Dali::Stage::GetCurrent(), *mFrameCallback);
}

inline btVector3 ConvertVector(Dali::Vector3 vector)
{
  return btVector3(vector.x, vector.y, vector.z);
}

Dali::Any PhysicsWorld::HitTest(Dali::Vector3 rayFromWorld, Dali::Vector3 rayToWorld, Dali::Vector3& localPivot, float& distanceFromCamera)
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

void PhysicsWorld::SetIntegrationState(Physics::PhysicsAdaptor::IntegrationState state)
{
  mPhysicsIntegrateState = state;
}

Physics::PhysicsAdaptor::IntegrationState PhysicsWorld::GetIntegrationState()
{
  return mPhysicsIntegrateState;
}

void PhysicsWorld::SetDebugState(Physics::PhysicsAdaptor::DebugState state)
{
  mPhysicsDebugState = state;
}

Physics::PhysicsAdaptor::DebugState PhysicsWorld::GetDebugState()
{
  return mPhysicsDebugState;
}

} // namespace Dali::Toolkit::Physics::Internal
