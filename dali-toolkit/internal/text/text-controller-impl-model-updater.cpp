/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/text-controller-impl-model-updater.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/bidirectional-support.h>
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/color-segmentation.h>
#include <dali-toolkit/internal/text/hyphenator.h>
#include <dali-toolkit/internal/text/multi-language-support.h>
#include <dali-toolkit/internal/text/segmentation.h>
#include <dali-toolkit/internal/text/shaper.h>
#include <dali-toolkit/internal/text/text-editable-control-interface.h>

namespace Dali::Toolkit::Text
{

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

// The relative luminance of a color is defined as (L = 0.2126 * R + 0.7152 * G + 0.0722 * B)
// based on W3C Recommendations (https://www.w3.org/TR/WCAG20/)
constexpr float         BRIGHTNESS_THRESHOLD = 0.179f;
constexpr float         CONSTANT_R           = 0.2126f;
constexpr float         CONSTANT_G           = 0.7152f;
constexpr float         CONSTANT_B           = 0.0722f;
constexpr Dali::Vector4 BLACK(0.f, 0.f, 0.f, 1.f);
constexpr Dali::Vector4 WHITE(1.f, 1.f, 1.f, 1.f);
constexpr Dali::Vector4 LIGHT_BLUE(0.75f, 0.96f, 1.f, 1.f);
constexpr Dali::Vector4 BACKGROUND_SUB4(0.58f, 0.87f, 0.96f, 1.f);
constexpr Dali::Vector4 BACKGROUND_SUB5(0.83f, 0.94f, 0.98f, 1.f);
constexpr Dali::Vector4 BACKGROUND_SUB6(1.f, 0.5f, 0.5f, 1.f);
constexpr Dali::Vector4 BACKGROUND_SUB7(1.f, 0.8f, 0.8f, 1.f);
} // namespace

bool ControllerImplModelUpdater::Update(Controller::Impl& impl, OperationsMask operationsRequired)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::UpdateModel\n");

  // Calculate the operations to be done.
  const OperationsMask operations = static_cast<OperationsMask>(impl.mOperationsPending & operationsRequired);

  if(Controller::NO_OPERATION == operations)
  {
    // Nothing to do if no operations are pending and required.
    return false;
  }

  Vector<Character>& srcCharacters = impl.mModel->mLogicalModel->mText;
  Vector<Character>  displayCharacters;
  bool               useHiddenText = false;
  if(impl.mHiddenInput && impl.mEventData != nullptr && !impl.mEventData->mIsShowingPlaceholderText)
  {
    impl.mHiddenInput->Substitute(srcCharacters, displayCharacters);
    useHiddenText = true;
  }

  Vector<Character>& utf32Characters    = useHiddenText ? displayCharacters : srcCharacters;
  const Length       numberOfCharacters = utf32Characters.Count();

  // Index to the first character of the first paragraph to be updated.
  CharacterIndex startIndex = 0u;
  // Number of characters of the paragraphs to be removed.
  Length paragraphCharacters = 0u;

  impl.CalculateTextUpdateIndices(paragraphCharacters);

  // Check whether the indices for updating the text is valid
  if(numberOfCharacters > 0u &&
     (impl.mTextUpdateInfo.mParagraphCharacterIndex > numberOfCharacters ||
         impl.mTextUpdateInfo.mRequestedNumberOfCharacters > numberOfCharacters))
  {
    std::string currentText;
    Utf32ToUtf8(impl.mModel->mLogicalModel->mText.Begin(), numberOfCharacters, currentText);

    DALI_LOG_ERROR("Controller::Impl::UpdateModel: mTextUpdateInfo has invalid indices\n");
    DALI_LOG_ERROR("Number of characters: %d, current text is: %s\n", numberOfCharacters, currentText.c_str());

    // Dump mTextUpdateInfo
    DALI_LOG_ERROR("Dump mTextUpdateInfo:\n");
    DALI_LOG_ERROR("     mTextUpdateInfo.mCharacterIndex = %u\n", impl.mTextUpdateInfo.mCharacterIndex);
    DALI_LOG_ERROR("     mTextUpdateInfo.mNumberOfCharactersToRemove = %u\n", impl.mTextUpdateInfo.mNumberOfCharactersToRemove);
    DALI_LOG_ERROR("     mTextUpdateInfo.mNumberOfCharactersToAdd = %u\n", impl.mTextUpdateInfo.mNumberOfCharactersToAdd);
    DALI_LOG_ERROR("     mTextUpdateInfo.mPreviousNumberOfCharacters = %u\n", impl.mTextUpdateInfo.mPreviousNumberOfCharacters);
    DALI_LOG_ERROR("     mTextUpdateInfo.mParagraphCharacterIndex = %u\n", impl.mTextUpdateInfo.mParagraphCharacterIndex);
    DALI_LOG_ERROR("     mTextUpdateInfo.mRequestedNumberOfCharacters = %u\n", impl.mTextUpdateInfo.mRequestedNumberOfCharacters);
    DALI_LOG_ERROR("     mTextUpdateInfo.mStartGlyphIndex = %u\n", impl.mTextUpdateInfo.mStartGlyphIndex);
    DALI_LOG_ERROR("     mTextUpdateInfo.mStartLineIndex = %u\n", impl.mTextUpdateInfo.mStartLineIndex);
    DALI_LOG_ERROR("     mTextUpdateInfo.mEstimatedNumberOfLines = %u\n", impl.mTextUpdateInfo.mEstimatedNumberOfLines);
    DALI_LOG_ERROR("     mTextUpdateInfo.mClearAll = %d\n", impl.mTextUpdateInfo.mClearAll);
    DALI_LOG_ERROR("     mTextUpdateInfo.mFullRelayoutNeeded = %d\n", impl.mTextUpdateInfo.mFullRelayoutNeeded);
    DALI_LOG_ERROR("     mTextUpdateInfo.mIsLastCharacterNewParagraph = %d\n", impl.mTextUpdateInfo.mIsLastCharacterNewParagraph);

    return false;
  }

  startIndex = impl.mTextUpdateInfo.mParagraphCharacterIndex;

  if(impl.mTextUpdateInfo.mClearAll ||
     (0u != paragraphCharacters))
  {
    impl.ClearModelData(startIndex, startIndex + ((paragraphCharacters > 0u) ? paragraphCharacters - 1u : 0u), operations);
  }

  impl.mTextUpdateInfo.mClearAll = false;

  // Whether the model is updated.
  bool updated = false;

  Vector<LineBreakInfo>& lineBreakInfo               = impl.mModel->mLogicalModel->mLineBreakInfo;
  const Length           requestedNumberOfCharacters = impl.mTextUpdateInfo.mRequestedNumberOfCharacters;

  if(Controller::NO_OPERATION != (Controller::GET_LINE_BREAKS & operations))
  {
    // Retrieves the line break info. The line break info is used to split the text in 'paragraphs' to
    // calculate the bidirectional info for each 'paragraph'.
    // It's also used to layout the text (where it should be a new line) or to shape the text (text in different lines
    // is not shaped together).
    lineBreakInfo.Resize(numberOfCharacters, TextAbstraction::LINE_NO_BREAK);

    SetLineBreakInfo(utf32Characters,
                     startIndex,
                     requestedNumberOfCharacters,
                     lineBreakInfo);

    if(impl.mModel->mLineWrapMode == ((Text::LineWrap::Mode)DevelText::LineWrap::HYPHENATION) ||
        impl.mModel->mLineWrapMode == ((Text::LineWrap::Mode)DevelText::LineWrap::MIXED))
    {
      CharacterIndex end                 = startIndex + requestedNumberOfCharacters;
      LineBreakInfo* lineBreakInfoBuffer = lineBreakInfo.Begin();

      for(CharacterIndex index = startIndex; index < end; index++)
      {
        CharacterIndex wordEnd = index;
        while((*(lineBreakInfoBuffer + wordEnd) != TextAbstraction::LINE_ALLOW_BREAK) && (*(lineBreakInfoBuffer + wordEnd) != TextAbstraction::LINE_MUST_BREAK))
        {
          wordEnd++;
        }

        if((wordEnd + 1) == end) // add last char
        {
          wordEnd++;
        }

        Vector<bool> hyphens = GetWordHyphens(utf32Characters.Begin() + index, wordEnd - index, nullptr);

        for(CharacterIndex i = 0; i < (wordEnd - index); i++)
        {
          if(hyphens[i])
          {
            *(lineBreakInfoBuffer + index + i) = TextAbstraction::LINE_HYPHENATION_BREAK;
          }
        }

        index = wordEnd;
      }
    }

    // Create the paragraph info.
    impl.mModel->mLogicalModel->CreateParagraphInfo(startIndex,
                                                    requestedNumberOfCharacters);
    updated = true;
  }

  const bool getScripts    = Controller::NO_OPERATION != (Controller::GET_SCRIPTS & operations);
  const bool validateFonts = Controller::NO_OPERATION != (Controller::VALIDATE_FONTS & operations);

  Vector<ScriptRun>& scripts    = impl.mModel->mLogicalModel->mScriptRuns;
  Vector<FontRun>&   validFonts = impl.mModel->mLogicalModel->mFontRuns;

  if(getScripts || validateFonts)
  {
    // Validates the fonts assigned by the application or assigns default ones.
    // It makes sure all the characters are going to be rendered by the correct font.
    MultilanguageSupport multilanguageSupport = MultilanguageSupport::Get();

    if(getScripts)
    {
      // Retrieves the scripts used in the text.
      multilanguageSupport.SetScripts(utf32Characters,
                                      startIndex,
                                      requestedNumberOfCharacters,
                                      scripts);
    }

    if(validateFonts)
    {
      // Validate the fonts set through the mark-up string.
      Vector<FontDescriptionRun>& fontDescriptionRuns = impl.mModel->mLogicalModel->mFontDescriptionRuns;

      // Get the default font's description.
      TextAbstraction::FontDescription defaultFontDescription;
      TextAbstraction::PointSize26Dot6 defaultPointSize = TextAbstraction::FontClient::DEFAULT_POINT_SIZE * impl.mFontSizeScale;

      //Get the number of points per one unit of point-size
      uint32_t numberOfPointsPerOneUnitOfPointSize = impl.mFontClient.GetNumberOfPointsPerOneUnitOfPointSize();

      if(impl.IsShowingPlaceholderText() && impl.mEventData && (nullptr != impl.mEventData->mPlaceholderFont))
      {
        // If the placeholder font is set specifically, only placeholder font is changed.
        defaultFontDescription = impl.mEventData->mPlaceholderFont->mFontDescription;
        if(impl.mEventData->mPlaceholderFont->sizeDefined)
        {
          defaultPointSize = impl.mEventData->mPlaceholderFont->mDefaultPointSize * impl.mFontSizeScale * numberOfPointsPerOneUnitOfPointSize;
        }
      }
      else if(nullptr != impl.mFontDefaults)
      {
        // Set the normal font and the placeholder font.
        defaultFontDescription = impl.mFontDefaults->mFontDescription;

        if(impl.mTextFitEnabled)
        {
          defaultPointSize = impl.mFontDefaults->mFitPointSize * numberOfPointsPerOneUnitOfPointSize;
        }
        else
        {
          defaultPointSize = impl.mFontDefaults->mDefaultPointSize * impl.mFontSizeScale * numberOfPointsPerOneUnitOfPointSize;
        }
      }

      // Validates the fonts. If there is a character with no assigned font it sets a default one.
      // After this call, fonts are validated.
      multilanguageSupport.ValidateFonts(utf32Characters,
                                         scripts,
                                         fontDescriptionRuns,
                                         defaultFontDescription,
                                         defaultPointSize,
                                         startIndex,
                                         requestedNumberOfCharacters,
                                         validFonts);
    }
    updated = true;
  }

  Vector<Character> mirroredUtf32Characters;
  bool              textMirrored       = false;
  const Length      numberOfParagraphs = impl.mModel->mLogicalModel->mParagraphInfo.Count();
  if(Controller::NO_OPERATION != (Controller::BIDI_INFO & operations))
  {
    Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo = impl.mModel->mLogicalModel->mBidirectionalParagraphInfo;
    bidirectionalInfo.Reserve(numberOfParagraphs);

    // Calculates the bidirectional info for the whole paragraph if it contains right to left scripts.
    SetBidirectionalInfo(utf32Characters,
                         scripts,
                         lineBreakInfo,
                         startIndex,
                         requestedNumberOfCharacters,
                         bidirectionalInfo,
                         (impl.mModel->mMatchLayoutDirection != DevelText::MatchLayoutDirection::CONTENTS),
                         impl.mLayoutDirection);

    if(0u != bidirectionalInfo.Count())
    {
      // Only set the character directions if there is right to left characters.
      Vector<CharacterDirection>& directions = impl.mModel->mLogicalModel->mCharacterDirections;
      GetCharactersDirection(bidirectionalInfo,
                             numberOfCharacters,
                             startIndex,
                             requestedNumberOfCharacters,
                             directions);

      // This paragraph has right to left text. Some characters may need to be mirrored.
      // TODO: consider if the mirrored string can be stored as well.

      textMirrored = GetMirroredText(utf32Characters,
                                     directions,
                                     bidirectionalInfo,
                                     startIndex,
                                     requestedNumberOfCharacters,
                                     mirroredUtf32Characters);
    }
    else
    {
      // There is no right to left characters. Clear the directions vector.
      impl.mModel->mLogicalModel->mCharacterDirections.Clear();
    }
    updated = true;
  }

  Vector<GlyphInfo>&      glyphs                = impl.mModel->mVisualModel->mGlyphs;
  Vector<CharacterIndex>& glyphsToCharactersMap = impl.mModel->mVisualModel->mGlyphsToCharacters;
  Vector<Length>&         charactersPerGlyph    = impl.mModel->mVisualModel->mCharactersPerGlyph;
  Vector<GlyphIndex>      newParagraphGlyphs;
  newParagraphGlyphs.Reserve(numberOfParagraphs);

  const Length currentNumberOfGlyphs = glyphs.Count();
  if(Controller::NO_OPERATION != (Controller::SHAPE_TEXT & operations))
  {
    const Vector<Character>& textToShape = textMirrored ? mirroredUtf32Characters : utf32Characters;
    // Shapes the text.
    ShapeText(textToShape,
              lineBreakInfo,
              scripts,
              validFonts,
              startIndex,
              impl.mTextUpdateInfo.mStartGlyphIndex,
              requestedNumberOfCharacters,
              glyphs,
              glyphsToCharactersMap,
              charactersPerGlyph,
              newParagraphGlyphs);

    // Create the 'number of glyphs' per character and the glyph to character conversion tables.
    impl.mModel->mVisualModel->CreateGlyphsPerCharacterTable(startIndex, impl.mTextUpdateInfo.mStartGlyphIndex, requestedNumberOfCharacters);
    impl.mModel->mVisualModel->CreateCharacterToGlyphTable(startIndex, impl.mTextUpdateInfo.mStartGlyphIndex, requestedNumberOfCharacters);

    updated = true;
  }

  const Length numberOfGlyphs = glyphs.Count() - currentNumberOfGlyphs;

  if(Controller::NO_OPERATION != (Controller::GET_GLYPH_METRICS & operations))
  {
    GlyphInfo* glyphsBuffer = glyphs.Begin();
    impl.mMetrics->GetGlyphMetrics(glyphsBuffer + impl.mTextUpdateInfo.mStartGlyphIndex, numberOfGlyphs);

    // Update the width and advance of all new paragraph characters.
    for(Vector<GlyphIndex>::ConstIterator it = newParagraphGlyphs.Begin(), endIt = newParagraphGlyphs.End(); it != endIt; ++it)
    {
      const GlyphIndex index = *it;
      GlyphInfo&       glyph = *(glyphsBuffer + index);

      glyph.xBearing = 0.f;
      glyph.width    = 0.f;
      glyph.advance  = 0.f;
    }
    updated = true;
  }

  if((nullptr != impl.mEventData) &&
      impl.mEventData->mPreEditFlag &&
     (0u != impl.mModel->mVisualModel->mCharactersToGlyph.Count()))
  {
    Dali::InputMethodContext::PreEditAttributeDataContainer attrs;
    impl.mEventData->mInputMethodContext.GetPreeditStyle(attrs);
    Dali::InputMethodContext::PreeditStyle type = Dali::InputMethodContext::PreeditStyle::NONE;

    // Check the type of preedit and run it.
    for(Dali::InputMethodContext::PreEditAttributeDataContainer::Iterator it = attrs.Begin(), endIt = attrs.End(); it != endIt; it++)
    {
      Dali::InputMethodContext::PreeditAttributeData attrData = *it;
      DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::UpdateModel PreeditStyle type : %d  start %d end %d \n", attrData.preeditType, attrData.startIndex, attrData.endIndex);
      type = attrData.preeditType;

      // Check the number of commit characters for the start position.
      unsigned int numberOfCommit  = impl.mEventData->mPrimaryCursorPosition - impl.mEventData->mPreEditLength;
      Length       numberOfIndices = attrData.endIndex - attrData.startIndex;

      switch(type)
      {
        case Dali::InputMethodContext::PreeditStyle::UNDERLINE:
        {
          // Add the underline for the pre-edit text.
          GlyphRun underlineRun;
          underlineRun.glyphIndex     = attrData.startIndex + numberOfCommit;
          underlineRun.numberOfGlyphs = numberOfIndices;
          impl.mModel->mVisualModel->mUnderlineRuns.PushBack(underlineRun);

          //Mark-up processor case
          if(impl.mModel->mVisualModel->IsMarkupProcessorEnabled())
          {
            impl.CopyUnderlinedFromLogicalToVisualModels(false);
          }
          break;
        }
        case Dali::InputMethodContext::PreeditStyle::REVERSE:
        {
          Vector4  textColor = impl.mModel->mVisualModel->GetTextColor();
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex     = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color                           = textColor;
          impl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack(backgroundColorRun);

          Vector4 backgroundColor = impl.mModel->mVisualModel->GetBackgroundColor();
          if(backgroundColor.a == 0) // There is no text background color.
          {
            // Try use the control's background color.
            if(nullptr != impl.mEditableControlInterface)
            {
              impl.mEditableControlInterface->GetControlBackgroundColor(backgroundColor);
              if(backgroundColor.a == 0) // There is no control background color.
              {
                // Determines black or white color according to text color.
                // Based on W3C Recommendations (https://www.w3.org/TR/WCAG20/)
                float L         = CONSTANT_R * textColor.r + CONSTANT_G * textColor.g + CONSTANT_B * textColor.b;
                backgroundColor = L > BRIGHTNESS_THRESHOLD ? BLACK : WHITE;
              }
            }
          }

          Vector<ColorRun> colorRuns;
          colorRuns.Resize(1u);
          ColorRun& colorRun                       = *(colorRuns.Begin());
          colorRun.color                           = backgroundColor;
          colorRun.characterRun.characterIndex     = attrData.startIndex + numberOfCommit;
          colorRun.characterRun.numberOfCharacters = numberOfIndices;
          impl.mModel->mLogicalModel->mColorRuns.PushBack(colorRun);

          //Mark-up processor case
          if(impl.mModel->mVisualModel->IsMarkupProcessorEnabled())
          {
            impl.CopyUnderlinedFromLogicalToVisualModels(false);
          }
          break;
        }
        case Dali::InputMethodContext::PreeditStyle::HIGHLIGHT:
        {
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex     = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color                           = LIGHT_BLUE;
          impl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack(backgroundColorRun);

          //Mark-up processor case
          if(impl.mModel->mVisualModel->IsMarkupProcessorEnabled())
          {
            impl.CopyUnderlinedFromLogicalToVisualModels(false);
          }
          break;
        }
        case Dali::InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_1:
        {
          // CUSTOM_PLATFORM_STYLE_1 should be drawn with background and underline together.
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex     = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color                           = BACKGROUND_SUB4;
          impl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack(backgroundColorRun);

          GlyphRun underlineRun;
          underlineRun.glyphIndex     = attrData.startIndex + numberOfCommit;
          underlineRun.numberOfGlyphs = numberOfIndices;
          impl.mModel->mVisualModel->mUnderlineRuns.PushBack(underlineRun);

          //Mark-up processor case
          if(impl.mModel->mVisualModel->IsMarkupProcessorEnabled())
          {
            impl.CopyUnderlinedFromLogicalToVisualModels(false);
          }
          break;
        }
        case Dali::InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_2:
        {
          // CUSTOM_PLATFORM_STYLE_2 should be drawn with background and underline together.
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex     = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color                           = BACKGROUND_SUB5;
          impl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack(backgroundColorRun);

          GlyphRun underlineRun;
          underlineRun.glyphIndex     = attrData.startIndex + numberOfCommit;
          underlineRun.numberOfGlyphs = numberOfIndices;
          impl.mModel->mVisualModel->mUnderlineRuns.PushBack(underlineRun);

          //Mark-up processor case
          if(impl.mModel->mVisualModel->IsMarkupProcessorEnabled())
          {
            impl.CopyUnderlinedFromLogicalToVisualModels(false);
          }
          break;
        }
        case Dali::InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_3:
        {
          // CUSTOM_PLATFORM_STYLE_3 should be drawn with background and underline together.
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex     = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color                           = BACKGROUND_SUB6;
          impl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack(backgroundColorRun);

          GlyphRun underlineRun;
          underlineRun.glyphIndex     = attrData.startIndex + numberOfCommit;
          underlineRun.numberOfGlyphs = numberOfIndices;
          impl.mModel->mVisualModel->mUnderlineRuns.PushBack(underlineRun);

          //Mark-up processor case
          if(impl.mModel->mVisualModel->IsMarkupProcessorEnabled())
          {
            impl.CopyUnderlinedFromLogicalToVisualModels(false);
          }
          break;
        }
        case Dali::InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_4:
        {
          // CUSTOM_PLATFORM_STYLE_4 should be drawn with background and underline together.
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex     = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color                           = BACKGROUND_SUB7;
          impl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack(backgroundColorRun);

          GlyphRun underlineRun;
          underlineRun.glyphIndex     = attrData.startIndex + numberOfCommit;
          underlineRun.numberOfGlyphs = numberOfIndices;
          impl.mModel->mVisualModel->mUnderlineRuns.PushBack(underlineRun);

          //Mark-up processor case
          if(impl.mModel->mVisualModel->IsMarkupProcessorEnabled())
          {
            impl.CopyUnderlinedFromLogicalToVisualModels(false);
          }
          break;
        }
        case Dali::InputMethodContext::PreeditStyle::NONE:
        default:
        {
          break;
        }
      }
    }
    attrs.Clear();
    updated = true;
  }

  if(Controller::NO_OPERATION != (Controller::COLOR & operations))
  {
    // Set the color runs in glyphs.
    SetColorSegmentationInfo(impl.mModel->mLogicalModel->mColorRuns,
                             impl.mModel->mVisualModel->mCharactersToGlyph,
                             impl.mModel->mVisualModel->mGlyphsPerCharacter,
                             startIndex,
                             impl.mTextUpdateInfo.mStartGlyphIndex,
                             requestedNumberOfCharacters,
                             impl.mModel->mVisualModel->mColors,
                             impl.mModel->mVisualModel->mColorIndices);

    // Set the background color runs in glyphs.
    SetColorSegmentationInfo(impl.mModel->mLogicalModel->mBackgroundColorRuns,
                             impl.mModel->mVisualModel->mCharactersToGlyph,
                             impl.mModel->mVisualModel->mGlyphsPerCharacter,
                             startIndex,
                             impl.mTextUpdateInfo.mStartGlyphIndex,
                             requestedNumberOfCharacters,
                             impl.mModel->mVisualModel->mBackgroundColors,
                             impl.mModel->mVisualModel->mBackgroundColorIndices);

    updated = true;
  }

  if((Controller::NO_OPERATION != (Controller::SHAPE_TEXT & operations)) &&
     !((nullptr != impl.mEventData) &&
         impl.mEventData->mPreEditFlag &&
       (0u != impl.mModel->mVisualModel->mCharactersToGlyph.Count())))
  {
    //Mark-up processor case
    if(impl.mModel->mVisualModel->IsMarkupProcessorEnabled())
    {
      impl.CopyUnderlinedFromLogicalToVisualModels(true);
    }

    updated = true;
  }

  // The estimated number of lines. Used to avoid reallocations when layouting.
  impl.mTextUpdateInfo.mEstimatedNumberOfLines = std::max(impl.mModel->mVisualModel->mLines.Count(), impl.mModel->mLogicalModel->mParagraphInfo.Count());

  // Set the previous number of characters for the next time the text is updated.
  impl.mTextUpdateInfo.mPreviousNumberOfCharacters = numberOfCharacters;

  return updated;
}

} // namespace Dali::Toolkit::Text
