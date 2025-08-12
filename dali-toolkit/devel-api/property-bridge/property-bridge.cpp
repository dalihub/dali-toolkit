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
#include "property-bridge.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali-toolkit/internal/property-bridge/property-bridge-impl.h>

namespace Dali
{
namespace Toolkit
{
PropertyBridge::PropertyBridge()
{
}

PropertyBridge::~PropertyBridge()
{
}

PropertyBridge PropertyBridge::Get()
{
  return Internal::PropertyBridge::Get();
}

PropertyBridge::PropertyBridge(Internal::PropertyBridge* impl)
: BaseHandle(impl)
{
}

std::string PropertyBridge::GetStringProperty(Actor actor, const std::string& propertyName)
{
  return GetImplementation(*this).GetStringProperty(actor, propertyName);
}

void PropertyBridgeRegisterStringGetter(StringGetterDelegate getter)
{
  Internal::PropertyBridge::RegisterStringGetter(getter);
}

} // namespace Toolkit

} // namespace Dali
