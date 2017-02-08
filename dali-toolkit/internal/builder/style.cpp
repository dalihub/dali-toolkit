/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/object/handle.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/internal/builder/style.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

StylePtr Style::New()
{
  StylePtr stylePtr( new Style() );
  return stylePtr;
}

void Style::ApplyVisualsAndPropertiesRecursively( Handle handle ) const
{
  ApplyVisuals( handle );
  ApplyProperties( handle );

  Toolkit::Control control = Toolkit::Control::DownCast(handle);
  if( control )
  {
    Property::Value value = control.GetProperty(DevelControl::Property::STATE);
    std::string stateName;
    if( value.Get( stateName ) )
    {
      // Look up state in states table:
      const StylePtr* stylePtr = subStates.FindCaseInsensitiveC( stateName );
      if( stylePtr )
      {
        const StylePtr statePtr(*stylePtr);

        // We have a state match.
        statePtr->ApplyVisuals( handle );
        statePtr->ApplyProperties( handle );

        // Apply substate visuals
        Property::Value value = control.GetProperty(DevelControl::Property::SUB_STATE);
        std::string subStateName;
        if( value.Get( subStateName ) && ! subStateName.empty() )
        {
          const StylePtr* stylePtr = statePtr->subStates.FindCaseInsensitiveC( subStateName );
          if( stylePtr )
          {
            const StylePtr subStatePtr(*stylePtr);
            // We have a sub-state match.
            subStatePtr->ApplyVisuals( handle );
            subStatePtr->ApplyProperties( handle );
          }
        }
      }
    }
  }
}

void Style::ApplyVisuals( Handle handle ) const
{
  for( Dictionary<Property::Map>::iterator iter = visuals.Begin(); iter != visuals.End() ; ++iter )
  {
    const std::string& visualName = (*iter).key;
    const Property::Map& map = (*iter).entry;
    Dali::Property::Index index = handle.GetPropertyIndex( visualName );
    if( index != Property::INVALID_INDEX )
    {
      const Property::Value value(const_cast<Property::Map&>(map));
      handle.SetProperty( index, value );
    }
  }
}

void Style::ApplyProperties( Handle handle ) const
{
  for( Property::Map::SizeType i=0; i<properties.Count(); ++i )
  {
    KeyValuePair keyValue = properties.GetKeyValue( i );
    if( keyValue.first.type == Property::Key::INDEX )
    {
      handle.SetProperty( keyValue.first.indexKey, keyValue.second );
    }
  }
}

Style::Style()
{
}

Style::~Style()
{
}


} // Internal
} // Toolkit
} // Dali
