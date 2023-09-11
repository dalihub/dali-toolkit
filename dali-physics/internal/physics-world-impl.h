#ifndef DALI_TOOLKIT_PHYSICS_INTERNAL_PHYSICS_WORLD_H
#define DALI_TOOLKIT_PHYSICS_INTERNAL_PHYSICS_WORLD_H

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

#include <dali/dali.h>
#include <dali/devel-api/update/frame-callback-interface.h>
#include <dali/devel-api/update/update-proxy.h>

#include <dali-physics/public-api/physics-adaptor.h>

#include <functional>
#include <mutex>
#include <queue>

namespace Dali::Toolkit::Physics::Internal
{
class PhysicsWorld;
class FrameCallback;

/**
 * Abstract class that handles the update frame callback, queuing and calling
 * functions before the integration step ; calling the integration step,
 * and owning the mutex for the update callback.
 *
 * Implementing classes should also hold the physics world.
 */
class PhysicsWorld
{
public:
  /**
   * Create a new physics world.
   */
  static std::unique_ptr<PhysicsWorld> New(Dali::Actor rootActor, Dali::CallbackBase* updateCallback);

  /**
   * Constructor which takes the root actor and a callback from the PhysicsAdaptor
   * @param[in] rootActor The root actor that physics actors will be added to
   * @param[in] updateCallback A callback from the PhysicsAdaptor which updates the physics actors after the integration step
   */
  PhysicsWorld(Dali::Actor rootActor, Dali::CallbackBase* updateCallback);

  /**
   * Virtual destructor.
   * Note, removes the frame callback.
   */
  virtual ~PhysicsWorld();

  /**
   * Initialize derived classes and creates the frame callback
   */
  void Initialize();

  /**
   * Initialize the derived class
   */
  virtual void OnInitialize() = 0;

  /**
   * Get the native physics world / space.
   * @return A pointer to the physics world / space
   */
  virtual Dali::Any GetNative() = 0;

  /**
   * Set how long the integration should take.
   * @param[in] timestep The length of time that the physics integration should take.
   */
  void SetTimestep(float timestep);

  /**
   * Get the current physics integration timestep
   * @return the current physics integration timestep
   */
  float GetTimestep();

  /**
   * Lock the mutex.
   */
  void Lock();

  /**
   * Unlock the mutex
   */
  void Unlock();

  class ScopedLock
  {
  public:
    ScopedLock(PhysicsWorld& world)
    : mWorld(world)
    {
      mWorld.Lock();
    }
    ~ScopedLock()
    {
      mWorld.Unlock();
    }
    PhysicsWorld& mWorld;
  };

  /**
   * Queue a function for execution in the update thread, prior to the physics integration.
   * Enables syncronization of DALi properties and physics controlled properties.
   */
  void Queue(std::function<void(void)> function);

  /**
   * Create a sync point for queued functions.
   *
   * Ensures that any queued functions are processed after this sync
   * point is seen in the Update::FrameCallback, which will be in the
   * same frame as any other DALi properties set during this event
   * handler invocation.
   *
   * @param[in] None
   */
  void CreateSyncPoint();

  /**
   * Hit test the physics world and return the nearest body.
   *
   * @param[in] rayFromWorld The origin in physics world space
   * @param[in] rayToWorld A point along the direction on the far side of the physics world
   * @param[in] nativeFilter a native body / shape filter
   * @param[out] localPivot The hit point local to the body
   * @param[out] distanceFromCamera The distance of the pick point from the camera
   * @return Empty value if no dynamic body found, otherwise a valid ptr to the hit body.
   */
  virtual Dali::Any HitTest(Dali::Vector3 rayFromWorld, Dali::Vector3 rayToWorld, Dali::Any nativeFilter, Dali::Vector3& localPivot, float& distanceFromCamera) = 0;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::SetIntegrationState
   */
  void SetIntegrationState(Physics::PhysicsAdaptor::IntegrationState state);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::GetIntegrationState
   */
  Physics::PhysicsAdaptor::IntegrationState GetIntegrationState();

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::SetDebugState
   */
  void SetDebugState(Physics::PhysicsAdaptor::DebugState state);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::GetDebugState
   */
  Physics::PhysicsAdaptor::DebugState GetDebugState();

public:
  bool OnUpdate(Dali::UpdateProxy& updateProxy, float elapsedSeconds);

protected:
  virtual void Integrate(float timestep) = 0;

protected:
  std::mutex                            mMutex;
  std::queue<std::function<void(void)>> commandQueue;
  Dali::UpdateProxy::NotifySyncPoint    mNotifySyncPoint{Dali::UpdateProxy::INVALID_SYNC};
  Dali::CallbackBase*                   mUpdateCallback{nullptr};
  std::unique_ptr<FrameCallback>        mFrameCallback;
  Dali::Actor                           mRootActor;

  float                                     mPhysicsTimeStep{1.0 / 180.0};
  Physics::PhysicsAdaptor::IntegrationState mPhysicsIntegrateState{Physics::PhysicsAdaptor::IntegrationState::ON};
  Physics::PhysicsAdaptor::DebugState       mPhysicsDebugState{Physics::PhysicsAdaptor::DebugState::OFF};
};

} // namespace Dali::Toolkit::Physics::Internal

#endif //DALI_TOOLKIT_PHYSICS_INTERNAL_PHYSICS_WORLD_H
