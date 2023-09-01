#pragma once

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
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/update/frame-callback-interface.h>
#include <dali/devel-api/update/update-proxy.h>

#include <dali-physics/internal/physics-world-impl.h>
#include <dali-physics/public-api/physics-adaptor.h>

#include <btBulletDynamicsCommon.h>

namespace Dali::Toolkit::Physics::Internal
{
class PhysicsWorld;
class FrameCallback;

class BulletPhysicsWorld : public PhysicsWorld
{
public:
  static std::unique_ptr<PhysicsWorld> New(Dali::Actor rootActor, Dali::CallbackBase* updateCallback);

  BulletPhysicsWorld(Dali::Actor rootActor, Dali::CallbackBase* updateCallback);
  ~BulletPhysicsWorld();

  void OnInitialize(/*void* dynamicsWorld*/) override;

  Dali::Any GetNative() override;

  /**
   * Hit test the physics world and return the nearest body.
   *
   * @param[in] rayFromWorld The origin in physics world space
   * @param[in] rayToWorld A point along the direction on the far side of the physics world
   * @param[out] localPivot The hit point local to the body
   * @param[out] distanceFromCamera The distance of the pick point from the camera
   * @return Empty value if no dynamic body found, otherwise a valid ptr to the hit body.
   */
  Dali::Any HitTest(Dali::Vector3 rayFromWorld, Dali::Vector3 rayToWorld, Dali::Vector3& localPivot, float& distanceFromCamera) override;

  void Integrate(float timestep) override;

private:
  btDiscreteDynamicsWorld*             mDynamicsWorld{nullptr};
  btCollisionDispatcher*               mDispatcher{nullptr};
  btDefaultCollisionConfiguration*     mCollisionConfiguration{nullptr};
  btBroadphaseInterface*               mBroadphase{nullptr};
  btSequentialImpulseConstraintSolver* mSolver{nullptr};
};

} // namespace Dali::Toolkit::Physics::Internal
