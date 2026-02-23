/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// CLASS HEADER

#include <dali-toolkit/public-api/styling/style-manager.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali-toolkit/internal/styling/style-manager-impl.h>
#include <dali/integration-api/string-utils.h>

using Dali::Integration::ToStdString;

namespace Dali
{
namespace Toolkit
{
StyleManager::StyleManager()
{
}

StyleManager::~StyleManager()
{
}

StyleManager StyleManager::Get()
{
  return Internal::StyleManager::Get();
}

void StyleManager::ApplyTheme(const Dali::String& themeFile)
{
  GetImpl(*this).ApplyTheme(ToStdString(themeFile));
}

void StyleManager::ApplyDefaultTheme()
{
  GetImpl(*this).ApplyDefaultTheme();
}

void StyleManager::SetStyleConstant(const Dali::String& key, const Property::Value& value)
{
  GetImpl(*this).SetStyleConstant(ToStdString(key), value);
}

bool StyleManager::GetStyleConstant(const Dali::String& key, Property::Value& valueOut)
{
  return GetImpl(*this).GetStyleConstant(ToStdString(key), valueOut);
}

void StyleManager::ApplyStyle(Toolkit::Control control, const Dali::String& jsonFileName, const Dali::String& styleName)
{
  GetImpl(*this).ApplyStyle(control, ToStdString(jsonFileName), ToStdString(styleName));
}

StyleManager::StyleChangedSignalType& StyleManager::StyleChangedSignal()
{
  return GetImpl(*this).StyleChangedSignal();
}

StyleManager::StyleManager(Internal::StyleManager* impl)
: BaseHandle(impl)
{
}

} // namespace Toolkit

} // namespace Dali
