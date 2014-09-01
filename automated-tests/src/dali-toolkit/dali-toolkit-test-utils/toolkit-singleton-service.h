#ifndef __DALI_TOOLKIT_TOOLKIT_SINGLETON_SERVICE_H__
#define __DALI_TOOLKIT_TOOLKIT_SINGLETON_SERVICE_H__

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

// EXTERNAL INCLUDES

#define __DALI_SINGELTON_SERVICE_H__
#include <typeinfo>
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES

namespace Dali
{

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class SingletonService;
}
}

class SingletonService : public BaseHandle
{
public:
  SingletonService();
  static SingletonService Get();
  ~SingletonService();
  void Register( const std::type_info& info, BaseHandle singleton );
  BaseHandle GetSingleton( const std::type_info& info ) const;

public: // Not intended for application developers
  SingletonService( Internal::Adaptor::SingletonService* singletonService );
};

} // namespace Dali

#endif // __DALI_TOOLKIT_TOOLKIT_SINGLETON_SERVICE_H__
