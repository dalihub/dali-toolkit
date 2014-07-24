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
 */

// CLASS HEADER

#include <dali-toolkit/public-api/styling/style-manager.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

#include <dali-toolkit/internal/styling/style-manager-impl.h>

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

void StyleManager::SetOrientationValue( int orientation )
{
  GetImpl(*this).SetOrientationValue( orientation );
}

int StyleManager::GetOrientationValue()
{
  return GetImpl(*this).GetOrientationValue();
}

void StyleManager::SetOrientation( Orientation orientation )
{
  GetImpl(*this).SetOrientation( orientation );
}

Orientation StyleManager::GetOrientation()
{
  return GetImpl(*this).GetOrientation();
}

void StyleManager::SetStyleConstant( const std::string& key, const Property::Value& value )
{
  GetImpl(*this).SetStyleConstant( key, value );
}

bool StyleManager::GetStyleConstant( const std::string& key, Property::Value& valueOut )
{
  return GetImpl(*this).GetStyleConstant( key, valueOut );
}

void StyleManager::ApplyStyle( Toolkit::Control control, const std::string& jsonFileName, const std::string& styleName )
{
  GetImpl(*this).ApplyStyle( control, jsonFileName, styleName );
}

StyleManager::StyleManager( Internal::StyleManager *impl )
  : BaseHandle( impl )
{
}

StyleManager::StyleChangeSignalType& StyleManager::StyleChangeSignal()
{
  return GetImpl( *this ).StyleChangeSignal();
}

void StyleManager::RequestThemeChange( const std::string& themeFile )
{
  GetImpl(*this).RequestThemeChange( themeFile );
}

void StyleManager::RequestDefaultTheme()
{
  GetImpl(*this).RequestDefaultTheme();
}

} // namespace Toolkit

} // namespace Dali

