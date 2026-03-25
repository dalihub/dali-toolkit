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
#include "video-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/native-image-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/devel-api/rendering/texture-devel.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/animation/constraint.h>
#include <cstring>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/controls/video-view/video-view-rendering-strategy.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/public-api/controls/video-view/video-view.h>
#include <dali-toolkit/public-api/image-loader/image-url.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>

#include <dali/devel-api/adaptor-framework/image-loading.h>

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
: ControlImpl(ControlBehaviour(ACTOR_BEHAVIOUR_DEFAULT | DISABLE_STYLE_CHANGE_SIGNALS)),
  mCurrentVideoPlayPosition(0),
  mFrameID(0),
  mIsPlay(false),
  mIsUnderlay(true),
  mSyncMode(syncMode),
  mSiblingOrder(0),
  // For frame interpolation
  mInterpolationInterval(0.0f)
{
}

VideoView::~VideoView()
{
  if(mRenderingStrategy)
  {
    mRenderingStrategy.reset();
    mRenderingStrategy = nullptr;
  }
}

Toolkit::VideoView VideoView::New(VideoSyncMode syncMode)
{
  VideoView*         impl   = new VideoView(syncMode);
  Toolkit::VideoView handle = Toolkit::VideoView(*impl);

  impl->mVideoPlayer = Dali::VideoPlayer::New(impl->Self(), syncMode);
  impl->Initialize();
  return handle;
}

Toolkit::VideoView VideoView::New(Dali::VideoPlayerPlugin::PlayerHandle playerHandle, VideoSyncMode syncMode)
{
  VideoView*         impl   = new VideoView(syncMode);
  Toolkit::VideoView handle = Toolkit::VideoView(*impl);

  impl->mVideoPlayer = Dali::VideoPlayer::New(impl->Self(), playerHandle, syncMode);
  impl->Initialize();
  return handle;
}

void VideoView::OnInitialize()
{
  Actor self = Self();
  mVideoPlayer.EventSignal().Connect(this, &VideoView::OnVideoPlayerEvent);

  // Accessibility
  self.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::VIDEO);
  self.SetProperty(DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, true);

  // update self property
  self.RegisterProperty(IS_VIDEO_VIEW_PROPERTY_NAME, true, Property::READ_WRITE);


  if(!mRenderingStrategy)
  {
    Toolkit::VideoView videoViewHandle = Toolkit::VideoView::DownCast(self);

    if(mIsUnderlay)
    {
      mRenderingStrategy = std::make_unique<WindowSurfaceStrategy>(videoViewHandle);
    }
    else
    {
      mRenderingStrategy = std::make_unique<NativeImageStrategy>(videoViewHandle);
    }
  }

  if(!mRenderingStrategy->Initialize())
  {
    if(!mIsUnderlay)
    {
      mIsUnderlay = true;
      Toolkit::VideoView videoViewHandle = Toolkit::VideoView::DownCast(self);
      mRenderingStrategy = std::make_unique<WindowSurfaceStrategy>(videoViewHandle);
      mRenderingStrategy->Initialize();
    }
  }
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

  if(target && GetStdString(*target, targetType))
  {
    if(targetType == WINDOW_SURFACE_TARGET)
    {
      SetUnderlay(true);
    }
    else if(targetType == NATIVE_IMAGE_TARGET)
    {
      SetUnderlay(false);
    }
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

  if(mRenderingStrategy)
  {
    mRenderingStrategy->UpdateProperties(mEffectPropertyMap);
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

  if(mRenderingStrategy)
  {
    mRenderingStrategy->Play();
  }
}

void VideoView::Pause()
{
  mVideoPlayer.Pause();
  mIsPlay = false;

  if(mRenderingStrategy)
  {
    mRenderingStrategy->Pause();
  }
}

void VideoView::Stop()
{
  mVideoPlayer.Stop();
  mIsPlay = false;

  if(mRenderingStrategy)
  {
    mRenderingStrategy->Stop();
  }
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

void VideoView::OnVideoPlayerEvent(Dali::VideoPlayerPlugin::PlayerEventType event)
{
  switch(event)
  {
    case Dali::VideoPlayerPlugin::PlayerEventType::PLAYBACK_FINISHED:
    {
      if(!mFinishedSignal.Empty())
      {
        Dali::Toolkit::VideoView handle(GetOwner());
        mFinishedSignal.Emit(handle);
      }
      break;
    }
    case Dali::VideoPlayerPlugin::PlayerEventType::ERROR:
    {
      // Future: add Error signal emission here
      break;
    }
    default:
      break;
  }
}

bool VideoView::DoAction(BaseObject* object, const Dali::String& actionName, const Property::Map& attributes)
{
  bool ret = false;

  Dali::BaseHandle   handle(object);
  Toolkit::VideoView videoView = Toolkit::VideoView::DownCast(handle);

  if(!videoView)
  {
    return ret;
  }

  VideoView& impl = GetImpl(videoView);

  if(strcmp(actionName.CStr(), ACTION_VIDEOVIEW_PLAY) == 0)
  {
    impl.Play();
    ret = true;
  }
  else if(strcmp(actionName.CStr(), ACTION_VIDEOVIEW_PAUSE) == 0)
  {
    impl.Pause();
    ret = true;
  }
  else if(strcmp(actionName.CStr(), ACTION_VIDEOVIEW_STOP) == 0)
  {
    impl.Stop();
    ret = true;
  }
  else if(strcmp(actionName.CStr(), ACTION_VIDEOVIEW_FORWARD) == 0)
  {
    int millisecond = 0;
    if(attributes["videoForward"].Get(millisecond))
    {
      impl.Forward(millisecond);
      ret = true;
    }
  }
  else if(strcmp(actionName.CStr(), ACTION_VIDEOVIEW_BACKWARD) == 0)
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

bool VideoView::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const Dali::String& signalName, FunctorDelegate* functor)
{
  Dali::BaseHandle handle(object);

  bool               connected(true);
  Toolkit::VideoView videoView = Toolkit::VideoView::DownCast(handle);

  if(0 == strcmp(signalName.CStr(), FINISHED_SIGNAL))
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

      if(GetStdString(value, videoUrl))
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
          value = ToPropertyValue(impl.mUrl);
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
        map.Insert(Dali::String(VOLUME_LEFT), left);
        map.Insert(Dali::String(VOLUME_RIGHT), right);
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

  if(!mRenderingStrategy)
  {
    Toolkit::VideoView videoViewHandle = Toolkit::VideoView::DownCast(self);

    if(mIsUnderlay)
    {
      mRenderingStrategy = std::make_unique<WindowSurfaceStrategy>(videoViewHandle);
    }
    else
    {
      mRenderingStrategy = std::make_unique<NativeImageStrategy>(videoViewHandle);
    }
  }

  int curPos = mVideoPlayer.GetPlayPosition();
  bool wasPlaying = IsPlay();

  if(wasPlaying)
  {
    Pause();
  }

  if(!mRenderingStrategy->Initialize())
  {
    if(!mIsUnderlay)
    {
      mIsUnderlay = true;
      Toolkit::VideoView videoViewHandle = Toolkit::VideoView::DownCast(self);
      mRenderingStrategy = std::make_unique<WindowSurfaceStrategy>(videoViewHandle);
      mRenderingStrategy->Initialize();
    }
  }

  ApplyBackupProperties();

  if(wasPlaying)
  {
    Play();
  }

  if(curPos > 0)
  {
    mVideoPlayer.SetPlayPosition(curPos);
  }

  if(mIsUnderlay)
  {
    mSiblingOrder = self.GetProperty<int>(Dali::DevelActor::Property::SIBLING_ORDER);
    DevelActor::ChildOrderChangedSignal(self.GetParent()).Connect(this, &VideoView::OnChildOrderChanged);
  }

  DALI_LOG_RELEASE_INFO("Calls mVideoPlayer.SceneConnection()\n");
  mVideoPlayer.SceneConnection();
  ControlImpl::OnSceneConnection(depth);
}

void VideoView::OnSceneDisconnection()
{
  DALI_LOG_RELEASE_INFO("Calls mVideoPlayer.SceneDisconnection()\n");
  mVideoPlayer.SceneDisconnection();
  ControlImpl::OnSceneDisconnection();
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
  ControlImpl::OnSizeSet(targetSize);
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
    return ControlImpl::GetNaturalSize();
  }
}



void VideoView::SetUnderlay(bool isUnderlay)
{
  if(isUnderlay != mIsUnderlay || !mRenderingStrategy)
  {
    int curPos = mVideoPlayer.GetPlayPosition();
    bool wasPlaying = IsPlay();

    if(wasPlaying)
    {
      Pause();
    }

    Toolkit::VideoView videoViewHandle = Toolkit::VideoView::DownCast(Self());

    if(isUnderlay)
    {
      mRenderingStrategy = std::make_unique<WindowSurfaceStrategy>(videoViewHandle);
    }
    else
    {
      mRenderingStrategy = std::make_unique<NativeImageStrategy>(videoViewHandle);
    }

    if(!mRenderingStrategy->Initialize())
    {
      if(wasPlaying)
      {
        Play();
      }
      return;
    }

    mIsUnderlay = isUnderlay;

    ApplyBackupProperties();

    if(wasPlaying)
    {
      Play();
    }

    if(curPos > 0)
    {
      mVideoPlayer.SetPlayPosition(curPos);
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
                                        MakeCallback(this, &VideoView::FrameRenderCallback),
                                        mFrameID);
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
  mInterpolationInterval = std::max(0.0f, intervalSeconds);
  mVideoPlayer.SetFrameInterpolationInterval(intervalSeconds);
  // If not interpolating, just update the interval. It will be used on the next SetVideoFrameBuffer.
}

float VideoView::GetFrameInterpolationInterval() const
{
  return mInterpolationInterval;
}

void VideoView::EnableOffscreenFrameRendering(bool useOffScreenFrame)
{
  mIsUsingOverlayTexture = useOffScreenFrame;
  if(mRenderingStrategy)
  {
    mRenderingStrategy->EnableOffscreenFrameRendering(useOffScreenFrame);
  }
}

void VideoView::SetVideoFrameBuffer(Dali::NativeImagePtr source)
{
  mVideoPlayer.SetVideoFrameBuffer(source);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
