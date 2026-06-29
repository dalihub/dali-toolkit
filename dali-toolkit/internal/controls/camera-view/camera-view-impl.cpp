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
#include "camera-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/adaptor-framework/native-image.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <cstring>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/controls/control/control-renderers.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/public-api/controls/camera-view/camera-view.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>

using Dali::Integration::ToDaliStringView;

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
BaseHandle Create()
{
  return Toolkit::CameraView::New(NULL);
}

DALI_TYPE_REGISTRATION_BEGIN(Toolkit::CameraView, Toolkit::Control, Create)
DALI_TYPE_REGISTRATION_END()
} // namespace

CameraView::CameraView(Dali::Toolkit::CameraView::DisplayType displayType)
: ControlImpl(ControlBehaviour(static_cast<ControlBehaviour>(ACTOR_BEHAVIOUR_DEFAULT) | DISABLE_STYLE_CHANGE_SIGNALS)),
  mDisplayType(displayType)
{
}

CameraView::~CameraView()
{
}

Toolkit::CameraView CameraView::New(Any cameraHandle, Dali::Toolkit::CameraView::DisplayType type)
{
  CameraView*         impl   = new CameraView(type);
  Toolkit::CameraView handle = Toolkit::CameraView(*impl);
  impl->mCameraPlayer        = Dali::CameraPlayer::New();
  impl->Initialize();
  if(impl->mCameraPlayer)
  {
    impl->mCameraPlayer.SetCameraPlayer(cameraHandle);
  }
  return handle;
}

void CameraView::Update()
{
  UpdateDisplayArea(mSizeUpdateNotification);
}

void CameraView::OnSceneConnection(int depth)
{
  ControlImpl::OnSceneConnection(depth);
  if(mDisplayType == Dali::Toolkit::CameraView::DisplayType::WINDOW)
  {
    SetWindowSurfaceTarget();
  }
  else if(mDisplayType == Dali::Toolkit::CameraView::DisplayType::IMAGE)
  {
    SetNativeImageTarget();
  }
  RelayoutRequest();
}

void CameraView::OnSceneDisconnection()
{
  ControlImpl::OnSceneDisconnection();
  Actor self = Self();

  // Remove PropertyNotifications to prevent accumulation on repeated scene connections.
  self.RemovePropertyNotification(mPositionUpdateNotification);
  self.RemovePropertyNotification(mSizeUpdateNotification);
  self.RemovePropertyNotification(mScaleUpdateNotification);

  if(mTextureRenderer)
  {
    self.RemoveRenderer(mTextureRenderer);
    mTextureRenderer.Reset();
  }

  if(mOverlayVisual)
  {
    if(mOverlayVisualIndex != Property::INVALID_INDEX)
    {
      Toolkit::CameraView   cameraViewHandle = Toolkit::CameraView::DownCast(self);
      Toolkit::Control      control          = Toolkit::Control::DownCast(cameraViewHandle);
      Toolkit::ControlImpl& controlImpl      = GetImplementation(control);

      Dali::Toolkit::DevelControl::UnregisterVisual(controlImpl, mOverlayVisualIndex);
      // mOverlayVisualIndex is kept for reuse on next OnSceneConnection
    }

    if(Dali::Adaptor::IsAvailable())
    {
      Dali::Toolkit::VisualFactory::Get().DiscardVisual(mOverlayVisual);
    }
    mOverlayVisual.Reset();
  }
}

void CameraView::SetWindowSurfaceTarget()
{
  Actor self = Self();

  mPositionUpdateNotification = self.AddPropertyNotification(Actor::Property::WORLD_POSITION, StepCondition(1.0f, 1.0f));
  mSizeUpdateNotification     = self.AddPropertyNotification(Actor::Property::SIZE, StepCondition(1.0f, 1.0f));
  mScaleUpdateNotification    = self.AddPropertyNotification(Actor::Property::WORLD_SCALE, StepCondition(0.1f, 1.0f));
  mPositionUpdateNotification.NotifySignal().Connect(this, &CameraView::UpdateDisplayArea);
  mSizeUpdateNotification.NotifySignal().Connect(this, &CameraView::UpdateDisplayArea);
  mScaleUpdateNotification.NotifySignal().Connect(this, &CameraView::UpdateDisplayArea);

  // For underlay rendering mode, camera display area have to be transparent.
  // Use ColorVisual with special blend settings so that CornerRadius can cut alpha on the boundary.
  if(!mOverlayVisual)
  {
    Toolkit::CameraView   cameraViewHandle = Toolkit::CameraView::DownCast(self);
    Toolkit::Control      control          = Toolkit::Control::DownCast(cameraViewHandle);
    Toolkit::ControlImpl& controlImpl      = GetImplementation(control);

    Property::Map properties;
    properties[Dali::Toolkit::Visual::Property::TYPE]      = Dali::Toolkit::Visual::Type::COLOR;
    properties[Dali::Toolkit::Visual::Property::MIX_COLOR] = Color::BLACK;

    mOverlayVisual = Dali::Toolkit::VisualFactory::Get().CreateVisual(properties);
    if(mOverlayVisual)
    {
      Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(mOverlayVisual);
      Renderer                renderer   = visualImpl.GetRenderer();
      if(!renderer)
      {
        DALI_LOG_ERROR("CameraView: Failed to get renderer from overlay visual\n");
        mOverlayVisual.Reset();
        return;
      }

      // Blend settings identical to VideoView WindowSurfaceStrategy.
      // Final RGB  = DestRGB * (1 - SrcAlpha)
      // Final Alpha = DestAlpha - SrcAlpha  (REVERSE_SUBTRACT)
      // CornerRadius fragment shader writes (1 - opacity) into SrcAlpha,
      // so pixels outside the rounded corner become a hole in the UI layer.
      renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
      renderer.SetProperty(Renderer::Property::BLEND_FACTOR_SRC_RGB, BlendFactor::ZERO);
      renderer.SetProperty(Renderer::Property::BLEND_FACTOR_DEST_RGB, BlendFactor::ONE_MINUS_SRC_ALPHA);
      renderer.SetProperty(Renderer::Property::BLEND_FACTOR_SRC_ALPHA, BlendFactor::ONE);
      renderer.SetProperty(Renderer::Property::BLEND_FACTOR_DEST_ALPHA, BlendFactor::ONE);
      renderer.SetProperty(Renderer::Property::BLEND_EQUATION_RGB, BlendEquation::ADD);
      renderer.SetProperty(Renderer::Property::BLEND_EQUATION_ALPHA, BlendEquation::REVERSE_SUBTRACT);

      // Skip CornerRadius calculation during offscreen rendering pass.
      visualImpl.CornerRadiusIgnoredAtOffscreenRendering(true);

      // Register an internal property index for the visual.
      // CameraView has no dedicated Property::OVERLAY enum (DisplayType is fixed at construction),
      // so use RegisterProperty to obtain an index — same pattern as VideoView's mOverlayTextureVisualIndex.
      if(mOverlayVisualIndex == Property::INVALID_INDEX)
      {
        mOverlayVisualIndex = cameraViewHandle.RegisterProperty(
          "cameraViewOverlay", "cameraViewOverlay", Property::AccessMode::READ_WRITE);
      }

      Dali::Toolkit::DevelControl::RegisterVisual(controlImpl, mOverlayVisualIndex, mOverlayVisual);

      // Auto-sync Control's cornerRadius / cornerSquareness properties → shader uniforms.
      Dali::Toolkit::DevelControl::EnableCornerPropertiesOverridden(controlImpl, mOverlayVisual, true);
    }
  }

  // Note: CameraPlayer::SetWindowRenderingTarget resets player options (url, mute, etc.)
  mCameraPlayer.SetWindowRenderingTarget(Window::Get(self));
}

void CameraView::SetNativeImageTarget()
{
  Actor self(Self());

  self.RemovePropertyNotification(mPositionUpdateNotification);
  self.RemovePropertyNotification(mSizeUpdateNotification);
  self.RemovePropertyNotification(mScaleUpdateNotification);

  Any                  source;
  Dali::NativeImagePtr nativeImagePtr = Dali::NativeImage::New(source);
  mNativeTexture                      = Dali::Texture::New(*nativeImagePtr);

  Dali::Geometry   geometry   = VisualFactoryCache::CreateQuadGeometry();
  Dali::Shader     shader     = CreateShader(nativeImagePtr);
  Dali::TextureSet textureSet = Dali::TextureSet::New();
  textureSet.SetTexture(0u, mNativeTexture);

  mTextureRenderer = Renderer::New(geometry, shader);
  mTextureRenderer.SetTextures(textureSet);

  Self().AddRenderer(mTextureRenderer);

  // Note CameraPlayer::SetNativeImageRenderingTarget.
  mCameraPlayer.SetNativeImageRenderingTarget(nativeImagePtr);
}

void CameraView::UpdateDisplayArea(Dali::PropertyNotification source)
{
  if(mDisplayType != Dali::Toolkit::CameraView::DisplayType::WINDOW)
  {
    return;
  }

  Actor self(Self());

  bool    positionUsesPivot = self.GetProperty(Actor::Property::POSITION_USES_PIVOT).Get<bool>();
  Vector3 actorSize         = self.GetCurrentProperty<Vector3>(Actor::Property::SIZE) * self.GetCurrentProperty<Vector3>(Actor::Property::WORLD_SCALE);
  Vector3 pivotOffSet       = actorSize * (positionUsesPivot ? self.GetCurrentProperty<Vector3>(Actor::Property::PIVOT) : Pivot::TOP_LEFT);

  Vector2 screenPosition = self.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  mDisplayArea.x      = screenPosition.x - pivotOffSet.x;
  mDisplayArea.y      = screenPosition.y - pivotOffSet.y;
  mDisplayArea.width  = actorSize.x;
  mDisplayArea.height = actorSize.y;

  mCameraPlayer.SetDisplayArea(mDisplayArea);
}

Dali::Shader CameraView::CreateShader(Dali::NativeImagePtr nativeImagePtr)
{
  std::string  vertexShader = SHADER_VIDEO_VIEW_TEXTURE_VERT.data();
  Dali::String fragmentShader(ToDaliStringView(SHADER_VIDEO_VIEW_TEXTURE_FRAG));

  nativeImagePtr->ApplyNativeFragmentShader(fragmentShader, 1);

  Dali::Shader shader = Dali::Shader::New(ToDaliStringView(vertexShader), fragmentShader, static_cast<Shader::Hint::Value>(Shader::Hint::FILE_CACHE_SUPPORT | Shader::Hint::INTERNAL), "CAMERA_VIEW");
  return shader;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
