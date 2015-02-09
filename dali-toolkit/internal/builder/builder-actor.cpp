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
#include <string>
#include <dali/integration-api/debug.h>
#include <dali/public-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/builder/replacement.h>
#include <dali-toolkit/internal/builder/builder-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

using namespace Dali::Scripting;
extern bool SetPropertyFromNode( const TreeNode& node, Property::Value& value, const Replacement& constant );

/*
 * Handles special case actor configuration (anything thats not already a property)
 *
 */
Actor SetupActor( const TreeNode& child, Actor& actor, const Replacement& constant )
{
  DALI_ASSERT_ALWAYS( actor && "Empty actor handle" );

  // we allow enums strings for parent-origin and anchor-point but as with the current json
  // strings always succeed if they exist then check its not vector. If they are Vec3s then
  // this has already been set as a generic property.
  if( !IsVector3( child, "parent-origin") )
  {
    if( OptionalVector3 v = IsVector3(child, "parent-origin") )
    {
      actor.SetParentOrigin( *v );
    }
    else if( OptionalString origin = IsString(child, "parent-origin") )
    {
      actor.SetParentOrigin( GetAnchorConstant(*origin) );
    }
  }

  if( !IsVector3(child, "anchor-point") )
  {
    if( OptionalVector3 v = IsVector3(child, "anchor-point") )
    {
      actor.SetAnchorPoint( *v );
    }
    else if( OptionalString anchor = IsString(child, "anchor-point") )
    {
      actor.SetAnchorPoint( GetAnchorConstant(*anchor) );
    }
  }

  // Add custom properties
  if( OptionalChild customPropertiesChild = IsChild(child,  "custom-properties") )
  {
    const TreeNode& customPropertiesNode = *customPropertiesChild;
    const TreeConstIter endIter = customPropertiesNode.CEnd();
    for( TreeConstIter iter = customPropertiesNode.CBegin(); endIter != iter; ++iter )
    {
      const TreeNode::KeyNodePair& keyChild = *iter;
      std::string key( keyChild.first );

      Property::Index index = actor.GetPropertyIndex( key );
      Property::Value value;
      if( SetPropertyFromNode( keyChild.second, value, constant ))
      {
        if( Property::INVALID_INDEX == index )
        {
          actor.RegisterProperty( key, value );
        }
        else
        {
          actor.SetProperty( index, value );
        }
      }
    }
  }

  return actor;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
