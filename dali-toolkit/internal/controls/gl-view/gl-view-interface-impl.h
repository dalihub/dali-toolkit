#ifndef DALI_TOOLKIT_INTERNAL_GL_VIEW_IMPL_H
#define DALI_TOOLKIT_INTERNAL_GL_VIEW_IMPL_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/native-image-source-queue.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/gl-view/gl-view-render-thread.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/gl-view/gl-view.h>

namespace Dali::Toolkit
{
class GlView;

namespace Internal
{
class GlViewImpl : public Dali::Toolkit::Internal::Control
{
protected:
  virtual ~GlViewImpl() = default;

public:
  /**
   * Construct a new GlView.
   */
  explicit GlViewImpl(GlView::BackendMode backendMode)
  : Control(ControlBehaviour(0u | ACTOR_BEHAVIOUR_DEFAULT | DISABLE_STYLE_CHANGE_SIGNALS)),
    mBackendMode(backendMode)
  {
  }

  /**
   * @copydoc Dali::Toolkit::GlView::RegisterGlCallbacks()
   */
  virtual void RegisterGlCallbacks(CallbackBase* initCallback, CallbackBase* renderFrameCallback, CallbackBase* terminateCallback) = 0;

  /**
   * @copydoc Dali::Toolkit::GlView::SetResizeCallback()
   */
  virtual void SetResizeCallback(CallbackBase* resizeCallback) = 0;

  /**
   * @copydoc Dali::Toolkit::GlView::SetGraphisConfig()
   */
  virtual bool SetGraphicsConfig(bool depth, bool stencil, int msaa, Dali::Toolkit::GlView::GraphicsApiVersion version) = 0;

  /**
   * @copydoc Dali::Toolkit::GlView::SetRenderingMode()
   */
  virtual void SetRenderingMode(Dali::Toolkit::GlView::RenderingMode mode) = 0;

  /**
   * @copydoc Dali::Toolkit::GlView::GetRenderingMode()
   */
  virtual Dali::Toolkit::GlView::RenderingMode GetRenderingMode() const = 0;

  /**
   * @copydoc Dali::Toolkit::GlView::GetBackendMode()
   */
  [[nodiscard]] Dali::Toolkit::GlView::BackendMode GetBackendMode() const
  {
    return mBackendMode;
  }

  /**
   * @copydoc Dali::Toolkit::GlView::RenderOnce()
   */
  virtual void RenderOnce() = 0;

  /**
   * @copydoc Dali::Toolkit::GlView::BindTextureResources()
   */
  virtual void BindTextureResources(std::vector<Dali::Texture> textures) = 0;

  /**
   * @copydoc Dali::Toolkit::GlView::Terminate()
   */
  virtual void Terminate() = 0;

private: // From Control
  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize() override = 0;

  /**
   * @copydoc Toolkit::Control::OnSceneConnection()
   */
  virtual void OnSceneConnection(int depth) override = 0;

  /**
   * @copydoc Toolkit::Control::OnSceneDisconnection()
   */
  virtual void OnSceneDisconnection() override = 0;

protected:
  GlView::BackendMode mBackendMode{GlView::BackendMode::DEFAULT}; ///< Implementation backend mode (DirectRendering, EGL image)
};

} // namespace Internal

inline Dali::Toolkit::Internal::GlViewImpl& GetImpl(Dali::Toolkit::GlView& handle)
{
  DALI_ASSERT_ALWAYS(handle);
  Dali::RefObject& impl = handle.GetImplementation();
  return static_cast<Dali::Toolkit::Internal::GlViewImpl&>(impl);
}

inline const Dali::Toolkit::Internal::GlViewImpl& GetImpl(const Dali::Toolkit::GlView& handle)
{
  DALI_ASSERT_ALWAYS(handle);
  const Dali::RefObject& impl = handle.GetImplementation();
  return static_cast<const Dali::Toolkit::Internal::GlViewImpl&>(impl);
}

} // namespace Dali::Toolkit

#endif // DALI_TOOLKIT_INTERNAL_GL_VIEW_IMPL_H
