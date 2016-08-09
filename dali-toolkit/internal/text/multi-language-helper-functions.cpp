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
                            const TextAbstraction::FontDescription& defaultFontDescription,
                            TextAbstraction::PointSize26Dot6 defaultPointSize,
                            CharacterIndex characterIndex,
                            TextAbstraction::FontDescription& fontDescription,
                            TextAbstraction::PointSize26Dot6& fontPointSize,
                            bool& isDefaultFont )
{
  // Initialize with the default font's point size.
  fontPointSize = defaultPointSize;

  // Initialize with the style parameters of the default font's style.
  fontDescription = defaultFontDescription;

  // Initialize as a default font.
  isDefaultFont = true;

  Length runIndex = 0u;

  Length familyIndex = 0u;
  Length weightIndex = 0u;
  Length widthIndex = 0u;
  Length slantIndex = 0u;
  Length sizeIndex = 0u;

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
    if( ( characterIndex >= fontRun.characterRun.characterIndex ) &&
        ( characterIndex < fontRun.characterRun.characterIndex + fontRun.characterRun.numberOfCharacters ) )
    {
      if( fontRun.familyDefined )
      {
        isDefaultFont = false;
        familyOverriden = true;
        familyIndex = runIndex;
      }
      if( fontRun.weightDefined )
      {
        isDefaultFont = false;
        weightOverriden = true;
        weightIndex = runIndex;
      }
      if( fontRun.widthDefined )
      {
        isDefaultFont = false;
        widthOverriden = true;
        widthIndex = runIndex;
      }
      if( fontRun.slantDefined )
      {
        isDefaultFont = false;
        slantOverriden = true;
        slantIndex = runIndex;
      }
      if( fontRun.sizeDefined )
      {
        isDefaultFont = false;
        sizeOverriden = true;
        sizeIndex = runIndex;
      }
    }
  }

  // Get the font's description if is not the default font.
  if( !isDefaultFont )
  {
    if( familyOverriden )
    {
      const FontDescriptionRun& fontRun = *( fontDescriptionsBuffer + familyIndex );
      fontDescription.family = std::string( fontRun.familyName, fontRun.familyLength );
    }

    if( weightOverriden )
    {
      const FontDescriptionRun& fontRun = *( fontDescriptionsBuffer + weightIndex );
      fontDescription.weight = fontRun.weight;
    }

    if( widthOverriden )
    {
      const FontDescriptionRun& fontRun = *( fontDescriptionsBuffer + widthIndex );
      fontDescription.width = fontRun.width;
    }

    if( slantOverriden )
    {
      const FontDescriptionRun& fontRun = *( fontDescriptionsBuffer + slantIndex );
      fontDescription.slant = fontRun.slant;
    }

    if( sizeOverriden )
    {
      const FontDescriptionRun& fontRun = *( fontDescriptionsBuffer + sizeIndex );
      fontPointSize = fontRun.size;
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
