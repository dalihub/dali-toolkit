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

const Dali::Toolkit::Text::CharacterDirection LTR = false; ///< Left To Right direction.

} //namespace

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

  for( Vector<LineRun>::ConstIterator it = lines.Begin(),
         endIt = lines.End();
       it != endIt;
       ++it, ++lineIndex )
  {
    const LineRun& lineRun = *it;

    // The line height is the addition of the line ascender and the line descender.
    // However, the line descender has a negative value, hence the subtraction.
    totalHeight += lineRun.ascender - lineRun.descender;

    if( visualY < totalHeight )
    {
      return lineIndex;
    }
  }

  if( lineIndex == 0u )
  {
    return 0;
  }

  return lineIndex-1;
}

float CalculateLineOffset( const Vector<LineRun>& lines,
                           LineIndex lineIndex )
{
  float offset = 0.f;

  for( Vector<LineRun>::ConstIterator it = lines.Begin(),
         endIt = lines.Begin() + lineIndex;
       it != endIt;
       ++it )
  {
    const LineRun& lineRun = *it;

    // The line height is the addition of the line ascender and the line descender.
    // However, the line descender has a negative value, hence the subtraction.
    offset += lineRun.ascender - lineRun.descender;
  }

  return offset;
}

CharacterIndex GetClosestCursorIndex( VisualModelPtr visualModel,
                                      LogicalModelPtr logicalModel,
                                      MetricsPtr metrics,
                                      float visualX,
                                      float visualY )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "GetClosestCursorIndex, closest visualX %f visualY %f\n", visualX, visualY );

  CharacterIndex logicalIndex = 0u;

  const Length totalNumberOfGlyphs = visualModel->mGlyphs.Count();
  const Length totalNumberOfLines  = visualModel->mLines.Count();
  if( ( 0 == totalNumberOfGlyphs ) ||
      ( 0 == totalNumberOfLines ) )
  {
    return logicalIndex;
  }

  // Find which line is closest.
  const LineIndex lineIndex = Text::GetClosestLine( visualModel,
                                                    visualY );

  // Convert from text's coords to line's coords.
  const LineRun& line = *( visualModel->mLines.Begin() + lineIndex );

  // Transform the tap point from text's coords to line's coords.
  visualX -= line.alignmentOffset;

  // Get the positions of the glyphs.
  const Vector2* const positionsBuffer = visualModel->mGlyphPositions.Begin();

  // Get the character to glyph conversion table.
  const GlyphIndex* const charactersToGlyphBuffer = visualModel->mCharactersToGlyph.Begin();

  // Get the glyphs per character table.
  const Length* const glyphsPerCharacterBuffer = visualModel->mGlyphsPerCharacter.Begin();

  // Get the characters per glyph table.
  const Length* const charactersPerGlyphBuffer = visualModel->mCharactersPerGlyph.Begin();

  // Get the glyph's info buffer.
  const GlyphInfo* const glyphInfoBuffer = visualModel->mGlyphs.Begin();

  const CharacterIndex startCharacter = line.characterRun.characterIndex;
  const CharacterIndex endCharacter   = line.characterRun.characterIndex + line.characterRun.numberOfCharacters;
  DALI_ASSERT_DEBUG( endCharacter <= logicalModel->mText.Count() && "Invalid line info" );

  // Whether this line is a bidirectional line.
  const bool bidiLineFetched = logicalModel->FetchBidirectionalLineInfo( startCharacter );

  // The character's direction buffer.
  const CharacterDirection* const directionsBuffer = bidiLineFetched ? logicalModel->mCharacterDirections.Begin() : NULL;

  // Whether there is a hit on a glyph.
  bool matched = false;

  // Traverses glyphs in visual order. To do that use the visual to logical conversion table.
  CharacterIndex visualIndex = startCharacter;
  Length numberOfVisualCharacters = 0u;
  for( ; visualIndex < endCharacter; ++visualIndex )
  {
    // The character in logical order.
    const CharacterIndex characterLogicalOrderIndex = ( bidiLineFetched ? logicalModel->GetLogicalCharacterIndex( visualIndex ) : visualIndex );
    const CharacterDirection direction = ( bidiLineFetched ? *( directionsBuffer + characterLogicalOrderIndex ) : LTR );

    // The number of glyphs for that character
    const Length numberOfGlyphs = *( glyphsPerCharacterBuffer + characterLogicalOrderIndex );
    ++numberOfVisualCharacters;

    if( 0u != numberOfGlyphs )
    {
      // Get the first character/glyph of the group of glyphs.
      const CharacterIndex firstVisualCharacterIndex = 1u + visualIndex - numberOfVisualCharacters;
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

      // Whether the glyph can be split, like Latin ligatures fi, ff or Arabic (ل + ا).
      Length numberOfCharacters = *( charactersPerGlyphBuffer + firstLogicalGlyphIndex );
      if( direction != LTR )
      {
        // As characters are being traversed in visual order,
        // for right to left ligatures, the character which contains the
        // number of glyphs in the table is found first.
        // Jump the number of characters to the next glyph is needed.

        if( 0u == numberOfCharacters )
        {
          // TODO: This is a workaround to fix an issue with complex characters in the arabic
          // script like i.e. رّ or الأَبْجَدِيَّة العَرَبِيَّة
          // There are characters that are not shaped in one glyph but in combination with
          // the next one generates two of them.
          // The visual to logical conversion table have characters in different order than
          // expected even if all of them are arabic.

          // The workaround doesn't fix the issue completely but it prevents the application
          // to hang in an infinite loop.

          // Find the number of characters.
          for( GlyphIndex index = firstLogicalGlyphIndex + 1u;
               ( 0u == numberOfCharacters ) && ( index < totalNumberOfGlyphs ) ;
               ++index )
          {
            numberOfCharacters = *( charactersPerGlyphBuffer + index );
          }

          if( 2u > numberOfCharacters )
          {
            continue;
          }

          --numberOfCharacters;
        }

        visualIndex += numberOfCharacters - 1u;
      }

      // Get the script of the character.
      const Script script = logicalModel->GetScript( characterLogicalOrderIndex );

      const bool isInterglyphIndex = ( numberOfCharacters > numberOfGlyphs ) && HasLigatureMustBreak( script );
      const Length numberOfBlocks = isInterglyphIndex ? numberOfCharacters : 1u;
      const float glyphAdvance = glyphMetrics.advance / static_cast<float>( numberOfBlocks );

      CharacterIndex index = 0u;
      for( ; index < numberOfBlocks; ++index )
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
        // If the glyph is shaped from more than one character, it matches the character of the glyph.
        visualIndex = firstVisualCharacterIndex + index;
        break;
      }

      numberOfVisualCharacters = 0u;
    }
  }

  // The number of characters of the whole text.
  const Length totalNumberOfCharacters = logicalModel->mText.Count();

  // Return the logical position of the cursor in characters.

  if( !matched )
  {
    // If no character is matched, then the last character (in visual order) of the line is used.
    visualIndex = endCharacter;
  }

  // Get the paragraph direction.
  const CharacterDirection paragraphDirection = line.direction;

  if( totalNumberOfCharacters != visualIndex )
  {
    // The visual index is not at the end of the text.

    if( LTR == paragraphDirection )
    {
      // The paragraph direction is left to right.

      if( visualIndex == endCharacter )
      {
        // It places the cursor just before the last character in visual order.
        // i.e. it places the cursor just before the '\n' or before the last character
        // if there is a long line with no word breaks which is wrapped.

        // It doesn't check if the closest line is the last one like the RTL branch below
        // because the total number of characters is different than the visual index and
        // the visual index is the last character of the line.
        --visualIndex;
      }
    }
    else
    {
      // The paragraph direction is right to left.

      if( ( lineIndex != totalNumberOfLines - 1u ) && // is not the last line.
          ( visualIndex == startCharacter ) )
      {
        // It places the cursor just after the first character in visual order.
        // i.e. it places the cursor just after the '\n' or after the last character
        // if there is a long line with no word breaks which is wrapped.

        // If the last line doesn't end with '\n' it won't increase the visual index
        // placing the cursor at the beginning of the line (in visual order).
        ++visualIndex;
      }
    }
  }
  else
  {
    // The visual index is at the end of text.

    // If the text ends with a new paragraph character i.e. a '\n', an extra line with no characters is added at the end of the text.
    // This branch checks if the closest line is the one with the last '\n'. If it is, it decrements the visual index to place
    // the cursor just before the last '\n'.

    if( ( lineIndex != totalNumberOfLines - 1u ) &&
        TextAbstraction::IsNewParagraph( *( logicalModel->mText.Begin() + visualIndex - 1u ) ) )
    {
      --visualIndex;
    }
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
  // Whether the logical cursor position is at the end of the whole text.
  const bool isLastPosition = logicalModel->mText.Count() == logical;

  // Get the line where the character is laid-out.
  const CharacterIndex characterOfLine = isLastPosition ? ( logical - 1u ) : logical;

  // Whether the cursor is in the last position and the last position is a new paragraph character.
  const bool isLastNewParagraph = isLastPosition && TextAbstraction::IsNewParagraph( *( logicalModel->mText.Begin() + characterOfLine ) );

  const LineRun* const modelLines = visualModel->mLines.Begin();

  const LineIndex lineIndex = visualModel->GetLineOfCharacter( characterOfLine );
  const LineRun& line = *( modelLines + lineIndex );

  if( isLastNewParagraph )
  {
    // The cursor is in a new line with no characters. Place the cursor in that line.
    const LineIndex newLineIndex = lineIndex + 1u;
    const LineRun& newLine = *( modelLines + newLineIndex );

    cursorInfo.isSecondaryCursor = false;

    // Set the line offset and height.
    cursorInfo.lineOffset = CalculateLineOffset( visualModel->mLines,
                                                 newLineIndex );

    // The line height is the addition of the line ascender and the line descender.
    // However, the line descender has a negative value, hence the subtraction.
    cursorInfo.lineHeight = newLine.ascender - newLine.descender;

    // Set the primary cursor's height.
    cursorInfo.primaryCursorHeight = cursorInfo.lineHeight;

    // Set the primary cursor's position.
    cursorInfo.primaryPosition.x = 0.f;
    cursorInfo.primaryPosition.y = cursorInfo.lineOffset;

    // Transform the cursor info from line's coords to text's coords.
    cursorInfo.primaryPosition.x += ( LTR == line.direction ) ? 0.f : visualModel->mControlSize.width;
  }
  else
  {
    // Whether this line is a bidirectional line.
    const bool bidiLineFetched = logicalModel->FetchBidirectionalLineInfo( characterOfLine );

    // Check if the logical position is the first or the last one of the line.
    const bool isFirstPositionOfLine = line.characterRun.characterIndex == logical;
    const bool isLastPositionOfLine = line.characterRun.characterIndex + line.characterRun.numberOfCharacters == logical;

    // 'logical' is the logical 'cursor' index.
    // Get the next and current logical 'character' index.
    const CharacterIndex characterIndex = isFirstPositionOfLine ? logical : logical - 1u;
    const CharacterIndex nextCharacterIndex = isLastPositionOfLine ? characterIndex : logical;

    // The character's direction buffer.
    const CharacterDirection* const directionsBuffer = bidiLineFetched ? logicalModel->mCharacterDirections.Begin() : NULL;

    CharacterDirection isCurrentRightToLeft = false;
    CharacterDirection isNextRightToLeft = false;
    if( bidiLineFetched ) // If bidiLineFetched is false, it means the whole text is left to right.
    {
      isCurrentRightToLeft = *( directionsBuffer + characterIndex );
      isNextRightToLeft = *( directionsBuffer + nextCharacterIndex );
    }

    // Get the paragraph's direction.
    const CharacterDirection isRightToLeftParagraph = line.direction;

    // Check whether there is an alternative position:
    cursorInfo.isSecondaryCursor = ( ( !isLastPositionOfLine && ( isCurrentRightToLeft != isNextRightToLeft ) )     ||
                                     ( isLastPositionOfLine && ( isRightToLeftParagraph != isCurrentRightToLeft ) ) ||
                                     ( isFirstPositionOfLine && ( isRightToLeftParagraph != isCurrentRightToLeft ) ) );

    // Set the line offset and height.
    cursorInfo.lineOffset = CalculateLineOffset( visualModel->mLines,
                                                 lineIndex );

    // The line height is the addition of the line ascender and the line descender.
    // However, the line descender has a negative value, hence the subtraction.
    cursorInfo.lineHeight = line.ascender - line.descender;

    // Calculate the primary cursor.

    CharacterIndex index = characterIndex;
    if( cursorInfo.isSecondaryCursor )
    {
      // If there is a secondary position, the primary cursor may be in a different place than the logical index.

      if( isLastPositionOfLine )
      {
        // The position of the cursor after the last character needs special
        // care depending on its direction and the direction of the paragraph.

        // Need to find the first character after the last character with the paragraph's direction.
        // i.e l0 l1 l2 r0 r1 should find r0.

        index = isRightToLeftParagraph ? line.characterRun.characterIndex : line.characterRun.characterIndex + line.characterRun.numberOfCharacters - 1u;
        if( bidiLineFetched )
        {
          index = logicalModel->GetLogicalCharacterIndex( index );
        }
      }
      else if( isFirstPositionOfLine )
      {
        index = isRightToLeftParagraph ? line.characterRun.characterIndex + line.characterRun.numberOfCharacters - 1u : line.characterRun.characterIndex;
        if( bidiLineFetched )
        {
          index = logicalModel->GetLogicalCharacterIndex( index );
        }
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
    // 1001 1
    // 1010 0
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

    const bool addGlyphAdvance = ( ( isLastPositionOfLine && !isRightToLeftParagraph ) ||
                                   ( isFirstPositionOfLine && isRightToLeftParagraph ) ||
                                   ( !isFirstPositionOfLine && !isLastPosition && !isCurrentRightToLeft ) );

    float glyphAdvance = addGlyphAdvance ? glyphMetrics.advance : 0.f;

    if( !isLastPositionOfLine &&
        ( primaryNumberOfCharacters > 1u ) )
    {
      const CharacterIndex firstIndex = *( glyphsToCharactersBuffer + primaryGlyphIndex );

      bool isCurrentRightToLeft = false;
      if( bidiLineFetched ) // If bidiLineFetched is false, it means the whole text is left to right.
      {
        isCurrentRightToLeft = *( directionsBuffer + index );
      }

      Length numberOfGlyphAdvance = ( isFirstPositionOfLine ? 0u : 1u ) + characterIndex - firstIndex;
      if( isCurrentRightToLeft )
      {
        numberOfGlyphAdvance = primaryNumberOfCharacters - numberOfGlyphAdvance;
      }

      glyphAdvance = static_cast<float>( numberOfGlyphAdvance ) * glyphMetrics.advance / static_cast<float>( primaryNumberOfCharacters );
    }

    // Get the glyph position and x bearing (in the line's coords).
    const Vector2& primaryPosition = *( glyphPositionsBuffer + primaryGlyphIndex );

    // Set the primary cursor's height.
    cursorInfo.primaryCursorHeight = cursorInfo.isSecondaryCursor ? 0.5f * glyphMetrics.fontHeight : glyphMetrics.fontHeight;

    // Set the primary cursor's position.
    cursorInfo.primaryPosition.x = -glyphMetrics.xBearing + primaryPosition.x + glyphAdvance;
    cursorInfo.primaryPosition.y = cursorInfo.lineOffset + line.ascender - glyphMetrics.ascender;

    // Transform the cursor info from line's coords to text's coords.
    cursorInfo.primaryPosition.x += line.alignmentOffset;

    // Calculate the secondary cursor.
    if( cursorInfo.isSecondaryCursor )
    {
      // Set the secondary cursor's height.
      cursorInfo.secondaryCursorHeight = 0.5f * glyphMetrics.fontHeight;

      CharacterIndex index = characterIndex;
      if( !isLastPositionOfLine )
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

      // FCP A
      // ------
      // 000 1
      // 001 x
      // 010 0
      // 011 0
      // 100 x
      // 101 0
      // 110 1
      // 111 x
      //
      // Where F -> isFirstPosition
      //       C -> isCurrentRightToLeft
      //       P -> isRightToLeftParagraph
      //       A -> Whether to add the glyph's advance.

      const bool addGlyphAdvance = ( ( !isFirstPositionOfLine && !isCurrentRightToLeft ) ||
                                     ( isFirstPositionOfLine && !isRightToLeftParagraph ) );

      cursorInfo.secondaryPosition.x = -glyphMetrics.xBearing + secondaryPosition.x + ( addGlyphAdvance ? glyphMetrics.advance : 0.f );
      cursorInfo.secondaryPosition.y = cursorInfo.lineOffset + cursorInfo.lineHeight - cursorInfo.secondaryCursorHeight - line.descender - ( glyphMetrics.fontHeight - glyphMetrics.ascender );

      // Transform the cursor info from line's coords to text's coords.
      cursorInfo.secondaryPosition.x += line.alignmentOffset;
    }
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
