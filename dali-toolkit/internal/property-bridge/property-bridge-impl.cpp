/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include "property-bridge-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/common/singleton-service.h>
#include <dali/integration-api/debug.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
namespace Dali
{
namespace Toolkit
{
namespace Internal
{
StringGetterDelegate gStringGetter = nullptr;

namespace
{

} // namespace

PropertyBridge::PropertyBridge()
: mStringGetter(nullptr)
{

}

PropertyBridge::~PropertyBridge()
{
}

Dali::Toolkit::PropertyBridge PropertyBridge::Get()
{
  Dali::Toolkit::PropertyBridge bridgeHandle;

  Dali::SingletonService service(SingletonService::Get());
  if(service)
  {
    // Check whether the singleton is already created
    Dali::BaseHandle handle = service.GetSingleton(typeid(Dali::Toolkit::PropertyBridge));
    if(handle)
    {
      // If so, downcast the handle
      PropertyBridge* impl = dynamic_cast<Dali::Toolkit::Internal::PropertyBridge*>(handle.GetObjectPtr());
      bridgeHandle = Dali::Toolkit::PropertyBridge(impl);
    }
    else // create and register the object
    {
      bridgeHandle = Dali::Toolkit::PropertyBridge(new PropertyBridge);
      service.Register(typeid(bridgeHandle), bridgeHandle);
    }
  }

  return bridgeHandle;
}

std::string PropertyBridge::GetStringProperty(Dali::Actor actor, const std::string& propertyName)
{
  std::string result;
  if(EnsureStringGetter())
  {
    void* refObject = actor ? actor.GetObjectPtr() : nullptr;
    mStringGetter(refObject, propertyName.c_str(), &result);
  }
  return result;
}

bool PropertyBridge::EnsureStringGetter()
{
  if(!gStringGetter)
  {
    DALI_LOG_ERROR("StringGetter not registered!\n");
    return false;
  }

  if(!mStringGetter)
  {
    mStringGetter = gStringGetter;
  }
  return true;
}

// static
void PropertyBridge::RegisterStringGetter(StringGetterDelegate getter)
{
  if(getter)
  {
    gStringGetter = getter;
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
