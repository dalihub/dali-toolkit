/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// FILE HEADER
#include <dali-toolkit/internal/text/cursor-helper-functions.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/glyph-metrics-helper.h>

namespace
{

#if defined(DEBUG_ENABLED)
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

} // namespace

namespace Dali
{

namespace Toolkit
{

namespace Text
{

LineIndex GetClosestLine( VisualModelPtr visualModel,
                          float visualY )
{
  float totalHeight = 0.f;
  LineIndex lineIndex = 0u;

  const Vector<LineRun>& lines = visualModel->mLines;
  for( LineIndex endLine = lines.Count();
       lineIndex < endLine;
       ++lineIndex )
  {
    const LineRun& lineRun = lines[lineIndex];
    totalHeight += lineRun.ascender + -lineRun.descender;
    if( visualY < totalHeight )
    {
      return lineIndex;
    }
  }

  if( lineIndex == 0 )
  {
    return 0;
  }

  return lineIndex-1;
}

CharacterIndex GetClosestCursorIndex( VisualModelPtr visualModel,
                                      LogicalModelPtr logicalModel,
                                      MetricsPtr metrics,
                                      float visualX,
                                      float visualY )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "GetClosestCursorIndex, closest visualX %f visualY %f\n", visualX, visualY );

  CharacterIndex logicalIndex = 0u;

  const Length numberOfGlyphs = visualModel->mGlyphs.Count();
  const Length numberOfLines  = visualModel->mLines.Count();
  if( ( 0 == numberOfGlyphs ) ||
      ( 0 == numberOfLines ) )
  {
    return logicalIndex;
  }

  // Find which line is closest.
  const LineIndex lineIndex = Text::GetClosestLine( visualModel,
                                                    visualY );
  const LineRun& line = visualModel->mLines[lineIndex];

  // Get the positions of the glyphs.
  const Vector<Vector2>& positions = visualModel->mGlyphPositions;
  const Vector2* const positionsBuffer = positions.Begin();

  // Get the character to glyph conversion table.
  const GlyphIndex* const charactersToGlyphBuffer = visualModel->mCharactersToGlyph.Begin();

  // Get the glyphs per character table.
  const Length* const glyphsPerCharacterBuffer = visualModel->mGlyphsPerCharacter.Begin();

  // Get the glyph's info buffer.
  const GlyphInfo* const glyphInfoBuffer = visualModel->mGlyphs.Begin();

  const CharacterIndex startCharacter = line.characterRun.characterIndex;
  const CharacterIndex endCharacter   = line.characterRun.characterIndex + line.characterRun.numberOfCharacters;
  DALI_ASSERT_DEBUG( endCharacter <= logicalModel->mText.Count() && "Invalid line info" );

  // Whether this line is a bidirectional line.
  const bool bidiLineFetched = logicalModel->FetchBidirectionalLineInfo( startCharacter );

  // Whether there is a hit on a glyph.
  bool matched = false;

  // Traverses glyphs in visual order. To do that use the visual to logical conversion table.
  CharacterIndex visualIndex = startCharacter;
  Length numberOfCharacters = 0u;
  for( ; !matched && ( visualIndex < endCharacter ); ++visualIndex )
  {
    // The character in logical order.
    const CharacterIndex characterLogicalOrderIndex = ( bidiLineFetched ? logicalModel->GetLogicalCharacterIndex( visualIndex ) : visualIndex );

    // Get the script of the character.
    const Script script = logicalModel->GetScript( characterLogicalOrderIndex );

    // The number of glyphs for that character
    const Length numberOfGlyphs = *( glyphsPerCharacterBuffer + characterLogicalOrderIndex );
    ++numberOfCharacters;


    if( 0u != numberOfGlyphs )
    {
      // Get the first character/glyph of the group of glyphs.
      const CharacterIndex firstVisualCharacterIndex = 1u + visualIndex - numberOfCharacters;
      const CharacterIndex firstLogicalCharacterIndex = ( bidiLineFetched ? logicalModel->GetLogicalCharacterIndex( firstVisualCharacterIndex ) : firstVisualCharacterIndex );
      const GlyphIndex firstLogicalGlyphIndex = *( charactersToGlyphBuffer + firstLogicalCharacterIndex );

      // Get the metrics for the group of glyphs.
      GlyphMetrics glyphMetrics;
      GetGlyphsMetrics( firstLogicalGlyphIndex,
                        numberOfGlyphs,
                        glyphMetrics,
                        glyphInfoBuffer,
                        metrics );

      // Get the position of the first glyph.
      const Vector2& position = *( positionsBuffer + firstLogicalGlyphIndex );

      // Whether the glyph can be split, like Latin ligatures fi, ff or Arabic ﻻ.
      const bool isInterglyphIndex = ( numberOfCharacters > numberOfGlyphs ) && HasLigatureMustBreak( script );
      const Length numberOfBlocks = isInterglyphIndex ? numberOfCharacters : 1u;
      const float glyphAdvance = glyphMetrics.advance / static_cast<float>( numberOfBlocks );

      GlyphIndex index = 0u;
      for( ; !matched && ( index < numberOfBlocks ); ++index )
      {
        // Find the mid-point of the area containing the glyph
        const float glyphCenter = -glyphMetrics.xBearing + position.x + ( static_cast<float>( index ) + 0.5f ) * glyphAdvance;

        if( visualX < glyphCenter )
        {
          matched = true;
          break;
        }
      }

      if( matched )
      {
        visualIndex = firstVisualCharacterIndex + index;
        break;
      }

      numberOfCharacters = 0u;
    }

  }

  // Return the logical position of the cursor in characters.

  if( !matched )
  {
    visualIndex = endCharacter;
  }

  logicalIndex = ( bidiLineFetched ? logicalModel->GetLogicalCursorIndex( visualIndex ) : visualIndex );
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "closest visualIndex %d logicalIndex %d\n", visualIndex, logicalIndex );

  DALI_ASSERT_DEBUG( ( logicalIndex <= logicalModel->mText.Count() && logicalIndex >= 0 ) && "GetClosestCursorIndex - Out of bounds index" );

  return logicalIndex;
}


void GetCursorPosition( VisualModelPtr visualModel,
                        LogicalModelPtr logicalModel,
                        MetricsPtr metrics,
                        CharacterIndex logical,
                        CursorInfo& cursorInfo )
{
  // TODO: Check for multiline with \n, etc...

  const Length numberOfCharacters = logicalModel->mText.Count();

  // Check if the logical position is the first or the last one of the text.
  const bool isFirstPosition = 0u == logical;
  const bool isLastPosition = numberOfCharacters == logical;

  // 'logical' is the logical 'cursor' index.
  // Get the next and current logical 'character' index.
  const CharacterIndex nextCharacterIndex = logical;
  const CharacterIndex characterIndex = isFirstPosition ? logical : logical - 1u;

  // Get the direction of the character and the next one.
  const CharacterDirection* const modelCharacterDirectionsBuffer = ( 0u != logicalModel->mCharacterDirections.Count() ) ? logicalModel->mCharacterDirections.Begin() : NULL;

  CharacterDirection isCurrentRightToLeft = false;
  CharacterDirection isNextRightToLeft = false;
  if( NULL != modelCharacterDirectionsBuffer ) // If modelCharacterDirectionsBuffer is NULL, it means the whole text is left to right.
  {
    isCurrentRightToLeft = *( modelCharacterDirectionsBuffer + characterIndex );
    isNextRightToLeft = *( modelCharacterDirectionsBuffer + nextCharacterIndex );
  }

  // Get the line where the character is laid-out.
  const LineRun* const modelLines = visualModel->mLines.Begin();

  const LineIndex lineIndex = visualModel->GetLineOfCharacter( characterIndex );
  const LineRun& line = *( modelLines + lineIndex );

  // Get the paragraph's direction.
  const CharacterDirection isRightToLeftParagraph = line.direction;

  // Check whether there is an alternative position:

  cursorInfo.isSecondaryCursor = ( !isLastPosition && ( isCurrentRightToLeft != isNextRightToLeft ) ) ||
                                 ( isLastPosition && ( isRightToLeftParagraph != isCurrentRightToLeft ) );

  // Set the line offset and height.
  cursorInfo.lineOffset = 0.f;
  cursorInfo.lineHeight = line.ascender + -line.descender;

  // Calculate the primary cursor.

  CharacterIndex index = characterIndex;
  if( cursorInfo.isSecondaryCursor )
  {
    // If there is a secondary position, the primary cursor may be in a different place than the logical index.

    if( isLastPosition )
    {
      // The position of the cursor after the last character needs special
      // care depending on its direction and the direction of the paragraph.

      // Need to find the first character after the last character with the paragraph's direction.
      // i.e l0 l1 l2 r0 r1 should find r0.

      // TODO: check for more than one line!
      index = isRightToLeftParagraph ? line.characterRun.characterIndex : line.characterRun.characterIndex + line.characterRun.numberOfCharacters - 1u;
      index = logicalModel->GetLogicalCharacterIndex( index );
    }
    else
    {
      index = ( isRightToLeftParagraph == isCurrentRightToLeft ) ? characterIndex : nextCharacterIndex;
    }
  }

  const GlyphIndex* const charactersToGlyphBuffer = visualModel->mCharactersToGlyph.Begin();
  const Length* const glyphsPerCharacterBuffer = visualModel->mGlyphsPerCharacter.Begin();
  const Length* const charactersPerGlyphBuffer = visualModel->mCharactersPerGlyph.Begin();
  const CharacterIndex* const glyphsToCharactersBuffer = visualModel->mGlyphsToCharacters.Begin();
  const Vector2* const glyphPositionsBuffer = visualModel->mGlyphPositions.Begin();
  const GlyphInfo* const glyphInfoBuffer = visualModel->mGlyphs.Begin();

  // Convert the cursor position into the glyph position.
  const GlyphIndex primaryGlyphIndex = *( charactersToGlyphBuffer + index );
  const Length primaryNumberOfGlyphs = *( glyphsPerCharacterBuffer + index );
  const Length primaryNumberOfCharacters = *( charactersPerGlyphBuffer + primaryGlyphIndex );

  // Get the metrics for the group of glyphs.
  GlyphMetrics glyphMetrics;
  GetGlyphsMetrics( primaryGlyphIndex,
                    primaryNumberOfGlyphs,
                    glyphMetrics,
                    glyphInfoBuffer,
                    metrics );

  // Whether to add the glyph's advance to the cursor position.
  // i.e if the paragraph is left to right and the logical cursor is zero, the position is the position of the first glyph and the advance is not added,
  //     if the logical cursor is one, the position is the position of the first glyph and the advance is added.
  // A 'truth table' was build and an online Karnaugh map tool was used to simplify the logic.
  //
  // FLCP A
  // ------
  // 0000 1
  // 0001 1
  // 0010 0
  // 0011 0
  // 0100 1
  // 0101 0
  // 0110 1
  // 0111 0
  // 1000 0
  // 1001 x
  // 1010 x
  // 1011 1
  // 1100 x
  // 1101 x
  // 1110 x
  // 1111 x
  //
  // Where F -> isFirstPosition
  //       L -> isLastPosition
  //       C -> isCurrentRightToLeft
  //       P -> isRightToLeftParagraph
  //       A -> Whether to add the glyph's advance.

  const bool addGlyphAdvance = ( ( isLastPosition && !isRightToLeftParagraph ) ||
                                 ( isFirstPosition && isRightToLeftParagraph ) ||
                                 ( !isFirstPosition && !isLastPosition && !isCurrentRightToLeft ) );

  float glyphAdvance = addGlyphAdvance ? glyphMetrics.advance : 0.f;

  if( !isLastPosition &&
      ( primaryNumberOfCharacters > 1u ) )
  {
    const CharacterIndex firstIndex = *( glyphsToCharactersBuffer + primaryGlyphIndex );

    bool isCurrentRightToLeft = false;
    if( NULL != modelCharacterDirectionsBuffer ) // If modelCharacterDirectionsBuffer is NULL, it means the whole text is left to right.
    {
      isCurrentRightToLeft = *( modelCharacterDirectionsBuffer + index );
    }

    Length numberOfGlyphAdvance = ( isFirstPosition ? 0u : 1u ) + characterIndex - firstIndex;
    if( isCurrentRightToLeft )
    {
      numberOfGlyphAdvance = primaryNumberOfCharacters - numberOfGlyphAdvance;
    }

    glyphAdvance = static_cast<float>( numberOfGlyphAdvance ) * glyphMetrics.advance / static_cast<float>( primaryNumberOfCharacters );
  }

  // Get the glyph position and x bearing.
  const Vector2& primaryPosition = *( glyphPositionsBuffer + primaryGlyphIndex );

  // Set the primary cursor's height.
  cursorInfo.primaryCursorHeight = cursorInfo.isSecondaryCursor ? 0.5f * glyphMetrics.fontHeight : glyphMetrics.fontHeight;

  // Set the primary cursor's position.
  cursorInfo.primaryPosition.x = -glyphMetrics.xBearing + primaryPosition.x + glyphAdvance;
  cursorInfo.primaryPosition.y = line.ascender - glyphMetrics.ascender;

  // Calculate the secondary cursor.

  if( cursorInfo.isSecondaryCursor )
  {
    // Set the secondary cursor's height.
    cursorInfo.secondaryCursorHeight = 0.5f * glyphMetrics.fontHeight;

    CharacterIndex index = characterIndex;
    if( !isLastPosition )
    {
      index = ( isRightToLeftParagraph == isCurrentRightToLeft ) ? nextCharacterIndex : characterIndex;
    }

    const GlyphIndex secondaryGlyphIndex = *( charactersToGlyphBuffer + index );
    const Length secondaryNumberOfGlyphs = *( glyphsPerCharacterBuffer + index );

    const Vector2& secondaryPosition = *( glyphPositionsBuffer + secondaryGlyphIndex );

    GetGlyphsMetrics( secondaryGlyphIndex,
                      secondaryNumberOfGlyphs,
                      glyphMetrics,
                      glyphInfoBuffer,
                      metrics );

    // Set the secondary cursor's position.
    cursorInfo.secondaryPosition.x = -glyphMetrics.xBearing + secondaryPosition.x + ( isCurrentRightToLeft ? 0.f : glyphMetrics.advance );
    cursorInfo.secondaryPosition.y = cursorInfo.lineHeight - cursorInfo.secondaryCursorHeight - line.descender - ( glyphMetrics.fontHeight - glyphMetrics.ascender );
  }
}

void FindSelectionIndices( VisualModelPtr visualModel,
                           LogicalModelPtr logicalModel,
                           MetricsPtr metrics,
                           float visualX,
                           float visualY,
                           CharacterIndex& startIndex,
                           CharacterIndex& endIndex )
{
  CharacterIndex hitCharacter = Text::GetClosestCursorIndex( visualModel,
                                                             logicalModel,
                                                             metrics,
                                                             visualX,
                                                             visualY );
  DALI_ASSERT_DEBUG( hitCharacter <= logicalModel->mText.Count() && "GetClosestCursorIndex returned out of bounds index" );

  if( logicalModel->mText.Count() == 0 )
  {
    return;  // if model empty
  }

  if( hitCharacter >= logicalModel->mText.Count() )
  {
    // Closest hit character is the last character.
    if( hitCharacter ==  logicalModel->mText.Count() )
    {
      hitCharacter--; //Hit character index set to last character in logical model
    }
    else
    {
      // hitCharacter is out of bounds
      return;
    }
  }

  startIndex = hitCharacter;
  endIndex = hitCharacter;
  bool isHitCharacterWhitespace = TextAbstraction::IsWhiteSpace( logicalModel->mText[hitCharacter] );

  // Find the start and end of the text
  for( startIndex = hitCharacter; startIndex > 0; --startIndex )
  {
    if( isHitCharacterWhitespace != TextAbstraction::IsWhiteSpace( logicalModel->mText[ startIndex-1 ] ) )
    {
      break;
    }
  }
  const CharacterIndex pastTheEnd = logicalModel->mText.Count();
  for( endIndex = hitCharacter + 1u; endIndex < pastTheEnd; ++endIndex )
  {
    if( isHitCharacterWhitespace != TextAbstraction::IsWhiteSpace( logicalModel->mText[ endIndex ] ) )
    {
      break;
    }
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
