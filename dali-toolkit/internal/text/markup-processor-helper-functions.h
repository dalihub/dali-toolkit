#ifndef __DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_HELPER_FUNCTIONS_H__
#define __DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_HELPER_FUNCTIONS_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{

struct Vector4;

namespace Toolkit
{

namespace Text
{

/**
 * @brief Stores an attribute pair: name, value.
 */
struct Attribute
{
  const char* nameBuffer;
  const char* valueBuffer;
  Length nameLength;
  Length valueLength;
};

/**
 * @brief Stores a tag and its attributes.
 */
 struct Tag
 {
   Vector<Attribute> attributes;
   const char* buffer;
   Length length;
   bool isEndTag;
 };

/**
 * @brief Compare if two tokens are equal.
 *
 * @pre @p string1 must be lower case. (The html-ish constant tokens)
 * The @p stringBuffer2 parameter is transformed to lower case.
 * This function is used in the mark-up parser.
 * It has no sense to transform the constants html-ish tokens to lower case when
 * it's known they already are.
 *
 * @param[in] string1 The html-ish constant token.
 * @param[in] stringBuffer2 Pointer to the html-ish token buffer.
 * @param[in] length The length of the html-ish token.
 *
 * @return @e true if both strings are equal.
 */
bool TokenComparison( const std::string& string1, const char* const stringBuffer2, Length length );

/**
 * @brief Skips any unnecessary white space.
 *
 * @param[in,out] markupStringBuffer The mark-up string buffer. It's a const iterator pointing the current character.
 * @param[in] markupStringEndBuffer Pointer to one character after the end of the mark-up string buffer.
 */
void SkipWhiteSpace( const char*& markupStringBuffer,
                     const char* const markupStringEndBuffer );

/**
 * @brief Converts a string into an hexadecimal unsigned int.
 *
 * @param[in] uintStr An hexadecimal unsigned int packed inside a string.
 *
 * @return The hexadecimal value.
 */
unsigned int StringToHex( const char* const uintStr );

/**
 * @brief Converts a string into a float value.
 *
 * @param[in] floatStr A float packed inside a string.
 *
 * @return The float value.
 */
float StringToFloat( const char* const floatStr );

/**
 * @brief Converts an ARGB color packed in 4 byte unsigned int into a Vector4 color used in Dali.
 *
 * @param[in] color An ARGB color packed in an unsigned int.
 * @param[out] retColor A Vector4 with the converted color.
 */
void UintColorToVector4( unsigned int color, Vector4& retColor );

/**
 * @brief Converts a color packed inside a string into an ARGB Vector4 color.
 *
 * The string color could be in hexadecimal ( 0xFF0000FF ), webcolor ( #0000FF or #00F ) or some constant values:
 * black, white, red, green, blue, yellow, magenta, cyan, transparent.
 *
 * @param[in] colorStr A color packed inside a string.
 * @param[in] length The length of the color string.
 * @param[out] retColor A color packed inside a Vector4.
 */
void ColorStringToVector4( const char* const colorStr, Length length, Vector4& retColor );

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_HELPER_FUNCTIONS_H__
