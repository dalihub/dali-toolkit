/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include "toolkit-singleton-service.h"

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{


namespace Internal
{
namespace Adaptor
{

/**
 * Stub for the SingletonService
 */
class SingletonService : public Dali::BaseObject
{
public:
  static Dali::SingletonService New();
  static Dali::SingletonService Get();
  void Register( const std::type_info& info, BaseHandle singleton );
  BaseHandle GetSingleton( const std::type_info& info ) const;

private:
  SingletonService();
  virtual ~SingletonService();

  // Undefined
  SingletonService( const SingletonService& );
  SingletonService& operator=( SingletonService& );

private:

  typedef std::pair<std::string, BaseHandle> SingletonPair;
  typedef std::map<std::string, BaseHandle>  SingletonContainer;
  typedef SingletonContainer::const_iterator SingletonConstIter;

  SingletonContainer mSingletonContainer; ///< The container to look up singleton by its type name

  static Dali::SingletonService mToolkitSingletonService;
};

Dali::SingletonService SingletonService::mToolkitSingletonService;

Dali::SingletonService SingletonService::New()
{
  return Get();
}

Dali::SingletonService SingletonService::Get()
{
  if( ! mToolkitSingletonService )
  {
    mToolkitSingletonService = Dali::SingletonService( new Dali::Internal::Adaptor::SingletonService );
  }
  return mToolkitSingletonService;
}

void SingletonService::Register( const std::type_info& info, BaseHandle singleton )
{
  if( singleton )
  {
    mSingletonContainer.insert( SingletonPair( info.name(), singleton ) );
  }
}

BaseHandle SingletonService::GetSingleton( const std::type_info& info ) const
{
  BaseHandle object;

  SingletonConstIter iter = mSingletonContainer.find(info.name());
  if( iter != mSingletonContainer.end() )
  {
    object = ( *iter ).second;
  }

  return object;
}

SingletonService::SingletonService()
: mSingletonContainer()
{
}

SingletonService::~SingletonService()
{
}

} // namespace Adaptor
} // namespace Internal

////////////////////////////////////////////////////////////////////////////////////////////////////

Internal::Adaptor::SingletonService& GetImplementation(Dali::SingletonService& player)
{
  DALI_ASSERT_ALWAYS( player && "SingletonService handle is empty" );

  BaseObject& handle = player.GetBaseObject();

  return static_cast<Internal::Adaptor::SingletonService&>(handle);
}

const Internal::Adaptor::SingletonService& GetImplementation(const Dali::SingletonService& player)
{
  DALI_ASSERT_ALWAYS( player && "SingletonService handle is empty" );

  const BaseObject& handle = player.GetBaseObject();

  return static_cast<const Internal::Adaptor::SingletonService&>(handle);
}

SingletonService::SingletonService()
{
}

SingletonService SingletonService::Get()
{
  return Internal::Adaptor::SingletonService::Get();
}

SingletonService::~SingletonService()
{
}

void SingletonService::Register( const std::type_info& info, BaseHandle singleton )
{
  GetImplementation( *this ).Register( info, singleton );
}

BaseHandle SingletonService::GetSingleton( const std::type_info& info ) const
{
  return GetImplementation( *this ).GetSingleton( info );
}

SingletonService::SingletonService( Internal::Adaptor::SingletonService* singletonService )
: BaseHandle( singletonService )
{
}

} // namespace Dali
