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
#include <dali/devel-api/addons/addon-base.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/gl-view/drawable-view-impl.h>
#include <dali-toolkit/internal/controls/gl-view/gl-view-impl.h>
#include <dali-toolkit/public-api/controls/gl-view/gl-view.h>

using namespace Dali;
using namespace Dali::Toolkit;
using BackendMode = GlView::BackendMode;
using GlViewImpl  = Dali::Toolkit::Internal::GlViewImpl;

namespace
{
const char* const DALI_TOOLKIT_GLES_ADDON_NAME("ToolkitGlesAddOn");

GlView GlViewNew(BackendMode backendMode, GlView::ColorFormat colorFormat)
{
  switch(backendMode)
  {
    case BackendMode::DIRECT_RENDERING:
    case BackendMode::DIRECT_RENDERING_THREADED:
    case BackendMode::UNSAFE_DIRECT_RENDERING:
    {
      return Toolkit::Internal::DrawableView::New(backendMode);
    }
    case BackendMode::EGL_IMAGE_OFFSCREEN_RENDERING:
    {
      return Toolkit::Internal::GlView::New(colorFormat);
    }
    default:
    {
      DALI_ABORT("Invalid BackendMode");
    }
  }
}

void GlViewRegisterGlCallbacks(
  GlViewImpl&&  glView,
  CallbackBase* initCallback,
  CallbackBase* renderFrameCallback,
  CallbackBase* terminateCallback)
{
  glView.RegisterGlCallbacks(initCallback, renderFrameCallback, terminateCallback);
}

void GlViewSetResizeCallback(GlViewImpl& glViewImpl, CallbackBase* resizeCallback)
{
  glViewImpl.SetResizeCallback(resizeCallback);
}

bool GlViewSetGraphicsConfig(
  GlViewImpl&                glViewImpl,
  bool                       depth,
  bool                       stencil,
  int                        msaa,
  GlView::GraphicsApiVersion version)
{
  return glViewImpl.SetGraphicsConfig(depth, stencil, msaa, version);
}

void GlViewSetRenderingMode(GlViewImpl& glViewImpl, GlView::RenderingMode mode)
{
  glViewImpl.SetRenderingMode(mode);
}

GlView::RenderingMode GlViewGetRenderingMode(const GlViewImpl&& glViewImpl)
{
  return glViewImpl.GetRenderingMode();
}

BackendMode GlViewGetBackendMode(const GlViewImpl&& glViewImpl)
{
  return glViewImpl.GetBackendMode();
}

void GlViewRenderOnce(GlViewImpl&& glViewImpl)
{
  glViewImpl.RenderOnce();
}

void GlViewBindTextureResources(GlViewImpl&& glViewImpl, std::vector<Dali::Texture> textures)
{
  glViewImpl.BindTextureResources(std::move(textures));
}

void GlViewTerminate(GlViewImpl&& glViewImpl)
{
  glViewImpl.Terminate();
}
} // unnamed namespace

class ToolkitGlesAddOn : public Dali::AddOns::AddOnBase
{
public:
  void GetAddOnInfo(Dali::AddOnInfo& info) override
  {
    info.type    = Dali::AddOnType::GENERIC;
    info.name    = DALI_TOOLKIT_GLES_ADDON_NAME;
    info.version = Dali::DALI_ADDON_VERSION(1, 0, 0);
    info.next    = nullptr;
  }

  /**
   * Dispatch table for global functions
   * @return
   */
  Dali::AddOns::DispatchTable* GetGlobalDispatchTable() override
  {
    static Dali::AddOns::DispatchTable dispatchTable{};
    if(dispatchTable.Empty())
    {
      dispatchTable["GlViewNew"]                  = GlViewNew;
      dispatchTable["GlViewRegisterGlCallbacks"]  = GlViewRegisterGlCallbacks;
      dispatchTable["GlViewSetResizeCallback"]    = GlViewSetResizeCallback;
      dispatchTable["GlViewSetGraphicsConfig"]    = GlViewSetGraphicsConfig;
      dispatchTable["GlViewSetRenderingMode"]     = GlViewSetRenderingMode;
      dispatchTable["GlViewGetRenderingMode"]     = GlViewGetRenderingMode;
      dispatchTable["GlViewGetBackendMode"]       = GlViewGetBackendMode;
      dispatchTable["GlViewRenderOnce"]           = GlViewRenderOnce;
      dispatchTable["GlViewBindTextureResources"] = GlViewBindTextureResources;
      dispatchTable["GlViewTerminate"]            = GlViewTerminate;
    }
    return &dispatchTable;
  }

  /**
   * Dispatch table for instance functions
   * @return
   */
  Dali::AddOns::DispatchTable* GetInstanceDispatchTable() override
  {
    return nullptr;
  }
};

REGISTER_ADDON_CLASS(ToolkitGlesAddOn);
