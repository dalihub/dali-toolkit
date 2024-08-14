/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali-toolkit/internal/text/hyphenator.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/character-set-conversion.h>

#include <cstring> // for strcmp

namespace Dali
{
namespace Toolkit
{
namespace Text
{
const char* UTF8 = "UTF-8";

Vector<bool> GetWordHyphens(TextAbstraction::Hyphenation& hyphenation,
                            const Character*              word,
                            Length                        wordSize,
                            const char*                   lang)
{
  Vector<bool> hyphens;

  if(0u == wordSize || word == nullptr)
  {
    // Nothing to do if there are no characters.
    return hyphens;
  }

  // first get the needed encoding
  std::string text;
  if(strcmp(hyphenation.GetDictionaryEncoding(lang), UTF8) == 0)
  {
    Utf32ToUtf8(word, wordSize, text);
  }
  else
  {
    text = std::string((char*)word, (size_t)(wordSize * sizeof(Character)));
  }

  return hyphenation.GetWordHyphens(text.c_str(), (int)text.length(), lang);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
