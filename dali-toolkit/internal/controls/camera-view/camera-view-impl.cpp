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
#include "camera-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/native-image-source.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <cstring>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/public-api/controls/camera-view/camera-view.h>

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
: Control(ControlBehaviour(ACTOR_BEHAVIOUR_DEFAULT | DISABLE_STYLE_CHANGE_SIGNALS)),
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
  Control::OnSceneConnection(depth);
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
  Control::OnSceneDisconnection();
  Actor self = Self();
  if(mTextureRenderer)
  {
    self.RemoveRenderer(mTextureRenderer);
    mTextureRenderer.Reset();
  }

  if(mOverlayRenderer)
  {
    self.RemoveRenderer(mOverlayRenderer);
    mOverlayRenderer.Reset();
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
  Geometry geometry = VisualFactoryCache::CreateQuadGeometry();
  Shader   shader   = Shader::New(SHADER_VIDEO_VIEW_VERT, SHADER_VIDEO_VIEW_FRAG, Shader::Hint::NONE, "CAMERA_VIEW_OVERLAY");
  mOverlayRenderer  = Renderer::New(geometry, shader);
  mOverlayRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::OFF);

  Self().AddRenderer(mOverlayRenderer);

  // Note CameraPlayer::SetWindowRenderingTarget
  mCameraPlayer.SetWindowRenderingTarget(DevelWindow::Get(self));
}

void CameraView::SetNativeImageTarget()
{
  Actor self(Self());

  self.RemovePropertyNotification(mPositionUpdateNotification);
  self.RemovePropertyNotification(mSizeUpdateNotification);
  self.RemovePropertyNotification(mScaleUpdateNotification);

  Any                        source;
  Dali::NativeImageSourcePtr nativeImageSourcePtr = Dali::NativeImageSource::New(source);
  mNativeTexture                                  = Dali::Texture::New(*nativeImageSourcePtr);

  Dali::Geometry   geometry   = VisualFactoryCache::CreateQuadGeometry();
  Dali::Shader     shader     = CreateShader(nativeImageSourcePtr);
  Dali::TextureSet textureSet = Dali::TextureSet::New();
  textureSet.SetTexture(0u, mNativeTexture);

  mTextureRenderer = Renderer::New(geometry, shader);
  mTextureRenderer.SetTextures(textureSet);

  Self().AddRenderer(mTextureRenderer);

  // Note CameraPlayer::SetNativeImageRenderingTarget.
  mCameraPlayer.SetNativeImageRenderingTarget(nativeImageSourcePtr);
}

void CameraView::UpdateDisplayArea(Dali::PropertyNotification& source)
{
  if(mDisplayType != Dali::Toolkit::CameraView::DisplayType::WINDOW)
  {
    return;
  }

  Actor self(Self());

  bool    positionUsesAnchorPoint = self.GetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT).Get<bool>();
  Vector3 actorSize               = self.GetCurrentProperty<Vector3>(Actor::Property::SIZE) * self.GetCurrentProperty<Vector3>(Actor::Property::WORLD_SCALE);
  Vector3 anchorPointOffSet       = actorSize * (positionUsesAnchorPoint ? self.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT) : AnchorPoint::TOP_LEFT);

  Vector2 screenPosition = self.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  mDisplayArea.x      = screenPosition.x - anchorPointOffSet.x;
  mDisplayArea.y      = screenPosition.y - anchorPointOffSet.y;
  mDisplayArea.width  = actorSize.x;
  mDisplayArea.height = actorSize.y;

  mCameraPlayer.SetDisplayArea(mDisplayArea);
}

Dali::Shader CameraView::CreateShader(Dali::NativeImageSourcePtr nativeImageSourcePtr)
{
  std::string vertexShader   = SHADER_VIDEO_VIEW_TEXTURE_VERT.data();
  std::string fragmentShader = SHADER_VIDEO_VIEW_TEXTURE_FRAG.data();

  nativeImageSourcePtr->ApplyNativeFragmentShader(fragmentShader);

  return Dali::Shader::New(vertexShader, fragmentShader, Shader::Hint::NONE, "CAMERA_VIEW");
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali