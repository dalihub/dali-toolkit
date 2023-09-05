#ifndef DALI_TOOLKIT_PHYSICS_ACTOR_H
#define DALI_TOOLKIT_PHYSICS_ACTOR_H

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
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali::Toolkit::Physics
{
class PhysicsAdaptor;

namespace Internal
{
class PhysicsActor;
}

/**
 * Class to associate a physics body (such as btRigidBody*) with a DALi
 * actor for rendering.
 *
 * This object offers methods to modify basic physics properties
 * asynchronously, that is, on the Update thread. It is also possible
 * for the developer to queue other physics functions on the body by
 * using PhysicsWorld::Queue() and passing in a lambda that captures the
 * body.
 *
 * For example:
 *   btRigidBody* body = physicsActor.GetBody().Get<btRigidBody*>();
 *   mPhysicsImpl.Queue([body](){ body->clearForces(); });
 * This enables the developer to synchronize setting physics properties
 * and setting DALi actor properties.
 */
class DALI_TOOLKIT_API PhysicsActor : public Dali::BaseHandle
{
public:
  /**
   * @brief Constructor
   *
   * @SINCE_2_2.43
   */
  PhysicsActor();

  /**
   * @brief Destructor.
   *
   * @SINCE_2_2.43
   * This is non-virtual since derived Handle types must not contain data or virtual methods
   */
  ~PhysicsActor();

  /**
   * @brief Copy Constructor.
   *
   * @SINCE_2_2.43
   * @param[in] handle The handle to copy
   * @note This creates a new handle, but does not create a new implementation object.
   */
  PhysicsActor(const PhysicsActor& handle);

  /**
   * @brief Move Constructor.
   *
   * @SINCE_2_2.43
   * @param[in] handle A reference to the handle to move
   */
  PhysicsActor(PhysicsActor&& rhs) noexcept;

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_2.43
   * @param[in] handle A reference to the handle to move
   * @return a reference to this handle
   */
  PhysicsActor& operator=(const PhysicsActor& handle);

  /**
   * @brief Move Assignment operator.
   *
   * @SINCE_2_2.43
   * @param[in] handle A reference to the handle to move
   * @return a reference to this handle
   */
  PhysicsActor& operator=(PhysicsActor&& handle) noexcept;

  /**
   * New method.
   * @SINCE_2_2.43
   *
   * Binds the actor to the given body. This should be a body that has
   * been added to the physics world, and has physical postion and
   * rotation in that space. The Actor is used to render that object in
   * DALi space.
   *
   * @note This object only stores the actor ID, which is used
   * internally in the FrameCallback. It is up to the caller to ensure
   * there is a reference to the actor (e.g. by parenting into the scene)
   *
   * @note This API is used internally, and is of little benefit to the developer.
   *
   * @param[in] actor The DALi actor used to render this object
   * @param[in] body The physics body used in the physics simulation
   * @param[in] adaptor The physics adaptor to use. @todo remove?
   */
  static PhysicsActor New(Dali::Actor actor, Dali::Any body, PhysicsAdaptor adaptor);

  /**
   * @brief Downcasts a handle to PhysicsActor handle.
   *
   * If handle points to an PhysicsActor object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_2_2.43
   * @param[in] handle to an object
   * @return handle to a PhysicsActor object or an uninitialized handle
   */
  static PhysicsActor DownCast(BaseHandle handle);

  /**
   * @brief Get the actor ID of the associated actor.
   *
   * @SINCE_2_2.43
   */
  uint32_t GetId() const;

  /**
   * @brief Get the actual physics body of this object.
   *
   * @SINCE_2_2.43
   * Using ANY wrapper to enable this interface to be used for any
   * types of physics bodies from either 2d or 3d physics.
   * @return The physics body. It can be cast to an appropriate type,
   * for example:
   *   btRigidBody* body = actor.GetBody().Get<btRigidBody*>();
   *
   * @note Please ensure to get a scoped accessor first to avoid modifying
   * physics properties during integration step, or use the Async methods
   * or the Queue.
   */
  Dali::Any GetBody() const;

  /**
   * @brief Queue a method to set the position on the associated physics body
   * in the update thread before the next integration.
   *
   * @SINCE_2_2.43
   * @param[in] actorPosition The position of the actor in DALi space
   */
  void AsyncSetPhysicsPosition(Dali::Vector3 actorPosition);

  /**
   * @brief Queue a method to set the rotation of the associated physics body
   * in the update thread before the next integration.
   *
   * @SINCE_2_2.43
   * @param[in] actorRotation The orientation of the actor in DALi space
   */
  void AsyncSetPhysicsRotation(Dali::Quaternion actorRotation);

  /**
   * @brief Get the current position of the physics body in Physics space.
   *
   * @SINCE_2_2.43
   * @return the current position of the physics body in Physics space.
   */
  Dali::Vector3 GetPhysicsPosition() const;

  /**
   * @brief Get the current rotation of the physics body in Physics space.
   *
   * @SINCE_2_2.43
   * @return the current rotation of the physics body in Physics space.
   */
  Dali::Quaternion GetPhysicsRotation() const;

  /**
   * @brief Get the current position of the physics body in DALi space.
   *
   * @SINCE_2_2.43
   * @return the current position of the physics body in DALi space.
   */
  Dali::Vector3 GetActorPosition() const;

  /**
   * @brief Get the current rotation of the physics body in DALi space.
   *
   * @SINCE_2_2.43
   * @return the current rotation of the physics body in DALi space.
   */
  Dali::Quaternion GetActorRotation() const;

public: // Not intended for developer use
  /// @cond internal
  /**
   * @brief This constructor is used by PhysicsActor::New() methods.
   *
   * @SINCE_2_2.43
   * @param[in] impl A pointer to a newly allocated Dali resource.
   * @note Not intended for application developers
   */
  explicit DALI_INTERNAL PhysicsActor(Internal::PhysicsActor* impl);
  /// @endcond
};

} // namespace Dali::Toolkit::Physics

#endif //DALI_TOOLKIT_PHYSICS_ACTOR_H
