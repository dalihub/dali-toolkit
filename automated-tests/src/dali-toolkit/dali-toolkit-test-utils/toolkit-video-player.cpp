/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include "toolkit-video-player.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/any.h>

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
  {
    mMuted = false;
    mLooping = false;
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

public:

  std::string mUrl;
  float mVolumeLeft;
  float mVolumeRight;
  Dali::VideoPlayerPlugin::VideoPlayerSignalType mFinishedSignal;

private:

  bool mMuted;
  bool mLooping;
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
}

int VideoPlayer::GetPlayPosition()
{
  return 0;
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

} // namespace Dali;

