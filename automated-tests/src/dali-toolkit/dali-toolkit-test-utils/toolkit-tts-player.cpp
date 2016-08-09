#include "toolkit-tts-player.h"

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

TtsPlayer::TtsPlayer()
{
}

TtsPlayer TtsPlayer::Get(Dali::TtsPlayer::Mode mode)
{
  TtsPlayer ttsPlayer;

  return ttsPlayer;
}

TtsPlayer::~TtsPlayer()
{
}

TtsPlayer::TtsPlayer(const TtsPlayer& handle)
: BaseHandle(handle)
{
}

TtsPlayer& TtsPlayer::operator=(const TtsPlayer& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void TtsPlayer::Play(const std::string& text)
{
  // GetImplementation(*this).Play(text);
}

void TtsPlayer::Stop()
{
  // GetImplementation(*this).Stop();
}

void TtsPlayer::Pause()
{
  // GetImplementation(*this).Pause();
}

void TtsPlayer::Resume()
{
  // GetImplementation(*this).Resume();
}

TtsPlayer::State TtsPlayer::GetState()
{
  return READY; // GetImplementation(*this).GetState();
}

TtsPlayer::StateChangedSignalType& TtsPlayer::StateChangedSignal()
{
  return mStateChangedSignal; // GetImplementation(*this).StateChangedSignal();
}

} // namespace Dali
