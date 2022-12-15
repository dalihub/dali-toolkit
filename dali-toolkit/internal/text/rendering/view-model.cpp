/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/rendering/view-model.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/font-client.h>
#include <memory.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/glyph-metrics-helper.h>
#include <dali-toolkit/internal/text/line-run.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
ViewModel::ViewModel(const ModelInterface* const model)
: mModel(model),
  mElidedGlyphs(),
  mElidedLayout(),
  mIsTextElided(false),
  mStartIndexOfElidedGlyphs(0u),
  mEndIndexOfElidedGlyphs(0u),
  mFirstMiddleIndexOfElidedGlyphs(0u),
  mSecondMiddleIndexOfElidedGlyphs(0u)
{
}

ViewModel::~ViewModel()
{
}

const Size& ViewModel::GetControlSize() const
{
  return mModel->GetControlSize();
}

const Size& ViewModel::GetLayoutSize() const
{
  return mModel->GetLayoutSize();
}

const Vector2& ViewModel::GetScrollPosition() const
{
  return mModel->GetScrollPosition();
}

HorizontalAlignment::Type ViewModel::GetHorizontalAlignment() const
{
  return mModel->GetHorizontalAlignment();
}

VerticalAlignment::Type ViewModel::GetVerticalAlignment() const
{
  return mModel->GetVerticalAlignment();
}

DevelText::VerticalLineAlignment::Type ViewModel::GetVerticalLineAlignment() const
{
  return mModel->GetVerticalLineAlignment();
}

DevelText::EllipsisPosition::Type ViewModel::GetEllipsisPosition() const
{
  return mModel->GetEllipsisPosition();
}

bool ViewModel::IsTextElideEnabled() const
{
  return mModel->IsTextElideEnabled();
}

Length ViewModel::GetNumberOfLines() const
{
  return mModel->GetNumberOfLines();
}

const LineRun* const ViewModel::GetLines() const
{
  return mModel->GetLines();
}

Length ViewModel::GetNumberOfScripts() const
{
  return mModel->GetNumberOfScripts();
}

const ScriptRun* const ViewModel::GetScriptRuns() const
{
  return mModel->GetScriptRuns();
}

Length ViewModel::GetNumberOfGlyphs() const
{
  if(mIsTextElided && mModel->IsTextElideEnabled())
  {
    return mElidedGlyphs.Count();
  }
  else
  {
    return mModel->GetNumberOfGlyphs();
  }

  return 0u;
}

GlyphIndex ViewModel::GetStartIndexOfElidedGlyphs() const
{
  if(mIsTextElided && mModel->IsTextElideEnabled())
  {
    return mStartIndexOfElidedGlyphs;
  }

  return mModel->GetStartIndexOfElidedGlyphs();
}

GlyphIndex ViewModel::GetEndIndexOfElidedGlyphs() const
{
  if(mIsTextElided && mModel->IsTextElideEnabled())
  {
    return mEndIndexOfElidedGlyphs;
  }

  return mModel->GetEndIndexOfElidedGlyphs();
}

GlyphIndex ViewModel::GetFirstMiddleIndexOfElidedGlyphs() const
{
  if(mIsTextElided && mModel->IsTextElideEnabled())
  {
    return mFirstMiddleIndexOfElidedGlyphs;
  }

  return mModel->GetFirstMiddleIndexOfElidedGlyphs();
}

GlyphIndex ViewModel::GetSecondMiddleIndexOfElidedGlyphs() const
{
  if(mIsTextElided && mModel->IsTextElideEnabled())
  {
    return mSecondMiddleIndexOfElidedGlyphs;
  }

  return mModel->GetSecondMiddleIndexOfElidedGlyphs();
}

const GlyphInfo* const ViewModel::GetGlyphs() const
{
  if(mIsTextElided && mModel->IsTextElideEnabled())
  {
    return mElidedGlyphs.Begin();
  }
  else
  {
    return mModel->GetGlyphs();
  }

  return NULL;
}

const Vector2* const ViewModel::GetLayout() const
{
  if(mIsTextElided && mModel->IsTextElideEnabled())
  {
    return mElidedLayout.Begin();
  }
  else
  {
    return mModel->GetLayout();
  }

  return NULL;
}

const Vector4* const ViewModel::GetColors() const
{
  return mModel->GetColors();
}

const ColorIndex* const ViewModel::GetColorIndices() const
{
  return mModel->GetColorIndices();
}

const Vector4* const ViewModel::GetBackgroundColors() const
{
  return mModel->GetBackgroundColors();
}

const ColorIndex* const ViewModel::GetBackgroundColorIndices() const
{
  return mModel->GetBackgroundColorIndices();
}

bool const ViewModel::IsMarkupBackgroundColorSet() const
{
  return mModel->IsMarkupBackgroundColorSet();
}

const Vector4& ViewModel::GetDefaultColor() const
{
  return mModel->GetDefaultColor();
}

const Vector2& ViewModel::GetShadowOffset() const
{
  return mModel->GetShadowOffset();
}

const Vector4& ViewModel::GetShadowColor() const
{
  return mModel->GetShadowColor();
}

const float& ViewModel::GetShadowBlurRadius() const
{
  return mModel->GetShadowBlurRadius();
}

const Vector4& ViewModel::GetUnderlineColor() const
{
  return mModel->GetUnderlineColor();
}

bool ViewModel::IsUnderlineEnabled() const
{
  return mModel->IsUnderlineEnabled();
}

bool const ViewModel::IsMarkupUnderlineSet() const
{
  return mModel->IsMarkupUnderlineSet();
}

float ViewModel::GetUnderlineHeight() const
{
  return mModel->GetUnderlineHeight();
}

Text::Underline::Type ViewModel::GetUnderlineType() const
{
  return mModel->GetUnderlineType();
}

float ViewModel::GetDashedUnderlineWidth() const
{
  return mModel->GetDashedUnderlineWidth();
}

float ViewModel::GetDashedUnderlineGap() const
{
  return mModel->GetDashedUnderlineGap();
}

Length ViewModel::GetNumberOfUnderlineRuns() const
{
  return mModel->GetNumberOfUnderlineRuns();
}

void ViewModel::GetUnderlineRuns(UnderlinedGlyphRun* underlineRuns, UnderlineRunIndex index, Length numberOfRuns) const
{
  mModel->GetUnderlineRuns(underlineRuns, index, numberOfRuns);
}

const Vector4& ViewModel::GetOutlineColor() const
{
  return mModel->GetOutlineColor();
}

uint16_t ViewModel::GetOutlineWidth() const
{
  return mModel->GetOutlineWidth();
}

const Vector4& ViewModel::GetBackgroundColor() const
{
  return mModel->GetBackgroundColor();
}

bool ViewModel::IsBackgroundEnabled() const
{
  return mModel->IsBackgroundEnabled();
}

bool ViewModel::IsMarkupProcessorEnabled() const
{
  return mModel->IsMarkupProcessorEnabled();
}

bool ViewModel::IsSpannedTextPlaced() const
{
  return mModel->IsSpannedTextPlaced();
}

const GlyphInfo* ViewModel::GetHyphens() const
{
  return mModel->GetHyphens();
}

const Length* ViewModel::GetHyphenIndices() const
{
  return mModel->GetHyphenIndices();
}

Length ViewModel::GetHyphensCount() const
{
  return mModel->GetHyphensCount();
}

const float ViewModel::GetCharacterSpacing() const
{
  return mModel->GetCharacterSpacing();
}

const Character* ViewModel::GetTextBuffer() const
{
  return mModel->GetTextBuffer();
}

const Vector<CharacterIndex>& ViewModel::GetGlyphsToCharacters() const
{
  return mModel->GetGlyphsToCharacters();
}

void ViewModel::ElideGlyphs()
{
  mIsTextElided             = false;
  mStartIndexOfElidedGlyphs = mFirstMiddleIndexOfElidedGlyphs = mSecondMiddleIndexOfElidedGlyphs = 0;
  mEndIndexOfElidedGlyphs                                                                        = mModel->GetNumberOfGlyphs() - 1u;

  auto                          ellipsisPosition          = GetEllipsisPosition();
  auto                          characterSpacing          = GetCharacterSpacing();
  const Character*              textBuffer                = GetTextBuffer();
  const Vector<CharacterIndex>& glyphToCharacterMap       = GetGlyphsToCharacters();
  const CharacterIndex*         glyphToCharacterMapBuffer = glyphToCharacterMap.Begin();
  float                         calculatedAdvance         = 0.f;

  if(IsTextElideEnabled())
  {
    const Length numberOfLines = mModel->GetNumberOfLines();
    if(0u != numberOfLines)
    {
      const LineRun* const lines = mModel->GetLines();

      //Get line of ellipsis
      const LineRun* ellipsisLine     = nullptr;
      const LineRun* ellipsisNextLine = nullptr;

      for(Length lineIndex = 0; lineIndex < numberOfLines; lineIndex++)
      {
        const LineRun* line = (lines + lineIndex);
        if(line->ellipsis)
        {
          ellipsisLine = line;
          if(lineIndex < numberOfLines - 1u)
          {
            ellipsisNextLine = (lines + lineIndex + 1u);
          }
          break;
        }
      }

      // Check if there is a line contains Ellipsis.
      // Then find total number of glyphs and total number of laid out glyphs.
      // Check where to set Ellipsis glyph in line.
      // Determine index of Ellipsis glyph and how many glyphs should be replaced by Ellipsis glyph, according to width of Ellipsis glyph.
      if(ellipsisLine != nullptr)
      {
        // Total number of glyphs.
        const Length numberOfGlyphs = mModel->GetNumberOfGlyphs();
        // Total number of laid out glyphs.
        Length numberOfActualLaidOutGlyphs = 0u;

        // Accumulate laid out glyphs for each line to find total number of laid out glyphs.
        for(Length lineIndex = 0u; lineIndex < numberOfLines; lineIndex++)
        {
          numberOfActualLaidOutGlyphs += lines[lineIndex].glyphRun.numberOfGlyphs + lines[lineIndex].glyphRunSecondHalf.numberOfGlyphs;
        }

        // Make sure there are laid out glyphs.
        if(0u != numberOfActualLaidOutGlyphs)
        {
          // There are elided glyphs.
          mIsTextElided                          = true;
          TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

          // Retrieve the whole glyphs and their positions.
          const GlyphInfo* const glyphs    = mModel->GetGlyphs();
          const Vector2* const   positions = mModel->GetLayout();

          // Copy the glyphs to be elided.
          mElidedGlyphs.Resize(numberOfGlyphs);
          mElidedLayout.Resize(numberOfGlyphs);
          GlyphInfo* elidedGlyphsBuffer    = mElidedGlyphs.Begin();
          Vector2*   elidedPositionsBuffer = mElidedLayout.Begin();

          memcpy(elidedGlyphsBuffer, glyphs, numberOfGlyphs * sizeof(GlyphInfo));
          memcpy(elidedPositionsBuffer, positions, numberOfGlyphs * sizeof(Vector2));

          const Size& controlSize = mModel->GetControlSize();

          // Set index where to set Ellipsis according to the selected position of Ellipsis.
          // Start with this index to replace its glyph by Ellipsis, if the width  is not enough, then remove more glyphs.
          GlyphIndex startIndexOfEllipsis = 0u;
          if(ellipsisPosition == DevelText::EllipsisPosition::START)
          {
            // It's the fisrt glyph in line.
            startIndexOfEllipsis = ellipsisLine->glyphRun.glyphIndex;
          }
          else if(ellipsisPosition == DevelText::EllipsisPosition::MIDDLE)
          {
            // It's the second middle of the line in case the line split to two halves.
            // Otherwise it's It's the last glyph in line (line before all removed lines).
            startIndexOfEllipsis = ellipsisLine->isSplitToTwoHalves ? (ellipsisLine->glyphRunSecondHalf.glyphIndex) : (ellipsisLine->glyphRun.glyphIndex + ellipsisLine->glyphRun.numberOfGlyphs - 1u);
          }
          else // DevelText::EllipsisPosition::END
          {
            // It's the last glyph in line.
            startIndexOfEllipsis = ellipsisLine->glyphRun.glyphIndex + ellipsisLine->glyphRun.numberOfGlyphs - 1u;
          }

          // When the hight is not enough then show one glyph and that should be the first laid out glyph.
          if((1u == numberOfLines) &&
             (ellipsisLine->ascender - ellipsisLine->descender > controlSize.height))
          {
            // Replace the first glyph with ellipsis glyph
            auto indexOfFirstGlyph = (ellipsisPosition == DevelText::EllipsisPosition::START) ? startIndexOfEllipsis : 0u;

            // Regardless where the location of ellipsis,in-case the hight of line is greater than control's height
            // then replace the first glyph with ellipsis glyph.

            // Get the first glyph which is going to be replaced and the ellipsis glyph.
            GlyphInfo&       glyphToRemove = *(elidedGlyphsBuffer + indexOfFirstGlyph);
            const GlyphInfo& ellipsisGlyph = fontClient.GetEllipsisGlyph(fontClient.GetPointSize(glyphToRemove.fontId));

            // Change the 'x' and 'y' position of the ellipsis glyph.
            Vector2& position = *(elidedPositionsBuffer + indexOfFirstGlyph);

            position.x = ellipsisGlyph.xBearing;
            position.y = -ellipsisLine->ascender + controlSize.height - ellipsisGlyph.yBearing;

            // Replace the glyph by the ellipsis glyph and resize the buffers.
            glyphToRemove = ellipsisGlyph;

            mElidedGlyphs.Resize(1u);
            mElidedLayout.Resize(1u);

            mEndIndexOfElidedGlyphs = mStartIndexOfElidedGlyphs = mFirstMiddleIndexOfElidedGlyphs = mSecondMiddleIndexOfElidedGlyphs = indexOfFirstGlyph;

            return;
          }

          // firstPenX, penY and firstPenSet are used to position the ellipsis glyph if needed.
          float firstPenX   = 0.f; // Used if rtl text is elided.
          float penY        = 0.f;
          bool  firstPenSet = false;

          // Add the ellipsis glyph.
          bool       inserted              = false;
          float      removedGlypsWidth     = 0.f;
          Length     numberOfRemovedGlyphs = 0u;
          GlyphIndex indexOfEllipsis       = startIndexOfEllipsis;

          // Tail Mode: start by the end of line.
          bool isTailMode = (ellipsisPosition == DevelText::EllipsisPosition::END) ||
                            (ellipsisPosition == DevelText::EllipsisPosition::MIDDLE && numberOfLines != 1u);

          // The ellipsis glyph has to fit in the place where the last glyph(s) is(are) removed.
          while(!inserted)
          {
            const GlyphInfo& glyphToRemove = *(elidedGlyphsBuffer + indexOfEllipsis);

            if(0u != glyphToRemove.fontId)
            {
              // i.e. The font id of the glyph shaped from the '\n' character is zero.

              // Need to reshape the glyph as the font may be different in size.
              const GlyphInfo& ellipsisGlyph = fontClient.GetEllipsisGlyph(fontClient.GetPointSize(glyphToRemove.fontId));

              if(!firstPenSet || EqualsZero(glyphToRemove.advance))
              {
                const Vector2& position = *(elidedPositionsBuffer + indexOfEllipsis);

                // Calculates the penY of the current line. It will be used to position the ellipsis glyph.
                penY = position.y + glyphToRemove.yBearing;

                // Calculates the first penX which will be used if rtl text is elided.
                firstPenX = position.x - glyphToRemove.xBearing;
                if(firstPenX < -ellipsisGlyph.xBearing)
                {
                  // Avoids to exceed the bounding box when rtl text is elided.
                  firstPenX = -ellipsisGlyph.xBearing;
                }

                removedGlypsWidth = -ellipsisGlyph.xBearing;

                firstPenSet = true;
              }

              calculatedAdvance = GetCalculatedAdvance(*(textBuffer + (*(glyphToCharacterMapBuffer + indexOfEllipsis))), characterSpacing, glyphToRemove.advance);
              removedGlypsWidth += std::min(calculatedAdvance, (glyphToRemove.xBearing + glyphToRemove.width));

              // Calculate the width of the ellipsis glyph and check if it fits.
              const float ellipsisGlyphWidth = ellipsisGlyph.width + ellipsisGlyph.xBearing;

              // If it is the last glyph to remove, add the ellipsis glyph without checking its width.
              if((ellipsisGlyphWidth < removedGlypsWidth) || (isTailMode ? (indexOfEllipsis == 0u) : (indexOfEllipsis == numberOfGlyphs - 1u)))
              {
                GlyphInfo& glyphInfo = *(elidedGlyphsBuffer + indexOfEllipsis);
                Vector2&   position  = *(elidedPositionsBuffer + indexOfEllipsis);
                position.x -= (0.f > glyphInfo.xBearing) ? glyphInfo.xBearing : 0.f;

                // Replace the glyph by the ellipsis glyph.
                glyphInfo = ellipsisGlyph;

                // Change the 'x' and 'y' position of the ellipsis glyph.
                if(position.x > firstPenX)
                {
                  if(isTailMode)
                  {
                    // To handle case of the mixed languages (LTR then RTL) with
                    // EllipsisPosition::END and the LayoutDirection::RIGHT_TO_LEFT
                    float nextXPositions = ellipsisLine->width;
                    if(indexOfEllipsis + 1u < numberOfGlyphs)
                    {
                      Vector2& positionOfNextGlyph = *(elidedPositionsBuffer + indexOfEllipsis + 1u);
                      nextXPositions               = positionOfNextGlyph.x;
                    }

                    if(position.x > nextXPositions) // RTL language
                    {
                      if((indexOfEllipsis > 0u) && ((position.x - nextXPositions) > removedGlypsWidth))
                      {
                        // To handle mixed directions
                        // Re-calculates the first penX which will be used if rtl text is elided.
                        firstPenX = position.x - glyphToRemove.xBearing;
                        if(firstPenX < -ellipsisGlyph.xBearing)
                        {
                          // Avoids to exceed the bounding box when rtl text is elided.
                          firstPenX = -ellipsisGlyph.xBearing;
                        }
                        //Reset the width of removed glyphs
                        removedGlypsWidth = std::min(calculatedAdvance, (glyphToRemove.xBearing + glyphToRemove.width)) - ellipsisGlyph.xBearing;

                        --indexOfEllipsis;
                        continue;
                      }
                      else
                      {
                        // To handle the case of RTL language with EllipsisPosition::END
                        position.x = firstPenX + removedGlypsWidth - ellipsisGlyphWidth;
                      }
                    }
                  }
                  else
                  {
                    // To handle the case of LTR language with EllipsisPosition::START
                    position.x = firstPenX + removedGlypsWidth - ellipsisGlyphWidth;
                  }
                }
                else
                {
                  if(!isTailMode)
                  {
                    // To handle case of the mixed languages (RTL then LTR) with
                    // EllipsisPosition::START and the LayoutDirection::RIGHT_TO_LEFT
                    float nextXPositions = ellipsisLine->width;
                    if(indexOfEllipsis + 1u < numberOfGlyphs)
                    {
                      Vector2& positionOfNextGlyph = *(elidedPositionsBuffer + indexOfEllipsis + 1u);
                      nextXPositions               = positionOfNextGlyph.x;
                    }

                    if(position.x < nextXPositions) // LTR language
                    {
                      position.x = firstPenX + removedGlypsWidth - ellipsisGlyphWidth;

                      if((position.x + ellipsisGlyphWidth + ellipsisGlyph.xBearing) > nextXPositions)
                      {
                        position.x -= (position.x + ellipsisGlyphWidth + ellipsisGlyph.xBearing) - nextXPositions;
                      }
                    }
                  }
                }

                position.x += ellipsisGlyph.xBearing;
                position.y = penY - ellipsisGlyph.yBearing;

                inserted = true;
              }
            }

            if(!inserted)
            {
              if(!isTailMode && indexOfEllipsis < numberOfGlyphs - 1u)
              {
                // Tail Mode: remove glyphs from startIndexOfEllipsis then decrement indexOfEllipsis, until arrive to index zero.
                ++indexOfEllipsis;
              }
              else if(isTailMode && indexOfEllipsis > 0u)
              {
                // Not Tail Mode: remove glyphs from startIndexOfEllipsis then increase indexOfEllipsis, until arrive to last index (numberOfGlyphs - 1u).
                --indexOfEllipsis;
              }
              else
              {
                // No space for the ellipsis.
                inserted = true;
              }
              ++numberOfRemovedGlyphs;
            }
          } // while( !inserted )

          //Reduce size, shift glyphs and start from ellipsis glyph
          Length numberOfElidedGlyphs = numberOfActualLaidOutGlyphs - numberOfRemovedGlyphs;
          mElidedGlyphs.Resize(numberOfElidedGlyphs);
          mElidedLayout.Resize(numberOfElidedGlyphs);

          if(ellipsisPosition == DevelText::EllipsisPosition::START)
          {
            // 'Shifts' glyphs after ellipsis glyph and 'Removes' before ellipsis glyph
            memcpy(elidedGlyphsBuffer, elidedGlyphsBuffer + indexOfEllipsis, numberOfElidedGlyphs * sizeof(GlyphInfo));
            memcpy(elidedPositionsBuffer, elidedPositionsBuffer + indexOfEllipsis, numberOfElidedGlyphs * sizeof(Vector2));

            mStartIndexOfElidedGlyphs = mFirstMiddleIndexOfElidedGlyphs = mSecondMiddleIndexOfElidedGlyphs = indexOfEllipsis;
          }
          else if(ellipsisPosition == DevelText::EllipsisPosition::MIDDLE)
          {
            // 'Shifts and connects' glyphs before and after ellipsis glyph and 'Removes' in-between.
            bool isOnlySecondHalf = false;

            if(isTailMode)
            {
              mFirstMiddleIndexOfElidedGlyphs = indexOfEllipsis;
              if(ellipsisNextLine != nullptr)
              {
                mSecondMiddleIndexOfElidedGlyphs = ellipsisNextLine->glyphRun.glyphIndex;
              }
              else
              {
                mEndIndexOfElidedGlyphs = mSecondMiddleIndexOfElidedGlyphs = mFirstMiddleIndexOfElidedGlyphs;
              }
            }
            else
            {
              mFirstMiddleIndexOfElidedGlyphs  = (ellipsisLine->glyphRun.numberOfGlyphs > 0u) ? (ellipsisLine->glyphRun.glyphIndex + ellipsisLine->glyphRun.numberOfGlyphs - 1u) : (ellipsisLine->glyphRun.glyphIndex);
              mSecondMiddleIndexOfElidedGlyphs = indexOfEllipsis;
              isOnlySecondHalf                 = ellipsisLine->glyphRun.numberOfGlyphs == 0u && ellipsisLine->glyphRunSecondHalf.numberOfGlyphs > 0u;
            }

            if(isOnlySecondHalf)
            {
              Length numberOfSecondHalfGlyphs = numberOfElidedGlyphs - mFirstMiddleIndexOfElidedGlyphs;

              //Copy elided glyphs after the ellipsis glyph.
              memcpy(elidedGlyphsBuffer + mFirstMiddleIndexOfElidedGlyphs, elidedGlyphsBuffer + mSecondMiddleIndexOfElidedGlyphs, numberOfSecondHalfGlyphs * sizeof(GlyphInfo));
              memcpy(elidedPositionsBuffer + mFirstMiddleIndexOfElidedGlyphs, elidedPositionsBuffer + mSecondMiddleIndexOfElidedGlyphs, numberOfSecondHalfGlyphs * sizeof(Vector2));
            }
            else
            {
              Length numberOfSecondHalfGlyphs = numberOfElidedGlyphs - mFirstMiddleIndexOfElidedGlyphs + 1u;

              //Copy elided glyphs after the ellipsis glyph.
              memcpy(elidedGlyphsBuffer + mFirstMiddleIndexOfElidedGlyphs + 1u, elidedGlyphsBuffer + mSecondMiddleIndexOfElidedGlyphs, numberOfSecondHalfGlyphs * sizeof(GlyphInfo));
              memcpy(elidedPositionsBuffer + mFirstMiddleIndexOfElidedGlyphs + 1u, elidedPositionsBuffer + mSecondMiddleIndexOfElidedGlyphs, numberOfSecondHalfGlyphs * sizeof(Vector2));
            }
          }
          else // DevelText::EllipsisPosition::END
          {
            // 'Removes' all the glyphs after the ellipsis glyph.
            mEndIndexOfElidedGlyphs = indexOfEllipsis;
          }
        }
      }
    }
  }
}

float ViewModel::GetStrikethroughHeight() const
{
  return mModel->GetStrikethroughHeight();
}

const Vector4& ViewModel::GetStrikethroughColor() const
{
  return mModel->GetStrikethroughColor();
}

bool ViewModel::IsStrikethroughEnabled() const
{
  return mModel->IsStrikethroughEnabled();
}

bool const ViewModel::IsMarkupStrikethroughSet() const
{
  return mModel->IsMarkupStrikethroughSet();
}

Length ViewModel::GetNumberOfStrikethroughRuns() const
{
  return mModel->GetNumberOfStrikethroughRuns();
}

void ViewModel::GetStrikethroughRuns(StrikethroughGlyphRun* strikethroughRuns, StrikethroughRunIndex index, Length numberOfRuns) const
{
  mModel->GetStrikethroughRuns(strikethroughRuns, index, numberOfRuns);
}

Length ViewModel::GetNumberOfBoundedParagraphRuns() const
{
  return mModel->GetNumberOfBoundedParagraphRuns();
}

const Vector<BoundedParagraphRun>& ViewModel::GetBoundedParagraphRuns() const
{
  return mModel->GetBoundedParagraphRuns();
}

Length ViewModel::GetNumberOfCharacterSpacingGlyphRuns() const
{
  return mModel->GetNumberOfCharacterSpacingGlyphRuns();
}

const Vector<CharacterSpacingGlyphRun>& ViewModel::GetCharacterSpacingGlyphRuns() const
{
  return mModel->GetCharacterSpacingGlyphRuns();
}

const Vector<FontRun>& ViewModel::GetFontRuns() const
{
  return mModel->GetFontRuns();
}

const Vector<FontDescriptionRun>& ViewModel::GetFontDescriptionRuns() const
{
  return mModel->GetFontDescriptionRuns();
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
