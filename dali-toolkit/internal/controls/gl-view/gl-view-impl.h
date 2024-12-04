#ifndef DALI_TOOLKIT_INTERNAL_GL_VIEW_H
#define DALI_TOOLKIT_INTERNAL_GL_VIEW_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/native-image-source-queue.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/gl-view/gl-view-interface-impl.h>
#include <dali-toolkit/internal/controls/gl-view/gl-view-render-thread.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/gl-view/gl-view.h>

namespace Dali::Toolkit
{
class GlView;

namespace Internal
{
class GlView : public Dali::Toolkit::Internal::GlViewImpl
{
protected:
  virtual ~GlView();

public:
  /**
   * @copydoc Dali::Toolkit::GlView::New()
   */
  static Dali::Toolkit::GlView New(Dali::Toolkit::GlView::ColorFormat colorFormat);

  /**
   * Construct a new GlView.
   */
  GlView(Dali::Toolkit::GlView::ColorFormat colorFormat);

  /**
   * @copydoc Dali::Toolkit::GlView::RegisterGlCallbacks()
   */
  void RegisterGlCallbacks(CallbackBase* initCallback, CallbackBase* renderFrameCallback, CallbackBase* terminateCallback) override;

  /**
   * @copydoc Dali::Toolkit::GlView::SetResizeCallback()
   */
  void SetResizeCallback(CallbackBase* resizeCallback) override;

  /**
   * @copydoc Dali::Toolkit::GlView::SetGraphicsConfig()
   */
  bool SetGraphicsConfig(bool depth, bool stencil, int msaa, Dali::Toolkit::GlView::GraphicsApiVersion version) override;

  /**
   * @copydoc Dali::Toolkit::GlView::SetRenderingMode()
   */
  void SetRenderingMode(Dali::Toolkit::GlView::RenderingMode mode) override;

  /**
   * @copydoc Dali::Toolkit::GlView::GetRenderingMode()
   */
  Dali::Toolkit::GlView::RenderingMode GetRenderingMode() const override
  {
    return mRenderingMode;
  }

  /**
   * @copydoc Dali::Toolkit::GlView::RenderOnce()
   */
  void RenderOnce() override;

  /**
   * @copydoc Dali::Toolkit::GlView::BindTextureResources()
   */
  void BindTextureResources(std::vector<Dali::Texture> textures) override;

private: // From Control
  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize() override;

  /**
   * @copydoc Toolkit::Control::OnSceneConnection()
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc Toolkit::Control::OnSceneDisconnection()
   */
  void OnSceneDisconnection() override;

  /**
   * @copydoc Toolkit::Control::OnSizeSet()
   */
  void OnSizeSet(const Vector3& targetSize) override;

private:
  // Undefined copy constructor and assignment operators
  GlView(const GlView& GlView);
  GlView& operator=(const GlView& GlView);

  /**
   * Callback when the inherited visibility of the GlView is changed
   */
  void OnControlInheritedVisibilityChanged(Dali::Actor actor, bool visible);

  /**
   * Callback when the visibility of the window is changed
   */
  void OnWindowVisibilityChanged(Dali::Window window, bool visible);

  /**
   * Creates the geometry for texturing.
   */
  Dali::Geometry CreateTexturedQuad();

  /**
   * Adds renderer to Actor.
   */
  void AddRenderer();

  /**
   * Creates shader for rendering.
   */
  Dali::Shader CreateShader();

  /**
   * @brief Gets the NativeImageSourceQueue's ColorFormat with the GlView's ColorFormat.
   * @param[in] colorFormat the color format of the GlView.
   * @return The color format of NativeImageSourceQueue
   */
  Dali::NativeImageSourceQueue::ColorFormat GetColorFormat(Dali::Toolkit::GlView::ColorFormat format);

private:
  std::unique_ptr<GlViewRenderThread>  mRenderThread;
  Dali::NativeImageSourceQueuePtr      mNativeImageQueue;
  Dali::Toolkit::GlView::RenderingMode mRenderingMode{Toolkit::GlView::RenderingMode::CONTINUOUS};
  Dali::Toolkit::GlView::ColorFormat   mColorFormat{Toolkit::GlView::ColorFormat::RGB888};
  bool                                 mDepth{false};
  bool                                 mStencil{false};
  int                                  mMSAA{0};
};

} // namespace Internal

} // namespace Dali::Toolkit

#endif // DALI_TOOLKIT_INTERNAL_GL_VIEW_H
