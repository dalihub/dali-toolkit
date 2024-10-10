#pragma once

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or adaptoried.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali-physics/internal/bullet-impl/bullet-physics-debug-renderer.h>
#include <dali-physics/internal/physics-adaptor-impl.h>

namespace Dali::Toolkit::Physics::Internal
{
class BulletPhysicsAdaptor : public PhysicsAdaptor
{
public:
  BulletPhysicsAdaptor();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~BulletPhysicsAdaptor() override;

  // Remove copy constructor and copy assignment
  BulletPhysicsAdaptor(const PhysicsAdaptor& handle) = delete;
  BulletPhysicsAdaptor& operator=(const PhysicsAdaptor& handle) = delete;

  static PhysicsAdaptorPtr New(const Dali::Matrix& transform, Uint16Pair size);

  /**
   * 2nd stage initialization
   */
  void Initialize(const Dali::Matrix& transform, Uint16Pair size);
  void OnInitialize(const Dali::Matrix& transform, Uint16Pair size) override;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::CreateDebugLayer
   */
  Dali::Layer CreateDebugLayer(Dali::Window window) override;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::SetTransformAndSize
   */
  void SetTransformAndSize(const Dali::Matrix& transform, Uint16Pair size) override;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::TranslateToPhysicsSpace
   */
  Dali::Vector3 TranslateToPhysicsSpace(Dali::Vector3 vector) const override;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::TranslateToPhysicsSpace
   */
  Dali::Quaternion TranslateToPhysicsSpace(Dali::Quaternion rotation) const override;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::TranslateFromPhysicsSpace
   */
  Dali::Vector3 TranslateFromPhysicsSpace(Dali::Vector3 vector) const override;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::TranslateFromPhysicsSpace
   */
  Dali::Quaternion TranslateFromPhysicsSpace(Quaternion rotation) const override;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::ConvertVectorToPhysicsSpace
   */
  Dali::Vector3 ConvertVectorToPhysicsSpace(Dali::Vector3 vector) const override;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::ConvertVectorFromPhysicsSpace
   */
  Dali::Vector3 ConvertVectorFromPhysicsSpace(Dali::Vector3 vector) const override;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::AddActorBody
   */
  PhysicsActorPtr AddActorBody(Dali::Actor actor, Dali::Any body) override;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::RemoveActorBody
   */
  void RemoveActorBody(PhysicsActor& physicsActor) override;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::GetPhysicsActor
   */
  PhysicsActorPtr GetPhysicsActor(Dali::Any body) const override;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::BuildPickingRay
   */
  void BuildPickingRay(Dali::Vector3 origin, Dali::Vector3 direction, Dali::Vector3& rayFromWorld, Dali::Vector3& rayToWorld) override;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::ProjectPoint
   */
  Dali::Vector3 ProjectPoint(Dali::Vector3 origin, Dali::Vector3 direction, float distance) override;

private:
  Actor                                 mDebugActor;
  std::unique_ptr<PhysicsDebugRenderer> mDebugRenderer;
};

} // namespace Dali::Toolkit::Physics::Internal
