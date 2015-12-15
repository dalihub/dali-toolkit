/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// FILE HEADER
#include <dali-toolkit/internal/text/markup-processor-helper-functions.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <stdlib.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

namespace
{
const char WHITE_SPACE       = 0x20; // ASCII value of the white space.
const char LAST_UPPER_CASE   = 0x5b; // ASCII value of the one after the last upper case character (Z).
const char TO_LOWER_CASE     = 32;   // Value to add to a upper case character to transform it into a lower case.

const char WEB_COLOR_TOKEN( '#' );
const char* const HEX_COLOR_TOKEN( "0x" );
const char* const ALPHA_ONE( "FF" );

const std::string BLACK_COLOR( "black" );
const std::string WHITE_COLOR( "white" );
const std::string RED_COLOR( "red" );
const std::string GREEN_COLOR( "green" );
const std::string BLUE_COLOR( "blue" );
const std::string YELLOW_COLOR( "yellow" );
const std::string MAGENTA_COLOR( "magenta" );
const std::string CYAN_COLOR( "cyan" );
const std::string TRANSPARENT_COLOR( "transparent" );
}

bool TokenComparison( const std::string& string1, const char* const stringBuffer2, Length length )
{
  const Length stringSize = string1.size();
  if( stringSize != length )
  {
    // Early return. Strings have different sizes.
    return false;
  }

  const char* const stringBuffer1 = string1.c_str();

  for( std::size_t index = 0; index < stringSize; ++index )
  {
    char character = *( stringBuffer2 + index );
    if( *( stringBuffer1 + index ) != ( ( ( character < LAST_UPPER_CASE ) && ( '0' != character ) ) ? character + TO_LOWER_CASE : character ) )
    {
      return false;
    }
  }

  return true;
}

void SkipWhiteSpace( const char*& markupStringBuffer,
                     const char* const markupStringEndBuffer )
{
  for( ; ( WHITE_SPACE >= *markupStringBuffer ) && ( markupStringBuffer < markupStringEndBuffer ); ++markupStringBuffer );
}

unsigned int StringToHex( const char* const uintStr )
{
  return static_cast<unsigned int>( strtoul( uintStr, NULL, 16 ) );
}

void UintColorToVector4( unsigned int color, Vector4& retColor )
{
  retColor.a = static_cast<float>( ( color & 0xFF000000 ) >> 24u ) / 255.f;
  retColor.r = static_cast<float>( ( color & 0x00FF0000 ) >> 16u ) / 255.f;
  retColor.g = static_cast<float>( ( color & 0x0000FF00 ) >> 8u ) / 255.f;
  retColor.b = static_cast<float>( color & 0x000000FF ) / 255.f;
}

void ColorStringToVector4( const char* const colorStr, Length length, Vector4& retColor )
{
  if( WEB_COLOR_TOKEN == *colorStr )
  {
    std::string webColor( colorStr + 1u, length - 1u );
    if( 4u == length )                      // 3 component web color #F00 (red)
    {
      webColor.insert( 2u, &( webColor[2] ), 1u );
      webColor.insert( 1u, &( webColor[1] ), 1u );
      webColor.insert( 0u, &( webColor[0] ), 1u );
      webColor.insert( 0u, ALPHA_ONE );
    }
    else if( 7u == length )                 // 6 component web color #FF0000 (red)
    {
      webColor.insert( 0u, ALPHA_ONE );
    }

    UintColorToVector4( StringToHex( webColor.c_str() ), retColor );
  }
  else if( TokenComparison( HEX_COLOR_TOKEN, colorStr, 2u ) )
  {
    UintColorToVector4( StringToHex( colorStr + 2u ), retColor );
  }
  else if( TokenComparison( BLACK_COLOR, colorStr, length ) )
  {
    retColor = Color::BLACK;
  }
  else if( TokenComparison( WHITE_COLOR, colorStr, length ) )
  {
    retColor = Color::WHITE;
  }
  else if( TokenComparison( RED_COLOR, colorStr, length ) )
  {
    retColor = Color::RED;
  }
  else if( TokenComparison( GREEN_COLOR, colorStr, length ) )
  {
    retColor = Color::GREEN;
  }
  else if( TokenComparison( BLUE_COLOR, colorStr, length ) )
  {
    retColor = Color::BLUE;
  }
  else if( TokenComparison( YELLOW_COLOR, colorStr, length ) )
  {
    retColor = Color::YELLOW;
  }
  else if( TokenComparison( MAGENTA_COLOR, colorStr, length ) )
  {
    retColor = Color::MAGENTA;
  }
  else if( TokenComparison( CYAN_COLOR, colorStr, length ) )
  {
    retColor = Color::CYAN;
  }
  else if( TokenComparison( TRANSPARENT_COLOR, colorStr, length ) )
  {
    retColor = Color::TRANSPARENT;
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
