/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/environment-variable.h>
#include <dali/devel-api/common/singleton-service.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/emoji-helper.h>
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

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_FONT_PERFORMANCE_MARKER, false);

const Dali::Toolkit::Text::Character UTF32_A     = 0x0041;
const Dali::Toolkit::Text::Character UTF32_COLON = 0x3A;
const Dali::Toolkit::Text::Character UTF32_EMOJI = 0x1F600; // Grinning Face

const char* DALI_TEXT_ENABLE_ICU("DALI_TEXT_ENABLE_ICU");
const int   DEFAULT_ENABLE_ICU   = 0;
const char* THAILAND_LOCALE_CODE = "th_TH";

} // namespace

namespace Text
{
namespace Internal
{

namespace
{
void CheckFontSupportsCharacter(
  bool& isValidFont,
  bool& isCommonScript,
  const Character& character,
  ValidateFontsPerScript**& validFontsPerScriptCacheBuffer,
  const Script& script,
  FontId& fontId,
  TextAbstraction::FontClient& fontClient,
  const bool isValidCachedDefaultFont,
  const FontId& cachedDefaultFontId,
  const TextAbstraction::FontDescription& currentFontDescription,
  const TextAbstraction::PointSize26Dot6& currentFontPointSize,
  DefaultFonts**& defaultFontPerScriptCacheBuffer,
  bool findFallbackFont)
{
  // Need to check if the given font supports the current character.
  if(!isValidFont && !findFallbackFont) // (1)
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
    isCommonScript = TextAbstraction::IsCommonScript(character) || TextAbstraction::IsEmojiPresentationSelector(character);

    // Check in the valid fonts cache.
    ValidateFontsPerScript* validateFontsPerScript = *(validFontsPerScriptCacheBuffer + script);

    if(NULL != validateFontsPerScript)
    {
      // This cache stores valid fonts set by the user.
      isValidFont = validateFontsPerScript->IsValidFont(fontId);

      // It may happen that a validated font for a script doesn't have all the glyphs for that script.
      // i.e a font validated for the CJK script may contain glyphs for the chinese language but not for the Japanese.
      if(isValidFont)
      {
        // Checks if the current character is supported by the font is needed.
        isValidFont = fontClient.IsCharacterSupportedByFont(fontId, character);
      }
    }

    if(!isValidFont) // (2)
    {
      // The selected font is not stored in any cache.

      // Checks if the current character is supported by the selected font.
      isValidFont = fontClient.IsCharacterSupportedByFont(fontId, character);

      // If there is a valid font, cache it.
      if(isValidFont && !isCommonScript)
      {
        if(NULL == validateFontsPerScript)
        {
          validateFontsPerScript = new ValidateFontsPerScript();

          *(validFontsPerScriptCacheBuffer + script) = validateFontsPerScript;
        }

        validateFontsPerScript->Cache(fontId);
      }

      if(!isValidFont && (fontId != cachedDefaultFontId) && (!TextAbstraction::IsNewParagraph(character))) // (3)
      {
        // The selected font by the user or the platform's default font has failed to validate the character.

        // Checks if the previously discarted cached default font supports the character.
        bool isValidCachedFont = false;
        if(isValidCachedDefaultFont)
        {
          isValidCachedFont = fontClient.IsCharacterSupportedByFont(cachedDefaultFontId, character);
        }

        if(isValidCachedFont)
        {
          // Use the cached default font for the script if there is one.
          fontId      = cachedDefaultFontId;
          isValidFont = true;
        }
        else
        {
          // There is no valid cached default font for the script.

          DefaultFonts* defaultFontsPerScript = NULL;

          // Find a fallback-font.
          fontId = fontClient.FindFallbackFont(character,
                                               currentFontDescription,
                                               currentFontPointSize,
                                               false);

          if(0u == fontId)
          {
            fontId = fontClient.FindDefaultFont(UTF32_A, currentFontPointSize);
          }

          if(!isCommonScript && (script != TextAbstraction::UNKNOWN))
          {
            // Cache the font if it is not an unknown script
            if(NULL == defaultFontsPerScript)
            {
              defaultFontsPerScript = *(defaultFontPerScriptCacheBuffer + script);

              if(NULL == defaultFontsPerScript)
              {
                defaultFontsPerScript                       = new DefaultFonts();
                *(defaultFontPerScriptCacheBuffer + script) = defaultFontsPerScript;
              }
            }

            // the fontId is cached only if it has not been cached before.
            if(!isValidCachedDefaultFont)
            {
              defaultFontsPerScript->Cache(currentFontDescription, fontId);
            }

            isValidFont = true;
          }
        }
      } // !isValidFont (3)
    }   // !isValidFont (2)
  }     // !isValidFont (1)
  else if(!isValidFont && findFallbackFont)
  {
    // Find a fallback-font.
    fontId = fontClient.FindFallbackFont(character,
                                         currentFontDescription,
                                         currentFontPointSize,
                                         false);

    if(0u == fontId)
    {
      fontId = fontClient.FindDefaultFont(character, currentFontPointSize);
    }
    isValidFont = true;
  }
}
} // unnamed namespace

bool ValidateFontsPerScript::IsValidFont(FontId fontId) const
{
  for(Vector<FontId>::ConstIterator it    = mValidFonts.Begin(),
                                    endIt = mValidFonts.End();
      it != endIt;
      ++it)
  {
    if(fontId == *it)
    {
      return true;
    }
  }

  return false;
}
void ValidateFontsPerScript::Cache(FontId fontId)
{
  mValidFonts.PushBack(fontId);
  return;
}

FontId DefaultFonts::FindFont(TextAbstraction::FontClient&            fontClient,
                              const TextAbstraction::FontDescription& description,
                              PointSize26Dot6                         size,
                              Character                               character) const
{
  for(std::vector<CacheItem>::const_iterator it    = mFonts.begin(),
                                             endIt = mFonts.end();
      it != endIt;
      ++it)
  {
    const CacheItem& item = *it;

    if(((TextAbstraction::FontWeight::NONE == description.weight) || (description.weight == item.description.weight)) &&
       ((TextAbstraction::FontWidth::NONE == description.width) || (description.width == item.description.width)) &&
       ((TextAbstraction::FontSlant::NONE == description.slant) || (description.slant == item.description.slant)) &&
       (size == fontClient.GetPointSize(item.fontId)) &&
       (description.family.empty() || (description.family == item.description.family)) &&
       fontClient.IsCharacterSupportedByFont(item.fontId, character))
    {
      return item.fontId;
    }
  }

  return 0u;
}

void DefaultFonts::Cache(const TextAbstraction::FontDescription& description, FontId fontId)
{
  CacheItem item;
  item.description = description;
  item.fontId      = fontId;
  mFonts.push_back(item);

  return;
}

MultilanguageSupport::MultilanguageSupport(bool connectLocaleChangedSignal)
: mICU(),
  mDefaultFontPerScriptCache(),
  mValidFontsPerScriptCache(),
  mLocale(),
  mIsICUEnabled(false),
  mIsICULineBreakNeededForLocale(false)
{
  // Initializes the default font cache to zero (invalid font).
  // Reserves space to cache the default fonts and access them with the script as an index.
  mDefaultFontPerScriptCache.Resize(TextAbstraction::GetNumberOfScripts(), NULL);

  // Initializes the valid fonts cache to NULL (no valid fonts).
  // Reserves space to cache the valid fonts and access them with the script as an index.
  mValidFontsPerScriptCache.Resize(TextAbstraction::GetNumberOfScripts(), NULL);

  mLocale = TextAbstraction::GetLocaleFull();

  // Check environment variable for DALI_TEXT_ENABLE_ICU.
  auto enableICUstring = Dali::EnvironmentVariable::GetEnvironmentVariable(DALI_TEXT_ENABLE_ICU);
  int  enableICU       = enableICUstring ? std::atoi(enableICUstring) : DEFAULT_ENABLE_ICU;
  if(enableICU == 1)
  {
    mIsICUEnabled                  = true;
    mIsICULineBreakNeededForLocale = IsICULineBreakNeededForLocale();
    mICU                           = TextAbstraction::ICU::New();
  }

  if(connectLocaleChangedSignal && Dali::Adaptor::IsAvailable())
  {
    Dali::Adaptor::Get().LocaleChangedSignal().Connect(this, &MultilanguageSupport::OnLocaleChanged);
  }
}

MultilanguageSupport::~MultilanguageSupport()
{
  // Destroy the default font per script cache.
  for(Vector<DefaultFonts*>::Iterator it    = mDefaultFontPerScriptCache.Begin(),
                                      endIt = mDefaultFontPerScriptCache.End();
      it != endIt;
      ++it)
  {
    delete *it;
  }

  // Destroy the valid fonts per script cache.
  for(Vector<ValidateFontsPerScript*>::Iterator it    = mValidFontsPerScriptCache.Begin(),
                                                endIt = mValidFontsPerScriptCache.End();
      it != endIt;
      ++it)
  {
    delete *it;
  }
}

void MultilanguageSupport::OnLocaleChanged(std::string locale)
{
  SetLocale(locale);
  ClearCache();
}

void MultilanguageSupport::ClearCache()
{
  for(Vector<DefaultFonts*>::Iterator it    = mDefaultFontPerScriptCache.Begin(),
                                      endIt = mDefaultFontPerScriptCache.End();
      it != endIt;
      ++it)
  {
    delete *it;
  }

  for(Vector<ValidateFontsPerScript*>::Iterator it    = mValidFontsPerScriptCache.Begin(),
                                                endIt = mValidFontsPerScriptCache.End();
      it != endIt;
      ++it)
  {
    delete *it;
  }

  mDefaultFontPerScriptCache.Clear();
  mValidFontsPerScriptCache.Clear();

  mDefaultFontPerScriptCache.Resize(TextAbstraction::GetNumberOfScripts(), NULL);
  mValidFontsPerScriptCache.Resize(TextAbstraction::GetNumberOfScripts(), NULL);
}

const std::string& MultilanguageSupport::GetLocale()
{
  return mLocale;
}

void MultilanguageSupport::SetLocale(const std::string& locale)
{
  mLocale                        = locale;
  mIsICULineBreakNeededForLocale = IsICULineBreakNeededForLocale();
}

bool MultilanguageSupport::IsICULineBreakNeeded()
{
  return mIsICUEnabled && mIsICULineBreakNeededForLocale;
}

void MultilanguageSupport::UpdateICULineBreak(const std::string&              text,
                                              TextAbstraction::Length         numberOfCharacters,
                                              TextAbstraction::LineBreakInfo* breakInfo)
{
  if(mIsICUEnabled && mICU)
  {
    mICU.UpdateLineBreakInfoByLocale(text, numberOfCharacters, mLocale.c_str(), breakInfo);
  }
}

Text::MultilanguageSupport MultilanguageSupport::Get()
{
  Text::MultilanguageSupport multilanguageSupportHandle;

  SingletonService service(SingletonService::Get());
  if(service)
  {
    // Check whether the singleton is already created
    Dali::BaseHandle handle = service.GetSingleton(typeid(Text::MultilanguageSupport));
    if(handle)
    {
      // If so, downcast the handle
      MultilanguageSupport* impl = dynamic_cast<Internal::MultilanguageSupport*>(handle.GetObjectPtr());
      multilanguageSupportHandle = Text::MultilanguageSupport(impl);
    }
    else // create and register the object
    {
      multilanguageSupportHandle = Text::MultilanguageSupport(new MultilanguageSupport);
      service.Register(typeid(multilanguageSupportHandle), multilanguageSupportHandle);
    }
  }

  return multilanguageSupportHandle;
}

void MultilanguageSupport::SetScripts(const Vector<Character>& text,
                                      CharacterIndex           startIndex,
                                      Length                   numberOfCharacters,
                                      Vector<ScriptRun>&       scripts)
{
  if(0u == numberOfCharacters)
  {
    // Nothing to do if there are no characters.
    return;
  }

  // Find the first index where to insert the script.
  ScriptRunIndex scriptIndex = 0u;
  if(0u != startIndex)
  {
    for(Vector<ScriptRun>::ConstIterator it    = scripts.Begin(),
                                         endIt = scripts.End();
        it != endIt;
        ++it, ++scriptIndex)
    {
      const ScriptRun& run = *it;
      if(startIndex < run.characterRun.characterIndex + run.characterRun.numberOfCharacters)
      {
        // Run found.
        break;
      }
    }
  }

  // Stores the current script run.
  ScriptRun currentScriptRun;
  currentScriptRun.characterRun.characterIndex     = startIndex;
  currentScriptRun.characterRun.numberOfCharacters = 0u;
  currentScriptRun.script                          = TextAbstraction::UNKNOWN;

  // Reserve some space to reduce the number of reallocations.
  scripts.Reserve(text.Count() << 2u);

  // Whether the first valid script needs to be set.
  bool isFirstScriptToBeSet = true;

  // Whether the first valid script is a right to left script.
  bool isParagraphRTL = false;

  // Whether there is an RTL marker in the invalid script.
  // This variable was added to solve the problem that autoscroll does not work properly when there are only RTL Marker and LTR Text.
  bool hasRTLMarker = false;

  // Count the number of characters which are valid for all scripts. i.e. white spaces or '\n'.
  Length numberOfAllScriptCharacters = 0u;

  // Pointers to the text buffer.
  const Character* const textBuffer = text.Begin();

  // Initialize whether is right to left direction
  currentScriptRun.isRightToLeft = false;

  // Traverse all characters and set the scripts.
  const Length lastCharacter = startIndex + numberOfCharacters - 1u;

  for(Length index = startIndex; index <= lastCharacter; ++index)
  {
    Character character = *(textBuffer + index);

    // Get the script of the character.
    Script script = TextAbstraction::GetCharacterScript(character);

    // Some characters (like white spaces) are valid for many scripts. The rules to set a script
    // for them are:
    // - If they are at the begining of a paragraph they get the script of the first character with
    //   a defined script. If they are at the end, they get the script of the last one.
    // - If they are between two scripts with the same direction, they get the script of the previous
    //   character with a defined script. If the two scripts have different directions, they get the
    //   script of the first character of the paragraph with a defined script.

    // Skip those characters valid for many scripts like white spaces or '\n'.
    bool endOfText = index > lastCharacter;

    //Handle all Emoji Sequence cases
    if(IsNewSequence(textBuffer, currentScriptRun.script, index, lastCharacter, script))
    {
      AddCurrentScriptAndCreatNewScript(script,
                                        false,
                                        false,
                                        currentScriptRun,
                                        numberOfAllScriptCharacters,
                                        scripts,
                                        scriptIndex);
    }
    else if(IsScriptChangedToFollowSequence(currentScriptRun.script, character, script))
    {
      // To guarantee behavior of VARIATION_SELECTOR_15.
      if(currentScriptRun.script != TextAbstraction::EMOJI_TEXT)
      {
        currentScriptRun.script = script;
      }
    }
    else if(IsOneOfEmojiScripts(currentScriptRun.script) && (TextAbstraction::COMMON == script))
    {
      // Emojis doesn't mix well with characters common to all scripts. Insert the emoji run.
      AddCurrentScriptAndCreatNewScript(TextAbstraction::UNKNOWN,
                                        false,
                                        false,
                                        currentScriptRun,
                                        numberOfAllScriptCharacters,
                                        scripts,
                                        scriptIndex);
    }

    while(!endOfText &&
          (TextAbstraction::COMMON == script))
    {
      // Check if whether is right to left markup and Keeps true if the previous value was true.
      currentScriptRun.isRightToLeft = currentScriptRun.isRightToLeft || TextAbstraction::IsRightToLeftMark(character);
      hasRTLMarker = hasRTLMarker || TextAbstraction::IsRightToLeftMark(character);

      // Count all these characters to be added into a script.
      ++numberOfAllScriptCharacters;

      if(TextAbstraction::IsNewParagraph(character))
      {
        // The character is a new paragraph.
        // To know when there is a new paragraph is needed because if there is a white space
        // between two scripts with different directions, it is added to the script with
        // the same direction than the first script of the paragraph.
        isFirstScriptToBeSet = true;

        AddCurrentScriptAndCreatNewScript(TextAbstraction::UNKNOWN,
                                          false,
                                          false,
                                          currentScriptRun,
                                          numberOfAllScriptCharacters,
                                          scripts,
                                          scriptIndex);
      }

      // Get the next character.
      ++index;
      endOfText = index > lastCharacter;
      if(!endOfText)
      {
        character = *(textBuffer + index);
        script    = TextAbstraction::GetCharacterScript(character);

        //Handle all Emoji Sequence cases
        if(IsNewSequence(textBuffer, currentScriptRun.script, index, lastCharacter, script))
        {
          AddCurrentScriptAndCreatNewScript(script,
                                            false,
                                            false,
                                            currentScriptRun,
                                            numberOfAllScriptCharacters,
                                            scripts,
                                            scriptIndex);
        }
        else if(IsScriptChangedToFollowSequence(currentScriptRun.script, character, script))
        {
          currentScriptRun.script = script;
        }
      }
    } // end while( !endOfText && ( TextAbstraction::COMMON == script ) )

    if(endOfText)
    {
      // Last characters of the text are 'white spaces'.
      // There is nothing else to do. Just add the remaining characters to the last script after this bucle.
      break;
    }

    // Check if it is the first character of a paragraph.
    if(isFirstScriptToBeSet &&
       (TextAbstraction::UNKNOWN != script) &&
       (TextAbstraction::COMMON != script) &&
       (TextAbstraction::EMOJI != script) &&
       (TextAbstraction::EMOJI_TEXT != script) &&
       (TextAbstraction::EMOJI_COLOR != script) &&
       (!TextAbstraction::IsSymbolScript(script)))
    {
      // Sets the direction of the first valid script.
      isParagraphRTL       = currentScriptRun.isRightToLeft || TextAbstraction::IsRightToLeftScript(script);
      isFirstScriptToBeSet = false;
    }

    // If the current script run is ASCII_DIGITS and character is colon, include the colon in the same script run.
    // TODO : Use the same script for all ASCII_DIGITS and ASCII_PS.
    bool isColonCase = (character == UTF32_COLON) && (TextAbstraction::ASCII_DIGITS == currentScriptRun.script) && (TextAbstraction::COMMON != script);

    if((script != currentScriptRun.script) &&
       (TextAbstraction::COMMON != script) &&
       !isColonCase)
    {
      // Current run needs to be stored and a new one initialized.

      if((isParagraphRTL == TextAbstraction::IsRightToLeftScript(currentScriptRun.script)) &&
         (TextAbstraction::UNKNOWN != currentScriptRun.script))
      {
        // Previous script has the same direction than the first script of the paragraph.
        // All the previously skipped characters need to be added to the previous script before it's stored.
        currentScriptRun.characterRun.numberOfCharacters += numberOfAllScriptCharacters;
        numberOfAllScriptCharacters = 0u;
      }
      else if((TextAbstraction::IsRightToLeftScript(currentScriptRun.script) == TextAbstraction::IsRightToLeftScript(script)) &&
              (TextAbstraction::UNKNOWN != currentScriptRun.script))
      {
        // Current script and previous one have the same direction.
        // All the previously skipped characters need to be added to the previous script before it's stored.
        currentScriptRun.characterRun.numberOfCharacters += numberOfAllScriptCharacters;
        numberOfAllScriptCharacters = 0u;
      }
      else if((TextAbstraction::UNKNOWN == currentScriptRun.script) &&
              (TextAbstraction::IsSymbolOrEmojiOrTextScript(script)))
      {
        currentScriptRun.characterRun.numberOfCharacters += numberOfAllScriptCharacters;
        numberOfAllScriptCharacters = 0u;
      }

      // Adds the white spaces which are at the begining of the script.
      numberOfAllScriptCharacters++;
      AddCurrentScriptAndCreatNewScript(script,
                                        hasRTLMarker ? true : TextAbstraction::IsRightToLeftScript(script),
                                        true,
                                        currentScriptRun,
                                        numberOfAllScriptCharacters,
                                        scripts,
                                        scriptIndex);

      hasRTLMarker = false;
    }
    else
    {
      if(TextAbstraction::UNKNOWN != currentScriptRun.script)
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

  if(0u != currentScriptRun.characterRun.numberOfCharacters)
  {
    // Store the last run.
    scripts.Insert(scripts.Begin() + scriptIndex, currentScriptRun);
    ++scriptIndex;
  }

  if(scriptIndex < scripts.Count())
  {
    // Update the indices of the next script runs.
    const ScriptRun& run                = *(scripts.Begin() + scriptIndex - 1u);
    CharacterIndex   nextCharacterIndex = run.characterRun.characterIndex + run.characterRun.numberOfCharacters;

    for(Vector<ScriptRun>::Iterator it    = scripts.Begin() + scriptIndex,
                                    endIt = scripts.End();
        it != endIt;
        ++it)
    {
      ScriptRun& run                  = *it;
      run.characterRun.characterIndex = nextCharacterIndex;
      nextCharacterIndex += run.characterRun.numberOfCharacters;
    }
  }
}

void MultilanguageSupport::ValidateFonts(TextAbstraction::FontClient&            fontClient,
                                         const Vector<Character>&                text,
                                         const Vector<ScriptRun>&                scripts,
                                         const Vector<FontDescriptionRun>&       fontDescriptions,
                                         const TextAbstraction::FontDescription& defaultFontDescription,
                                         TextAbstraction::PointSize26Dot6        defaultFontPointSize,
                                         float                                   fontSizeScale,
                                         CharacterIndex                          startIndex,
                                         Length                                  numberOfCharacters,
                                         Vector<FontRun>&                        fonts,
                                         Property::Map*                          variationsMapPtr)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "-->MultilanguageSupport::ValidateFonts\n");

  if(0u == numberOfCharacters)
  {
    DALI_LOG_INFO(gLogFilter, Debug::General, "<--MultilanguageSupport::ValidateFonts\n");
    // Nothing to do if there are no characters.
    return;
  }

  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_FONTS_VALIDATE");

  // Find the first index where to insert the font run.
  FontRunIndex fontIndex = 0u;
  if(0u != startIndex)
  {
    for(Vector<FontRun>::ConstIterator it    = fonts.Begin(),
                                       endIt = fonts.End();
        it != endIt;
        ++it, ++fontIndex)
    {
      const FontRun& run = *it;
      if(startIndex < run.characterRun.characterIndex + run.characterRun.numberOfCharacters)
      {
        // Run found.
        break;
      }
    }
  }

  // Traverse the characters and validate/set the fonts.

  // Get the caches.
  DefaultFonts**           defaultFontPerScriptCacheBuffer = mDefaultFontPerScriptCache.Begin();
  ValidateFontsPerScript** validFontsPerScriptCacheBuffer  = mValidFontsPerScriptCache.Begin();

  // Stores the validated font runs.
  fonts.Reserve(fontDescriptions.Count());

  // Initializes a validated font run.
  FontRun currentFontRun;
  currentFontRun.characterRun.characterIndex     = startIndex;
  currentFontRun.characterRun.numberOfCharacters = 0u;
  currentFontRun.fontId                          = 0u;
  currentFontRun.isBoldRequired                  = false;
  currentFontRun.isItalicRequired                = false;

  const Character* const textBuffer = text.Begin();

  // Iterators of the script runs.
  Vector<ScriptRun>::ConstIterator scriptRunIt             = scripts.Begin();
  Vector<ScriptRun>::ConstIterator scriptRunEndIt          = scripts.End();
  bool                             isNewParagraphCharacter = false;

  FontId                  currentFontId       = 0u;
  FontId                  previousFontId      = 0u;
  TextAbstraction::Script previousScript      = TextAbstraction::UNKNOWN;

  CharacterIndex lastCharacter = startIndex + numberOfCharacters - 1u;
  for(Length index = startIndex; index <= lastCharacter; ++index)
  {
    // Get the current character.
    const Character character        = *(textBuffer + index);
    bool            isItalicRequired = false;
    bool            isBoldRequired   = false;

    // new description for current character
    TextAbstraction::FontDescription currentFontDescription;
    TextAbstraction::PointSize26Dot6 currentFontPointSize = defaultFontPointSize;
    bool                             isDefaultFont        = true;
    MergeFontDescriptions(fontDescriptions,
                          defaultFontDescription,
                          defaultFontPointSize,
                          fontSizeScale,
                          index,
                          currentFontDescription,
                          currentFontPointSize,
                          isDefaultFont);

    // Get the font for the current character.
    FontId fontId = fontClient.GetFontId(currentFontDescription, currentFontPointSize, 0, variationsMapPtr);

    currentFontId = fontId;

    // Prevent double-bolding from both markup and style
    bool isFontChangedByBold = false;

    if(currentFontDescription.weight == TextAbstraction::FontWeight::BOLD)
    {
      TextAbstraction::FontDescription currentFontDescriptionWithoutBold = TextAbstraction::FontDescription(currentFontDescription);
      currentFontDescriptionWithoutBold.weight = TextAbstraction::FontWeight::NORMAL;

      FontId fontIdWithoutBold = fontClient.GetFontId(currentFontDescriptionWithoutBold, currentFontPointSize);
      if(fontId != fontIdWithoutBold)
      {
        // If a font is already changed by the bold feature, do not let Freetype further embolden it.
        isFontChangedByBold = true;
      }
    }

    // Get the script for the current character.
    Script script = GetScript(index,
                              scriptRunIt,
                              scriptRunEndIt);

#ifdef DEBUG_ENABLED
    if(gLogFilter->IsEnabledFor(Debug::Verbose))
    {
      Dali::TextAbstraction::FontDescription description;
      fontClient.GetDescription(fontId, description);

      DALI_LOG_INFO(gLogFilter,
                    Debug::Verbose,
                    "  Initial font set\n  Character : %x, Script : %s, Font : %s \n",
                    character,
                    Dali::TextAbstraction::ScriptName[script],
                    description.path.c_str());
    }
#endif

    // Validate whether the current character is supported by the given font.
    bool isValidFont = false;

    // Check first in the cache of default fonts per script and size.

    FontId        cachedDefaultFontId = 0u;
    DefaultFonts* defaultFonts        = *(defaultFontPerScriptCacheBuffer + script);
    if(NULL != defaultFonts)
    {
      // This cache stores fall-back fonts.
      cachedDefaultFontId = defaultFonts->FindFont(fontClient,
                                                   currentFontDescription,
                                                   currentFontPointSize,
                                                   character);
    }

    // Whether the cached default font is valid.
    const bool isValidCachedDefaultFont = 0u != cachedDefaultFontId;

    // The font is valid if it matches with the default one for the current script and size and it's different than zero.
    isValidFont = isValidCachedDefaultFont && (fontId == cachedDefaultFontId);

    if(isValidFont)
    {
      // Check if the font supports the character.
      isValidFont = fontClient.IsCharacterSupportedByFont(fontId, character);
    }

    bool isEmojiScript = IsEmojiColorScript(script) || IsEmojiTextScript(script);
    bool isZWJ         = TextAbstraction::IsZeroWidthJoiner(character);

    if((previousScript == script) &&
       (isEmojiScript || isZWJ))
    {
      // This sequence should use the previous font.
      if(0u != previousFontId)
      {
        fontId      = previousFontId;
        isValidFont = true;
      }
    }

    if(TextAbstraction::IsSpace(character) &&
       TextAbstraction::HasLigatureMustBreak(script) &&
       isValidCachedDefaultFont &&
       (isDefaultFont || (currentFontId == previousFontId)))
    {
      fontId      = cachedDefaultFontId;
      isValidFont = true;
    }

    bool findFallbackFont = false;
    if(TextAbstraction::IsEmojiVariationSequences(character) && !TextAbstraction::IsASCIIDigits(character))
    {
      if(index + 1 <= lastCharacter)
      {
        const Character nextCharacter = *(textBuffer + index + 1);
        findFallbackFont = (!TextAbstraction::IsEmojiPresentationSelector(nextCharacter) && !TextAbstraction::IsTextPresentationSelector(nextCharacter) &&
                            !TextAbstraction::IsZeroWidthJoiner(nextCharacter) && !TextAbstraction::IsEmojiModifier(nextCharacter));
      }
      else if(index == lastCharacter)
      {
        findFallbackFont = true;
      }
    }

    // This is valid after CheckFontSupportsCharacter();
    bool isCommonScript = false;

    // If the given font is not valid, it means either:
    // - there is no cached font for the current script yet or,
    // - the user has set a different font than the default one for the current script or,
    // - the platform default font is different than the default font for the current script.

    // Need to check if the given font supports the current character.
    CheckFontSupportsCharacter(isValidFont, isCommonScript, character, validFontsPerScriptCacheBuffer, script, fontId, fontClient,
                               isValidCachedDefaultFont, cachedDefaultFontId, currentFontDescription, currentFontPointSize, defaultFontPerScriptCacheBuffer, findFallbackFont);

    if(isEmojiScript && (previousScript != script))
    {
      //New Emoji sequence should select font according to the variation selector (VS15 or VS16).
      if(0u != currentFontRun.characterRun.numberOfCharacters)
      {
        // Store the font run.
        fonts.Insert(fonts.Begin() + fontIndex, currentFontRun);
        ++fontIndex;
      }

      // Initialize the new one.
      currentFontRun.characterRun.characterIndex     = currentFontRun.characterRun.characterIndex + currentFontRun.characterRun.numberOfCharacters;
      currentFontRun.characterRun.numberOfCharacters = 0u;
      currentFontRun.fontId                          = fontId;
      currentFontRun.isItalicRequired                = false;
      currentFontRun.isBoldRequired                  = false;

      if(TextAbstraction::IsEmojiColorScript(script) || TextAbstraction::IsEmojiTextScript(script))
      {
        bool       isModifiedByVariationSelector = false;
        GlyphIndex glyphIndexChar                = fontClient.GetGlyphIndex(fontId, character);
        GlyphIndex glyphIndexCharByVS            = fontClient.GetGlyphIndex(fontId, character, Text::GetVariationSelectorByScript(script));

        isModifiedByVariationSelector = glyphIndexChar != glyphIndexCharByVS;

        if(isModifiedByVariationSelector)
        {
          FontId requestedFontId = 0u;
          if(TextAbstraction::IsEmojiTextScript(script))
          {
            // Find a fallback-font.
            requestedFontId = fontClient.FindFallbackFont(character, currentFontDescription, currentFontPointSize, false);
            if(fontClient.IsColorGlyph(requestedFontId, glyphIndexChar))
            {
              // Try to find text style glyph.
              requestedFontId = 0;
            }
          }
          else if(TextAbstraction::IsEmojiColorScript(script) && TextAbstraction::IsEmojiPresentationSelector(character))
          {
            if(IsEmojiScript(previousScript) && fontClient.IsColorFont(previousFontId))
            {
              requestedFontId = previousFontId;
            }
            else
            {
              // There are Color emoji fonts that do not have Variation Selector glyphs. Search for fonts using the basic emoji unicode code point.
              requestedFontId = fontClient.FindFallbackFont(UTF32_EMOJI, currentFontDescription, currentFontPointSize, true);
            }
          }
          if(0u == requestedFontId)
          {
            requestedFontId = fontClient.FindDefaultFont(character, currentFontPointSize, IsEmojiColorScript(script));
          }
          if(0u != requestedFontId)
          {
            currentFontRun.fontId = fontId = requestedFontId;
            isValidFont                    = true;
          }
        }
      }
    }

#ifdef DEBUG_ENABLED
    if(gLogFilter->IsEnabledFor(Debug::Verbose))
    {
      Dali::TextAbstraction::FontDescription description;
      fontClient.GetDescription(fontId, description);
      DALI_LOG_INFO(gLogFilter,
                    Debug::Verbose,
                    "  Validated font set\n  Character : %x, Script : %s, Font : %s \n",
                    character,
                    Dali::TextAbstraction::ScriptName[script],
                    description.path.c_str());
    }
#endif
    if(!isValidFont && !isCommonScript)
    {
      Dali::TextAbstraction::FontDescription descriptionForLog;
      fontClient.GetDescription(fontId, descriptionForLog);
      DALI_LOG_RELEASE_INFO("Validated font set fail : Character : %x, Script : %s, Font : %s \n",
                            character,
                            Dali::TextAbstraction::ScriptName[script],
                            descriptionForLog.path.c_str());
    }

    // Whether bols style is required.
    isBoldRequired = (!isFontChangedByBold && (currentFontDescription.weight >= TextAbstraction::FontWeight::BOLD));

    // Whether italic style is required.
    isItalicRequired = (currentFontDescription.slant >= TextAbstraction::FontSlant::ITALIC);

    // The font is now validated.
    if((fontId != currentFontRun.fontId) ||
       isNewParagraphCharacter ||
       // If font id is same as previous but style is diffrent, initialize new one
       ((fontId == currentFontRun.fontId) && ((isBoldRequired != currentFontRun.isBoldRequired) || (isItalicRequired != currentFontRun.isItalicRequired))))
    {
      // Current run needs to be stored and a new one initialized.

      if(0u != currentFontRun.characterRun.numberOfCharacters)
      {
        // Store the font run.
        fonts.Insert(fonts.Begin() + fontIndex, currentFontRun);
        ++fontIndex;
      }

      // Initialize the new one.
      currentFontRun.characterRun.characterIndex     = currentFontRun.characterRun.characterIndex + currentFontRun.characterRun.numberOfCharacters;
      currentFontRun.characterRun.numberOfCharacters = 0u;
      currentFontRun.fontId                          = fontId;
      currentFontRun.isBoldRequired                  = isBoldRequired;
      currentFontRun.isItalicRequired                = isItalicRequired;
    }

    // Add one more character to the run.
    ++currentFontRun.characterRun.numberOfCharacters;

    // Whether the current character is a new paragraph character.
    isNewParagraphCharacter = TextAbstraction::IsNewParagraph(character);
    previousScript          = script;
    currentFontId           = fontId;
    previousFontId          = currentFontId;
  } // end traverse characters.

  if(0u != currentFontRun.characterRun.numberOfCharacters)
  {
    // Store the last run.
    fonts.Insert(fonts.Begin() + fontIndex, currentFontRun);
    ++fontIndex;
  }

  if(fontIndex < fonts.Count())
  {
    // Update the indices of the next font runs.
    const FontRun& run                = *(fonts.Begin() + fontIndex - 1u);
    CharacterIndex nextCharacterIndex = run.characterRun.characterIndex + run.characterRun.numberOfCharacters;

    for(Vector<FontRun>::Iterator it    = fonts.Begin() + fontIndex,
                                  endIt = fonts.End();
        it != endIt;
        ++it)
    {
      FontRun& run = *it;

      run.characterRun.characterIndex = nextCharacterIndex;
      nextCharacterIndex += run.characterRun.numberOfCharacters;
    }
  }

  DALI_LOG_INFO(gLogFilter, Debug::General, "<--MultilanguageSupport::ValidateFonts\n");
}

void MultilanguageSupport::AddCurrentScriptAndCreatNewScript(const Script       requestedScript,
                                                             const bool         isRightToLeft,
                                                             const bool         addScriptCharactersToNewScript,
                                                             ScriptRun&         currentScriptRun,
                                                             Length&            numberOfAllScriptCharacters,
                                                             Vector<ScriptRun>& scripts,
                                                             ScriptRunIndex&    scriptIndex)
{
  // Add the pending characters to the current script
  currentScriptRun.characterRun.numberOfCharacters += (addScriptCharactersToNewScript ? 0u : numberOfAllScriptCharacters);

  // In-case the current script is empty then no need to add it for scripts
  if(0u != currentScriptRun.characterRun.numberOfCharacters)
  {
    // Store the script run.
    scripts.Insert(scripts.Begin() + scriptIndex, currentScriptRun);
    ++scriptIndex;
  }

  // Initialize the new one by the requested script
  currentScriptRun.characterRun.characterIndex     = currentScriptRun.characterRun.characterIndex + currentScriptRun.characterRun.numberOfCharacters;
  currentScriptRun.characterRun.numberOfCharacters = (addScriptCharactersToNewScript ? numberOfAllScriptCharacters : 0u);
  currentScriptRun.script                          = requestedScript;
  numberOfAllScriptCharacters                      = 0u;
  // Initialize whether is right to left direction
  currentScriptRun.isRightToLeft = isRightToLeft;
}

bool MultilanguageSupport::IsICULineBreakNeededForLocale()
{
  return mLocale == THAILAND_LOCALE_CODE;
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali
