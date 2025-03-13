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

// CLASS  HEADER
#include <dali-toolkit/internal/text/async-text/async-text-loader-impl.h>

// EXTERNAL INCLUDES
#include <cmath>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/integration-api/trace.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/bidirectional-support.h>
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/color-segmentation.h>
#include <dali-toolkit/internal/text/hyphenator.h>
#include <dali-toolkit/internal/text/markup-processor/markup-processor.h>
#include <dali-toolkit/internal/text/segmentation.h>
#include <dali-toolkit/internal/text/shaper.h>

namespace Dali
{
namespace Toolkit
{
namespace
{
constexpr float MAX_FLOAT = std::numeric_limits<float>::max();

const float VERTICAL_ALIGNMENT_TABLE[Text::VerticalAlignment::BOTTOM + 1] =
{
    0.0f, // VerticalAlignment::TOP
    0.5f, // VerticalAlignment::CENTER
    1.0f  // VerticalAlignment::BOTTOM
};

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_TEXT_ASYNC, false);
} // namespace

namespace Text
{
namespace Internal
{
const float TO_POINT_26_DOT_6 = 64.f;

AsyncTextLoader::AsyncTextLoader()
: mModule(),
  mTextModel(),
  mMetrics(),
  mNumberOfCharacters(0u),
  mFitActualEllipsis(true),
  mIsTextDirectionRTL(false),
  mIsTextMirrored(false),
  mModuleClearNeeded(false),
  mMutex()
{
  mModule = Dali::Toolkit::Text::AsyncTextModule::New();

  mTextModel = Model::New();

  mTypesetter = Text::Typesetter::New(mTextModel.Get());

  // Use this to access FontClient i.e. to get down-scaled Emoji metrics.
  mMetrics = Metrics::New(mModule.GetFontClient());
  mLayoutEngine.SetMetrics(mMetrics);
}

AsyncTextLoader::~AsyncTextLoader()
{
}

void AsyncTextLoader::ClearModule()
{
  mModule.ClearCache();
}

void AsyncTextLoader::SetCustomFontDirectories(const TextAbstraction::FontPathList& customFontDirectories)
{
  for(auto &path: customFontDirectories)
  {
    mModule.GetFontClient().AddCustomFontDirectory(path);
  }
}

void AsyncTextLoader::SetModuleClearNeeded(bool clear)
{
  Dali::Mutex::ScopedLock lock(mMutex);
  mModuleClearNeeded = clear;
}

bool AsyncTextLoader::IsModuleClearNeeded()
{
  return mModuleClearNeeded;
}

// Worker thread
void AsyncTextLoader::Initialize()
{
  mModule.GetFontClient().InitDefaultFontDescription();

  ClearTextModelData();

  mNumberOfCharacters = 0u;
  mIsTextDirectionRTL = false;
  mIsTextMirrored = false;

  // Set the text properties to default
  mTextModel->mVisualModel->SetUnderlineEnabled(false);
  mTextModel->mVisualModel->SetUnderlineHeight(0.0f);
  mTextModel->mVisualModel->SetOutlineWidth(0.0f);
  mTextModel->mVisualModel->SetShadowOffset(Vector2(0.0f, 0.0f));
  mTextModel->mVisualModel->SetStrikethroughEnabled(false);
  mTextModel->mVisualModel->SetStrikethroughHeight(0.0f);
}

void AsyncTextLoader::ClearTextModelData()
{
  mTextModel->mLogicalModel->mText.Clear();
  mTextModel->mLogicalModel->mScriptRuns.Clear();
  mTextModel->mLogicalModel->mFontRuns.Clear();
  mTextModel->mLogicalModel->mColorRuns.Clear();
  mTextModel->mLogicalModel->mBackgroundColorRuns.Clear();
  mTextModel->mLogicalModel->mLineBreakInfo.Clear();
  mTextModel->mLogicalModel->mParagraphInfo.Clear();
  mTextModel->mLogicalModel->ClearBidirectionalParagraphInfo(mModule.GetBidirectionalSupport());
  mTextModel->mLogicalModel->mBidirectionalParagraphInfo.Clear();
  mTextModel->mLogicalModel->mCharacterDirections.Clear();
  mTextModel->mLogicalModel->mCharacterSpacingCharacterRuns.Clear();

  mTextModel->mLogicalModel->ClearFontDescriptionRuns();
  mTextModel->mLogicalModel->ClearStrikethroughRuns();
  mTextModel->mLogicalModel->ClearUnderlineRuns();
  mTextModel->mLogicalModel->ClearEmbeddedImages();
  mTextModel->mLogicalModel->ClearAnchors();

  // Free the allocated memory used to store the conversion table in the bidirectional line info run.
  for(Vector<BidirectionalLineInfoRun>::Iterator it    = mTextModel->mLogicalModel->mBidirectionalLineInfo.Begin(),
                                                 endIt = mTextModel->mLogicalModel->mBidirectionalLineInfo.End();
      it != endIt;
      ++it)
  {
    BidirectionalLineInfoRun& bidiLineInfo = *it;

    free(bidiLineInfo.visualToLogicalMap);
    bidiLineInfo.visualToLogicalMap = NULL;

    free(bidiLineInfo.visualToLogicalMapSecondHalf);
    bidiLineInfo.visualToLogicalMapSecondHalf = NULL;
  }
  mTextModel->mLogicalModel->mBidirectionalLineInfo.Clear();

  mTextModel->mVisualModel->ClearCaches();
  mTextModel->mVisualModel->mGlyphs.Clear();
  mTextModel->mVisualModel->mGlyphsToCharacters.Clear();
  mTextModel->mVisualModel->mCharactersToGlyph.Clear();
  mTextModel->mVisualModel->mCharactersPerGlyph.Clear();
  mTextModel->mVisualModel->mGlyphsPerCharacter.Clear();
  mTextModel->mVisualModel->mGlyphPositions.Clear();
  mTextModel->mVisualModel->mLines.Clear();
  mTextModel->mVisualModel->mColorIndices.Clear();
  mTextModel->mVisualModel->mBackgroundColorIndices.Clear();
}

void AsyncTextLoader::Update(AsyncTextParameters& parameters)
{
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_ASYNC_UPDATE");

  if(parameters.text.empty())
  {
    DALI_LOG_ERROR("Text is empty\n");
    return;
  }

  const uint8_t* utf8     = nullptr; // pointer to the first character of the text (encoded in utf8)
  Length         textSize = 0u;      // The length of the utf8 string.

  Length&           numberOfCharacters = mNumberOfCharacters;
  Vector<Character> mirroredUtf32Characters;

  Vector<Character>&                     utf32Characters     = mTextModel->mLogicalModel->mText;                       // Characters encoded in utf32.
  Vector<LineBreakInfo>&                 lineBreakInfo       = mTextModel->mLogicalModel->mLineBreakInfo;              // The line break info.
  Vector<ScriptRun>&                     scripts             = mTextModel->mLogicalModel->mScriptRuns;                 // Charactes's script.
  Vector<FontDescriptionRun>&            fontDescriptionRuns = mTextModel->mLogicalModel->mFontDescriptionRuns;        // Desired font descriptions.
  Vector<FontRun>&                       validFonts          = mTextModel->mLogicalModel->mFontRuns;                   // Validated fonts.
  Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo   = mTextModel->mLogicalModel->mBidirectionalParagraphInfo; // The bidirectional info per paragraph.
  Vector<ColorRun>&                      colorRuns           = mTextModel->mLogicalModel->mColorRuns;                  // colors of the text.


  // Set the default font's description with the given text parameters.
  TextAbstraction::FontDescription defaultFontDescription;
  defaultFontDescription.family = parameters.fontFamily;
  defaultFontDescription.weight = parameters.fontWeight;
  defaultFontDescription.width = parameters.fontWidth;
  defaultFontDescription.slant = parameters.fontSlant;

  mTextModel->mHorizontalAlignment   = parameters.horizontalAlignment;
  mTextModel->mVerticalAlignment     = parameters.verticalAlignment;
  mTextModel->mVerticalLineAlignment = parameters.verticalLineAlignment;


  ////////////////////////////////////////////////////////////////////////////////
  // Update visual model.
  ////////////////////////////////////////////////////////////////////////////////

  // Store the size used to layout the text.
  // control size is used in ElideGlyphs in ViewModel.
  mTextModel->mVisualModel->mControlSize = Size(parameters.textWidth, parameters.textHeight);

  // Update style properties.
  mTextModel->mVisualModel->SetTextColor(parameters.textColor);

  if(parameters.isUnderlineEnabled)
  {
    mTextModel->mVisualModel->SetUnderlineEnabled(parameters.isUnderlineEnabled);
    mTextModel->mVisualModel->SetUnderlineType(parameters.underlineType);
    mTextModel->mVisualModel->SetUnderlineColor(parameters.underlineColor);
    mTextModel->mVisualModel->SetUnderlineHeight(parameters.underlineHeight);
    mTextModel->mVisualModel->SetDashedUnderlineWidth(parameters.dashedUnderlineWidth);
    mTextModel->mVisualModel->SetDashedUnderlineGap(parameters.dashedUnderlineGap);
  }

  if(parameters.isStrikethroughEnabled)
  {
    mTextModel->mVisualModel->SetStrikethroughEnabled(parameters.isStrikethroughEnabled);
    mTextModel->mVisualModel->SetStrikethroughColor(parameters.strikethroughColor);
    mTextModel->mVisualModel->SetStrikethroughHeight(parameters.strikethroughHeight);
  }

  const Vector2& shadowOffset = parameters.shadowOffset;
  const float    shadowAlpha  = parameters.shadowColor.a;
  if(fabsf(shadowAlpha) > Math::MACHINE_EPSILON_1 && (fabsf(shadowOffset.x) > Math::MACHINE_EPSILON_1 || fabsf(shadowOffset.y) > Math::MACHINE_EPSILON_1))
  {
    mTextModel->mVisualModel->SetShadowBlurRadius(parameters.shadowBlurRadius);
    mTextModel->mVisualModel->SetShadowColor(parameters.shadowColor);
    mTextModel->mVisualModel->SetShadowOffset(parameters.shadowOffset);
  }

  const uint16_t outlineWidth = parameters.outlineWidth;
  const float    outlineAlpha = parameters.outlineColor.a;
  if(outlineWidth != 0u && fabsf(outlineAlpha) > Math::MACHINE_EPSILON_1)
  {
    mTextModel->mVisualModel->SetOutlineColor(parameters.outlineColor);
    mTextModel->mVisualModel->SetOutlineWidth(parameters.outlineWidth);
    mTextModel->mVisualModel->SetOutlineBlurRadius(parameters.outlineBlurRadius);
    mTextModel->mVisualModel->SetOutlineOffset(parameters.outlineOffset);
  }

  mTextModel->mVisualModel->SetCutoutEnabled(parameters.cutout);
  mTextModel->mVisualModel->SetBackgroundWithCutoutEnabled(parameters.backgroundWithCutoutEnabled);
  mTextModel->mVisualModel->SetBackgroundColorWithCutout(parameters.backgroundColorWithCutout);

  mTextModel->mRemoveFrontInset = parameters.removeFrontInset;
  mTextModel->mRemoveBackInset  = parameters.removeBackInset;


  ////////////////////////////////////////////////////////////////////////////////
  // Process the markup string if the mark-up processor is enabled.
  ////////////////////////////////////////////////////////////////////////////////

  MarkupProcessData markupProcessData(colorRuns,
                                      fontDescriptionRuns,
                                      mTextModel->mLogicalModel->mEmbeddedItems,
                                      mTextModel->mLogicalModel->mAnchors,
                                      mTextModel->mLogicalModel->mUnderlinedCharacterRuns,
                                      mTextModel->mLogicalModel->mBackgroundColorRuns,
                                      mTextModel->mLogicalModel->mStrikethroughCharacterRuns,
                                      mTextModel->mLogicalModel->mBoundedParagraphRuns,
                                      mTextModel->mLogicalModel->mCharacterSpacingCharacterRuns);

  mTextModel->mVisualModel->SetMarkupProcessorEnabled(parameters.enableMarkup);

  if(parameters.enableMarkup)
  {
    // TODO : Currently unable to support anchor clicked events.
    MarkupPropertyData markupPropertyData(Color::MEDIUM_BLUE, Color::DARK_MAGENTA);

    ProcessMarkupString(parameters.text, markupPropertyData, markupProcessData);
    textSize = markupProcessData.markupProcessedText.size();

    // This is a bit horrible but std::string returns a (signed) char*
    utf8 = reinterpret_cast<const uint8_t*>(markupProcessData.markupProcessedText.c_str());
  }
  else
  {
    textSize = parameters.text.size();

    // This is a bit horrible but std::string returns a (signed) char*
    utf8 = reinterpret_cast<const uint8_t*>(parameters.text.c_str());
  }


  ////////////////////////////////////////////////////////////////////////////////
  // Convert from utf8 to utf32
  ////////////////////////////////////////////////////////////////////////////////

  utf32Characters.Resize(textSize);

  // Transform a text array encoded in utf8 into an array encoded in utf32.
  // It returns the actual number of characters.
  numberOfCharacters = Utf8ToUtf32(utf8, textSize, utf32Characters.Begin());
  utf32Characters.Resize(numberOfCharacters);


  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the Line and Word Break Info.
  ////////////////////////////////////////////////////////////////////////////////

  lineBreakInfo.Resize(numberOfCharacters, TextAbstraction::LINE_NO_BREAK);
  SetLineBreakInfo(mModule.GetSegmentation(), utf32Characters, 0u, numberOfCharacters, lineBreakInfo);

  // Hyphenation
  if(parameters.lineWrapMode == ((Text::LineWrap::Mode)DevelText::LineWrap::HYPHENATION) ||
     parameters.lineWrapMode == ((Text::LineWrap::Mode)DevelText::LineWrap::MIXED))
  {
    CharacterIndex startIndex          = 0u;
    CharacterIndex end                 = numberOfCharacters;
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

      Vector<bool> hyphens = GetWordHyphens(mModule.GetHyphenation(), utf32Characters.Begin() + index, wordEnd - index, nullptr);

      for(CharacterIndex i = 0; i < (wordEnd - index) && i < hyphens.Size(); i++)
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
  mTextModel->mLogicalModel->CreateParagraphInfo(0u, numberOfCharacters);


  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the script runs.
  ////////////////////////////////////////////////////////////////////////////////

  mModule.GetMultilanguageSupport().SetScripts(utf32Characters, 0u, numberOfCharacters, scripts);


  ////////////////////////////////////////////////////////////////////////////////
  // Validate Fonts.
  ////////////////////////////////////////////////////////////////////////////////


  float scale = parameters.fontSizeScale;

  TextAbstraction::PointSize26Dot6 defaultPointSize = TextAbstraction::FontClient::DEFAULT_POINT_SIZE * scale;

  //Get the number of points per one unit of point-size
  uint32_t numberOfPointsPerOneUnitOfPointSize = mModule.GetFontClient().GetNumberOfPointsPerOneUnitOfPointSize();

  defaultPointSize = parameters.fontSize * scale * numberOfPointsPerOneUnitOfPointSize;

  // Validates the fonts. If there is a character with no assigned font it sets a default one.
  // After this call, fonts are validated.
  mModule.GetMultilanguageSupport().ValidateFonts(mModule.GetFontClient(),
                                                  utf32Characters,
                                                  scripts,
                                                  fontDescriptionRuns,
                                                  defaultFontDescription,
                                                  defaultPointSize,
                                                  scale,
                                                  0u,
                                                  numberOfCharacters,
                                                  validFonts);


  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the Bidirectional info.
  ////////////////////////////////////////////////////////////////////////////////

  // Update the layout direction policy to text model.
  mTextModel->mMatchLayoutDirection = parameters.layoutDirectionPolicy;

  mIsTextMirrored = false;
  const Length numberOfParagraphs = mTextModel->mLogicalModel->mParagraphInfo.Count();

  bidirectionalInfo.Reserve(numberOfParagraphs);

  // Calculates the bidirectional info for the whole paragraph if it contains right to left scripts.
  SetBidirectionalInfo(mModule.GetBidirectionalSupport(),
                       utf32Characters,
                       scripts,
                       lineBreakInfo,
                       0u,
                       numberOfCharacters,
                       bidirectionalInfo,
                       mTextModel->mLogicalModel->mBidirectionalLineInfo,
                       (mTextModel->mMatchLayoutDirection != DevelText::MatchLayoutDirection::CONTENTS),
                       parameters.layoutDirection);

  if(0u != bidirectionalInfo.Count())
  {
    // Only set the character directions if there is right to left characters.
    Vector<CharacterDirection>& directions = mTextModel->mLogicalModel->mCharacterDirections;
    GetCharactersDirection(mModule.GetBidirectionalSupport(),
                           bidirectionalInfo,
                           numberOfCharacters,
                           0u,
                           numberOfCharacters,
                           directions);

    // This paragraph has right to left text. Some characters may need to be mirrored.
    // TODO: consider if the mirrored string can be stored as well.

    mIsTextMirrored = GetMirroredText(mModule.GetBidirectionalSupport(),
                                      utf32Characters,
                                      directions,
                                      bidirectionalInfo,
                                      0u,
                                      numberOfCharacters,
                                      mirroredUtf32Characters);
  }
  else
  {
    // There is no right to left characters. Clear the directions vector.
    mTextModel->mLogicalModel->mCharacterDirections.Clear();
  }


  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the glyphs. Text shaping
  ////////////////////////////////////////////////////////////////////////////////

  Vector<GlyphInfo>&      glyphs                = mTextModel->mVisualModel->mGlyphs;
  Vector<CharacterIndex>& glyphsToCharactersMap = mTextModel->mVisualModel->mGlyphsToCharacters;
  Vector<Length>&         charactersPerGlyph    = mTextModel->mVisualModel->mCharactersPerGlyph;
  Vector<GlyphIndex>      newParagraphGlyphs;
  newParagraphGlyphs.Reserve(numberOfParagraphs);

  const Length currentNumberOfGlyphs = glyphs.Count();

  const Vector<Character>& textToShape = mIsTextMirrored ? mirroredUtf32Characters : utf32Characters;

  // Shapes the text.
  ShapeText(mModule.GetShaping(),
            mModule.GetFontClient(),
            textToShape,
            lineBreakInfo,
            scripts,
            validFonts,
            0u,
            0u,
            numberOfCharacters,
            glyphs,
            glyphsToCharactersMap,
            charactersPerGlyph,
            newParagraphGlyphs);

  // Create the 'number of glyphs' per character and the glyph to character conversion tables.
  mTextModel->mVisualModel->CreateGlyphsPerCharacterTable(0u, 0u, numberOfCharacters);
  mTextModel->mVisualModel->CreateCharacterToGlyphTable(0u, 0u, numberOfCharacters);


  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the glyph's metrics.
  ////////////////////////////////////////////////////////////////////////////////

  const Length numberOfGlyphs = static_cast<Length>(glyphs.Count()) - currentNumberOfGlyphs;

  mMetrics->GetGlyphMetrics(glyphs.Begin(), numberOfGlyphs);

  GlyphInfo* glyphsBuffer = glyphs.Begin();

  // Update the width and advance of all new paragraph characters.
  for(Vector<GlyphIndex>::ConstIterator it = newParagraphGlyphs.Begin(), endIt = newParagraphGlyphs.End(); it != endIt; ++it)
  {
    const GlyphIndex index = *it;
    GlyphInfo&       glyph = *(glyphsBuffer + index);

    glyph.xBearing = 0.f;
    glyph.width    = 0.f;
    glyph.advance  = 0.f;
  }

  ////////////////////////////////////////////////////////////////////////////////
  // Set the color runs in glyphs.
  ////////////////////////////////////////////////////////////////////////////////

  // Set the color runs in glyphs.
  SetColorSegmentationInfo(mTextModel->mLogicalModel->mColorRuns,
                           mTextModel->mVisualModel->mCharactersToGlyph,
                           mTextModel->mVisualModel->mGlyphsPerCharacter,
                           0u,
                           0u,
                           numberOfCharacters,
                           mTextModel->mVisualModel->mColors,
                           mTextModel->mVisualModel->mColorIndices);

  // Set the background color runs in glyphs.
  SetColorSegmentationInfo(mTextModel->mLogicalModel->mBackgroundColorRuns,
                           mTextModel->mVisualModel->mCharactersToGlyph,
                           mTextModel->mVisualModel->mGlyphsPerCharacter,
                           0u,
                           0u,
                           numberOfCharacters,
                           mTextModel->mVisualModel->mBackgroundColors,
                           mTextModel->mVisualModel->mBackgroundColorIndices);


  ////////////////////////////////////////////////////////////////////////////////
  // Update visual model for markup style.
  ////////////////////////////////////////////////////////////////////////////////

  if(mTextModel->mVisualModel->IsMarkupProcessorEnabled())
  {
    const Vector<UnderlinedCharacterRun>&       underlinedCharacterRuns       = mTextModel->mLogicalModel->mUnderlinedCharacterRuns;
    const Vector<StrikethroughCharacterRun>&    strikethroughCharacterRuns    = mTextModel->mLogicalModel->mStrikethroughCharacterRuns;
    const Vector<CharacterSpacingCharacterRun>& characterSpacingCharacterRuns = mTextModel->mLogicalModel->mCharacterSpacingCharacterRuns;
    const Vector<GlyphIndex>&                   charactersToGlyph             = mTextModel->mVisualModel->mCharactersToGlyph;
    const Vector<Length>&                       glyphsPerCharacter            = mTextModel->mVisualModel->mGlyphsPerCharacter;


    ////////////////////////////////////////////////////////////////////////////////
    // Markup underline
    ////////////////////////////////////////////////////////////////////////////////

    // Should clear previous underline runs.
    mTextModel->mVisualModel->mUnderlineRuns.Clear();

    for(Vector<UnderlinedCharacterRun>::ConstIterator it = underlinedCharacterRuns.Begin(), endIt = underlinedCharacterRuns.End(); it != endIt; ++it)
    {
      CharacterIndex characterIndex     = it->characterRun.characterIndex;
      Length         numberOfCharacters = it->characterRun.numberOfCharacters;

      if(numberOfCharacters == 0)
      {
        continue;
      }

      // Create one run for all glyphs of all run's characters that has same properties
      // This enhance performance and reduce the needed memory to store glyphs-runs
      UnderlinedGlyphRun underlineGlyphRun;
      underlineGlyphRun.properties = it->properties;
      underlineGlyphRun.glyphRun.glyphIndex     = charactersToGlyph[characterIndex];
      underlineGlyphRun.glyphRun.numberOfGlyphs = glyphsPerCharacter[characterIndex];

      for(Length index = 1u; index < numberOfCharacters; index++)
      {
        underlineGlyphRun.glyphRun.numberOfGlyphs += glyphsPerCharacter[characterIndex + index];
      }

      mTextModel->mVisualModel->mUnderlineRuns.PushBack(underlineGlyphRun);
    }

    // Reset flag. The updates have been applied from logical to visual.
    mTextModel->mLogicalModel->mUnderlineRunsUpdated = false;


    ////////////////////////////////////////////////////////////////////////////////
    // Markup strikethrough
    ////////////////////////////////////////////////////////////////////////////////

    // Should clear previous strikethrough runs.
    mTextModel->mVisualModel->mStrikethroughRuns.Clear();

    for(Vector<StrikethroughCharacterRun>::ConstIterator it = strikethroughCharacterRuns.Begin(), endIt = strikethroughCharacterRuns.End(); it != endIt; ++it)
    {
      CharacterIndex characterIndex     = it->characterRun.characterIndex;
      Length         numberOfCharacters = it->characterRun.numberOfCharacters;

      if(numberOfCharacters == 0)
      {
        continue;
      }

      StrikethroughGlyphRun strikethroughGlyphRun;
      strikethroughGlyphRun.properties              = it->properties;
      strikethroughGlyphRun.glyphRun.glyphIndex     = charactersToGlyph[characterIndex];
      strikethroughGlyphRun.glyphRun.numberOfGlyphs = glyphsPerCharacter[characterIndex];

      for(Length index = 1u; index < numberOfCharacters; index++)
      {
        strikethroughGlyphRun.glyphRun.numberOfGlyphs += glyphsPerCharacter[characterIndex + index];
      }

      mTextModel->mVisualModel->mStrikethroughRuns.PushBack(strikethroughGlyphRun);
    }

    // Reset flag. The updates have been applied from logical to visual.
    mTextModel->mLogicalModel->mStrikethroughRunsUpdated = false;


    ////////////////////////////////////////////////////////////////////////////////
    // Markup character spacing
    ////////////////////////////////////////////////////////////////////////////////

    // Should clear previous character spacing runs.
    mTextModel->mVisualModel->mCharacterSpacingRuns.Clear();

    for(Vector<CharacterSpacingCharacterRun>::ConstIterator it = characterSpacingCharacterRuns.Begin(), endIt = characterSpacingCharacterRuns.End(); it != endIt; ++it)
    {
      const CharacterIndex& characterIndex     = it->characterRun.characterIndex;
      const Length&         numberOfCharacters = it->characterRun.numberOfCharacters;

      if(numberOfCharacters == 0)
      {
        continue;
      }

      CharacterSpacingGlyphRun characterSpacingGlyphRun;
      characterSpacingGlyphRun.value                   = it->value;
      characterSpacingGlyphRun.glyphRun.glyphIndex     = charactersToGlyph[characterIndex];
      characterSpacingGlyphRun.glyphRun.numberOfGlyphs = glyphsPerCharacter[characterIndex];

      for(Length index = 1u; index < numberOfCharacters; index++)
      {
        characterSpacingGlyphRun.glyphRun.numberOfGlyphs += glyphsPerCharacter[characterIndex + index];
      }

      mTextModel->mVisualModel->mCharacterSpacingRuns.PushBack(characterSpacingGlyphRun);
    }
    mTextModel->mLogicalModel->mCharacterSpacingRunsUpdated = false;
  }
}

Size AsyncTextLoader::Layout(AsyncTextParameters& parameters, bool& updated)
{
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_ASYNC_LAYOUT");

  ////////////////////////////////////////////////////////////////////////////////
  // Layout the text.
  ////////////////////////////////////////////////////////////////////////////////

  Length& numberOfCharacters = mNumberOfCharacters;

  // Calculate the number of glyphs to layout.
  const Vector<GlyphIndex>& charactersToGlyph        = mTextModel->mVisualModel->mCharactersToGlyph;
  const Vector<Length>&     glyphsPerCharacter       = mTextModel->mVisualModel->mGlyphsPerCharacter;
  const GlyphIndex* const   charactersToGlyphBuffer  = charactersToGlyph.Begin();
  const Length* const       glyphsPerCharacterBuffer = glyphsPerCharacter.Begin();

  const CharacterIndex startIndex      = 0u;
  const CharacterIndex lastIndex       = numberOfCharacters > 0u ? numberOfCharacters - 1u : 0u;
  const GlyphIndex     startGlyphIndex = 0u;

  // Make sure the index is not out of bound
  if(charactersToGlyph.Count() != glyphsPerCharacter.Count() ||
     numberOfCharacters > charactersToGlyph.Count() ||
     (lastIndex > charactersToGlyph.Count() && charactersToGlyph.Count() > 0u))
  {
    DALI_LOG_ERROR("Attempting to access invalid buffer\n");
    DALI_LOG_ERROR("Current text is: %s\n", parameters.text.c_str());
    DALI_LOG_ERROR("startIndex: %u, lastIndex: %u, requestedNumberOfCharacters: %u, charactersToGlyph.Count = %lu, glyphsPerCharacter.Count = %lu\n", startIndex, lastIndex, numberOfCharacters, charactersToGlyph.Count(), glyphsPerCharacter.Count());
    return Size::ZERO;
  }

  const Length numberOfGlyphs      = (numberOfCharacters > 0u) ? *(charactersToGlyphBuffer + lastIndex) + *(glyphsPerCharacterBuffer + lastIndex) - startGlyphIndex : 0u;
  const Length totalNumberOfGlyphs = mTextModel->mVisualModel->mGlyphs.Count();

  if(0u == totalNumberOfGlyphs)
  {
    mTextModel->mVisualModel->SetLayoutSize(Size::ZERO);

    // Nothing else to do if there is no glyphs.
    DALI_LOG_RELEASE_INFO("no glyphs\n");
    return Size::ZERO;
  }

  const Text::Layout::Engine::Type layoutType = parameters.isMultiLine ? Text::Layout::Engine::MULTI_LINE_BOX : Text::Layout::Engine::SINGLE_LINE_BOX;
  mLayoutEngine.SetLayout(layoutType);

  // Set minimun line size, line spacing, relative line size.
  mLayoutEngine.SetDefaultLineSize(parameters.minLineSize);
  mLayoutEngine.SetDefaultLineSpacing(parameters.lineSpacing);
  mLayoutEngine.SetRelativeLineSize(parameters.relativeLineSize);

  // Set vertical line alignment.
  mTextModel->mVerticalLineAlignment = parameters.verticalLineAlignment;

  // Set character spacing.
  mTextModel->mVisualModel->SetCharacterSpacing(parameters.characterSpacing);

  // Set the layout parameters.
  Size textLayoutArea(parameters.textWidth, parameters.textHeight);

  mTextModel->mLineWrapMode          = parameters.lineWrapMode;
  mTextModel->mIgnoreSpacesAfterText = false;

  // Set the layout parameters.
  Layout::Parameters layoutParameters(textLayoutArea, mTextModel, mModule.GetFontClient(), mModule.GetBidirectionalSupport());

  // Resize the vector of positions to have the same size than the vector of glyphs.
  Vector<Vector2>& glyphPositions = mTextModel->mVisualModel->mGlyphPositions;
  glyphPositions.Resize(totalNumberOfGlyphs);

  // The initial glyph and the number of glyphs to layout.
  layoutParameters.startGlyphIndex        = startGlyphIndex;
  layoutParameters.numberOfGlyphs         = numberOfGlyphs;
  layoutParameters.startLineIndex         = 0u;
  layoutParameters.estimatedNumberOfLines = 1u;
  layoutParameters.interGlyphExtraAdvance = 0.f;

  // Whether the last character is a new paragraph character.
  const Character* const textBuffer = mTextModel->mLogicalModel->mText.Begin();
  layoutParameters.isLastNewParagraph = TextAbstraction::IsNewParagraph(*(textBuffer + (mTextModel->mLogicalModel->mText.Count() - 1u)));

  // Update the ellipsis
  bool ellipsisEnabled      = parameters.ellipsis;
  mTextModel->mElideEnabled = ellipsisEnabled;
  mTextModel->mVisualModel->SetTextElideEnabled(ellipsisEnabled);

  auto ellipsisPosition = parameters.ellipsisPosition;
  mTextModel->mEllipsisPosition = ellipsisPosition;
  mTextModel->mVisualModel->SetEllipsisPosition(ellipsisPosition);

  // Update the visual model.
  Size newLayoutSize; // The size of the text after it has been laid-out.
  bool isAutoScrollEnabled            = parameters.isAutoScrollEnabled;
  bool isAutoScrollMaxTextureExceeded = parameters.isAutoScrollMaxTextureExceeded;
  bool isHiddenInputEnabled           = false;

  updated = mLayoutEngine.LayoutText(layoutParameters,
                                     newLayoutSize,
                                     ellipsisEnabled,
                                     isAutoScrollEnabled,
                                     isAutoScrollMaxTextureExceeded,
                                     isHiddenInputEnabled,
                                     ellipsisPosition);

  mIsTextDirectionRTL = false;

  if(!mTextModel->mVisualModel->mLines.Empty())
  {
    mIsTextDirectionRTL = mTextModel->mVisualModel->mLines[0u].direction;
  }

  // Store the actual size of the text after it has been laid-out.
  mTextModel->mVisualModel->SetLayoutSize(newLayoutSize);


  ////////////////////////////////////////////////////////////////////////////////
  // Align the text.
  ////////////////////////////////////////////////////////////////////////////////

  mTextModel->mHorizontalAlignment = parameters.horizontalAlignment;

  Vector<LineRun>& lines = mTextModel->mVisualModel->mLines; // The laid out lines.

  // Calculate the horizontal offset according with the given alignment.
  float alignmentOffset = 0.f;

  // Need to align with the control's size as the text may contain lines
  // starting either with left to right text or right to left.
  mLayoutEngine.Align(textLayoutArea,
                      0u,
                      numberOfCharacters,
                      parameters.horizontalAlignment,
                      lines,
                      alignmentOffset,
                      parameters.layoutDirection,
                      (mTextModel->mMatchLayoutDirection != DevelText::MatchLayoutDirection::CONTENTS));

  // Calculate vertical offset.
  Size layoutSize = mTextModel->mVisualModel->GetLayoutSize();

  switch(parameters.verticalAlignment)
  {
    case VerticalAlignment::TOP:
    {
      mTextModel->mScrollPosition.y = 0.f;
      break;
    }
    case VerticalAlignment::CENTER:
    {
      mTextModel->mScrollPosition.y = floorf(0.5f * (textLayoutArea.height - layoutSize.height));
      break;
    }
    case VerticalAlignment::BOTTOM:
    {
      mTextModel->mScrollPosition.y = textLayoutArea.height - layoutSize.height;
      break;
    }
  }

#ifdef TRACE_ENABLED
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("ControlSize : %f, %f, LayoutSize : %f, %f\n", textLayoutArea.x, textLayoutArea.y, newLayoutSize.x, newLayoutSize.y);
  }
#endif

  return newLayoutSize;
}

AsyncTextRenderInfo AsyncTextLoader::Render(AsyncTextParameters& parameters)
{
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_ASYNC_RENDER");

  // render test
  mTypesetter->SetFontClient(mModule.GetFontClient());

  // Check whether it is a markup text with multiple text colors
  const Vector4* const colorsBuffer          = mTextModel->GetColors();
  bool                 hasMultipleTextColors = (NULL != colorsBuffer);

  // Check whether the text contains any color glyph
  bool containsColorGlyph = false;

  const Text::GlyphInfo* const glyphsBuffer   = mTextModel->GetGlyphs();
  const Text::Length           numberOfGlyphs = mTextModel->GetNumberOfGlyphs();
  for(Text::Length glyphIndex = 0; glyphIndex < numberOfGlyphs; glyphIndex++)
  {
    // Retrieve the glyph's info.
    const Text::GlyphInfo* const glyphInfo = glyphsBuffer + glyphIndex;

    // Whether the current glyph is a color one.
    if(mModule.GetFontClient().IsColorGlyph(glyphInfo->fontId, glyphInfo->index))
    {
      containsColorGlyph = true;
      break;
    }
  }

  // Check whether the text contains any style colors (e.g. underline color, shadow color, etc.)
  bool           shadowEnabled = false;
  const Vector2& shadowOffset  = mTextModel->GetShadowOffset();
  if(fabsf(shadowOffset.x) > Math::MACHINE_EPSILON_1 || fabsf(shadowOffset.y) > Math::MACHINE_EPSILON_1)
  {
    shadowEnabled = true;
  }

  const bool outlineEnabled              = mTextModel->GetOutlineWidth() > Math::MACHINE_EPSILON_1;
  const bool backgroundEnabled           = mTextModel->IsBackgroundEnabled();
  const bool markupOrSpannedText         = parameters.enableMarkup || mTextModel->IsSpannedTextPlaced();
  const bool markupUnderlineEnabled      = markupOrSpannedText && mTextModel->IsMarkupUnderlineSet();
  const bool markupStrikethroughEnabled  = markupOrSpannedText && mTextModel->IsMarkupStrikethroughSet();
  const bool underlineEnabled            = mTextModel->IsUnderlineEnabled() || markupUnderlineEnabled;
  const bool strikethroughEnabled        = mTextModel->IsStrikethroughEnabled() || markupStrikethroughEnabled;
  const bool backgroundMarkupSet         = mTextModel->IsMarkupBackgroundColorSet();
  const bool cutoutEnabled               = mTextModel->IsCutoutEnabled();
  const bool backgroundWithCutoutEnabled = mTextModel->IsBackgroundWithCutoutEnabled();
  const bool styleEnabled                = (shadowEnabled || outlineEnabled || backgroundEnabled || markupOrSpannedText || backgroundMarkupSet || cutoutEnabled || backgroundWithCutoutEnabled);
  const bool isOverlayStyle              = underlineEnabled || strikethroughEnabled;

  // Create RGBA texture if the text contains emojis or multiple text colors, otherwise L8 texture
  Pixel::Format textPixelFormat = (containsColorGlyph || hasMultipleTextColors || cutoutEnabled) ? Pixel::RGBA8888 : Pixel::L8;

  // The width is the control's width, height is the minimum height of the text.
  // This calculated layout size determines the size of the pixel data buffer.
  Size layoutSize = mTextModel->mVisualModel->GetLayoutSize();
  layoutSize.x    = parameters.textWidth;

  if(shadowOffset.y > Math::MACHINE_EPSILON_1)
  {
    layoutSize.y += shadowOffset.y;
  }

  float outlineWidth = mTextModel->GetOutlineWidth();
  layoutSize.y += outlineWidth * 2.0f;
  layoutSize.y = std::min(layoutSize.y, parameters.textHeight);

  if(cutoutEnabled)
  {
    // We need to store the offset including padding and vertical alignment.
    float xOffset = parameters.padding.start;
    float yOffset = parameters.padding.top + std::round((parameters.textHeight - layoutSize.y) * VERTICAL_ALIGNMENT_TABLE[parameters.verticalAlignment]);
    mTextModel->mVisualModel->SetOffsetWithCutout(Vector2(xOffset, yOffset));

    // The layout size is set to the text control size including padding.
    layoutSize.x = parameters.textWidth + (parameters.padding.start + parameters.padding.end);
    layoutSize.y = parameters.textHeight + (parameters.padding.top + parameters.padding.bottom);
  }

#ifdef TRACE_ENABLED
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("ControlSize : %f, %f, LayoutSize : %f, %f\n", parameters.textWidth, parameters.textHeight, layoutSize.x, layoutSize.y);
  }
#endif

  // Check the text direction
  Toolkit::DevelText::TextDirection::Type textDirection = mIsTextDirectionRTL ? Toolkit::DevelText::TextDirection::RIGHT_TO_LEFT : Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT;

  // Set information for creating pixel datas.
  AsyncTextRenderInfo renderInfo;
  renderInfo.width  = static_cast<uint32_t>(layoutSize.x);
  renderInfo.height = static_cast<uint32_t>(layoutSize.y);

  // Set the direction of text.
  renderInfo.isTextDirectionRTL = mIsTextDirectionRTL;

  Devel::PixelBuffer cutoutData;
  if(cutoutEnabled)
  {
    cutoutData = mTypesetter->RenderWithPixelBuffer(layoutSize, textDirection, Text::Typesetter::RENDER_NO_STYLES, false, textPixelFormat);

    // Make transparent buffer.
    // If the cutout is enabled, a separate texture is not used for the text.
    Devel::PixelBuffer buffer = mTypesetter->CreateFullBackgroundBuffer(1, 1, Color::TRANSPARENT);
    renderInfo.textPixelData = Devel::PixelBuffer::Convert(buffer);

    // Set the flag of cutout.
    renderInfo.isCutout = cutoutEnabled && (cutoutData != nullptr);
  }
  else
  {
    // Create a pixel data for the text without any styles
    renderInfo.textPixelData = mTypesetter->Render(layoutSize, textDirection, Text::Typesetter::RENDER_NO_STYLES, false, textPixelFormat);
  }

  if(styleEnabled)
  {
    if(renderInfo.isCutout)
    {
      float cutoutAlpha = mTextModel->GetDefaultColor().a;
      renderInfo.stylePixelData = mTypesetter->RenderWithCutout(layoutSize, textDirection, cutoutData, Text::Typesetter::RENDER_NO_TEXT, false, Pixel::RGBA8888, cutoutAlpha);
    }
    else
    {
      // Create RGBA pixel data for all the text styles (without the text itself)
      renderInfo.stylePixelData = mTypesetter->Render(layoutSize, textDirection, Text::Typesetter::RENDER_NO_TEXT, false, Pixel::RGBA8888);
    }
  }
  if(isOverlayStyle)
  {
    // Create RGBA pixel data for all the overlay styles
    renderInfo.overlayStylePixelData = mTypesetter->Render(layoutSize, textDirection, Text::Typesetter::RENDER_OVERLAY_STYLE, false, Pixel::RGBA8888);
  }
  if(containsColorGlyph && !hasMultipleTextColors)
  {
    // Create a L8 pixel data as a mask to avoid color glyphs (e.g. emojis) to be affected by text color animation
    renderInfo.maskPixelData = mTypesetter->Render(layoutSize, textDirection, Text::Typesetter::RENDER_MASK, false, Pixel::L8);
  }
  if(parameters.isAutoScrollEnabled)
  {
    // This will be uploaded in async text interface's setup auto scroll.
    renderInfo.autoScrollPixelData = mTypesetter->Render(layoutSize, textDirection, Text::Typesetter::RENDER_TEXT_AND_STYLES, true, Pixel::RGBA8888); // ignore the horizontal alignment
  }

  renderInfo.hasMultipleTextColors = hasMultipleTextColors;
  renderInfo.containsColorGlyph    = containsColorGlyph;
  renderInfo.styleEnabled          = styleEnabled;
  renderInfo.isOverlayStyle        = isOverlayStyle;
  renderInfo.manualRendered        = parameters.manualRender;
  renderInfo.lineCount             = mTextModel->GetNumberOfLines();

  if(cutoutEnabled)
  {
    renderInfo.renderedSize = Size(static_cast<float>(renderInfo.width), static_cast<float>(renderInfo.height));
  }
  else
  {
    renderInfo.renderedSize = Size(parameters.textWidth, parameters.textHeight);
  }

  return renderInfo;
}

AsyncTextRenderInfo AsyncTextLoader::RenderText(AsyncTextParameters& parameters)
{
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_ASYNC_RENDER_TEXT");

  if(parameters.requestType == Async::RENDER_CONSTRAINT)
  {
    Size textNaturalSize = ComputeNaturalSize(parameters);
    // textWidth is widthConstraint
    if(parameters.textWidth > textNaturalSize.width)
    {
      parameters.textWidth = textNaturalSize.width;
    }
  }

  if(parameters.requestType == Async::RENDER_FIXED_WIDTH || parameters.requestType == Async::RENDER_CONSTRAINT)
  {
    // In case of CONSTRAINT, the natural size has already been calculated.
    // So we can skip Initialize and Update at this stage.
    // Only the layout is newly calculated to obtain the height.
    bool layoutOnly = (parameters.requestType == Async::RENDER_CONSTRAINT);
    float height = ComputeHeightForWidth(parameters, parameters.textWidth, layoutOnly);

    // textHeight is heightConstraint.
    if(parameters.textHeight < height)
    {
      bool layoutUpdated = false;
      // Re-layout is required to apply new height.
      Layout(parameters, layoutUpdated);
    }
    else
    {
      parameters.textHeight = height;
    }

    mTextModel->mVisualModel->mControlSize = Size(parameters.textWidth, parameters.textHeight);
  }
  else
  {
    Initialize();
    Update(parameters);
    bool layoutUpdated = false;
    Layout(parameters, layoutUpdated);
  }

  return Render(parameters);
}

float AsyncTextLoader::ComputeHeightForWidth(AsyncTextParameters& parameters, float width, bool layoutOnly)
{
#ifdef TRACE_ENABLED
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("ComputeHeightForWidth, width:%f, layoutOnly:%d\n", width, layoutOnly);
  }
#endif

  float actualWidth  = parameters.textWidth;
  float actualHeight = parameters.textHeight;

  parameters.textWidth  = width;
  parameters.textHeight = MAX_FLOAT;

  if(!layoutOnly)
  {
    Initialize();
    Update(parameters);
  }

  bool layoutUpdated = false;
  Size layoutSize = Layout(parameters, layoutUpdated);

  // Restore actual size.
  parameters.textWidth  = actualWidth;
  parameters.textHeight = actualHeight;
  mTextModel->mVisualModel->mControlSize = Size(parameters.textWidth, parameters.textHeight);

  return layoutSize.height;
}

Size AsyncTextLoader::ComputeNaturalSize(AsyncTextParameters& parameters)
{
#ifdef TRACE_ENABLED
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("ComputeNaturalSize\n");
  }
#endif

  float actualWidth  = parameters.textWidth;
  float actualHeight = parameters.textHeight;

  // To measure natural size, set the size of the control to the maximum.
  parameters.textWidth  = MAX_FLOAT;
  parameters.textHeight = MAX_FLOAT;

  Initialize();
  Update(parameters);
  bool layoutUpdated = false;

  Size naturalSize = Layout(parameters, layoutUpdated);

  // Restore actual size.
  parameters.textWidth  = actualWidth;
  parameters.textHeight = actualHeight;
  mTextModel->mVisualModel->mControlSize = Size(parameters.textWidth, parameters.textHeight);

  return naturalSize;
}

AsyncTextRenderInfo AsyncTextLoader::GetHeightForWidth(AsyncTextParameters& parameters)
{
  float height = ComputeHeightForWidth(parameters, parameters.textWidth, false);
  AsyncTextRenderInfo renderInfo;
  renderInfo.renderedSize.width  = parameters.textWidth;
  renderInfo.renderedSize.height = height;
  renderInfo.requestType         = Async::COMPUTE_HEIGHT_FOR_WIDTH;
  renderInfo.lineCount           = mTextModel->GetNumberOfLines();

  return renderInfo;
}

AsyncTextRenderInfo AsyncTextLoader::GetNaturalSize(AsyncTextParameters& parameters)
{
  Size textNaturalSize = ComputeNaturalSize(parameters);
  AsyncTextRenderInfo renderInfo;
  renderInfo.renderedSize = textNaturalSize;
  renderInfo.requestType  = Async::COMPUTE_NATURAL_SIZE;
  renderInfo.lineCount    = mTextModel->GetNumberOfLines();

  return renderInfo;
}

AsyncTextRenderInfo AsyncTextLoader::RenderAutoScroll(AsyncTextParameters& parameters)
{
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_ASYNC_RENDER_AUTO_SCROLL");

  Size controlSize(parameters.textWidth, parameters.textHeight);

  // As relayout of text may not be done at this point natural size is used to get size. Single line scrolling only.
  Size textNaturalSize = ComputeNaturalSize(parameters);
  textNaturalSize.width += (parameters.padding.start + parameters.padding.end);
  textNaturalSize.height += (parameters.padding.top + parameters.padding.bottom);

  if(parameters.requestType == Async::RENDER_FIXED_WIDTH || parameters.requestType == Async::RENDER_CONSTRAINT)
  {
    // The real height calculated during layout should be set.
    parameters.textHeight = textNaturalSize.height - (parameters.padding.top + parameters.padding.bottom);
    controlSize.height = parameters.textHeight;
    mTextModel->mVisualModel->mControlSize = Size(parameters.textWidth, parameters.textHeight);
  }

#ifdef TRACE_ENABLED
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("natural size : %f, %f, control size : %f, %f\n", textNaturalSize.x, textNaturalSize.y, controlSize.x, controlSize.y);
  }
#endif

  // Calculate the actual gap before scrolling wraps.
  int     textPadding = std::max(controlSize.x - textNaturalSize.x, 0.0f);
  float   wrapGap     = std::max(parameters.autoScrollGap, textPadding);
  Vector2 textureSize = textNaturalSize + Vector2(wrapGap, 0.0f); // Add the gap as a part of the texture.

  // Calculate a size of texture for text scrolling
  Size      verifiedSize   = textureSize;
  const int maxTextureSize = parameters.maxTextureSize;

  // If the texture size width exceed maxTextureSize, modify the visual model size and enabled the ellipsis.
  if(verifiedSize.width > maxTextureSize)
  {
    verifiedSize.width = maxTextureSize;
    if(textNaturalSize.width > maxTextureSize)
    {
      float actualWidth  = parameters.textWidth;
      float actualHeight = parameters.textHeight;

      parameters.textWidth  = verifiedSize.width;
      parameters.textHeight = textNaturalSize.height;
      parameters.isAutoScrollMaxTextureExceeded = true;

      bool layoutUpdated = false;

      // Re-layout is required to apply ellipsis.
      Layout(parameters, layoutUpdated);

      parameters.textWidth  = actualWidth;
      parameters.textHeight = actualHeight;
    }
    wrapGap = std::max(maxTextureSize - textNaturalSize.width, 0.0f);
  }

  uint32_t actualWidth = parameters.textWidth;
  parameters.textWidth = verifiedSize.width;

  AsyncTextRenderInfo renderInfo = Render(parameters);

  // Restore actual size.
  parameters.textWidth = actualWidth;

  // Store the control size and calculated wrap gap in render info.
  renderInfo.controlSize       = controlSize;
  renderInfo.autoScrollWrapGap = wrapGap;
  renderInfo.renderedSize      = controlSize;

  return renderInfo;
}

bool AsyncTextLoader::CheckForTextFit(AsyncTextParameters& parameters, float pointSize, const Size& allowedSize)
{
  parameters.fontSize = pointSize;

  Initialize();
  Update(parameters);
  bool layoutUpdated = false;
  Size layoutSize = Layout(parameters, layoutUpdated);

  if(!layoutUpdated || layoutSize.width > allowedSize.width || layoutSize.height > allowedSize.height)
  {
    return false;
  }
  return true;
}

AsyncTextRenderInfo AsyncTextLoader::RenderTextFit(AsyncTextParameters& parameters)
{
  if(parameters.requestType == Async::RENDER_CONSTRAINT)
  {
    Size textNaturalSize = ComputeNaturalSize(parameters);
    // textWidth is widthConstraint
    if(parameters.textWidth > textNaturalSize.width)
    {
      parameters.textWidth = textNaturalSize.width;
    }
  }

  if(parameters.requestType == Async::RENDER_FIXED_WIDTH || parameters.requestType == Async::RENDER_CONSTRAINT)
  {
    // In case of CONSTRAINT, the natural size has already been calculated.
    // So we can skip Initialize and Update at this stage.
    // Only the layout is newly calculated to obtain the height.
    bool layoutOnly = (parameters.requestType == Async::RENDER_CONSTRAINT);
    float height = ComputeHeightForWidth(parameters, parameters.textWidth, layoutOnly);

    // textHeight is heightConstraint
    if(parameters.textHeight > height)
    {
      parameters.textHeight = height;
    }
    DALI_LOG_WARNING("TextFit requires a fixed size. Render with natural size : %f, %f\n", parameters.textWidth, parameters.textHeight);
  }

  if(parameters.isTextFitArrayEnabled)
  {
#ifdef TRACE_ENABLED
    if(gTraceFilter && gTraceFilter->IsTraceEnabled())
    {
      DALI_LOG_RELEASE_INFO("AsyncTextLoader::RenderTextFit -> TextFitArray\n");
    }
#endif

    std::vector<DevelTextLabel::FitOption> fitOptions = parameters.textFitArray;
    int numberOfFitOptions = static_cast<int>(fitOptions.size());
    if(numberOfFitOptions == 0)
    {
      DALI_LOG_ERROR("fitOptions is empty, render with default value, point size:%f, min line size:%f\n", parameters.fontSize, parameters.minLineSize);
      fitOptions.push_back(DevelTextLabel::FitOption(parameters.fontSize, parameters.minLineSize));
      numberOfFitOptions = 1;
    }

    mFitActualEllipsis  = parameters.ellipsis;
    parameters.ellipsis = false;

    Size allowedSize(parameters.textWidth, parameters.textHeight);

    // Sort in ascending order by PointSize.
    std::sort(fitOptions.begin(), fitOptions.end(), compareByPointSize);

    // Decide whether to use binary search.
    // If MinLineSize is not sorted in ascending order,
    // binary search cannot guarantee that it will always find the best value.
    bool  binarySearch    = true;
    float prevMinLineSize = 0.0f;
    for(DevelTextLabel::FitOption& option : fitOptions)
    {
      float optionMinLineSize = option.GetMinLineSize();
      if(prevMinLineSize > optionMinLineSize)
      {
        binarySearch = false;
        break;
      }
      prevMinLineSize = optionMinLineSize;
    }

    // Set the first FitOption(Minimum PointSize) to the best value.
    // If the search does not find an optimal value, the minimum PointSize will be used to text fit.
    DevelTextLabel::FitOption firstOption = fitOptions.front();
    bool  bestSizeUpdatedLatest = false;
    float bestPointSize         = firstOption.GetPointSize();
    float bestMinLineSize       = firstOption.GetMinLineSize();

    if(binarySearch)
    {
      int left = 0u;
      int right = numberOfFitOptions - 1;

      while (left <= right)
      {
        int mid = left + (right - left) / 2;
        DevelTextLabel::FitOption option = fitOptions[mid];
        float testPointSize   = option.GetPointSize();
        float testMinLineSize = option.GetMinLineSize();
        parameters.minLineSize = testMinLineSize;

        if(CheckForTextFit(parameters, testPointSize, allowedSize))
        {
          bestSizeUpdatedLatest = true;
          bestPointSize   = testPointSize;
          bestMinLineSize = testMinLineSize;
          left = mid + 1;
        }
        else
        {
          bestSizeUpdatedLatest = false;
          right = mid - 1;
        }
      }
    }
    else
    {
      // If binary search is not possible, search sequentially starting from the largest PointSize.
      for(auto it = fitOptions.rbegin(); it != fitOptions.rend(); ++it)
      {
        DevelTextLabel::FitOption option = *it;
        float testPointSize   = option.GetPointSize();
        float testMinLineSize = option.GetMinLineSize();
        parameters.minLineSize = testMinLineSize;

        if(CheckForTextFit(parameters, testPointSize, allowedSize))
        {
          bestSizeUpdatedLatest = true;
          bestPointSize   = testPointSize;
          bestMinLineSize = testMinLineSize;
          break;
        }
        else
        {
          bestSizeUpdatedLatest = false;
        }
      }
    }

    // Best point size was not updated. re-run so the TextFit should be fitted really.
    if(!bestSizeUpdatedLatest)
    {
      parameters.ellipsis    = mFitActualEllipsis;
      parameters.minLineSize = bestMinLineSize;
      CheckForTextFit(parameters, bestPointSize, allowedSize);
    }

    return Render(parameters);
  }
  else if(parameters.isTextFitEnabled)
  {
#ifdef TRACE_ENABLED
    if(gTraceFilter && gTraceFilter->IsTraceEnabled())
    {
      DALI_LOG_RELEASE_INFO("AsyncTextLoader::RenderTextFit -> TextFit\n");
    }
#endif

    float minPointSize           = parameters.textFitMinSize;
    float maxPointSize           = parameters.textFitMaxSize;
    float pointInterval          = parameters.textFitStepSize;

    mFitActualEllipsis  = parameters.ellipsis;
    parameters.ellipsis = false;
    float bestPointSize = minPointSize;

    Size allowedSize(parameters.textWidth, parameters.textHeight);

    // check zero value
    if(pointInterval < 1.f)
    {
      parameters.textFitStepSize = pointInterval = 1.0f;
    }

    uint32_t pointSizeRange = static_cast<uint32_t>(ceil((maxPointSize - minPointSize) / pointInterval));

    // Ensure minPointSize + pointSizeRange * pointInverval >= maxPointSize
    while(minPointSize + static_cast<float>(pointSizeRange) * pointInterval < maxPointSize)
    {
      ++pointSizeRange;
    }

    uint32_t bestSizeIndex = 0;
    uint32_t minIndex      = bestSizeIndex + 1u;
    uint32_t maxIndex      = pointSizeRange + 1u;

    bool bestSizeUpdatedLatest = false;
    // Find best size as binary search.
    // Range format as [l r). (left closed, right opened)
    // It mean, we already check all i < l is valid, and r <= i is invalid.
    // Below binary search will check m = (l+r)/2 point.
    // Search area sperate as [l m) or [m+1 r)
    //
    // Basically, we can assume that 0 (minPointSize) is always valid.
    // Now, we will check [1 pointSizeRange] range s.t. pointSizeRange mean the maxPointSize
    while(minIndex < maxIndex)
    {
      uint32_t    testIndex     = minIndex + ((maxIndex - minIndex) >> 1u);
      const float testPointSize = std::min(maxPointSize, minPointSize + static_cast<float>(testIndex) * pointInterval);

      if(CheckForTextFit(parameters, testPointSize, allowedSize))
      {
        bestSizeUpdatedLatest = true;

        bestSizeIndex = testIndex;
        minIndex      = testIndex + 1u;
      }
      else
      {
        bestSizeUpdatedLatest = false;
        maxIndex              = testIndex;
      }
    }
    bestPointSize = std::min(maxPointSize, minPointSize + static_cast<float>(bestSizeIndex) * pointInterval);

    // Best point size was not updated. re-run so the TextFit should be fitted really.
    if(!bestSizeUpdatedLatest)
    {
      parameters.ellipsis = mFitActualEllipsis;
      CheckForTextFit(parameters, bestPointSize, allowedSize);
    }

    return Render(parameters);
  }
  else
  {
    DALI_LOG_ERROR("There is no TextFit information in AsyncTextParameters. It returns empty AsyncTextRenderInfo.\n");
    AsyncTextRenderInfo renderInfo;
    return renderInfo;
  }
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali
