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

#include <dali-physics/internal/physics-world-impl.h>

#include <chipmunk/chipmunk.h>
#include <dali-physics/internal/chipmunk-impl/chipmunk-physics-debug-renderer.h>
#include <memory>

namespace Dali::Toolkit::Physics::Internal
{
class ChipmunkPhysicsWorld : public PhysicsWorld
{
public:
  static std::unique_ptr<PhysicsWorld> New(Dali::Actor rootActor, Dali::CallbackBase* updateCallback);

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
    mDebugRenderer.reset(renderer);
  }

  PhysicsDebugRenderer& GetDebugRenderer()
  {
    return *mDebugRenderer.get();
  }

  bool HasDebugRenderer()
  {
    return mDebugRenderer.get() != nullptr;
  }

private:
  cpSpace*                              mSpace{nullptr};
  std::unique_ptr<PhysicsDebugRenderer> mDebugRenderer;
};

} //namespace Dali::Toolkit::Physics::Internal
