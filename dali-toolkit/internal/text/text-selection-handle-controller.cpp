/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/text-selection-handle-controller.h>

#include <dali/integration-api/debug.h>
#include <limits>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/cursor-helper-functions.h>
#include <dali-toolkit/internal/text/text-controller-impl-event-handler.h>

using namespace Dali;

namespace
{
/**
 * @brief Struct used to calculate the selection box.
 */
struct SelectionBoxInfo
{
  float lineOffset;
  float lineHeight;
  float minX;
  float maxX;
};

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

const float                                   MAX_FLOAT = std::numeric_limits<float>::max();
const float                                   MIN_FLOAT = std::numeric_limits<float>::min();
const Dali::Toolkit::Text::CharacterDirection LTR       = false; ///< Left To Right direction

} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Text
{
void SelectionHandleController::Reposition(Controller::Impl& impl)
{
  EventData*& eventData = impl.mEventData;

  CharacterIndex selectionStart = eventData->mLeftSelectionPosition;
  CharacterIndex selectionEnd   = eventData->mRightSelectionPosition;

  DecoratorPtr& decorator = eventData->mDecorator;

  if(selectionStart == selectionEnd)
  {
    // Nothing to select if handles are in the same place.
    // So, deactive Highlight box.
    decorator->SetHighlightActive(false);
    return;
  }

  decorator->ClearHighlights();

  ModelPtr&        model        = impl.mModel;
  VisualModelPtr&  visualModel  = model->mVisualModel;
  LogicalModelPtr& logicalModel = model->mLogicalModel;

  const GlyphIndex* const         charactersToGlyphBuffer        = visualModel->mCharactersToGlyph.Begin();
  const Length* const             glyphsPerCharacterBuffer       = visualModel->mGlyphsPerCharacter.Begin();
  const GlyphInfo* const          glyphsBuffer                   = visualModel->mGlyphs.Begin();
  const Vector2* const            positionsBuffer                = visualModel->mGlyphPositions.Begin();
  const Length* const             charactersPerGlyphBuffer       = visualModel->mCharactersPerGlyph.Begin();
  const CharacterIndex* const     glyphToCharacterBuffer         = visualModel->mGlyphsToCharacters.Begin();
  const CharacterDirection* const modelCharacterDirectionsBuffer = (0u != logicalModel->mCharacterDirections.Count()) ? logicalModel->mCharacterDirections.Begin() : NULL;

  const bool               isLastCharacter = selectionEnd >= logicalModel->mText.Count();
  const CharacterDirection startDirection  = ((NULL == modelCharacterDirectionsBuffer) ? false : *(modelCharacterDirectionsBuffer + selectionStart));
  const CharacterDirection endDirection    = ((NULL == modelCharacterDirectionsBuffer) ? false : *(modelCharacterDirectionsBuffer + (selectionEnd - (isLastCharacter ? 1u : 0u))));

  // Swap the indices if the start is greater than the end.
  const bool indicesSwapped = selectionStart > selectionEnd;

  // Tell the decorator to flip the selection handles if needed.
  decorator->SetSelectionHandleFlipState(indicesSwapped, startDirection, endDirection);

  if(indicesSwapped)
  {
    std::swap(selectionStart, selectionEnd);
  }

  // Get the indices to the first and last selected glyphs.
  const CharacterIndex selectionEndMinusOne = selectionEnd - 1u;
  const GlyphIndex     glyphStart           = *(charactersToGlyphBuffer + selectionStart);
  const Length         numberOfGlyphs       = *(glyphsPerCharacterBuffer + selectionEndMinusOne);
  const GlyphIndex     glyphEnd             = *(charactersToGlyphBuffer + selectionEndMinusOne) + ((numberOfGlyphs > 0) ? numberOfGlyphs - 1u : 0u);

  // Get the lines where the glyphs are laid-out.
  const LineRun* lineRun = visualModel->mLines.Begin();

  LineIndex lineIndex     = 0u;
  Length    numberOfLines = 0u;
  visualModel->GetNumberOfLines(glyphStart,
                                1u + glyphEnd - glyphStart,
                                lineIndex,
                                numberOfLines);
  const LineIndex firstLineIndex = lineIndex;

  // Create the structure to store some selection box info.
  Vector<SelectionBoxInfo> selectionBoxLinesInfo;
  selectionBoxLinesInfo.Resize(numberOfLines);

  SelectionBoxInfo* selectionBoxInfo = selectionBoxLinesInfo.Begin();
  selectionBoxInfo->minX             = MAX_FLOAT;
  selectionBoxInfo->maxX             = MIN_FLOAT;

  // Keep the min and max 'x' position to calculate the size and position of the highlighed text.
  float   minHighlightX = std::numeric_limits<float>::max();
  float   maxHighlightX = std::numeric_limits<float>::min();
  Size    highLightSize;
  Vector2 highLightPosition; // The highlight position in decorator's coords.

  // Retrieve the first line and get the line's vertical offset, the line's height and the index to the last glyph.

  // The line's vertical offset of all the lines before the line where the first glyph is laid-out.
  selectionBoxInfo->lineOffset = CalculateLineOffset(visualModel->mLines,
                                                     firstLineIndex);

  // Transform to decorator's (control) coords.
  selectionBoxInfo->lineOffset += model->mScrollPosition.y;

  lineRun += firstLineIndex;

  // The line height is the addition of the line ascender and the line descender.
  // However, the line descender has a negative value, hence the subtraction.
  selectionBoxInfo->lineHeight = lineRun->ascender - lineRun->descender;

  GlyphIndex lastGlyphOfLine = lineRun->glyphRun.glyphIndex + lineRun->glyphRun.numberOfGlyphs - 1u;

  // Check if the first glyph is a ligature that must be broken like Latin ff, fi, or Arabic ﻻ, etc which needs special code.
  const Length numberOfCharactersStart = *(charactersPerGlyphBuffer + glyphStart);
  bool         splitStartGlyph         = (numberOfCharactersStart > 1u) && HasLigatureMustBreak(logicalModel->GetScript(selectionStart));

  // Check if the last glyph is a ligature that must be broken like Latin ff, fi, or Arabic ﻻ, etc which needs special code.
  const Length numberOfCharactersEnd = *(charactersPerGlyphBuffer + glyphEnd);
  bool         splitEndGlyph         = (glyphStart != glyphEnd) && (numberOfCharactersEnd > 1u) && HasLigatureMustBreak(logicalModel->GetScript(selectionEndMinusOne));

  // The number of quads of the selection box.
  const unsigned int numberOfQuads = 1u + (glyphEnd - glyphStart) + ((numberOfLines > 1u) ? 2u * numberOfLines : 0u);
  decorator->ResizeHighlightQuads(numberOfQuads);

  // Count the actual number of quads.
  unsigned int actualNumberOfQuads = 0u;
  Vector4      quad;

  // Traverse the glyphs.
  for(GlyphIndex index = glyphStart; index <= glyphEnd; ++index)
  {
    const GlyphInfo& glyph    = *(glyphsBuffer + index);
    const Vector2&   position = *(positionsBuffer + index);

    if(splitStartGlyph)
    {
      // If the first glyph is a ligature that must be broken it may be needed to add only part of the glyph to the highlight box.

      const float          glyphAdvance    = glyph.advance / static_cast<float>(numberOfCharactersStart);
      const CharacterIndex interGlyphIndex = selectionStart - *(glyphToCharacterBuffer + glyphStart);
      // Get the direction of the character.
      CharacterDirection isCurrentRightToLeft = false;
      if(nullptr != modelCharacterDirectionsBuffer) // If modelCharacterDirectionsBuffer is NULL, it means the whole text is left to right.
      {
        isCurrentRightToLeft = *(modelCharacterDirectionsBuffer + selectionStart);
      }

      // The end point could be in the middle of the ligature.
      // Calculate the number of characters selected.
      const Length numberOfCharacters = (glyphStart == glyphEnd) ? (selectionEnd - selectionStart) : (numberOfCharactersStart - interGlyphIndex);

      quad.x = lineRun->alignmentOffset + position.x - glyph.xBearing + model->mScrollPosition.x + glyphAdvance * static_cast<float>(isCurrentRightToLeft ? (numberOfCharactersStart - interGlyphIndex - numberOfCharacters) : interGlyphIndex);
      quad.y = selectionBoxInfo->lineOffset;
      quad.z = quad.x + static_cast<float>(numberOfCharacters) * glyphAdvance;
      quad.w = selectionBoxInfo->lineOffset + selectionBoxInfo->lineHeight;

      // Store the min and max 'x' for each line.
      selectionBoxInfo->minX = std::min(selectionBoxInfo->minX, quad.x);
      selectionBoxInfo->maxX = std::max(selectionBoxInfo->maxX, quad.z);

      decorator->AddHighlight(actualNumberOfQuads, quad);
      ++actualNumberOfQuads;

      splitStartGlyph = false;
      continue;
    }

    if(splitEndGlyph && (index == glyphEnd))
    {
      // Equally, if the last glyph is a ligature that must be broken it may be needed to add only part of the glyph to the highlight box.

      const float          glyphAdvance    = glyph.advance / static_cast<float>(numberOfCharactersEnd);
      const CharacterIndex interGlyphIndex = selectionEnd - *(glyphToCharacterBuffer + glyphEnd);
      // Get the direction of the character.
      CharacterDirection isCurrentRightToLeft = false;
      if(nullptr != modelCharacterDirectionsBuffer) // If modelCharacterDirectionsBuffer is NULL, it means the whole text is left to right.
      {
        isCurrentRightToLeft = *(modelCharacterDirectionsBuffer + selectionEnd);
      }

      const Length numberOfCharacters = numberOfCharactersEnd - interGlyphIndex;

      quad.x = lineRun->alignmentOffset + position.x - glyph.xBearing + model->mScrollPosition.x + (isCurrentRightToLeft ? (glyphAdvance * static_cast<float>(numberOfCharacters)) : 0.f);
      quad.y = selectionBoxInfo->lineOffset;
      quad.z = quad.x + static_cast<float>(interGlyphIndex) * glyphAdvance;
      quad.w = quad.y + selectionBoxInfo->lineHeight;

      // Store the min and max 'x' for each line.
      selectionBoxInfo->minX = std::min(selectionBoxInfo->minX, quad.x);
      selectionBoxInfo->maxX = std::max(selectionBoxInfo->maxX, quad.z);

      decorator->AddHighlight(actualNumberOfQuads,
                              quad);
      ++actualNumberOfQuads;

      splitEndGlyph = false;
      continue;
    }

    quad.x = lineRun->alignmentOffset + position.x - glyph.xBearing + model->mScrollPosition.x;
    quad.y = selectionBoxInfo->lineOffset;
    quad.z = quad.x + glyph.advance;
    quad.w = quad.y + selectionBoxInfo->lineHeight;

    // Store the min and max 'x' for each line.
    selectionBoxInfo->minX = std::min(selectionBoxInfo->minX, quad.x);
    selectionBoxInfo->maxX = std::max(selectionBoxInfo->maxX, quad.z);

    decorator->AddHighlight(actualNumberOfQuads,
                            quad);
    ++actualNumberOfQuads;

    // Whether to retrieve the next line.
    if(index == lastGlyphOfLine)
    {
      ++lineIndex;
      if(lineIndex < firstLineIndex + numberOfLines)
      {
        // Retrieve the next line.
        ++lineRun;

        // Get the last glyph of the new line.
        lastGlyphOfLine = lineRun->glyphRun.glyphIndex + lineRun->glyphRun.numberOfGlyphs - 1u;

        // Keep the offset and height of the current selection box.
        const float currentLineOffset = selectionBoxInfo->lineOffset;
        const float currentLineHeight = selectionBoxInfo->lineHeight;

        // Get the selection box info for the next line.
        ++selectionBoxInfo;

        selectionBoxInfo->minX = MAX_FLOAT;
        selectionBoxInfo->maxX = MIN_FLOAT;

        // Update the line's vertical offset.
        selectionBoxInfo->lineOffset = currentLineOffset + currentLineHeight;

        // The line height is the addition of the line ascender and the line descender.
        // However, the line descender has a negative value, hence the subtraction.
        selectionBoxInfo->lineHeight = lineRun->ascender - lineRun->descender;
      }
    }
  }

  // Traverses all the lines and updates the min and max 'x' positions and the total height.
  // The final width is calculated after 'boxifying' the selection.
  for(Vector<SelectionBoxInfo>::ConstIterator it    = selectionBoxLinesInfo.Begin(),
                                              endIt = selectionBoxLinesInfo.End();
      it != endIt;
      ++it)
  {
    const SelectionBoxInfo& info = *it;

    // Update the size of the highlighted text.
    highLightSize.height += info.lineHeight;
    minHighlightX = std::min(minHighlightX, info.minX);
    maxHighlightX = std::max(maxHighlightX, info.maxX);
  }

  // Add extra geometry to 'boxify' the selection.

  if(1u < numberOfLines)
  {
    // Boxify the first line.
    lineRun                                           = visualModel->mLines.Begin() + firstLineIndex;
    const SelectionBoxInfo& firstSelectionBoxLineInfo = *(selectionBoxLinesInfo.Begin());

    bool boxifyBegin = (LTR != lineRun->direction) && (LTR != startDirection);
    bool boxifyEnd   = (LTR == lineRun->direction) && (LTR == startDirection);

    if(boxifyBegin)
    {
      quad.x = 0.f;
      quad.y = firstSelectionBoxLineInfo.lineOffset;
      quad.z = firstSelectionBoxLineInfo.minX;
      quad.w = firstSelectionBoxLineInfo.lineOffset + firstSelectionBoxLineInfo.lineHeight;

      // Boxify at the beginning of the line.
      decorator->AddHighlight(actualNumberOfQuads,
                              quad);
      ++actualNumberOfQuads;

      // Update the size of the highlighted text.
      minHighlightX = 0.f;
    }

    if(boxifyEnd)
    {
      quad.x = firstSelectionBoxLineInfo.maxX;
      quad.y = firstSelectionBoxLineInfo.lineOffset;
      quad.z = visualModel->mControlSize.width;
      quad.w = firstSelectionBoxLineInfo.lineOffset + firstSelectionBoxLineInfo.lineHeight;

      // Boxify at the end of the line.
      decorator->AddHighlight(actualNumberOfQuads,
                              quad);
      ++actualNumberOfQuads;

      // Update the size of the highlighted text.
      maxHighlightX = visualModel->mControlSize.width;
    }

    // Boxify the central lines.
    if(2u < numberOfLines)
    {
      for(Vector<SelectionBoxInfo>::ConstIterator it    = selectionBoxLinesInfo.Begin() + 1u,
                                                  endIt = selectionBoxLinesInfo.End() - 1u;
          it != endIt;
          ++it)
      {
        const SelectionBoxInfo& info = *it;

        quad.x = 0.f;
        quad.y = info.lineOffset;
        quad.z = info.minX;
        quad.w = info.lineOffset + info.lineHeight;

        decorator->AddHighlight(actualNumberOfQuads,
                                quad);
        ++actualNumberOfQuads;

        quad.x = info.maxX;
        quad.y = info.lineOffset;
        quad.z = visualModel->mControlSize.width;
        quad.w = info.lineOffset + info.lineHeight;

        decorator->AddHighlight(actualNumberOfQuads,
                                quad);
        ++actualNumberOfQuads;
      }

      // Update the size of the highlighted text.
      minHighlightX = 0.f;
      maxHighlightX = visualModel->mControlSize.width;
    }

    // Boxify the last line.
    lineRun                                          = visualModel->mLines.Begin() + firstLineIndex + numberOfLines - 1u;
    const SelectionBoxInfo& lastSelectionBoxLineInfo = *(selectionBoxLinesInfo.End() - 1u);

    boxifyBegin = (LTR == lineRun->direction) && (LTR == endDirection);
    boxifyEnd   = (LTR != lineRun->direction) && (LTR != endDirection);

    if(boxifyBegin)
    {
      quad.x = 0.f;
      quad.y = lastSelectionBoxLineInfo.lineOffset;
      quad.z = lastSelectionBoxLineInfo.minX;
      quad.w = lastSelectionBoxLineInfo.lineOffset + lastSelectionBoxLineInfo.lineHeight;

      // Boxify at the beginning of the line.
      decorator->AddHighlight(actualNumberOfQuads,
                              quad);
      ++actualNumberOfQuads;

      // Update the size of the highlighted text.
      minHighlightX = 0.f;
    }

    if(boxifyEnd)
    {
      quad.x = lastSelectionBoxLineInfo.maxX;
      quad.y = lastSelectionBoxLineInfo.lineOffset;
      quad.z = visualModel->mControlSize.width;
      quad.w = lastSelectionBoxLineInfo.lineOffset + lastSelectionBoxLineInfo.lineHeight;

      // Boxify at the end of the line.
      decorator->AddHighlight(actualNumberOfQuads, quad);
      ++actualNumberOfQuads;

      // Update the size of the highlighted text.
      maxHighlightX = visualModel->mControlSize.width;
    }
  }

  // Set the actual number of quads.
  decorator->ResizeHighlightQuads(actualNumberOfQuads);

  // Sets the highlight's size and position. In decorator's coords.
  // The highlight's height has been calculated above (before 'boxifying' the highlight).
  highLightSize.width = maxHighlightX - minHighlightX;

  highLightPosition.x                               = minHighlightX;
  const SelectionBoxInfo& firstSelectionBoxLineInfo = *(selectionBoxLinesInfo.Begin());
  highLightPosition.y                               = firstSelectionBoxLineInfo.lineOffset;

  decorator->SetHighLightBox(highLightPosition, highLightSize, static_cast<float>(model->GetOutlineWidth()));

  if(!decorator->IsSmoothHandlePanEnabled())
  {
    CursorInfo primaryCursorInfo;
    impl.GetCursorPosition(eventData->mLeftSelectionPosition, primaryCursorInfo);

    const Vector2 primaryPosition = primaryCursorInfo.primaryPosition + model->mScrollPosition;

    decorator->SetPosition(LEFT_SELECTION_HANDLE,
                           primaryPosition.x,
                           primaryCursorInfo.lineOffset + model->mScrollPosition.y,
                           primaryCursorInfo.lineHeight);

    CursorInfo secondaryCursorInfo;
    impl.GetCursorPosition(eventData->mRightSelectionPosition, secondaryCursorInfo);

    const Vector2 secondaryPosition = secondaryCursorInfo.primaryPosition + model->mScrollPosition;

    decorator->SetPosition(RIGHT_SELECTION_HANDLE,
                           secondaryPosition.x,
                           secondaryCursorInfo.lineOffset + model->mScrollPosition.y,
                           secondaryCursorInfo.lineHeight);
  }

  // Set the flag to update the decorator.
  eventData->mDecoratorUpdated = true;
}

void SelectionHandleController::Reposition(Controller::Impl& impl, float visualX, float visualY, Controller::NoTextTap::Action action)
{
  EventData*& eventData = impl.mEventData;
  if(nullptr == eventData)
  {
    // Nothing to do if there is no text input.
    return;
  }

  if(impl.IsShowingPlaceholderText())
  {
    // Nothing to do if there is the place-holder text.
    return;
  }

  ModelPtr&       model          = impl.mModel;
  VisualModelPtr& visualModel    = model->mVisualModel;
  const Length    numberOfGlyphs = visualModel->mGlyphs.Count();
  const Length    numberOfLines  = visualModel->mLines.Count();
  if((0 == numberOfGlyphs) ||
     (0 == numberOfLines))
  {
    // Nothing to do if there is no text.
    return;
  }

  // Find which word was selected
  CharacterIndex selectionStart(0);
  CharacterIndex selectionEnd(0);
  CharacterIndex noTextHitIndex(0);
  const bool     characterHit = FindSelectionIndices(visualModel,
                                                 model->mLogicalModel,
                                                 impl.mMetrics,
                                                 visualX,
                                                 visualY,
                                                 selectionStart,
                                                 selectionEnd,
                                                 noTextHitIndex);
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "%p selectionStart %d selectionEnd %d\n", &impl, selectionStart, selectionEnd);

  if(characterHit || (Controller::NoTextTap::HIGHLIGHT == action))
  {
    impl.ChangeState(EventData::SELECTING);

    eventData->mLeftSelectionPosition  = selectionStart;
    eventData->mRightSelectionPosition = selectionEnd;

    eventData->mUpdateLeftSelectionPosition  = true;
    eventData->mUpdateRightSelectionPosition = true;
    eventData->mUpdateHighlightBox           = true;

    // It may happen an InputMethodContext commit event arrives before the selection event
    // if the InputMethodContext is in pre-edit state. The commit event will set the
    // eventData->mUpdateCursorPosition flag to true. If it's not set back
    // to false, the highlight box won't be updated.
    eventData->mUpdateCursorPosition = false;

    eventData->mScrollAfterUpdatePosition = (eventData->mLeftSelectionPosition != eventData->mRightSelectionPosition);

    // Cursor to be positioned at end of selection so if selection interrupted and edit mode restarted the cursor will be at end of selection
    eventData->mPrimaryCursorPosition = std::max(eventData->mLeftSelectionPosition, eventData->mRightSelectionPosition);
  }
  else if(Controller::NoTextTap::SHOW_SELECTION_POPUP == action)
  {
    // Nothing to select. i.e. a white space, out of bounds
    impl.ChangeState(EventData::EDITING_WITH_POPUP);

    eventData->mPrimaryCursorPosition = noTextHitIndex;

    eventData->mUpdateCursorPosition      = true;
    eventData->mUpdateGrabHandlePosition  = true;
    eventData->mScrollAfterUpdatePosition = true;
    eventData->mUpdateInputStyle          = true;
  }
  else if(Controller::NoTextTap::NO_ACTION == action)
  {
    // Nothing to select. i.e. a white space, out of bounds
    eventData->mPrimaryCursorPosition = noTextHitIndex;

    eventData->mUpdateCursorPosition      = true;
    eventData->mUpdateGrabHandlePosition  = true;
    eventData->mScrollAfterUpdatePosition = true;
    eventData->mUpdateInputStyle          = true;
  }
}

void SelectionHandleController::Update(Controller::Impl& impl, HandleType handleType, const CursorInfo& cursorInfo)
{
  if((LEFT_SELECTION_HANDLE != handleType) &&
     (RIGHT_SELECTION_HANDLE != handleType))
  {
    return;
  }

  ModelPtr&     model          = impl.mModel;
  const Vector2 cursorPosition = cursorInfo.primaryPosition + model->mScrollPosition;

  // Sets the handle's position.
  EventData*& eventData = impl.mEventData;
  eventData->mDecorator->SetPosition(handleType,
                                     cursorPosition.x,
                                     cursorInfo.lineOffset + model->mScrollPosition.y,
                                     cursorInfo.lineHeight);

  // If selection handle at start of the text and other at end of the text then all text is selected.
  const CharacterIndex startOfSelection = std::min(eventData->mLeftSelectionPosition, eventData->mRightSelectionPosition);
  const CharacterIndex endOfSelection   = std::max(eventData->mLeftSelectionPosition, eventData->mRightSelectionPosition);
  eventData->mAllTextSelected           = (startOfSelection == 0) && (endOfSelection == model->mLogicalModel->mText.Count());
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
