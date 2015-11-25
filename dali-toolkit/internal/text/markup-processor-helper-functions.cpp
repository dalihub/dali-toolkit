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

} // namespace Text

} // namespace Toolkit

} // namespace Dali
