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
#include "video-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/native-image-source-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/rendering/texture-devel.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <cstring>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/public-api/controls/video-view/video-view.h>
#include <dali-toolkit/public-api/image-loader/image-url.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>

#include <dali/devel-api/adaptor-framework/image-loading.h>

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
  return Toolkit::VideoView::New();
}

DALI_TYPE_REGISTRATION_BEGIN(Toolkit::VideoView, Toolkit::Control, Create);

DALI_PROPERTY_REGISTRATION(Toolkit, VideoView, "video", MAP, VIDEO)
DALI_PROPERTY_REGISTRATION(Toolkit, VideoView, "looping", BOOLEAN, LOOPING)
DALI_PROPERTY_REGISTRATION(Toolkit, VideoView, "muted", BOOLEAN, MUTED)
DALI_PROPERTY_REGISTRATION(Toolkit, VideoView, "volume", MAP, VOLUME)
DALI_PROPERTY_REGISTRATION(Toolkit, VideoView, "underlay", BOOLEAN, UNDERLAY)
DALI_PROPERTY_REGISTRATION(Toolkit, VideoView, "playPosition", INTEGER, PLAY_POSITION)
DALI_PROPERTY_REGISTRATION(Toolkit, VideoView, "displayMode", INTEGER, DISPLAY_MODE)
DALI_PROPERTY_REGISTRATION(Toolkit, VideoView, "texture", MAP, TEXTURE)
DALI_PROPERTY_REGISTRATION(Toolkit, VideoView, "overlay", MAP, OVERLAY)

DALI_SIGNAL_REGISTRATION(Toolkit, VideoView, "finished", FINISHED_SIGNAL)

DALI_ACTION_REGISTRATION(Toolkit, VideoView, "play", ACTION_VIDEOVIEW_PLAY)
DALI_ACTION_REGISTRATION(Toolkit, VideoView, "pause", ACTION_VIDEOVIEW_PAUSE)
DALI_ACTION_REGISTRATION(Toolkit, VideoView, "stop", ACTION_VIDEOVIEW_STOP)
DALI_ACTION_REGISTRATION(Toolkit, VideoView, "forward", ACTION_VIDEOVIEW_FORWARD)
DALI_ACTION_REGISTRATION(Toolkit, VideoView, "backward", ACTION_VIDEOVIEW_BACKWARD)

DALI_TYPE_REGISTRATION_END()

const char* const VOLUME_LEFT("volumeLeft");
const char* const VOLUME_RIGHT("volumeRight");

// 3.0 TC uses RENDERING_TARGET. It should be removed in next release
const char* const RENDERING_TARGET("renderingTarget");
const char* const WINDOW_SURFACE_TARGET("windowSurfaceTarget");
const char* const NATIVE_IMAGE_TARGET("nativeImageTarget");

const char* const CUSTOM_SHADER("shader");
const char* const CUSTOM_VERTEX_SHADER("vertexShader");
const char* const CUSTOM_FRAGMENT_SHADER("fragmentShader");

const char* const IS_VIDEO_VIEW_PROPERTY_NAME = "isVideoView";

} // namespace

VideoView::VideoView(Dali::VideoSyncMode syncMode)
: Control(ControlBehaviour(ACTOR_BEHAVIOUR_DEFAULT | DISABLE_STYLE_CHANGE_SIGNALS)),
  mCurrentVideoPlayPosition(0),
  mFrameID(0),
  mIsPlay(false),
  mIsUnderlay(true),
  mSyncMode(syncMode),
  mSiblingOrder(0),
  // For frame interpolation
  mInterpolationInterval(0.0f),
  mInterpolationFactorPropertyIndex(Property::INVALID_INDEX)
{
}

VideoView::~VideoView()
{
}

Toolkit::VideoView VideoView::New(VideoSyncMode syncMode)
{
  VideoView*         impl   = new VideoView(syncMode);
  Toolkit::VideoView handle = Toolkit::VideoView(*impl);

  impl->mVideoPlayer = Dali::VideoPlayer::New(impl->Self(), syncMode);
  impl->Initialize();
  return handle;
}

void VideoView::OnInitialize()
{
  Actor self = Self();
  mVideoPlayer.FinishedSignal().Connect(this, &VideoView::EmitSignalFinish);

  // Accessibility
  self.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::VIDEO);
  self.SetProperty(DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, true);

  // update self property
  self.RegisterProperty(IS_VIDEO_VIEW_PROPERTY_NAME, true, Property::READ_WRITE);
}

void VideoView::SetUrl(const std::string& url)
{
  mUrl = url;
  mPropertyMap.Clear();

  mVideoPlayer.SetUrl(mUrl);
}

void VideoView::SetPropertyMap(Property::Map map)
{
  mPropertyMap = map;

  Property::Value* target = map.Find(RENDERING_TARGET);
  std::string      targetType;

  if(target && target->Get(targetType) && targetType == WINDOW_SURFACE_TARGET)
  {
    mIsUnderlay = true;
    SetWindowSurfaceTarget();
  }
  else if(target && target->Get(targetType) && targetType == NATIVE_IMAGE_TARGET)
  {
    mIsUnderlay = false;
    SetNativeImageTarget();
  }

  // Custom shader
  Property::Value* shaderValue;
  if(!map.Empty())
  {
    shaderValue = map.Find(CUSTOM_SHADER);

    if(shaderValue)
    {
      Property::Map* shaderMap = shaderValue->GetMap();
      if(shaderMap)
      {
        mEffectPropertyMap = *shaderMap;
      }
    }
  }

  if(mTextureVisual && !mEffectPropertyMap.Empty())
  {
    Toolkit::Control control     = Toolkit::Control(GetOwner());
    Control&         controlImpl = GetImplementation(control);

    Property::Map properties;
    properties[Toolkit::Visual::Property::TYPE]   = Toolkit::Visual::Type::COLOR;
    properties[Toolkit::Visual::Property::SHADER] = CreateShader();

    // Regenerate mTextureVisual
    if(Dali::Adaptor::IsAvailable() && mTextureVisual)
    {
      Toolkit::VisualFactory::Get().DiscardVisual(mTextureVisual);
    }
    mTextureVisual.Reset();

    mTextureVisual = Toolkit::VisualFactory::Get().CreateVisual(properties);
    if(mTextureVisual)
    {
      Toolkit::DevelControl::RegisterVisual(controlImpl, Toolkit::VideoView::Property::TEXTURE, mTextureVisual);
      Toolkit::DevelControl::EnableCornerPropertiesOverridden(controlImpl, mTextureVisual, true);
    }
  }

  RelayoutRequest();
}

std::string VideoView::GetUrl()
{
  return mUrl;
}

void VideoView::SetLooping(bool looping)
{
  mVideoPlayer.SetLooping(looping);
}

bool VideoView::IsLooping()
{
  return mVideoPlayer.IsLooping();
}

void VideoView::Play()
{
  mVideoPlayer.Play();
  mIsPlay = true;
}

void VideoView::Pause()
{
  mVideoPlayer.Pause();
  mIsPlay = false;
}

void VideoView::Stop()
{
  mVideoPlayer.Stop();
  mIsPlay = false;
}

void VideoView::Forward(int millisecond)
{
  int curPos = mVideoPlayer.GetPlayPosition();

  int nextPos = curPos + millisecond;

  mVideoPlayer.SetPlayPosition(nextPos);
}

void VideoView::Backward(int millisecond)
{
  int curPos = mVideoPlayer.GetPlayPosition();

  int nextPos = curPos - millisecond;
  nextPos     = (nextPos < 0) ? 0 : nextPos;

  mVideoPlayer.SetPlayPosition(nextPos);
}

void VideoView::SetMute(bool mute)
{
  mVideoPlayer.SetMute(mute);
}

bool VideoView::IsMuted()
{
  return mVideoPlayer.IsMuted();
}

void VideoView::SetVolume(float left, float right)
{
  mVideoPlayer.SetVolume(left, right);
}

void VideoView::GetVolume(float& left, float& right)
{
  mVideoPlayer.GetVolume(left, right);
}

Dali::Toolkit::VideoView::VideoViewSignalType& VideoView::FinishedSignal()
{
  return mFinishedSignal;
}

void VideoView::EmitSignalFinish()
{
  if(!mFinishedSignal.Empty())
  {
    Dali::Toolkit::VideoView handle(GetOwner());
    mFinishedSignal.Emit(handle);
  }
}

bool VideoView::DoAction(BaseObject* object, const std::string& actionName, const Property::Map& attributes)
{
  bool ret = false;

  Dali::BaseHandle   handle(object);
  Toolkit::VideoView videoView = Toolkit::VideoView::DownCast(handle);

  if(!videoView)
  {
    return ret;
  }

  VideoView& impl = GetImpl(videoView);

  if(strcmp(actionName.c_str(), ACTION_VIDEOVIEW_PLAY) == 0)
  {
    impl.Play();
    ret = true;
  }
  else if(strcmp(actionName.c_str(), ACTION_VIDEOVIEW_PAUSE) == 0)
  {
    impl.Pause();
    ret = true;
  }
  else if(strcmp(actionName.c_str(), ACTION_VIDEOVIEW_STOP) == 0)
  {
    impl.Stop();
    ret = true;
  }
  else if(strcmp(actionName.c_str(), ACTION_VIDEOVIEW_FORWARD) == 0)
  {
    int millisecond = 0;
    if(attributes["videoForward"].Get(millisecond))
    {
      impl.Forward(millisecond);
      ret = true;
    }
  }
  else if(strcmp(actionName.c_str(), ACTION_VIDEOVIEW_BACKWARD) == 0)
  {
    int millisecond = 0;
    if(attributes["videoBackward"].Get(millisecond))
    {
      impl.Backward(millisecond);
      ret = true;
    }
  }

  return ret;
}

bool VideoView::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  Dali::BaseHandle handle(object);

  bool               connected(true);
  Toolkit::VideoView videoView = Toolkit::VideoView::DownCast(handle);

  if(0 == strcmp(signalName.c_str(), FINISHED_SIGNAL))
  {
    videoView.FinishedSignal().Connect(tracker, functor);
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void VideoView::SetPropertyInternal(Property::Index index, const Property::Value& value)
{
  switch(index)
  {
    case Toolkit::VideoView::Property::VIDEO:
    {
      std::string   videoUrl;
      Property::Map map;

      if(value.Get(videoUrl))
      {
        SetUrl(videoUrl);
      }
      else if(value.Get(map))
      {
        SetPropertyMap(map);
      }
      break;
    }
    case Toolkit::VideoView::Property::LOOPING:
    {
      bool looping;
      if(value.Get(looping))
      {
        SetLooping(looping);
      }
      break;
    }
    case Toolkit::VideoView::Property::MUTED:
    {
      bool mute;
      if(value.Get(mute))
      {
        SetMute(mute);
      }
      break;
    }
    case Toolkit::VideoView::Property::VOLUME:
    {
      Property::Map map;
      float         left, right;
      if(value.Get(map))
      {
        Property::Value* volumeLeft  = map.Find(VOLUME_LEFT);
        Property::Value* volumeRight = map.Find(VOLUME_RIGHT);
        if(volumeLeft && volumeLeft->Get(left) && volumeRight && volumeRight->Get(right))
        {
          SetVolume(left, right);
        }
      }
      break;
    }
    case Toolkit::VideoView::Property::UNDERLAY:
    {
      bool underlay;
      if(value.Get(underlay))
      {
        SetUnderlay(underlay);
      }
      break;
    }
    case Toolkit::VideoView::Property::PLAY_POSITION:
    {
      int pos;
      if(value.Get(pos))
      {
        SetPlayPosition(pos);
      }
      break;
    }
    case Toolkit::VideoView::Property::DISPLAY_MODE:
    {
      int mode;
      if(value.Get(mode))
      {
        SetDisplayMode(mode);
      }
      break;
    }
  }
}

void VideoView::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  Toolkit::VideoView videoView = Toolkit::VideoView::DownCast(Dali::BaseHandle(object));

  if(videoView)
  {
    VideoView& impl = GetImpl(videoView);

    impl.SetPropertyInternal(index, value);

    if(index != Toolkit::VideoView::Property::UNDERLAY)
    {
      // Backup values.
      // These values will be used when underlay mode is changed.
      impl.mPropertyBackup[index] = value;
    }
  }
}

Property::Value VideoView::GetProperty(BaseObject* object, Property::Index propertyIndex)
{
  Property::Value    value;
  Toolkit::VideoView videoView = Toolkit::VideoView::DownCast(Dali::BaseHandle(object));

  if(videoView)
  {
    VideoView& impl = GetImpl(videoView);

    switch(propertyIndex)
    {
      case Toolkit::VideoView::Property::VIDEO:
      {
        if(!impl.mUrl.empty())
        {
          value = impl.mUrl;
        }
        else if(!impl.mPropertyMap.Empty())
        {
          value = impl.mPropertyMap;
        }
        break;
      }
      case Toolkit::VideoView::Property::LOOPING:
      {
        value = impl.IsLooping();
        break;
      }
      case Toolkit::VideoView::Property::MUTED:
      {
        value = impl.IsMuted();
        break;
      }
      case Toolkit::VideoView::Property::VOLUME:
      {
        Property::Map map;
        float         left, right;

        impl.GetVolume(left, right);
        map.Insert(VOLUME_LEFT, left);
        map.Insert(VOLUME_RIGHT, right);
        value = map;
        break;
      }
      case Toolkit::VideoView::Property::UNDERLAY:
      {
        value = impl.IsUnderlay();
        break;
      }
      case Toolkit::VideoView::Property::PLAY_POSITION:
      {
        value = impl.GetPlayPosition();
        break;
      }
      case Toolkit::VideoView::Property::DISPLAY_MODE:
      {
        value = impl.GetDisplayMode();
        break;
      }
    }
  }

  return value;
}

void VideoView::SetDepthIndex(int depthIndex)
{
  if(mTextureVisual)
  {
    mTextureVisual.SetDepthIndex(depthIndex);
  }
}

void VideoView::OnSceneConnection(int depth)
{
  Actor self = Self();
  if(mIsUnderlay)
  {
    mSiblingOrder = self.GetProperty<int>(Dali::DevelActor::Property::SIBLING_ORDER);
    DevelActor::ChildOrderChangedSignal(self.GetParent()).Connect(this, &VideoView::OnChildOrderChanged);
    SetWindowSurfaceTarget();
  }

  DALI_LOG_RELEASE_INFO("Calls mVideoPlayer.SceneConnection()\n");
  mVideoPlayer.SceneConnection();
  Control::OnSceneConnection(depth);
}

void VideoView::OnSceneDisconnection()
{
  DALI_LOG_RELEASE_INFO("Calls mVideoPlayer.SceneDisconnection()\n");
  mVideoPlayer.SceneDisconnection();
  Control::OnSceneDisconnection();
}

void VideoView::OnSizeSet(const Vector3& targetSize)
{
  if(mIsUnderlay && mSyncMode == Dali::VideoSyncMode::ENABLED)
  {
    // TODO: SR Video shell's designed is completed,
    // it will be re-designed and implemented.
    // Until it is completed, the below code will be commented.

    // SetFrameRenderCallback();
    mVideoPlayer.StartSynchronization();
  }
  Control::OnSizeSet(targetSize);
}

void VideoView::OnChildOrderChanged(Actor actor)
{
  Actor self                = Self();
  int   currentSiblingOrder = self.GetProperty<int>(Dali::DevelActor::Property::SIBLING_ORDER);
  if(currentSiblingOrder != mSiblingOrder)
  {
    Actor parent = self.GetParent();
    Actor child;
    Actor upper;
    Actor lower;

    int numChildren = static_cast<int>(parent.GetChildCount());
    for(int i = 0; i < numChildren; i++)
    {
      child = parent.GetChildAt(i);
      if(!IsVideoView(child))
      {
        continue;
      }

      if(child == self)
      {
        continue;
      }

      if(i < currentSiblingOrder)
      {
        lower = child;
      }
      else if(i > currentSiblingOrder)
      {
        upper = child;
        break;
      }
    }

    if(lower)
    {
      Toolkit::VideoView lowerView = Toolkit::VideoView::DownCast(lower);
      mVideoPlayer.RaiseAbove(GetImpl(lowerView).GetVideoPlayer());
    }

    if(upper)
    {
      Toolkit::VideoView upperView = Toolkit::VideoView::DownCast(upper);
      mVideoPlayer.LowerBelow(GetImpl(upperView).GetVideoPlayer());
    }
    mSiblingOrder = currentSiblingOrder;
  }
}

Vector3 VideoView::GetNaturalSize()
{
  Vector3 size;
  size.x = mVideoSize.GetWidth();
  size.y = mVideoSize.GetHeight();

  if(size.x > 0 && size.y > 0)
  {
    size.z = std::min(size.x, size.y);
    return size;
  }
  else
  {
    return Control::GetNaturalSize();
  }
}

void VideoView::SetWindowSurfaceTarget()
{
  Actor self = Self();

  if(!self.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    // When the control is off the stage, it does not have Window.
    return;
  }

  Dali::Window window = DevelWindow::Get(self);
  window.ResizeSignal().Connect(this, &VideoView::OnWindowResized);

  int curPos = mVideoPlayer.GetPlayPosition();

  if(mIsPlay)
  {
    mVideoPlayer.Pause();
  }

  mPositionUpdateNotification = self.AddPropertyNotification(Actor::Property::WORLD_POSITION, StepCondition(1.0f, 1.0f));
  mSizeUpdateNotification     = self.AddPropertyNotification(Actor::Property::SIZE, StepCondition(1.0f, 1.0f));
  mScaleUpdateNotification    = self.AddPropertyNotification(Actor::Property::WORLD_SCALE, StepCondition(0.1f, 1.0f));
  mPositionUpdateNotification.NotifySignal().Connect(this, &VideoView::UpdateDisplayArea);
  mSizeUpdateNotification.NotifySignal().Connect(this, &VideoView::UpdateDisplayArea);
  mScaleUpdateNotification.NotifySignal().Connect(this, &VideoView::UpdateDisplayArea);

  Toolkit::Control            control     = Toolkit::Control(GetOwner());
  Toolkit::Internal::Control& controlImpl = GetImplementation(control);

  if(mTextureVisual)
  {
    Toolkit::DevelControl::UnregisterVisual(controlImpl, Toolkit::VideoView::Property::TEXTURE);

    if(Dali::Adaptor::IsAvailable() && mTextureVisual)
    {
      Toolkit::VisualFactory::Get().DiscardVisual(mTextureVisual);
    }
    mTextureVisual.Reset();
  }

  // Note VideoPlayer::SetRenderingTarget resets all the options. (e.g. url, mute, looping)
  mVideoPlayer.SetRenderingTarget(Dali::Adaptor::Get().GetNativeWindowHandle(self));

  ApplyBackupProperties();

  if(!mOverlayVisual)
  {
    Property::Map properties;
    properties[Toolkit::Visual::Property::TYPE]      = Toolkit::Visual::Type::COLOR;
    properties[Toolkit::Visual::Property::MIX_COLOR] = Color::BLACK;

    mOverlayVisual = Toolkit::VisualFactory::Get().CreateVisual(properties);
    if(mOverlayVisual)
    {
      Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(mOverlayVisual);

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

      Toolkit::DevelControl::RegisterVisual(controlImpl, Toolkit::VideoView::Property::OVERLAY, mOverlayVisual);

      // Sync corner values to Control
      Toolkit::DevelControl::EnableCornerPropertiesOverridden(controlImpl, mOverlayVisual, true);
    }
  }

  if(!mOverlayTextureVisual)
  {
    CreateOverlayTextureVisual();
  }

  if(mIsPlay)
  {
    Play();
  }

  if(curPos > 0)
  {
    mVideoPlayer.SetPlayPosition(curPos);
  }
}

void VideoView::SetNativeImageTarget()
{
  if(mVideoPlayer.IsVideoTextureSupported() == false)
  {
    DALI_LOG_ERROR("Platform doesn't support decoded video frame images\n");
    mIsUnderlay = true;
    return;
  }

  if(mIsPlay)
  {
    mVideoPlayer.Pause();
  }

  Actor self(Self());

  Toolkit::Control            control     = Toolkit::Control(GetOwner());
  Toolkit::Internal::Control& controlImpl = GetImplementation(control);

  if(mOverlayVisual)
  {
    Toolkit::DevelControl::UnregisterVisual(controlImpl, Toolkit::VideoView::Property::OVERLAY);

    if(Dali::Adaptor::IsAvailable() && mOverlayVisual)
    {
      Toolkit::VisualFactory::Get().DiscardVisual(mOverlayVisual);
    }
    mOverlayVisual.Reset();
  }

  if(mOverlayTextureVisual && mOverlayTextureVisualIndex != Property::INVALID_INDEX)
  {
    Toolkit::DevelControl::UnregisterVisual(controlImpl, mOverlayTextureVisualIndex);

    if(Dali::Adaptor::IsAvailable() && mOverlayTextureVisual)
    {
      Toolkit::VisualFactory::Get().DiscardVisual(mOverlayTextureVisual);
    }
    mOverlayTextureVisual.Reset();
  }

  // Reset frame interpolation related members as they are not used in native image target mode
  mPreviousFrameTexture.Reset();
  mCurrentFrameTexture.Reset();
  mInterpolationInterval            = 0.0f;
  mInterpolationFactorPropertyIndex = Property::INVALID_INDEX;
  if(mInterpolationAnimation)
  {
    mInterpolationAnimation.Stop();
    mInterpolationAnimation.Clear();
  }

  self.RemovePropertyNotification(mPositionUpdateNotification);
  self.RemovePropertyNotification(mSizeUpdateNotification);
  self.RemovePropertyNotification(mScaleUpdateNotification);

  int curPos = mVideoPlayer.GetPlayPosition();

  Any                        source;
  Dali::NativeImageSourcePtr nativeImageSourcePtr = Dali::NativeImageSource::New(source);
  mNativeTexture                                  = Dali::Texture::New(*nativeImageSourcePtr);

  if(!mTextureVisual)
  {
    Toolkit::ImageUrl imageUrl = Toolkit::ImageUrl::New(mNativeTexture);

    Property::Map shaderSource = CreateShader();

    Property::Map properties;
    properties[Toolkit::Visual::Property::TYPE]     = Toolkit::Visual::Type::IMAGE;
    properties[Toolkit::ImageVisual::Property::URL] = imageUrl.GetUrl();
    properties[Toolkit::Visual::Property::SHADER]   = shaderSource;

    mTextureVisual = Toolkit::VisualFactory::Get().CreateVisual(properties);
    if(mTextureVisual)
    {
      Toolkit::DevelControl::RegisterVisual(controlImpl, Toolkit::VideoView::Property::TEXTURE, mTextureVisual);
      Toolkit::DevelControl::EnableCornerPropertiesOverridden(controlImpl, mTextureVisual, true);
    }
  }

  // Note VideoPlayer::SetRenderingTarget resets all the options. (e.g. url, mute, looping)
  mVideoPlayer.SetRenderingTarget(nativeImageSourcePtr);

  ApplyBackupProperties();

  if(mIsPlay)
  {
    Play();
  }

  if(curPos > 0)
  {
    mVideoPlayer.SetPlayPosition(curPos);
  }
}

void VideoView::UpdateDisplayArea(Dali::PropertyNotification& source)
{
  // If mSyncMode is enabled, Video player's size and poistion is updated in Video player's constraint.
  // Because video view and player should be work syncronization.

  // TODO: SR Video shell's designed is completed,
  // it will be re-designed and implemented.
  // Until it is completed, the below code will be commented.
  if(!mIsUnderlay /* || mSyncMode == Dali::VideoSyncMode::ENABLED */)
  {
    return;
  }

  Actor self(Self());

  bool    positionUsesAnchorPoint = self.GetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT).Get<bool>();
  Vector3 actorSize               = self.GetCurrentProperty<Vector3>(Actor::Property::SIZE) * self.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  Vector3 anchorPointOffSet       = actorSize * (positionUsesAnchorPoint ? self.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT) : AnchorPoint::TOP_LEFT);

  Vector2 screenPosition = self.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  mDisplayArea.x      = screenPosition.x - anchorPointOffSet.x;
  mDisplayArea.y      = screenPosition.y - anchorPointOffSet.y;
  mDisplayArea.width  = actorSize.x;
  mDisplayArea.height = actorSize.y;

  mVideoPlayer.SetDisplayArea(mDisplayArea);
}

void VideoView::SetUnderlay(bool set)
{
  if(set != mIsUnderlay)
  {
    mIsUnderlay = set;

    if(mIsUnderlay)
    {
      SetWindowSurfaceTarget();
    }
    else
    {
      SetNativeImageTarget();
    }

    RelayoutRequest();
  }
}

bool VideoView::IsUnderlay()
{
  return mIsUnderlay;
}

void VideoView::SetSWCodec(bool on)
{
  // If setting SW or HW type is failed , video-view shows video by default codec type.
  // The default codec type is selected by platform.
  if(on)
  {
    mVideoPlayer.SetCodecType(Dali::VideoPlayerPlugin::CodecType::SW);
  }
  else
  {
    mVideoPlayer.SetCodecType(Dali::VideoPlayerPlugin::CodecType::HW);
  }
}

int VideoView::GetPlayPosition()
{
  return mVideoPlayer.GetPlayPosition();
}

void VideoView::SetPlayPosition(int pos)
{
  mVideoPlayer.SetPlayPosition(pos);
}

void VideoView::SetDisplayMode(int mode)
{
  mVideoPlayer.SetDisplayMode(static_cast<Dali::VideoPlayerPlugin::DisplayMode::Type>(mode));
}

int VideoView::GetDisplayMode() const
{
  return static_cast<int>(mVideoPlayer.GetDisplayMode());
}

Any VideoView::GetMediaPlayer()
{
  return mVideoPlayer.GetMediaPlayer();
}

void VideoView::OnAnimationFinished(Animation& animation)
{
  // send desync
  // TODO: SR Video shell's designed is completed,
  // it will be re-designed and implemented.
  // Until it is completed, the below code will be commented.

  // SetFrameRenderCallback();
}

void VideoView::OnWindowResized(Dali::Window winHandle, Dali::Window::WindowSize size)
{
  Dali::VideoPlayerPlugin::DisplayRotation videoAngle  = mVideoPlayer.GetDisplayRotation();
  int                                      windowAngle = (DevelWindow::GetPhysicalOrientation(winHandle) / 90);

  if(windowAngle != videoAngle)
  {
    mVideoPlayer.SetDisplayRotation(static_cast<Dali::VideoPlayerPlugin::DisplayRotation>(windowAngle));
  }
}

void VideoView::PlayAnimation(Dali::Animation animation)
{
  if(mIsUnderlay && mSyncMode == Dali::VideoSyncMode::ENABLED)
  {
    mVideoPlayer.StartSynchronization();
    animation.FinishedSignal().Connect(this, &VideoView::OnAnimationFinished);
  }
  animation.Play();
}

Property::Map VideoView::CreateShader()
{
  std::string fragmentShader;
  std::string vertexShader;
  std::string customFragmentShader;
  bool        checkShader = false;

  if(!mEffectPropertyMap.Empty())
  {
    Property::Value* vertexShaderValue = mEffectPropertyMap.Find(CUSTOM_VERTEX_SHADER);
    if(vertexShaderValue)
    {
      checkShader = GetStringFromProperty(*vertexShaderValue, vertexShader);
    }

    if(!vertexShaderValue || !checkShader)
    {
      vertexShader = SHADER_VIDEO_VIEW_TEXTURE_VERT.data();
    }

    Property::Value* fragmentShaderValue = mEffectPropertyMap.Find(CUSTOM_FRAGMENT_SHADER);
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
      DevelTexture::ApplyNativeFragmentShader(mNativeTexture, fragmentShader, 1);
    }
  }
  else
  {
    vertexShader   = SHADER_VIDEO_VIEW_TEXTURE_VERT.data();
    fragmentShader = SHADER_VIDEO_VIEW_TEXTURE_FRAG.data();
    DevelTexture::ApplyNativeFragmentShader(mNativeTexture, fragmentShader, 1);
  }

  Property::Map shader;
  shader[Toolkit::Visual::Shader::Property::VERTEX_SHADER]   = vertexShader;
  shader[Toolkit::Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShader;
  shader[Toolkit::Visual::Shader::Property::HINTS]           = Shader::Hint::NONE;
  shader[Toolkit::Visual::Shader::Property::NAME]            = "VIDEO_VIEW";

  return shader;
}

bool VideoView::GetStringFromProperty(const Dali::Property::Value& value, std::string& output)
{
  bool extracted = false;
  if(value.Get(output))
  {
    extracted = true;
  }

  return extracted;
}

void VideoView::ApplyBackupProperties()
{
  Property::Map::SizeType pos   = 0;
  Property::Map::SizeType count = mPropertyBackup.Count();

  for(; pos < count; pos++)
  {
    KeyValuePair property = mPropertyBackup.GetKeyValue(pos);

    SetPropertyInternal(property.first.indexKey, property.second);
  }
}

void VideoView::FrameRenderCallback(int frameID)
{
  // send desync
  if(frameID == mFrameID)
  {
    mVideoPlayer.FinishSynchronization();
    mFrameID = 0;
  }
}

void VideoView::SetFrameRenderCallback()
{
  mFrameID++;
  DevelWindow::AddFrameRenderedCallback(DevelWindow::Get(Self()),
                                        std::unique_ptr<CallbackBase>(MakeCallback(this, &VideoView::FrameRenderCallback)),
                                        mFrameID);
}

void VideoView::CreateOverlayTextureVisual()
{
  if(!mCurrentFrameTexture || mOverlayTextureVisual)
  {
    return;
  }

  std::string fragmentShaderString = std::string(SHADER_VIDEO_VIEW_SOURCE_FRAG);
  DevelTexture::ApplyNativeFragmentShader(mCurrentFrameTexture, fragmentShaderString, 2);

  //// For underlay rendering mode, video display area have to be transparent.
  Property::Map shaderMap;
  shaderMap[Toolkit::Visual::Shader::Property::VERTEX_SHADER]   = SHADER_VIDEO_VIEW_SOURCE_VERT.data();
  shaderMap[Toolkit::Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShaderString;
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

    // Set default(prevent trash values)
    Shader shader = renderer.GetShader();
    shader.RegisterProperty("cornerRadius", Vector4::ZERO);
    shader.RegisterProperty("cornerRadiusPolicy", Toolkit::Visual::Transform::Policy::ABSOLUTE);
    shader.RegisterProperty("cornerSquareness", Vector4::ZERO);
    mInterpolationFactorPropertyIndex = shader.RegisterProperty("uInterpolationFactor", 0.0f);

    auto                     self = Self();
    Dali::Toolkit::VideoView handle(Dali::Toolkit::VideoView::DownCast(self));
    if(mOverlayTextureVisualIndex == Property::INVALID_INDEX)
    {
      mOverlayTextureVisualIndex = handle.RegisterProperty("videoViewTextureVisual", "videoViewTextureVisual", Property::AccessMode::READ_WRITE);
    }
    Toolkit::Control control     = Toolkit::Control(GetOwner());
    Control&         controlImpl = GetImplementation(control);
    Toolkit::DevelControl::RegisterVisual(controlImpl, mOverlayTextureVisualIndex, mOverlayTextureVisual);

    Dali::TextureSet textures = Dali::TextureSet::New();
    textures.SetTexture(0, mPreviousFrameTexture);
    textures.SetTexture(1, mCurrentFrameTexture);
    renderer.SetTextures(textures);
    // Sync corner values to Control
    Toolkit::DevelControl::EnableCornerPropertiesOverridden(controlImpl, mOverlayTextureVisual, true);
  }
}

bool VideoView::IsVideoView(Actor actor) const
{
  // Check whether the actor is a VideoView
  bool isVideoView = false;

  if(actor)
  {
    Property::Index propertyIsVideoView = actor.GetPropertyIndex(IS_VIDEO_VIEW_PROPERTY_NAME);
    if(propertyIsVideoView != Property::INVALID_INDEX)
    {
      isVideoView = actor.GetProperty<bool>(propertyIsVideoView);
    }
  }

  return isVideoView;
}

VideoPlayer VideoView::GetVideoPlayer()
{
  return mVideoPlayer;
}

void VideoView::SetAutoRotationEnabled(bool enable)
{
  mVideoPlayer.SetAutoRotationEnabled(enable);
}

bool VideoView::IsAutoRotationEnabled() const
{
  return mVideoPlayer.IsAutoRotationEnabled();
}

void VideoView::SetLetterBoxEnabled(bool enable)
{
  mVideoPlayer.SetLetterBoxEnabled(enable);
}

bool VideoView::IsLetterBoxEnabled() const
{
  return mVideoPlayer.IsLetterBoxEnabled();
}

void VideoView::SetFrameInterpolationInterval(float intervalSeconds)
{
  mInterpolationInterval = intervalSeconds;
  // If not interpolating, just update the interval. It will be used on the next SetNativeImageSourceForCurrentFrame.
}

float VideoView::GetFrameInterpolationInterval() const
{
  return mInterpolationInterval;
}

void VideoView::SetNativeImageSourceForCurrentFrame(NativeImageSourcePtr nativeImageSource)
{
  if(!nativeImageSource)
  {
    return;
  }

  mPreviousFrameTexture = mCurrentFrameTexture;
  mCurrentFrameTexture  = Texture::New(*nativeImageSource);

  if(!Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    mPreviousFrameTexture = mCurrentFrameTexture;
    return;
  }

  if(!mOverlayTextureVisual)
  {
    mPreviousFrameTexture = (mPreviousFrameTexture) ? mPreviousFrameTexture : mCurrentFrameTexture;
    CreateOverlayTextureVisual();
  }

  Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(mOverlayTextureVisual);
  Renderer                renderer   = visualImpl.GetRenderer();

  TextureSet textures = renderer.GetTextures();
  textures.SetTexture(0u, mPreviousFrameTexture);
  textures.SetTexture(1u, mCurrentFrameTexture);

  if(mInterpolationAnimation && mInterpolationAnimation.GetState() == Dali::Animation::State::PLAYING)
  {
    mInterpolationAnimation.Stop();
    mInterpolationAnimation.Clear();
  }

  mInterpolationAnimation.Reset();

  Shader shader = renderer.GetShader();
  if(mCurrentFrameTexture != mPreviousFrameTexture && mInterpolationInterval > 0.0f)
  {
    // Use KeyFrames to ensure the animation always starts from 0.0f
    KeyFrames interpolationKeyFrames = KeyFrames::New();
    interpolationKeyFrames.Add(0.0f, 0.0f); // At time 0.0s, value is 0.0f
    interpolationKeyFrames.Add(1.0f, 1.0f); // At time 1.0f, value is 1.0f

    // Set default(prevent trash values)
    mInterpolationAnimation = Animation::New(mInterpolationInterval);
    mInterpolationAnimation.AnimateBetween(Dali::Property(shader, mInterpolationFactorPropertyIndex), interpolationKeyFrames, AlphaFunction::LINEAR);
    mInterpolationAnimation.Play();
  }
  else
  {
    // Show current texture
    shader.SetProperty(mInterpolationFactorPropertyIndex, 1.0f);
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
