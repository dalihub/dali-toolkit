/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/adaptor-framework/video-player.h>
#include <dali/devel-api/adaptor-framework/video-sync-mode.h>
#include <dali/public-api/object/any.h>
#include <dali/public-api/object/base-object.h>
#include <toolkit-application.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class VideoPlayer: public Dali::BaseObject
{
public:

  VideoPlayer()
  : mUrl(),
    mVolumeLeft( 0.0f ),
    mVolumeRight( 0.0f ),
    mFinishedSignal(),
    mMuted( false ),
    mLooping( false),
    mPlayPosition( 0 ),
    mDisplyMode( Dali::VideoPlayerPlugin::DisplayMode::DST_ROI )
  {
  }

  void SetMuted( bool muted )
  {
    mMuted = muted;
  }

  bool IsMuted()
  {
    return mMuted;
  }

  void SetLooping( bool looping )
  {
    mLooping = looping;
  }

  bool IsLooping()
  {
    return mLooping;
  }

  void Stop()
  {
    if( !mFinishedSignal.Empty() )
    {
      mFinishedSignal.Emit();
    }
  }

  int GetPlayPosition()
  {
    return mPlayPosition;
  }

  void SetPlayPosition( int pos )
  {
    mPlayPosition = pos;
  }

  Dali::VideoPlayerPlugin::DisplayMode::Type GetDisplayMode() const
  {
    return mDisplyMode;
  }

  void SetDisplayMode( Dali::VideoPlayerPlugin::DisplayMode::Type mode )
  {
    mDisplyMode = mode;
  }

  Any GetMediaPlayer()
  {
    return NULL;
  }

  void StartSynchronization()
  {

  }

  void FinishSynchronization()
  {

  }

  void RaiseAbove(Dali::VideoPlayer target)
  {

  }

  void LowerBelow(Dali::VideoPlayer target)
  {

  }

  void RaiseToTop()
  {

  }

  void LowerToBottom()
  {

  }

public:

  std::string mUrl;
  float mVolumeLeft;
  float mVolumeRight;
  Dali::VideoPlayerPlugin::VideoPlayerSignalType mFinishedSignal;

private:

  bool mMuted;
  bool mLooping;
  int mPlayPosition;
  Dali::VideoPlayerPlugin::DisplayMode::Type mDisplyMode;
};

inline VideoPlayer& GetImplementation( Dali::VideoPlayer& player )
{
  DALI_ASSERT_ALWAYS(player && "VideoPlayer handle is empty");
  BaseObject& handle = player.GetBaseObject();
  return static_cast< Internal::Adaptor::VideoPlayer& >( handle );
}

inline const VideoPlayer& GetImplementation( const Dali::VideoPlayer& player )
{
  DALI_ASSERT_ALWAYS(player && "VideoPlayer handle is empty");
  const BaseObject& handle = player.GetBaseObject();
  return static_cast< const Internal::Adaptor::VideoPlayer& >( handle );
}

} // namespace Adaptor

} // namespace Internal


/********************************************************************************/
/*********************************  PUBLIC CLASS  *******************************/
/********************************************************************************/

VideoPlayer::VideoPlayer()
{
}

VideoPlayer::VideoPlayer( Internal::Adaptor::VideoPlayer* internal )
: BaseHandle( internal )
{
}

VideoPlayer::~VideoPlayer()
{
}

VideoPlayer VideoPlayer::New()
{
  Internal::Adaptor::VideoPlayer* player = new Internal::Adaptor::VideoPlayer();

  return VideoPlayer( player );
}

VideoPlayer VideoPlayer::New( Dali::Actor actor, Dali::VideoSyncMode syncMode )
{
  Internal::Adaptor::VideoPlayer* player = new Internal::Adaptor::VideoPlayer();

  return VideoPlayer( player );
}

VideoPlayer::VideoPlayer( const VideoPlayer& player )
: BaseHandle( player )
{
}

VideoPlayer& VideoPlayer::operator=( const VideoPlayer& player )
{
  BaseHandle::operator=( player );
  return *this;
}

VideoPlayer VideoPlayer::DownCast( BaseHandle handle )
{
  VideoPlayer videoPlayer;
  return videoPlayer;
}

void VideoPlayer::SetUrl( const std::string& url )
{
  Internal::Adaptor::GetImplementation( *this ).mUrl = url;
}

std::string VideoPlayer::GetUrl()
{
  return Internal::Adaptor::GetImplementation( *this ).mUrl;
}

void VideoPlayer::SetLooping(bool looping)
{
  Internal::Adaptor::GetImplementation( *this ).SetLooping( looping );
}

bool VideoPlayer::IsLooping()
{
  return Internal::Adaptor::GetImplementation( *this ).IsLooping();
}

void VideoPlayer::Play()
{
}

void VideoPlayer::Pause()
{
}

void VideoPlayer::Stop()
{
  Internal::Adaptor::GetImplementation( *this ).Stop();
}

void VideoPlayer::SetMute( bool mute )
{
  Internal::Adaptor::GetImplementation( *this ).SetMuted( mute );
}

bool VideoPlayer::IsMuted()
{
  return Internal::Adaptor::GetImplementation( *this ).IsMuted();
}

void VideoPlayer::SetVolume( float left, float right )
{
  Internal::Adaptor::GetImplementation( *this ).mVolumeLeft = left;
  Internal::Adaptor::GetImplementation( *this ).mVolumeRight = right;
}

void VideoPlayer::GetVolume( float& left, float& right )
{
  left = Internal::Adaptor::GetImplementation( *this ).mVolumeLeft;
  right = Internal::Adaptor::GetImplementation( *this ).mVolumeRight;
}

void VideoPlayer::SetRenderingTarget( Any target )
{
}

void VideoPlayer::SetPlayPosition( int millisecond )
{
  Internal::Adaptor::GetImplementation( *this ).SetPlayPosition( millisecond );
}

int VideoPlayer::GetPlayPosition()
{
  return Internal::Adaptor::GetImplementation( *this ).GetPlayPosition();
}

void VideoPlayer::SetDisplayArea( DisplayArea area )
{
}

void VideoPlayer::SetDisplayRotation( Dali::VideoPlayerPlugin::DisplayRotation rotation )
{
}

Dali::VideoPlayerPlugin::DisplayRotation VideoPlayer::GetDisplayRotation()
{
  return Dali::VideoPlayerPlugin::ROTATION_NONE;
}

Dali::VideoPlayerPlugin::VideoPlayerSignalType& VideoPlayer::FinishedSignal()
{
  return Internal::Adaptor::GetImplementation( *this ).mFinishedSignal;
}

void VideoPlayer::Forward( int millisecond )
{
}

void VideoPlayer::Backward( int millisecond )
{
}

bool VideoPlayer::IsVideoTextureSupported()
{
  return ToolkitApplication::DECODED_IMAGES_SUPPORTED;
}

void VideoPlayer::SetCodecType( Dali::VideoPlayerPlugin::CodecType type )
{
}

Dali::VideoPlayerPlugin::CodecType VideoPlayer::GetCodecType() const
{
  return Dali::VideoPlayerPlugin::CodecType::DEFAULT;
}

void VideoPlayer::SetDisplayMode( Dali::VideoPlayerPlugin::DisplayMode::Type mode )
{
  Internal::Adaptor::GetImplementation( *this ).SetDisplayMode( mode );
}

Dali::VideoPlayerPlugin::DisplayMode::Type VideoPlayer::GetDisplayMode() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetDisplayMode();
}

Any VideoPlayer::GetMediaPlayer()
{
  return Internal::Adaptor::GetImplementation( *this ).GetMediaPlayer();
}

void VideoPlayer::StartSynchronization()
{
  Internal::Adaptor::GetImplementation( *this ).StartSynchronization();
}

void VideoPlayer::FinishSynchronization()
{
  Internal::Adaptor::GetImplementation( *this ).FinishSynchronization();
}

void VideoPlayer::RaiseAbove(Dali::VideoPlayer target)
{
  Internal::Adaptor::GetImplementation( *this ).RaiseAbove(target);
}

void VideoPlayer::LowerBelow(Dali::VideoPlayer target)
{
  Internal::Adaptor::GetImplementation( *this ).LowerBelow(target);
}

void VideoPlayer::RaiseToTop()
{
  Internal::Adaptor::GetImplementation( *this ).RaiseToTop();
}

void VideoPlayer::LowerToBottom()
{
  Internal::Adaptor::GetImplementation( *this ).LowerToBottom();
}

} // namespace Dali;

