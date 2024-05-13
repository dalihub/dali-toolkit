#ifndef DALI_TOOLKIT_INTERNAL_DRAWABLE_VIEW_H
#define DALI_TOOLKIT_INTERNAL_DRAWABLE_VIEW_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/signals/render-callback.h>
#include "gl-view-interface-impl.h"

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/gl-view/drawable-view-native-renderer.h>
#include <dali-toolkit/internal/controls/gl-view/gl-view-interface-impl.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/gl-view/gl-view.h>

namespace Dali::Toolkit
{
class GlView;

namespace Internal
{
class DrawableView : public Dali::Toolkit::Internal::GlViewImpl
{
protected:
  virtual ~DrawableView();

public:
  /**
   * @brief Creates GlView interface object using DrawableView implementation
   *
   * @param[in] backendMode Backend mode to be used. Only DIRECT_RENDERING and DIRECT_RENDERING_THREADED
   *                        are accepted.
   * @return Valid GlView object or nullptr on error
   */
  static Dali::Toolkit::GlView New(GlView::BackendMode backendMode);

  /**
   * @brief Constructor creates GlView interface object using DrawableView implementation
   *
   * @param[in] backendMode Backend mode to be used. Only DIRECT_RENDERING and DIRECT_RENDERING_THREADED
   *                        are accepted.
   */
  explicit DrawableView(GlView::BackendMode backendMode);

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
  Dali::Toolkit::GlView::RenderingMode GetRenderingMode() const override;

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
  DrawableView(const DrawableView& GlView);
  DrawableView& operator=(const DrawableView& GlView);

  /**
   * Callback when the visibility of the GlView is changed
   */
  void OnControlVisibilityChanged(Dali::Actor actor, bool visible, Dali::DevelActor::VisibilityChange::Type type);

  /**
   * Callback when the visibility of the window is changed
   */
  void OnWindowVisibilityChanged(Dali::Window window, bool visible);

  /**
   * Adds renderer to Actor.
   */
  void AddRenderer();

private:
  bool OnRenderCallback(const RenderCallbackInput& renderCallbackInput);

private:
  WeakHandle<Window>                   mPlacementWindow;
  Dali::Toolkit::GlView::RenderingMode mRenderingMode;

  bool mDepth;
  bool mStencil;
  int  mMSAA;

  std::unique_ptr<RenderCallback> mRenderCallback;

  /*
   * Used within RenderCallback to handle the current render state
   */
  enum class ViewState
  {
    INIT,
    RENDER,
    TERMINATE
  };

  ViewState mCurrentViewState{ViewState::INIT}; ///< state within RenderCallback

  std::unique_ptr<CallbackBase> mOnResizeCallback; ///< Resize callback called when surface size changes

  std::atomic_bool mSurfaceResized{false}; ///< Flag to invoke surface resize callback

  Size mSurfaceSize{}; ///< Surface size

  std::unique_ptr<Dali::Internal::DrawableViewNativeRenderer> mNativeRenderer; ///< Pointer to the native renderer
};

} // namespace Internal

} // namespace Dali::Toolkit

#endif // DALI_TOOLKIT_INTERNAL_DRAWABLE_VIEW_H
