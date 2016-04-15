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

#include <dali/devel-api/adaptor-framework/singleton-service.h>

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

namespace
{
Dali::IntrusivePtr<SingletonService> gSingletonService;
} // unnamed namespace


class SingletonService : public Dali::BaseObject
{
public:

  /**
   * Create a SingletonService.
   * This should only be called once by the Application class.
   * @return A newly created SingletonService.
   */
  static Dali::SingletonService New()
  {
    DALI_ASSERT_ALWAYS( 0 && "SingletonService New method used");
    gSingletonService = Dali::IntrusivePtr<SingletonService>( new SingletonService() );
    return Dali::SingletonService( gSingletonService.Get() );
  }

  /**
   * @copydoc Dali::SingletonService::Get()
   */
  static Dali::SingletonService Get()
  {
    Dali::SingletonService singletonService;
    if ( !gSingletonService )
    {
      gSingletonService = Dali::IntrusivePtr<SingletonService>( new SingletonService() );
    }
    return Dali::SingletonService( gSingletonService.Get() );
  }

  /**
   * @copydoc Dali::SingletonService::Register()
   */
  void Register( const std::type_info& info, BaseHandle singleton )
  {
    if( singleton )
    {
      mSingletonContainer.insert( SingletonPair( info.name(), singleton ) );
    }
  }

  /**
   * @copydoc Dali::SingletonService::UnregisterAll()
   */
  void UnregisterAll()
  {
    mSingletonContainer.clear();
  }

  /**
   * @copydoc Dali::SingletonService::GetSingleton()
   */
  BaseHandle GetSingleton( const std::type_info& info ) const
  {
    BaseHandle object;

    SingletonConstIter iter = mSingletonContainer.find(info.name());
    if( iter != mSingletonContainer.end() )
    {
      object = ( *iter ).second;
    }
    return object;
  }

private:

  /**
   * Private Constructor
   * @see SingletonService::New()
   */
  SingletonService()
  : mSingletonContainer()
  {
    // Can only have one instance of SingletonService
    DALI_ASSERT_ALWAYS( !gSingletonService && "Only one instance of SingletonService is allowed");
    gSingletonService = this;
  }

  /**
   * Virtual Destructor
   */
  virtual ~SingletonService()
  {
    gSingletonService = 0;
  }

  // Undefined
  SingletonService( const SingletonService& );
  SingletonService& operator=( SingletonService& );

private:
  typedef std::pair<std::string, BaseHandle> SingletonPair;
  typedef std::map<std::string, BaseHandle>  SingletonContainer;
  typedef SingletonContainer::const_iterator SingletonConstIter;

  SingletonContainer mSingletonContainer; ///< The container to look up singleton by its type name
};

} // namespace Adaptor
} // namespace Internal

////////////////////////////////////////////////////////////////////////////////////////////////////

inline Internal::Adaptor::SingletonService& GetImplementation(Dali::SingletonService& player)
{
  DALI_ASSERT_ALWAYS( player && "SingletonService handle is empty" );
  BaseObject& handle = player.GetBaseObject();
  return static_cast<Internal::Adaptor::SingletonService&>(handle);
}

inline const Internal::Adaptor::SingletonService& GetImplementation(const Dali::SingletonService& player)
{
  DALI_ASSERT_ALWAYS( player && "SingletonService handle is empty" );
  const BaseObject& handle = player.GetBaseObject();
  return static_cast<const Internal::Adaptor::SingletonService&>(handle);
}

SingletonService::SingletonService()
{
}

SingletonService SingletonService::New()
{
  return Internal::Adaptor::SingletonService::New();
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

void SingletonService::UnregisterAll()
{
  GetImplementation( *this ).UnregisterAll();
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
