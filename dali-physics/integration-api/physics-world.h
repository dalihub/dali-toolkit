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
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/object/any.h>

namespace Dali::Toolkit::Physics::Internal
{
class PhysicsWorld;
}

namespace Dali::Toolkit::Physics::Integration
{
/** Proxy to the physics world impl.
 *
 * Does NOT own physics world.
 * Copyable. Doesn't track lock.
 */
class DALI_TOOLKIT_API PhysicsWorld
{
public:
  /**
   * Constructor
   * @param[in] impl The physics world implementation class.
   */
  PhysicsWorld(Dali::Toolkit::Physics::Internal::PhysicsWorld* impl);

  /**
   * Destructor.
   * Does not destroy the physic world.
   */
  ~PhysicsWorld();

  /**
   * Copy Constructor
   */
  PhysicsWorld(const PhysicsWorld& rhs) = default;

  /**
   * Assignment operator
   */
  PhysicsWorld& operator=(const PhysicsWorld& rhs) = default;

  /**
   * Lock the physics world. The caller MUST unlock it afterwards, otherwise
   * the physics integration step will block, and DALi update thread will be locked.
   */
  void Lock();

  /**
   * Unlock the physics world.
   */
  void Unlock();

  /**
   * Get a handle to the native physics world / space
   *
   * This API should be wrapped with Lock/Unlock in any interop implementation
   * @return a pointer to the native world.
   */
  Dali::Any GetNative();

  /**
   * Hit test the physics world.
   *
   * This API should be wrapped with Lock/Unlock in any interop implementation
   * @param[in] rayFromWorld The origin in physics world space
   * @param[in] rayToWorld A point along the direction on the far side of the physics world
   * @param[in] nativeFilter a native body / shape filter
   * @param[out] localPivot The hit point local to the body
   * @param[out] distanceFromCamera The distance of the pick point from the camera
   * @return Either a pointer to the native body, or an empty value.
   */
  Dali::Any HitTest(Dali::Vector3 rayFromWorld, Dali::Vector3 rayToWorld, Dali::Any nativeFilter, Dali::Vector3& localPivot, float& distanceFromCamera);

  /**
   * Get the implementation pointer.
   *
   * @return the implementation pointer.
   */
  Dali::Toolkit::Physics::Internal::PhysicsWorld* GetImpl();

private:
  Dali::Toolkit::Physics::Internal::PhysicsWorld* impl;
};

} // namespace Dali::Toolkit::Physics::Integration
