/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <dali/integration-api/debug.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/controls/control/control-data-impl-debug.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <iostream>
#include <algorithm>
#include <functional>

#if defined(DEBUG_ENABLED)

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

std::ostream& operator<<( std::ostream& o, Visual::Base& visual )
{
  Property::Map map;
  o << "{\n\"name\":\"" << visual.GetName() << "\",\n";
  visual.CreatePropertyMap( map );
  o << map << "}\n"; // Not JSON format.
  return o;
}

std::ostream& operator<<( std::ostream& o, const RegisteredVisual& registeredVisual )
{
  o << "{\n" << "\"index\":" << registeredVisual.index << ",\n";
  o << "\"enabled\":" << (registeredVisual.enabled?"true":"false") << ",\n";
  o << "\"pending\":" << (registeredVisual.pending?"true":"false") << ",\n";
  o << "\"visual\":" << registeredVisual << "\n}\n";
  return o;
}

std::ostream& operator<<( std::ostream& o, const RegisteredVisualContainer& visualContainer )
{
  o<<"[\n";

  bool first=true;
  for( auto&& elem : visualContainer )
  {
    if(!first)
    {
      o << ",";
    }
    first = false;

    o<<elem<<"\n";
  }
  o<<"]\n";
  return o;
}

std::ostream& DumpProperty( std::ostream& o, Property::Index index, Handle handle )
{
  o << "{\n";
  o << "\"index\":" << index << ",\n";
  o << "\"name\":" << handle.GetPropertyName( index ) << ",\n";
  o << "\"value\":" << handle.GetProperty( index ) << "\n";
  o << "}";
  return o;
}


std::ostream& DumpPropertiesWithPredicate( std::ostream& o, Dali::Handle handle,
                                           Property::IndexContainer& indices, std::function<bool(int)> predicate)
{
  bool first = true;
  for( auto index : indices )
  {
    if( predicate( index ) )
    {
      if( !first )
      {
        o << ",";
      }
      o << std::endl;
      first = false;
      DumpProperty( o, index, handle );
    }
  }
  return o;
}

std::ostream& DumpProperties( std::ostream& o, Handle handle )
{
  Property::IndexContainer indices;
  handle.GetPropertyIndices( indices );

  auto childPropertiesP = [](int index) -> bool
    {
      return CHILD_PROPERTY_REGISTRATION_START_INDEX <= index && index <= CHILD_PROPERTY_REGISTRATION_MAX_INDEX;
    };
  auto propertiesP = [](int index) -> bool
    {
      return !(CHILD_PROPERTY_REGISTRATION_START_INDEX <= index && index <= CHILD_PROPERTY_REGISTRATION_MAX_INDEX);
    };

  o << "\"childProperties\":[\n" ;
  DumpPropertiesWithPredicate( o, handle, indices, childPropertiesP );
  o << std::endl << "]," << std::endl;

  o << "\"Properties\":[\n" ;
  DumpPropertiesWithPredicate( o, handle, indices, propertiesP );
  o << std::endl << "]," << std::endl;

  return o;
}

std::string DumpControl( const Internal::Control& control )
{
  auto& controlData = Internal::Control::Impl::Get( control );

  std::ostringstream oss;
  oss << "{\n  ";
  const std::string& name = control.Self().GetName();
  if( ! name.empty() )
  {
    oss << "\"name\":\"" << name << "\",\n";
  }
  oss << "\"id\":\"" << control.Self().GetId() << "\",\n";
  oss << "\"registeredVisuals\":\n" << controlData.mVisuals << ",\n";
  oss << ",\n";
  oss << "\"removeVisuals\":\n" << controlData.mRemoveVisuals << ",\n";
  oss << "\"rendererCount\":" << control.Self().GetRendererCount() << ",\n";
  oss << "\"properties\":\n{\n";
  DumpProperties( oss, control.Self() ) << "}\n";
  oss << "}\n";
  return oss.str();
}


} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif
