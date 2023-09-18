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
#include <dali-physics/internal/physics-world-impl.h>
#include <dali-physics/public-api/physics-adaptor.h>

namespace Dali::Toolkit::Physics
{
namespace Internal
{
class PhysicsAdaptor;
class PhysicsDebugRenderer;

using PhysicsAdaptorPtr = IntrusivePtr<PhysicsAdaptor>;

// Declaration of factory function, implemented by derived class
PhysicsAdaptorPtr CreateNewPhysicsAdaptor(const Dali::Matrix& transform, Uint16Pair worldSize);

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

  /**
   * 2nd stage initialization
   */
  void         Initialize(const Dali::Matrix& transform, Uint16Pair size);
  virtual void OnInitialize(const Dali::Matrix& transform, Uint16Pair size) = 0;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::SetTimestep
   */
  void SetTimestep(float timestep);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::GetTimestep
   */
  float GetTimestep() const;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::GetPhysicsAccessor
   */
  Physics::PhysicsAdaptor::ScopedPhysicsAccessorPtr GetPhysicsAccessor();

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::CreateDebugLayer
   */
  virtual Dali::Layer CreateDebugLayer(Dali::Window window) = 0;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::TranslateToPhysicsSpace
   */
  virtual Dali::Vector3 TranslateToPhysicsSpace(Dali::Vector3 vector) const = 0;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::TranslateToPhysicsSpace
   */
  virtual Dali::Quaternion TranslateToPhysicsSpace(Dali::Quaternion rotation) const = 0;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::TranslateFromPhysicsSpace
   */
  virtual Dali::Vector3 TranslateFromPhysicsSpace(Dali::Vector3 vector) const = 0;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::TranslateFromPhysicsSpace
   */
  virtual Dali::Quaternion TranslateFromPhysicsSpace(Quaternion rotation) const = 0;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::ConvertVectorToPhysicsSpace
   */
  virtual Dali::Vector3 ConvertVectorToPhysicsSpace(Dali::Vector3 vector) const = 0;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::ConvertVectorFromPhysicsSpace
   */
  virtual Dali::Vector3 ConvertVectorFromPhysicsSpace(Dali::Vector3 vector) const = 0;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::SetTransformAndSize
   */
  virtual void SetTransformAndSize(const Dali::Matrix& transform, Uint16Pair size) = 0;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::SetIntegrationState
   */
  void SetIntegrationState(Physics::PhysicsAdaptor::IntegrationState state);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::GetIntegrationState
   */
  Physics::PhysicsAdaptor::IntegrationState GetIntegrationState() const;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::SetDebugState
   */
  void SetDebugState(Physics::PhysicsAdaptor::DebugState state);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::GetDebugState
   */
  Physics::PhysicsAdaptor::DebugState GetDebugState() const;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::AddActorBody
   */
  virtual PhysicsActorPtr AddActorBody(Dali::Actor actor, Dali::Any body) = 0;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::RemoveActorBody
   */
  virtual void RemoveActorBody(PhysicsActor& physicsActor) = 0;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::GetPhysicsActor
   */
  virtual PhysicsActorPtr GetPhysicsActor(Dali::Any body) const = 0;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::GetRootActor
   */
  Dali::Actor GetRootActor() const;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::BuildPickingRay
   */
  virtual void BuildPickingRay(Dali::Vector3 origin, Dali::Vector3 direction, Dali::Vector3& rayFromWorld, Dali::Vector3& rayToWorld) = 0;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::ProjectPoint
   */
  virtual Dali::Vector3 ProjectPoint(Dali::Vector3 origin, Dali::Vector3 direction, float distance) = 0;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::Queue
   */
  void Queue(std::function<void(void)> function);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsAdaptor::Queue
   */
  void CreateSyncPoint();

  /**
   * Handle the update of all of the known bound actors
   */
  void OnUpdateActors(Dali::UpdateProxy* updateProxy);

  std::unique_ptr<PhysicsWorld>& GetPhysicsWorld();

protected:
  std::unique_ptr<PhysicsWorld>                 mPhysicsWorld;
  std::unordered_map<uint32_t, PhysicsActorPtr> mPhysicsActors;
  Dali::Actor                                   mRootActor;

  Dali::Matrix     mTransform;
  Dali::Matrix     mInverseTransform;
  Dali::Uint16Pair mSize;

  Dali::SlotDelegate<PhysicsAdaptor> mSlotDelegate;
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
