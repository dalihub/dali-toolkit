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
#include <dali-toolkit/internal/text/controller/text-controller-impl-data-clearer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/controller/text-controller-impl.h>
#include <dali-toolkit/internal/text/text-run-container.h>

namespace Dali::Toolkit::Text
{

void ControllerImplDataClearer::ClearFullModelData(Controller::Impl& impl, Controller::OperationsMask operations)
{
  ModelPtr& model = impl.mModel;

  if(Controller::NO_OPERATION != (Controller::GET_LINE_BREAKS & operations))
  {
    model->mLogicalModel->mLineBreakInfo.Clear();
    model->mLogicalModel->mParagraphInfo.Clear();
  }

  if(Controller::NO_OPERATION != (Controller::GET_SCRIPTS & operations))
  {
    model->mLogicalModel->mScriptRuns.Clear();
  }

  if(Controller::NO_OPERATION != (Controller::VALIDATE_FONTS & operations))
  {
    model->mLogicalModel->mFontRuns.Clear();
  }

  if(0u != model->mLogicalModel->mBidirectionalParagraphInfo.Count())
  {
    if(Controller::NO_OPERATION != (Controller::BIDI_INFO & operations))
    {
      model->mLogicalModel->mBidirectionalParagraphInfo.Clear();
      model->mLogicalModel->mCharacterDirections.Clear();
    }

    if(Controller::NO_OPERATION != (Controller::REORDER & operations))
    {
      // Free the allocated memory used to store the conversion table in the bidirectional line info run.
      for(Vector<BidirectionalLineInfoRun>::Iterator it    = model->mLogicalModel->mBidirectionalLineInfo.Begin(),
                                                     endIt = model->mLogicalModel->mBidirectionalLineInfo.End();
          it != endIt;
          ++it)
      {
        BidirectionalLineInfoRun& bidiLineInfo = *it;

        free(bidiLineInfo.visualToLogicalMap);
        bidiLineInfo.visualToLogicalMap = NULL;

        free(bidiLineInfo.visualToLogicalMapSecondHalf);
        bidiLineInfo.visualToLogicalMapSecondHalf = NULL;
      }
      model->mLogicalModel->mBidirectionalLineInfo.Clear();
    }
  }

  if(Controller::NO_OPERATION != (Controller::SHAPE_TEXT & operations))
  {
    model->mVisualModel->mGlyphs.Clear();
    model->mVisualModel->mGlyphsToCharacters.Clear();
    model->mVisualModel->mCharactersToGlyph.Clear();
    model->mVisualModel->mCharactersPerGlyph.Clear();
    model->mVisualModel->mGlyphsPerCharacter.Clear();
    model->mVisualModel->mGlyphPositions.Clear();
  }

  if(Controller::NO_OPERATION != (Controller::LAYOUT & operations))
  {
    model->mVisualModel->mLines.Clear();
  }

  if(Controller::NO_OPERATION != (Controller::COLOR & operations))
  {
    model->mVisualModel->mColorIndices.Clear();
    model->mVisualModel->mBackgroundColorIndices.Clear();
  }
}

void ControllerImplDataClearer::ClearCharacterModelData(Controller::Impl& impl, CharacterIndex startIndex, CharacterIndex endIndex, Controller::OperationsMask operations)
{
  const CharacterIndex endIndexPlusOne = endIndex + 1u;
  ModelPtr& model = impl.mModel;

  if(Controller::NO_OPERATION != (Controller::GET_LINE_BREAKS & operations))
  {
    // Clear the line break info.
    LineBreakInfo* lineBreakInfoBuffer = model->mLogicalModel->mLineBreakInfo.Begin();

    model->mLogicalModel->mLineBreakInfo.Erase(lineBreakInfoBuffer + startIndex,
                                                     lineBreakInfoBuffer + endIndexPlusOne);

    // Clear the paragraphs.
    ClearCharacterRuns(startIndex,
                       endIndex,
                       model->mLogicalModel->mParagraphInfo);
  }

  if(Controller::NO_OPERATION != (Controller::GET_SCRIPTS & operations))
  {
    // Clear the scripts.
    ClearCharacterRuns(startIndex,
                       endIndex,
                       model->mLogicalModel->mScriptRuns);
  }

  if(Controller::NO_OPERATION != (Controller::VALIDATE_FONTS & operations))
  {
    // Clear the fonts.
    ClearCharacterRuns(startIndex,
                       endIndex,
                       model->mLogicalModel->mFontRuns);
  }

  if(0u != model->mLogicalModel->mBidirectionalParagraphInfo.Count())
  {
    if(Controller::NO_OPERATION != (Controller::BIDI_INFO & operations))
    {
      // Clear the bidirectional paragraph info.
      ClearCharacterRuns(startIndex,
                         endIndex,
                         model->mLogicalModel->mBidirectionalParagraphInfo);

      // Clear the character's directions.
      CharacterDirection* characterDirectionsBuffer = model->mLogicalModel->mCharacterDirections.Begin();

      model->mLogicalModel->mCharacterDirections.Erase(characterDirectionsBuffer + startIndex,
                                                       characterDirectionsBuffer + endIndexPlusOne);
    }

    if(Controller::NO_OPERATION != (Controller::REORDER & operations))
    {
      uint32_t startRemoveIndex = model->mLogicalModel->mBidirectionalLineInfo.Count();
      uint32_t endRemoveIndex   = startRemoveIndex;
      ClearCharacterRuns(startIndex,
                         endIndex,
                         model->mLogicalModel->mBidirectionalLineInfo,
                         startRemoveIndex,
                         endRemoveIndex);

      BidirectionalLineInfoRun* bidirectionalLineInfoBuffer = model->mLogicalModel->mBidirectionalLineInfo.Begin();

      // Free the allocated memory used to store the conversion table in the bidirectional line info run.
      for(Vector<BidirectionalLineInfoRun>::Iterator it    = bidirectionalLineInfoBuffer + startRemoveIndex,
                                                     endIt = bidirectionalLineInfoBuffer + endRemoveIndex;
          it != endIt;
          ++it)
      {
        BidirectionalLineInfoRun& bidiLineInfo = *it;

        free(bidiLineInfo.visualToLogicalMap);
        bidiLineInfo.visualToLogicalMap = NULL;

        free(bidiLineInfo.visualToLogicalMapSecondHalf);
        bidiLineInfo.visualToLogicalMapSecondHalf = NULL;
      }

      model->mLogicalModel->mBidirectionalLineInfo.Erase(bidirectionalLineInfoBuffer + startRemoveIndex,
                                                         bidirectionalLineInfoBuffer + endRemoveIndex);
    }
  }
}

void ControllerImplDataClearer::ClearGlyphModelData(Controller::Impl& impl, CharacterIndex startIndex, CharacterIndex endIndex, Controller::OperationsMask operations)
{
  const CharacterIndex endIndexPlusOne           = endIndex + 1u;
  const Length         numberOfCharactersRemoved = endIndexPlusOne - startIndex;
  ModelPtr&            model                     = impl.mModel;
  TextUpdateInfo&      textUpdateInfo            = impl.mTextUpdateInfo;


  // Convert the character index to glyph index before deleting the character to glyph and the glyphs per character buffers.
  GlyphIndex* charactersToGlyphBuffer  = model->mVisualModel->mCharactersToGlyph.Begin();
  Length*     glyphsPerCharacterBuffer = model->mVisualModel->mGlyphsPerCharacter.Begin();

  const GlyphIndex endGlyphIndexPlusOne  = *(charactersToGlyphBuffer + endIndex) + *(glyphsPerCharacterBuffer + endIndex);
  const Length     numberOfGlyphsRemoved = endGlyphIndexPlusOne - textUpdateInfo.mStartGlyphIndex;

  if(Controller::NO_OPERATION != (Controller::SHAPE_TEXT & operations))
  {
    // Update the character to glyph indices.
    for(Vector<GlyphIndex>::Iterator it    = charactersToGlyphBuffer + endIndexPlusOne,
                                     endIt = charactersToGlyphBuffer + model->mVisualModel->mCharactersToGlyph.Count();
        it != endIt;
        ++it)
    {
      CharacterIndex& index = *it;
      index -= numberOfGlyphsRemoved;
    }

    // Clear the character to glyph conversion table.
    model->mVisualModel->mCharactersToGlyph.Erase(charactersToGlyphBuffer + startIndex,
                                                  charactersToGlyphBuffer + endIndexPlusOne);

    // Clear the glyphs per character table.
    model->mVisualModel->mGlyphsPerCharacter.Erase(glyphsPerCharacterBuffer + startIndex,
                                                   glyphsPerCharacterBuffer + endIndexPlusOne);

    // Clear the glyphs buffer.
    GlyphInfo* glyphsBuffer = model->mVisualModel->mGlyphs.Begin();
    model->mVisualModel->mGlyphs.Erase(glyphsBuffer + textUpdateInfo.mStartGlyphIndex,
                                       glyphsBuffer + endGlyphIndexPlusOne);

    CharacterIndex* glyphsToCharactersBuffer = model->mVisualModel->mGlyphsToCharacters.Begin();

    // Update the glyph to character indices.
    for(Vector<CharacterIndex>::Iterator it    = glyphsToCharactersBuffer + endGlyphIndexPlusOne,
                                         endIt = glyphsToCharactersBuffer + model->mVisualModel->mGlyphsToCharacters.Count();
        it != endIt;
        ++it)
    {
      CharacterIndex& index = *it;
      index -= numberOfCharactersRemoved;
    }

    // Clear the glyphs to characters buffer.
    model->mVisualModel->mGlyphsToCharacters.Erase(glyphsToCharactersBuffer + textUpdateInfo.mStartGlyphIndex,
                                                   glyphsToCharactersBuffer + endGlyphIndexPlusOne);

    // Clear the characters per glyph buffer.
    Length* charactersPerGlyphBuffer = model->mVisualModel->mCharactersPerGlyph.Begin();
    model->mVisualModel->mCharactersPerGlyph.Erase(charactersPerGlyphBuffer + textUpdateInfo.mStartGlyphIndex,
                                                   charactersPerGlyphBuffer + endGlyphIndexPlusOne);

    // Should pass if mGlyphPositions has already been cleared in Controller::Relayouter::Relayout
    if(0u != model->mVisualModel->mGlyphPositions.Count())
    {
      // Clear the positions buffer.
      Vector2* positionsBuffer = model->mVisualModel->mGlyphPositions.Begin();
      model->mVisualModel->mGlyphPositions.Erase(positionsBuffer + textUpdateInfo.mStartGlyphIndex,
                                                 positionsBuffer + endGlyphIndexPlusOne);
    }
  }

  if(Controller::NO_OPERATION != (Controller::LAYOUT & operations))
  {
    // Clear the lines.
    uint32_t startRemoveIndex = model->mVisualModel->mLines.Count();
    uint32_t endRemoveIndex   = startRemoveIndex;
    ClearCharacterRuns(startIndex,
                       endIndex,
                       model->mVisualModel->mLines,
                       startRemoveIndex,
                       endRemoveIndex);

    // Will update the glyph runs.
    startRemoveIndex = model->mVisualModel->mLines.Count();
    endRemoveIndex   = startRemoveIndex;
    ClearGlyphRuns(textUpdateInfo.mStartGlyphIndex,
                   endGlyphIndexPlusOne - 1u,
                   model->mVisualModel->mLines,
                   startRemoveIndex,
                   endRemoveIndex);

    // Set the line index from where to insert the new laid-out lines.
    textUpdateInfo.mStartLineIndex = startRemoveIndex;

    LineRun* linesBuffer = model->mVisualModel->mLines.Begin();
    model->mVisualModel->mLines.Erase(linesBuffer + startRemoveIndex,
                                      linesBuffer + endRemoveIndex);
  }

  if(Controller::NO_OPERATION != (Controller::COLOR & operations))
  {
    if(0u != model->mVisualModel->mColorIndices.Count())
    {
      ColorIndex* colorIndexBuffer = model->mVisualModel->mColorIndices.Begin();
      model->mVisualModel->mColorIndices.Erase(colorIndexBuffer + textUpdateInfo.mStartGlyphIndex,
                                                colorIndexBuffer + endGlyphIndexPlusOne);
    }

    if(0u != model->mVisualModel->mBackgroundColorIndices.Count())
    {
      ColorIndex* backgroundColorIndexBuffer = model->mVisualModel->mBackgroundColorIndices.Begin();
      model->mVisualModel->mBackgroundColorIndices.Erase(backgroundColorIndexBuffer + textUpdateInfo.mStartGlyphIndex,
                                                         backgroundColorIndexBuffer + endGlyphIndexPlusOne);
    }
  }
}

void ControllerImplDataClearer::ClearModelData(Controller::Impl& impl, CharacterIndex startIndex, CharacterIndex endIndex, Controller::OperationsMask operations)
{
  TextUpdateInfo& textUpdateInfo = impl.mTextUpdateInfo;

  if(textUpdateInfo.mClearAll ||
     ((0u == startIndex) &&
      (textUpdateInfo.mPreviousNumberOfCharacters == endIndex + 1u)))
  {
    ClearFullModelData(impl, operations);
  }
  else
  {
    // Clear the model data related with characters.
    ClearCharacterModelData(impl, startIndex, endIndex, operations);

    // Clear the model data related with glyphs.
    ClearGlyphModelData(impl, startIndex, endIndex, operations);
  }

  ModelPtr& model = impl.mModel;

  // The estimated number of lines. Used to avoid reallocations when layouting.
  textUpdateInfo.mEstimatedNumberOfLines = std::max(model->mVisualModel->mLines.Count(), model->mLogicalModel->mParagraphInfo.Count());

  model->mVisualModel->ClearCaches();
}

} // namespace Dali::Toolkit::Text
