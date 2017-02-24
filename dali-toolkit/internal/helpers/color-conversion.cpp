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

// CLASS HEADER
#include <dali-toolkit/internal/helpers/color-conversion.h>

// EXTERNAL INCLUDES
#include <sstream>
#include <dali/public-api/math/vector4.h>
#include <dali/devel-api/adaptor-framework/color-controller.h>

using Dali::Vector4;

namespace
{

/**
 * Converts a HTML style 'color' hex string ("#FF0000" for bright red) to a Vector4.
 * The Vector4 alpha component will be set to 1.0f
 * @param hexString The HTML style hex string
 * @return a Vector4 containing the new color value
 */
Vector4 HexStringToVector4( const char* s )
{
  unsigned int value(0u);
  std::istringstream( s ) >> std::hex >> value;
  return Vector4( ((value >> 16 ) & 0xff ) / 255.0f,
                  ((value >> 8 ) & 0xff ) / 255.0f,
                  (value & 0xff ) / 255.0f,
                  1.0f );
}

} // unnamed namespace

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

bool ConvertStringToColor( const std::string& colorString, Vector4& outColor )
{
  bool success( false );

  if( ( '#' == colorString[0] ) &&
      (  7  == colorString.size() ) )
  {
    const char* cString = colorString.c_str();
    outColor = HexStringToVector4( &cString[1] );
    success = true;
  }
  else
  {
    Dali::ColorController controller = Dali::ColorController::Get();

    if( controller )
    {
      success = controller.RetrieveColor( colorString, outColor );
    }
  }

  return success;
}

bool ConvertPropertyToColor( const Property::Value& colorValue, Vector4& outColor )
{
  bool success( false );

  if( Property::VECTOR4 == colorValue.GetType() )
  {
    success = colorValue.Get( outColor );
  }
  else if( Property::STRING == colorValue.GetType() )
  {
    std::string colorString;
    if( colorValue.Get( colorString ) )
    {
      success = ConvertStringToColor( colorString, outColor );
    }
  }

  return success;
}

} // Internal
} // Toolkit
} // Dali
