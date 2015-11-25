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

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_HELPER_FUNCTIONS_H__
