#ifndef __DALI_TOOLKIT_VIDEO_PLAYER_H__
#define __DALI_TOOLKIT_VIDEO_PLAYER_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/base-object.h>

#include <dali/devel-api/adaptor-framework/video-player-plugin.h>

namespace Dali
{

class Any;

namespace Internal
{
namespace Adaptor
{

class VideoPlayer;

}
}

/**
 * @brief VideoPlayer class is used for video playback.
 * @SINCE_1_1.38
 */
class VideoPlayer: public BaseHandle
{
public:

  VideoPlayer();

  ~VideoPlayer();

  static VideoPlayer New();

  VideoPlayer( const VideoPlayer& player );

  VideoPlayer& operator=( const VideoPlayer& player );

  static VideoPlayer DownCast( BaseHandle handle );

  void SetUrl( const std::string& url );

  std::string GetUrl();

  void SetLooping(bool looping);

  bool IsLooping();

  void Play();

  void Pause();

  void Stop();

  void SetMute( bool mute );

  bool IsMuted();

  void SetVolume( float left, float right );

  void GetVolume( float& left, float& right );

  void SetRenderingTarget( Any target );

  void SetPlayPosition( int millisecond );

  int GetPlayPosition();

  void SetDisplayRotation( Dali::VideoPlayerPlugin::DisplayRotation rotation );

  Dali::VideoPlayerPlugin::DisplayRotation GetDisplayRotation();

  Dali::VideoPlayerPlugin::VideoPlayerSignalType& FinishedSignal();

private:

  VideoPlayer( Internal::Adaptor::VideoPlayer* internal );

};

} // namespace Dali;

#endif
