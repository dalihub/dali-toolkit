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

// CLASS HEADER
#include <dali-toolkit/internal/text/multi-language-support-impl.h>

// EXTERNAL INCLUDES
#include <memory.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/singleton-service.h>
#include <dali/public-api/text-abstraction/font-client.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/logical-model.h>
#include <dali-toolkit/public-api/text/font-run.h>
#include <dali-toolkit/public-api/text/script.h>
#include <dali-toolkit/public-api/text/script-run.h>

namespace Dali
{

namespace Toolkit
{

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_MULTI_LANGUAGE_SUPPORT");
#endif
}

namespace Text
{

namespace Internal
{

/**
 * @brief Retrieves the font Id from the font run for a given character's @p index.
 *
 * If the character's index exceeds the current font run it increases the iterator to get the next one.
 *
 * @param[in] index The character's index.
 * @param[in,out] fontRunIt Iterator to the current font run.
 * @param[in] fontRunEndIt Iterator to one after the last font run.
 *
 * @return The font id.
 */
FontId GetFontId( Length index,
                  Vector<FontRun>::ConstIterator& fontRunIt,
                  const Vector<FontRun>::ConstIterator& fontRunEndIt )
{
  FontId fontId = 0u;

  if( fontRunIt != fontRunEndIt )
  {
    const FontRun& fontRun = *fontRunIt;

    if( ( index >= fontRun.characterRun.characterIndex ) &&
        ( index < fontRun.characterRun.characterIndex + fontRun.characterRun.numberOfCharacters ) )
    {
      fontId = fontRun.fontId;
    }

    if( index + 1u == fontRun.characterRun.characterIndex + fontRun.characterRun.numberOfCharacters )
    {
      // All the characters of the current run have been traversed. Get the next one for the next iteration.
      ++fontRunIt;
    }
  }

  return fontId;
}

/**
 * @brief Retrieves the script Id from the script run for a given character's @p index.
 *
 * If the character's index exceeds the current script run it increases the iterator to get the next one.
 *
 * @param[in] index The character's index.
 * @param[in,out] scriptRunIt Iterator to the current font run.
 * @param[in] scriptRunEndIt Iterator to one after the last script run.
 *
 * @return The script.
 */
Script GetScript( Length index,
                  Vector<ScriptRun>::ConstIterator& scriptRunIt,
                  const Vector<ScriptRun>::ConstIterator& scriptRunEndIt )
{
  Script script = TextAbstraction::UNKNOWN;

  if( scriptRunIt != scriptRunEndIt )
  {
    const ScriptRun& scriptRun = *scriptRunIt;

    if( ( index >= scriptRun.characterRun.characterIndex ) &&
        ( index < scriptRun.characterRun.characterIndex + scriptRun.characterRun.numberOfCharacters ) )
    {
      script = scriptRun.script;
    }

    if( index + 1u == scriptRun.characterRun.characterIndex + scriptRun.characterRun.numberOfCharacters )
    {
      // All the characters of the current run have been traversed. Get the next one for the next iteration.
      ++scriptRunIt;
    }
  }

  return script;
}

/**
 * @brief Whether the character is valid for all scripts. i.e. the white space.
 *
 * @param[in] character The character.
 *
 * @return @e true if the character is valid for all scripts.
 */
bool IsValidForAllScripts( Character character )
{
  return ( IsWhiteSpace( character )         ||
           IsZeroWidthNonJoiner( character ) ||
           IsZeroWidthJoiner( character )    ||
           IsZeroWidthSpace( character )     ||
           IsLeftToRightMark( character )    ||
           IsRightToLeftMark( character )    ||
           IsThinSpace( character ) );
}

bool ValidateFontsPerScript::FindValidFont( FontId fontId ) const
{
  for( Vector<FontId>::ConstIterator it = mValidFonts.Begin(),
         endIt = mValidFonts.End();
       it != endIt;
       ++it )
  {
    if( fontId == *it )
    {
      return true;
    }
  }

  return false;
}

MultilanguageSupport::MultilanguageSupport()
: mDefaultFontPerScriptCache(),
  mValidFontsPerScriptCache()
{
  // Initializes the default font cache to zero (invalid font).
  // Reserves space to cache the default fonts and access them with the script as an index.
  mDefaultFontPerScriptCache.Resize( TextAbstraction::UNKNOWN, 0u );

  // Initializes the valid fonts cache to NULL (no valid fonts).
  // Reserves space to cache the valid fonts and access them with the script as an index.
  mValidFontsPerScriptCache.Resize( TextAbstraction::UNKNOWN, NULL );
}

MultilanguageSupport::~MultilanguageSupport()
{
  // Destroy the valid fonts per script cache.

  for( Vector<ValidateFontsPerScript*>::Iterator it = mValidFontsPerScriptCache.Begin(),
         endIt = mValidFontsPerScriptCache.End();
       it != endIt;
       ++it )
  {
    delete *it;
  }
}

Text::MultilanguageSupport MultilanguageSupport::Get()
{
  Text::MultilanguageSupport multilanguageSupportHandle;

  SingletonService service( SingletonService::Get() );
  if( service )
  {
    // Check whether the singleton is already created
    Dali::BaseHandle handle = service.GetSingleton( typeid( Text::MultilanguageSupport ) );
    if( handle )
    {
      // If so, downcast the handle
      MultilanguageSupport* impl = dynamic_cast< Internal::MultilanguageSupport* >( handle.GetObjectPtr() );
      multilanguageSupportHandle = Text::MultilanguageSupport( impl );
    }
    else // create and register the object
    {
      multilanguageSupportHandle = Text::MultilanguageSupport( new MultilanguageSupport );
      service.Register( typeid( multilanguageSupportHandle ), multilanguageSupportHandle );
    }
  }

  return multilanguageSupportHandle;
}

void MultilanguageSupport::SetScripts( const Vector<Character>& text,
                                       const Vector<LineBreakInfo>& lineBreakInfo,
                                       Vector<ScriptRun>& scripts )
{
  const Length numberOfCharacters = text.Count();

  if( 0u == numberOfCharacters )
  {
    // Nothing to do if there are no characters.
    return;
  }

  // Stores the current script run.
  ScriptRun currentScriptRun;
  currentScriptRun.characterRun.characterIndex = 0u;
  currentScriptRun.characterRun.numberOfCharacters = 0u;
  currentScriptRun.script = TextAbstraction::UNKNOWN;

  // Reserve some space to reduce the number of reallocations.
  scripts.Reserve( numberOfCharacters << 2u );

  // Whether the first valid script need to be set.
  bool firstValidScript = true;

  // Whether the first valid script is a right to left script.
  bool isParagraphRTL = false;

  // Count the number of characters which are valid for all scripts. i.e. white spaces or '\n'.
  Length numberOfAllScriptCharacters = 0u;

  // Pointers to the text and break info buffers.
  const Character* textBuffer = text.Begin();
  const LineBreakInfo* breakInfoBuffer = lineBreakInfo.Begin();

  // Traverse all characters and set the scripts.
  for( Length index = 0u; index < numberOfCharacters; ++index )
  {
    Character character = *( textBuffer + index );
    LineBreakInfo breakInfo = *( breakInfoBuffer + index );

    // Some characters (like white spaces) are valid for many scripts. The rules to set a script
    // for them are:
    // - If they are at the begining of a paragraph they get the script of the first character with
    //   a defined script. If they are at the end, they get the script of the last one.
    // - If they are between two scripts with the same direction, they get the script of the previous
    //   character with a defined script. If the two scripts have different directions, they get the
    //   script of the first character of the paragraph with a defined script.

    // Skip those characters valid for many scripts like white spaces or '\n'.
    bool endOfText = index == numberOfCharacters;
    while( !endOfText &&
           IsValidForAllScripts( character ) )
    {
      // Count all these characters to be added into a script.
      ++numberOfAllScriptCharacters;

      if( TextAbstraction::LINE_MUST_BREAK == breakInfo )
      {
        // The next character is a new paragraph.
        // Know when there is a new paragraph is needed because if there is a white space
        // between two scripts with different directions, it is added to the script with
        // the same direction than the first script of the paragraph.
        firstValidScript = true;
        isParagraphRTL = false;
      }

      // Get the next character.
      ++index;
      endOfText = index == numberOfCharacters;
      if( !endOfText )
      {
        character = *( textBuffer + index );
        breakInfo = *( breakInfoBuffer + index );
      }
    }

    if( endOfText )
    {
      // Last characters of the text are 'white spaces'.
      // There is nothing else to do. Just add the remaining characters to the last script after this bucle.
      break;
    }

    // Get the script of the character.
    Script script = GetCharacterScript( character );

    // Check if it is the first character of a paragraph.
    if( firstValidScript &&
        ( TextAbstraction::UNKNOWN != script ) )
    {
      // Sets the direction of the first valid script.
      isParagraphRTL = ( TextAbstraction::ARABIC == script );
      firstValidScript = false;
    }

    if( script != currentScriptRun.script )
    {
      // Current run needs to be stored and a new one initialized.

      if( isParagraphRTL != ( TextAbstraction::ARABIC == script ) )
      {
        // Current script has different direction than the first script of the paragraph.
        // All the previously skipped characters need to be added to the previous script before it's stored.
        currentScriptRun.characterRun.numberOfCharacters += numberOfAllScriptCharacters;
        numberOfAllScriptCharacters = 0u;
      }

      if( 0u != currentScriptRun.characterRun.numberOfCharacters )
      {
        // Store the script run.
        scripts.PushBack( currentScriptRun );
      }

      // Initialize the new one.
      currentScriptRun.characterRun.characterIndex = currentScriptRun.characterRun.characterIndex + currentScriptRun.characterRun.numberOfCharacters;
      currentScriptRun.characterRun.numberOfCharacters = numberOfAllScriptCharacters; // Adds the white spaces which are at the begining of the script.
      currentScriptRun.script = script;
      numberOfAllScriptCharacters = 0u;
    }
    else
    {
      // Adds white spaces between characters.
      currentScriptRun.characterRun.numberOfCharacters += numberOfAllScriptCharacters;
      numberOfAllScriptCharacters = 0u;
    }

    if( TextAbstraction::LINE_MUST_BREAK == breakInfo )
    {
      // The next character is a new paragraph.
      firstValidScript = true;
      isParagraphRTL = false;
    }

    // Add one more character to the run.
    ++currentScriptRun.characterRun.numberOfCharacters;
  }

  // Add remaining characters into the last script.
  currentScriptRun.characterRun.numberOfCharacters += numberOfAllScriptCharacters;
  if( 0u != currentScriptRun.characterRun.numberOfCharacters )
  {
    if( TextAbstraction::UNKNOWN == currentScriptRun.script )
    {
      // There are only white spaces in the last script. Set the latin script.
      currentScriptRun.script = TextAbstraction::LATIN;
    }

    // Store the last run.
    scripts.PushBack( currentScriptRun );
  }
}

void MultilanguageSupport::ReplaceScripts( LogicalModel& model,
                                           CharacterIndex characterIndex,
                                           Length numberOfCharactersToRemove,
                                           Length numberOfCharactersToInsert )
{
}

void MultilanguageSupport::ValidateFonts( const Vector<Character>& text,
                                          const Vector<ScriptRun>& scripts,
                                          Vector<FontRun>& fonts )
{
  const Length numberOfCharacters = text.Count();

  if( 0u == numberOfCharacters )
  {
    // Nothing to do if there are no characters.
    return;
  }

  // Copy the fonts set by application developers.
  const Length numberOfFontRuns = fonts.Count();
  const Vector<FontRun> definedFonts = fonts;
  fonts.Clear();

  // Traverse the characters and validate/set the fonts.

  // Get the caches.
  FontId* defaultFontPerScriptCacheBuffer = mDefaultFontPerScriptCache.Begin();
  ValidateFontsPerScript** validFontsPerScriptCacheBuffer = mValidFontsPerScriptCache.Begin();

  // Stores the validated font runs.
  fonts.Reserve( numberOfFontRuns );

  // Initializes a validated font run.
  FontRun currentFontRun;
  currentFontRun.characterRun.characterIndex = 0u;
  currentFontRun.characterRun.numberOfCharacters = 0u;
  currentFontRun.fontId = 0u;
  currentFontRun.isDefault = false;

  // Get the font client.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  // Iterators of the font and script runs.
  Vector<FontRun>::ConstIterator fontRunIt = definedFonts.Begin();
  Vector<FontRun>::ConstIterator fontRunEndIt = definedFonts.End();
  Vector<ScriptRun>::ConstIterator scriptRunIt = scripts.Begin();
  Vector<ScriptRun>::ConstIterator scriptRunEndIt = scripts.End();

  for( Length index = 0u; index < numberOfCharacters; ++index )
  {
    // Get the character.
    const Character character = *( text.Begin() + index );

    // Get the font for the character.
    FontId fontId = GetFontId( index,
                               fontRunIt,
                               fontRunEndIt );

    // Get the script for the character.
    Script script = GetScript( index,
                               scriptRunIt,
                               scriptRunEndIt );

    if( TextAbstraction::UNKNOWN == script )
    {
      DALI_LOG_WARNING( "MultilanguageSupport::ValidateFonts. Unknown script!" );
      script = TextAbstraction::LATIN;
    }

    // Whether the font being validated is a default one not set by the user.
    const bool isDefault = ( 0u == fontId );

    // The default font point size.
    PointSize26Dot6 pointSize = TextAbstraction::FontClient::DEFAULT_POINT_SIZE;

    if( !isDefault )
    {
      // Validate if the font set by the user supports the character.

      // Check first in the caches.

      // The user may have set the default font. Check it. Otherwise check in the valid fonts cache.
      if( fontId != *( defaultFontPerScriptCacheBuffer + script ) )
      {
        // Check in the valid fonts cache.
        ValidateFontsPerScript* validateFontsPerScript = *( validFontsPerScriptCacheBuffer + script );
        if( NULL != validateFontsPerScript )
        {
          if( !validateFontsPerScript->FindValidFont( fontId ) )
          {
            // Use the font client to validate the font.
            const GlyphIndex glyphIndex = fontClient.GetGlyphIndex( fontId, character );

            if( 0u == glyphIndex )
            {
              // Get the point size of the current font. It will be used to get a default font id.
              pointSize = fontClient.GetPointSize( fontId );

              // The font is not valid. Set to zero and a default one will be set.
              fontId = 0u;
            }
            else
            {
              // Add the font to the valid font cache.
              validateFontsPerScript->mValidFonts.PushBack( fontId );
            }
          }
        }
        else
        {
          // Use the font client to validate the font.
          const GlyphIndex glyphIndex = fontClient.GetGlyphIndex( fontId, character );

          if( 0u == glyphIndex )
          {
            // Get the point size of the current font. It will be used to get a default font id.
            pointSize = fontClient.GetPointSize( fontId );

            // The font is not valid. Set to zero and a default one will be set.
            fontId = 0u;
          }
          else
          {
            // Add the font to the valid font cache.
            validateFontsPerScript = new ValidateFontsPerScript();
            *( validFontsPerScriptCacheBuffer + script ) = validateFontsPerScript;

            validateFontsPerScript->mValidFonts.PushBack( fontId );
          }
        }
      }
    }

    // The font has not been validated. Find a default one.
    if( 0u == fontId )
    {
      // The character has no font assigned. Get a default one from the cache
      fontId = *( defaultFontPerScriptCacheBuffer + script );

      // If the cache has not a default font, get one from the font client.
      if( 0u == fontId )
      {
        // Find a default font.
        fontId = fontClient.FindDefaultFont( character, pointSize );

#ifdef DEBUG_ENABLED
        Dali::TextAbstraction::FontDescription description;
        fontClient.GetDescription( fontId, description );
        DALI_LOG_INFO( gLogFilter, Debug::Concise, "Script: %s; Selected font: %s\n", Dali::TextAbstraction::ScriptName[script], description.path.c_str() );
#endif
        // Cache the font.
        *( defaultFontPerScriptCacheBuffer + script ) = fontId;
      }
    }

    // The font is now validated.

    if( ( fontId != currentFontRun.fontId ) ||
        ( isDefault != currentFontRun.isDefault ) )
    {
      // Current run needs to be stored and a new one initialized.

      if( 0u != currentFontRun.characterRun.numberOfCharacters )
      {
        // Store the font run.
        fonts.PushBack( currentFontRun );
      }

      // Initialize the new one.
      currentFontRun.characterRun.characterIndex = currentFontRun.characterRun.characterIndex + currentFontRun.characterRun.numberOfCharacters;
      currentFontRun.characterRun.numberOfCharacters = 0u;
      currentFontRun.fontId = fontId;
      currentFontRun.isDefault = isDefault;
    }

    // Add one more character to the run.
    ++currentFontRun.characterRun.numberOfCharacters;
  }

  if( 0u != currentFontRun.characterRun.numberOfCharacters )
  {
    // Store the last run.
    fonts.PushBack( currentFontRun );
  }
}

void MultilanguageSupport::ValidateFonts( LogicalModel& model,
                                          CharacterIndex characterIndex,
                                          Length numberOfCharactersToRemove,
                                          Length numberOfCharactersToInsert )
{
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali
