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

// CLASS HEADER

#include <dali-toolkit/public-api/factory/localized-control-factory.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

#include <dali-toolkit/internal/factory/localized-control-factory-impl.h>

namespace Dali
{

namespace Toolkit
{

Dali::Toolkit::TextView LocalizedControlFactory::CreateLocalizedTextView( const std::string& textID, const std::string& textDomain, const std::string& textViewTheme )
{
  LocalizedControlFactory factory = Get();
  return GetImpl(factory).CreateLocalizedTextView(textID, textDomain, textViewTheme);
}

LocalizedControlFactory::LocalizedControlFactory()
{
}

LocalizedControlFactory::~LocalizedControlFactory()
{
}

LocalizedControlFactory LocalizedControlFactory::Get()
{
  LocalizedControlFactory factory;

  // Check whether the focus factory is already created
  Dali::Adaptor& adaptor = Dali::Adaptor::Get();
  Dali::BaseHandle handle = adaptor.GetSingleton(typeid(LocalizedControlFactory));
  if(handle)
  {
    // If so, downcast the handle of singleton to focus factory
    factory = LocalizedControlFactory(dynamic_cast<Internal::LocalizedControlFactory*>(handle.GetObjectPtr()));
  }

  if(!factory)
  {
    // If not, create the focus factory and register it as a singleton
    factory = LocalizedControlFactory(new Internal::LocalizedControlFactory());
    adaptor.RegisterSingleton(typeid(factory), factory);
  }

  return factory;
}

LocalizedControlFactory::LocalizedControlFactory(Internal::LocalizedControlFactory *impl)
  : BaseHandle(impl)
{
}

} // namespace Toolkit

} // namespace Dali
