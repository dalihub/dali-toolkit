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

namespace // unnamed namespace
{

using namespace Dali;
using namespace Dali::Scripting;

ColorMode GetColorMode( const std::string& value )
{
  ColorMode v( USE_OWN_COLOR );

  bool set = \
    SetIfEqual(value, "USE_OWN_COLOR"                , v, USE_OWN_COLOR                 ) || \
    SetIfEqual(value, "USE_PARENT_COLOR"             , v, USE_PARENT_COLOR              ) || \
    SetIfEqual(value, "USE_OWN_MULTIPLY_PARENT_COLOR", v, USE_OWN_MULTIPLY_PARENT_COLOR);

  if( !set )
  {
    DALI_ASSERT_ALWAYS( !"Unknown Color mode" );
  }

  return v;
}

PositionInheritanceMode GetPositionInheritanceMode( const std::string& value )
{
  PositionInheritanceMode v(INHERIT_PARENT_POSITION);

  bool set = \
    SetIfEqual(value, "INHERIT_PARENT_POSITION", v, INHERIT_PARENT_POSITION                                    ) || \
    SetIfEqual(value, "USE_PARENT_POSITION"    , v, USE_PARENT_POSITION                                        ) || \
    SetIfEqual(value, "USE_PARENT_POSITION_PLUS_LOCAL_POSITION", v, USE_PARENT_POSITION_PLUS_LOCAL_POSITION    ) || \
    SetIfEqual(value, "DONT_INHERIT_POSITION"  , v, DONT_INHERIT_POSITION);

  if( !set )
  {
    DALI_ASSERT_ALWAYS( !"Unknown position inheritance mode" );
  }

  return v;
}

DrawMode::Type GetDrawMode( const std::string& value )
{
  DrawMode::Type e(DrawMode::NORMAL);

  bool set = \
    SetIfEqual(value, "NORMAL", e, DrawMode::NORMAL  ) || \
    SetIfEqual(value, "OVERLAY",e, DrawMode::OVERLAY ) || \
    SetIfEqual(value, "STENCIL",e, DrawMode::STENCIL);

  if(!set)
  {
    DALI_ASSERT_ALWAYS( !"Unknown DrawMode type" );
  }

  return e;
}


Vector3 GetAnchorConstant( const std::string& value )
{
  Vector3 v = ParentOrigin::CENTER; // a Vector3x

  bool set =
    SetIfEqual( value, "BACK_TOP_LEFT",        v, ParentOrigin::BACK_TOP_LEFT      ) ||
    SetIfEqual( value, "BACK_TOP_CENTER",      v, ParentOrigin::BACK_TOP_CENTER    ) ||
    SetIfEqual( value, "BACK_TOP_RIGHT",       v, ParentOrigin::BACK_TOP_RIGHT     ) ||
    SetIfEqual( value, "BACK_CENTER_LEFT",     v, ParentOrigin::BACK_CENTER_LEFT   ) ||
    SetIfEqual( value, "BACK_CENTER",          v, ParentOrigin::BACK_CENTER        ) ||
    SetIfEqual( value, "BACK_CENTER_RIGHT",    v, ParentOrigin::BACK_CENTER_RIGHT  ) ||
    SetIfEqual( value, "BACK_BOTTOM_LEFT",     v, ParentOrigin::BACK_BOTTOM_LEFT   ) ||
    SetIfEqual( value, "BACK_BOTTOM_CENTER",   v, ParentOrigin::BACK_BOTTOM_CENTER ) ||
    SetIfEqual( value, "BACK_BOTTOM_RIGHT",    v, ParentOrigin::BACK_BOTTOM_RIGHT  ) ||
    SetIfEqual( value, "TOP_LEFT",             v, ParentOrigin::TOP_LEFT           ) ||
    SetIfEqual( value, "TOP_CENTER",           v, ParentOrigin::TOP_CENTER         ) ||
    SetIfEqual( value, "TOP_RIGHT",            v, ParentOrigin::TOP_RIGHT          ) ||
    SetIfEqual( value, "CENTER_LEFT",          v, ParentOrigin::CENTER_LEFT        ) ||
    SetIfEqual( value, "CENTER",               v, ParentOrigin::CENTER             ) ||
    SetIfEqual( value, "CENTER_RIGHT",         v, ParentOrigin::CENTER_RIGHT       ) ||
    SetIfEqual( value, "BOTTOM_LEFT",          v, ParentOrigin::BOTTOM_LEFT        ) ||
    SetIfEqual( value, "BOTTOM_CENTER",        v, ParentOrigin::BOTTOM_CENTER      ) ||
    SetIfEqual( value, "BOTTOM_RIGHT",         v, ParentOrigin::BOTTOM_RIGHT       ) ||
    SetIfEqual( value, "FRONT_TOP_LEFT",       v, ParentOrigin::FRONT_TOP_LEFT     ) ||
    SetIfEqual( value, "FRONT_TOP_CENTER",     v, ParentOrigin::FRONT_TOP_CENTER   ) ||
    SetIfEqual( value, "FRONT_TOP_RIGHT",      v, ParentOrigin::FRONT_TOP_RIGHT    ) ||
    SetIfEqual( value, "FRONT_CENTER_LEFT",    v, ParentOrigin::FRONT_CENTER_LEFT  ) ||
    SetIfEqual( value, "FRONT_CENTER",         v, ParentOrigin::FRONT_CENTER       ) ||
    SetIfEqual( value, "FRONT_CENTER_RIGHT",   v, ParentOrigin::FRONT_CENTER_RIGHT ) ||
    SetIfEqual( value, "FRONT_BOTTOM_LEFT",    v, ParentOrigin::FRONT_BOTTOM_LEFT  ) ||
    SetIfEqual( value, "FRONT_BOTTOM_CENTER",  v, ParentOrigin::FRONT_BOTTOM_CENTER) ||
    SetIfEqual( value, "FRONT_BOTTOM_RIGHT",   v, ParentOrigin::FRONT_BOTTOM_RIGHT );

  if(!set)
  {
    DALI_ASSERT_ALWAYS(!"Unknown Parent origin Constant" );
  }

  return v;
}


}; // unnamed namespace


namespace Dali
{

namespace Toolkit
{

namespace Internal
{

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
      actor.SetParentOrigin( *v );
    }
    else if( OptionalString anchor = IsString(child, "anchor-point") )
    {
      actor.SetAnchorPoint( GetAnchorConstant(*anchor) );
    }
  }

  if( OptionalFloat opacity = IsFloat(child, "opacity") )
  {
    actor.SetOpacity( *opacity );
  }

  if( OptionalFloat opacity = IsFloat(child, "opacity-by") )
  {
    actor.OpacityBy( *opacity );
  }

  if( OptionalString colorMode = IsString(child, "color-mode") )
  {
    actor.SetColorMode( GetColorMode(*colorMode) );
  }

  if( OptionalBoolean inherit = IsBoolean(child, "inherit-shader-effect") )
  {
    actor.SetInheritShaderEffect( *inherit );
  }

  if( OptionalBoolean sensitive = IsBoolean(child, "sensitive") )
  {
    actor.SetSensitive( *sensitive );
  }

  if( OptionalBoolean leaveRequired = IsBoolean(child, "leave-required") )
  {
    actor.SetLeaveRequired( *leaveRequired );
  }

  if( OptionalString v = IsString(child, "position-inheritance") )
  {
    actor.SetPositionInheritanceMode( GetPositionInheritanceMode(*v) );
  }

  if( OptionalString v = IsString(child, "draw-mode") )
  {
    actor.SetDrawMode( GetDrawMode(*v) );
  }

  if( OptionalBoolean v = IsBoolean(child, "inherit-rotation") )
  {
    actor.SetInheritRotation( *v );
  }

  if( OptionalBoolean v = IsBoolean(child, "inherit-scale") )
  {
    actor.SetInheritScale( *v );
  }

  return actor;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
