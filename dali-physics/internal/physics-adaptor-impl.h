#ifndef DALI_TOOLKIT_PHYSICS_INTERNAL_ADAPTOR_H
#define DALI_TOOLKIT_PHYSICS_INTERNAL_ADAPTOR_H

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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or adaptoried.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>
#include <memory>
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali-physics/internal/physics-actor-impl.h>
#include <dali-physics/internal/physics-debug-renderer.h>
#include <dali-physics/internal/physics-world-impl.h>
#include <dali-physics/public-api/physics-adaptor.h>

namespace Dali::Toolkit::Physics
{
namespace Internal
{
class PhysicsAdaptor;
using PhysicsAdaptorPtr = IntrusivePtr<PhysicsAdaptor>;

class PhysicsAdaptor : public BaseObject
{
public:
  PhysicsAdaptor();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~PhysicsAdaptor() override;

  // Remove copy constructor and copy assignment
  PhysicsAdaptor(const PhysicsAdaptor& handle) = delete;
  PhysicsAdaptor& operator=(const PhysicsAdaptor& handle) = delete;

  static PhysicsAdaptorPtr New(const Dali::Matrix& transform, Uint16Pair size);

  /**
   * 2nd stage initialization
   */
  void Initialize(const Dali::Matrix& transform, Uint16Pair size);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::SetTimestep
   */
  void SetTimestep(float timestep);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::GetTimestep
   */
  float GetTimestep();

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::GetPhysicsAccessor
   */
  Physics::PhysicsAdaptor::ScopedPhysicsAccessorPtr GetPhysicsAccessor();

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::CreateDebugLayer
   */
  Dali::Layer CreateDebugLayer(Dali::Window window);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::TranslateToPhysicsSpace
   */
  Dali::Vector3 TranslateToPhysicsSpace(Dali::Vector3 vector);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::TranslateToPhysicsSpace
   */
  Dali::Quaternion TranslateToPhysicsSpace(Dali::Quaternion rotation);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::TranslateFromPhysicsSpace
   */
  Dali::Vector3 TranslateFromPhysicsSpace(Dali::Vector3 vector);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::TranslateFromPhysicsSpace
   */
  Dali::Quaternion TranslateFromPhysicsSpace(Quaternion rotation);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::ConvertVectorToPhysicsSpace
   */
  Dali::Vector3 ConvertVectorToPhysicsSpace(Dali::Vector3 vector);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::ConvertVectorFromPhysicsSpace
   */
  Dali::Vector3 ConvertVectorFromPhysicsSpace(Dali::Vector3 vector);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::SetTransformAndSize
   */
  void SetTransformAndSize(const Dali::Matrix& transform, Uint16Pair size);

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

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::AddActorBody
   */
  PhysicsActorPtr AddActorBody(Dali::Actor actor, Dali::Any body);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::GetPhysicsActor
   */
  PhysicsActorPtr GetPhysicsActor(Dali::Any body);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::GetRootActor
   */
  Dali::Actor GetRootActor();

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::BuildPickingRay
   */
  void BuildPickingRay(Dali::Vector3 origin, Dali::Vector3 direction, Dali::Vector3& rayFromWorld, Dali::Vector3& rayToWorld);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::ProjectPoint
   */
  Dali::Vector3 ProjectPoint(Dali::Vector3 origin, Dali::Vector3 direction, float distance);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::Queue
   */
  void Queue(std::function<void(void)> function);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::Queue
   */
  void CreateSyncPoint();

private:
  /**
   * Handle the update of all of the known bound actors
   */
  void OnUpdateActors(Dali::UpdateProxy* updateProxy);

private:
  std::unique_ptr<PhysicsWorld>                 mPhysicsWorld;
  std::unordered_map<uint32_t, PhysicsActorPtr> mPhysicsActors;
  Dali::Actor                                   mRootActor;
  Dali::Actor                                   mDebugActor;

  Dali::Matrix     mTransform;
  Dali::Matrix     mInverseTransform;
  Dali::Uint16Pair mSize;

  std::unique_ptr<PhysicsDebugRenderer> mDebugRenderer;
  Dali::SlotDelegate<PhysicsAdaptor>    mSlotDelegate;
};

} //namespace Internal

inline Internal::PhysicsAdaptor& GetImplementation(Dali::Toolkit::Physics::PhysicsAdaptor& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Physics adaptor handle is empty");
  BaseObject& object = handle.GetBaseObject();
  return static_cast<Internal::PhysicsAdaptor&>(object);
}

inline const Internal::PhysicsAdaptor& GetImplementation(const Dali::Toolkit::Physics::PhysicsAdaptor& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Physics adaptor handle is empty");
  const BaseObject& object = handle.GetBaseObject();
  return static_cast<const Internal::PhysicsAdaptor&>(object);
}

} // namespace Dali::Toolkit::Physics

#endif //DALI_TOOLKIT_PHYSICS_INTERNAL_ADAPTOR_H
