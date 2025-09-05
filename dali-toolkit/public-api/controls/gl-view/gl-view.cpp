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

// CLASS HEADER
#include <dali-toolkit/public-api/controls/gl-view/gl-view.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/gl-view/gl-view-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/addon-binder.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/graphics-backend.h>
#include <dlfcn.h>
#include <memory>

namespace Dali::Toolkit
{
namespace
{
const char* const DALI_TOOLKIT_GLES_SO("libdali2-toolkit-gles.so");
const char* const DALI_TOOLKIT_GLES_ADDON_NAME("ToolkitGlesAddOn");

struct ToolkitGlesAddOn : public Dali::AddOn::AddOnBinder
{
  ToolkitGlesAddOn()
  : Dali::AddOn::AddOnBinder(DALI_TOOLKIT_GLES_ADDON_NAME, DALI_TOOLKIT_GLES_SO)
  {
  }
  ~ToolkitGlesAddOn() = default;

  ADDON_BIND_FUNCTION(GlViewNew, GlView(GlView::BackendMode, GlView::ColorFormat));
  ADDON_BIND_FUNCTION(GlViewRegisterGlCallbacks, void(Internal::GlViewImpl&, CallbackBase*, CallbackBase*, CallbackBase*));
  ADDON_BIND_FUNCTION(GlViewSetResizeCallback, void(Internal::GlViewImpl&, CallbackBase*));
  ADDON_BIND_FUNCTION(GlViewSetGraphicsConfig, bool(Internal::GlViewImpl&, bool, bool, int, GlView::GraphicsApiVersion));
  ADDON_BIND_FUNCTION(GlViewSetRenderingMode, void(Internal::GlViewImpl&, GlView::RenderingMode));
  ADDON_BIND_FUNCTION(GlViewGetRenderingMode, GlView::RenderingMode(const Internal::GlViewImpl&));
  ADDON_BIND_FUNCTION(GlViewGetBackendMode, GlView::BackendMode(const Internal::GlViewImpl&));
  ADDON_BIND_FUNCTION(GlViewRenderOnce, void(Internal::GlViewImpl&));
  ADDON_BIND_FUNCTION(GlViewBindTextureResources, void(Internal::GlViewImpl&, std::vector<Dali::Texture>));
  ADDON_BIND_FUNCTION(GlViewTerminate, void(Internal::GlViewImpl&));
};

std::unique_ptr<ToolkitGlesAddOn> gToolkitGlesAddon;
} // namespace

GlView::GlView() = default;

GlView::GlView(const GlView& GlView) = default;

GlView::GlView(GlView&& rhs) noexcept = default;

GlView& GlView::operator=(const GlView& GlView) = default;

GlView& GlView::operator=(GlView&& rhs) noexcept = default;

GlView::~GlView() = default;

GlView GlView::New(ColorFormat colorFormat)
{
  return New(BackendMode::EGL_IMAGE_OFFSCREEN_RENDERING, colorFormat);
}

GlView GlView::New(BackendMode backendMode, ColorFormat colorFormat)
{
  if(Graphics::GetCurrentGraphicsBackend() == Graphics::Backend::GLES)
  {
    if(!gToolkitGlesAddon)
    {
      gToolkitGlesAddon.reset(new ToolkitGlesAddOn);
    }
    DALI_ASSERT_ALWAYS(gToolkitGlesAddon && "Cannot load the GlView Addon\n");
    return gToolkitGlesAddon->GlViewNew(backendMode, colorFormat);
  }
  DALI_ABORT("Current Graphics Backend does not support GlView\n");
}

GlView GlView::DownCast(BaseHandle handle)
{
  return Control::DownCast<GlView, Internal::GlViewImpl>(handle);
}

void GlView::RegisterGlCallbacks(CallbackBase* initCallback, CallbackBase* renderFrameCallback, CallbackBase* terminateCallback)
{
  Internal::GlViewImpl& impl = GetImpl(*this); // Get Impl here to catch uninitialized usage
  if(gToolkitGlesAddon)
  {
    gToolkitGlesAddon->GlViewRegisterGlCallbacks(impl, initCallback, renderFrameCallback, terminateCallback);
  }
}

void GlView::SetResizeCallback(CallbackBase* resizeCallback)
{
  Internal::GlViewImpl& impl = GetImpl(*this); // Get Impl here to catch uninitialized usage
  if(gToolkitGlesAddon)
  {
    gToolkitGlesAddon->GlViewSetResizeCallback(impl, resizeCallback);
  }
}

bool GlView::SetGraphicsConfig(bool depth, bool stencil, int msaa, GraphicsApiVersion version)
{
  Internal::GlViewImpl& impl = GetImpl(*this); // Get Impl here to catch uninitialized usage
  return gToolkitGlesAddon ? gToolkitGlesAddon->GlViewSetGraphicsConfig(impl, depth, stencil, msaa, version) : false;
}

void GlView::SetRenderingMode(RenderingMode mode)
{
  Internal::GlViewImpl& impl = GetImpl(*this);
  if(gToolkitGlesAddon)
  {
    gToolkitGlesAddon->GlViewSetRenderingMode(impl, mode);
  }
}

Dali::Toolkit::GlView::RenderingMode GlView::GetRenderingMode() const
{
  const Internal::GlViewImpl& impl = GetImpl(*this); // Get Impl here to catch uninitialized usage
  return gToolkitGlesAddon ? gToolkitGlesAddon->GlViewGetRenderingMode(impl) : RenderingMode::CONTINUOUS;
}

Dali::Toolkit::GlView::BackendMode GlView::GetBackendMode() const
{
  const Internal::GlViewImpl& impl = GetImpl(*this); // Get Impl here to catch uninitialized usage
  return gToolkitGlesAddon ? gToolkitGlesAddon->GlViewGetBackendMode(impl) : BackendMode::DEFAULT;
}

void GlView::RenderOnce()
{
  Internal::GlViewImpl& impl = GetImpl(*this); // Get Impl here to catch uninitialized usage
  if(gToolkitGlesAddon)
  {
    gToolkitGlesAddon->GlViewRenderOnce(impl);
  }
}

void GlView::BindTextureResources(std::vector<Dali::Texture> textures)
{
  Internal::GlViewImpl& impl = GetImpl(*this); // Get Impl here to catch uninitialized usage
  if(gToolkitGlesAddon)
  {
    gToolkitGlesAddon->GlViewBindTextureResources(impl, std::move(textures));
  }
}

void GlView::Terminate()
{
  Internal::GlViewImpl& impl = GetImpl(*this); // Get Impl here to catch uninitialized usage
  if(gToolkitGlesAddon)
  {
    gToolkitGlesAddon->GlViewTerminate(impl);
  }
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
