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
#include <dali/public-api/math/vector2.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/integration-api/debug.h>

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
    widthAdvanceDiff( 0.f ),
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
    widthAdvanceDiff = 0.f;
    wsLengthEndOfLine = 0.f;
    ascender = 0.f;
    descender = MAX_FLOAT;
  }

  GlyphIndex     glyphIndex;         ///< Index of the first glyph to be laid-out.
  CharacterIndex characterIndex;     ///< Index of the first character to be laid-out.
  Length         numberOfGlyphs;     ///< The number of glyph which fit in one line.
  Length         numberOfCharacters; ///< The number of characters which fit in one line.
  float          length;             ///< The length of the glyphs which fit in one line.
  float          widthAdvanceDiff;   ///< The difference between the xBearing + width and the advance of the last glyph.
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
      lineLayout.widthAdvanceDiff = tmpLineLayout.widthAdvanceDiff;
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
   * @param[in] parameters The layout parameters.
   * @param[out] lineLayout The line layout.
   * @param[in] completelyFill Whether to completely fill the line ( even if the last word exceeds the boundaries ).
   */
  void GetLineLayoutForBox( const LayoutParameters& parameters,
                            LineLayout& lineLayout,
                            bool completelyFill )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "-->GetLineLayoutForBox\n" );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  initial glyph index : %d\n", lineLayout.glyphIndex );
    // Stores temporary line layout which has not been added to the final line layout.
    LineLayout tmpLineLayout;

    const bool isMultiline = mLayout == MULTI_LINE_BOX;
    const GlyphIndex lastGlyphIndex = parameters.totalNumberOfGlyphs - 1u;

    // If the first glyph has a negative bearing its absolute value needs to be added to the line length.
    // In the case the line starts with a right to left character the bearing needs to be substracted to the line length.
    const GlyphInfo& glyphInfo = *( parameters.glyphsBuffer + lineLayout.glyphIndex );
    float initialHorizontalBearing = glyphInfo.xBearing;

    lineLayout.characterIndex = *( parameters.glyphsToCharactersBuffer + lineLayout.glyphIndex );
    const CharacterDirection firstCharacterDirection = ( NULL == parameters.characterDirectionBuffer ) ? false : *( parameters.characterDirectionBuffer + lineLayout.characterIndex );

    if( RTL == firstCharacterDirection )
    {
      initialHorizontalBearing = -initialHorizontalBearing;

      if( 0.f < glyphInfo.xBearing )
      {
        tmpLineLayout.length = glyphInfo.xBearing;
        initialHorizontalBearing = 0.f;
      }
    }
    else
    {
      if( 0.f > glyphInfo.xBearing )
      {
        tmpLineLayout.length = -glyphInfo.xBearing;
        initialHorizontalBearing = 0.f;
      }
    }

    // Calculate the line height if there is no characters.
    FontId lastFontId = glyphInfo.fontId;
    UpdateLineHeight( lastFontId, tmpLineLayout );

    const float boundingBoxWidth = parameters.boundingBox.width - initialHorizontalBearing;

    bool oneWordLaidOut = false;

    for( GlyphIndex glyphIndex = lineLayout.glyphIndex;
         glyphIndex < parameters.totalNumberOfGlyphs;
         ++glyphIndex )
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  glyph index : %d\n", glyphIndex );
      const bool isLastGlyph = glyphIndex == lastGlyphIndex;

      // Get the glyph info.
      const GlyphInfo& glyphInfo = *( parameters.glyphsBuffer + glyphIndex );

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
      const float previousTmpWidthAdvanceDiff = tmpLineLayout.widthAdvanceDiff;

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
        if( RTL == firstCharacterDirection )
        {
          tmpLineLayout.widthAdvanceDiff = -glyphInfo.xBearing;
        }
        else
        {
          tmpLineLayout.widthAdvanceDiff = glyphInfo.xBearing + glyphInfo.width - glyphInfo.advance;
        }

        // Clear the white space length at the end of the line.
        tmpLineLayout.wsLengthEndOfLine = 0.f;
      }

      // Check if the accumulated length fits in the width of the box.
      if( ( completelyFill || isMultiline ) && !isWhiteSpace &&
          ( lineLayout.length + lineLayout.wsLengthEndOfLine + tmpLineLayout.length + tmpLineLayout.widthAdvanceDiff > boundingBoxWidth ) )
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
            tmpLineLayout.widthAdvanceDiff = previousTmpWidthAdvanceDiff;
          }

          // Add part of the word to the line layout.
          MergeLineLayout( lineLayout, tmpLineLayout );
        }
        else
        {
          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  Current word does not fit.\n" );
        }
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--GetLineLayoutForBox\n" );
        return;
      }

      if( ( isMultiline || isLastGlyph ) &&
          ( TextAbstraction::LINE_MUST_BREAK == lineBreakInfo ) )
      {
        // Must break the line. Update the line layout and return.
        MergeLineLayout( lineLayout, tmpLineLayout );

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

      // Check if the font of the current glyph is the same of the previous one.
      // If it's different the ascender and descender need to be updated.
      if( lastFontId != glyphInfo.fontId )
      {
        UpdateLineHeight( glyphInfo.fontId, tmpLineLayout );
        lastFontId = glyphInfo.fontId;
      }
    }
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
    float penX = 0.f;

    const GlyphInfo& glyph = *glyphsBuffer;
    if( 0.f > glyph.xBearing )
    {
      penX = -glyph.xBearing;
    }

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

    float penY = 0.f;
    for( GlyphIndex index = 0u; index < layoutParameters.totalNumberOfGlyphs; )
    {
      // Get the layout for the line.
      LineLayout layout;
      layout.glyphIndex = index;
      GetLineLayoutForBox( layoutParameters,
                           layout,
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
              ( layout.length + layout.widthAdvanceDiff > layoutParameters.boundingBox.width ) ) ) )
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

          ellipsisLayout.glyphIndex = lineRun.glyphIndex;
        }
        else
        {
          lineRun.glyphIndex = 0u;
          ellipsisLayout.glyphIndex = 0u;
        }

        GetLineLayoutForBox( layoutParameters,
                             ellipsisLayout,
                             true );

        lineRun.numberOfGlyphs = ellipsisLayout.numberOfGlyphs;
        lineRun.characterRun.characterIndex = ellipsisLayout.characterIndex;
        lineRun.characterRun.numberOfCharacters = ellipsisLayout.numberOfCharacters;
        lineRun.width = layoutParameters.boundingBox.width;
        lineRun.ascender = ellipsisLayout.ascender;
        lineRun.descender = ellipsisLayout.descender;
        lineRun.extraLength = ellipsisLayout.wsLengthEndOfLine > 0.f ? ellipsisLayout.wsLengthEndOfLine - ellipsisLayout.widthAdvanceDiff : 0.f;
        lineRun.ellipsis = true;

        actualSize.width = layoutParameters.boundingBox.width;
        actualSize.height += ( lineRun.ascender + -lineRun.descender );

        SetGlyphPositions( layoutParameters.glyphsBuffer + lineRun.glyphIndex,
                           ellipsisLayout.numberOfGlyphs,
                           penY,
                           glyphPositions.Begin() + lineRun.glyphIndex );

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
        LineRun lineRun;
        lineRun.glyphIndex = index;
        lineRun.numberOfGlyphs = layout.numberOfGlyphs;
        lineRun.characterRun.characterIndex = layout.characterIndex;
        lineRun.characterRun.numberOfCharacters = layout.numberOfCharacters;
        lineRun.width = layout.length + layout.widthAdvanceDiff;
        lineRun.ascender = layout.ascender;
        lineRun.descender = layout.descender;
        lineRun.extraLength = layout.wsLengthEndOfLine > 0.f ? layout.wsLengthEndOfLine - layout.widthAdvanceDiff : 0.f;
        lineRun.direction = false;
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
      }
    }

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

      float penX = 0.f;

      const CharacterIndex characterVisualIndex = bidiLine.characterRun.characterIndex + *bidiLine.visualToLogicalMap;
      const GlyphInfo& glyph = *( layoutParameters.glyphsBuffer + *( layoutParameters.charactersToGlyphsBuffer + characterVisualIndex ) );

      penX = -glyph.xBearing;

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

  void Align( const LayoutParameters& layoutParameters,
              const Size& layoutSize,
              const Vector<LineRun>& lines,
              Vector<Vector2>& glyphPositions )
  {
    Vector2* glyphPositionsBuffer = glyphPositions.Begin();

    // Traverse all lines and align the glyphs.
    // LayoutParameters contains bidirectional info for those lines with
    // right to left text, this info includes the paragraph's direction.

    LineIndex bidiLineIndex = 0u;
    for( Vector<LineRun>::ConstIterator it = lines.Begin(), endIt = lines.End();
         it != endIt;
         ++it )
    {
      const LineRun& line = *it;

      // 1) Get the paragrap's direction.
      bool paragraphDirection = false;

      // Check if there is any right to left line.
      if( ( NULL != layoutParameters.lineBidirectionalInfoRunsBuffer ) &&
          ( bidiLineIndex < layoutParameters.numberOfBidirectionalInfoRuns ) )
      {
        const BidirectionalLineInfoRun* bidiLine = layoutParameters.lineBidirectionalInfoRunsBuffer + bidiLineIndex;

        // Get the right to left line that match with current line.
        while( ( line.characterRun.characterIndex > bidiLine->characterRun.characterIndex ) &&
               ( bidiLineIndex < layoutParameters.numberOfBidirectionalInfoRuns ) )
        {
          ++bidiLineIndex;
          bidiLine = layoutParameters.lineBidirectionalInfoRunsBuffer + bidiLineIndex;
        }

        if( line.characterRun.characterIndex == bidiLine->characterRun.characterIndex )
        {
          paragraphDirection = bidiLine->direction;
        }
      }

      // 2) Calculate the alignment offset accordingly with the align option,
      //    the box width, line length, and the paragraphs direction.
      float alignOffset = CalculateHorizontalAlignment( layoutSize.width,
                                                        line.width,
                                                        line.extraLength,
                                                        paragraphDirection );

      // 3) Traverse all glyphs and update the 'x' position.
      for( GlyphIndex index = line.glyphIndex,
             endIndex = line.glyphIndex + line.numberOfGlyphs;
           index < endIndex;
           ++index )
      {
        Vector2& position = *( glyphPositionsBuffer + index );

        position.x += alignOffset;
      }
    }
  }

  float CalculateHorizontalAlignment( float boxWidth,
                                      float lineLength,
                                      float extraLength,
                                      bool paragraphDirection )
  {
    float offset = 0.f;

    HorizontalAlignment alignment = mHorizontalAlignment;
    if( paragraphDirection &&
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
        offset = 0.f;
        break;
      }
      case HORIZONTAL_ALIGN_CENTER:
      {
        offset = 0.5f * ( boxWidth - lineLength );
        const int intOffset = static_cast<int>( offset ); // try to avoid pixel alignment.
        offset = static_cast<float>( intOffset );
        break;
      }
      case HORIZONTAL_ALIGN_END:
      {
        offset = boxWidth - lineLength;
        break;
      }
    }

    if( paragraphDirection )
    {
      offset -= extraLength;
    }

    return offset;
  }

  LayoutEngine::Layout mLayout;
  LayoutEngine::HorizontalAlignment mHorizontalAlignment;
  LayoutEngine::VerticalAlignment mVerticalAlignment;

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

void LayoutEngine::Align( const LayoutParameters& layoutParameters,
                          const Size& layoutSize,
                          const Vector<LineRun>& lines,
                          Vector<Vector2>& glyphPositions )
{
  mImpl->Align( layoutParameters,
                layoutSize,
                lines,
                glyphPositions );
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
