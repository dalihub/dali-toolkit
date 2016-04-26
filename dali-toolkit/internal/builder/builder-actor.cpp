/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/scripting/scripting.h>

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
extern void DeterminePropertyFromNode( const TreeNode& node, Property::Value& value, const Replacement& constant );

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
  if( !IsVector3( child, "parentOrigin") )
  {
    if( OptionalVector3 v = constant.IsVector3( IsChild(child, "parentOrigin") ) )
    {
      actor.SetParentOrigin( *v );
    }
    else if( OptionalString origin = constant.IsString( IsChild(child, "parentOrigin") ) )
    {
      actor.SetParentOrigin( GetAnchorConstant(*origin) );
    }
  }

  if( !IsVector3(child, "anchorPoint") )
  {
    if( OptionalVector3 v = constant.IsVector3( IsChild(child, "anchorPoint") ) )
    {
      actor.SetAnchorPoint( *v );
    }
    else if( OptionalString anchor = constant.IsString( IsChild(child, "anchorPoint") ) )
    {
      actor.SetAnchorPoint( GetAnchorConstant(*anchor) );
    }
  }

  return actor;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
