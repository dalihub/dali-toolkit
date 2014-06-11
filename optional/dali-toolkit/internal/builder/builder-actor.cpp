//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES
#include <string>
#include <dali/integration-api/debug.h>
#include <dali/public-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/builder/builder-get-is.inl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

using namespace Dali::Scripting;

/*
 * Handles special case actor configuration (anything thats not already a property)
 *
 */
Actor SetupActor( const TreeNode& child, Actor& actor )
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

  return actor;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
