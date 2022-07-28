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

// CLASS HEADER
#include <dali-toolkit/internal/controls/gl-view/drawable-view-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/lifecycle-controller.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/signals/render-callback.h>

namespace Dali::Toolkit::Internal
{
Dali::Toolkit::GlView DrawableView::New(GlView::BackendMode backendMode)
{
  auto*                 impl   = new DrawableView(backendMode);
  Dali::Toolkit::GlView handle = Dali::Toolkit::GlView(*impl);
  impl->Initialize();
  return handle;
}

DrawableView::DrawableView(GlView::BackendMode backendMode)
: Dali::Toolkit::Internal::GlViewImpl(backendMode),
  mRenderingMode(Toolkit::GlView::RenderingMode::CONTINUOUS),
  mDepth(false),
  mStencil(false),
  mMSAA(0)
{
  mRenderCallback = RenderCallback::New(this, &DrawableView::OnRenderCallback);

  // Create NativeRenderer
  Dali::Internal::NativeRendererCreateInfo createInfo;
  createInfo.maxOffscreenBuffers = 2u;
  createInfo.threadEnabled       = (backendMode == GlView::BackendMode::DIRECT_RENDERING_THREADED);
  createInfo.presentationMode    = Dali::Internal::NativeRendererCreateInfo::PresentationMode::FIFO;
  mNativeRenderer                = std::make_unique<Dali::Internal::DrawableViewNativeRenderer>(createInfo);
}

DrawableView::~DrawableView() = default;

void DrawableView::RegisterGlCallbacks(CallbackBase* initCallback, CallbackBase* renderFrameCallback, CallbackBase* terminateCallback)
{
  mNativeRenderer->RegisterGlCallbacks(initCallback, renderFrameCallback, terminateCallback);
}

void DrawableView::SetResizeCallback(CallbackBase* resizeCallback)
{
  mOnResizeCallback.reset(resizeCallback);
}

bool DrawableView::SetGraphicsConfig(bool depth, bool stencil, int msaa, Dali::Toolkit::GlView::GraphicsApiVersion version)
{
  // Currently, the settings are not relevant for the DirectRendering feature as all the
  // setup is inherited from DALi graphics backend.
  return true;
}

void DrawableView::SetRenderingMode(Dali::Toolkit::GlView::RenderingMode mode)
{
  mRenderingMode    = mode;
  Renderer renderer = Self().GetRendererAt(0);

  if(mRenderingMode == Dali::Toolkit::GlView::RenderingMode::ON_DEMAND)
  {
    renderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::IF_REQUIRED);
  }
  else
  {
    renderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::CONTINUOUSLY);
  }
}

Dali::Toolkit::GlView::RenderingMode DrawableView::GetRenderingMode() const
{
  return mRenderingMode;
}

void DrawableView::RenderOnce()
{
  // Ignored.
  // TODO: without rendering on the separate thread the RenderOnce won't
  // work as expected. Potential implementation of threading may enable that
  // feature.
}

void DrawableView::BindTextureResources(std::vector<Dali::Texture> textures)
{
  mRenderCallback->BindTextureResources(std::move(textures));
}

void DrawableView::OnInitialize()
{
  AddRenderer();

  // Adding VisibilityChange Signal.
  Actor self = Self();
  Dali::DevelActor::VisibilityChangedSignal(self).Connect(this, &DrawableView::OnControlVisibilityChanged);
}

void DrawableView::OnSizeSet(const Vector3& targetSize)
{
  Control::OnSizeSet(targetSize);

  mSurfaceSize = targetSize;

  // If the callbacks are set then schedule execution of resize callback
  if(mRenderCallback && mNativeRenderer)
  {
    mNativeRenderer->Resize(uint32_t(targetSize.width), uint32_t(targetSize.height));
    mSurfaceResized = true;
  }
}

void DrawableView::OnControlVisibilityChanged(Dali::Actor actor, bool visible, Dali::DevelActor::VisibilityChange::Type type)
{
  // Ignored due to lack dedicated rendering thread
}

void DrawableView::OnWindowVisibilityChanged(Window window, bool visible)
{
  // Ignored due to lack dedicated rendering thread
}

void DrawableView::OnSceneConnection(int depth)
{
  Control::OnSceneConnection(depth);

  Actor  self   = Self();
  Window window = DevelWindow::Get(self);

  // Despite OnWindowVisibilityChanged() is ignored it still should follow
  // the designed behaviour of GlView so signal is connected regardless
  if(window)
  {
    DevelWindow::VisibilityChangedSignal(window).Connect(this, &DrawableView::OnWindowVisibilityChanged);
  }
}

void DrawableView::OnSceneDisconnection()
{
  Control::OnSceneDisconnection();

  mNativeRenderer->Terminate();
}

void DrawableView::AddRenderer()
{
  Actor    self     = Self();
  Renderer renderer = Renderer::New(*mRenderCallback);
  self.AddRenderer(renderer);
}

bool DrawableView::OnRenderCallback(const RenderCallbackInput& renderCallbackInput)
{
  if(mNativeRenderer)
  {
    mNativeRenderer->PushRenderCallbackInputData(renderCallbackInput);
  }

  // Init state
  if(mCurrentViewState == ViewState::INIT)
  {
    mNativeRenderer->InvokeGlInitCallback(renderCallbackInput);
    mCurrentViewState = ViewState::RENDER;
  }

  if(mSurfaceResized)
  {
    mNativeRenderer->Resize(uint32_t(mSurfaceSize.width), uint32_t(mSurfaceSize.height));
    mSurfaceResized = false;
  }

  if(mCurrentViewState == ViewState::RENDER)
  {
    // The mSurfaceResized is set by another thread so atomic check must be provided
    bool expected{true};
    if(mSurfaceResized.compare_exchange_weak(expected, false, std::memory_order_release, std::memory_order_relaxed) && mOnResizeCallback)
    {
      CallbackBase::Execute(*mOnResizeCallback, static_cast<int>(mSurfaceSize.x), static_cast<int>(mSurfaceSize.y));
    }

    mNativeRenderer->InvokeGlRenderCallback(renderCallbackInput);
  }

  // The terminate callback isn't easy to implement for DR. The NativeImage backend
  // calls it when the GlView is being destroyed. For DrawableView it means that
  // the RenderCallback won't be executed (as it is a part of graphics pipeline).
  // We don't have currently have any way to know whether the View will be destroyed and
  // to execute last native draw command in the pipeline.
  //
  // else if( mCurrentViewState == ViewState::TERMINATE )
  // {
  //    CallbackBase::Execute(*mOnTerminateCallback);
  // }

  return true;
}

} // namespace Dali::Toolkit::Internal
