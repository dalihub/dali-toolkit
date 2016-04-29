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
 */

#include <dali/public-api/object/base-object.h>
#include <dali/devel-api/adaptor-framework/singleton-service.h>
#include <dali/devel-api/adaptor-framework/sound-player.h>

using namespace Dali;

namespace Dali
{
class Adaptor;

namespace Internal
{
namespace Adaptor
{


class SoundPlayer : public Dali::BaseObject
{
public:
  static Dali::SoundPlayer New()
  {
    Dali::SoundPlayer player = Dali::SoundPlayer( new SoundPlayer() );
    return player;
  }

  static Dali::SoundPlayer Get()
  {
    Dali::SoundPlayer player;

    Dali::SingletonService service( Dali::SingletonService::Get() );
    if ( service )
    {
      // Check whether the singleton is already created
      Dali::BaseHandle handle = service.GetSingleton( typeid( Dali::SoundPlayer ) );
      if ( handle )
      {
        // If so, downcast the handle
        player = Dali::SoundPlayer( dynamic_cast< SoundPlayer* >( handle.GetObjectPtr() ) );
      }
      else
      {
        player = Dali::SoundPlayer( New() );
        service.Register( typeid( player ), player );
      }
    }
    return player;
  }

  int PlaySound(const std::string fileName)
  {
    return 0;
  }

  void Stop(int handle)
  {
  }

private:
  SoundPlayer()
  {
  }

  virtual ~SoundPlayer()
  {
  }

  SoundPlayer(const SoundPlayer&);
  SoundPlayer& operator=(SoundPlayer&);
};

} // Adaptor namespace
} // Internal namespace

inline Internal::Adaptor::SoundPlayer& GetImplementation(Dali::SoundPlayer& player)
{
  DALI_ASSERT_ALWAYS( player && "SoundPlayer handle is empty" );
  BaseObject& handle = player.GetBaseObject();
  return static_cast<Internal::Adaptor::SoundPlayer&>(handle);
}

inline const Internal::Adaptor::SoundPlayer& GetImplementation(const Dali::SoundPlayer& player)
{
  DALI_ASSERT_ALWAYS( player && "SoundPlayer handle is empty" );
  const BaseObject& handle = player.GetBaseObject();
  return static_cast<const Internal::Adaptor::SoundPlayer&>(handle);
}

SoundPlayer SoundPlayer::Get()
{
  return Internal::Adaptor::SoundPlayer::Get();
}

SoundPlayer::SoundPlayer()
{
}

SoundPlayer::SoundPlayer(Internal::Adaptor::SoundPlayer*player)
:BaseHandle(player)
{
}

SoundPlayer::~SoundPlayer()
{
}

int SoundPlayer::PlaySound(const std::string fileName)
{
  return GetImplementation(*this).PlaySound(fileName);
}

} // Dali
