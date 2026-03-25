/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include "video-view-rendering-strategy.h"
// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/texture-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/video-view/video-view-impl.h>
#include <dali-toolkit/public-api/image-loader/image-url.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>

using Dali::Integration::GetStdString;
using Dali::Integration::ToPropertyValue;

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

namespace
{
const char* const CUSTOM_VERTEX_SHADER("vertexShader");
const char* const CUSTOM_FRAGMENT_SHADER("fragmentShader");

bool GetStringFromProperty(const Dali::Property::Value& value, std::string& output)
{
  bool extracted = false;
  if(GetStdString(value, output))
  {
    extracted = true;
  }
  return extracted;
}
} // namespace

//=============================================================================
// WindowSurfaceStrategy
//=============================================================================

WindowSurfaceStrategy::WindowSurfaceStrategy(Toolkit::VideoView videoViewHandle)
: VideoRenderingStrategy(videoViewHandle)
{
}

WindowSurfaceStrategy::~WindowSurfaceStrategy()
{
  Toolkit::VideoView videoViewHandle = GetVideoView();
  Toolkit::Control control = Toolkit::Control::DownCast(videoViewHandle);
  if(control)
  {
    VideoView* videoView = GetVideoViewImpl();
    Actor self = videoView->Self();
    if(self)
    {
      self.RemovePropertyNotification(mPositionUpdateNotification);
      self.RemovePropertyNotification(mSizeUpdateNotification);
      self.RemovePropertyNotification(mScaleUpdateNotification);
    }

    Toolkit::ControlImpl& controlImpl = GetImplementation(control);

    if(videoView->GetOverlayVisual())
    {
      Dali::Toolkit::DevelControl::UnregisterVisual(controlImpl, Dali::Toolkit::VideoView::Property::OVERLAY);

      if(Dali::Adaptor::IsAvailable() && videoView->GetOverlayVisual())
      {
        Dali::Toolkit::VisualFactory::Get().DiscardVisual(videoView->GetOverlayVisual());
      }
      videoView->SetOverlayVisual(Dali::Toolkit::Visual::Base());
    }

    ResetOverlayTextureVisual();
  }
}

bool WindowSurfaceStrategy::Initialize()
{
  VideoView* videoView = GetVideoViewImpl();

  Actor self = videoView->Self();

  if(!self.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    // When the control is off the stage, it does not have Window.
    return true;
  }

  Dali::Window window = DevelWindow::Get(self);
  window.ResizeSignal().Connect(videoView, &VideoView::OnWindowResized);

  mPositionUpdateNotification = self.AddPropertyNotification(Actor::Property::WORLD_POSITION, StepCondition(1.0f, 1.0f));
  mSizeUpdateNotification     = self.AddPropertyNotification(Actor::Property::SIZE, StepCondition(1.0f, 1.0f));
  mScaleUpdateNotification    = self.AddPropertyNotification(Actor::Property::WORLD_SCALE, StepCondition(0.1f, 1.0f));
  mPositionUpdateNotification.NotifySignal().Connect(this, &WindowSurfaceStrategy::UpdateDisplayArea);
  mSizeUpdateNotification.NotifySignal().Connect(this, &WindowSurfaceStrategy::UpdateDisplayArea);
  mScaleUpdateNotification.NotifySignal().Connect(this, &WindowSurfaceStrategy::UpdateDisplayArea);

  Toolkit::VideoView videoViewHandle = GetVideoView();
  Toolkit::Control      control     = Toolkit::Control::DownCast(videoViewHandle);
  Toolkit::ControlImpl& controlImpl = GetImplementation(control);

  if(!videoView->GetOverlayVisual())
  {
    Property::Map properties;
    properties[Dali::Toolkit::Visual::Property::TYPE]      = Dali::Toolkit::Visual::Type::COLOR;
    properties[Dali::Toolkit::Visual::Property::MIX_COLOR] = Color::BLACK;

    Dali::Toolkit::Visual::Base overlayVisual = Dali::Toolkit::VisualFactory::Get().CreateVisual(properties);
    videoView->SetOverlayVisual(overlayVisual);
    if(videoView->GetOverlayVisual())
    {
      Dali::Toolkit::Visual::Base overlayVisual = videoView->GetOverlayVisual();
      Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(overlayVisual);

      Renderer renderer = visualImpl.GetRenderer();

      //// For underlay rendering mode, video display area have to be transparent.
      // Note :  The actuall result is like this.
      //
      // Final RGB = (Dest RGB) * (Dest A - Src A) / (Dest A)
      // Final A   = (Dest A - Src A)
      //
      // But their is limitation that we cannot explain (1 - Src A / Dest A) by blend factor.
      // So it will have problem if we overlap 2 or more Underlay VideoView.
      // Else, most of cases are Dest A == 1. So just use ONE_MINUS_SRC_ALPHA as DEST_RGB.
      renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
      renderer.SetProperty(Renderer::Property::BLEND_FACTOR_SRC_RGB, BlendFactor::ZERO);
      renderer.SetProperty(Renderer::Property::BLEND_FACTOR_DEST_RGB, BlendFactor::ONE_MINUS_SRC_ALPHA);
      renderer.SetProperty(Renderer::Property::BLEND_FACTOR_SRC_ALPHA, BlendFactor::ONE);
      renderer.SetProperty(Renderer::Property::BLEND_FACTOR_DEST_ALPHA, BlendFactor::ONE);
      renderer.SetProperty(Renderer::Property::BLEND_EQUATION_RGB, BlendEquation::ADD);
      renderer.SetProperty(Renderer::Property::BLEND_EQUATION_ALPHA, BlendEquation::REVERSE_SUBTRACT);

      // Ignore corner radius for offscreen case.
      Toolkit::GetImplementation(overlayVisual).CornerRadiusIgnoredAtOffscreenRendering(true);
      Dali::Toolkit::DevelControl::RegisterVisual(controlImpl, Dali::Toolkit::VideoView::Property::OVERLAY, overlayVisual);

      // Sync corner values to Control
      Dali::Toolkit::DevelControl::EnableCornerPropertiesOverridden(controlImpl, overlayVisual, true);
    }

    // Note VideoPlayer::SetRenderingTarget resets all the options. (e.g. url, mute, looping)
    videoView->GetVideoPlayer().SetRenderingTarget(Dali::Adaptor::Get().GetNativeWindowHandle(self));
  }

  if(!mOverlayTextureVisual && videoView->IsUsingOverlayTexture())
  {
    CreateOverlayTextureVisual();
  }

  return true;
}

void WindowSurfaceStrategy::UpdateDisplayArea(Dali::PropertyNotification& source)
{
  VideoView* videoView = GetVideoViewImpl();

  Actor self(videoView->Self());

  bool    positionUsesAnchorPoint = self.GetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT).Get<bool>();
  Vector3 actorSize               = self.GetCurrentProperty<Vector3>(Actor::Property::SIZE) * self.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  Vector3 anchorPointOffSet       = actorSize * (positionUsesAnchorPoint ? self.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT) : AnchorPoint::TOP_LEFT);

  Vector2 screenPosition = self.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  Dali::DisplayArea displayArea;
  displayArea.x      = screenPosition.x - anchorPointOffSet.x;
  displayArea.y      = screenPosition.y - anchorPointOffSet.y;
  displayArea.width  = actorSize.x;
  displayArea.height = actorSize.y;

  videoView->GetVideoPlayer().SetDisplayArea(displayArea);
}

void WindowSurfaceStrategy::Play()
{
  if(!mOverlayTextureVisual)
  {
    return;
  }

  Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(mOverlayTextureVisual);
  Renderer                renderer   = visualImpl.GetRenderer();

  renderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::CONTINUOUSLY);
}

void WindowSurfaceStrategy::Pause()
{
  if(!mOverlayTextureVisual)
  {
    return;
  }

  Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(mOverlayTextureVisual);
  Renderer                renderer   = visualImpl.GetRenderer();

  renderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::IF_REQUIRED);
}

void WindowSurfaceStrategy::Stop()
{
  if(!mOverlayTextureVisual)
  {
    return;
  }

  Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(mOverlayTextureVisual);
  Renderer                renderer   = visualImpl.GetRenderer();

  renderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::IF_REQUIRED);
}


void WindowSurfaceStrategy::CreateOverlayTextureVisual()
{
  VideoView* videoView = GetVideoViewImpl();

  if(!mCurrentFrameTexture || mOverlayTextureVisual)
  {
    return;
  }

  std::string fragmentShaderString = std::string(SHADER_VIDEO_VIEW_SOURCE_FRAG);
  DevelTexture::ApplyNativeFragmentShader(mCurrentFrameTexture, fragmentShaderString, 3);

  Property::Map shaderMap;
  shaderMap[Toolkit::Visual::Shader::Property::VERTEX_SHADER]   = SHADER_VIDEO_VIEW_SOURCE_VERT.data();
  shaderMap[Toolkit::Visual::Shader::Property::FRAGMENT_SHADER] = ToPropertyValue(fragmentShaderString);
  shaderMap[Toolkit::Visual::Shader::Property::RENDER_PASS_TAG] = 11;
  shaderMap[Toolkit::Visual::Shader::Property::HINTS]           = static_cast<Shader::Hint::Value>(Shader::Hint::FILE_CACHE_SUPPORT | Shader::Hint::INTERNAL);
  shaderMap[Toolkit::Visual::Shader::Property::NAME]            = "VIDEO_VIEW_OVERLAY_SOURCE_TEXTURE";

  Property::Map properties;
  properties[Toolkit::Visual::Property::TYPE]   = Toolkit::Visual::Type::COLOR;
  properties[Toolkit::Visual::Property::SHADER] = shaderMap;
  mOverlayTextureVisual                         = Toolkit::VisualFactory::Get().CreateVisual(properties);

  if(mOverlayTextureVisual)
  {
    Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(mOverlayTextureVisual);
    Renderer                renderer   = visualImpl.GetRenderer();

    if(videoView->IsPlay())
    {
      renderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::CONTINUOUSLY);
    }

    Shader shader = renderer.GetShader();
    shader.RegisterProperty("cornerRadius", Vector4::ZERO);
    shader.RegisterProperty("cornerRadiusPolicy", Toolkit::Visual::Transform::Policy::ABSOLUTE);
    shader.RegisterProperty("cornerSquareness", Vector4::ZERO);

    Actor                     self = videoView->Self();
    Toolkit::VideoView handle = Toolkit::VideoView::DownCast(self);
    if(mOverlayTextureVisualIndex == Property::INVALID_INDEX)
    {
      mOverlayTextureVisualIndex = handle.RegisterProperty("videoViewTextureVisual", "videoViewTextureVisual", Property::AccessMode::READ_WRITE);
    }

    Toolkit::VideoView videoViewHandle = GetVideoView();
    Toolkit::Control       control     = Toolkit::Control::DownCast(videoViewHandle);
    Toolkit::ControlImpl&  controlImpl = GetImplementation(control);

    // Ignore corner radius for offscreen case.
    visualImpl.CornerRadiusIgnoredAtOffscreenRendering(true);
    Toolkit::DevelControl::RegisterVisual(controlImpl, mOverlayTextureVisualIndex, mOverlayTextureVisual);

    Dali::TextureSet textures = Dali::TextureSet::New();
    textures.SetTexture(0, mPreviousFrameTexture);
    textures.SetTexture(1, mCurrentFrameTexture);
    renderer.SetTextures(textures);
    Toolkit::DevelControl::EnableCornerPropertiesOverridden(controlImpl, mOverlayTextureVisual, true);
  }
}

void WindowSurfaceStrategy::ResetOverlayTextureVisual()
{
  if(mOverlayTextureVisual && mOverlayTextureVisualIndex != Property::INVALID_INDEX)
  {
    Toolkit::VideoView videoViewHandle = GetVideoView();
    Toolkit::Control      control      = Toolkit::Control::DownCast(videoViewHandle);
    Toolkit::ControlImpl& controlImpl  = GetImplementation(control);
    Toolkit::DevelControl::UnregisterVisual(controlImpl, mOverlayTextureVisualIndex);

    if(Dali::Adaptor::IsAvailable() && mOverlayTextureVisual)
    {
      Toolkit::VisualFactory::Get().DiscardVisual(mOverlayTextureVisual);
    }
    mOverlayTextureVisual.Reset();
  }

  mPreviousFrameTexture.Reset();
  mCurrentFrameTexture.Reset();
}

void WindowSurfaceStrategy::EnableOffscreenFrameRendering(bool useOffScreenFrame)
{
  VideoView* videoView = GetVideoViewImpl();

  if(!mOverlayTextureVisual && useOffScreenFrame)
  {
    NativeImagePtr previousNativeImage = Dali::NativeImage::New(0, 0, NativeImage::ColorDepth::COLOR_DEPTH_DEFAULT);
    mPreviousFrameTexture              = Dali::Texture::New(*previousNativeImage);

    NativeImagePtr currentNativeImage  = Dali::NativeImage::New(0, 0, NativeImage::ColorDepth::COLOR_DEPTH_DEFAULT);
    mCurrentFrameTexture               = Dali::Texture::New(*currentNativeImage);

    CreateOverlayTextureVisual();

    videoView->GetVideoPlayer().EnableOffscreenFrameRendering(useOffScreenFrame, previousNativeImage, currentNativeImage);
  }

  if(mOverlayTextureVisual && !useOffScreenFrame)
  {
    ResetOverlayTextureVisual();
    videoView->GetVideoPlayer().EnableOffscreenFrameRendering(useOffScreenFrame, nullptr, nullptr);
  }
}

//=============================================================================
// NativeImageStrategy
//=============================================================================

NativeImageStrategy::NativeImageStrategy(Toolkit::VideoView videoViewHandle)
: VideoRenderingStrategy(videoViewHandle)
{
}

NativeImageStrategy::~NativeImageStrategy()
{
  Toolkit::VideoView videoViewHandle = GetVideoView();
  Toolkit::Control control = Toolkit::Control::DownCast(videoViewHandle);
  if(control)
  {
    Toolkit::ControlImpl& controlImpl = GetImplementation(control);
    VideoView* videoView = GetVideoViewImpl();

    if(videoView->GetTextureVisual())
    {
      Dali::Toolkit::DevelControl::UnregisterVisual(controlImpl, Dali::Toolkit::VideoView::Property::TEXTURE);

      if(Dali::Adaptor::IsAvailable() && videoView->GetTextureVisual())
      {
        Dali::Toolkit::VisualFactory::Get().DiscardVisual(videoView->GetTextureVisual());
      }
      videoView->SetTextureVisual(Dali::Toolkit::Visual::Base());
    }

    videoView->SetNativeTexture(Dali::Texture());
  }
}

bool NativeImageStrategy::Initialize()
{
  VideoView* videoView = GetVideoViewImpl();

  if(videoView->GetVideoPlayer().IsVideoTextureSupported() == false)
  {
    DALI_LOG_ERROR("Platform doesn't support decoded video frame images\n");
    return false;
  }

  Actor self(videoView->Self());

  Toolkit::VideoView videoViewHandle = GetVideoView();
  Toolkit::Control      control      = Toolkit::Control::DownCast(videoViewHandle);
  Toolkit::ControlImpl& controlImpl  = GetImplementation(control);

  Any                        source;
  Dali::NativeImagePtr nativeImagePtr = Dali::NativeImage::New(source);
  videoView->SetNativeTexture(Dali::Texture::New(*nativeImagePtr));

  if(!videoView->GetTextureVisual())
  {
    Dali::Texture nativeTexture = videoView->GetNativeTexture();
    Dali::Toolkit::ImageUrl imageUrl = Dali::Toolkit::ImageUrl::New(nativeTexture);

    Property::Map shaderSource = CreateShader();

    Property::Map properties;
    properties[Dali::Toolkit::Visual::Property::TYPE]     = Dali::Toolkit::Visual::Type::IMAGE;
    properties[Dali::Toolkit::ImageVisual::Property::URL] = imageUrl.GetUrl();
    properties[Dali::Toolkit::Visual::Property::SHADER]   = shaderSource;

    Dali::Toolkit::Visual::Base textureVisual = Dali::Toolkit::VisualFactory::Get().CreateVisual(properties);
    videoView->SetTextureVisual(textureVisual);
    if(textureVisual)
    {
      // Ignore corner radius for offscreen case.
      Toolkit::GetImplementation(textureVisual).CornerRadiusIgnoredAtOffscreenRendering(true);
      Dali::Toolkit::DevelControl::RegisterVisual(controlImpl, Dali::Toolkit::VideoView::Property::TEXTURE, textureVisual);
      Dali::Toolkit::DevelControl::EnableCornerPropertiesOverridden(controlImpl, textureVisual, true);
    }

    videoView->GetVideoPlayer().SetRenderingTarget(nativeImagePtr);
  }

  return true;
}

void NativeImageStrategy::UpdateDisplayArea(Dali::PropertyNotification& source)
{
  // Nothing to do for NativeImageStrategy
}

void NativeImageStrategy::Play()
{
}

void NativeImageStrategy::Pause()
{
}

void NativeImageStrategy::Stop()
{
}

void NativeImageStrategy::UpdateProperties(const Property::Map& properties)
{
  VideoView* videoView = GetVideoViewImpl();

  if(videoView->GetTextureVisual() && !properties.Empty())
  {
    Toolkit::VideoView videoViewHandle = GetVideoView();
    Toolkit::Control      control      = Toolkit::Control::DownCast(videoViewHandle);
    Toolkit::ControlImpl& controlImpl  = GetImplementation(control);

    Property::Map visualProperties;
    visualProperties[Toolkit::Visual::Property::TYPE]   = Toolkit::Visual::Type::COLOR;
    visualProperties[Toolkit::Visual::Property::SHADER] = CreateShader();

    if(Dali::Adaptor::IsAvailable() && videoView->GetTextureVisual())
    {
      Toolkit::VisualFactory::Get().DiscardVisual(videoView->GetTextureVisual());
    }
    videoView->SetTextureVisual(Dali::Toolkit::Visual::Base());

    Dali::Toolkit::Visual::Base textureVisual = Dali::Toolkit::VisualFactory::Get().CreateVisual(visualProperties);
    videoView->SetTextureVisual(textureVisual);
    if(textureVisual)
    {
      // Ignore corner radius for offscreen case.
      Toolkit::GetImplementation(textureVisual).CornerRadiusIgnoredAtOffscreenRendering(true);
      Toolkit::DevelControl::RegisterVisual(controlImpl, Toolkit::VideoView::Property::TEXTURE, textureVisual);
      Toolkit::DevelControl::EnableCornerPropertiesOverridden(controlImpl, textureVisual, true);
    }
  }
}

Property::Map NativeImageStrategy::CreateShader()
{
  VideoView* videoView = GetVideoViewImpl();

  std::string fragmentShader;
  std::string vertexShader;
  std::string customFragmentShader;
  bool        checkShader = false;

  if(!videoView->GetEffectPropertyMap().Empty())
  {
    Property::Value* vertexShaderValue = videoView->GetEffectPropertyMap().Find(CUSTOM_VERTEX_SHADER);
    if(vertexShaderValue)
    {
      checkShader = GetStringFromProperty(*vertexShaderValue, vertexShader);
    }

    if(!vertexShaderValue || !checkShader)
    {
      vertexShader = SHADER_VIDEO_VIEW_TEXTURE_VERT.data();
    }

    Property::Value* fragmentShaderValue = videoView->GetEffectPropertyMap().Find(CUSTOM_FRAGMENT_SHADER);
    if(fragmentShaderValue)
    {
      checkShader = GetStringFromProperty(*fragmentShaderValue, customFragmentShader);

      if(checkShader)
      {
        fragmentShader = customFragmentShader;
      }
    }

    if(!fragmentShaderValue || !checkShader)
    {
      fragmentShader = SHADER_VIDEO_VIEW_TEXTURE_FRAG.data();
      Dali::Texture nativeTexture = videoView->GetNativeTexture();
      DevelTexture::ApplyNativeFragmentShader(nativeTexture, fragmentShader, 1);
    }
  }
  else
  {
    vertexShader   = SHADER_VIDEO_VIEW_TEXTURE_VERT.data();
    fragmentShader = SHADER_VIDEO_VIEW_TEXTURE_FRAG.data();
    Dali::Texture nativeTexture = videoView->GetNativeTexture();
      DevelTexture::ApplyNativeFragmentShader(nativeTexture, fragmentShader, 1);
  }

  Property::Map shader;
  shader[Toolkit::Visual::Shader::Property::VERTEX_SHADER]   = ToPropertyValue(vertexShader);
  shader[Toolkit::Visual::Shader::Property::FRAGMENT_SHADER] = ToPropertyValue(fragmentShader);
  shader[Toolkit::Visual::Shader::Property::HINTS]           = Shader::Hint::NONE;
  shader[Toolkit::Visual::Shader::Property::NAME]            = "VIDEO_VIEW";

  return shader;
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
