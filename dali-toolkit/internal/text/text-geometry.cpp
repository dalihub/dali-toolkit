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
#include <dali-toolkit/internal/text/text-geometry.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/cursor-helper-functions.h>

using namespace Dali;

namespace Dali
{
namespace Toolkit
{
namespace Text
{
bool GetNextLine(GlyphIndex index, LineIndex& lineIndex, LineRun*& lineRun, GlyphIndex& lastGlyphOfLine, Length numberOfLines, bool& isLastLine)
{
  if(index == lastGlyphOfLine)
  {
    ++lineIndex;
    if(lineIndex < numberOfLines)
    {
      isLastLine = (lineIndex + 1 == numberOfLines);
      ++lineRun;
      return true;
    }
  }

  return false;
}

void UpdateLineInfo(const LineRun* lineRun, float& currentLineOffset, float& currentLineHeight, GlyphIndex& lastGlyphOfLine, bool isLastLine)
{
  lastGlyphOfLine   = lineRun->glyphRun.glyphIndex + lineRun->glyphRun.numberOfGlyphs - 1u;
  currentLineOffset = currentLineOffset + currentLineHeight;
  currentLineHeight = GetLineHeight(*lineRun, isLastLine);
}

void GetTextGeometry(ModelPtr textModel, CharacterIndex startIndex, CharacterIndex endIndex, Vector<Vector2>& sizesList, Vector<Vector2>& positionsList)
{
  VisualModelPtr&  visualModel  = textModel->mVisualModel;
  LogicalModelPtr& logicalModel = textModel->mLogicalModel;

  const GlyphIndex* const         charactersToGlyphBuffer        = visualModel->mCharactersToGlyph.Begin();
  const Length* const             glyphsPerCharacterBuffer       = visualModel->mGlyphsPerCharacter.Begin();
  const GlyphInfo* const          glyphsBuffer                   = visualModel->mGlyphs.Begin();
  const Vector2* const            positionsBuffer                = visualModel->mGlyphPositions.Begin();
  const Length* const             charactersPerGlyphBuffer       = visualModel->mCharactersPerGlyph.Begin();
  const CharacterIndex* const     glyphToCharacterBuffer         = visualModel->mGlyphsToCharacters.Begin();
  const CharacterDirection* const modelCharacterDirectionsBuffer = (0u != logicalModel->mCharacterDirections.Count()) ? logicalModel->mCharacterDirections.Begin() : NULL;

  if(startIndex >= logicalModel->mText.Count() && endIndex >= logicalModel->mText.Count())
    return;

  if(startIndex >= logicalModel->mText.Count())
    startIndex = logicalModel->mText.Count() - 1;

  if(endIndex >= logicalModel->mText.Count())
    endIndex = logicalModel->mText.Count() - 1;

  if(startIndex > endIndex)
  {
    std::swap(startIndex, endIndex);
  }

  LineRun*   lineRun    = visualModel->mLines.Begin();
  GlyphIndex glyphStart = *(charactersToGlyphBuffer + startIndex);

  //if glyph not in the first line (in some ellipsis cases)
  if(glyphStart < lineRun->glyphRun.glyphIndex)
  {
    glyphStart = lineRun->glyphRun.glyphIndex;
    startIndex = *(glyphToCharacterBuffer + glyphStart);

    if(startIndex > endIndex)
    {
      std::swap(startIndex, endIndex);
    }
  }

  const Length numberOfGlyphs = *(glyphsPerCharacterBuffer + endIndex);
  GlyphIndex   glyphEnd       = *(charactersToGlyphBuffer + endIndex) + ((numberOfGlyphs > 0) ? numberOfGlyphs - 1u : 0u);
  LineIndex    lineIndex      = visualModel->GetLineOfCharacter(startIndex);
  Length       numberOfLines  = visualModel->GetTotalNumberOfLines();
  bool         isLastLine     = lineIndex + 1 == numberOfLines;

  LineIndex firstLineIndex = lineIndex;
  Size      textInLineSize;
  Vector2   textInLinePosition;

  lineRun += firstLineIndex;

  //get the first line and its vertical offset
  float      currentLineOffset = CalculateLineOffset(visualModel->mLines, firstLineIndex);
  float      currentLineHeight = GetLineHeight(*lineRun, isLastLine);
  GlyphIndex lastGlyphOfLine   = lineRun->glyphRun.glyphIndex + lineRun->glyphRun.numberOfGlyphs - 1;

  // Check if the first/last glyph is a ligature that needs be splitted like English fi or Arabic ﻻ.
  const Length numberOfCharactersStart = *(charactersPerGlyphBuffer + glyphStart);
  const Length numberOfCharactersEnd   = *(charactersPerGlyphBuffer + glyphEnd);

  bool splitStartGlyph = (numberOfCharactersStart > 1u) && HasLigatureMustBreak(logicalModel->GetScript(startIndex));
  bool splitEndGlyph   = (glyphStart != glyphEnd) && (numberOfCharactersEnd > 1u) && HasLigatureMustBreak(logicalModel->GetScript(endIndex));

  Vector2            currentSize;
  Vector2            currentPosition;
  Vector2            blockSize;
  Vector2            blockPos;
  CharacterDirection isCurrentRightToLeft;

  CharacterDirection                      isPrevoiusRightToLeft           = (nullptr != modelCharacterDirectionsBuffer ? *(modelCharacterDirectionsBuffer + startIndex) : false);
  const bool                              isEllipsisEnabled               = textModel->mElideEnabled;
  const GlyphIndex                        startIndexOfGlyphs              = textModel->GetStartIndexOfElidedGlyphs();
  const GlyphIndex                        endIndexOfGlyphs                = textModel->GetEndIndexOfElidedGlyphs();
  const GlyphIndex                        firstMiddleIndexOfElidedGlyphs  = textModel->GetFirstMiddleIndexOfElidedGlyphs();
  const GlyphIndex                        secondMiddleIndexOfElidedGlyphs = textModel->GetSecondMiddleIndexOfElidedGlyphs();
  const DevelText::EllipsisPosition::Type ellipsisPosition                = textModel->GetEllipsisPosition();

  for(GlyphIndex index = glyphStart; index <= glyphEnd; ++index)
  {
    if(isEllipsisEnabled)
    {
      if(ellipsisPosition == DevelText::EllipsisPosition::MIDDLE)
      {
        if(index >= firstMiddleIndexOfElidedGlyphs &&
           index < secondMiddleIndexOfElidedGlyphs)
        {
          if((index - 1 == firstMiddleIndexOfElidedGlyphs) && (firstMiddleIndexOfElidedGlyphs != 0))
          {
            sizesList.PushBack(blockSize);
            positionsList.PushBack(blockPos);
          }

          if(GetNextLine(index, lineIndex, lineRun, lastGlyphOfLine, numberOfLines, isLastLine))
          {
            UpdateLineInfo(lineRun, currentLineOffset, currentLineHeight, lastGlyphOfLine, isLastLine);
          }
          // Ignore any glyph that was removed
          continue;
        }
      }
      else
      {
        if((ellipsisPosition == DevelText::EllipsisPosition::END) && (index > endIndexOfGlyphs))
        {
          //skip remaining elided glyphs
          break;
        }
        else if((ellipsisPosition == DevelText::EllipsisPosition::START) && (index <= startIndexOfGlyphs))
        {
          if(GetNextLine(index, lineIndex, lineRun, lastGlyphOfLine, numberOfLines, isLastLine))
          {
            UpdateLineInfo(lineRun, currentLineOffset, currentLineHeight, lastGlyphOfLine, isLastLine);
          }

          continue;
        }
      }
    }

    const GlyphInfo& glyph    = *(glyphsBuffer + index);
    const Vector2&   position = *(positionsBuffer + index);

    // If NULL, means all of the characters is left to right.
    isCurrentRightToLeft = (nullptr != modelCharacterDirectionsBuffer ? *(modelCharacterDirectionsBuffer + *(glyphToCharacterBuffer + index)) : false);

    if(splitStartGlyph && (index == glyphStart))
    {
      // If the first glyph is a ligature that needs to be splitted, we may need only to add part of the glyph.
      const float          glyphAdvance       = glyph.advance / static_cast<float>(numberOfCharactersStart);
      const CharacterIndex interGlyphIndex    = startIndex - *(glyphToCharacterBuffer + glyphStart);
      const Length         numberOfCharacters = (glyphStart == glyphEnd) ? (endIndex - startIndex) + 1 : (numberOfCharactersStart - interGlyphIndex);

      currentPosition.x = lineRun->alignmentOffset + position.x - glyph.xBearing + textModel->mScrollPosition.x + glyphAdvance * static_cast<float>(isCurrentRightToLeft ? (numberOfCharactersStart - interGlyphIndex - numberOfCharacters) : interGlyphIndex);
      currentPosition.y = currentLineOffset + textModel->mScrollPosition.y;
      currentSize.x     = static_cast<float>(numberOfCharacters) * glyphAdvance;
      currentSize.y     = currentLineHeight;
      splitStartGlyph   = false;
    }
    else if(splitEndGlyph && (index == glyphEnd))
    {
      const float          glyphAdvance       = glyph.advance / static_cast<float>(numberOfCharactersEnd);
      const CharacterIndex interGlyphIndex    = endIndex - *(glyphToCharacterBuffer + glyphEnd);
      const Length         numberOfCharacters = numberOfCharactersEnd - interGlyphIndex - 1;

      currentPosition.x = lineRun->alignmentOffset + position.x - glyph.xBearing + textModel->mScrollPosition.x + (isCurrentRightToLeft ? (glyphAdvance * static_cast<float>(numberOfCharacters)) : 0.f);
      currentPosition.y = currentLineOffset + textModel->mScrollPosition.y;
      currentSize.x     = static_cast<float>(interGlyphIndex + 1) * glyphAdvance;
      currentSize.y     = currentLineHeight;
      splitEndGlyph     = false;
    }
    else
    {
      currentPosition.x = lineRun->alignmentOffset + position.x - glyph.xBearing + textModel->mScrollPosition.x;
      currentPosition.y = currentLineOffset + textModel->mScrollPosition.y;
      currentSize.x     = glyph.advance;
      currentSize.y     = currentLineHeight;

      // if there is next line to retrieve.
      if(GetNextLine(index, lineIndex, lineRun, lastGlyphOfLine, numberOfLines, isLastLine))
      {
        UpdateLineInfo(lineRun, currentLineOffset, currentLineHeight, lastGlyphOfLine, isLastLine);
      }
    }

    if((index == glyphStart) || (isEllipsisEnabled && (((ellipsisPosition == DevelText::EllipsisPosition::MIDDLE) && (index == secondMiddleIndexOfElidedGlyphs)) || ((ellipsisPosition == DevelText::EllipsisPosition::START) && (index - 1 == startIndexOfGlyphs)))))
    {
      blockPos  = currentPosition;
      blockSize = currentSize;
    }
    else if((isPrevoiusRightToLeft != isCurrentRightToLeft) || (blockPos.y != currentPosition.y)) //new direction or new line
    {
      sizesList.PushBack(blockSize);
      positionsList.PushBack(blockPos);

      blockPos  = currentPosition;
      blockSize = currentSize;
    }
    else
    {
      if(isCurrentRightToLeft)
      {
        blockPos.x -= currentSize.x;
      }

      blockSize.x += currentSize.x;
    }

    isPrevoiusRightToLeft = isCurrentRightToLeft;
  }

  //add last block
  sizesList.PushBack(blockSize);
  positionsList.PushBack(blockPos);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
