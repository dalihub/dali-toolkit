#ifndef DALI_TOOLKIT_INTERNAL_BACKGROUND_EFFECT_H
#define DALI_TOOLKIT_INTERNAL_BACKGROUND_EFFECT_H

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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

//INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/render-effects/render-effect.h>

// EXTERNAL INCLUDE
#include <dali/public-api/dali-core.h>

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
   * @brief Activates effect on ownerControl
   */
  virtual void Activate() = 0;

  /**
   * @brief Deactivates effect
   */
  virtual void Deactivate() = 0;

  /**
   * @brief Sets owner Control. Applies effect on the owner.
   * @param[in] control The owner control to apply RenderEffect.
   */
  void SetOwnerControl(Toolkit::Control control);

  /**
   * @brief Clears owner Control.
   */
  void ClearOwnerControl();

protected:
  /**
   * @copydoc Dali::Toolkit::RenderEffect::RenderEffect
   */
  RenderEffectImpl() = default;

  /**
   * @copydoc Dali::Toolkit::RenderEffect::~RenderEffect
   */
  virtual ~RenderEffectImpl() override;

  RenderEffectImpl(const RenderEffectImpl&) = delete;
  RenderEffectImpl(RenderEffectImpl&&)      = delete;
  RenderEffectImpl& operator=(RenderEffectImpl&&) = delete;      // no move()
  RenderEffectImpl& operator=(const RenderEffectImpl&) = delete; // no copy()

  virtual void Initialize() = 0;

  /**
   * @brief Get target renderer
   * On internal Activate(), the renderer draws our visual effect and is added to our Owner control.
   * @return mRenderer
   */
  Renderer GetTargetRenderer() const;

  /**
   * @brief Callback when the size changes.
   */
  void OnSizeSet(PropertyNotification& source);

  /**
   * @brief The final size of the owner after resizing or relayouts.
   * @return mTargetSize
   */
  Vector2 GetTargetSize() const;

  /**
   * @brief Get Owner control.
   * @return mOwnerControl
   */
  Toolkit::Control GetOwnerControl() const;

private:
  Dali::Renderer         mRenderer; // An additional renderer for mOwnerControl
  Dali::Toolkit::Control mOwnerControl;

  PropertyNotification mSizeNotification; // Resize/Relayout signal
  Vector2              mTargetSize;       // The final size of mOwnerControl
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
#endif // DALI_TOOLKIT_INTERNAL_BACKGROUND_EFFECT_H
