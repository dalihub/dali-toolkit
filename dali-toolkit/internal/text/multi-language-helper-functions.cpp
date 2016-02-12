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
#include <dali-toolkit/internal/text/multi-language-helper-functions.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/font-client.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

void MergeFontDescriptions( const Vector<FontDescriptionRun>& fontDescriptions,
                            Vector<FontId>& fontIds,
                            const TextAbstraction::FontDescription& defaultFontDescription,
                            TextAbstraction::PointSize26Dot6 defaultPointSize,
                            CharacterIndex startIndex,
                            Length numberOfCharacters )
{
  // Get the handle to the font client.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  // Pointer to the font id buffer.
  FontId* fontIdsBuffer = fontIds.Begin();

  // Traverse all the characters.
  for( CharacterIndex index = startIndex; index < numberOfCharacters; ++index )
  {
    // The default font description and font point size.
    TextAbstraction::FontDescription fontDescription = defaultFontDescription;
    TextAbstraction::PointSize26Dot6 fontSize = defaultPointSize;
    bool defaultFont = true;

    // Traverse all the font descriptions.
    for( Vector<FontDescriptionRun>::ConstIterator it = fontDescriptions.Begin(),
           endIt = fontDescriptions.End();
         it != endIt;
         ++it )
    {
      // Check whether the character's font is modified by the current font description.
      const FontDescriptionRun& fontRun = *it;
      if( ( index >= fontRun.characterRun.characterIndex ) &&
          ( index < fontRun.characterRun.characterIndex + fontRun.characterRun.numberOfCharacters ) )
      {
        if( fontRun.familyDefined )
        {
          fontDescription.family = std::string( fontRun.familyName, fontRun.familyLength );
          defaultFont = false;
        }
        if( fontRun.weightDefined )
        {
          fontDescription.weight = fontRun.weight;
          defaultFont = false;
        }
        if( fontRun.widthDefined )
        {
          fontDescription.width = fontRun.width;
          defaultFont = false;
        }
        if( fontRun.slantDefined )
        {
          fontDescription.slant = fontRun.slant;
          defaultFont = false;
        }
        if( fontRun.sizeDefined )
        {
          fontSize = fontRun.size;
          defaultFont = false;
        }
      }
    }

    // Get the font id if is not the default font.
    if( !defaultFont )
    {
      *( fontIdsBuffer + index - startIndex ) = fontClient.GetFontId( fontDescription, fontSize );
    }
  }
}

Script GetScript( Length index,
                  Vector<ScriptRun>::ConstIterator& scriptRunIt,
                  const Vector<ScriptRun>::ConstIterator& scriptRunEndIt )
{
  Script script = TextAbstraction::UNKNOWN;

  while( scriptRunIt != scriptRunEndIt )
  {
    const ScriptRun& scriptRun = *scriptRunIt;

    if( index >= scriptRun.characterRun.characterIndex + scriptRun.characterRun.numberOfCharacters )
    {
      ++scriptRunIt;
    }
    else if( index >= scriptRun.characterRun.characterIndex )
    {
      script = scriptRun.script;

      if( index + 1u == scriptRun.characterRun.characterIndex + scriptRun.characterRun.numberOfCharacters )
      {
        // All the characters of the current run have been traversed. Get the next one for the next iteration.
        ++scriptRunIt;
      }

      break;
    }
  }

  return script;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
