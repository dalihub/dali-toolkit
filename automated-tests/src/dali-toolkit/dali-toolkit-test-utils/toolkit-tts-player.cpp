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
#include <dali/dali.h>


namespace Dali
{

TtsPlayer::TtsPlayer()
{
}

namespace Internal
{
namespace Adaptor
{

class TtsPlayer : public BaseObject
{
public:
  TtsPlayer()
  {
  }

  void Play(const std::string& text)
  {
  }

  void Stop()
  {
  }

  void Pause()
  {
  }

  void Resume()
  {
  }

  Dali::TtsPlayer::State GetState()
  {
    return Dali::TtsPlayer::READY;
  }

  Dali::TtsPlayer::StateChangedSignalType& StateChangedSignal()
  {
    return mStateChangedSignal;
  }
private:
  Dali::TtsPlayer::StateChangedSignalType mStateChangedSignal;
};


inline TtsPlayer& GetImplementation(Dali::TtsPlayer& player)
{
  DALI_ASSERT_ALWAYS( player && "TtsPlayer handle is empty" );
  BaseObject& handle = player.GetBaseObject();
  return static_cast<Internal::Adaptor::TtsPlayer&>(handle);
}

inline const TtsPlayer& GetImplementation(const Dali::TtsPlayer& player)
{
  DALI_ASSERT_ALWAYS( player && "TtsPlayer handle is empty" );
  const BaseObject& handle = player.GetBaseObject();
  return static_cast<const Internal::Adaptor::TtsPlayer&>(handle);
}

} // Adaptor
} // Internal

static IntrusivePtr<Internal::Adaptor::TtsPlayer> ttsSingleton = NULL;

TtsPlayer TtsPlayer::Get(Dali::TtsPlayer::Mode mode)
{
  if( ! ttsSingleton )
  {
    ttsSingleton.Reset( new Dali::Internal::Adaptor::TtsPlayer() );
  }
  TtsPlayer playerHandle(ttsSingleton.Get());

  return playerHandle;
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
  return Internal::Adaptor::GetImplementation(*this).StateChangedSignal();
}

TtsPlayer::TtsPlayer( Internal::Adaptor::TtsPlayer* player )
: BaseHandle( player )
{
}

} // namespace Dali
