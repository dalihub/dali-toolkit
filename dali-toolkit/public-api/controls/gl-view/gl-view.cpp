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

// CLASS HEADER
#include <dali-toolkit/public-api/controls/gl-view/gl-view.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/gl-view/drawable-view-impl.h>
#include <dali-toolkit/internal/controls/gl-view/gl-view-impl.h>

namespace Dali::Toolkit
{
GlView::GlView() = default;

GlView::GlView(const GlView& GlView) = default;

GlView::GlView(GlView&& rhs) noexcept = default;

GlView& GlView::operator=(const GlView& GlView) = default;

GlView& GlView::operator=(GlView&& rhs) noexcept = default;

GlView::~GlView() = default;

GlView GlView::New(ColorFormat colorFormat)
{
  // This function is backward compatible and always returns
  // backend based on NativeImage.
  return Internal::GlView::New(colorFormat);
}

GlView GlView::New(BackendMode backendMode, ColorFormat colorFormat)
{
  switch(backendMode)
  {
    case BackendMode::DIRECT_RENDERING:
    case BackendMode::DIRECT_RENDERING_THREADED:
    case BackendMode::UNSAFE_DIRECT_RENDERING:
    {
      return Internal::DrawableView::New(backendMode);
    }
    case BackendMode::EGL_IMAGE_OFFSCREEN_RENDERING:
    {
      return Internal::GlView::New(colorFormat);
    }
    default:
    {
      DALI_ASSERT_ALWAYS("Invalid BackendMode");
    }
  }
  return {};
}

GlView GlView::DownCast(BaseHandle handle)
{
  return Control::DownCast<GlView, Internal::GlViewImpl>(handle);
}

void GlView::RegisterGlCallbacks(CallbackBase* initCallback, CallbackBase* renderFrameCallback, CallbackBase* terminateCallback)
{
  Dali::Toolkit::GetImpl(*this).RegisterGlCallbacks(initCallback, renderFrameCallback, terminateCallback);
}

void GlView::SetResizeCallback(CallbackBase* resizeCallback)
{
  Dali::Toolkit::GetImpl(*this).SetResizeCallback(resizeCallback);
}

bool GlView::SetGraphicsConfig(bool depth, bool stencil, int msaa, GraphicsApiVersion version)
{
  return Dali::Toolkit::GetImpl(*this).SetGraphicsConfig(depth, stencil, msaa, version);
}

void GlView::SetRenderingMode(RenderingMode mode)
{
  Dali::Toolkit::GetImpl(*this).SetRenderingMode(mode);
}

Dali::Toolkit::GlView::RenderingMode GlView::GetRenderingMode() const
{
  return Dali::Toolkit::GetImpl(*this).GetRenderingMode();
}

Dali::Toolkit::GlView::BackendMode GlView::GetBackendMode() const
{
  return Dali::Toolkit::GetImpl(*this).GetBackendMode();
}

void GlView::RenderOnce()
{
  Dali::Toolkit::GetImpl(*this).RenderOnce();
}

void GlView::BindTextureResources(std::vector<Dali::Texture> textures)
{
  Dali::Toolkit::GetImpl(*this).BindTextureResources(std::move(textures));
}

GlView::GlView(Internal::GlViewImpl& implementation)
: Control(implementation)
{
}

GlView::GlView(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::GlViewImpl>(internal);
}

} // namespace Dali::Toolkit
