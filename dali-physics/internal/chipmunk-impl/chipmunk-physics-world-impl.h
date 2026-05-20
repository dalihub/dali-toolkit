#pragma once

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <chipmunk/chipmunk.h>
#include <dali/public-api/common/unique-ptr.h>

// INTERNAL INCLUDES
#include <dali-physics/internal/chipmunk-impl/chipmunk-physics-debug-renderer.h>
#include <dali-physics/internal/physics-world-impl.h>

namespace Dali::Toolkit::Physics::Internal
{
class ChipmunkPhysicsWorld : public PhysicsWorld
{
public:
  static UniquePtr<PhysicsWorld> New(Dali::Actor rootActor, Dali::CallbackBase* updateCallback);

  ChipmunkPhysicsWorld(Dali::Actor rootActor, Dali::CallbackBase* updateCallback);
  ~ChipmunkPhysicsWorld() override;

  void OnInitialize(/*void* dynamicsWorld*/) override;

  Dali::Any GetNative() override;

  void Integrate(float timestep) override;

  Dali::Any HitTest(Dali::Vector3 rayFromWorld, Dali::Vector3 rayToWorld, Dali::Any nativeFilter, Dali::Vector3& localPivot, float& distanceFromCamera) override;

  /**
   * Set the debug renderer. PhysicsWorld will take ownership
   */
  void SetDebugRenderer(PhysicsDebugRenderer* renderer)
  {
    mDebugRenderer.Reset(renderer);
  }

  PhysicsDebugRenderer& GetDebugRenderer()
  {
    return *mDebugRenderer.Get();
  }

  bool HasDebugRenderer()
  {
    return mDebugRenderer.Get() != nullptr;
  }

private:
  cpSpace*                        mSpace{nullptr};
  UniquePtr<PhysicsDebugRenderer> mDebugRenderer;
};

} //namespace Dali::Toolkit::Physics::Internal
