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

#include <dali/devel-api/adaptor-framework/feedback-player.h>
#include <dali/devel-api/adaptor-framework/singleton-service.h>
#include <dali/public-api/object/base-object.h>

using namespace Dali;

namespace Dali
{
class Adaptor;

namespace Internal
{
namespace Adaptor
{

class FeedbackPlayer : public Dali::BaseObject
{
public:
  static Dali::FeedbackPlayer New()
  {
    Dali::FeedbackPlayer player = Dali::FeedbackPlayer( new FeedbackPlayer() );
    return player;
  }

  static Dali::FeedbackPlayer Get()
  {
    Dali::FeedbackPlayer player;
    Dali::SingletonService service( Dali::SingletonService::Get() );
    if( service )
    {
      // Check whether the singleton is already created
      Dali::BaseHandle handle = service.GetSingleton( typeid( Dali::FeedbackPlayer ) );
      if( handle )
      {
        player = Dali::FeedbackPlayer( dynamic_cast< FeedbackPlayer* >( handle.GetObjectPtr() ) );
      }
      else
      {
        player = Dali::FeedbackPlayer( New() );
        service.Register( typeid( player ), player );
      }
    }
    return player;
  }
  void PlayMonotone(unsigned int duration)
  {
  }

  void PlayFile( const std::string& filePath )
  {
  }

  void Stop()
  {
  }

  int PlaySound( const std::string& fileName )
  {
    return 0;
  }

  void StopSound( int handle )
  {
  }

  void PlayFeedbackPattern( int type, int pattern )
  {
  }

  bool LoadFile(const std::string& filename, std::string& data)
  {
    return true;
  }

private:
  FeedbackPlayer()
  {
  }

  virtual ~FeedbackPlayer()
  {
  }

  FeedbackPlayer(const FeedbackPlayer&)
  {
  }

  FeedbackPlayer& operator=(FeedbackPlayer&)
  {
    return *this;
  }
};

} // Adaptor
} // Internal

inline Internal::Adaptor::FeedbackPlayer& GetImplementation(Dali::FeedbackPlayer& player)
{
  DALI_ASSERT_ALWAYS( player && "FeedbackPlayer handle is empty" );
  BaseObject& handle = player.GetBaseObject();
  return static_cast<Internal::Adaptor::FeedbackPlayer&>(handle);
}

inline const Internal::Adaptor::FeedbackPlayer& GetImplementation(const Dali::FeedbackPlayer& player)
{
  DALI_ASSERT_ALWAYS( player && "FeedbackPlayer handle is empty" );
  const BaseObject& handle = player.GetBaseObject();
  return static_cast<const Internal::Adaptor::FeedbackPlayer&>(handle);
}

FeedbackPlayer::FeedbackPlayer()
{
}

FeedbackPlayer FeedbackPlayer::Get()
{
  return Internal::Adaptor::FeedbackPlayer::Get();
}

FeedbackPlayer::~FeedbackPlayer()
{
}

void FeedbackPlayer::PlayMonotone(unsigned int duration)
{
  GetImplementation(*this).PlayMonotone(duration);
}

void FeedbackPlayer::PlayFile(const std::string filePath)
{
  GetImplementation(*this).PlayFile(filePath);
}

void FeedbackPlayer::Stop()
{
  GetImplementation(*this).Stop();
}

int FeedbackPlayer::PlaySound( const std::string& fileName )
{
  return GetImplementation(*this).PlaySound(fileName);
}

void FeedbackPlayer::StopSound( int handle )
{
  GetImplementation(*this).StopSound(handle);
}

void FeedbackPlayer::PlayFeedbackPattern( int type, int pattern )
{
  GetImplementation(*this).PlayFeedbackPattern(type, pattern);
}

bool FeedbackPlayer::LoadFile(const std::string& filename, std::string& data)
{
  return GetImplementation(*this).LoadFile(filename, data);
}

FeedbackPlayer::FeedbackPlayer( Internal::Adaptor::FeedbackPlayer* player )
: BaseHandle( player )
{
}

} // Dali
