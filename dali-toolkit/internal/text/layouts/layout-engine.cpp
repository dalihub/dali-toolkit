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
#include <dali-toolkit/internal/text/layouts/layout-engine.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/integration-api/debug.h>
#include <cmath>
#include <limits>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/bidirectional-support.h>
#include <dali-toolkit/internal/text/cursor-helper-functions.h>
#include <dali-toolkit/internal/text/glyph-metrics-helper.h>
#include <dali-toolkit/internal/text/layouts/layout-engine-helper-functions.h>
#include <dali-toolkit/internal/text/layouts/layout-parameters.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
float GetLineHeight(const LineRun lineRun, bool isLastLine)
{
  // The line height is the addition of the line ascender, the line descender and the line spacing.
  // However, the line descender has a negative value, hence the subtraction.
  // In case this is the only/last line then line spacing should be ignored.
  float lineHeight = lineRun.ascender - lineRun.descender;

  if(!isLastLine || lineRun.lineSpacing > 0)
  {
    lineHeight += lineRun.lineSpacing;
  }
  return lineHeight;
}

namespace Layout
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_LAYOUT");
#endif

const float              MAX_FLOAT          = std::numeric_limits<float>::max();
const CharacterDirection LTR                = false;
const CharacterDirection RTL                = !LTR;
const float              LINE_SPACING       = 0.f;
const float              MIN_LINE_SIZE      = 0.f;
const Character          HYPHEN_UNICODE     = 0x002D;
const float              RELATIVE_LINE_SIZE = 1.f;

inline bool isEmptyLineAtLast(const Vector<LineRun>& lines, const Vector<LineRun>::Iterator& line)
{
  return ((*line).characterRun.numberOfCharacters == 0 && line + 1u == lines.End());
}

} //namespace

/**
 * @brief Stores temporary layout info of the line.
 */
struct LineLayout
{
  LineLayout()
  : glyphIndex{0u},
    characterIndex{0u},
    numberOfGlyphs{0u},
    numberOfCharacters{0u},
    ascender{-MAX_FLOAT},
    descender{MAX_FLOAT},
    lineSpacing{0.f},
    penX{0.f},
    previousAdvance{0.f},
    length{0.f},
    whiteSpaceLengthEndOfLine{0.f},
    direction{LTR},
    isSplitToTwoHalves(false),
    glyphIndexInSecondHalfLine{0u},
    characterIndexInSecondHalfLine{0u},
    numberOfGlyphsInSecondHalfLine{0u},
    numberOfCharactersInSecondHalfLine{0u},
    relativeLineSize{1.0f}

  {
  }

  ~LineLayout()
  {
  }

  void Clear()
  {
    glyphIndex                         = 0u;
    characterIndex                     = 0u;
    numberOfGlyphs                     = 0u;
    numberOfCharacters                 = 0u;
    ascender                           = -MAX_FLOAT;
    descender                          = MAX_FLOAT;
    direction                          = LTR;
    isSplitToTwoHalves                 = false;
    glyphIndexInSecondHalfLine         = 0u;
    characterIndexInSecondHalfLine     = 0u;
    numberOfGlyphsInSecondHalfLine     = 0u;
    numberOfCharactersInSecondHalfLine = 0u;
    relativeLineSize                   = 1.0f;
  }

  GlyphIndex         glyphIndex;                ///< Index of the first glyph to be laid-out.
  CharacterIndex     characterIndex;            ///< Index of the first character to be laid-out.
  Length             numberOfGlyphs;            ///< The number of glyph which fit in one line.
  Length             numberOfCharacters;        ///< The number of characters which fit in one line.
  float              ascender;                  ///< The maximum ascender of all fonts in the line.
  float              descender;                 ///< The minimum descender of all fonts in the line.
  float              lineSpacing;               ///< The line spacing
  float              penX;                      ///< The origin of the current glyph ( is the start point plus the accumulation of all advances ).
  float              previousAdvance;           ///< The advance of the previous glyph.
  float              length;                    ///< The current length of the line.
  float              whiteSpaceLengthEndOfLine; ///< The length of the white spaces at the end of the line.
  CharacterDirection direction;

  bool           isSplitToTwoHalves;                 ///< Whether the second half is defined.
  GlyphIndex     glyphIndexInSecondHalfLine;         ///< Index of the first glyph to be laid-out for the second half of line.
  CharacterIndex characterIndexInSecondHalfLine;     ///< Index of the first character to be laid-out for the second half of line.
  Length         numberOfGlyphsInSecondHalfLine;     ///< The number of glyph which fit in one line for the second half of line.
  Length         numberOfCharactersInSecondHalfLine; ///< The number of characters which fit in one line for the second half of line.

  float relativeLineSize; ///< The relative line size to be applied for this line.
};

struct LayoutBidiParameters
{
  void Clear()
  {
    paragraphDirection = LTR;
    bidiParagraphIndex = 0u;
    bidiLineIndex      = 0u;
    isBidirectional    = false;
  }

  CharacterDirection        paragraphDirection = LTR;   ///< The paragraph's direction.
  BidirectionalRunIndex     bidiParagraphIndex = 0u;    ///< Index to the paragraph's bidi info.
  BidirectionalLineRunIndex bidiLineIndex      = 0u;    ///< Index where to insert the next bidi line info.
  bool                      isBidirectional    = false; ///< Whether the text is bidirectional.
};

struct Engine::Impl
{
  Impl()
  : mLayout{Layout::Engine::SINGLE_LINE_BOX},
    mCursorWidth{0.f},
    mDefaultLineSpacing{LINE_SPACING},
    mDefaultLineSize{MIN_LINE_SIZE},
    mRelativeLineSize{RELATIVE_LINE_SIZE}
  {
  }

  /**
   * @brief get the line spacing.
   *
   * @param[in] textSize The text size.
   * @param[in] relativeLineSize The relative line size to be applied.
   * @return the line spacing value.
   */
  float GetLineSpacing(float textSize, float relativeLineSize)
  {
    float lineSpacing;
    float relTextSize;

    // Sets the line size
    lineSpacing = mDefaultLineSize - textSize;
    lineSpacing = lineSpacing < 0.f ? 0.f : lineSpacing;

    // Add the line spacing
    lineSpacing += mDefaultLineSpacing;

    //subtract line spcaing if relativeLineSize < 1 & larger than min height
    relTextSize = textSize * relativeLineSize;
    if(relTextSize > mDefaultLineSize)
    {
      if(relativeLineSize < 1)
      {
        //subtract the difference (always will be positive)
        lineSpacing -= (textSize - relTextSize);
      }
      else
      {
        //reverse the addition in the top.
        if(mDefaultLineSize > textSize)
        {
          lineSpacing -= mDefaultLineSize - textSize;
        }

        //add difference instead
        lineSpacing += relTextSize - textSize;
      }
    }

    return lineSpacing;
  }

  /**
   * @brief Updates the line ascender and descender with the metrics of a new font.
   *
   * @param[in] glyphMetrics The metrics of the new font.
   * @param[in,out] lineLayout The line layout.
   */
  void UpdateLineHeight(const GlyphMetrics& glyphMetrics, LineLayout& lineLayout)
  {
    Text::FontMetrics fontMetrics;
    if(0u != glyphMetrics.fontId)
    {
      mMetrics->GetFontMetrics(glyphMetrics.fontId, fontMetrics);
    }
    else
    {
      fontMetrics.ascender           = glyphMetrics.fontHeight;
      fontMetrics.descender          = 0.f;
      fontMetrics.height             = fontMetrics.ascender;
      fontMetrics.underlinePosition  = 0.f;
      fontMetrics.underlineThickness = 1.f;
    }

    // Sets the maximum ascender.
    lineLayout.ascender = std::max(lineLayout.ascender, fontMetrics.ascender);

    // Sets the minimum descender.
    lineLayout.descender = std::min(lineLayout.descender, fontMetrics.descender);

    lineLayout.lineSpacing = GetLineSpacing(lineLayout.ascender + -lineLayout.descender, lineLayout.relativeLineSize);
  }

  /**
   * @brief Merges a temporary line layout into the line layout.
   *
   * @param[in,out] lineLayout The line layout.
   * @param[in] tmpLineLayout A temporary line layout.
   * @param[in] isShifted Whether to shift first glyph and character indices.
   */
  void MergeLineLayout(LineLayout&       lineLayout,
                       const LineLayout& tmpLineLayout,
                       bool              isShifted)
  {
    lineLayout.numberOfCharacters += tmpLineLayout.numberOfCharacters;
    lineLayout.numberOfGlyphs += tmpLineLayout.numberOfGlyphs;

    lineLayout.penX            = tmpLineLayout.penX;
    lineLayout.previousAdvance = tmpLineLayout.previousAdvance;

    lineLayout.length                    = tmpLineLayout.length;
    lineLayout.whiteSpaceLengthEndOfLine = tmpLineLayout.whiteSpaceLengthEndOfLine;

    // Sets the maximum ascender.
    lineLayout.ascender = std::max(lineLayout.ascender, tmpLineLayout.ascender);

    // Sets the minimum descender.
    lineLayout.descender = std::min(lineLayout.descender, tmpLineLayout.descender);

    // To handle cases START in ellipsis position when want to shift first glyph to let width fit.
    if(isShifted)
    {
      lineLayout.glyphIndex     = tmpLineLayout.glyphIndex;
      lineLayout.characterIndex = tmpLineLayout.characterIndex;
    }

    lineLayout.isSplitToTwoHalves                 = tmpLineLayout.isSplitToTwoHalves;
    lineLayout.glyphIndexInSecondHalfLine         = tmpLineLayout.glyphIndexInSecondHalfLine;
    lineLayout.characterIndexInSecondHalfLine     = tmpLineLayout.characterIndexInSecondHalfLine;
    lineLayout.numberOfGlyphsInSecondHalfLine     = tmpLineLayout.numberOfGlyphsInSecondHalfLine;
    lineLayout.numberOfCharactersInSecondHalfLine = tmpLineLayout.numberOfCharactersInSecondHalfLine;
  }

  void LayoutRightToLeft(const Parameters&               parameters,
                         const BidirectionalLineInfoRun& bidirectionalLineInfo,
                         float&                          length,
                         float&                          whiteSpaceLengthEndOfLine)
  {
    // Travers characters in line then draw it form right to left by mapping index using visualToLogicalMap.
    // When the line is spllited by MIDDLE ellipsis then travers the second half of line "characterRunForSecondHalfLine"
    // then the first half of line "characterRun",
    // Otherwise travers whole characters in"characterRun".

    const Character* const  textBuffer               = parameters.textModel->mLogicalModel->mText.Begin();
    const Length* const     charactersPerGlyphBuffer = parameters.textModel->mVisualModel->mCharactersPerGlyph.Begin();
    const GlyphInfo* const  glyphsBuffer             = parameters.textModel->mVisualModel->mGlyphs.Begin();
    const GlyphIndex* const charactersToGlyphsBuffer = parameters.textModel->mVisualModel->mCharactersToGlyph.Begin();

    const float      outlineWidth                = static_cast<float>(parameters.textModel->GetOutlineWidth());
    const GlyphIndex lastGlyphOfParagraphPlusOne = parameters.startGlyphIndex + parameters.numberOfGlyphs;
    const float      characterSpacing            = parameters.textModel->mVisualModel->GetCharacterSpacing();

    CharacterIndex characterLogicalIndex = 0u;
    CharacterIndex characterVisualIndex  = 0u;

    float calculatedAdvance = 0.f;

    // If there are characters in the second half of Line then the first visual index mapped from visualToLogicalMapSecondHalf
    // Otherwise maps the first visual index from visualToLogicalMap.
    // This is to initialize the first visual index.
    if(bidirectionalLineInfo.characterRunForSecondHalfLine.numberOfCharacters > 0u)
    {
      characterVisualIndex = bidirectionalLineInfo.characterRunForSecondHalfLine.characterIndex + *(bidirectionalLineInfo.visualToLogicalMapSecondHalf + characterLogicalIndex);
    }
    else
    {
      characterVisualIndex = bidirectionalLineInfo.characterRun.characterIndex + *(bidirectionalLineInfo.visualToLogicalMap + characterLogicalIndex);
    }

    bool extendedToSecondHalf = false; // Whether the logical index is extended to second half

    if(RTL == bidirectionalLineInfo.direction)
    {
      // If there are characters in the second half of Line.
      if(bidirectionalLineInfo.characterRunForSecondHalfLine.numberOfCharacters > 0u)
      {
        // Keep adding the WhiteSpaces to the whiteSpaceLengthEndOfLine
        while(TextAbstraction::IsWhiteSpace(*(textBuffer + characterVisualIndex)))
        {
          const GlyphInfo& glyphInfo = *(glyphsBuffer + *(charactersToGlyphsBuffer + characterVisualIndex));

          calculatedAdvance = GetCalculatedAdvance(*(textBuffer + characterVisualIndex), characterSpacing, glyphInfo.advance);
          whiteSpaceLengthEndOfLine += calculatedAdvance;

          ++characterLogicalIndex;
          characterVisualIndex = bidirectionalLineInfo.characterRunForSecondHalfLine.characterIndex + *(bidirectionalLineInfo.visualToLogicalMapSecondHalf + characterLogicalIndex);
        }
      }

      // If all characters in the second half of Line are WhiteSpaces.
      // then continue adding the WhiteSpaces from the first hel of Line.
      // Also this is valid when the line was not splitted.
      if(characterLogicalIndex == bidirectionalLineInfo.characterRunForSecondHalfLine.numberOfCharacters)
      {
        extendedToSecondHalf  = true; // Whether the logical index is extended to second half
        characterLogicalIndex = 0u;
        characterVisualIndex  = bidirectionalLineInfo.characterRun.characterIndex + *(bidirectionalLineInfo.visualToLogicalMap + characterLogicalIndex);

        // Keep adding the WhiteSpaces to the whiteSpaceLengthEndOfLine
        while(TextAbstraction::IsWhiteSpace(*(textBuffer + characterVisualIndex)))
        {
          const GlyphInfo& glyphInfo = *(glyphsBuffer + *(charactersToGlyphsBuffer + characterVisualIndex));

          calculatedAdvance = GetCalculatedAdvance(*(textBuffer + characterVisualIndex), characterSpacing, glyphInfo.advance);
          whiteSpaceLengthEndOfLine += calculatedAdvance;

          ++characterLogicalIndex;
          characterVisualIndex = bidirectionalLineInfo.characterRun.characterIndex + *(bidirectionalLineInfo.visualToLogicalMap + characterLogicalIndex);
        }
      }
    }

    // Here's the first index of character is not WhiteSpace
    const GlyphIndex glyphIndex = *(charactersToGlyphsBuffer + characterVisualIndex);

    // Check whether the first glyph comes from a character that is shaped in multiple glyphs.
    const Length numberOfGLyphsInGroup = GetNumberOfGlyphsOfGroup(glyphIndex,
                                                                  lastGlyphOfParagraphPlusOne,
                                                                  charactersPerGlyphBuffer);

    GlyphMetrics glyphMetrics;
    calculatedAdvance = GetCalculatedAdvance(*(textBuffer + characterVisualIndex), characterSpacing, (*(glyphsBuffer + glyphIndex)).advance);
    GetGlyphsMetrics(glyphIndex,
                     numberOfGLyphsInGroup,
                     glyphMetrics,
                     glyphsBuffer,
                     mMetrics,
                     calculatedAdvance);

    float penX = -glyphMetrics.xBearing + mCursorWidth + outlineWidth;

    // Traverses the characters of the right to left paragraph.
    // Continue in the second half of line, because in it the first index of character that is not WhiteSpace.
    if(!extendedToSecondHalf &&
       bidirectionalLineInfo.characterRunForSecondHalfLine.numberOfCharacters > 0u)
    {
      for(; characterLogicalIndex < bidirectionalLineInfo.characterRunForSecondHalfLine.numberOfCharacters;)
      {
        // Convert the character in the logical order into the character in the visual order.
        const CharacterIndex characterVisualIndex = bidirectionalLineInfo.characterRunForSecondHalfLine.characterIndex + *(bidirectionalLineInfo.visualToLogicalMapSecondHalf + characterLogicalIndex);
        const bool           isWhiteSpace         = TextAbstraction::IsWhiteSpace(*(textBuffer + characterVisualIndex));

        const GlyphIndex glyphIndex = *(charactersToGlyphsBuffer + characterVisualIndex);

        // Check whether this glyph comes from a character that is shaped in multiple glyphs.
        const Length numberOfGLyphsInGroup = GetNumberOfGlyphsOfGroup(glyphIndex,
                                                                      lastGlyphOfParagraphPlusOne,
                                                                      charactersPerGlyphBuffer);

        characterLogicalIndex += *(charactersPerGlyphBuffer + glyphIndex + numberOfGLyphsInGroup - 1u);

        GlyphMetrics glyphMetrics;
        calculatedAdvance = GetCalculatedAdvance(*(textBuffer + characterVisualIndex), characterSpacing, (*(glyphsBuffer + glyphIndex)).advance);
        GetGlyphsMetrics(glyphIndex,
                         numberOfGLyphsInGroup,
                         glyphMetrics,
                         glyphsBuffer,
                         mMetrics,
                         calculatedAdvance);

        if(isWhiteSpace)
        {
          // If glyph is WhiteSpace then:
          // For RTL it is whitespace but not at endOfLine. Use "advance" to accumulate length and shift penX.
          // the endOfLine in RTL was the headOfLine for layouting.
          // But for LTR added it to the endOfLine and use "advance" to accumulate length.
          if(RTL == bidirectionalLineInfo.direction)
          {
            length += glyphMetrics.advance;
          }
          else
          {
            whiteSpaceLengthEndOfLine += glyphMetrics.advance;
          }
          penX += glyphMetrics.advance;
        }
        else
        {
          // If glyph is not whiteSpace then:
          // Reset endOfLine for LTR because there is a non-whiteSpace so the tail of line is not whiteSpaces
          // Use "advance" and "interGlyphExtraAdvance" to shift penX.
          // Set length to the maximum possible length, of the current glyph "xBearing" and "width" are shifted penX to length greater than current lenght.
          // Otherwise the current length is maximum.
          if(LTR == bidirectionalLineInfo.direction)
          {
            whiteSpaceLengthEndOfLine = 0.f;
          }
          length = std::max(length, penX + glyphMetrics.xBearing + glyphMetrics.width);
          penX += (glyphMetrics.advance + parameters.interGlyphExtraAdvance);
        }
      }
    }

    // Continue traversing in the first half of line or in the whole line.
    // If the second half of line was extended then continue from logical index in the first half of line
    // Also this is valid when the line was not splitted and there were WhiteSpace.
    // Otherwise start from first logical index in line.
    characterLogicalIndex = extendedToSecondHalf ? characterLogicalIndex : 0u;
    for(; characterLogicalIndex < bidirectionalLineInfo.characterRun.numberOfCharacters;)
    {
      // Convert the character in the logical order into the character in the visual order.
      const CharacterIndex characterVisualIndex = bidirectionalLineInfo.characterRun.characterIndex + *(bidirectionalLineInfo.visualToLogicalMap + characterLogicalIndex);
      const bool           isWhiteSpace         = TextAbstraction::IsWhiteSpace(*(textBuffer + characterVisualIndex));

      const GlyphIndex glyphIndex = *(charactersToGlyphsBuffer + characterVisualIndex);

      // Check whether this glyph comes from a character that is shaped in multiple glyphs.
      const Length numberOfGLyphsInGroup = GetNumberOfGlyphsOfGroup(glyphIndex,
                                                                    lastGlyphOfParagraphPlusOne,
                                                                    charactersPerGlyphBuffer);

      characterLogicalIndex += *(charactersPerGlyphBuffer + glyphIndex + numberOfGLyphsInGroup - 1u);

      GlyphMetrics glyphMetrics;
      calculatedAdvance = GetCalculatedAdvance(*(textBuffer + characterVisualIndex), characterSpacing, (*(glyphsBuffer + glyphIndex)).advance);
      GetGlyphsMetrics(glyphIndex,
                       numberOfGLyphsInGroup,
                       glyphMetrics,
                       glyphsBuffer,
                       mMetrics,
                       calculatedAdvance);

      if(isWhiteSpace)
      {
        // If glyph is WhiteSpace then:
        // For RTL it is whitespace but not at endOfLine. Use "advance" to accumulate length and shift penX.
        // the endOfLine in RTL was the headOfLine for layouting.
        // But for LTR added it to the endOfLine and use "advance" to accumulate length.
        if(RTL == bidirectionalLineInfo.direction)
        {
          length += glyphMetrics.advance;
        }
        else
        {
          whiteSpaceLengthEndOfLine += glyphMetrics.advance;
        }
        penX += glyphMetrics.advance;
      }
      else
      {
        // If glyph is not whiteSpace then:
        // Reset endOfLine for LTR because there is a non-whiteSpace so the tail of line is not whiteSpaces
        // Use "advance" and "interGlyphExtraAdvance" to shift penX.
        // Set length to the maximum possible length, of the current glyph "xBearing" and "width" are shifted penX to length greater than current lenght.
        // Otherwise the current length is maximum.
        if(LTR == bidirectionalLineInfo.direction)
        {
          whiteSpaceLengthEndOfLine = 0.f;
        }
        length = std::max(length, penX + glyphMetrics.xBearing + glyphMetrics.width);
        penX += (glyphMetrics.advance + parameters.interGlyphExtraAdvance);
      }
    }
  }

  void ReorderBiDiLayout(const Parameters&     parameters,
                         LayoutBidiParameters& bidiParameters,
                         const LineLayout&     currentLineLayout,
                         LineLayout&           lineLayout,
                         bool                  breakInCharacters,
                         bool                  enforceEllipsisInSingleLine)
  {
    const Length* const charactersPerGlyphBuffer = parameters.textModel->mVisualModel->mCharactersPerGlyph.Begin();

    // The last glyph to be laid-out.
    const GlyphIndex lastGlyphOfParagraphPlusOne = parameters.startGlyphIndex + parameters.numberOfGlyphs;

    const Vector<BidirectionalParagraphInfoRun>& bidirectionalParagraphsInfo = parameters.textModel->mLogicalModel->mBidirectionalParagraphInfo;

    const BidirectionalParagraphInfoRun& bidirectionalParagraphInfo = bidirectionalParagraphsInfo[bidiParameters.bidiParagraphIndex];
    if((lineLayout.characterIndex >= bidirectionalParagraphInfo.characterRun.characterIndex) &&
       (lineLayout.characterIndex < bidirectionalParagraphInfo.characterRun.characterIndex + bidirectionalParagraphInfo.characterRun.numberOfCharacters))
    {
      Vector<BidirectionalLineInfoRun>& bidirectionalLinesInfo = parameters.textModel->mLogicalModel->mBidirectionalLineInfo;

      // Sets the visual to logical map tables needed to reorder the text.
      ReorderLine(bidirectionalParagraphInfo,
                  bidirectionalLinesInfo,
                  bidiParameters.bidiLineIndex,
                  lineLayout.characterIndex,
                  lineLayout.numberOfCharacters,
                  lineLayout.characterIndexInSecondHalfLine,
                  lineLayout.numberOfCharactersInSecondHalfLine,
                  bidiParameters.paragraphDirection);

      // Recalculate the length of the line and update the layout.
      const BidirectionalLineInfoRun& bidirectionalLineInfo = *(bidirectionalLinesInfo.Begin() + bidiParameters.bidiLineIndex);

      if(!bidirectionalLineInfo.isIdentity)
      {
        float length                    = 0.f;
        float whiteSpaceLengthEndOfLine = 0.f;
        LayoutRightToLeft(parameters,
                          bidirectionalLineInfo,
                          length,
                          whiteSpaceLengthEndOfLine);

        lineLayout.whiteSpaceLengthEndOfLine = whiteSpaceLengthEndOfLine;
        if(!Equals(length, lineLayout.length))
        {
          const bool isMultiline = (!enforceEllipsisInSingleLine) && (mLayout == MULTI_LINE_BOX);

          if(isMultiline && (length > parameters.boundingBox.width))
          {
            if(breakInCharacters || (isMultiline && (0u == currentLineLayout.numberOfGlyphs)))
            {
              // The word doesn't fit in one line. It has to be split by character.

              // Remove the last laid out glyph(s) as they doesn't fit.
              for(GlyphIndex glyphIndex = lineLayout.glyphIndex + lineLayout.numberOfGlyphs - 1u; glyphIndex >= lineLayout.glyphIndex;)
              {
                const Length numberOfGLyphsInGroup = GetNumberOfGlyphsOfGroup(glyphIndex,
                                                                              lastGlyphOfParagraphPlusOne,
                                                                              charactersPerGlyphBuffer);

                const Length numberOfCharacters = *(charactersPerGlyphBuffer + glyphIndex + numberOfGLyphsInGroup - 1u);

                lineLayout.numberOfGlyphs -= numberOfGLyphsInGroup;
                lineLayout.numberOfCharacters -= numberOfCharacters;

                AdjustLayout(parameters,
                             bidiParameters,
                             bidirectionalParagraphInfo,
                             lineLayout);

                if(lineLayout.length < parameters.boundingBox.width)
                {
                  break;
                }

                if(glyphIndex < numberOfGLyphsInGroup)
                {
                  // avoids go under zero for an unsigned int.
                  break;
                }

                glyphIndex -= numberOfGLyphsInGroup;
              }
            }
            else
            {
              lineLayout = currentLineLayout;

              AdjustLayout(parameters,
                           bidiParameters,
                           bidirectionalParagraphInfo,
                           lineLayout);
            }
          }
          else
          {
            lineLayout.length = std::max(length, lineLayout.length);
          }
        }
      }
    }
  }

  void AdjustLayout(const Parameters&                    parameters,
                    LayoutBidiParameters&                bidiParameters,
                    const BidirectionalParagraphInfoRun& bidirectionalParagraphInfo,
                    LineLayout&                          lineLayout)
  {
    Vector<BidirectionalLineInfoRun>& bidirectionalLinesInfo = parameters.textModel->mLogicalModel->mBidirectionalLineInfo;

    // Remove current reordered line.
    bidirectionalLinesInfo.Erase(bidirectionalLinesInfo.Begin() + bidiParameters.bidiLineIndex);

    // Re-build the conversion table without the removed glyphs.
    ReorderLine(bidirectionalParagraphInfo,
                bidirectionalLinesInfo,
                bidiParameters.bidiLineIndex,
                lineLayout.characterIndex,
                lineLayout.numberOfCharacters,
                lineLayout.characterIndexInSecondHalfLine,
                lineLayout.numberOfCharactersInSecondHalfLine,
                bidiParameters.paragraphDirection);

    const BidirectionalLineInfoRun& bidirectionalLineInfo = *(bidirectionalLinesInfo.Begin() + bidiParameters.bidiLineIndex);

    float length                    = 0.f;
    float whiteSpaceLengthEndOfLine = 0.f;
    LayoutRightToLeft(parameters,
                      bidirectionalLineInfo,
                      length,
                      whiteSpaceLengthEndOfLine);

    lineLayout.length                    = length;
    lineLayout.whiteSpaceLengthEndOfLine = whiteSpaceLengthEndOfLine;
  }

  /**
   * Retrieves the line layout for a given box width.
   *
   * @note This method starts to layout text as if it was left to right. However, it might be differences in the length
   *       of the line if it's a bidirectional one. If the paragraph is bidirectional, this method will call a function
   *       to reorder the line and recalculate its length.
   *

   * @param[in] parameters The layout parameters.
   * @param[] bidiParameters Bidirectional info for the current line.
   * @param[out] lineLayout The line layout.
   * @param[in] completelyFill Whether to completely fill the line ( even if the last word exceeds the boundaries ).
   * @param[in] ellipsisPosition Where is the location the text elide
   */
  void GetLineLayoutForBox(const Parameters&                 parameters,
                           LayoutBidiParameters&             bidiParameters,
                           LineLayout&                       lineLayout,
                           bool                              completelyFill,
                           DevelText::EllipsisPosition::Type ellipsisPosition,
                           bool                              enforceEllipsisInSingleLine,
                           bool                              elideTextEnabled)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->GetLineLayoutForBox\n");
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  initial glyph index : %d\n", lineLayout.glyphIndex);

    const Character* const      textBuffer               = parameters.textModel->mLogicalModel->mText.Begin();
    const Length* const         charactersPerGlyphBuffer = parameters.textModel->mVisualModel->mCharactersPerGlyph.Begin();
    const GlyphInfo* const      glyphsBuffer             = parameters.textModel->mVisualModel->mGlyphs.Begin();
    const CharacterIndex* const glyphsToCharactersBuffer = parameters.textModel->mVisualModel->mGlyphsToCharacters.Begin();
    const LineBreakInfo* const  lineBreakInfoBuffer      = parameters.textModel->mLogicalModel->mLineBreakInfo.Begin();

    const float  outlineWidth        = static_cast<float>(parameters.textModel->GetOutlineWidth());
    const Length totalNumberOfGlyphs = parameters.textModel->mVisualModel->mGlyphs.Count();

    const bool isMultiline   = !enforceEllipsisInSingleLine && (mLayout == MULTI_LINE_BOX);
    const bool isWordLaidOut = parameters.textModel->mLineWrapMode == Text::LineWrap::WORD ||
                               (parameters.textModel->mLineWrapMode == (Text::LineWrap::Mode)DevelText::LineWrap::HYPHENATION) ||
                               (parameters.textModel->mLineWrapMode == (Text::LineWrap::Mode)DevelText::LineWrap::MIXED);
    const bool isHyphenMode = parameters.textModel->mLineWrapMode == (Text::LineWrap::Mode)DevelText::LineWrap::HYPHENATION;
    const bool isMixedMode  = parameters.textModel->mLineWrapMode == (Text::LineWrap::Mode)DevelText::LineWrap::MIXED;

    const bool isSplitToTwoHalves = elideTextEnabled && !isMultiline && ellipsisPosition == DevelText::EllipsisPosition::MIDDLE;

    // The last glyph to be laid-out.
    const GlyphIndex lastGlyphOfParagraphPlusOne = parameters.startGlyphIndex + parameters.numberOfGlyphs;

    // If the first glyph has a negative bearing its absolute value needs to be added to the line length.
    // In the case the line starts with a right to left character, if the width is longer than the advance,
    // the difference needs to be added to the line length.

    // Check whether the first glyph comes from a character that is shaped in multiple glyphs.
    const Length numberOfGLyphsInGroup = GetNumberOfGlyphsOfGroup(lineLayout.glyphIndex,
                                                                  lastGlyphOfParagraphPlusOne,
                                                                  charactersPerGlyphBuffer);

    float targetWidth    = parameters.boundingBox.width;
    float widthFirstHalf = ((ellipsisPosition != DevelText::EllipsisPosition::MIDDLE) ? targetWidth : targetWidth - std::floor(targetWidth / 2));

    bool isSecondHalf = false;
    // Character Spacing
    const float             characterSpacing          = parameters.textModel->mVisualModel->GetCharacterSpacing();
    float                   calculatedAdvance         = 0.f;
    Vector<CharacterIndex>& glyphToCharacterMap       = parameters.textModel->mVisualModel->mGlyphsToCharacters;
    const CharacterIndex*   glyphToCharacterMapBuffer = glyphToCharacterMap.Begin();

    GlyphMetrics glyphMetrics;
    calculatedAdvance = GetCalculatedAdvance(*(textBuffer + (*(glyphToCharacterMapBuffer + lineLayout.glyphIndex))), characterSpacing, (*(glyphsBuffer + lineLayout.glyphIndex)).advance);
    GetGlyphsMetrics(lineLayout.glyphIndex,
                     numberOfGLyphsInGroup,
                     glyphMetrics,
                     glyphsBuffer,
                     mMetrics,
                     calculatedAdvance);

    // Set the direction of the first character of the line.
    lineLayout.characterIndex = *(glyphsToCharactersBuffer + lineLayout.glyphIndex);

    // Stores temporary line layout which has not been added to the final line layout.
    LineLayout tmpLineLayout;

    // Initialize the start point.

    // The initial start point is zero. However it needs a correction according the 'x' bearing of the first glyph.
    // i.e. if the bearing of the first glyph is negative it may exceed the boundaries of the text area.
    // It needs to add as well space for the cursor if the text is in edit mode and extra space in case the text is outlined.
    tmpLineLayout.penX = -glyphMetrics.xBearing + mCursorWidth + outlineWidth;

    tmpLineLayout.relativeLineSize = lineLayout.relativeLineSize;

    // Calculate the line height if there is no characters.
    FontId lastFontId = glyphMetrics.fontId;
    UpdateLineHeight(glyphMetrics, tmpLineLayout);

    bool       oneWordLaidOut   = false;
    bool       oneHyphenLaidOut = false;
    GlyphIndex hyphenIndex      = 0;
    GlyphInfo  hyphenGlyph;

    for(GlyphIndex glyphIndex = lineLayout.glyphIndex;
        glyphIndex < lastGlyphOfParagraphPlusOne;)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  glyph index : %d\n", glyphIndex);

      // Check whether this glyph comes from a character that is shaped in multiple glyphs.
      const Length numberOfGLyphsInGroup = GetNumberOfGlyphsOfGroup(glyphIndex,
                                                                    lastGlyphOfParagraphPlusOne,
                                                                    charactersPerGlyphBuffer);

      GlyphMetrics glyphMetrics;
      calculatedAdvance = GetCalculatedAdvance(*(textBuffer + (*(glyphToCharacterMapBuffer + glyphIndex))), characterSpacing, (*(glyphsBuffer + glyphIndex)).advance);
      GetGlyphsMetrics(glyphIndex,
                       numberOfGLyphsInGroup,
                       glyphMetrics,
                       glyphsBuffer,
                       mMetrics,
                       calculatedAdvance);

      const bool isLastGlyph = glyphIndex + numberOfGLyphsInGroup == totalNumberOfGlyphs;

      // Check if the font of the current glyph is the same of the previous one.
      // If it's different the ascender and descender need to be updated.
      if(lastFontId != glyphMetrics.fontId)
      {
        UpdateLineHeight(glyphMetrics, tmpLineLayout);
        lastFontId = glyphMetrics.fontId;
      }

      // Get the character indices for the current glyph. The last character index is needed
      // because there are glyphs formed by more than one character but their break info is
      // given only for the last character.
      const Length         charactersPerGlyph  = *(charactersPerGlyphBuffer + glyphIndex + numberOfGLyphsInGroup - 1u);
      const bool           hasCharacters       = charactersPerGlyph > 0u;
      const CharacterIndex characterFirstIndex = *(glyphsToCharactersBuffer + glyphIndex);
      const CharacterIndex characterLastIndex  = characterFirstIndex + (hasCharacters ? charactersPerGlyph - 1u : 0u);

      // Get the line break info for the current character.
      const LineBreakInfo lineBreakInfo = hasCharacters ? *(lineBreakInfoBuffer + characterLastIndex) : TextAbstraction::LINE_NO_BREAK;

      if(isSecondHalf)
      {
        // Increase the number of characters.
        tmpLineLayout.numberOfCharactersInSecondHalfLine += charactersPerGlyph;

        // Increase the number of glyphs.
        tmpLineLayout.numberOfGlyphsInSecondHalfLine += numberOfGLyphsInGroup;
      }
      else
      {
        // Increase the number of characters.
        tmpLineLayout.numberOfCharacters += charactersPerGlyph;

        // Increase the number of glyphs.
        tmpLineLayout.numberOfGlyphs += numberOfGLyphsInGroup;
      }

      // Check whether is a white space.
      const Character character    = *(textBuffer + characterFirstIndex);
      const bool      isWhiteSpace = TextAbstraction::IsWhiteSpace(character);

      // Calculate the length of the line.

      // Used to restore the temporal line layout when a single word does not fit in the control's width and is split by character.
      const float previousTmpPenX                      = tmpLineLayout.penX;
      const float previousTmpAdvance                   = tmpLineLayout.previousAdvance;
      const float previousTmpLength                    = tmpLineLayout.length;
      const float previousTmpWhiteSpaceLengthEndOfLine = tmpLineLayout.whiteSpaceLengthEndOfLine;

      if(isWhiteSpace)
      {
        // Add the length to the length of white spaces at the end of the line.
        tmpLineLayout.whiteSpaceLengthEndOfLine += glyphMetrics.advance;
        // The advance is used as the width is always zero for the white spaces.
      }
      else
      {
        tmpLineLayout.penX += tmpLineLayout.previousAdvance + tmpLineLayout.whiteSpaceLengthEndOfLine;
        tmpLineLayout.previousAdvance = (glyphMetrics.advance + parameters.interGlyphExtraAdvance);

        tmpLineLayout.length = std::max(tmpLineLayout.length, tmpLineLayout.penX + glyphMetrics.xBearing + glyphMetrics.width);

        // Clear the white space length at the end of the line.
        tmpLineLayout.whiteSpaceLengthEndOfLine = 0.f;
      }

      if(isSplitToTwoHalves && (!isSecondHalf) &&
         (tmpLineLayout.length + tmpLineLayout.whiteSpaceLengthEndOfLine > widthFirstHalf))
      {
        tmpLineLayout.numberOfCharacters -= charactersPerGlyph;
        tmpLineLayout.numberOfGlyphs -= numberOfGLyphsInGroup;

        tmpLineLayout.numberOfCharactersInSecondHalfLine += charactersPerGlyph;
        tmpLineLayout.numberOfGlyphsInSecondHalfLine += numberOfGLyphsInGroup;

        tmpLineLayout.glyphIndexInSecondHalfLine     = tmpLineLayout.glyphIndex + tmpLineLayout.numberOfGlyphs;
        tmpLineLayout.characterIndexInSecondHalfLine = tmpLineLayout.characterIndex + tmpLineLayout.numberOfCharacters;

        tmpLineLayout.isSplitToTwoHalves = isSecondHalf = true;
      }
      // Check if the accumulated length fits in the width of the box.
      if((ellipsisPosition == DevelText::EllipsisPosition::START ||
          (ellipsisPosition == DevelText::EllipsisPosition::MIDDLE && isSecondHalf)) &&
         completelyFill && !isMultiline &&
         (tmpLineLayout.length + tmpLineLayout.whiteSpaceLengthEndOfLine > targetWidth))
      {
        GlyphIndex glyphIndexToRemove = isSecondHalf ? tmpLineLayout.glyphIndexInSecondHalfLine : tmpLineLayout.glyphIndex;

        while(tmpLineLayout.length + tmpLineLayout.whiteSpaceLengthEndOfLine > targetWidth && glyphIndexToRemove < glyphIndex)
        {
          GlyphMetrics glyphMetrics;
          calculatedAdvance = GetCalculatedAdvance(*(textBuffer + (*(glyphToCharacterMapBuffer + glyphIndexToRemove))), characterSpacing, (*(glyphsBuffer + glyphIndexToRemove)).advance);
          GetGlyphsMetrics(glyphIndexToRemove,
                           numberOfGLyphsInGroup,
                           glyphMetrics,
                           glyphsBuffer,
                           mMetrics,
                           calculatedAdvance);

          const Length numberOfGLyphsInGroup = GetNumberOfGlyphsOfGroup(glyphIndexToRemove,
                                                                        lastGlyphOfParagraphPlusOne,
                                                                        charactersPerGlyphBuffer);

          const Length         charactersPerGlyph  = *(charactersPerGlyphBuffer + glyphIndexToRemove + numberOfGLyphsInGroup - 1u);
          const bool           hasCharacters       = charactersPerGlyph > 0u;
          const CharacterIndex characterFirstIndex = *(glyphsToCharactersBuffer + glyphIndexToRemove);
          const CharacterIndex characterLastIndex  = characterFirstIndex + (hasCharacters ? charactersPerGlyph - 1u : 0u);

          // Check whether is a white space.
          const Character character                = *(textBuffer + characterFirstIndex);
          const bool      isRemovedGlyphWhiteSpace = TextAbstraction::IsWhiteSpace(character);

          if(isSecondHalf)
          {
            // Decrease the number of characters for SecondHalf.
            tmpLineLayout.numberOfCharactersInSecondHalfLine -= charactersPerGlyph;

            // Decrease the number of glyphs for SecondHalf.
            tmpLineLayout.numberOfGlyphsInSecondHalfLine -= numberOfGLyphsInGroup;
          }
          else
          {
            // Decrease the number of characters.
            tmpLineLayout.numberOfCharacters -= charactersPerGlyph;

            // Decrease the number of glyphs.
            tmpLineLayout.numberOfGlyphs -= numberOfGLyphsInGroup;
          }

          if(isRemovedGlyphWhiteSpace)
          {
            tmpLineLayout.penX -= glyphMetrics.advance;
            tmpLineLayout.length -= glyphMetrics.advance;
          }
          else
          {
            tmpLineLayout.penX -= (glyphMetrics.advance + parameters.interGlyphExtraAdvance);
            tmpLineLayout.length -= (std::min(glyphMetrics.advance + parameters.interGlyphExtraAdvance, glyphMetrics.xBearing + glyphMetrics.width));
          }

          if(isSecondHalf)
          {
            tmpLineLayout.glyphIndexInSecondHalfLine += numberOfGLyphsInGroup;
            tmpLineLayout.characterIndexInSecondHalfLine = characterLastIndex + 1u;
            glyphIndexToRemove                           = tmpLineLayout.glyphIndexInSecondHalfLine;
          }
          else
          {
            tmpLineLayout.glyphIndex += numberOfGLyphsInGroup;
            tmpLineLayout.characterIndex = characterLastIndex + 1u;
            glyphIndexToRemove           = tmpLineLayout.glyphIndex;
          }
        }
      }
      else if((completelyFill || isMultiline) &&
              (tmpLineLayout.length > targetWidth))
      {
        // Current word does not fit in the box's width.
        if(((oneHyphenLaidOut && isHyphenMode) ||
            (!oneWordLaidOut && isMixedMode && oneHyphenLaidOut)) &&
           !completelyFill)
        {
          parameters.textModel->mVisualModel->mHyphen.glyph.PushBack(hyphenGlyph);
          parameters.textModel->mVisualModel->mHyphen.index.PushBack(hyphenIndex + 1);
        }

        if((!oneWordLaidOut && !oneHyphenLaidOut) || completelyFill)
        {
          DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  Break the word by character\n");

          // The word doesn't fit in the control's width. It needs to be split by character.
          if(tmpLineLayout.numberOfGlyphs + tmpLineLayout.numberOfGlyphsInSecondHalfLine > 0u)
          {
            if(isSecondHalf)
            {
              tmpLineLayout.numberOfCharactersInSecondHalfLine -= charactersPerGlyph;
              tmpLineLayout.numberOfGlyphsInSecondHalfLine -= numberOfGLyphsInGroup;
            }
            else
            {
              tmpLineLayout.numberOfCharacters -= charactersPerGlyph;
              tmpLineLayout.numberOfGlyphs -= numberOfGLyphsInGroup;
            }

            tmpLineLayout.penX                      = previousTmpPenX;
            tmpLineLayout.previousAdvance           = previousTmpAdvance;
            tmpLineLayout.length                    = previousTmpLength;
            tmpLineLayout.whiteSpaceLengthEndOfLine = previousTmpWhiteSpaceLengthEndOfLine;
          }

          if(ellipsisPosition == DevelText::EllipsisPosition::START && !isMultiline)
          {
            // Add part of the word to the line layout and shift the first glyph.
            MergeLineLayout(lineLayout, tmpLineLayout, true);
          }
          else if(ellipsisPosition != DevelText::EllipsisPosition::START ||
                  (ellipsisPosition == DevelText::EllipsisPosition::START && (!completelyFill)))
          {
            // Add part of the word to the line layout.
            MergeLineLayout(lineLayout, tmpLineLayout, false);
          }
        }
        else
        {
          DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  Current word does not fit.\n");
        }

        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--GetLineLayoutForBox.\n");

        // Reorder the RTL line.
        if(bidiParameters.isBidirectional)
        {
          ReorderBiDiLayout(parameters,
                            bidiParameters,
                            lineLayout,
                            lineLayout,
                            true,
                            enforceEllipsisInSingleLine);
        }

        return;
      }

      if((isMultiline || isLastGlyph) &&
         (TextAbstraction::LINE_MUST_BREAK == lineBreakInfo))
      {
        LineLayout currentLineLayout = lineLayout;
        oneHyphenLaidOut             = false;

        if(ellipsisPosition == DevelText::EllipsisPosition::START && !isMultiline)
        {
          // Must break the line. Update the line layout, shift the first glyph and return.
          MergeLineLayout(lineLayout, tmpLineLayout, true);
        }
        else
        {
          // Must break the line. Update the line layout and return.
          MergeLineLayout(lineLayout, tmpLineLayout, false);
        }

        // Reorder the RTL line.
        if(bidiParameters.isBidirectional)
        {
          ReorderBiDiLayout(parameters,
                            bidiParameters,
                            currentLineLayout,
                            lineLayout,
                            false,
                            enforceEllipsisInSingleLine);
        }

        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  Must break\n");
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--GetLineLayoutForBox\n");

        return;
      }

      if(isMultiline &&
         (TextAbstraction::LINE_ALLOW_BREAK == lineBreakInfo))
      {
        oneHyphenLaidOut = false;
        oneWordLaidOut   = isWordLaidOut;
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  One word laid-out\n");

        // Current glyph is the last one of the current word.
        // Add the temporal layout to the current one.
        MergeLineLayout(lineLayout, tmpLineLayout, false);

        tmpLineLayout.Clear();
      }

      if(isMultiline &&
         ((isHyphenMode || (!oneWordLaidOut && isMixedMode))) &&
         (TextAbstraction::LINE_HYPHENATION_BREAK == lineBreakInfo))
      {
        hyphenGlyph        = GlyphInfo();
        hyphenGlyph.fontId = glyphsBuffer[glyphIndex].fontId;

        TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
        hyphenGlyph.index                      = fontClient.GetGlyphIndex(hyphenGlyph.fontId, HYPHEN_UNICODE);

        mMetrics->GetGlyphMetrics(&hyphenGlyph, 1);

        if((tmpLineLayout.length + hyphenGlyph.width) <= targetWidth)
        {
          hyphenIndex      = glyphIndex;
          oneHyphenLaidOut = true;

          DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  One hyphen laid-out\n");

          // Current glyph is the last one of the current word hyphen.
          // Add the temporal layout to the current one.
          MergeLineLayout(lineLayout, tmpLineLayout, false);

          tmpLineLayout.Clear();
        }
      }

      glyphIndex += numberOfGLyphsInGroup;
    }

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--GetLineLayoutForBox\n");
  }

  void SetGlyphPositions(const Parameters& layoutParameters,
                         Vector2*          glyphPositionsBuffer,
                         const LineLayout& layout)
  {
    // Traverse the glyphs and set the positions.

    const GlyphInfo* const glyphsBuffer           = layoutParameters.textModel->mVisualModel->mGlyphs.Begin();
    const float            outlineWidth           = static_cast<float>(layoutParameters.textModel->GetOutlineWidth());
    const Length           numberOfGlyphs         = layout.numberOfGlyphs;
    const float            interGlyphExtraAdvance = layoutParameters.interGlyphExtraAdvance;

    const GlyphIndex startIndexForGlyph          = layout.glyphIndex;
    const GlyphIndex startIndexForGlyphPositions = startIndexForGlyph - layoutParameters.startGlyphIndex;

    // Check if the x bearing of the first character is negative.
    // If it has a negative x bearing, it will exceed the boundaries of the actor,
    // so the penX position needs to be moved to the right.
    const GlyphInfo& glyph = *(glyphsBuffer + startIndexForGlyph);
    float            penX  = -glyph.xBearing + mCursorWidth + outlineWidth; //

    CalculateGlyphPositionsLTR(layoutParameters.textModel->mVisualModel,
                               layoutParameters.textModel->mLogicalModel,
                               interGlyphExtraAdvance,
                               numberOfGlyphs,
                               startIndexForGlyph, // startIndexForGlyph is the index of the first glyph in the line
                               startIndexForGlyphPositions,
                               glyphPositionsBuffer,
                               penX);

    if(layout.isSplitToTwoHalves)
    {
      const GlyphIndex startIndexForGlyphInSecondHalf         = layout.glyphIndexInSecondHalfLine;
      const Length     numberOfGlyphsInSecondHalfLine         = layout.numberOfGlyphsInSecondHalfLine;
      const GlyphIndex startIndexForGlyphPositionsnSecondHalf = layout.glyphIndexInSecondHalfLine - layoutParameters.startGlyphIndex;

      CalculateGlyphPositionsLTR(layoutParameters.textModel->mVisualModel,
                                 layoutParameters.textModel->mLogicalModel,
                                 interGlyphExtraAdvance,
                                 numberOfGlyphsInSecondHalfLine,
                                 startIndexForGlyphInSecondHalf, // startIndexForGlyph is the index of the first glyph in the line
                                 startIndexForGlyphPositionsnSecondHalf,
                                 glyphPositionsBuffer,
                                 penX);
    }
  }

  void SetGlyphPositions(const Parameters&     layoutParameters,
                         Vector2*              glyphPositionsBuffer,
                         LayoutBidiParameters& layoutBidiParameters,
                         const LineLayout&     layout)
  {
    const BidirectionalLineInfoRun& bidiLine                 = layoutParameters.textModel->mLogicalModel->mBidirectionalLineInfo[layoutBidiParameters.bidiLineIndex];
    const GlyphInfo* const          glyphsBuffer             = layoutParameters.textModel->mVisualModel->mGlyphs.Begin();
    const GlyphIndex* const         charactersToGlyphsBuffer = layoutParameters.textModel->mVisualModel->mCharactersToGlyph.Begin();

    CharacterIndex characterLogicalIndex = 0u;
    CharacterIndex characterVisualIndex  = bidiLine.characterRunForSecondHalfLine.characterIndex + *(bidiLine.visualToLogicalMapSecondHalf + characterLogicalIndex);
    bool           extendedToSecondHalf  = false; // Whether the logical index is extended to second half

    float penX = 0.f;

    if(layout.isSplitToTwoHalves)
    {
      CalculateGlyphPositionsRTL(layoutParameters.textModel->mVisualModel,
                                 layoutParameters.textModel->mLogicalModel,
                                 layoutBidiParameters.bidiLineIndex,
                                 layoutParameters.startGlyphIndex,
                                 glyphPositionsBuffer,
                                 characterVisualIndex,
                                 characterLogicalIndex,
                                 penX);
    }

    if(characterLogicalIndex == bidiLine.characterRunForSecondHalfLine.numberOfCharacters)
    {
      extendedToSecondHalf  = true;
      characterLogicalIndex = 0u;
      characterVisualIndex  = bidiLine.characterRun.characterIndex + *(bidiLine.visualToLogicalMap + characterLogicalIndex);

      CalculateGlyphPositionsRTL(layoutParameters.textModel->mVisualModel,
                                 layoutParameters.textModel->mLogicalModel,
                                 layoutBidiParameters.bidiLineIndex,
                                 layoutParameters.startGlyphIndex,
                                 glyphPositionsBuffer,
                                 characterVisualIndex,
                                 characterLogicalIndex,
                                 penX);
    }

    const GlyphIndex glyphIndex = *(charactersToGlyphsBuffer + characterVisualIndex);
    const GlyphInfo& glyph      = *(glyphsBuffer + glyphIndex);

    penX += -glyph.xBearing;

    // Traverses the characters of the right to left paragraph.
    if(layout.isSplitToTwoHalves && !extendedToSecondHalf)
    {
      TraversesCharactersForGlyphPositionsRTL(layoutParameters.textModel->mVisualModel,
                                              layoutParameters.textModel->mLogicalModel->mText.Begin(),
                                              layoutParameters.startGlyphIndex,
                                              layoutParameters.interGlyphExtraAdvance,
                                              bidiLine.characterRunForSecondHalfLine,
                                              bidiLine.visualToLogicalMapSecondHalf,
                                              glyphPositionsBuffer,
                                              characterLogicalIndex,
                                              penX);
    }

    characterLogicalIndex = extendedToSecondHalf ? characterLogicalIndex : 0u;

    TraversesCharactersForGlyphPositionsRTL(layoutParameters.textModel->mVisualModel,
                                            layoutParameters.textModel->mLogicalModel->mText.Begin(),
                                            layoutParameters.startGlyphIndex,
                                            layoutParameters.interGlyphExtraAdvance,
                                            bidiLine.characterRun,
                                            bidiLine.visualToLogicalMap,
                                            glyphPositionsBuffer,
                                            characterLogicalIndex,
                                            penX);
  }

  /**
   * @brief Resizes the line buffer.
   *
   * @param[in,out] lines The vector of lines. Used when the layout is created from scratch.
   * @param[in,out] newLines The vector of lines used instead of @p lines when the layout is updated.
   * @param[in,out] linesCapacity The capacity of the vector (either lines or newLines).
   * @param[in] updateCurrentBuffer Whether the layout is updated.
   *
   * @return Pointer to either lines or newLines.
   */
  LineRun* ResizeLinesBuffer(Vector<LineRun>& lines,
                             Vector<LineRun>& newLines,
                             Length&          linesCapacity,
                             bool             updateCurrentBuffer)
  {
    LineRun* linesBuffer = nullptr;
    // Reserve more space for the next lines.
    linesCapacity *= 2u;
    if(updateCurrentBuffer)
    {
      newLines.Resize(linesCapacity);
      linesBuffer = newLines.Begin();
    }
    else
    {
      lines.Resize(linesCapacity);
      linesBuffer = lines.Begin();
    }

    return linesBuffer;
  }

  /**
   * Ellipsis a line if it exceeds the width's of the bounding box.
   *
   * @param[in] layoutParameters The parameters needed to layout the text.
   * @param[in] layout The line layout.
   * @param[in,out] layoutSize The text's layout size.
   * @param[in,out] linesBuffer Pointer to the line's buffer.
   * @param[in,out] glyphPositionsBuffer Pointer to the position's buffer.
   * @param[in,out] numberOfLines The number of laid-out lines.
   * @param[in] penY The vertical layout position.
   * @param[in] currentParagraphDirection The current paragraph's direction.
   * @param[in,out] isAutoScrollEnabled If the isAutoScrollEnabled is true and the height of the text exceeds the boundaries of the control the text is elided and the isAutoScrollEnabled is set to false to disable the autoscroll
   * @param[in] ellipsisPosition Where is the location the text elide
   *
   * return Whether the line is ellipsized.
   */
  bool EllipsisLine(const Parameters&                 layoutParameters,
                    LayoutBidiParameters&             layoutBidiParameters,
                    const LineLayout&                 layout,
                    Size&                             layoutSize,
                    LineRun*                          linesBuffer,
                    Vector2*                          glyphPositionsBuffer,
                    Length&                           numberOfLines,
                    float                             penY,
                    bool&                             isAutoScrollEnabled,
                    DevelText::EllipsisPosition::Type ellipsisPosition,
                    bool                              enforceEllipsisInSingleLine)
  {
    const bool ellipsis    = enforceEllipsisInSingleLine || (isAutoScrollEnabled ? (penY - layout.descender > layoutParameters.boundingBox.height) : ((penY - layout.descender > layoutParameters.boundingBox.height) || ((mLayout == SINGLE_LINE_BOX) && (layout.length > layoutParameters.boundingBox.width))));
    const bool isMultiline = !enforceEllipsisInSingleLine && (mLayout == MULTI_LINE_BOX);
    if(ellipsis && (ellipsisPosition == DevelText::EllipsisPosition::END || !isMultiline))
    {
      isAutoScrollEnabled = false;
      // Do not layout more lines if ellipsis is enabled.

      // The last line needs to be completely filled with characters.
      // Part of a word may be used.

      LineRun*   lineRun = nullptr;
      LineLayout ellipsisLayout;

      ellipsisLayout.relativeLineSize = layout.relativeLineSize;

      if(0u != numberOfLines)
      {
        // Get the last line and layout it again with the 'completelyFill' flag to true.
        lineRun = linesBuffer + (numberOfLines - 1u);
        penY -= layout.ascender - lineRun->descender + lineRun->lineSpacing;

        ellipsisLayout.glyphIndex = lineRun->glyphRun.glyphIndex;
      }
      else
      {
        // At least there is space reserved for one line.
        lineRun = linesBuffer;

        lineRun->glyphRun.glyphIndex = 0u;
        ellipsisLayout.glyphIndex    = 0u;
        lineRun->isSplitToTwoHalves  = false;

        ++numberOfLines;
      }

      GetLineLayoutForBox(layoutParameters,
                          layoutBidiParameters,
                          ellipsisLayout,
                          true,
                          ellipsisPosition,
                          enforceEllipsisInSingleLine,
                          true);

      if(ellipsisPosition == DevelText::EllipsisPosition::START && !isMultiline)
      {
        lineRun->glyphRun.glyphIndex = ellipsisLayout.glyphIndex;
      }

      lineRun->glyphRun.numberOfGlyphs         = ellipsisLayout.numberOfGlyphs;
      lineRun->characterRun.characterIndex     = ellipsisLayout.characterIndex;
      lineRun->characterRun.numberOfCharacters = ellipsisLayout.numberOfCharacters;
      lineRun->width                           = ellipsisLayout.length;
      lineRun->extraLength                     = std::ceil(ellipsisLayout.whiteSpaceLengthEndOfLine);
      lineRun->ascender                        = ellipsisLayout.ascender;
      lineRun->descender                       = ellipsisLayout.descender;
      lineRun->ellipsis                        = true;

      lineRun->isSplitToTwoHalves                               = ellipsisLayout.isSplitToTwoHalves;
      lineRun->glyphRunSecondHalf.glyphIndex                    = ellipsisLayout.glyphIndexInSecondHalfLine;
      lineRun->glyphRunSecondHalf.numberOfGlyphs                = ellipsisLayout.numberOfGlyphsInSecondHalfLine;
      lineRun->characterRunForSecondHalfLine.characterIndex     = ellipsisLayout.characterIndexInSecondHalfLine;
      lineRun->characterRunForSecondHalfLine.numberOfCharacters = ellipsisLayout.numberOfCharactersInSecondHalfLine;

      layoutSize.width = layoutParameters.boundingBox.width;
      if(layoutSize.height < Math::MACHINE_EPSILON_1000)
      {
        layoutSize.height += GetLineHeight(*lineRun, true);
      }
      else
      {
        //when we apply ellipsis, the last line should not take negative linespacing into account for layoutSize.height calculation
        //usually we don't includ it in normal cases using GetLineHeight()
        if(lineRun->lineSpacing < 0)
        {
          layoutSize.height -= lineRun->lineSpacing;
        }
      }

      const Vector<BidirectionalLineInfoRun>& bidirectionalLinesInfo = layoutParameters.textModel->mLogicalModel->mBidirectionalLineInfo;

      if(layoutBidiParameters.isBidirectional)
      {
        layoutBidiParameters.bidiLineIndex = 0u;
        for(Vector<BidirectionalLineInfoRun>::ConstIterator it    = bidirectionalLinesInfo.Begin(),
                                                            endIt = bidirectionalLinesInfo.End();
            it != endIt;
            ++it, ++layoutBidiParameters.bidiLineIndex)
        {
          const BidirectionalLineInfoRun& run = *it;
          //To handle case when the laid characters exist in next line.
          //More than one BidirectionalLineInfoRun could start with same character.
          //When need to check also numberOfCharacters in line.
          //Note: This fixed the incorrect view of extra spaces of RTL as in Arabic then view ellipsis glyph
          if(ellipsisLayout.characterIndex == run.characterRun.characterIndex &&
             ellipsisLayout.numberOfCharacters == run.characterRun.numberOfCharacters &&
             ellipsisLayout.characterIndexInSecondHalfLine == run.characterRunForSecondHalfLine.characterIndex &&
             ellipsisLayout.numberOfCharactersInSecondHalfLine == run.characterRunForSecondHalfLine.numberOfCharacters)
          {
            // Found where to insert the bidi line info.
            break;
          }
        }
      }

      const BidirectionalLineInfoRun* const bidirectionalLineInfo = (layoutBidiParameters.isBidirectional && !bidirectionalLinesInfo.Empty()) ? &bidirectionalLinesInfo[layoutBidiParameters.bidiLineIndex] : nullptr;

      if((nullptr != bidirectionalLineInfo) &&
         !bidirectionalLineInfo->isIdentity &&
         (ellipsisLayout.characterIndex == bidirectionalLineInfo->characterRun.characterIndex))
      {
        lineRun->direction = RTL;
        SetGlyphPositions(layoutParameters,
                          glyphPositionsBuffer,
                          layoutBidiParameters,
                          ellipsisLayout);
      }
      else
      {
        lineRun->direction = LTR;
        SetGlyphPositions(layoutParameters,
                          glyphPositionsBuffer,
                          ellipsisLayout);
      }
    }

    return ellipsis;
  }

  /**
   * @brief Updates the text layout with a new laid-out line.
   *
   * @param[in] layoutParameters The parameters needed to layout the text.
   * @param[in] layout The line layout.
   * @param[in,out] layoutSize The text's layout size.
   * @param[in,out] linesBuffer Pointer to the line's buffer.
   * @param[in] index Index to the vector of glyphs.
   * @param[in,out] numberOfLines The number of laid-out lines.
   * @param[in] isLastLine Whether the laid-out line is the last one.
   */
  void UpdateTextLayout(const Parameters& layoutParameters,
                        const LineLayout& layout,
                        Size&             layoutSize,
                        LineRun*          linesBuffer,
                        GlyphIndex        index,
                        Length&           numberOfLines,
                        bool              isLastLine)
  {
    LineRun& lineRun = *(linesBuffer + numberOfLines);
    ++numberOfLines;

    lineRun.glyphRun.glyphIndex             = index;
    lineRun.glyphRun.numberOfGlyphs         = layout.numberOfGlyphs;
    lineRun.characterRun.characterIndex     = layout.characterIndex;
    lineRun.characterRun.numberOfCharacters = layout.numberOfCharacters;
    lineRun.width                           = layout.length;
    lineRun.extraLength                     = std::ceil(layout.whiteSpaceLengthEndOfLine);

    lineRun.isSplitToTwoHalves                               = layout.isSplitToTwoHalves;
    lineRun.glyphRunSecondHalf.glyphIndex                    = layout.glyphIndexInSecondHalfLine;
    lineRun.glyphRunSecondHalf.numberOfGlyphs                = layout.numberOfGlyphsInSecondHalfLine;
    lineRun.characterRunForSecondHalfLine.characterIndex     = layout.characterIndexInSecondHalfLine;
    lineRun.characterRunForSecondHalfLine.numberOfCharacters = layout.numberOfCharactersInSecondHalfLine;

    // Rounds upward to avoid a non integer size.
    lineRun.width = std::ceil(lineRun.width);

    lineRun.ascender  = layout.ascender;
    lineRun.descender = layout.descender;
    lineRun.direction = layout.direction;
    lineRun.ellipsis  = false;

    lineRun.lineSpacing = GetLineSpacing(lineRun.ascender + -lineRun.descender, layout.relativeLineSize);

    // Update the actual size.
    if(lineRun.width > layoutSize.width)
    {
      layoutSize.width = lineRun.width;
    }

    layoutSize.height += GetLineHeight(lineRun, isLastLine);
  }

  /**
   * @brief Updates the text layout with the last laid-out line.
   *
   * @param[in] layoutParameters The parameters needed to layout the text.
   * @param[in] characterIndex The character index of the line.
   * @param[in] glyphIndex The glyph index of the line.
   * @param[in,out] layoutSize The text's layout size.
   * @param[in,out] linesBuffer Pointer to the line's buffer.
   * @param[in,out] numberOfLines The number of laid-out lines.
   */
  void UpdateTextLayout(const Parameters& layoutParameters,
                        CharacterIndex    characterIndex,
                        GlyphIndex        glyphIndex,
                        Size&             layoutSize,
                        LineRun*          linesBuffer,
                        Length&           numberOfLines)
  {
    const Vector<GlyphInfo>& glyphs = layoutParameters.textModel->mVisualModel->mGlyphs;

    // Need to add a new line with no characters but with height to increase the layoutSize.height
    const GlyphInfo& glyphInfo = glyphs[glyphs.Count() - 1u];

    Text::FontMetrics fontMetrics;
    if(0u != glyphInfo.fontId)
    {
      mMetrics->GetFontMetrics(glyphInfo.fontId, fontMetrics);
    }

    LineRun& lineRun = *(linesBuffer + numberOfLines);
    ++numberOfLines;

    lineRun.glyphRun.glyphIndex             = glyphIndex;
    lineRun.glyphRun.numberOfGlyphs         = 0u;
    lineRun.characterRun.characterIndex     = characterIndex;
    lineRun.characterRun.numberOfCharacters = 0u;
    lineRun.width                           = 0.f;
    lineRun.ascender                        = fontMetrics.ascender;
    lineRun.descender                       = fontMetrics.descender;
    lineRun.extraLength                     = 0.f;
    lineRun.alignmentOffset                 = 0.f;
    lineRun.direction                       = LTR;
    lineRun.ellipsis                        = false;

    BoundedParagraphRun currentParagraphRun;
    LineLayout          tempLineLayout;
    (GetBoundedParagraph(layoutParameters.textModel->GetBoundedParagraphRuns(), characterIndex, currentParagraphRun) ? SetRelativeLineSize(&currentParagraphRun, tempLineLayout) : SetRelativeLineSize(nullptr, tempLineLayout));

    lineRun.lineSpacing = GetLineSpacing(lineRun.ascender + -lineRun.descender, tempLineLayout.relativeLineSize);

    layoutSize.height += GetLineHeight(lineRun, true);
  }

  /**
   * @brief Updates the text's layout size adding the size of the previously laid-out lines.
   *
   * @param[in] lines The vector of lines (before the new laid-out lines are inserted).
   * @param[in,out] layoutSize The text's layout size.
   */
  void UpdateLayoutSize(const Vector<LineRun>& lines,
                        Size&                  layoutSize)
  {
    for(Vector<LineRun>::ConstIterator it    = lines.Begin(),
                                       endIt = lines.End();
        it != endIt;
        ++it)
    {
      const LineRun& line       = *it;
      bool           isLastLine = (it + 1 == endIt);

      if(line.width > layoutSize.width)
      {
        layoutSize.width = line.width;
      }

      layoutSize.height += GetLineHeight(line, isLastLine);
    }
  }

  /**
   * @brief Updates the indices of the character and glyph runs of the lines before the new lines are inserted.
   *
   * @param[in] layoutParameters The parameters needed to layout the text.
   * @param[in,out] lines The vector of lines (before the new laid-out lines are inserted).
   * @param[in] characterOffset The offset to be added to the runs of characters.
   * @param[in] glyphOffset The offset to be added to the runs of glyphs.
   */
  void UpdateLineIndexOffsets(const Parameters& layoutParameters,
                              Vector<LineRun>&  lines,
                              Length            characterOffset,
                              Length            glyphOffset)
  {
    // Update the glyph and character runs.
    for(Vector<LineRun>::Iterator it    = lines.Begin() + layoutParameters.startLineIndex,
                                  endIt = lines.End();
        it != endIt;
        ++it)
    {
      LineRun& line = *it;

      line.glyphRun.glyphIndex         = glyphOffset;
      line.characterRun.characterIndex = characterOffset;

      glyphOffset += line.glyphRun.numberOfGlyphs;
      characterOffset += line.characterRun.numberOfCharacters;
    }
  }

  /**
   * @brief Sets the relative line size for the LineLayout
   *
   * @param[in] currentParagraphRun Contains the bounded paragraph for this line layout.
   * @param[in,out] lineLayout The line layout to be updated.
   */
  void SetRelativeLineSize(BoundedParagraphRun* currentParagraphRun, LineLayout& lineLayout)
  {
    lineLayout.relativeLineSize = mRelativeLineSize;

    if(currentParagraphRun != nullptr && currentParagraphRun->relativeLineSizeDefined)
    {
      lineLayout.relativeLineSize = currentParagraphRun->relativeLineSize;
    }
  }

  /**
   * @brief Get the bounded paragraph for the characterIndex if exists.
   *
   * @param[in] boundedParagraphRuns The bounded paragraph list to search in.
   * @param[in] characterIndex The character index to get bounded paragraph for.
   * @param[out] currentParagraphRun Contains the bounded paragraph if found for the characterIndex.
   *
   * @return returns true if a bounded paragraph was found.
   */
  bool GetBoundedParagraph(const Vector<BoundedParagraphRun> boundedParagraphRuns, CharacterIndex characterIndex, BoundedParagraphRun& currentParagraphRun)
  {
    for(Vector<BoundedParagraphRun>::Iterator it    = boundedParagraphRuns.Begin(),
                                              endIt = boundedParagraphRuns.End();
        it != endIt;
        ++it)
    {
      BoundedParagraphRun& tempParagraphRun = *it;

      if(characterIndex >= tempParagraphRun.characterRun.characterIndex &&
         characterIndex < (tempParagraphRun.characterRun.characterIndex + tempParagraphRun.characterRun.numberOfCharacters))
      {
        currentParagraphRun = tempParagraphRun;
        return true;
      }
    }

    return false;
  }

  bool LayoutText(Parameters&                       layoutParameters,
                  Size&                             layoutSize,
                  bool                              elideTextEnabled,
                  bool&                             isAutoScrollEnabled,
                  DevelText::EllipsisPosition::Type ellipsisPosition)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->LayoutText\n");
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  box size %f, %f\n", layoutParameters.boundingBox.width, layoutParameters.boundingBox.height);

    layoutParameters.textModel->mVisualModel->mHyphen.glyph.Clear();
    layoutParameters.textModel->mVisualModel->mHyphen.index.Clear();

    //Reset indices of ElidedGlyphs
    layoutParameters.textModel->mVisualModel->SetStartIndexOfElidedGlyphs(0u);
    layoutParameters.textModel->mVisualModel->SetEndIndexOfElidedGlyphs(layoutParameters.textModel->GetNumberOfGlyphs() - 1u);
    layoutParameters.textModel->mVisualModel->SetFirstMiddleIndexOfElidedGlyphs(0u);
    layoutParameters.textModel->mVisualModel->SetSecondMiddleIndexOfElidedGlyphs(0u);

    Vector<LineRun>&                   lines                = layoutParameters.textModel->mVisualModel->mLines;
    const Vector<BoundedParagraphRun>& boundedParagraphRuns = layoutParameters.textModel->GetBoundedParagraphRuns();

    if(0u == layoutParameters.numberOfGlyphs)
    {
      // Add an extra line if the last character is a new paragraph character and the last line doesn't have zero characters.
      if(layoutParameters.isLastNewParagraph)
      {
        Length numberOfLines = lines.Count();
        if(0u != numberOfLines)
        {
          const LineRun& lastLine = *(lines.End() - 1u);

          if(0u != lastLine.characterRun.numberOfCharacters)
          {
            // Need to add a new line with no characters but with height to increase the layoutSize.height
            LineRun newLine;
            Initialize(newLine);
            lines.PushBack(newLine);

            UpdateTextLayout(layoutParameters,
                             lastLine.characterRun.characterIndex + lastLine.characterRun.numberOfCharacters,
                             lastLine.glyphRun.glyphIndex + lastLine.glyphRun.numberOfGlyphs,
                             layoutSize,
                             lines.Begin(),
                             numberOfLines);
          }
        }
      }

      // Calculates the layout size.
      UpdateLayoutSize(lines,
                       layoutSize);

      // Rounds upward to avoid a non integer size.
      layoutSize.height = std::ceil(layoutSize.height);

      // Nothing else do if there are no glyphs to layout.
      return false;
    }

    const GlyphIndex lastGlyphPlusOne    = layoutParameters.startGlyphIndex + layoutParameters.numberOfGlyphs;
    const Length     totalNumberOfGlyphs = layoutParameters.textModel->mVisualModel->mGlyphs.Count();
    Vector<Vector2>& glyphPositions      = layoutParameters.textModel->mVisualModel->mGlyphPositions;

    // In a previous layout, an extra line with no characters may have been added if the text ended with a new paragraph character.
    // This extra line needs to be removed.
    if(0u != lines.Count())
    {
      Vector<LineRun>::Iterator lastLine = lines.End() - 1u;

      if((0u == lastLine->characterRun.numberOfCharacters) &&
         (lastGlyphPlusOne == totalNumberOfGlyphs))
      {
        lines.Remove(lastLine);
      }
    }

    // Retrieve BiDi info.
    const bool hasBidiParagraphs = !layoutParameters.textModel->mLogicalModel->mBidirectionalParagraphInfo.Empty();

    const CharacterIndex* const                  glyphsToCharactersBuffer    = layoutParameters.textModel->mVisualModel->mGlyphsToCharacters.Begin();
    const Vector<BidirectionalParagraphInfoRun>& bidirectionalParagraphsInfo = layoutParameters.textModel->mLogicalModel->mBidirectionalParagraphInfo;
    const Vector<BidirectionalLineInfoRun>&      bidirectionalLinesInfo      = layoutParameters.textModel->mLogicalModel->mBidirectionalLineInfo;

    // Set the layout bidirectional paramters.
    LayoutBidiParameters layoutBidiParameters;

    // Whether the layout is being updated or set from scratch.
    const bool updateCurrentBuffer = layoutParameters.numberOfGlyphs < totalNumberOfGlyphs;

    Vector2*        glyphPositionsBuffer = nullptr;
    Vector<Vector2> newGlyphPositions;

    LineRun*        linesBuffer = nullptr;
    Vector<LineRun> newLines;

    // Estimate the number of lines.
    Length linesCapacity = std::max(1u, layoutParameters.estimatedNumberOfLines);
    Length numberOfLines = 0u;

    if(updateCurrentBuffer)
    {
      newGlyphPositions.Resize(layoutParameters.numberOfGlyphs);
      glyphPositionsBuffer = newGlyphPositions.Begin();

      newLines.Resize(linesCapacity);
      linesBuffer = newLines.Begin();
    }
    else
    {
      glyphPositionsBuffer = glyphPositions.Begin();

      lines.Resize(linesCapacity);
      linesBuffer = lines.Begin();
    }

    float penY            = CalculateLineOffset(lines,
                                     layoutParameters.startLineIndex);
    bool  anyLineIsEliped = false;
    for(GlyphIndex index = layoutParameters.startGlyphIndex; index < lastGlyphPlusOne;)
    {
      layoutBidiParameters.Clear();

      if(hasBidiParagraphs)
      {
        const CharacterIndex startCharacterIndex = *(glyphsToCharactersBuffer + index);

        for(Vector<BidirectionalParagraphInfoRun>::ConstIterator it    = bidirectionalParagraphsInfo.Begin(),
                                                                 endIt = bidirectionalParagraphsInfo.End();
            it != endIt;
            ++it, ++layoutBidiParameters.bidiParagraphIndex)
        {
          const BidirectionalParagraphInfoRun& run = *it;

          const CharacterIndex lastCharacterIndex = run.characterRun.characterIndex + run.characterRun.numberOfCharacters;

          if(lastCharacterIndex <= startCharacterIndex)
          {
            // Do not process, the paragraph has already been processed.
            continue;
          }

          if(startCharacterIndex >= run.characterRun.characterIndex && startCharacterIndex < lastCharacterIndex)
          {
            layoutBidiParameters.paragraphDirection = run.direction;
            layoutBidiParameters.isBidirectional    = true;
          }

          // Has already been found.
          break;
        }

        if(layoutBidiParameters.isBidirectional)
        {
          for(Vector<BidirectionalLineInfoRun>::ConstIterator it    = bidirectionalLinesInfo.Begin(),
                                                              endIt = bidirectionalLinesInfo.End();
              it != endIt;
              ++it, ++layoutBidiParameters.bidiLineIndex)
          {
            const BidirectionalLineInfoRun& run = *it;

            const CharacterIndex lastCharacterIndex = run.characterRun.characterIndex + run.characterRun.numberOfCharacters;

            if(lastCharacterIndex <= startCharacterIndex)
            {
              // skip
              continue;
            }

            if(startCharacterIndex < lastCharacterIndex)
            {
              // Found where to insert the bidi line info.
              break;
            }
          }
        }
      }

      CharacterDirection currentParagraphDirection = layoutBidiParameters.paragraphDirection;

      // Get the layout for the line.
      LineLayout layout;
      layout.direction  = layoutBidiParameters.paragraphDirection;
      layout.glyphIndex = index;

      BoundedParagraphRun currentParagraphRun;
      (GetBoundedParagraph(boundedParagraphRuns, *(glyphsToCharactersBuffer + index), currentParagraphRun) ? SetRelativeLineSize(&currentParagraphRun, layout) : SetRelativeLineSize(nullptr, layout));

      GetLineLayoutForBox(layoutParameters,
                          layoutBidiParameters,
                          layout,
                          false,
                          ellipsisPosition,
                          false,
                          elideTextEnabled);

      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "           glyph index %d\n", layout.glyphIndex);
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "       character index %d\n", layout.characterIndex);
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "      number of glyphs %d\n", layout.numberOfGlyphs);
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  number of characters %d\n", layout.numberOfCharacters);
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "                length %f\n", layout.length);

      CharacterIndex lastCharacterInParagraph = currentParagraphRun.characterRun.characterIndex + currentParagraphRun.characterRun.numberOfCharacters - 1;

      //check if this is the last line in paragraph, if false we should use the default relative line size (the one set using the property)
      if(lastCharacterInParagraph >= layout.characterIndex && lastCharacterInParagraph < layout.characterIndex+layout.numberOfCharacters)
      {
        layout.relativeLineSize = mRelativeLineSize;
      }

      if(0u == layout.numberOfGlyphs + layout.numberOfGlyphsInSecondHalfLine)
      {
        // The width is too small and no characters are laid-out.
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--LayoutText width too small!\n\n");

        lines.Resize(numberOfLines);

        // Rounds upward to avoid a non integer size.
        layoutSize.height = std::ceil(layoutSize.height);

        return false;
      }

      // Set the line position. DISCARD if ellipsis is enabled and the position exceeds the boundaries
      // of the box.
      penY += layout.ascender;

      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  pen y %f\n", penY);

      bool ellipsis = false;
      if(elideTextEnabled)
      {
        layoutBidiParameters.paragraphDirection = currentParagraphDirection;

        // Does the ellipsis of the last line.
        ellipsis = EllipsisLine(layoutParameters,
                                layoutBidiParameters,
                                layout,
                                layoutSize,
                                linesBuffer,
                                glyphPositionsBuffer,
                                numberOfLines,
                                penY,
                                isAutoScrollEnabled,
                                ellipsisPosition,
                                false);
      }

      if(ellipsis && ((ellipsisPosition == DevelText::EllipsisPosition::END) || (numberOfLines == 1u)))
      {
        const bool isMultiline = mLayout == MULTI_LINE_BOX;
        if(isMultiline && ellipsisPosition != DevelText::EllipsisPosition::END)
        {
          ellipsis = EllipsisLine(layoutParameters,
                                  layoutBidiParameters,
                                  layout,
                                  layoutSize,
                                  linesBuffer,
                                  glyphPositionsBuffer,
                                  numberOfLines,
                                  penY,
                                  isAutoScrollEnabled,
                                  ellipsisPosition,
                                  true);
        }

        //clear hyphen from ellipsis line
        const Length* hyphenIndices = layoutParameters.textModel->mVisualModel->mHyphen.index.Begin();
        Length        hyphensCount  = layoutParameters.textModel->mVisualModel->mHyphen.glyph.Size();

        while(hyphenIndices && hyphensCount > 0 && hyphenIndices[hyphensCount - 1] >= layout.glyphIndex)
        {
          layoutParameters.textModel->mVisualModel->mHyphen.index.Remove(layoutParameters.textModel->mVisualModel->mHyphen.index.Begin() + hyphensCount - 1);
          layoutParameters.textModel->mVisualModel->mHyphen.glyph.Remove(layoutParameters.textModel->mVisualModel->mHyphen.glyph.Begin() + hyphensCount - 1);
          hyphensCount--;
        }

        // No more lines to layout.
        break;
      }
      else
      {
        //In START location of ellipsis whether to shift lines or not.
        anyLineIsEliped |= ellipsis;

        // Whether the last line has been laid-out.
        const bool isLastLine = index + (layout.numberOfGlyphs + layout.numberOfGlyphsInSecondHalfLine) == totalNumberOfGlyphs;

        if(numberOfLines == linesCapacity)
        {
          // Reserve more space for the next lines.
          linesBuffer = ResizeLinesBuffer(lines,
                                          newLines,
                                          linesCapacity,
                                          updateCurrentBuffer);
        }

        // Updates the current text's layout with the line's layout.
        UpdateTextLayout(layoutParameters,
                         layout,
                         layoutSize,
                         linesBuffer,
                         index,
                         numberOfLines,
                         isLastLine);

        const GlyphIndex nextIndex = index + layout.numberOfGlyphs + layout.numberOfGlyphsInSecondHalfLine;

        if((nextIndex == totalNumberOfGlyphs) &&
           layoutParameters.isLastNewParagraph &&
           (mLayout == MULTI_LINE_BOX))
        {
          // The last character of the text is a new paragraph character.
          // An extra line with no characters is added to increase the text's height
          // in order to place the cursor.

          if(numberOfLines == linesCapacity)
          {
            // Reserve more space for the next lines.
            linesBuffer = ResizeLinesBuffer(lines,
                                            newLines,
                                            linesCapacity,
                                            updateCurrentBuffer);
          }

          UpdateTextLayout(layoutParameters,
                           layout.characterIndex + (layout.numberOfCharacters + layout.numberOfCharactersInSecondHalfLine),
                           index + (layout.numberOfGlyphs + layout.numberOfGlyphsInSecondHalfLine),
                           layoutSize,
                           linesBuffer,
                           numberOfLines);
        } // whether to add a last line.

        const BidirectionalLineInfoRun* const bidirectionalLineInfo = (layoutBidiParameters.isBidirectional && !bidirectionalLinesInfo.Empty()) ? &bidirectionalLinesInfo[layoutBidiParameters.bidiLineIndex] : nullptr;

        if((nullptr != bidirectionalLineInfo) &&
           !bidirectionalLineInfo->isIdentity &&
           (layout.characterIndex == bidirectionalLineInfo->characterRun.characterIndex))
        {
          SetGlyphPositions(layoutParameters,
                            glyphPositionsBuffer,
                            layoutBidiParameters,
                            layout);
        }
        else
        {
          // Sets the positions of the glyphs.
          SetGlyphPositions(layoutParameters,
                            glyphPositionsBuffer,
                            layout);
        }

        // Updates the vertical pen's position.
        penY += -layout.descender + layout.lineSpacing + GetLineSpacing(layout.ascender + -layout.descender, layout.relativeLineSize);

        // Increase the glyph index.
        index = nextIndex;
      } // no ellipsis
    }   // end for() traversing glyphs.

    //Shift lines to up if ellipsis and multilines and set ellipsis of first line to true
    if(anyLineIsEliped && numberOfLines > 1u)
    {
      if(ellipsisPosition == DevelText::EllipsisPosition::START)
      {
        Length lineIndex = 0;
        while(lineIndex < numberOfLines && layoutParameters.boundingBox.height < layoutSize.height)
        {
          LineRun& delLine = linesBuffer[lineIndex];
          delLine.ellipsis = true;

          layoutSize.height -= (delLine.ascender + -delLine.descender) + delLine.lineSpacing;
          for(Length lineIndex = 0; lineIndex < numberOfLines - 1; lineIndex++)
          {
            linesBuffer[lineIndex]          = linesBuffer[lineIndex + 1];
            linesBuffer[lineIndex].ellipsis = false;
          }
          numberOfLines--;
        }
        linesBuffer[0u].ellipsis = true;
      }
      else if(ellipsisPosition == DevelText::EllipsisPosition::MIDDLE)
      {
        Length middleLineIndex   = (numberOfLines) / 2u;
        Length ellipsisLineIndex = 0u;
        while(1u < numberOfLines && 0u < middleLineIndex && layoutParameters.boundingBox.height < layoutSize.height)
        {
          LineRun& delLine = linesBuffer[middleLineIndex];
          delLine.ellipsis = true;

          layoutSize.height -= (delLine.ascender + -delLine.descender) + delLine.lineSpacing;
          for(Length lineIndex = middleLineIndex; lineIndex < numberOfLines - 1; lineIndex++)
          {
            linesBuffer[lineIndex]          = linesBuffer[lineIndex + 1];
            linesBuffer[lineIndex].ellipsis = false;
          }
          numberOfLines--;
          ellipsisLineIndex = middleLineIndex - 1u;
          middleLineIndex   = (numberOfLines) / 2u;
        }

        linesBuffer[ellipsisLineIndex].ellipsis = true;
      }
    }

    if(updateCurrentBuffer)
    {
      glyphPositions.Insert(glyphPositions.Begin() + layoutParameters.startGlyphIndex,
                            newGlyphPositions.Begin(),
                            newGlyphPositions.End());
      glyphPositions.Resize(totalNumberOfGlyphs);

      newLines.Resize(numberOfLines);

      // Current text's layout size adds only the newly laid-out lines.
      // Updates the layout size with the previously laid-out lines.
      UpdateLayoutSize(lines,
                       layoutSize);

      if(0u != newLines.Count())
      {
        const LineRun& lastLine = *(newLines.End() - 1u);

        const Length characterOffset = lastLine.characterRun.characterIndex + lastLine.characterRun.numberOfCharacters;
        const Length glyphOffset     = lastLine.glyphRun.glyphIndex + lastLine.glyphRun.numberOfGlyphs;

        // Update the indices of the runs before the new laid-out lines are inserted.
        UpdateLineIndexOffsets(layoutParameters,
                               lines,
                               characterOffset,
                               glyphOffset);

        // Insert the lines.
        lines.Insert(lines.Begin() + layoutParameters.startLineIndex,
                     newLines.Begin(),
                     newLines.End());
      }
    }
    else
    {
      lines.Resize(numberOfLines);
    }

    // Rounds upward to avoid a non integer size.
    layoutSize.height = std::ceil(layoutSize.height);

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--LayoutText\n\n");

    return true;
  }

  void Align(const Size&                     size,
             CharacterIndex                  startIndex,
             Length                          numberOfCharacters,
             Text::HorizontalAlignment::Type horizontalAlignment,
             Vector<LineRun>&                lines,
             float&                          alignmentOffset,
             Dali::LayoutDirection::Type     layoutDirection,
             bool                            matchLayoutDirection)
  {
    const CharacterIndex lastCharacterPlusOne = startIndex + numberOfCharacters;

    alignmentOffset = MAX_FLOAT;
    // Traverse all lines and align the glyphs.
    for(Vector<LineRun>::Iterator it = lines.Begin(), endIt = lines.End();
        it != endIt;
        ++it)
    {
      LineRun& line = *it;

      if(line.characterRun.characterIndex < startIndex)
      {
        // Do not align lines which have already been aligned.
        continue;
      }

      if(line.characterRun.characterIndex > lastCharacterPlusOne)
      {
        // Do not align lines beyond the last laid-out character.
        break;
      }

      if(line.characterRun.characterIndex == lastCharacterPlusOne && !isEmptyLineAtLast(lines, it))
      {
        // Do not align lines beyond the last laid-out character unless the line is last and empty.
        break;
      }

      // Calculate the line's alignment offset accordingly with the align option,
      // the box width, line length, and the paragraph's direction.
      CalculateHorizontalAlignment(size.width,
                                   horizontalAlignment,
                                   line,
                                   layoutDirection,
                                   matchLayoutDirection);

      // Updates the alignment offset.
      alignmentOffset = std::min(alignmentOffset, line.alignmentOffset);
    }
  }

  void CalculateHorizontalAlignment(float                       boxWidth,
                                    HorizontalAlignment::Type   horizontalAlignment,
                                    LineRun&                    line,
                                    Dali::LayoutDirection::Type layoutDirection,
                                    bool                        matchLayoutDirection)
  {
    line.alignmentOffset = 0.f;
    const bool isLineRTL = RTL == line.direction;

    // Whether to swap the alignment.
    // Swap if the line is RTL and is not required to match the direction of the system's language or if it's required to match the direction of the system's language and it's RTL.
    bool  isLayoutRTL = isLineRTL;
    float lineLength  = line.width;

    // match align for system language direction
    if(matchLayoutDirection)
    {
      // Swap the alignment type if the line is right to left.
      isLayoutRTL = layoutDirection == LayoutDirection::RIGHT_TO_LEFT;
    }
    // Calculate the horizontal line offset.
    switch(horizontalAlignment)
    {
      case HorizontalAlignment::BEGIN:
      {
        if(isLayoutRTL)
        {
          if(isLineRTL)
          {
            lineLength += line.extraLength;
          }

          line.alignmentOffset = boxWidth - lineLength;
        }
        else
        {
          line.alignmentOffset = 0.f;

          if(isLineRTL)
          {
            // 'Remove' the white spaces at the end of the line (which are at the beginning in visual order)
            line.alignmentOffset -= line.extraLength;
          }
        }
        break;
      }
      case HorizontalAlignment::CENTER:
      {
        line.alignmentOffset = 0.5f * (boxWidth - lineLength);

        if(isLineRTL)
        {
          line.alignmentOffset -= line.extraLength;
        }

        line.alignmentOffset = std::floor(line.alignmentOffset); // floor() avoids pixel alignment issues.
        break;
      }
      case HorizontalAlignment::END:
      {
        if(isLayoutRTL)
        {
          line.alignmentOffset = 0.f;

          if(isLineRTL)
          {
            // 'Remove' the white spaces at the end of the line (which are at the beginning in visual order)
            line.alignmentOffset -= line.extraLength;
          }
        }
        else
        {
          if(isLineRTL)
          {
            lineLength += line.extraLength;
          }

          line.alignmentOffset = boxWidth - lineLength;
        }
        break;
      }
    }
  }

  void Initialize(LineRun& line)
  {
    line.glyphRun.glyphIndex                              = 0u;
    line.glyphRun.numberOfGlyphs                          = 0u;
    line.characterRun.characterIndex                      = 0u;
    line.characterRun.numberOfCharacters                  = 0u;
    line.width                                            = 0.f;
    line.ascender                                         = 0.f;
    line.descender                                        = 0.f;
    line.extraLength                                      = 0.f;
    line.alignmentOffset                                  = 0.f;
    line.direction                                        = LTR;
    line.ellipsis                                         = false;
    line.lineSpacing                                      = mDefaultLineSpacing;
    line.isSplitToTwoHalves                               = false;
    line.glyphRunSecondHalf.glyphIndex                    = 0u;
    line.glyphRunSecondHalf.numberOfGlyphs                = 0u;
    line.characterRunForSecondHalfLine.characterIndex     = 0u;
    line.characterRunForSecondHalfLine.numberOfCharacters = 0u;
  }

  Type  mLayout;
  float mCursorWidth;
  float mDefaultLineSpacing;
  float mDefaultLineSize;

  IntrusivePtr<Metrics> mMetrics;
  float                 mRelativeLineSize;
};

Engine::Engine()
: mImpl{nullptr}
{
  mImpl = new Engine::Impl();
}

Engine::~Engine()
{
  delete mImpl;
}

void Engine::SetMetrics(MetricsPtr& metrics)
{
  mImpl->mMetrics = metrics;
}

void Engine::SetLayout(Type layout)
{
  mImpl->mLayout = layout;
}

Engine::Type Engine::GetLayout() const
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "GetLayout[%d]\n", mImpl->mLayout);
  return mImpl->mLayout;
}

void Engine::SetCursorWidth(int width)
{
  mImpl->mCursorWidth = static_cast<float>(width);
}

int Engine::GetCursorWidth() const
{
  return static_cast<int>(mImpl->mCursorWidth);
}

bool Engine::LayoutText(Parameters&                       layoutParameters,
                        Size&                             layoutSize,
                        bool                              elideTextEnabled,
                        bool&                             isAutoScrollEnabled,
                        DevelText::EllipsisPosition::Type ellipsisPosition)
{
  return mImpl->LayoutText(layoutParameters,
                           layoutSize,
                           elideTextEnabled,
                           isAutoScrollEnabled,
                           ellipsisPosition);
}

void Engine::Align(const Size&                     size,
                   CharacterIndex                  startIndex,
                   Length                          numberOfCharacters,
                   Text::HorizontalAlignment::Type horizontalAlignment,
                   Vector<LineRun>&                lines,
                   float&                          alignmentOffset,
                   Dali::LayoutDirection::Type     layoutDirection,
                   bool                            matchLayoutDirection)
{
  mImpl->Align(size,
               startIndex,
               numberOfCharacters,
               horizontalAlignment,
               lines,
               alignmentOffset,
               layoutDirection,
               matchLayoutDirection);
}

void Engine::SetDefaultLineSpacing(float lineSpacing)
{
  mImpl->mDefaultLineSpacing = lineSpacing;
}

float Engine::GetDefaultLineSpacing() const
{
  return mImpl->mDefaultLineSpacing;
}

void Engine::SetDefaultLineSize(float lineSize)
{
  mImpl->mDefaultLineSize = lineSize;
}

float Engine::GetDefaultLineSize() const
{
  return mImpl->mDefaultLineSize;
}

void Engine::SetRelativeLineSize(float relativeLineSize)
{
  mImpl->mRelativeLineSize = relativeLineSize;
}

float Engine::GetRelativeLineSize() const
{
  return mImpl->mRelativeLineSize;
}

} // namespace Layout

} // namespace Text

} // namespace Toolkit

} // namespace Dali
