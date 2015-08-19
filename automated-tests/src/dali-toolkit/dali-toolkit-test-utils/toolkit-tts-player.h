#ifndef __DALI_TOOLKIT_TTS_PLAYER_H__
#define __DALI_TOOLKIT_TTS_PLAYER_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class TtsPlayer;
}
}

class DALI_IMPORT_API TtsPlayer : public BaseHandle
{
public:

  enum Mode
  {
    DEFAULT = 0,        ///< Default mode for normal application
    NOTIFICATION,       ///< Notification mode
    SCREEN_READER,      ///< Screen reader mode
    MODE_NUM
  };

  enum State
  {
    UNAVAILABLE = 0,    ///< Player is not available
    READY,              ///< Player is ready to play
    PLAYING,            ///< Player is playing
    PAUSED              ///< Player is paused
  };

  typedef Signal< void ( const Dali::TtsPlayer::State, const Dali::TtsPlayer::State ) > StateChangedSignalType;

  TtsPlayer();
  static TtsPlayer Get(Dali::TtsPlayer::Mode mode = Dali::TtsPlayer::DEFAULT);
  ~TtsPlayer();

  TtsPlayer(const TtsPlayer& handle);
  TtsPlayer& operator=(const TtsPlayer& rhs);
  void Play(const std::string& text);
  void Stop();
  void Pause();
  void Resume();
  State GetState();
  Dali::TtsPlayer::StateChangedSignalType& StateChangedSignal();

public:

  // explicit DALI_INTERNAL TtsPlayer( Internal::Adaptor::TtsPlayer* ttsPlayer );

private:

  StateChangedSignalType mStateChangedSignal;
};

} // namespace Dali

#endif // __DALI_TOOLKIT_TTS_PLAYER_H__
