/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/markup-processor/markup-processor-helper-functions.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/vector2.h>
#include <stdlib.h>
#include <iomanip>
#include <sstream>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace
{
const char WHITE_SPACE      = 0x20; // ASCII value of the white space.
const char FIRST_UPPER_CASE = 0x41; // ASCII value of the one after the first upper case character (A).
const char LAST_UPPER_CASE  = 0x5b; // ASCII value of the one after the last upper case character (Z).
const char TO_LOWER_CASE    = 32;   // Value to add to a upper case character to transform it into a lower case.

const unsigned int MAX_FLOAT_ATTRIBUTE_SIZE = 17u; ///< The maximum length of any of the possible float values.  +99999.999999999f  (sign, five digits, dot, nine digits, f)

const char        WEB_COLOR_TOKEN('#');
const char* const HEX_COLOR_TOKEN("0x");
const char* const ALPHA_ONE("FF");

const char* BLACK_COLOR("black");
const char* WHITE_COLOR("white");
const char* RED_COLOR("red");
const char* GREEN_COLOR("green");
const char* BLUE_COLOR("blue");
const char* YELLOW_COLOR("yellow");
const char* MAGENTA_COLOR("magenta");
const char* CYAN_COLOR("cyan");
const char* TRANSPARENT_COLOR("transparent");

const char* SOLID_UNDERLINE("solid");
const char* DASHED_UNDERLINE("dashed");
const char* DOUBLE_UNDERLINE("double");

const char* BEGIN_HORIZONTAL_ALIGNMENT("begin");
const char* CENTER_HORIZONTAL_ALIGNMENT("center");
const char* END_HORIZONTAL_ALIGNMENT("end");

} // namespace

bool TokenComparison(const std::string& string1, const char* const stringBuffer2, Length length)
{
  const Length stringSize = string1.size();
  if(stringSize != length)
  {
    // Early return. Strings have different sizes.
    return false;
  }

  const char* const stringBuffer1 = string1.c_str();

  for(std::size_t index = 0; index < stringSize; ++index)
  {
    const char character = *(stringBuffer2 + index);
    const bool toLower   = (character < LAST_UPPER_CASE) && (character >= FIRST_UPPER_CASE);
    if(*(stringBuffer1 + index) != (toLower ? character + TO_LOWER_CASE : character))
    {
      return false;
    }
  }

  return true;
}

void SkipWhiteSpace(const char*&      stringBuffer,
                    const char* const stringEndBuffer)
{
  for(; (WHITE_SPACE >= *stringBuffer) && (stringBuffer < stringEndBuffer); ++stringBuffer)
    ;
}

void JumpToWhiteSpace(const char*&      stringBuffer,
                      const char* const stringEndBuffer)
{
  for(; (WHITE_SPACE != *stringBuffer) && (stringBuffer < stringEndBuffer); ++stringBuffer)
    ;
}

unsigned int StringToUint(const char* const uintStr)
{
  return static_cast<unsigned int>(strtoul(uintStr, NULL, 10));
}

unsigned int StringToHex(const char* const uintStr)
{
  return static_cast<unsigned int>(strtoul(uintStr, NULL, 16));
}

float StringToFloat(const char* const floatStr)
{
  return static_cast<float>(strtod(floatStr, NULL));
}

void FloatToString(float value, std::string& floatStr)
{
  std::stringstream ss;
  ss << value;
  floatStr = ss.str();
}

void UintToString(unsigned int value, std::string& uIntStr)
{
  std::stringstream ss;
  ss << value;
  uIntStr = ss.str();
}

void UintColorToVector4(unsigned int color, Vector4& retColor)
{
  retColor.a = static_cast<float>((color & 0xFF000000) >> 24u) / 255.f;
  retColor.r = static_cast<float>((color & 0x00FF0000) >> 16u) / 255.f;
  retColor.g = static_cast<float>((color & 0x0000FF00) >> 8u) / 255.f;
  retColor.b = static_cast<float>(color & 0x000000FF) / 255.f;
}

void ColorStringToVector4(const char* const colorStr, Length length, Vector4& retColor)
{
  if(WEB_COLOR_TOKEN == *colorStr)
  {
    std::string webColor(colorStr + 1u, length - 1u);
    if(4u == length) // 3 component web color #F00 (red)
    {
      webColor.insert(2u, &(webColor[2]), 1u);
      webColor.insert(1u, &(webColor[1]), 1u);
      webColor.insert(0u, &(webColor[0]), 1u);
      webColor.insert(0u, ALPHA_ONE);
    }
    else if(7u == length) // 6 component web color #FF0000 (red)
    {
      webColor.insert(0u, ALPHA_ONE);
    }

    UintColorToVector4(StringToHex(webColor.c_str()), retColor);
  }
  else if(TokenComparison(HEX_COLOR_TOKEN, colorStr, 2u))
  {
    UintColorToVector4(StringToHex(colorStr + 2u), retColor);
  }
  else if(TokenComparison(BLACK_COLOR, colorStr, length))
  {
    retColor = Color::BLACK;
  }
  else if(TokenComparison(WHITE_COLOR, colorStr, length))
  {
    retColor = Color::WHITE;
  }
  else if(TokenComparison(RED_COLOR, colorStr, length))
  {
    retColor = Color::RED;
  }
  else if(TokenComparison(GREEN_COLOR, colorStr, length))
  {
    retColor = Color::GREEN;
  }
  else if(TokenComparison(BLUE_COLOR, colorStr, length))
  {
    retColor = Color::BLUE;
  }
  else if(TokenComparison(YELLOW_COLOR, colorStr, length))
  {
    retColor = Color::YELLOW;
  }
  else if(TokenComparison(MAGENTA_COLOR, colorStr, length))
  {
    retColor = Color::MAGENTA;
  }
  else if(TokenComparison(CYAN_COLOR, colorStr, length))
  {
    retColor = Color::CYAN;
  }
  else if(TokenComparison(TRANSPARENT_COLOR, colorStr, length))
  {
    retColor = Color::TRANSPARENT;
  }
}

void Vector4ToColorString(const Vector4& value, std::string& vector2Str)
{
  if(Color::BLACK == value)
  {
    vector2Str = BLACK_COLOR;
    return;
  }

  if(Color::WHITE == value)
  {
    vector2Str = WHITE_COLOR;
    return;
  }

  if(Color::RED == value)
  {
    vector2Str = RED_COLOR;
    return;
  }

  if(Color::GREEN == value)
  {
    vector2Str = GREEN_COLOR;
    return;
  }

  if(Color::BLUE == value)
  {
    vector2Str = BLUE_COLOR;
    return;
  }

  if(Color::YELLOW == value)
  {
    vector2Str = YELLOW_COLOR;
    return;
  }

  if(Color::MAGENTA == value)
  {
    vector2Str = MAGENTA_COLOR;
    return;
  }

  if(Color::CYAN == value)
  {
    vector2Str = CYAN_COLOR;
    return;
  }

  if(Color::TRANSPARENT == value)
  {
    vector2Str = TRANSPARENT_COLOR;
    return;
  }

  const unsigned int alpha = static_cast<unsigned int>(255.f * value.a);
  const unsigned int red   = static_cast<unsigned int>(255.f * value.r);
  const unsigned int green = static_cast<unsigned int>(255.f * value.g);
  const unsigned int blue  = static_cast<unsigned int>(255.f * value.b);

  std::stringstream  ss;
  const unsigned int size = 2u * sizeof(unsigned char);

  ss << "0x"
     << std::setfill('0') << std::setw(size)
     << std::hex << alpha
     << std::setfill('0') << std::setw(size)
     << std::hex << red
     << std::setfill('0') << std::setw(size)
     << std::hex << green
     << std::setfill('0') << std::setw(size)
     << std::hex << blue;
  vector2Str = ss.str();
}

void StringToVector2(const char* const vectorStr, Length length, Vector2& vector2)
{
  // Points to the first character of the string.
  const char* strBuffer = vectorStr;

  // Points to the first character of the 'x' value.
  const char* const xBuffer = strBuffer;

  // Jumps to the next white space.
  JumpToWhiteSpace(strBuffer, strBuffer + length);

  // Points to the first character of the 'y' value.
  const char* const yBuffer = strBuffer;

  // Converts the shadow's offset to float.
  vector2.x = StringToFloat(xBuffer);
  vector2.y = StringToFloat(yBuffer);
}

void Vector2ToString(const Vector2& value, std::string& vector2Str)
{
  FloatToString(value.x, vector2Str);
  vector2Str += " ";

  std::string yStr;
  FloatToString(value.y, yStr);

  vector2Str += yStr;
}

void UnderlineTypeStringToTypeValue(const char* const typeStr, Length length, Text::Underline::Type& retType)
{
  if(TokenComparison(SOLID_UNDERLINE, typeStr, length))
  {
    retType = Text::Underline::SOLID;
  }
  else if(TokenComparison(DASHED_UNDERLINE, typeStr, length))
  {
    retType = Text::Underline::DASHED;
  }
  else if(TokenComparison(DOUBLE_UNDERLINE, typeStr, length))
  {
    retType = Text::Underline::DOUBLE;
  }
}

bool HorizontalAlignmentTypeStringToTypeValue(const char* const typeStr, Length length, Text::HorizontalAlignment::Type& retType)
{
  // The string is valid value for HorizontalAlignment
  bool valid = false;
  if(TokenComparison(BEGIN_HORIZONTAL_ALIGNMENT, typeStr, length))
  {
    retType = Text::HorizontalAlignment::BEGIN;
    valid   = true;
  }
  else if(TokenComparison(CENTER_HORIZONTAL_ALIGNMENT, typeStr, length))
  {
    retType = Text::HorizontalAlignment::CENTER;
    valid   = true;
  }
  else if(TokenComparison(END_HORIZONTAL_ALIGNMENT, typeStr, length))
  {
    retType = Text::HorizontalAlignment::END;
    valid   = true;
  }

  return valid;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
