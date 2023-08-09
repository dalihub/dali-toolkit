#ifndef DALI_TOOLKIT_PHYSICS_INTERNAL_ACTOR_H
#define DALI_TOOLKIT_PHYSICS_INTERNAL_ACTOR_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/object/any.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-physics/public-api/physics-actor.h>
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali::Toolkit::Physics
{
namespace Internal
{
class PhysicsActor;
class PhysicsAdaptor;

using PhysicsActorPtr = Dali::IntrusivePtr<PhysicsActor>;

class DALI_TOOLKIT_API PhysicsActor : public Dali::BaseObject
{
public:
  PhysicsActor(Dali::Actor actor, Dali::Any body, PhysicsAdaptor& adaptor);
  ~PhysicsActor() override;
  PhysicsActor(const PhysicsActor& handle) = delete;
  PhysicsActor& operator=(const PhysicsActor& handle) = delete;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsActor::New()
   */
  static PhysicsActorPtr New(Dali::Actor actor, Dali::Any body, PhysicsAdaptor& adaptor);

  void Initialize(void);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsActor::GetId
   */
  uint32_t GetId() const
  {
    return mActorId;
  }

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsActor::GetBody
   */
  Dali::Any GetBody() const
  {
    return mBody;
  }

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsActor::AsyncSetPhysicsPosition
   */
  void AsyncSetPhysicsPosition(Dali::Vector3 actorPosition);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsActor::AsyncSetPhysicsRotation
   */
  void AsyncSetPhysicsRotation(Dali::Quaternion actorRotation);

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsActor::GetPhysicsPosition
   */
  Dali::Vector3 GetPhysicsPosition() const;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsActor::GetPhysicsRotation
   */
  Dali::Quaternion GetPhysicsRotation() const;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsActor::GetActorPosition
   */
  Dali::Vector3 GetActorPosition() const;

  /**
   * @copydoc Dali::Toolkit::Physics::PhysicsActor::GetActorRotation
   */
  Dali::Quaternion GetActorRotation() const;

private:
  PhysicsAdaptor& mAdaptor;
  uint32_t        mActorId{0};
  Dali::Any       mBody;
};

} // namespace Internal

inline Internal::PhysicsActor& GetImplementation(Physics::PhysicsActor& physics)
{
  DALI_ASSERT_ALWAYS(physics && "Physics actor handle is empty");
  BaseObject& handle = physics.GetBaseObject();
  return static_cast<Internal::PhysicsActor&>(handle);
}

inline const Internal::PhysicsActor& GetImplementation(const Physics::PhysicsActor& physics)
{
  DALI_ASSERT_ALWAYS(physics && "Physics actor handle is empty");
  const BaseObject& handle = physics.GetBaseObject();
  return static_cast<const Internal::PhysicsActor&>(handle);
}

} // namespace Dali::Toolkit::Physics

#endif //DALI_TOOLKIT_PHYSICS_INTERNAL_ACTOR_H
