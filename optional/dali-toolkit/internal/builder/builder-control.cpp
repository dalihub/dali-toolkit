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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/builder/builder-get-is.inl.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/alignment/alignment.h>
#include <dali/public-api/scripting/scripting.h>

namespace // unnamed namespace
{

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Scripting; // SetIfEqual

Control::SizePolicy GetSizePolicy( const std::string& value )
{
  Control::SizePolicy v(Control::Flexible);

  bool set = \
    SetIfEqual(value, "FIXED",    v, Control::Fixed    ) || \
    SetIfEqual(value, "MINIMUM",  v, Control::Minimum  ) || \
    SetIfEqual(value, "MAXIMUM",  v, Control::Maximum  ) || \
    SetIfEqual(value, "RANGE",    v, Control::Range    ) || \
    SetIfEqual(value, "FLEXIBLE", v, Control::Flexible );

  if( !set )
  {
    DALI_ASSERT_ALWAYS( !"Unknown Color mode" );
  }

  return v;
}

} // anon namespace


namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/*
 * Handles special case control configuration (anything thats not already a property)
 *
 */
Dali::Toolkit::Control SetupControl( const TreeNode& child, Dali::Toolkit::Control& control )
{
  DALI_ASSERT_ALWAYS( control && "Empty actor handle" );

  Control::SizePolicy widthPolicy(Control::Flexible);
  Control::SizePolicy heightPolicy(Control::Flexible);

  if( OptionalString v = IsString(child, "width-policy") )
  {
    widthPolicy = GetSizePolicy(*v);
    control.SetSizePolicy( widthPolicy, heightPolicy );
  }

  if( OptionalString v = IsString(child, "height-policy") )
  {
    heightPolicy = GetSizePolicy(*v);
    control.SetSizePolicy( widthPolicy, heightPolicy );
  }

  if( OptionalVector3 v = IsVector3(child, "minimum-size") )
  {
    control.SetMinimumSize( *v );
  }

  if( OptionalVector3 v = IsVector3(child, "maximum-size") )
  {
    control.SetMaximumSize( *v );
  }

  if( OptionalBoolean v = IsBoolean(child, "key-input-focus") )
  {
    if( *v )
    {
      control.SetKeyInputFocus();
    }
  }

  return control;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
