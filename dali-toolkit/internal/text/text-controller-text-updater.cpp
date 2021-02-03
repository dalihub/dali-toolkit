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
#include <dali-toolkit/internal/text/text-controller-text-updater.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <memory.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/markup-processor.h>
#include <dali-toolkit/internal/text/text-controller-impl.h>
#include <dali-toolkit/internal/text/text-editable-control-interface.h>

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
void Controller::TextUpdater::SetText(Controller& controller, const std::string& text)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::SetText\n");

  Controller::Impl& impl = *controller.mImpl;

  // Reset keyboard as text changed
  impl.ResetInputMethodContext();

  // Remove the previously set text and style.
  ResetText(controller);

  // Remove the style.
  controller.ClearStyleData();

  CharacterIndex lastCursorIndex = 0u;

  EventData*& eventData = impl.mEventData;

  if(nullptr != eventData)
  {
    // If popup shown then hide it by switching to Editing state
    if((EventData::SELECTING == eventData->mState) ||
       (EventData::EDITING_WITH_POPUP == eventData->mState) ||
       (EventData::EDITING_WITH_GRAB_HANDLE == eventData->mState) ||
       (EventData::EDITING_WITH_PASTE_POPUP == eventData->mState))
    {
      impl.ChangeState(EventData::EDITING);
    }
  }

  if(!text.empty())
  {
    ModelPtr&        model        = impl.mModel;
    LogicalModelPtr& logicalModel = model->mLogicalModel;
    model->mVisualModel->SetTextColor(impl.mTextColor);

    MarkupProcessData markupProcessData(logicalModel->mColorRuns,
                                        logicalModel->mFontDescriptionRuns,
                                        logicalModel->mEmbeddedItems);

    Length         textSize = 0u;
    const uint8_t* utf8     = NULL;
    if(impl.mMarkupProcessorEnabled)
    {
      ProcessMarkupString(text, markupProcessData);
      textSize = markupProcessData.markupProcessedText.size();

      // This is a bit horrible but std::string returns a (signed) char*
      utf8 = reinterpret_cast<const uint8_t*>(markupProcessData.markupProcessedText.c_str());
    }
    else
    {
      textSize = text.size();

      // This is a bit horrible but std::string returns a (signed) char*
      utf8 = reinterpret_cast<const uint8_t*>(text.c_str());
    }

    //  Convert text into UTF-32
    Vector<Character>& utf32Characters = logicalModel->mText;
    utf32Characters.Resize(textSize);

    // Transform a text array encoded in utf8 into an array encoded in utf32.
    // It returns the actual number of characters.
    Length characterCount = Utf8ToUtf32(utf8, textSize, utf32Characters.Begin());
    utf32Characters.Resize(characterCount);

    DALI_ASSERT_DEBUG(textSize >= characterCount && "Invalid UTF32 conversion length");
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::SetText %p UTF8 size %d, UTF32 size %d\n", &controller, textSize, logicalModel->mText.Count());

    // The characters to be added.
    impl.mTextUpdateInfo.mNumberOfCharactersToAdd = logicalModel->mText.Count();

    // To reset the cursor position
    lastCursorIndex = characterCount;

    // Update the rest of the model during size negotiation
    impl.QueueModifyEvent(ModifyEvent::TEXT_REPLACED);

    // The natural size needs to be re-calculated.
    impl.mRecalculateNaturalSize = true;

    // The text direction needs to be updated.
    impl.mUpdateTextDirection = true;

    // Apply modifications to the model
    impl.mOperationsPending = ALL_OPERATIONS;
  }
  else
  {
    controller.ShowPlaceholderText();
  }

  // Resets the cursor position.
  controller.ResetCursorPosition(lastCursorIndex);

  // Scrolls the text to make the cursor visible.
  controller.ResetScrollPosition();

  impl.RequestRelayout();

  if(nullptr != eventData)
  {
    // Cancel previously queued events
    eventData->mEventQueue.clear();
  }

  // Do this last since it provides callbacks into application code.
  if(NULL != impl.mEditableControlInterface)
  {
    impl.mEditableControlInterface->TextChanged();
  }
}

void Controller::TextUpdater::InsertText(Controller& controller, const std::string& text, Controller::InsertType type)
{
  Controller::Impl& impl      = *controller.mImpl;
  EventData*&       eventData = impl.mEventData;

  DALI_ASSERT_DEBUG(nullptr != eventData && "Unexpected InsertText")

  if(NULL == eventData)
  {
    return;
  }

  bool removedPrevious  = false;
  bool removedSelected  = false;
  bool maxLengthReached = false;

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::InsertText %p %s (%s) mPrimaryCursorPosition %d mPreEditFlag %d mPreEditStartPosition %d mPreEditLength %d\n", &controller, text.c_str(), (COMMIT == type ? "COMMIT" : "PRE_EDIT"), eventData->mPrimaryCursorPosition, eventData->mPreEditFlag, eventData->mPreEditStartPosition, eventData->mPreEditLength);

  ModelPtr&        model        = impl.mModel;
  LogicalModelPtr& logicalModel = model->mLogicalModel;

  // TODO: At the moment the underline runs are only for pre-edit.
  model->mVisualModel->mUnderlineRuns.Clear();

  // Remove the previous InputMethodContext pre-edit.
  if(eventData->mPreEditFlag && (0u != eventData->mPreEditLength))
  {
    removedPrevious = RemoveText(controller,
                                 -static_cast<int>(eventData->mPrimaryCursorPosition - eventData->mPreEditStartPosition),
                                 eventData->mPreEditLength,
                                 DONT_UPDATE_INPUT_STYLE);

    eventData->mPrimaryCursorPosition = eventData->mPreEditStartPosition;
    eventData->mPreEditLength         = 0u;
  }
  else
  {
    // Remove the previous Selection.
    removedSelected = RemoveSelectedText(controller);
  }

  Vector<Character> utf32Characters;
  Length            characterCount = 0u;

  if(!text.empty())
  {
    //  Convert text into UTF-32
    utf32Characters.Resize(text.size());

    // This is a bit horrible but std::string returns a (signed) char*
    const uint8_t* utf8 = reinterpret_cast<const uint8_t*>(text.c_str());

    // Transform a text array encoded in utf8 into an array encoded in utf32.
    // It returns the actual number of characters.
    characterCount = Utf8ToUtf32(utf8, text.size(), utf32Characters.Begin());
    utf32Characters.Resize(characterCount);

    DALI_ASSERT_DEBUG(text.size() >= utf32Characters.Count() && "Invalid UTF32 conversion length");
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "UTF8 size %d, UTF32 size %d\n", text.size(), utf32Characters.Count());
  }

  if(0u != utf32Characters.Count()) // Check if Utf8ToUtf32 conversion succeeded
  {
    // The placeholder text is no longer needed
    if(impl.IsShowingPlaceholderText())
    {
      ResetText(controller);
    }

    impl.ChangeState(EventData::EDITING);

    // Handle the InputMethodContext (predicitive text) state changes
    if(COMMIT == type)
    {
      // InputMethodContext is no longer handling key-events
      impl.ClearPreEditFlag();
    }
    else // PRE_EDIT
    {
      if(!eventData->mPreEditFlag)
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Entered PreEdit state\n");

        // Record the start of the pre-edit text
        eventData->mPreEditStartPosition = eventData->mPrimaryCursorPosition;
      }

      eventData->mPreEditLength = utf32Characters.Count();
      eventData->mPreEditFlag   = true;

      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "mPreEditStartPosition %d mPreEditLength %d\n", eventData->mPreEditStartPosition, eventData->mPreEditLength);
    }

    const Length numberOfCharactersInModel = logicalModel->mText.Count();

    // Restrict new text to fit within Maximum characters setting.
    Length maxSizeOfNewText = std::min((impl.mMaximumNumberOfCharacters - numberOfCharactersInModel), characterCount);
    maxLengthReached        = (characterCount > maxSizeOfNewText);

    // The cursor position.
    CharacterIndex& cursorIndex = eventData->mPrimaryCursorPosition;

    // Update the text's style.

    // Updates the text style runs by adding characters.
    logicalModel->UpdateTextStyleRuns(cursorIndex, maxSizeOfNewText);

    // Get the character index from the cursor index.
    const CharacterIndex styleIndex = (cursorIndex > 0u) ? cursorIndex - 1u : 0u;

    // Retrieve the text's style for the given index.
    InputStyle style;
    impl.RetrieveDefaultInputStyle(style);
    logicalModel->RetrieveStyle(styleIndex, style);

    InputStyle& inputStyle = eventData->mInputStyle;

    // Whether to add a new text color run.
    const bool addColorRun = (style.textColor != inputStyle.textColor) && !inputStyle.isDefaultColor;

    // Whether to add a new font run.
    const bool addFontNameRun   = (style.familyName != inputStyle.familyName) && inputStyle.isFamilyDefined;
    const bool addFontWeightRun = (style.weight != inputStyle.weight) && inputStyle.isWeightDefined;
    const bool addFontWidthRun  = (style.width != inputStyle.width) && inputStyle.isWidthDefined;
    const bool addFontSlantRun  = (style.slant != inputStyle.slant) && inputStyle.isSlantDefined;
    const bool addFontSizeRun   = (style.size != inputStyle.size) && inputStyle.isSizeDefined;

    // Add style runs.
    if(addColorRun)
    {
      const VectorBase::SizeType numberOfRuns = logicalModel->mColorRuns.Count();
      logicalModel->mColorRuns.Resize(numberOfRuns + 1u);

      ColorRun& colorRun                       = *(logicalModel->mColorRuns.Begin() + numberOfRuns);
      colorRun.color                           = inputStyle.textColor;
      colorRun.characterRun.characterIndex     = cursorIndex;
      colorRun.characterRun.numberOfCharacters = maxSizeOfNewText;
    }

    if(addFontNameRun ||
       addFontWeightRun ||
       addFontWidthRun ||
       addFontSlantRun ||
       addFontSizeRun)
    {
      const VectorBase::SizeType numberOfRuns = logicalModel->mFontDescriptionRuns.Count();
      logicalModel->mFontDescriptionRuns.Resize(numberOfRuns + 1u);

      FontDescriptionRun& fontDescriptionRun = *(logicalModel->mFontDescriptionRuns.Begin() + numberOfRuns);

      if(addFontNameRun)
      {
        fontDescriptionRun.familyLength = inputStyle.familyName.size();
        fontDescriptionRun.familyName   = new char[fontDescriptionRun.familyLength];
        memcpy(fontDescriptionRun.familyName, inputStyle.familyName.c_str(), fontDescriptionRun.familyLength);
        fontDescriptionRun.familyDefined = true;

        // The memory allocated for the font family name is freed when the font description is removed from the logical model.
      }

      if(addFontWeightRun)
      {
        fontDescriptionRun.weight        = inputStyle.weight;
        fontDescriptionRun.weightDefined = true;
      }

      if(addFontWidthRun)
      {
        fontDescriptionRun.width        = inputStyle.width;
        fontDescriptionRun.widthDefined = true;
      }

      if(addFontSlantRun)
      {
        fontDescriptionRun.slant        = inputStyle.slant;
        fontDescriptionRun.slantDefined = true;
      }

      if(addFontSizeRun)
      {
        fontDescriptionRun.size        = static_cast<PointSize26Dot6>(inputStyle.size * impl.mFontSizeScale * 64.f);
        fontDescriptionRun.sizeDefined = true;
      }

      fontDescriptionRun.characterRun.characterIndex     = cursorIndex;
      fontDescriptionRun.characterRun.numberOfCharacters = maxSizeOfNewText;
    }

    // Insert at current cursor position.
    Vector<Character>& modifyText = logicalModel->mText;

    auto pos = modifyText.End();
    if(cursorIndex < numberOfCharactersInModel)
    {
      pos = modifyText.Begin() + cursorIndex;
    }
    unsigned int realPos = pos - modifyText.Begin();
    modifyText.Insert(pos, utf32Characters.Begin(), utf32Characters.Begin() + maxSizeOfNewText);

    if(NULL != impl.mEditableControlInterface)
    {
      impl.mEditableControlInterface->TextInserted(realPos, maxSizeOfNewText, text);
    }

    TextUpdateInfo& textUpdateInfo = impl.mTextUpdateInfo;

    // Mark the first paragraph to be updated.
    if(Layout::Engine::SINGLE_LINE_BOX == impl.mLayoutEngine.GetLayout())
    {
      textUpdateInfo.mCharacterIndex             = 0;
      textUpdateInfo.mNumberOfCharactersToRemove = textUpdateInfo.mPreviousNumberOfCharacters;
      textUpdateInfo.mNumberOfCharactersToAdd    = numberOfCharactersInModel + maxSizeOfNewText;
      textUpdateInfo.mClearAll                   = true;
    }
    else
    {
      textUpdateInfo.mCharacterIndex = std::min(cursorIndex, textUpdateInfo.mCharacterIndex);
      textUpdateInfo.mNumberOfCharactersToAdd += maxSizeOfNewText;
    }

    // Update the cursor index.
    cursorIndex += maxSizeOfNewText;

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Inserted %d characters, new size %d new cursor %d\n", maxSizeOfNewText, logicalModel->mText.Count(), eventData->mPrimaryCursorPosition);
  }

  if((0u == logicalModel->mText.Count()) &&
     impl.IsPlaceholderAvailable())
  {
    // Show place-holder if empty after removing the pre-edit text
    controller.ShowPlaceholderText();
    eventData->mUpdateCursorPosition = true;
    impl.ClearPreEditFlag();
  }
  else if(removedPrevious ||
          removedSelected ||
          (0 != utf32Characters.Count()))
  {
    // Queue an inserted event
    impl.QueueModifyEvent(ModifyEvent::TEXT_INSERTED);

    eventData->mUpdateCursorPosition = true;
    if(removedSelected)
    {
      eventData->mScrollAfterDelete = true;
    }
    else
    {
      eventData->mScrollAfterUpdatePosition = true;
    }
  }

  if(maxLengthReached)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "MaxLengthReached (%d)\n", logicalModel->mText.Count());

    impl.ResetInputMethodContext();

    if(NULL != impl.mEditableControlInterface)
    {
      // Do this last since it provides callbacks into application code
      impl.mEditableControlInterface->MaxLengthReached();
    }
  }
}

void Controller::TextUpdater::PasteText(Controller& controller, const std::string& stringToPaste)
{
  InsertText(controller, stringToPaste, Text::Controller::COMMIT);
  Controller::Impl& impl = *controller.mImpl;
  impl.ChangeState(EventData::EDITING);
  impl.RequestRelayout();

  if(NULL != impl.mEditableControlInterface)
  {
    // Do this last since it provides callbacks into application code
    impl.mEditableControlInterface->TextChanged();
  }
}

bool Controller::TextUpdater::RemoveText(
  Controller&          controller,
  int                  cursorOffset,
  int                  numberOfCharacters,
  UpdateInputStyleType type)
{
  bool removed = false;

  Controller::Impl& impl      = *controller.mImpl;
  EventData*&       eventData = impl.mEventData;

  if(nullptr == eventData)
  {
    return removed;
  }

  ModelPtr&        model        = impl.mModel;
  LogicalModelPtr& logicalModel = model->mLogicalModel;

  DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::RemoveText %p mText.Count() %d cursor %d cursorOffset %d numberOfCharacters %d\n", &controller, logicalModel->mText.Count(), eventData->mPrimaryCursorPosition, cursorOffset, numberOfCharacters);

  if(!impl.IsShowingPlaceholderText())
  {
    // Delete at current cursor position
    Vector<Character>& currentText    = logicalModel->mText;
    CharacterIndex&    oldCursorIndex = eventData->mPrimaryCursorPosition;

    CharacterIndex cursorIndex = 0;

    // Validate the cursor position & number of characters
    if((static_cast<int>(eventData->mPrimaryCursorPosition) + cursorOffset) >= 0)
    {
      cursorIndex = eventData->mPrimaryCursorPosition + cursorOffset;
    }

    if((cursorIndex + numberOfCharacters) > currentText.Count())
    {
      numberOfCharacters = currentText.Count() - cursorIndex;
    }

    TextUpdateInfo& textUpdateInfo = impl.mTextUpdateInfo;

    if(eventData->mPreEditFlag || // If the preedit flag is enabled, it means two (or more) of them came together i.e. when two keys have been pressed at the same time.
       ((cursorIndex + numberOfCharacters) <= textUpdateInfo.mPreviousNumberOfCharacters))
    {
      // Mark the paragraphs to be updated.
      if(Layout::Engine::SINGLE_LINE_BOX == impl.mLayoutEngine.GetLayout())
      {
        textUpdateInfo.mCharacterIndex             = 0;
        textUpdateInfo.mNumberOfCharactersToRemove = textUpdateInfo.mPreviousNumberOfCharacters;
        textUpdateInfo.mNumberOfCharactersToAdd    = textUpdateInfo.mPreviousNumberOfCharacters - numberOfCharacters;
        textUpdateInfo.mClearAll                   = true;
      }
      else
      {
        textUpdateInfo.mCharacterIndex = std::min(cursorIndex, textUpdateInfo.mCharacterIndex);
        textUpdateInfo.mNumberOfCharactersToRemove += numberOfCharacters;
      }

      // Update the input style and remove the text's style before removing the text.

      if(UPDATE_INPUT_STYLE == type)
      {
        InputStyle& eventDataInputStyle = eventData->mInputStyle;

        // Keep a copy of the current input style.
        InputStyle currentInputStyle;
        currentInputStyle.Copy(eventDataInputStyle);

        // Set first the default input style.
        impl.RetrieveDefaultInputStyle(eventDataInputStyle);

        // Update the input style.
        logicalModel->RetrieveStyle(cursorIndex, eventDataInputStyle);

        // Compare if the input style has changed.
        const bool hasInputStyleChanged = !currentInputStyle.Equal(eventDataInputStyle);

        if(hasInputStyleChanged)
        {
          const InputStyle::Mask styleChangedMask = currentInputStyle.GetInputStyleChangeMask(eventDataInputStyle);
          // Queue the input style changed signal.
          eventData->mInputStyleChangedQueue.PushBack(styleChangedMask);
        }
      }

      // If the number of current text and the number of characters to be deleted are same,
      // it means all texts should be removed and all Preedit variables should be initialized.
      if((currentText.Count() - numberOfCharacters == 0) && (cursorIndex == 0))
      {
        impl.ClearPreEditFlag();
        textUpdateInfo.mNumberOfCharactersToAdd = 0;
      }

      // Updates the text style runs by removing characters. Runs with no characters are removed.
      logicalModel->UpdateTextStyleRuns(cursorIndex, -numberOfCharacters);

      // Remove the characters.
      Vector<Character>::Iterator first = currentText.Begin() + cursorIndex;
      Vector<Character>::Iterator last  = first + numberOfCharacters;

      if(NULL != impl.mEditableControlInterface)
      {
        std::string utf8;
        Utf32ToUtf8(first, numberOfCharacters, utf8);
        impl.mEditableControlInterface->TextDeleted(cursorIndex, numberOfCharacters, utf8);
      }

      currentText.Erase(first, last);

      // Cursor position retreat
      oldCursorIndex = cursorIndex;

      eventData->mScrollAfterDelete = true;

      if(EventData::INACTIVE == eventData->mState)
      {
        impl.ChangeState(EventData::EDITING);
      }

      DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::RemoveText %p removed %d\n", &controller, numberOfCharacters);
      removed = true;
    }
  }

  return removed;
}

bool Controller::TextUpdater::RemoveSelectedText(Controller& controller)
{
  bool textRemoved(false);

  Controller::Impl& impl = *controller.mImpl;

  if(EventData::SELECTING == impl.mEventData->mState)
  {
    std::string removedString;
    impl.RetrieveSelection(removedString, true);

    if(!removedString.empty())
    {
      textRemoved = true;
      impl.ChangeState(EventData::EDITING);
    }
  }

  return textRemoved;
}

void Controller::TextUpdater::ResetText(Controller& controller)
{
  Controller::Impl& impl         = *controller.mImpl;
  LogicalModelPtr&  logicalModel = impl.mModel->mLogicalModel;

  // Reset buffers.
  logicalModel->mText.Clear();

  // Reset the embedded images buffer.
  logicalModel->ClearEmbeddedImages();

  // We have cleared everything including the placeholder-text
  impl.PlaceholderCleared();

  impl.mTextUpdateInfo.mCharacterIndex             = 0u;
  impl.mTextUpdateInfo.mNumberOfCharactersToRemove = impl.mTextUpdateInfo.mPreviousNumberOfCharacters;
  impl.mTextUpdateInfo.mNumberOfCharactersToAdd    = 0u;

  // Clear any previous text.
  impl.mTextUpdateInfo.mClearAll = true;

  // The natural size needs to be re-calculated.
  impl.mRecalculateNaturalSize = true;

  // The text direction needs to be updated.
  impl.mUpdateTextDirection = true;

  // Apply modifications to the model
  impl.mOperationsPending = ALL_OPERATIONS;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
