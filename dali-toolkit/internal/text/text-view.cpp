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
#include <dali-toolkit/internal/text/text-view.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/public-api/math/vector2.h>
#include <memory.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
struct View::Impl
{
  VisualModelPtr              mVisualModel;
  TextAbstraction::FontClient mFontClient; ///< Handle to the font client.
};

View::View()
: mImpl(NULL)
{
  mImpl = new View::Impl();

  mImpl->mFontClient = TextAbstraction::FontClient::Get();
}

View::~View()
{
  delete mImpl;
}

void View::SetVisualModel(VisualModelPtr visualModel)
{
  mImpl->mVisualModel = visualModel;
}

const Vector2& View::GetControlSize() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->mControlSize;
  }

  return Vector2::ZERO;
}

const Vector2& View::GetLayoutSize() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetLayoutSize();
  }

  return Vector2::ZERO;
}

Length View::GetNumberOfGlyphs() const
{
  if(mImpl->mVisualModel)
  {
    const VisualModel& model = *mImpl->mVisualModel;

    const Length glyphCount    = model.mGlyphs.Count();
    const Length positionCount = model.mGlyphPositions.Count();

    DALI_ASSERT_DEBUG(positionCount <= glyphCount && "Invalid glyph positions in Model");

    return (positionCount < glyphCount) ? positionCount : glyphCount;
  }

  return 0;
}

Length View::GetGlyphs(GlyphInfo* glyphs,
                       Vector2*   glyphPositions,
                       float&     minLineOffset,
                       GlyphIndex glyphIndex,
                       Length     numberOfGlyphs) const
{
  Length numberOfLaidOutGlyphs       = 0u;
  Length numberOfActualLaidOutGlyphs = 0u;

  if(mImpl->mVisualModel)
  {
    bool                              textElided       = false;
    DevelText::EllipsisPosition::Type ellipsisPosition = GetEllipsisPosition();

    //Reset indices of ElidedGlyphs
    mImpl->mVisualModel->SetStartIndexOfElidedGlyphs(0u);
    mImpl->mVisualModel->SetEndIndexOfElidedGlyphs(numberOfGlyphs);
    mImpl->mVisualModel->SetFirstMiddleIndexOfElidedGlyphs(0u);
    mImpl->mVisualModel->SetSecondMiddleIndexOfElidedGlyphs(0u);

    // If ellipsis is enabled, the number of glyphs the layout engine has laid out may be less than 'numberOfGlyphs'.
    // Check the last laid out line to know if the layout engine elided some text.

    const Length numberOfLines = mImpl->mVisualModel->mLines.Count();
    if(numberOfLines > 0u)
    {
      const LineRun* const lines = mImpl->mVisualModel->mLines.Begin();

      //Get line of ellipsis
      const LineRun* ellipsisLine     = nullptr;
      const LineRun* ellipsisNextLine = nullptr;
      bool           hasEllipsis      = false;
      for(Length lineIndex = 0; lineIndex < numberOfLines; lineIndex++)
      {
        const LineRun* line = (lines + lineIndex);
        if(line->ellipsis)
        {
          ellipsisLine = line;
          hasEllipsis  = true;
          if(lineIndex < numberOfLines - 1u)
          {
            ellipsisNextLine = (lines + lineIndex + 1u);
          }
          break;
        }
      }

      // If ellipsis is enabled, calculate the number of laid out glyphs.
      // Otherwise use the given number of glyphs.
      if(hasEllipsis)
      {
        textElided            = true;
        numberOfLaidOutGlyphs = numberOfGlyphs;

        if(ellipsisPosition == DevelText::EllipsisPosition::START)
        {
          numberOfActualLaidOutGlyphs = numberOfGlyphs - ellipsisLine->glyphRun.glyphIndex;
        }
        else if(ellipsisPosition == DevelText::EllipsisPosition::MIDDLE)
        {
          numberOfActualLaidOutGlyphs = 0u;
          for(Length lineIndex = 0u; lineIndex < numberOfLines; lineIndex++)
          {
            numberOfActualLaidOutGlyphs += lines[lineIndex].glyphRun.numberOfGlyphs + lines[lineIndex].glyphRunSecondHalf.numberOfGlyphs;
          }
        }
        else // DevelText::EllipsisPosition::END
        {
          numberOfActualLaidOutGlyphs = ellipsisLine->glyphRun.glyphIndex + ellipsisLine->glyphRun.numberOfGlyphs;
        }
      }
      else
      {
        numberOfActualLaidOutGlyphs = numberOfLaidOutGlyphs = numberOfGlyphs;
      }

      if(0u < numberOfActualLaidOutGlyphs)
      {
        // Retrieve from the visual model the glyphs and positions.
        mImpl->mVisualModel->GetGlyphs(glyphs,
                                       glyphIndex,
                                       numberOfLaidOutGlyphs);

        mImpl->mVisualModel->GetGlyphPositions(glyphPositions,
                                               glyphIndex,
                                               numberOfLaidOutGlyphs);

        // Get the lines for the given range of glyphs.
        // The lines contain the alignment offset which needs to be added to the glyph's position.
        LineIndex firstLineIndex = 0u;
        Length    numberOfLines  = 0u;
        mImpl->mVisualModel->GetNumberOfLines(glyphIndex,
                                              numberOfLaidOutGlyphs,
                                              firstLineIndex,
                                              numberOfLines);

        Vector<LineRun> lines;
        lines.Resize(numberOfLines);
        LineRun* lineBuffer = lines.Begin();

        mImpl->mVisualModel->GetLinesOfGlyphRange(lineBuffer,
                                                  glyphIndex,
                                                  numberOfLaidOutGlyphs);

        // Get the first line for the given glyph range.
        LineIndex lineIndex = firstLineIndex;
        LineRun*  line      = lineBuffer + lineIndex;

        // Index of the last glyph of the line.
        GlyphIndex lastGlyphIndexOfLine = (line->isSplitToTwoHalves ? line->glyphRunSecondHalf.glyphIndex + line->glyphRunSecondHalf.numberOfGlyphs : line->glyphRun.glyphIndex + line->glyphRun.numberOfGlyphs) - 1u;

        // Add the alignment offset to the glyph's position.

        minLineOffset = line->alignmentOffset;
        float penY    = line->ascender;
        for(Length index = 0u; index < numberOfLaidOutGlyphs; ++index)
        {
          Vector2& position = *(glyphPositions + index);
          position.x += line->alignmentOffset;
          position.y += penY;

          if(lastGlyphIndexOfLine == index)
          {
            penY += -line->descender + line->lineSpacing;

            // Get the next line.
            ++lineIndex;

            if(lineIndex < numberOfLines)
            {
              line          = lineBuffer + lineIndex;
              minLineOffset = std::min(minLineOffset, line->alignmentOffset);

              lastGlyphIndexOfLine = (line->isSplitToTwoHalves ? line->glyphRunSecondHalf.glyphIndex + line->glyphRunSecondHalf.numberOfGlyphs : line->glyphRun.glyphIndex + line->glyphRun.numberOfGlyphs) - 1u;

              penY += line->ascender;
            }
          }
        }

        // Set index where to set Ellipsis according to the selected position of Ellipsis.
        // Start with this index to replace its glyph by Ellipsis, if the width  is not enough, then remove more glyphs.
        GlyphIndex startIndexOfEllipsis = 0u;
        if(hasEllipsis)
        {
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
        }

        if(1u == numberOfLaidOutGlyphs)
        {
          // not a point try to do ellipsis with only one laid out character.

          return numberOfLaidOutGlyphs;
        }

        if(textElided)
        {
          const LineRun& elidedLine = *ellipsisLine;

          if((1u == numberOfLines) &&
             (GetLineHeight(elidedLine) > mImpl->mVisualModel->mControlSize.height))
          {
            // Replace the first glyph with ellipsis glyph
            auto indexOfFirstGlyph = (ellipsisPosition == DevelText::EllipsisPosition::START) ? startIndexOfEllipsis : 0u;

            // Regardless where the location of ellipsis,in-case the hight of line is greater than control's height
            // then replace the first glyph with ellipsis glyph.

            // Get the first glyph which is going to be replaced and the ellipsis glyph.
            GlyphInfo&       glyphInfo     = *(glyphs + indexOfFirstGlyph);
            const GlyphInfo& ellipsisGlyph = mImpl->mFontClient.GetEllipsisGlyph(mImpl->mFontClient.GetPointSize(glyphInfo.fontId));

            // Change the 'x' and 'y' position of the ellipsis glyph.
            Vector2& position = *(glyphPositions + indexOfFirstGlyph);
            position.x        = ellipsisGlyph.xBearing;
            position.y        = mImpl->mVisualModel->mControlSize.height - ellipsisGlyph.yBearing;

            // Replace the glyph by the ellipsis glyph.
            glyphInfo = ellipsisGlyph;

            mImpl->mVisualModel->SetStartIndexOfElidedGlyphs(indexOfFirstGlyph);
            mImpl->mVisualModel->SetEndIndexOfElidedGlyphs(indexOfFirstGlyph);
            mImpl->mVisualModel->SetFirstMiddleIndexOfElidedGlyphs(indexOfFirstGlyph);
            mImpl->mVisualModel->SetSecondMiddleIndexOfElidedGlyphs(indexOfFirstGlyph);

            numberOfLaidOutGlyphs = 1u;

            return numberOfLaidOutGlyphs;
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
          const bool isTailMode = ellipsisPosition == DevelText::EllipsisPosition::END ||
                                  (ellipsisPosition == DevelText::EllipsisPosition::MIDDLE && numberOfLines != 1u);

          // The ellipsis glyph has to fit in the place where the last glyph(s) is(are) removed.
          while(!inserted)
          {
            const GlyphInfo& glyphToRemove = *(glyphs + indexOfEllipsis);

            if(0u != glyphToRemove.fontId)
            {
              // i.e. The font id of the glyph shaped from the '\n' character is zero.

              // Need to reshape the glyph as the font may be different in size.
              const GlyphInfo& ellipsisGlyph = mImpl->mFontClient.GetEllipsisGlyph(mImpl->mFontClient.GetPointSize(glyphToRemove.fontId));

              if(!firstPenSet)
              {
                const Vector2& position = *(glyphPositions + indexOfEllipsis);

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

              removedGlypsWidth += std::min(glyphToRemove.advance, (glyphToRemove.xBearing + glyphToRemove.width));

              // Calculate the width of the ellipsis glyph and check if it fits.
              const float ellipsisGlyphWidth = ellipsisGlyph.width + ellipsisGlyph.xBearing;
              if((ellipsisGlyphWidth < removedGlypsWidth) || (isTailMode ? (indexOfEllipsis == 0u) : (indexOfEllipsis == numberOfGlyphs - 1u)))
              {
                GlyphInfo& glyphInfo = *(glyphs + indexOfEllipsis);
                Vector2&   position  = *(glyphPositions + indexOfEllipsis);
                position.x -= (0.f > glyphInfo.xBearing) ? glyphInfo.xBearing : 0.f;

                // Replace the glyph by the ellipsis glyph.
                glyphInfo = ellipsisGlyph;

                // Change the 'x' and 'y' position of the ellipsis glyph.

                if(position.x > firstPenX)
                {
                  position.x = firstPenX + removedGlypsWidth - ellipsisGlyphWidth;
                }

                position.x += ellipsisGlyph.xBearing;
                position.y = penY - ellipsisGlyph.yBearing;

                inserted = true;
              }
            }

            if(!inserted)
            {
              if(isTailMode && indexOfEllipsis > 0u)
              {
                // Tail Mode: remove glyphs from startIndexOfEllipsis then decrement indexOfEllipsis, until arrive to index zero.
                --indexOfEllipsis;
              }
              else if(!isTailMode && indexOfEllipsis < numberOfLaidOutGlyphs - 1u)
              {
                // Not Tail Mode: remove glyphs from startIndexOfEllipsis then increase indexOfEllipsis, until arrive to last index (numberOfGlyphs - 1u).
                ++indexOfEllipsis;
              }
              else
              {
                // No space for the ellipsis.
                inserted = true;
              }
              ++numberOfRemovedGlyphs;
            }
          }

          // 'Removes' all the glyphs after the ellipsis glyph.
          if(ellipsisPosition == DevelText::EllipsisPosition::MIDDLE)
          {
            //Reduce size, shift glyphs and start from ellipsis glyph
            numberOfLaidOutGlyphs = numberOfActualLaidOutGlyphs - numberOfRemovedGlyphs;

            GlyphIndex firstMiddleIndexOfElidedGlyphs  = 0u;
            GlyphIndex secondMiddleIndexOfElidedGlyphs = 0u;

            bool isOnlySecondHalf = false;
            if(isTailMode)
            {
              // Multi-lines case with MIDDLE
              // In case the Ellipsis in the end of line,
              // then this index will be the firstMiddleIndex.
              // The secondMiddleIndex will be the fisrt index in next line.
              // But in case there is no line after Ellipsis's line then secondMiddleIndex and endIndex equal firstMiddle
              // Example:
              // A: are laid out glyphs in line has Ellipsis in the end.
              // N: are laid out glyphs in lines after removed lines.
              // R: are removed glyphs.
              // L: are removed glyphs when removed lines.
              // AAAAAAAAAAAA...RRR    => Here's the firstMiddleIndex (First index after last A)
              // LLLLLLLLLLLLLLL
              // LLLLLLLLLLLLLLL
              // NNNNNNNNNNNNNN        => Here's the secondMiddleIndex (First N)
              // NNNNNNNNNN

              firstMiddleIndexOfElidedGlyphs = indexOfEllipsis;
              if(ellipsisNextLine != nullptr)
              {
                secondMiddleIndexOfElidedGlyphs = ellipsisNextLine->glyphRun.glyphIndex;
              }
              else
              {
                secondMiddleIndexOfElidedGlyphs = firstMiddleIndexOfElidedGlyphs;
                mImpl->mVisualModel->SetEndIndexOfElidedGlyphs(firstMiddleIndexOfElidedGlyphs);
              }
            }
            else
            {
              // Single line case with MIDDLE
              // In case the Ellipsis in the middle of line,
              // Then the last index in first half will be firstMiddleIndex.
              // And the indexOfEllipsis will be secondMiddleIndex, which is the first index in second half.
              // Example:
              // A: are laid out glyphs in first half of line.
              // N: are laid out glyphs in second half of line.
              // R: are removed glyphs.
              // L: re removed glyphs when layouting text
              // AAAAAAALLLLLLLLLLLRRR...NNNNN
              // firstMiddleIndex (index of last A)
              // secondMiddleIndex (index before first N)

              firstMiddleIndexOfElidedGlyphs  = (ellipsisLine->glyphRun.numberOfGlyphs > 0u) ? (ellipsisLine->glyphRun.glyphIndex + ellipsisLine->glyphRun.numberOfGlyphs - 1u) : (ellipsisLine->glyphRun.glyphIndex);
              secondMiddleIndexOfElidedGlyphs = indexOfEllipsis;
              isOnlySecondHalf                = ellipsisLine->glyphRun.numberOfGlyphs == 0u && ellipsisLine->glyphRunSecondHalf.numberOfGlyphs > 0u;
            }

            mImpl->mVisualModel->SetFirstMiddleIndexOfElidedGlyphs(firstMiddleIndexOfElidedGlyphs);
            mImpl->mVisualModel->SetSecondMiddleIndexOfElidedGlyphs(secondMiddleIndexOfElidedGlyphs);

            // The number of shifted glyphs and shifting positions will be different according to Single-line or Multi-lines.
            // isOnlySecondHalf will be true when MIDDLE Ellipsis glyph in single line.
            if(isOnlySecondHalf)
            {
              Length numberOfSecondHalfGlyphs = numberOfLaidOutGlyphs - firstMiddleIndexOfElidedGlyphs;

              //Copy elided glyphs after the ellipsis glyph.
              memcpy(glyphs + firstMiddleIndexOfElidedGlyphs, glyphs + secondMiddleIndexOfElidedGlyphs, numberOfSecondHalfGlyphs * sizeof(GlyphInfo));
              memcpy(glyphPositions + firstMiddleIndexOfElidedGlyphs, glyphPositions + secondMiddleIndexOfElidedGlyphs, numberOfSecondHalfGlyphs * sizeof(Vector2));
            }
            else
            {
              Length numberOfSecondHalfGlyphs = numberOfLaidOutGlyphs - firstMiddleIndexOfElidedGlyphs + 1u;

              //Copy elided glyphs after the ellipsis glyph.
              memcpy(glyphs + firstMiddleIndexOfElidedGlyphs + 1u, glyphs + secondMiddleIndexOfElidedGlyphs, numberOfSecondHalfGlyphs * sizeof(GlyphInfo));
              memcpy(glyphPositions + firstMiddleIndexOfElidedGlyphs + 1u, glyphPositions + secondMiddleIndexOfElidedGlyphs, numberOfSecondHalfGlyphs * sizeof(Vector2));
            }
          }
          else if(ellipsisPosition == DevelText::EllipsisPosition::START)
          {
            numberOfLaidOutGlyphs = numberOfActualLaidOutGlyphs - numberOfRemovedGlyphs;
            //Copy elided glyphs after the ellipsis glyph.
            memcpy(glyphs, glyphs + startIndexOfEllipsis + numberOfRemovedGlyphs, numberOfLaidOutGlyphs * sizeof(GlyphInfo));
            memcpy(glyphPositions, glyphPositions + startIndexOfEllipsis + numberOfRemovedGlyphs, numberOfLaidOutGlyphs * sizeof(Vector2));
            mImpl->mVisualModel->SetStartIndexOfElidedGlyphs(indexOfEllipsis);
          }
          else // DevelText::EllipsisPosition::END
          {
            numberOfLaidOutGlyphs = numberOfActualLaidOutGlyphs - numberOfRemovedGlyphs;
            mImpl->mVisualModel->SetEndIndexOfElidedGlyphs(indexOfEllipsis);
          }
        }
      }
    }
  }

  return numberOfLaidOutGlyphs;
}

const Vector4* const View::GetColors() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->mColors.Begin();
  }

  return NULL;
}

const ColorIndex* const View::GetColorIndices() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->mColorIndices.Begin();
  }

  return NULL;
}

const Vector4* const View::GetBackgroundColors() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->mBackgroundColors.Begin();
  }

  return nullptr;
}

const ColorIndex* const View::GetBackgroundColorIndices() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->mBackgroundColorIndices.Begin();
  }

  return nullptr;
}

bool const View::IsMarkupBackgroundColorSet() const
{
  if(mImpl->mVisualModel)
  {
    return (mImpl->mVisualModel->mBackgroundColors.Count() > 0);
  }

  return false;
}

const Vector4& View::GetTextColor() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetTextColor();
  }
  return Vector4::ZERO;
}

const Vector2& View::GetShadowOffset() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetShadowOffset();
  }
  return Vector2::ZERO;
}

const Vector4& View::GetShadowColor() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetShadowColor();
  }
  return Vector4::ZERO;
}

const Vector4& View::GetUnderlineColor() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetUnderlineColor();
  }
  return Vector4::ZERO;
}

bool View::IsUnderlineEnabled() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->IsUnderlineEnabled();
  }
  return false;
}

const GlyphInfo* View::GetHyphens() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->mHyphen.glyph.Begin();
  }

  return nullptr;
}

const Length* View::GetHyphenIndices() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->mHyphen.index.Begin();
  }

  return nullptr;
}

Length View::GetHyphensCount() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->mHyphen.glyph.Size();
  }

  return 0;
}
float View::GetUnderlineHeight() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetUnderlineHeight();
  }
  return 0.0f;
}

Length View::GetNumberOfUnderlineRuns() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetNumberOfUnderlineRuns();
  }

  return 0u;
}

void View::GetUnderlineRuns(GlyphRun*         underlineRuns,
                            UnderlineRunIndex index,
                            Length            numberOfRuns) const
{
  if(mImpl->mVisualModel)
  {
    mImpl->mVisualModel->GetUnderlineRuns(underlineRuns,
                                          index,
                                          numberOfRuns);
  }
}

const Vector4& View::GetOutlineColor() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetOutlineColor();
  }
  return Vector4::ZERO;
}

uint16_t View::GetOutlineWidth() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetOutlineWidth();
  }
  return 0u;
}

DevelText::EllipsisPosition::Type View::GetEllipsisPosition() const
{
  DevelText::EllipsisPosition::Type ellipsisPosition = DevelText::EllipsisPosition::END;
  if(mImpl->mVisualModel)
  {
    const VisualModel& model = *mImpl->mVisualModel;
    ellipsisPosition         = model.GetEllipsisPosition();
  }

  return ellipsisPosition;
}

bool View::IsTextElideEnabled() const
{
  bool isTextElideEnabled = false;

  if(mImpl->mVisualModel)
  {
    const VisualModel& model = *mImpl->mVisualModel;
    isTextElideEnabled       = model.IsTextElideEnabled();
  }

  return isTextElideEnabled;
}

GlyphIndex View::GetStartIndexOfElidedGlyphs() const
{
  GlyphIndex startIndexOfElidedGlyphs = 0u;

  if(mImpl->mVisualModel)
  {
    const VisualModel& model = *mImpl->mVisualModel;
    startIndexOfElidedGlyphs = model.GetStartIndexOfElidedGlyphs();
  }

  return startIndexOfElidedGlyphs;
}

GlyphIndex View::GetEndIndexOfElidedGlyphs() const
{
  GlyphIndex endIndexOfElidedGlyphs = 0u;

  if(mImpl->mVisualModel)
  {
    const VisualModel& model = *mImpl->mVisualModel;
    endIndexOfElidedGlyphs   = model.GetEndIndexOfElidedGlyphs();
  }

  return endIndexOfElidedGlyphs;
}

GlyphIndex View::GetFirstMiddleIndexOfElidedGlyphs() const
{
  GlyphIndex firstMiddleIndexOfElidedGlyphs = 0u;

  if(mImpl->mVisualModel)
  {
    const VisualModel& model       = *mImpl->mVisualModel;
    firstMiddleIndexOfElidedGlyphs = model.GetFirstMiddleIndexOfElidedGlyphs();
  }

  return firstMiddleIndexOfElidedGlyphs;
}

GlyphIndex View::GetSecondMiddleIndexOfElidedGlyphs() const
{
  GlyphIndex secondMiddleIndexOfElidedGlyphs = 0u;

  if(mImpl->mVisualModel)
  {
    const VisualModel& model        = *mImpl->mVisualModel;
    secondMiddleIndexOfElidedGlyphs = model.GetSecondMiddleIndexOfElidedGlyphs();
  }

  return secondMiddleIndexOfElidedGlyphs;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
