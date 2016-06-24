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
                            Vector<bool>& isDefaultFont,
                            const TextAbstraction::FontDescription& defaultFontDescription,
                            TextAbstraction::PointSize26Dot6 defaultPointSize,
                            CharacterIndex startIndex,
                            Length numberOfCharacters )
{
  // Get the handle to the font client.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  // Pointer to the font id buffer.
  FontId* fontIdsBuffer = fontIds.Begin();

  // Pointer to the 'is default' font buffer.
  bool* isDefaultFontBuffer = isDefaultFont.Begin();

  // Used to temporarily store the style per character.
  TextAbstraction::FontDescription fontDescription;
  TextAbstraction::PointSize26Dot6 fontSize;

  Length familyIndex = 0u;
  Length weightIndex = 0u;
  Length widthIndex = 0u;
  Length slantIndex = 0u;
  Length sizeIndex = 0u;

  // Traverse all the characters.
  const CharacterIndex lastCharacterPlusOne = startIndex + numberOfCharacters;
  for( CharacterIndex index = startIndex; index < lastCharacterPlusOne; ++index )
  {
    bool& defaultFont = *(isDefaultFontBuffer + index - startIndex );

    Length runIndex = 0u;

    bool familyOverriden = false;
    bool weightOverriden = false;
    bool widthOverriden = false;
    bool slantOverriden = false;
    bool sizeOverriden = false;

    // Traverse all the font descriptions.
    const FontDescriptionRun* const fontDescriptionsBuffer = fontDescriptions.Begin();
    for( Vector<FontDescriptionRun>::ConstIterator it = fontDescriptionsBuffer,
           endIt = fontDescriptions.End();
         it != endIt;
         ++it, ++runIndex )
    {
      // Check whether the character's font is modified by the current font description.
      const FontDescriptionRun& fontRun = *it;
      if( ( index >= fontRun.characterRun.characterIndex ) &&
          ( index < fontRun.characterRun.characterIndex + fontRun.characterRun.numberOfCharacters ) )
      {
        if( fontRun.familyDefined )
        {
          defaultFont = false;
          familyOverriden = true;
          familyIndex = runIndex;
        }
        if( fontRun.weightDefined )
        {
          defaultFont = false;
          weightOverriden = true;
          weightIndex = runIndex;
        }
        if( fontRun.widthDefined )
        {
          defaultFont = false;
          widthOverriden = true;
          widthIndex = runIndex;
        }
        if( fontRun.slantDefined )
        {
          defaultFont = false;
          slantOverriden = true;
          slantIndex = runIndex;
        }
        if( fontRun.sizeDefined )
        {
          defaultFont = false;
          sizeOverriden = true;
          sizeIndex = runIndex;
        }
      }
    }

    // Get the font id if is not the default font.
    if( !defaultFont )
    {
      if( familyOverriden )
      {
        const FontDescriptionRun& fontRun = *( fontDescriptionsBuffer + familyIndex );
        fontDescription.family = std::string( fontRun.familyName, fontRun.familyLength ); // TODO Could use move constructor when switch to c++11.
      }
      else
      {
        fontDescription.family = defaultFontDescription.family;
      }

      if( weightOverriden )
      {
        const FontDescriptionRun& fontRun = *( fontDescriptionsBuffer + weightIndex );
        fontDescription.weight = fontRun.weight;
      }
      else
      {
        fontDescription.weight = defaultFontDescription.weight;
      }

      if( widthOverriden )
      {
        const FontDescriptionRun& fontRun = *( fontDescriptionsBuffer + widthIndex );
        fontDescription.width = fontRun.width;
      }
      else
      {
        fontDescription.width = defaultFontDescription.width;
      }

      if( slantOverriden )
      {
        const FontDescriptionRun& fontRun = *( fontDescriptionsBuffer + slantIndex );
        fontDescription.slant = fontRun.slant;
      }
      else
      {
        fontDescription.slant = defaultFontDescription.slant;
      }

      if( sizeOverriden )
      {
        const FontDescriptionRun& fontRun = *( fontDescriptionsBuffer + sizeIndex );
        fontSize = fontRun.size;
      }
      else
      {
        fontSize = defaultPointSize;
      }

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
