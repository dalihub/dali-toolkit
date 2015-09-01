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
#include <dali-toolkit/internal/text/layouts/layout-engine.h>

// EXTERNAL INCLUDES
#include <limits>
#include <dali/integration-api/debug.h>
#include <dali/devel-api/text-abstraction/font-client.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/layouts/layout-parameters.h>
#include <dali-toolkit/internal/text/bidirectional-line-info-run.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

namespace
{

#if defined(DEBUG_ENABLED)
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_LAYOUT");
#endif

const float MAX_FLOAT = std::numeric_limits<float>::max();
const bool RTL = true;
const float CURSOR_WIDTH = 1.f;

} //namespace

/**
 * @brief Stores temporary layout info of the line.
 */
struct LineLayout
{
  LineLayout()
  : glyphIndex( 0u ),
    characterIndex( 0u ),
    numberOfGlyphs( 0u ),
    numberOfCharacters( 0u ),
    length( 0.f ),
    extraBearing( 0.f ),
    extraWidth( 0.f ),
    wsLengthEndOfLine( 0.f ),
    ascender( 0.f ),
    descender( MAX_FLOAT )
  {}

  ~LineLayout()
  {}

  void Clear()
  {
    glyphIndex = 0u;
    characterIndex = 0u;
    numberOfGlyphs = 0u;
    numberOfCharacters = 0u;
    length = 0.f;
    extraBearing = 0.f;
    extraWidth = 0.f;
    wsLengthEndOfLine = 0.f;
    ascender = 0.f;
    descender = MAX_FLOAT;
  }

  GlyphIndex     glyphIndex;         ///< Index of the first glyph to be laid-out.
  CharacterIndex characterIndex;     ///< Index of the first character to be laid-out.
  Length         numberOfGlyphs;     ///< The number of glyph which fit in one line.
  Length         numberOfCharacters; ///< The number of characters which fit in one line.
  float          length;             ///< The addition of the advance metric of all the glyphs which fit in one line.
  float          extraBearing;       ///< The extra width to be added to the line's length when the bearing of the first glyph is negative.
  float          extraWidth;         ///< The extra width to be added to the line's length when the bearing + width of the last glyph is greater than the advance.
  float          wsLengthEndOfLine;  ///< The length of the white spaces at the end of the line.
  float          ascender;           ///< The maximum ascender of all fonts in the line.
  float          descender;          ///< The minimum descender of all fonts in the line.
};

struct LayoutEngine::Impl
{
  Impl()
  : mLayout( LayoutEngine::SINGLE_LINE_BOX ),
    mHorizontalAlignment( LayoutEngine::HORIZONTAL_ALIGN_BEGIN ),
    mVerticalAlignment( LayoutEngine::VERTICAL_ALIGN_TOP ),
    mCursorWidth( CURSOR_WIDTH ),
    mEllipsisEnabled( false )
  {
    mFontClient = TextAbstraction::FontClient::Get();
  }

  /**
   * @brief Updates the line ascender and descender with the metrics of a new font.
   *
   * @param[in] fontId The id of the new font.
   * @param[in,out] lineLayout The line layout.
   */
  void UpdateLineHeight( FontId fontId, LineLayout& lineLayout )
  {
    Text::FontMetrics fontMetrics;
    mFontClient.GetFontMetrics( fontId, fontMetrics );

    // Sets the maximum ascender.
    if( fontMetrics.ascender > lineLayout.ascender )
    {
      lineLayout.ascender = fontMetrics.ascender;
    }

    // Sets the minimum descender.
    if( fontMetrics.descender < lineLayout.descender )
    {
      lineLayout.descender = fontMetrics.descender;
    }
  }

  /**
   * @brief Merges a temporary line layout into the line layout.
   *
   * @param[in,out] lineLayout The line layout.
   * @param[in] tmpLineLayout A temporary line layout.
   */
  void MergeLineLayout( LineLayout& lineLayout,
                        const LineLayout& tmpLineLayout )
  {
    lineLayout.numberOfCharacters += tmpLineLayout.numberOfCharacters;
    lineLayout.numberOfGlyphs += tmpLineLayout.numberOfGlyphs;
    lineLayout.length += tmpLineLayout.length;

    if( 0.f < tmpLineLayout.length )
    {
      lineLayout.length += lineLayout.wsLengthEndOfLine;

      lineLayout.wsLengthEndOfLine = tmpLineLayout.wsLengthEndOfLine;
    }
    else
    {
      lineLayout.wsLengthEndOfLine += tmpLineLayout.wsLengthEndOfLine;
    }

    if( tmpLineLayout.ascender > lineLayout.ascender )
    {
      lineLayout.ascender = tmpLineLayout.ascender;
    }

    if( tmpLineLayout.descender < lineLayout.descender )
    {
      lineLayout.descender = tmpLineLayout.descender;
    }
  }

  /**
   * Retrieves the line layout for a given box width.
   *
   * @note This method lais out text as it were left to right. At this point is not possible to reorder the line
   *       because the number of characters of the line is not known (one of the responsabilities of this method
   *       is calculate that). Due to glyph's 'x' bearing, width and advance, when right to left or mixed right to left
   *       and left to right text is laid out, it can be small differences in the line length. One solution is to
   *       reorder and re-lay out the text after this method and add or remove one extra glyph if needed. However,
   *       this method calculates which are the first and last glyphs of the line (the ones that causes the
   *       differences). This is a good point to check if there is problems with the text exceeding the boundaries
   *       of the control when there is right to left text.
   *
   * @param[in] parameters The layout parameters.
   * @param[out] lineLayout The line layout.
   * @param[in,out] paragraphDirection in: the current paragraph's direction, out: the next paragraph's direction. Is set after a must break.
   * @param[in] completelyFill Whether to completely fill the line ( even if the last word exceeds the boundaries ).
   */
  void GetLineLayoutForBox( const LayoutParameters& parameters,
                            LineLayout& lineLayout,
                            CharacterDirection& paragraphDirection,
                            bool completelyFill )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "-->GetLineLayoutForBox\n" );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  initial glyph index : %d\n", lineLayout.glyphIndex );
    // Stores temporary line layout which has not been added to the final line layout.
    LineLayout tmpLineLayout;

    const bool isMultiline = mLayout == MULTI_LINE_BOX;
    const GlyphIndex lastGlyphIndex = parameters.totalNumberOfGlyphs - 1u;

    // If the first glyph has a negative bearing its absolute value needs to be added to the line length.
    // In the case the line starts with a right to left character, if the width is longer than the advance,
    // the difference needs to be added to the line length.
    const GlyphInfo& glyphInfo = *( parameters.glyphsBuffer + lineLayout.glyphIndex );

    // Set the direction of the first character of the line.
    lineLayout.characterIndex = *( parameters.glyphsToCharactersBuffer + lineLayout.glyphIndex );
    const CharacterDirection firstCharacterDirection = ( NULL == parameters.characterDirectionBuffer ) ? false : *( parameters.characterDirectionBuffer + lineLayout.characterIndex );
    CharacterDirection previousCharacterDirection = firstCharacterDirection;

    const float extraWidth = glyphInfo.xBearing + glyphInfo.width - glyphInfo.advance;
    float tmpExtraWidth = ( 0.f < extraWidth ) ? extraWidth : 0.f;

    float tmpExtraBearing = ( 0.f > glyphInfo.xBearing ) ? -glyphInfo.xBearing : 0.f;

    tmpLineLayout.length += mCursorWidth; // Added to give some space to the cursor.

    // Calculate the line height if there is no characters.
    FontId lastFontId = glyphInfo.fontId;
    UpdateLineHeight( lastFontId, tmpLineLayout );

    bool oneWordLaidOut = false;

    for( GlyphIndex glyphIndex = lineLayout.glyphIndex;
         glyphIndex < parameters.totalNumberOfGlyphs;
         ++glyphIndex )
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  glyph index : %d\n", glyphIndex );
      const bool isLastGlyph = glyphIndex == lastGlyphIndex;

      // Get the glyph info.
      const GlyphInfo& glyphInfo = *( parameters.glyphsBuffer + glyphIndex );

      // Check if the font of the current glyph is the same of the previous one.
      // If it's different the ascender and descender need to be updated.
      if( lastFontId != glyphInfo.fontId )
      {
        UpdateLineHeight( glyphInfo.fontId, tmpLineLayout );
        lastFontId = glyphInfo.fontId;
      }

      // Get the character indices for the current glyph. The last character index is needed
      // because there are glyphs formed by more than one character but their break info is
      // given only for the last character.
      const Length charactersPerGlyph = *( parameters.charactersPerGlyphBuffer + glyphIndex );
      const CharacterIndex characterFirstIndex = *( parameters.glyphsToCharactersBuffer + glyphIndex );
      const CharacterIndex characterLastIndex = characterFirstIndex + ( ( 1u > charactersPerGlyph ) ? 0u : charactersPerGlyph - 1u );

      // Get the line break info for the current character.
      const LineBreakInfo lineBreakInfo = *( parameters.lineBreakInfoBuffer + characterLastIndex );

      // Get the word break info for the current character.
      const WordBreakInfo wordBreakInfo = *( parameters.wordBreakInfoBuffer + characterLastIndex );

      // Increase the number of characters.
      tmpLineLayout.numberOfCharacters += charactersPerGlyph;

      // Increase the number of glyphs.
      tmpLineLayout.numberOfGlyphs++;

      // Check whether is a white space.
      const Character character = *( parameters.textBuffer + characterFirstIndex );
      const bool isWhiteSpace = TextAbstraction::IsWhiteSpace( character );

      // Used to restore the temporal line layout when a single word does not fit in the control's width and is split by character.
      const float previousTmpLineLength = tmpLineLayout.length;
      const float previousTmpExtraBearing = tmpExtraBearing;
      const float previousTmpExtraWidth = tmpExtraWidth;

      // Get the character's direction.
      const CharacterDirection characterDirection = ( NULL == parameters.characterDirectionBuffer ) ? false : *( parameters.characterDirectionBuffer + characterFirstIndex );

      // Increase the accumulated length.
      if( isWhiteSpace )
      {
        // Add the length to the length of white spaces at the end of the line.
        tmpLineLayout.wsLengthEndOfLine += glyphInfo.advance; // The advance is used as the width is always zero for the white spaces.
      }
      else
      {
        // Add as well any previous white space length.
        tmpLineLayout.length += tmpLineLayout.wsLengthEndOfLine + glyphInfo.advance;

        // An extra space may be added to the line for the first and last glyph of the line.
        // If the bearing of the first glyph is negative, its positive value needs to be added.
        // If the bearing plus the width of the last glyph is greater than the advance, the difference
        // needs to be added.

        if( characterDirection == paragraphDirection )
        {
          if( RTL == characterDirection )
          {
            //       <--
            // |   Rrrrr|
            // or
            // |  Rllrrr|
            // or
            // |lllrrrrr|
            // |     Rll|
            //

            tmpExtraBearing = ( 0.f > glyphInfo.xBearing ) ? -glyphInfo.xBearing : 0.f;
          }
          else // LTR
          {
            //  -->
            // |lllL    |
            // or
            // |llrrL   |
            // or
            // |lllllrrr|
            // |rrL     |
            //

            const float extraWidth = glyphInfo.xBearing + glyphInfo.width - glyphInfo.advance;
            tmpExtraWidth = ( 0.f < extraWidth ) ? extraWidth : 0.f;
          }
        }
        else
        {
          if( characterDirection != previousCharacterDirection )
          {
            if( RTL == characterDirection )
            {
              //  -->
              // |lllR    |

              const float extraWidth = glyphInfo.xBearing + glyphInfo.width - glyphInfo.advance;
              tmpExtraWidth = ( 0.f < extraWidth ) ? extraWidth : 0.f;
            }
            else // LTR
            {
              //       <--
              // |   Lrrrr|

              tmpExtraBearing = ( 0.f > glyphInfo.xBearing ) ? -glyphInfo.xBearing : 0.f;
            }
          }
          else if( characterDirection == firstCharacterDirection )
          {
            if( RTL == characterDirection )
            {
              //  -->
              // |llllllrr|
              // |Rr      |

              tmpExtraBearing = ( 0.f > glyphInfo.xBearing ) ? -glyphInfo.xBearing : 0.f;
            }
            else // LTR
            {
              //       <--
              // |llllrrrr|
              // |     llL|

              const float extraWidth = glyphInfo.xBearing + glyphInfo.width - glyphInfo.advance;
              tmpExtraWidth = ( 0.f < extraWidth ) ? extraWidth : 0.f;
            }
          }
        }

        // Clear the white space length at the end of the line.
        tmpLineLayout.wsLengthEndOfLine = 0.f;
      }

      // Check if the accumulated length fits in the width of the box.
      if( ( completelyFill || isMultiline ) && !isWhiteSpace &&
          ( tmpExtraBearing + lineLayout.length + lineLayout.wsLengthEndOfLine + tmpLineLayout.length + tmpExtraWidth > parameters.boundingBox.width ) )
      {
        // Current word does not fit in the box's width.
        if( !oneWordLaidOut || completelyFill )
        {
          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  Break the word by character\n" );

          // The word's with doesn't fit in the control's with. It needs to be split by character.
          if( tmpLineLayout.numberOfGlyphs > 0u )
          {
            tmpLineLayout.numberOfCharacters -= charactersPerGlyph;
            --tmpLineLayout.numberOfGlyphs;
            tmpLineLayout.length = previousTmpLineLength;
            tmpExtraBearing = previousTmpExtraBearing;
            tmpExtraWidth = previousTmpExtraWidth;
          }

          // Add part of the word to the line layout.
          MergeLineLayout( lineLayout, tmpLineLayout );
        }
        else
        {
          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  Current word does not fit.\n" );
        }

        lineLayout.extraBearing = tmpExtraBearing;
        lineLayout.extraWidth = tmpExtraWidth;

        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--GetLineLayoutForBox.\n" );

        return;
      }

      if( ( isMultiline || isLastGlyph ) &&
          ( TextAbstraction::LINE_MUST_BREAK == lineBreakInfo ) )
      {
        // Must break the line. Update the line layout and return.
        MergeLineLayout( lineLayout, tmpLineLayout );

        // Set the next paragraph's direction.
        if( !isLastGlyph &&
            ( NULL != parameters.characterDirectionBuffer ) )
        {
          paragraphDirection = *( parameters.characterDirectionBuffer + 1u + characterLastIndex );
        }

        lineLayout.extraBearing = tmpExtraBearing;
        lineLayout.extraWidth = tmpExtraWidth;

        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  Must break\n" );
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--GetLineLayoutForBox\n" );
        return;
      }

      if( isMultiline &&
          ( TextAbstraction::WORD_BREAK == wordBreakInfo ) )
      {
        oneWordLaidOut = true;
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  One word laid out\n" );

        // Current glyph is the last one of the current word.
        // Add the temporal layout to the current one.
        MergeLineLayout( lineLayout, tmpLineLayout );

        tmpLineLayout.Clear();
      }

      previousCharacterDirection = characterDirection;
    }

    lineLayout.extraBearing = tmpExtraBearing;
    lineLayout.extraWidth = tmpExtraWidth;

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--GetLineLayoutForBox\n" );
  }

  void SetGlyphPositions( const GlyphInfo* const glyphsBuffer,
                          Length numberOfGlyphs,
                          float penY,
                          Vector2* glyphPositionsBuffer )
  {
    // Traverse the glyphs and set the positions.

    // Check if the x bearing of the first character is negative.
    // If it has a negative x bearing, it will exceed the boundaries of the actor,
    // so the penX position needs to be moved to the right.

    const GlyphInfo& glyph = *glyphsBuffer;
    float penX = ( 0.f > glyph.xBearing ) ? -glyph.xBearing : 0.f;
    penX += mCursorWidth; // Added to give some space to the cursor.

    for( GlyphIndex i = 0u; i < numberOfGlyphs; ++i )
    {
      const GlyphInfo& glyph = *( glyphsBuffer + i );
      Vector2& position = *( glyphPositionsBuffer + i );

      position.x = penX + glyph.xBearing;
      position.y = penY - glyph.yBearing;

      penX += glyph.advance;
    }
  }

  bool LayoutText( const LayoutParameters& layoutParameters,
                   Vector<Vector2>& glyphPositions,
                   Vector<LineRun>& lines,
                   Size& actualSize )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "-->LayoutText\n" );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  box size %f, %f\n", layoutParameters.boundingBox.width, layoutParameters.boundingBox.height );

    // Set the first paragraph's direction.
    CharacterDirection paragraphDirection = ( NULL != layoutParameters.characterDirectionBuffer ) ? *layoutParameters.characterDirectionBuffer : !RTL;

    float penY = 0.f;
    for( GlyphIndex index = 0u; index < layoutParameters.totalNumberOfGlyphs; )
    {
      CharacterDirection currentParagraphDirection = paragraphDirection;

      // Get the layout for the line.
      LineLayout layout;
      layout.glyphIndex = index;
      GetLineLayoutForBox( layoutParameters,
                           layout,
                           paragraphDirection,
                           false );

      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "           glyph index %d\n", layout.glyphIndex );
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "       character index %d\n", layout.characterIndex );
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "      number of glyphs %d\n", layout.numberOfGlyphs );
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  number of characters %d\n", layout.numberOfCharacters );
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "                length %f\n", layout.length );

      if( 0u == layout.numberOfGlyphs )
      {
        // The width is too small and no characters are laid-out.
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--LayoutText width too small!\n\n" );
        return false;
      }

      // Set the line position. Discard if ellipsis is enabled and the position exceeds the boundaries
      // of the box.
      penY += layout.ascender;

      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  pen y %f\n", penY );
      if( mEllipsisEnabled &&
          ( ( penY - layout.descender > layoutParameters.boundingBox.height ) ||
            ( ( mLayout == SINGLE_LINE_BOX ) &&
              ( layout.extraBearing + layout.length + layout.extraWidth > layoutParameters.boundingBox.width ) ) ) )
      {
        // Do not layout more lines if ellipsis is enabled.

        // The last line needs to be completely filled with characters.
        // Part of a word may be used.

        const Length numberOfLines = lines.Count();

        LineRun lineRun;
        LineLayout ellipsisLayout;
        if( 0u != numberOfLines )
        {
          // Get the last line and layout it again with the 'completelyFill' flag to true.
          lineRun = *( lines.Begin() + ( numberOfLines - 1u ) );

          penY -= layout.ascender - lineRun.descender;

          ellipsisLayout.glyphIndex = lineRun.glyphRun.glyphIndex;
        }
        else
        {
          lineRun.glyphRun.glyphIndex = 0u;
          ellipsisLayout.glyphIndex = 0u;
        }

        GetLineLayoutForBox( layoutParameters,
                             ellipsisLayout,
                             currentParagraphDirection,
                             true );

        lineRun.glyphRun.numberOfGlyphs = ellipsisLayout.numberOfGlyphs;
        lineRun.characterRun.characterIndex = ellipsisLayout.characterIndex;
        lineRun.characterRun.numberOfCharacters = ellipsisLayout.numberOfCharacters;
        lineRun.width = ellipsisLayout.length;
        lineRun.extraLength =  ( ellipsisLayout.wsLengthEndOfLine > 0.f ) ? ellipsisLayout.wsLengthEndOfLine - ellipsisLayout.extraWidth : 0.f;
        lineRun.ascender = ellipsisLayout.ascender;
        lineRun.descender = ellipsisLayout.descender;
        lineRun.direction = !RTL;
        lineRun.ellipsis = true;

        actualSize.width = layoutParameters.boundingBox.width;
        actualSize.height += ( lineRun.ascender + -lineRun.descender );

        SetGlyphPositions( layoutParameters.glyphsBuffer + lineRun.glyphRun.glyphIndex,
                           ellipsisLayout.numberOfGlyphs,
                           penY,
                           glyphPositions.Begin() + lineRun.glyphRun.glyphIndex );

        if( 0u != numberOfLines )
        {
          // Set the last line with the ellipsis layout.
          *( lines.Begin() + ( numberOfLines - 1u ) ) = lineRun;
        }
        else
        {
          // Push the line.
          lines.PushBack( lineRun );
        }

        break;
      }
      else
      {
        const bool isLastLine = index + layout.numberOfGlyphs == layoutParameters.totalNumberOfGlyphs;

        LineRun lineRun;
        lineRun.glyphRun.glyphIndex = index;
        lineRun.glyphRun.numberOfGlyphs = layout.numberOfGlyphs;
        lineRun.characterRun.characterIndex = layout.characterIndex;
        lineRun.characterRun.numberOfCharacters = layout.numberOfCharacters;
        if( isLastLine && !layoutParameters.isLastNewParagraph )
        {
          const float width = layout.extraBearing + layout.length + layout.extraWidth + layout.wsLengthEndOfLine;
          if( MULTI_LINE_BOX == mLayout )
          {
            lineRun.width = ( width > layoutParameters.boundingBox.width ) ? layoutParameters.boundingBox.width : width;
          }
          else
          {
            lineRun.width = width;
          }

          lineRun.extraLength = 0.f;
        }
        else
        {
          lineRun.width = layout.extraBearing + layout.length + layout.extraWidth;
          lineRun.extraLength = ( layout.wsLengthEndOfLine > 0.f ) ? layout.wsLengthEndOfLine - layout.extraWidth : 0.f;
        }
        lineRun.ascender = layout.ascender;
        lineRun.descender = layout.descender;
        lineRun.direction = !RTL;
        lineRun.ellipsis = false;

        lines.PushBack( lineRun );

        // Update the actual size.
        if( lineRun.width > actualSize.width )
        {
          actualSize.width = lineRun.width;
        }

        actualSize.height += ( lineRun.ascender + -lineRun.descender );

        SetGlyphPositions( layoutParameters.glyphsBuffer + index,
                           layout.numberOfGlyphs,
                           penY,
                           glyphPositions.Begin() + index );

        penY += -layout.descender;

        // Increase the glyph index.
        index += layout.numberOfGlyphs;

        if( isLastLine &&
            layoutParameters.isLastNewParagraph &&
            ( mLayout == MULTI_LINE_BOX ) )
        {
          // Need to add a new line with no characters but with height to increase the actualSize.height
          const GlyphInfo& glyphInfo = *( layoutParameters.glyphsBuffer + layoutParameters.totalNumberOfGlyphs - 1u );

          Text::FontMetrics fontMetrics;
          mFontClient.GetFontMetrics( glyphInfo.fontId, fontMetrics );

          LineRun lineRun;
          lineRun.glyphRun.glyphIndex = 0u;
          lineRun.glyphRun.numberOfGlyphs = 0u;
          lineRun.characterRun.characterIndex = 0u;
          lineRun.characterRun.numberOfCharacters = 0u;
          lineRun.width = 0.f;
          lineRun.ascender = fontMetrics.ascender;
          lineRun.descender = fontMetrics.descender;
          lineRun.extraLength = 0.f;
          lineRun.alignmentOffset = 0.f;
          lineRun.direction = !RTL;
          lineRun.ellipsis = false;

          actualSize.height += ( lineRun.ascender + -lineRun.descender );

          lines.PushBack( lineRun );
        }
      }
    } // end for() traversing glyphs.

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--LayoutText\n\n" );

    return true;
  }

  void ReLayoutRightToLeftLines( const LayoutParameters& layoutParameters,
                                 Vector<Vector2>& glyphPositions )
  {
    // Traverses the paragraphs with right to left characters.
    for( LineIndex lineIndex = 0u; lineIndex < layoutParameters.numberOfBidirectionalInfoRuns; ++lineIndex )
    {
      const BidirectionalLineInfoRun& bidiLine = *( layoutParameters.lineBidirectionalInfoRunsBuffer + lineIndex );

      const CharacterIndex characterVisualIndex = bidiLine.characterRun.characterIndex + *bidiLine.visualToLogicalMap;
      const GlyphInfo& glyph = *( layoutParameters.glyphsBuffer + *( layoutParameters.charactersToGlyphsBuffer + characterVisualIndex ) );

      float penX = ( 0.f > glyph.xBearing ) ? -glyph.xBearing : 0.f;
      penX += mCursorWidth; // Added to give some space to the cursor.

      Vector2* glyphPositionsBuffer = glyphPositions.Begin();

      // Traverses the characters of the right to left paragraph.
      for( CharacterIndex characterLogicalIndex = 0u;
           characterLogicalIndex < bidiLine.characterRun.numberOfCharacters;
           ++characterLogicalIndex )
      {
        // Convert the character in the logical order into the character in the visual order.
        const CharacterIndex characterVisualIndex = bidiLine.characterRun.characterIndex + *( bidiLine.visualToLogicalMap + characterLogicalIndex );

        // Get the number of glyphs of the character.
        const Length numberOfGlyphs = *( layoutParameters.glyphsPerCharacterBuffer + characterVisualIndex );

        for( GlyphIndex index = 0u; index < numberOfGlyphs; ++index )
        {
          // Convert the character in the visual order into the glyph in the visual order.
          const GlyphIndex glyphIndex = *( layoutParameters.charactersToGlyphsBuffer + characterVisualIndex ) + index;

          DALI_ASSERT_DEBUG( 0u <= glyphIndex && glyphIndex < layoutParameters.totalNumberOfGlyphs );

          const GlyphInfo& glyph = *( layoutParameters.glyphsBuffer + glyphIndex );
          Vector2& position = *( glyphPositionsBuffer + glyphIndex );

          position.x = penX + glyph.xBearing;
          penX += glyph.advance;
        }
      }
    }
  }

  void Align( const Size& layoutSize,
              Vector<LineRun>& lines )
  {
    // Traverse all lines and align the glyphs.

    for( Vector<LineRun>::Iterator it = lines.Begin(), endIt = lines.End();
         it != endIt;
         ++it )
    {
      LineRun& line = *it;
      const bool isLastLine = lines.End() == it + 1u;

      // Calculate the alignment offset accordingly with the align option,
      // the box width, line length, and the paragraphs direction.
      CalculateHorizontalAlignment( layoutSize.width,
                                    line,
                                    isLastLine );
    }
  }

  void CalculateHorizontalAlignment( float boxWidth,
                                     LineRun& line,
                                     bool isLastLine )
  {
    line.alignmentOffset = 0.f;
    const bool isRTL = RTL == line.direction;
    float lineLength = line.width;

    HorizontalAlignment alignment = mHorizontalAlignment;
    if( isRTL &&
        ( HORIZONTAL_ALIGN_CENTER != alignment ) )
    {
      if( HORIZONTAL_ALIGN_BEGIN == alignment )
      {
        alignment = HORIZONTAL_ALIGN_END;
      }
      else
      {
        alignment = HORIZONTAL_ALIGN_BEGIN;
      }
    }

    switch( alignment )
    {
      case HORIZONTAL_ALIGN_BEGIN:
      {
        line.alignmentOffset = 0.f;

        if( isRTL )
        {
          // 'Remove' the white spaces at the end of the line (which are at the beginning in visual order)
          line.alignmentOffset -= line.extraLength;

          if( isLastLine )
          {
            line.alignmentOffset += std::min( line.extraLength, boxWidth - lineLength );
          }
        }
        break;
      }
      case HORIZONTAL_ALIGN_CENTER:
      {
        if( isLastLine && !isRTL )
        {
          lineLength += line.extraLength;
          if( lineLength > boxWidth )
          {
            lineLength = boxWidth;
            line.alignmentOffset = 0.f;
            break;
          }
        }

        line.alignmentOffset = 0.5f * ( boxWidth - lineLength );

        if( isRTL )
        {
          line.alignmentOffset -= line.extraLength;

          if( isLastLine )
          {
            line.alignmentOffset += 0.5f * std::min( line.extraLength, boxWidth - lineLength );
          }
        }

        line.alignmentOffset = floorf( line.alignmentOffset ); // try to avoid pixel alignment.
        break;
      }
      case HORIZONTAL_ALIGN_END:
      {
        if( isLastLine && !isRTL )
        {
          lineLength += line.extraLength;
          if( lineLength > boxWidth )
          {
            line.alignmentOffset = 0.f;
            break;
          }
        }

        if( isRTL )
        {
          lineLength += line.extraLength;
        }

        line.alignmentOffset = boxWidth - lineLength;
        break;
      }
    }
  }

  LayoutEngine::Layout mLayout;
  LayoutEngine::HorizontalAlignment mHorizontalAlignment;
  LayoutEngine::VerticalAlignment mVerticalAlignment;
  float mCursorWidth;

  TextAbstraction::FontClient mFontClient;

  bool mEllipsisEnabled:1;
};

LayoutEngine::LayoutEngine()
: mImpl( NULL )
{
  mImpl = new LayoutEngine::Impl();
}

LayoutEngine::~LayoutEngine()
{
  delete mImpl;
}

void LayoutEngine::SetLayout( Layout layout )
{
  mImpl->mLayout = layout;
}

unsigned int LayoutEngine::GetLayout() const
{
  return mImpl->mLayout;
}

void LayoutEngine::SetTextEllipsisEnabled( bool enabled )
{
  mImpl->mEllipsisEnabled = enabled;
}

bool LayoutEngine::GetTextEllipsisEnabled() const
{
  return mImpl->mEllipsisEnabled;
}

void LayoutEngine::SetHorizontalAlignment( HorizontalAlignment alignment )
{
  mImpl->mHorizontalAlignment = alignment;
}

LayoutEngine::HorizontalAlignment LayoutEngine::GetHorizontalAlignment() const
{
  return mImpl->mHorizontalAlignment;
}

void LayoutEngine::SetVerticalAlignment( VerticalAlignment alignment )
{
  mImpl->mVerticalAlignment = alignment;
}

LayoutEngine::VerticalAlignment LayoutEngine::GetVerticalAlignment() const
{
  return mImpl->mVerticalAlignment;
}

void LayoutEngine::SetCursorWidth( int width )
{
  mImpl->mCursorWidth = static_cast<float>( width );
}

int LayoutEngine::GetCursorWidth() const
{
  return static_cast<int>( mImpl->mCursorWidth );
}

bool LayoutEngine::LayoutText( const LayoutParameters& layoutParameters,
                               Vector<Vector2>& glyphPositions,
                               Vector<LineRun>& lines,
                               Size& actualSize )
{
  return mImpl->LayoutText( layoutParameters,
                            glyphPositions,
                            lines,
                            actualSize );
}

void LayoutEngine::ReLayoutRightToLeftLines( const LayoutParameters& layoutParameters,
                                             Vector<Vector2>& glyphPositions )
{
  mImpl->ReLayoutRightToLeftLines( layoutParameters,
                                   glyphPositions );
}

void LayoutEngine::Align( const Size& layoutSize,
                          Vector<LineRun>& lines )
{
  mImpl->Align( layoutSize,
                lines );
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
