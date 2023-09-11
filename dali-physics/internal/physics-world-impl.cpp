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

PhysicsWorld::PhysicsWorld(Dali::Actor rootActor, Dali::CallbackBase* updateCallback)
: mUpdateCallback(updateCallback),
  mRootActor(rootActor)
{
}

void PhysicsWorld::Initialize()
{
  // Call derived class's initializer
  OnInitialize();

  // Automatically start the frame callback. This means everything should
  // be accessed with a mutex lock, which is automatically locked when
  // ScopedAccessor is used.
  mFrameCallback = std::make_unique<FrameCallback>(*this);
  Dali::DevelStage::AddFrameCallback(Dali::Stage::GetCurrent(), *mFrameCallback, mRootActor);
  Dali::Stage::GetCurrent().KeepRendering(30); // @todo Remove!
}

PhysicsWorld::~PhysicsWorld()
{
  // Derived class's destructor should clean down physics objects under mutex lock
  // On completion, can remove the callback.

  Dali::DevelStage::RemoveFrameCallback(Dali::Stage::GetCurrent(), *mFrameCallback);
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
    while(!commandQueue.empty())
    {
      commandQueue.front()(); // Execute the queued methods
      commandQueue.pop();
    }

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
