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
  enum class IntegrationState
  {
    OFF,
    ON
  };
  enum class DebugState
  {
    OFF,
    ON
  };

  /**
   * Scoped accessor to the physics world. Automatically locks the physics world
   * with a mutex to prevent the integration step from running whilst the
   * developer is accessing the world, e.g. to add/remove bodies or constraints,
   * or to perform hit-test.
   *
   * When it goes out of scope, the mutex is unlocked, and the integration step
   * can resume.
   */
  struct ScopedPhysicsAccessor
  {
  public:
    /**
     * Get a pointer to the native world. This uses DALi::Any wrapper to ensure
     * that the same interface can be used for both 2d and 3d physics. It can be
     * cast to the right type using the following construct:
     *   auto accessor = PhysicsAdaptor.GetPhysicsAccessor();
     *   auto bulletWorld = accessor->GetNative().Get<btDiscreteDynamiscWorld*>();
     */
    Dali::Any GetNative();

    /**
     * Hit test the physics world and return the nearest body.
     *
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
     * Destructor
     */
    ~ScopedPhysicsAccessor();

  private:
    ScopedPhysicsAccessor(Internal::PhysicsWorld& world);
    friend Internal::PhysicsAdaptor;

    struct Impl;
    Impl* mImpl;
  };

  PhysicsAdaptor();
  ~PhysicsAdaptor();
  PhysicsAdaptor(const PhysicsAdaptor& handle);
  PhysicsAdaptor(PhysicsAdaptor&& rhs) noexcept;
  PhysicsAdaptor& operator=(const PhysicsAdaptor& handle);
  PhysicsAdaptor& operator=(PhysicsAdaptor&& handle) noexcept;

  /**
   * Initialize the physics system.
   *
   * @todo Consider allowing developer to create the physics world and pass it in here.
   *
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
   * @SINCE_2.2.40
   * @param[in] handle to an object
   * @return handle to a PhysicsAdaptor object or an uninitialized handle
   */
  static PhysicsAdaptor DownCast(BaseHandle handle);

  /**
   * Set how long the integration should take.
   * @param[in] timestep The length of time that the physics integration should take.
   */
  void SetTimestep(float timestep);

  /**
   * Get the current physics integration timestep
   * @return the current physics integration timestep
   */
  float GetTimestep();

  /**
   * Returns an accessor pointer to the physics world. It automatically locks a mutex
   * to prevent the integration step from running whilst the world is being modified.
   *
   * When the pointer goes out of scope, the mutex is unlocked and the physics world
   * can run again.
   */
  using ScopedPhysicsAccessorPtr = std::unique_ptr<PhysicsAdaptor::ScopedPhysicsAccessor>;
  ScopedPhysicsAccessorPtr GetPhysicsAccessor();

  /**
   * Create a layer & debug renderer
   * The debug renderer may utilize the debug features of the native physics
   * engine.
   *
   * @param[in] window The window to draw in (requires camera)
   */
  Dali::Layer CreateDebugLayer(Dali::Window window);

  /**
   * Converts a point in RootActor local coords (e.g. gesture)
   * into physics space coords.
   * @param vector The point to convert
   * @return The converted point
   */
  Dali::Vector3 TranslateToPhysicsSpace(Dali::Vector3 vector);

  /**
   * Convert a rotation in DALi coordinate system into physics space.
   * @param[in] rotation The rotation to convert
   * @return the converted rotation.
   */
  Dali::Quaternion TranslateToPhysicsSpace(Dali::Quaternion rotation);

  /**
   * Converts a point in physics space coords.
   * into RootActor local coords
   * @param vector The point to convert
   * @return The converted point
   */
  Dali::Vector3 TranslateFromPhysicsSpace(Dali::Vector3 vector);

  /**
   * Convert a rotation in physics coordinate system into DALi space.
   * @param[in] rotation The rotation to convert
   * @return the converted rotation.
   */
  Dali::Quaternion TranslateFromPhysicsSpace(Dali::Quaternion rotation);

  /**
   * Converts a vector (not a point) in DALi space into physics space.
   * @param vector The vector to convert
   * @return The converted vector
   */
  Dali::Vector3 ConvertVectorToPhysicsSpace(Dali::Vector3 vector);

  /**
   * Converts a vector (not a point) in physics space to DALi space
   * @param vector The vector to convert
   * @return The converted vector
   */
  Dali::Vector3 ConvertVectorFromPhysicsSpace(Dali::Vector3 vector);

  /**
   * Set up the transform from world space to physics space
   * @param[in] transform The transform matrix for DALi to Physics world space
   * @param[in] size The size of the layer the physics actors will be drawn in
   */
  void SetTransformAndSize(const Dali::Matrix& transform, Uint16Pair size);

  /**
   * Set the integration state. If it's turned on, physics will run
   * during the update frame callback.
   * @note This is ON by default
   * @param[in] state the new integration state
   */
  void SetIntegrationState(IntegrationState state);

  /**
   * Get the integration state.
   * @return the new integration state
   */
  IntegrationState GetIntegrationState();

  /**
   * Set the debug state. If debug is turned on, use the physics engine
   * debug to show wireframes in a layer above the root actor.
   * @note This is OFF by default
   * @param[in] state the new debug state
   */
  void SetDebugState(DebugState state);

  /**
   * Get the debug state.
   * @return the new debug state
   */
  DebugState GetDebugState();

  /**
   * Add an actor / body pair.
   * @pre It's expected that the client has added the body to the physics world.
   *
   * @param[in] actor The actor used for rendering the physics object
   * @param[in] body The physics object
   * @return a handle to the actor / body pair.
   */
  PhysicsActor AddActorBody(Dali::Actor actor, Dali::Any body);

  /**
   * Get the physics actor associated with the given body
   * @param[in] body The physics body
   * @return the associated physics actor
   */
  PhysicsActor GetPhysicsActor(Dali::Any body);

  /**
   * Get the root actor (which holds all the actor/body pairs)
   */
  Dali::Actor GetRootActor();

  /**
   * Convert DALi touch point into a picking ray in the physics world.
   *
   * These can then be used to hit test the PhysicsWorld
   *
   * @param[in] origin The origin in DALi world space
   * @param[in] direction The direction of the picking ray
   * @param[out] rayFromWorld The origin in physics world space
   * @param[out] rayToWorld A point along the direction on the far side of the
   * physics world
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
   * Project a point from the origin (in DALi space) a distance along
   * the direction vector (in DALi space), and return the projected
   * point in Physics space.
   *
   * @param[in] origin    Origin in DALi world space
   * @param[in] direction Direction in DALi world space
   * @param[in] distance  Distance along the direction vector
   * @return the projected point, converted to the Physics space.
   */
  Dali::Vector3 ProjectPoint(Dali::Vector3 origin, Dali::Vector3 direction, float distance);

  /**
   * Queue a function to be executed before the physics integration in the update thread.
   *
   * @param[in] function to execute. This can be any method, and it can work fine with
   * physics bodies etc, but it must not be used with DALI event side objects, as this
   * will very likely cause the update thread to crash.
   */
  void Queue(std::function<void(void)> function);

  /**
   * Create a sync point for queued functions.
   *
   * Ensures that any queued functions are processed after this sync
   * point is seen in the Update::FrameCallback, which will be in the
   * same frame as any other DALi properties set during this event
   * handler invocation.
   *
   * @param[in] None
   */
  void CreateSyncPoint();

public: // Not intended for developer use
  /// @cond internal
  /**
   * @note Not intented for application developers
   */
  explicit DALI_INTERNAL PhysicsAdaptor(Internal::PhysicsAdaptor* impl);
  /// @endcond
};

} // namespace Dali::Toolkit::Physics

#endif //DALI_TOOLKIT_PHYSICS_ADAPTOR_H
