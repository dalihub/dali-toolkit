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
#include <dali/public-api/text-abstraction/font-client.h>

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
    numberOfCharacters( 0u ),
    numberOfGlyphs( 0u ),
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
    numberOfCharacters = 0u;
    numberOfGlyphs = 0u;
    length = 0.f;
    widthAdvanceDiff = 0.f;
    wsLengthEndOfLine = 0.f;
    ascender = 0.f;
    descender = MAX_FLOAT;
  }

  GlyphIndex     glyphIndex;         ///< Index of the first glyph to be laid-out.
  CharacterIndex characterIndex;     ///< Index of the first character to be laid-out.
  Length         numberOfCharacters; ///< The number of characters which fit in one line.
  Length         numberOfGlyphs;     ///< The number of glyph which fit in one line.
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
    mVerticalAlignment( LayoutEngine::VERTICAL_ALIGN_TOP )
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
   */
  void GetLineLayoutForBox( const LayoutParameters& parameters,
                            LineLayout& lineLayout )
  {
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
      if( isMultiline && oneWordLaidOut && !isWhiteSpace &&
          ( lineLayout.length + lineLayout.wsLengthEndOfLine + tmpLineLayout.length + tmpLineLayout.widthAdvanceDiff > boundingBoxWidth ) )
      {
        // Current word does not fit in the box's width.
        return;
      }

      if( ( isMultiline || isLastGlyph ) &&
          ( TextAbstraction::LINE_MUST_BREAK == lineBreakInfo ) )
      {
        // Must break the line. Update the line layout and return.
        MergeLineLayout( lineLayout, tmpLineLayout );

        return;
      }

      if( isMultiline &&
          ( TextAbstraction::WORD_BREAK == wordBreakInfo ) )
      {
        if( !oneWordLaidOut && !isWhiteSpace )
        {
          oneWordLaidOut = true;
        }

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
  }

  bool LayoutText( const LayoutParameters& layoutParameters,
                   Vector<Vector2>& glyphPositions,
                   Vector<LineRun>& lines,
                   Size& actualSize )
  {
    Vector2* glyphPositionsBuffer = glyphPositions.Begin();

    float penY = 0.f;
    for( GlyphIndex index = 0u; index < layoutParameters.totalNumberOfGlyphs; )
    {
      // Get the layout for the line.
      LineLayout layout;
      layout.glyphIndex = index;
      GetLineLayoutForBox( layoutParameters,
                           layout );

      if( 0u == layout.numberOfGlyphs )
      {
        // The width is too small and no characters are laid-out.
        return false;
      }

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

      lines.PushBack( lineRun );

      // Update the actual size.
      if( lineRun.width > actualSize.width )
      {
        actualSize.width = lineRun.width;
      }

      actualSize.height += ( lineRun.ascender + -lineRun.descender );

      // Traverse the glyphs and set the positions.

      penY += layout.ascender;

      // Check if the x bearing of the first character is negative.
      // If it has a negative x bearing, it will exceed the boundaries of the actor,
      // so the penX position needs to be moved to the right.
      float penX = 0.f;

      const GlyphInfo& glyph = *( layoutParameters.glyphsBuffer + index );
      if( 0.f > glyph.xBearing )
      {
        penX = -glyph.xBearing;
      }

      for( GlyphIndex i = index; i < index + layout.numberOfGlyphs; ++i )
      {
        const GlyphInfo& glyph = *( layoutParameters.glyphsBuffer + i );
        Vector2& position = *( glyphPositionsBuffer + i );

        position.x = penX + glyph.xBearing;
        position.y = penY - glyph.yBearing;

        penX += glyph.advance;
      }

      penY += -layout.descender;

      // Increase the glyph index.
      index += layout.numberOfGlyphs;
    }

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
