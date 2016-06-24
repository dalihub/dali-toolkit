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
#include <dali/integration-api/debug.h>
#include <dali/devel-api/adaptor-framework/singleton-service.h>
#include <dali/devel-api/text-abstraction/font-client.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/multi-language-helper-functions.h>

namespace Dali
{

namespace Toolkit
{

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_MULTI_LANGUAGE_SUPPORT");
#endif

const Dali::Toolkit::Text::Character UTF32_A = 0x0041;
}

namespace Text
{

namespace Internal
{

bool ValidateFontsPerScript::IsValidFont( FontId fontId ) const
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

FontId DefaultFonts::FindFont( TextAbstraction::FontClient& fontClient, PointSize26Dot6 size ) const
{
  for( Vector<FontId>::ConstIterator it = mFonts.Begin(),
         endIt = mFonts.End();
       it != endIt;
       ++it )
  {
    const FontId fontId = *it;
    if( size == fontClient.GetPointSize( fontId ) )
    {
      return fontId;
    }
  }

  return 0u;
}

MultilanguageSupport::MultilanguageSupport()
: mDefaultFontPerScriptCache(),
  mValidFontsPerScriptCache()
{
  // Initializes the default font cache to zero (invalid font).
  // Reserves space to cache the default fonts and access them with the script as an index.
  mDefaultFontPerScriptCache.Resize( TextAbstraction::UNKNOWN, NULL );

  // Initializes the valid fonts cache to NULL (no valid fonts).
  // Reserves space to cache the valid fonts and access them with the script as an index.
  mValidFontsPerScriptCache.Resize( TextAbstraction::UNKNOWN, NULL );
}

MultilanguageSupport::~MultilanguageSupport()
{
  // Destroy the default font per script cache.
  for( Vector<DefaultFonts*>::Iterator it = mDefaultFontPerScriptCache.Begin(),
         endIt = mDefaultFontPerScriptCache.End();
       it != endIt;
       ++it )
  {
    delete *it;
  }

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
                                       CharacterIndex startIndex,
                                       Length numberOfCharacters,
                                       Vector<ScriptRun>& scripts )
{
  if( 0u == numberOfCharacters )
  {
    // Nothing to do if there are no characters.
    return;
  }

  // Find the first index where to insert the script.
  ScriptRunIndex scriptIndex = 0u;
  if( 0u != startIndex )
  {
    for( Vector<ScriptRun>::ConstIterator it = scripts.Begin(),
           endIt = scripts.End();
         it != endIt;
         ++it, ++scriptIndex )
    {
      const ScriptRun& run = *it;
      if( startIndex < run.characterRun.characterIndex + run.characterRun.numberOfCharacters )
      {
        // Run found.
        break;
      }
    }
  }

  // Stores the current script run.
  ScriptRun currentScriptRun;
  currentScriptRun.characterRun.characterIndex = startIndex;
  currentScriptRun.characterRun.numberOfCharacters = 0u;
  currentScriptRun.script = TextAbstraction::UNKNOWN;

  // Reserve some space to reduce the number of reallocations.
  scripts.Reserve( text.Count() << 2u );

  // Whether the first valid script needs to be set.
  bool isFirstScriptToBeSet = true;

  // Whether the first valid script is a right to left script.
  bool isParagraphRTL = false;

  // Count the number of characters which are valid for all scripts. i.e. white spaces or '\n'.
  Length numberOfAllScriptCharacters = 0u;

  // Pointers to the text buffer.
  const Character* const textBuffer = text.Begin();

  // Traverse all characters and set the scripts.
  const Length lastCharacter = startIndex + numberOfCharacters;
  for( Length index = startIndex; index < lastCharacter; ++index )
  {
    Character character = *( textBuffer + index );

    // Get the script of the character.
    Script script = TextAbstraction::GetCharacterScript( character );

    // Some characters (like white spaces) are valid for many scripts. The rules to set a script
    // for them are:
    // - If they are at the begining of a paragraph they get the script of the first character with
    //   a defined script. If they are at the end, they get the script of the last one.
    // - If they are between two scripts with the same direction, they get the script of the previous
    //   character with a defined script. If the two scripts have different directions, they get the
    //   script of the first character of the paragraph with a defined script.

    // Skip those characters valid for many scripts like white spaces or '\n'.
    bool endOfText = index == lastCharacter;
    while( !endOfText &&
           ( TextAbstraction::COMMON == script ) )
    {
      // Count all these characters to be added into a script.
      ++numberOfAllScriptCharacters;

      if( TextAbstraction::IsNewParagraph( character ) )
      {
        // The character is a new paragraph.
        // To know when there is a new paragraph is needed because if there is a white space
        // between two scripts with different directions, it is added to the script with
        // the same direction than the first script of the paragraph.
        isFirstScriptToBeSet = true;

        // Characters common to all scripts at the end of the paragraph are added to the last script.
        currentScriptRun.characterRun.numberOfCharacters += numberOfAllScriptCharacters;

        // Store the script run.
        if( TextAbstraction::UNKNOWN == currentScriptRun.script )
        {
          currentScriptRun.script = TextAbstraction::LATIN;
        }
        scripts.Insert( scripts.Begin() + scriptIndex, currentScriptRun );
        ++scriptIndex;

        // Initialize the new one.
        currentScriptRun.characterRun.characterIndex = currentScriptRun.characterRun.characterIndex + currentScriptRun.characterRun.numberOfCharacters;
        currentScriptRun.characterRun.numberOfCharacters = 0u;
        currentScriptRun.script = TextAbstraction::UNKNOWN;
        numberOfAllScriptCharacters = 0u;
      }

      // Get the next character.
      ++index;
      endOfText = index == lastCharacter;
      if( !endOfText )
      {
        character = *( textBuffer + index );
        script = TextAbstraction::GetCharacterScript( character );
      }
    }

    if( endOfText )
    {
      // Last characters of the text are 'white spaces'.
      // There is nothing else to do. Just add the remaining characters to the last script after this bucle.
      break;
    }

    // Check if it is the first character of a paragraph.
    if( isFirstScriptToBeSet &&
        ( TextAbstraction::UNKNOWN != script ) &&
        ( TextAbstraction::COMMON != script ) )
    {
      // Sets the direction of the first valid script.
      isParagraphRTL = TextAbstraction::IsRightToLeftScript( script );
      isFirstScriptToBeSet = false;
    }

    if( ( script != currentScriptRun.script ) &&
        ( TextAbstraction::COMMON != script ) )
    {
      // Current run needs to be stored and a new one initialized.

      if( ( isParagraphRTL == TextAbstraction::IsRightToLeftScript( currentScriptRun.script ) ) &&
          ( TextAbstraction::UNKNOWN != currentScriptRun.script ) )
      {
        // Previous script has the same direction than the first script of the paragraph.
        // All the previously skipped characters need to be added to the previous script before it's stored.
        currentScriptRun.characterRun.numberOfCharacters += numberOfAllScriptCharacters;
        numberOfAllScriptCharacters = 0u;
      }
      else if( ( TextAbstraction::IsRightToLeftScript( currentScriptRun.script ) == TextAbstraction::IsRightToLeftScript( script ) ) &&
               ( TextAbstraction::UNKNOWN != currentScriptRun.script ) )
      {
        // Current script and previous one have the same direction.
        // All the previously skipped characters need to be added to the previous script before it's stored.
        currentScriptRun.characterRun.numberOfCharacters += numberOfAllScriptCharacters;
        numberOfAllScriptCharacters = 0u;
      }

      if( 0u != currentScriptRun.characterRun.numberOfCharacters )
      {
        // Store the script run.
        scripts.Insert( scripts.Begin() + scriptIndex, currentScriptRun );
        ++scriptIndex;
      }

      // Initialize the new one.
      currentScriptRun.characterRun.characterIndex = currentScriptRun.characterRun.characterIndex + currentScriptRun.characterRun.numberOfCharacters;
      currentScriptRun.characterRun.numberOfCharacters = numberOfAllScriptCharacters + 1u; // Adds the white spaces which are at the begining of the script.
      currentScriptRun.script = script;
      numberOfAllScriptCharacters = 0u;
    }
    else
    {
      if( TextAbstraction::UNKNOWN != currentScriptRun.script )
      {
        // Adds white spaces between characters.
        currentScriptRun.characterRun.numberOfCharacters += numberOfAllScriptCharacters;
        numberOfAllScriptCharacters = 0u;
      }

      // Add one more character to the run.
      ++currentScriptRun.characterRun.numberOfCharacters;
    }
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
    scripts.Insert( scripts.Begin() + scriptIndex, currentScriptRun );
    ++scriptIndex;
  }

  if( scriptIndex < scripts.Count() )
  {
    // Update the indices of the next script runs.
    const ScriptRun& run = *( scripts.Begin() + scriptIndex - 1u );
    CharacterIndex nextCharacterIndex = run.characterRun.characterIndex + run.characterRun.numberOfCharacters;

    for( Vector<ScriptRun>::Iterator it = scripts.Begin() + scriptIndex,
           endIt = scripts.End();
         it != endIt;
         ++it )
    {
      ScriptRun& run = *it;
      run.characterRun.characterIndex = nextCharacterIndex;
      nextCharacterIndex += run.characterRun.numberOfCharacters;
    }
  }
}

void MultilanguageSupport::ValidateFonts( const Vector<Character>& text,
                                          const Vector<ScriptRun>& scripts,
                                          const Vector<FontDescriptionRun>& fontDescriptions,
                                          FontId defaultFontId,
                                          CharacterIndex startIndex,
                                          Length numberOfCharacters,
                                          Vector<FontRun>& fonts )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "-->MultilanguageSupport::ValidateFonts\n" );

  if( 0u == numberOfCharacters )
  {
    DALI_LOG_INFO( gLogFilter, Debug::General, "<--MultilanguageSupport::ValidateFonts\n" );
    // Nothing to do if there are no characters.
    return;
  }

  // Find the first index where to insert the font run.
  FontRunIndex fontIndex = 0u;
  if( 0u != startIndex )
  {
    for( Vector<FontRun>::ConstIterator it = fonts.Begin(),
           endIt = fonts.End();
         it != endIt;
         ++it, ++fontIndex )
    {
      const FontRun& run = *it;
      if( startIndex < run.characterRun.characterIndex + run.characterRun.numberOfCharacters )
      {
        // Run found.
        break;
      }
    }
  }

  // Traverse the characters and validate/set the fonts.

  // Get the caches.
  DefaultFonts** defaultFontPerScriptCacheBuffer = mDefaultFontPerScriptCache.Begin();
  ValidateFontsPerScript** validFontsPerScriptCacheBuffer = mValidFontsPerScriptCache.Begin();

  // Stores the validated font runs.
  fonts.Reserve( fontDescriptions.Count() );

  // Initializes a validated font run.
  FontRun currentFontRun;
  currentFontRun.characterRun.characterIndex = startIndex;
  currentFontRun.characterRun.numberOfCharacters = 0u;
  currentFontRun.fontId = 0u;

  // Get the font client.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  // Get the default font description and default size.
  TextAbstraction::FontDescription defaultFontDescription;
  TextAbstraction::PointSize26Dot6 defaultPointSize = TextAbstraction::FontClient::DEFAULT_POINT_SIZE;
  if( defaultFontId > 0u )
  {
    fontClient.GetDescription( defaultFontId, defaultFontDescription );
    defaultPointSize = fontClient.GetPointSize( defaultFontId );
  }

  // Merge font descriptions
  Vector<FontId> fontIds;
  fontIds.Resize( numberOfCharacters, defaultFontId );
  Vector<bool> isDefaultFont;
  isDefaultFont.Resize( numberOfCharacters, true );
  MergeFontDescriptions( fontDescriptions,
                         fontIds,
                         isDefaultFont,
                         defaultFontDescription,
                         defaultPointSize,
                         startIndex,
                         numberOfCharacters );

  const Character* const textBuffer = text.Begin();
  const FontId* const fontIdsBuffer = fontIds.Begin();
  const bool* const isDefaultFontBuffer = isDefaultFont.Begin();
  Vector<ScriptRun>::ConstIterator scriptRunIt = scripts.Begin();
  Vector<ScriptRun>::ConstIterator scriptRunEndIt = scripts.End();
  bool isNewParagraphCharacter = false;

  PointSize26Dot6 currentPointSize = defaultPointSize;
  FontId currentFontId = 0u;
  FontId previousFontId = 0u;

  // Whether it's the first set of characters to be validated.
  // Used in case the paragraph starts with characters common to all scripts.
  bool isFirstSetToBeValidated = true;

  CharacterIndex lastCharacter = startIndex + numberOfCharacters;
  for( Length index = startIndex; index < lastCharacter; ++index )
  {
    // Get the current character.
    const Character character = *( textBuffer + index );

    // Get the font for the current character.
    FontId fontId = *( fontIdsBuffer + index - startIndex );

    // Whether the font being validated for the current character is a default one not set by the user.
    const bool isDefault = *( isDefaultFontBuffer + index - startIndex );

    // Get the script for the current character.
    const Script script = GetScript( index,
                                     scriptRunIt,
                                     scriptRunEndIt );

    // Get the current point size.
    if( currentFontId != fontId )
    {
      currentPointSize = fontClient.GetPointSize( fontId );
      currentFontId = fontId;
    }

#ifdef DEBUG_ENABLED
    {
      Dali::TextAbstraction::FontDescription description;
      fontClient.GetDescription( fontId, description );

      DALI_LOG_INFO( gLogFilter,
                     Debug::Verbose,
                     "  Initial font set\n  Character : %x, Script : %s, Font : %s \n",
                     character,
                     Dali::TextAbstraction::ScriptName[script],
                     description.path.c_str() );
    }
#endif

    // Validate whether the current character is supported by the given font.
    bool isValidFont = false;

    // Check first in the cache of default fonts per script and size.

    DefaultFonts* defaultFonts = *( defaultFontPerScriptCacheBuffer + script );
    FontId cachedDefaultFontId = 0u;
    if( NULL != defaultFonts )
    {
      cachedDefaultFontId = defaultFonts->FindFont( fontClient, currentPointSize );
    }

    // Whether the cached default font is valid.
    const bool isValidCachedDefaultFont = 0u != cachedDefaultFontId;

    // The font is valid if it matches with the default one for the current script and size and it's different than zero.
    isValidFont = isValidCachedDefaultFont && ( fontId == cachedDefaultFontId );

    bool isCommonScript = false;

    // If the given font is not valid, it means either:
    // - there is no cached font for the current script yet or,
    // - the user has set a different font than the default one for the current script or,
    // - the platform default font is different than the default font for the current script.

    // Need to check if the given font supports the current character.
    if( !isValidFont ) // (1)
    {
      // Whether the current character is common for all scripts (i.e. white spaces, ...)

      // Is not desirable to cache fonts for the common script.
      //
      // i.e. Consider the text " हिंदी", the 'white space' has assigned the DEVANAGARI script.
      //      The user may have set a font or the platform's default is used.
      //
      //      As the 'white space' is the first character, no font is cached so the font validation
      //      retrieves a glyph from the given font.
      //
      //      Many fonts support 'white spaces' so probably the font set by the user or the platform's default
      //      supports the 'white space'. However, that font may not support the DEVANAGARI script.
      isCommonScript = TextAbstraction::IsCommonScript( character );

      if( isCommonScript )
      {
        if( isValidCachedDefaultFont &&
            ( isDefault || ( currentFontId == previousFontId ) )  )
        {
          // At this point the character common for all scripts has no font assigned.
          // If there is a valid previously cached default font for it, use that one.
          fontId = cachedDefaultFontId;
          isValidFont = true;
        }
      }
      else
      {
        // Check in the valid fonts cache.
        ValidateFontsPerScript* validateFontsPerScript = *( validFontsPerScriptCacheBuffer + script );

        if( NULL != validateFontsPerScript )
        {
          isValidFont = validateFontsPerScript->IsValidFont( fontId );
        }

        if( !isValidFont ) // (2)
        {
          // Use the font client to validate the font.
          const GlyphIndex glyphIndex = fontClient.GetGlyphIndex( fontId, character );

          // The font is valid if there is a glyph for that character.
          isValidFont = 0u != glyphIndex;

          // Emojis are present in many monochrome fonts; prefer color by default.
          if( isValidFont &&
              ( TextAbstraction::EMOJI == script ) )
          {
            const BufferImage bitmap = fontClient.CreateBitmap( fontId, glyphIndex );

            // For color emojis, the font is valid if the bitmap is RGBA.
            isValidFont = bitmap && ( Pixel::BGRA8888 == bitmap.GetPixelFormat() );
          }

          // If there is a valid font, cache it.
          if( isValidFont )
          {
            if( NULL == validateFontsPerScript )
            {
              validateFontsPerScript = new ValidateFontsPerScript();

              *( validFontsPerScriptCacheBuffer + script ) = validateFontsPerScript;
            }

            validateFontsPerScript->mValidFonts.PushBack( fontId );
          }

          if( !isValidFont ) // (3)
          {
            // The given font has not been validated.

            if( isValidCachedDefaultFont )
            {
              // Use the cached default font for the script if there is one.
              fontId = cachedDefaultFontId;
            }
            else
            {
              // There is no valid cached default font for the script.

              DefaultFonts* defaultFontsPerScript = NULL;

              // Emojis are present in many monochrome fonts; prefer color by default.
              const bool preferColor = ( TextAbstraction::EMOJI == script );

              // Find a fallback-font.
              fontId = fontClient.FindFallbackFont( currentFontId, character, currentPointSize, preferColor );

              if( 0u == fontId )
              {
                // If the system does not support a suitable font, fallback to Latin
                defaultFontsPerScript = *( defaultFontPerScriptCacheBuffer + TextAbstraction::LATIN );
                if( NULL != defaultFontsPerScript )
                {
                  fontId = defaultFontsPerScript->FindFont( fontClient, currentPointSize );
                }
              }

              if( 0u == fontId )
              {
                fontId = fontClient.FindDefaultFont( UTF32_A, currentPointSize );
              }

              // Cache the font.
              if( NULL == defaultFontsPerScript )
              {
                defaultFontsPerScript = new DefaultFonts();
                *( defaultFontPerScriptCacheBuffer + script ) = defaultFontsPerScript;
              }
              defaultFontsPerScript->mFonts.PushBack( fontId );
            }
          } // !isValidFont (3)
        } // !isValidFont (2)
      } // !isCommonScript
    } // !isValidFont (1)

#ifdef DEBUG_ENABLED
    {
      Dali::TextAbstraction::FontDescription description;
      fontClient.GetDescription( fontId, description );
      DALI_LOG_INFO( gLogFilter,
                     Debug::Verbose,
                     "  Validated font set\n  Character : %x, Script : %s, Font : %s \n",
                     character,
                     Dali::TextAbstraction::ScriptName[script],
                     description.path.c_str() );
    }
#endif

    if( isFirstSetToBeValidated && !isCommonScript )
    {
      currentFontRun.fontId = fontId;
      isFirstSetToBeValidated = false;
    }

    // The font is now validated.
    if( ( fontId != currentFontRun.fontId ) ||
        isNewParagraphCharacter )
    {
      // Current run needs to be stored and a new one initialized.

      if( 0u != currentFontRun.characterRun.numberOfCharacters )
      {
        // Store the font run.
        fonts.Insert( fonts.Begin() + fontIndex, currentFontRun );
        ++fontIndex;
      }

      // Initialize the new one.
      currentFontRun.characterRun.characterIndex = currentFontRun.characterRun.characterIndex + currentFontRun.characterRun.numberOfCharacters;
      currentFontRun.characterRun.numberOfCharacters = 0u;
      currentFontRun.fontId = fontId;

      if( isNewParagraphCharacter )
      {
        isFirstSetToBeValidated = true;
      }
    }

    // Add one more character to the run.
    ++currentFontRun.characterRun.numberOfCharacters;

    // Whether the current character is a new paragraph character.
    isNewParagraphCharacter = TextAbstraction::IsNewParagraph( character );
    previousFontId = currentFontId;
  } // end traverse characters.

  if( 0u != currentFontRun.characterRun.numberOfCharacters )
  {
    // Store the last run.
    fonts.Insert( fonts.Begin() + fontIndex, currentFontRun );
    ++fontIndex;
  }

  if( fontIndex < fonts.Count() )
  {
    // Update the indices of the next font runs.
    const FontRun& run = *( fonts.Begin() + fontIndex - 1u );
    CharacterIndex nextCharacterIndex = run.characterRun.characterIndex + run.characterRun.numberOfCharacters;

    for( Vector<FontRun>::Iterator it = fonts.Begin() + fontIndex,
           endIt = fonts.End();
         it != endIt;
         ++it )
    {
      FontRun& run = *it;

      run.characterRun.characterIndex = nextCharacterIndex;
      nextCharacterIndex += run.characterRun.numberOfCharacters;
    }
  }

  DALI_LOG_INFO( gLogFilter, Debug::General, "<--MultilanguageSupport::ValidateFonts\n" );
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali
