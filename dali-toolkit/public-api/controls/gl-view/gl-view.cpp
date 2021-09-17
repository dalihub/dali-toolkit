/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/gl-view/gl-view-impl.h>

namespace Dali
{
namespace Toolkit
{
GlView::GlView()
{
}

GlView::GlView(const GlView& GlView) = default;

GlView::GlView(GlView&& rhs) = default;

GlView& GlView::operator=(const GlView& GlView) = default;

GlView& GlView::operator=(GlView&& rhs) = default;

GlView::~GlView()
{
}

GlView GlView::New(ColorFormat colorFormat)
{
  return Internal::GlView::New(colorFormat);
}

GlView GlView::DownCast(BaseHandle handle)
{
  return Control::DownCast<GlView, Internal::GlView>(handle);
}

void GlView::RegisterGlCallback(CallbackBase* initCallback, CallbackBase* renderFrameCallback, CallbackBase* terminateCallback)
{
  Dali::Toolkit::GetImpl(*this).RegisterGlCallback(initCallback, renderFrameCallback, terminateCallback);
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

void GlView::RenderOnce()
{
  Dali::Toolkit::GetImpl(*this).RenderOnce();
}

GlView::GlView(Internal::GlView& implementation)
: Control(implementation)
{
}

GlView::GlView(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::GlView>(internal);
}

} // namespace Toolkit

} // namespace Dali
