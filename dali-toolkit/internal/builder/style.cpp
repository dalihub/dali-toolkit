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
#include <dali/devel-api/scripting/scripting.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/builder/style.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>

namespace Dali
{
namespace Toolkit
{

namespace Internal
{

extern const Dali::Scripting::StringEnum ControlStateTable[];
extern const unsigned int ControlStateTableCount;

StylePtr Style::New()
{
  StylePtr stylePtr( new Style() );
  return stylePtr;
}

void Style::ApplyVisualsAndPropertiesRecursively(
  Handle handle,
  const Dictionary<Property::Map>& instancedProperties ) const
{
  ApplyVisuals( handle, instancedProperties );
  ApplyProperties( handle );

  Toolkit::Control control = Toolkit::Control::DownCast(handle);
  if( control )
  {
    std::string stateName;
    Property::Value value = control.GetProperty(DevelControl::Property::STATE);
    Dali::Toolkit::DevelControl::State state = static_cast<Dali::Toolkit::DevelControl::State>(value.Get<int>());
    stateName = Scripting::GetEnumerationName< Toolkit::DevelControl::State >( state, ControlStateTable, ControlStateTableCount );

    if( ! stateName.empty() )
    {
      // Look up state in states table:
      const StylePtr* stylePtr = subStates.FindConst( stateName );
      if( stylePtr )
      {
        const StylePtr statePtr(*stylePtr);

        // We have a state match.
        statePtr->ApplyVisuals( handle, instancedProperties );
        statePtr->ApplyProperties( handle );

        // Apply substate visuals
        Property::Value value = control.GetProperty(DevelControl::Property::SUB_STATE);
        std::string subStateName;
        if( value.Get( subStateName ) && ! subStateName.empty() )
        {
          const StylePtr* stylePtr = statePtr->subStates.FindConst( subStateName );
          if( stylePtr )
          {
            const StylePtr subStatePtr(*stylePtr);
            // We have a sub-state match.
            subStatePtr->ApplyVisuals( handle, instancedProperties );
            subStatePtr->ApplyProperties( handle );
          }
        }
      }
    }
  }
}

void Style::ApplyVisuals(
  Handle handle,
  const Dictionary<Property::Map>& instancedProperties ) const
{
  ApplyVisuals( handle, visuals, instancedProperties );
}

void Style::ApplyVisuals(
  Handle handle,
  const Dictionary<Property::Map>& visualMaps,
  const Dictionary<Property::Map>& instancedProperties )
{
  for( Dictionary<Property::Map>::iterator iter = visualMaps.Begin(); iter != visualMaps.End() ; ++iter )
  {
    const std::string& visualName = (*iter).key;
    Property::Map map = (*iter).entry;
    Property::Map* instancedMap = instancedProperties.Find( visualName );
    ApplyVisual( handle, visualName, map, instancedMap );
  }
}

void Style::ApplyVisual(
  Handle handle,
  const std::string& visualName,
  const Property::Map& visualMap,
  const Property::Map* instancedProperties )
{
  // Check if this visual name is a valid property of handle
  Dali::Property::Index index = handle.GetPropertyIndex( visualName );
  if( index != Property::INVALID_INDEX )
  {
    const Property::Map* applyMap = &visualMap;
    Property::Map mergedMap;

    // If there are instanced properties, and the visual types match,
    // merge them into the visual map
    if( instancedProperties )
    {
      Property::Value* instanceTypeValue = instancedProperties->Find( Toolkit::DevelVisual::Property::TYPE);
      Property::Value* newTypeValue = visualMap.Find( Toolkit::DevelVisual::Property::TYPE, VISUAL_TYPE );
      if( instanceTypeValue && newTypeValue )
      {
        int instanceVisualType=-1;
        int newVisualType=-1;
        Scripting::GetEnumerationProperty( *instanceTypeValue, VISUAL_TYPE_TABLE, VISUAL_TYPE_TABLE_COUNT, instanceVisualType );
        Scripting::GetEnumerationProperty( *newTypeValue, VISUAL_TYPE_TABLE, VISUAL_TYPE_TABLE_COUNT, newVisualType );

        if( instanceVisualType == newVisualType )
        {
          // Same type - merge remaining instance data
          mergedMap.Merge( visualMap );
          mergedMap.Merge( *instancedProperties );
          applyMap = &mergedMap;
        }
      }
    }

    // Apply the visual property map to the handle
    const Property::Value value(const_cast<Property::Map&>(*applyMap));
    handle.SetProperty( index, value );
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
