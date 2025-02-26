#ifndef DALI_TOOLKIT_INTERNAL_RENDER_EFFECT_H
#define DALI_TOOLKIT_INTERNAL_RENDER_EFFECT_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
 *
 */

// EXTERNAL INCLUDE
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/signals/connection-tracker.h>

//INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/render-effects/render-effect.h>

namespace Dali
{
namespace Toolkit
{
class RenderEffectImpl;

namespace Internal
{
using RenderEffectImplPtr = IntrusivePtr<RenderEffectImpl>;

class RenderEffectImpl : public BaseObject, public ConnectionTracker
{
public:
  /**
   * @brief Sets owner Control. Applies effect on the owner.
   * @param[in] control The owner control to apply RenderEffect.
   */
  void SetOwnerControl(Toolkit::Control control);

  /**
   * @brief Clears owner Control.
   */
  void ClearOwnerControl();

  /**
   * @brief Set shader constants of target renderer. Without an explicit call, it will pull off BACKGROUND property of the owner control.
   * @param[in] map may include corner radius, corner radius policy, and squareness
   */
  void SetCornerConstants(Property::Map map);

  /**
   * @brief Clones current instance.
   * @return Clone implementation
   */
  virtual RenderEffectImplPtr Clone() const = 0;

  /**
   * @brief Retrieves OffScreenRenderableType of this RenderEffect.
   *
   * @return OffScreenRenderableType for this RenderEffect.
   */
  virtual OffScreenRenderable::Type GetOffScreenRenderableType() = 0;

  /**
   * @brief Retrieves the off-screen RenderTasks associated with the RenderEffect.
   * This method returns the internal RenderTasks held by the RenderEffect. This tasks are
   * used for off-screen rendering, and the system will assign order index to each
   * tasks based on the render order.
   *
   * RenderEffect with a non-NONE OffScreenRenderableType should override this method to
   * provide their render tasks.
   *
   * @param[out] tasks A list of RenderTasks to be populated with the RenderEffect's forward
   * or backward off-screen RenderTask.
   * @param[in] isForward Indicates whether to retrieve forward (true) or backward (false)
   * RenderTasks.
   */
  virtual void GetOffScreenRenderTasks(std::vector<Dali::RenderTask>& tasks, bool isForward) = 0;

protected:
  /**
   * @copydoc Dali::Toolkit::RenderEffect::RenderEffect
   */
  RenderEffectImpl();

  /**
   * @copydoc Dali::Toolkit::RenderEffect::~RenderEffect
   */
  virtual ~RenderEffectImpl() override;

  RenderEffectImpl(const RenderEffectImpl&) = delete;
  RenderEffectImpl(RenderEffectImpl&&)      = delete;
  RenderEffectImpl& operator=(RenderEffectImpl&&) = delete;      // no move()
  RenderEffectImpl& operator=(const RenderEffectImpl&) = delete; // no copy()

  /**
   * @brief Second-phase Initialization
   */
  void Initialize();

  /**
   * @brief Get target renderer
   * On internal Activate(), the renderer draws our visual effect and is added to our Owner control.
   * @return mRenderer
   */
  Renderer GetTargetRenderer() const;

  /**
   * @brief The final size of the owner after resizing or relayouts.
   * @return mTargetSize
   */
  Vector2 GetTargetSize() const;

  /**
   * @brief Get camera that captures full size texture of mOwnerControl
   * @return mCamera
   */
  CameraActor GetCameraActor() const;

  /**
   * @brief Get Owner control. It could be return empty handle if owner control is not set, or destroyed.
   * @return mOwnerControl
   */
  Toolkit::Control GetOwnerControl() const;

  /**
   * @brief Get scene holder of owner control.
   * @return mPlacementSceneHolder
   */
  Integration::SceneHolder GetSceneHolder() const;

  /// For sub classes
protected:
  /**
   * @brief Initializes sub classes effect
   */
  virtual void OnInitialize() = 0;

  /**
   * @brief Activates sub classes effect on ownerControl
   */
  virtual void OnActivate() = 0;

  /**
   * @brief Deactivates sub classes effect
   */
  virtual void OnDeactivate() = 0;

  /**
   * @brief Redraws effect without deactivation
   * @note Assumes activation
   */
  virtual void OnRefresh() = 0;

private:
  /**
   * @brief Activates effect on ownerControl
   */
  void Activate();

  /**
   * @brief Deactivates effect
   */
  void Deactivate();

  /**
   * @brief Check whether it is possible to activate effect or not.
   *        It will check various status, e.g. the control's visibility.
   * @note This API don't consider mIsActivated
   */
  bool IsActivateValid() const;

  /**
   * @brief Get whether this effect activated or not.
   * @return True if effect is activated. False otherwise.
   */
  bool IsActivated() const;

private:
  /**
   * @brief Calculates a valid target size for texture.
   * Invalid cases include: zero vector, minus numbers or large numbers(larger than the maximum).
   */
  void UpdateTargetSize();

  /**
   * @brief Callback when the size changes.
   */
  void OnSizeSet(PropertyNotification& source);

  /**
   * @brief Callback when the visibility of the actor is changed.
   * @param[in] actor The actor
   * @param[in] visible Whether this actor is visible or not.
   */
  void OnControlInheritedVisibilityChanged(Actor actor, bool visible);

private:
  Dali::Renderer mRenderer; // An additional renderer for mOwnerControl
  CameraActor    mCamera;   // A camera that captures full size texture of mOwnerControl

  Dali::WeakHandle<Dali::Toolkit::Control> mOwnerControl;         ///< Weakhandle of owner control.
  WeakHandle<Integration::SceneHolder>     mPlacementSceneHolder; ///< Weakhandle of scene

  PropertyNotification mSizeNotification; // Resize/Relayout signal.
  Vector2              mTargetSize;       // The final size of mOwnerControl

  bool mIsActivated : 1;
};
} // namespace Internal

inline Toolkit::Internal::RenderEffectImpl& GetImplementation(Toolkit::RenderEffect& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Toolkit::Internal::RenderEffectImpl&>(handle);
}

inline const Toolkit::Internal::RenderEffectImpl& GetImplementation(const Toolkit::RenderEffect& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Toolkit::Internal::RenderEffectImpl&>(handle);
}

} // namespace Toolkit
} // namespace Dali
#endif // DALI_TOOLKIT_INTERNAL_RENDER_EFFECT_H
