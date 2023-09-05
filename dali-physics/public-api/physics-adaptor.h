#ifndef DALI_TOOLKIT_PHYSICS_ADAPTOR_H
#define DALI_TOOLKIT_PHYSICS_ADAPTOR_H

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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/object/any.h>
#include <dali/public-api/object/base-handle.h>
#include <functional> ///< for std::function

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali::Toolkit::Physics
{
class PhysicsActor;

namespace Internal
{
class PhysicsAdaptor;
class PhysicsWorld;
} // namespace Internal

/**
 * Adaptor to manage access to the physics world and pairing actors and physics
 * bodies, plus some translation methods to/from the physics space and dali space.
 *
 * Also manages a debug renderer that may utilize the physics engine debug.
 * It is up to the developer to retrieve the root actor and parent it into the scene.
 */
class DALI_TOOLKIT_API PhysicsAdaptor : public BaseHandle
{
public:
  /**
   * @brief Enumeration to turn the integration step on or off.
   */
  enum class IntegrationState
  {
    OFF,
    ON
  };

  /**
   * @brief Enumeration to turn the debug rendering on or off
   */
  enum class DebugState
  {
    OFF,
    ON
  };

  /**
   * @brief Scoped accessor to the physics world.
   *
   * @SINCE_2_2.43
   * Automatically locks the physics world with a mutex to prevent the
   * integration step from running whilst the developer is accessing
   * the world, e.g. to add/remove bodies or constraints, or to
   * perform hit-test.
   *
   * When it goes out of scope, the mutex is unlocked, and the integration step
   * can resume.
   */
  struct ScopedPhysicsAccessor
  {
  public:
    /**
     * @brief Get a pointer to the native world.
     *
     * @SINCE_2_2.43
     * This uses DALi::Any wrapper to ensure that the same interface can be used
     * for both 2d and 3d physics. It can be cast to the right type using the
     * following construct:
     *   auto accessor = PhysicsAdaptor.GetPhysicsAccessor();
     *   auto bulletWorld = accessor->GetNative().Get<btDiscreteDynamiscWorld*>();
     */
    Dali::Any GetNative();

    /**
     * @brief Hit test the physics world and return the nearest body.
     *
     * @SINCE_2_2.43
     * @param[in] rayFromWorld The origin in physics world space
     * @param[in] rayToWorld A point along the direction on the far side of the physics world
     * @param[out] localPivot The hit point local to the body
     * @param[out] distanceFromCamera The distance of the pick point from the camera
     * @return Empty value if no dynamic body found, otherwise a valid ptr to the hit body.
     */
    Dali::Any HitTest(Dali::Vector3 rayFromWorld, Dali::Vector3 rayToWorld, Dali::Vector3& localPivot, float& distanceFromCamera);

    // Not copyable
    ScopedPhysicsAccessor(ScopedPhysicsAccessor&) = delete;
    const ScopedPhysicsAccessor& operator=(const ScopedPhysicsAccessor&) = delete;

    /**
     * @brief Destructor.
     *
     * On leaving scope, the mutex is unlocked and the physics integration step
     * can resume.
     */
    ~ScopedPhysicsAccessor();

  private:
    /**
     * @brief Private constructor.
     *
     * It is created by the physics adaptor.
     */
    ScopedPhysicsAccessor(Internal::PhysicsWorld& world);
    friend Internal::PhysicsAdaptor;

    struct Impl; ///< Opaque implementation structure
    Impl* mImpl;
  };

  /**
   * @brief Creates an uninitalized PhysicsAdaptor, this can be initialized with PhysicsAdaptor::New().
   *
   * @SINCE_2_2.43
   * Calling member functions with an uninitialized PhysicsAdaptor handle is not allowed.
   */
  PhysicsAdaptor();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_2_2.43
   */
  ~PhysicsAdaptor();

  /**
   * @brief Copy Constructor.
   *
   * @SINCE_2_2.43
   * @param[in] handle The handle to copy
   * @note This creates a new handle, but does not create a new implementation object.
   */
  PhysicsAdaptor(const PhysicsAdaptor& handle);

  /**
   * @brief Move Constructor.
   *
   * @SINCE_2_2.43
   * @param[in] handle A reference to the handle to move
   */
  PhysicsAdaptor(PhysicsAdaptor&& rhs) noexcept;

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_2.43
   * @param[in] handle A reference to the handle to move
   * @return a reference to this handle
   */
  PhysicsAdaptor& operator=(const PhysicsAdaptor& handle);

  /**
   * @brief Move Assignment operator.
   *
   * @SINCE_2_2.43
   * @param[in] handle A reference to the handle to move
   * @return a reference to this handle
   */
  PhysicsAdaptor& operator=(PhysicsAdaptor&& handle) noexcept;

  /**
   * @brief Initialize the physics system.
   *
   * @SINCE_2_2.43
   * @param[in] transform The transform matrix for DALi to Physics world space
   * @param[in] size The size of the layer the physics actors will be drawn in
   */
  static PhysicsAdaptor New(const Dali::Matrix& transform, Uint16Pair size);

  /**
   * @brief Downcasts a handle to PhysicsAdaptor handle.
   *
   * If handle points to an PhysicsAdaptor object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_2_2.43
   * @param[in] handle to an object
   * @return handle to a PhysicsAdaptor object or an uninitialized handle
   */
  static PhysicsAdaptor DownCast(BaseHandle handle);

  /**
   * @brief Set how long the integration should take.
   *
   * @SINCE_2_2.43
   * @param[in] timestep The length of time that the physics integration should take.
   */
  void SetTimestep(float timestep);

  /**
   * @brief Get the current physics integration timestep.
   *
   * @SINCE_2_2.43
   * @return the current physics integration timestep
   */
  float GetTimestep() const;

  /**
   * @brief Type to represent a pointer to a scoped accessor.
   *
   * @SINCE_2_2.43
   */
  using ScopedPhysicsAccessorPtr = std::unique_ptr<PhysicsAdaptor::ScopedPhysicsAccessor>;

  /**
   * @brief Returns an accessor to the physics world.
   *
   * @SINCE_2_2.43
   * It automatically locks a mutex to prevent the integration step
   * from running whilst the world is being modified.
   *
   * When the pointer goes out of scope, the mutex is unlocked and the physics world
   * can run again.
   * @return the scoped accessor
   */
  ScopedPhysicsAccessorPtr GetPhysicsAccessor();

  /**
   * @brief Create a layer & debug renderer.
   *
   * @SINCE_2_2.43
   * The debug renderer may utilize the debug features of the native physics
   * engine.
   *
   * @param[in] window The window to draw in (requires camera)
   * @return The debug layer
   */
  Dali::Layer CreateDebugLayer(Dali::Window window);

  /**
   * @brief Converts a point in RootActor local coords (e.g. gesture)
   * into physics space coords.
   *
   * @SINCE_2_2.43
   * @param vector The point to convert
   * @return The converted point
   */
  Dali::Vector3 TranslateToPhysicsSpace(Dali::Vector3 vector) const;

  /**
   * @brief Convert a rotation in DALi coordinate system into physics space.
   *
   * @SINCE_2_2.43
   * @param[in] rotation The rotation to convert
   * @return the converted rotation.
   */
  Dali::Quaternion TranslateToPhysicsSpace(Dali::Quaternion rotation) const;

  /**
   * @brief Converts a point in physics space coords into RootActor local coords.
   *
   * @SINCE_2_2.43
   * @param vector The point to convert
   * @return The converted point
   */
  Dali::Vector3 TranslateFromPhysicsSpace(Dali::Vector3 vector) const;

  /**
   * @brief Convert a rotation in physics coordinate system into DALi space.
   *
   * @SINCE_2_2.43
   * @param[in] rotation The rotation to convert
   * @return the converted rotation.
   */
  Dali::Quaternion TranslateFromPhysicsSpace(Dali::Quaternion rotation) const;

  /**
   * @brief Converts a vector (not a point) in DALi space into physics space.
   *
   * @SINCE_2_2.43
   * @param vector The vector to convert
   * @return The converted vector
   */
  Dali::Vector3 ConvertVectorToPhysicsSpace(Dali::Vector3 vector) const;

  /**
   * @brief Converts a vector (not a point) in physics space to DALi space.
   *
   * @SINCE_2_2.43
   * @param vector The vector to convert
   * @return The converted vector
   */
  Dali::Vector3 ConvertVectorFromPhysicsSpace(Dali::Vector3 vector) const;

  /**
   * @brief Set up the transform from world space to physics space.
   *
   * @SINCE_2_2.43
   * @param[in] transform The transform matrix for DALi to Physics world space
   * @param[in] size The size of the layer the physics actors will be drawn in
   */
  void SetTransformAndSize(const Dali::Matrix& transform, Uint16Pair size);

  /**
   * @brief Set the integration state.
   *
   * @SINCE_2_2.43
   * If it's turned off, physics will be paused.
   * @note This is ON by default
   * @param[in] state the new integration state
   */
  void SetIntegrationState(IntegrationState state);

  /**
   * @brief Get the integration state.
   *
   * @SINCE_2_2.43
   * @return the new integration state
   */
  IntegrationState GetIntegrationState() const;

  /**
   * @brief Set the debug state.
   *
   * @SINCE_2_2.43
   * If debug is turned on, use the physics engine
   * debug to show wireframes in a layer above the root actor.
   * @note This is OFF by default
   * @param[in] state the new debug state
   */
  void SetDebugState(DebugState state);

  /**
   * @brief Get the debug state.
   *
   * @SINCE_2_2.43
   * @return the new debug state
   */
  DebugState GetDebugState() const;

  /**
   * @brief Add an actor / body pair.
   * @pre It's expected that the client has added the body to the physics world.
   *
   * The adaptor does not "take ownership" of the actor or the physics body.
   * @SINCE_2_2.43
   * @param[in] actor The actor used for rendering the physics object
   * @param[in] body The physics object
   * @return a handle to the actor / body pair.
   */
  PhysicsActor AddActorBody(Dali::Actor actor, Dali::Any body);

  /**
   * @brief Remove the actor / body.
   *
   * This will unparent the actor from the root actor and disassociate it from
   * the body.
   *
   * It is the responsibility of the client to remove the body from the physics world
   * and destroy it at an appropriate time. Create a scoped accessor to ensure
   * that the integration step isn't being run when doing so.
   *
   * If the root actor is holding the last reference to the actor, it will be
   * destroyed automatically, otherwise it is the responsibility of the client to
   * dereference the actor.
   *
   * @SINCE_2_2.43
   * @param[in] physicsActor The actor / body pair to remove.
   */
  void RemoveActorBody(PhysicsActor physicsActor);

  /**
   * @brief Get the physics actor associated with the given body.
   *
   * @SINCE_2_2.43
   * @param[in] body The physics body
   * @return the associated physics actor
   */
  PhysicsActor GetPhysicsActor(Dali::Any body) const;

  /**
   * @brief Get the root actor (which holds all the actor/body pairs).
   *
   * @SINCE_2_2.43
   * @return the root actor.
   */
  Dali::Actor GetRootActor() const;

  /**
   * @brief Convert DALi touch point into a picking ray in the physics world.
   *
   * These can then be used to hit test the PhysicsWorld
   *
   * @SINCE_2_2.43
   * @param[in] origin The origin in DALi world space
   * @param[in] direction The direction of the picking ray
   * @param[out] rayFromWorld The origin in physics world space
   * @param[out] rayToWorld A point along the direction on the far side of the physics world
   *
   * Example:
   *   OnTouched(Dali::Actor actor, Dali::TouchEvent& touch)
   *   {
   *     ..
   *     Vector3 origin, direction;
   *     Dali::HitTestAlgorithm::BuildPickingRay(renderTask, touch.GetScreenPosition(0), origin, direction);
   *     btVector3 rayFromWorld, rayToWorld;
   *     physicsAdaptor.BuildPickingRay(origin, direction, rayFromWorld, rayToWorld);
   *     auto scopedAccessor = physicsAdaptor.GetPhysicsAccessor();
   *     body = scopedAccessor->Get().HitTest(rayFromWorld, rayToWorld, ..);
   *   }
   */
  void BuildPickingRay(Dali::Vector3 origin, Dali::Vector3 direction, Dali::Vector3& rayFromWorld, Dali::Vector3& rayToWorld);

  /**
   * @brief Project a point from the origin (in DALi space) a distance along
   * the direction vector (in DALi space), and return the projected
   * point in Physics space.
   *
   * @SINCE_2_2.43
   * @param[in] origin    Origin in DALi world space
   * @param[in] direction Direction in DALi world space
   * @param[in] distance  Distance along the direction vector
   * @return the projected point, converted to the Physics space.
   */
  Dali::Vector3 ProjectPoint(Dali::Vector3 origin, Dali::Vector3 direction, float distance);

  /**
   * @brief Queue a function to be executed before the physics integration in the update thread.
   *
   * @SINCE_2_2.43
   * Multiple functions can be queued up. They are executed in the update frame
   * callback when the next sync point is seen, so CreateSyncPoint() should be called
   * afterwards for this to be executed.
   *
   * @param[in] function to execute. This can be any method, and it can work fine with
   * physics bodies etc, but it must not be used with DALI event side objects, as this
   * will very likely cause the update thread to crash.
   */
  void Queue(std::function<void(void)> function);

  /**
   * @brief Create a sync point for queued functions.
   *
   * @SINCE_2_2.43
   * Ensures that any previously queued functions are processed
   * in the Update::FrameCallback during the same frame as other
   * DALi properties set during this event handler invocation.
   * For example,
   *   boxActor = Actor::New();
   *   //... Create box actor renderer ...
   *   btRigidBodyConstructionInfo info;
   *   //... set construction properties
   *   boxBody = new btRigidBody(info);
   *   auto boxPhysicsActor = physicsAdaptor.AddActorBody(boxActor, boxBody);
   *   boxActor.SetProperty(Actor::Property::VISIBLE, true);
   *   boxActor.SetProperty(Actor::Property::OPACITY, 0.5f);
   *   physicsAdaptor.Queue([boxBody](){ boxBody->activate(true);});
   *   btVector3 impulse(4, 5, 6);
   *   btVector3 position();
   *   physicsAdaptor.Queue([boxBody](){ boxBody->applyImpulse(impulse, position);});
   *   physicsAdaptor.CreateSyncPoint();
   *
   * Ensures that the box has both render properties and physics properties applied
   * during the same frame.
   */
  void CreateSyncPoint();

public: // Not intended for developer use
  /// @cond internal
  /**
   * @brief This constructor is used by PhysicsAdaptor::New() methods.
   *
   * @SINCE_2_2.43
   * @param[in] impl A pointer to a newly allocated Dali resource.
   * @note Not intended for application developers
   */
  explicit DALI_INTERNAL PhysicsAdaptor(Internal::PhysicsAdaptor* impl);
  /// @endcond
};

} // namespace Dali::Toolkit::Physics

#endif //DALI_TOOLKIT_PHYSICS_ADAPTOR_H
