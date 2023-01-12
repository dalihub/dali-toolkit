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
#include <dali-toolkit/internal/text/controller/text-controller-event-handler.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/clipboard-event-notifier.h>
#include <dali/devel-api/adaptor-framework/key-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/controller/text-controller-impl.h>
#include <dali-toolkit/internal/text/controller/text-controller-placeholder-handler.h>
#include <dali-toolkit/internal/text/controller/text-controller-text-updater.h>
#include <dali-toolkit/internal/text/cursor-helper-functions.h>
#include <dali-toolkit/internal/text/text-editable-control-interface.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_TEXT_PERFORMANCE_MARKER, false);

const std::string KEY_C_NAME      = "c";
const std::string KEY_V_NAME      = "v";
const std::string KEY_X_NAME      = "x";
const std::string KEY_A_NAME      = "a";
const std::string KEY_INSERT_NAME = "Insert";

} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Text
{
void Controller::EventHandler::KeyboardFocusGainEvent(Controller& controller)
{
  DALI_ASSERT_DEBUG(controller.mImpl->mEventData && "Unexpected KeyboardFocusGainEvent");

  if(NULL != controller.mImpl->mEventData)
  {
    if((EventData::INACTIVE == controller.mImpl->mEventData->mState) ||
       (EventData::INTERRUPTED == controller.mImpl->mEventData->mState))
    {
      controller.mImpl->ChangeState(EventData::EDITING);
      controller.mImpl->mEventData->mUpdateCursorPosition      = true; //If editing started without tap event, cursor update must be triggered.
      controller.mImpl->mEventData->mUpdateInputStyle          = true;
      controller.mImpl->mEventData->mScrollAfterUpdatePosition = true;
    }
    controller.mImpl->NotifyInputMethodContextMultiLineStatus();
    if(controller.mImpl->IsShowingPlaceholderText())
    {
      // Show alternative placeholder-text when editing
      PlaceholderHandler::ShowPlaceholderText(*controller.mImpl);
    }

    controller.mImpl->RequestRelayout();
  }
}

void Controller::EventHandler::KeyboardFocusLostEvent(Controller& controller)
{
  DALI_ASSERT_DEBUG(controller.mImpl->mEventData && "Unexpected KeyboardFocusLostEvent");

  if(NULL != controller.mImpl->mEventData)
  {
    if(EventData::INTERRUPTED != controller.mImpl->mEventData->mState)
    {
      // Init selection position
      if(controller.mImpl->mEventData->mState == EventData::SELECTING)
      {
        uint32_t oldStart, oldEnd;
        oldStart = controller.mImpl->mEventData->mLeftSelectionPosition;
        oldEnd   = controller.mImpl->mEventData->mRightSelectionPosition;

        controller.mImpl->mEventData->mLeftSelectionPosition  = controller.mImpl->mEventData->mPrimaryCursorPosition;
        controller.mImpl->mEventData->mRightSelectionPosition = controller.mImpl->mEventData->mPrimaryCursorPosition;

        if(controller.mImpl->mSelectableControlInterface != nullptr)
        {
          controller.mImpl->mSelectableControlInterface->SelectionChanged(oldStart, oldEnd, controller.mImpl->mEventData->mPrimaryCursorPosition, controller.mImpl->mEventData->mPrimaryCursorPosition);
        }
      }

      controller.mImpl->ChangeState(EventData::INACTIVE);

      if(!controller.mImpl->IsShowingRealText())
      {
        // Revert to regular placeholder-text when not editing
        PlaceholderHandler::ShowPlaceholderText(*controller.mImpl);
      }
    }
  }
  controller.mImpl->RequestRelayout();
}

bool Controller::EventHandler::KeyEvent(Controller& controller, const Dali::KeyEvent& keyEvent)
{
  DALI_ASSERT_DEBUG(controller.mImpl->mEventData && "Unexpected KeyEvent");

  bool textChanged    = false;
  bool relayoutNeeded = false;
  bool isEditable     = controller.IsEditable() && controller.IsUserInteractionEnabled();

  if((NULL != controller.mImpl->mEventData) &&
     (keyEvent.GetState() == KeyEvent::DOWN))
  {
    int                keyCode   = keyEvent.GetKeyCode();
    const std::string& keyString = keyEvent.GetKeyString();
    const std::string  keyName   = keyEvent.GetKeyName();
    // Key will produce same logical-key value when ctrl
    // is down, regardless of language layout
    const std::string logicalKey = keyEvent.GetLogicalKey();

    const bool isNullKey = (0 == keyCode) && (keyString.empty());

    // Pre-process to separate modifying events from non-modifying input events.
    if(isNullKey)
    {
      // In some platforms arrive key events with no key code.
      // Do nothing.
      return false;
    }
    else if(Dali::DALI_KEY_ESCAPE == keyCode || Dali::DALI_KEY_BACK == keyCode || Dali::DALI_KEY_SEARCH == keyCode)
    {
      // Do nothing
      return false;
    }
    else if((Dali::DALI_KEY_CURSOR_LEFT == keyCode) ||
            (Dali::DALI_KEY_CURSOR_RIGHT == keyCode) ||
            (Dali::DALI_KEY_CURSOR_UP == keyCode) ||
            (Dali::DALI_KEY_CURSOR_DOWN == keyCode))
    {
      // If don't have any text, do nothing.
      if(!controller.mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters || !isEditable)
      {
        return false;
      }

      uint32_t cursorPosition     = controller.mImpl->mEventData->mPrimaryCursorPosition;
      uint32_t numberOfCharacters = controller.mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;
      uint32_t cursorLine         = controller.mImpl->mModel->mVisualModel->GetLineOfCharacter(cursorPosition);
      uint32_t numberOfLines      = controller.mImpl->mModel->GetNumberOfLines();

      // Logic to determine whether this text control will lose focus or not.
      if((Dali::DALI_KEY_CURSOR_LEFT == keyCode && 0 == cursorPosition && !keyEvent.IsShiftModifier()) ||
         (Dali::DALI_KEY_CURSOR_RIGHT == keyCode && numberOfCharacters == cursorPosition && !keyEvent.IsShiftModifier()) ||
         (Dali::DALI_KEY_CURSOR_DOWN == keyCode && cursorLine == numberOfLines - 1) ||
         (Dali::DALI_KEY_CURSOR_DOWN == keyCode && numberOfCharacters == cursorPosition && cursorLine - 1 == numberOfLines - 1) ||
         (Dali::DALI_KEY_CURSOR_UP == keyCode && cursorLine == 0) ||
         (Dali::DALI_KEY_CURSOR_UP == keyCode && numberOfCharacters == cursorPosition && cursorLine == 1))
      {
        // Release the active highlight.
        if(controller.mImpl->mEventData->mState == EventData::SELECTING)
        {
          uint32_t oldStart, oldEnd;
          oldStart = controller.mImpl->mEventData->mLeftSelectionPosition;
          oldEnd   = controller.mImpl->mEventData->mRightSelectionPosition;

          controller.mImpl->ChangeState(EventData::EDITING);

          // Update selection position.
          controller.mImpl->mEventData->mLeftSelectionPosition  = controller.mImpl->mEventData->mPrimaryCursorPosition;
          controller.mImpl->mEventData->mRightSelectionPosition = controller.mImpl->mEventData->mPrimaryCursorPosition;
          controller.mImpl->mEventData->mUpdateCursorPosition   = true;

          if(controller.mImpl->mSelectableControlInterface != nullptr)
          {
            controller.mImpl->mSelectableControlInterface->SelectionChanged(oldStart, oldEnd, controller.mImpl->mEventData->mLeftSelectionPosition, controller.mImpl->mEventData->mRightSelectionPosition);
          }

          controller.mImpl->RequestRelayout();
        }
        return false;
      }

      if(controller.mImpl->mEventData->mState == EventData::INACTIVE)
      {
        // Cursor position will be updated
        controller.mImpl->ChangeState(EventData::EDITING);
      }

      controller.mImpl->mEventData->mCheckScrollAmount = true;
      Event event(Event::CURSOR_KEY_EVENT);
      event.p1.mInt  = keyCode;
      event.p2.mBool = keyEvent.IsShiftModifier();
      controller.mImpl->mEventData->mEventQueue.push_back(event);

      // Will request for relayout.
      relayoutNeeded = true;
    }
    else if(Dali::DevelKey::DALI_KEY_CONTROL_LEFT == keyCode || Dali::DevelKey::DALI_KEY_CONTROL_RIGHT == keyCode)
    {
      // Left or Right Control key event is received before Ctrl-C/V/X key event is received
      // If not handle it here, any selected text will be deleted

      // Do nothing
      return false;
    }
    else if(keyEvent.IsCtrlModifier() && !keyEvent.IsShiftModifier() && isEditable)
    {
      bool consumed = false;
      if(keyName == KEY_C_NAME || keyName == KEY_INSERT_NAME || logicalKey == KEY_C_NAME || logicalKey == KEY_INSERT_NAME)
      {
        // Ctrl-C or Ctrl+Insert to copy the selected text
        controller.TextPopupButtonTouched(Toolkit::TextSelectionPopup::COPY);
        consumed = true;
      }
      else if(keyName == KEY_V_NAME || logicalKey == KEY_V_NAME)
      {
        // Ctrl-V to paste the copied text
        controller.TextPopupButtonTouched(Toolkit::TextSelectionPopup::PASTE);
        consumed = true;
      }
      else if(keyName == KEY_X_NAME || logicalKey == KEY_X_NAME)
      {
        // Ctrl-X to cut the selected text
        controller.TextPopupButtonTouched(Toolkit::TextSelectionPopup::CUT);
        consumed = true;
      }
      else if(keyName == KEY_A_NAME || logicalKey == KEY_A_NAME)
      {
        // Ctrl-A to select All the text
        controller.TextPopupButtonTouched(Toolkit::TextSelectionPopup::SELECT_ALL);
        consumed = true;
      }
      return consumed;
    }
    else if((Dali::DALI_KEY_BACKSPACE == keyCode) ||
            (Dali::DevelKey::DALI_KEY_DELETE == keyCode))
    {
      textChanged = DeleteEvent(controller, keyCode);

      // Will request for relayout.
      relayoutNeeded = true;
    }
    else if(IsKey(keyEvent, Dali::DALI_KEY_POWER) ||
            IsKey(keyEvent, Dali::DALI_KEY_MENU) ||
            IsKey(keyEvent, Dali::DALI_KEY_HOME))
    {
      // Power key/Menu/Home key behaviour does not allow edit mode to resume.
      controller.mImpl->ChangeState(EventData::INACTIVE);

      // Will request for relayout.
      relayoutNeeded = true;

      // This branch avoids calling the InsertText() method of the 'else' branch which can delete selected text.
    }
    else if((Dali::DALI_KEY_SHIFT_LEFT == keyCode) || (Dali::DALI_KEY_SHIFT_RIGHT == keyCode))
    {
      // DALI_KEY_SHIFT_LEFT or DALI_KEY_SHIFT_RIGHT is the key code for Shift. It's sent (by the InputMethodContext?) when the predictive text is enabled
      // and a character is typed after the type of a upper case latin character.

      // Do nothing.
      return false;
    }
    else if((Dali::DALI_KEY_VOLUME_UP == keyCode) || (Dali::DALI_KEY_VOLUME_DOWN == keyCode))
    {
      // This branch avoids calling the InsertText() method of the 'else' branch which can delete selected text.
      // Do nothing.
      return false;
    }
    else
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::KeyEvent %p keyString %s\n", &controller, keyString.c_str());
      if(!isEditable) return false;

      std::string refinedKey = keyString;
      if(controller.mImpl->mInputFilter != NULL && !refinedKey.empty())
      {
        bool accepted = false;
        bool rejected = false;
        accepted      = controller.mImpl->mInputFilter->Contains(Toolkit::InputFilter::Property::ACCEPTED, keyString);
        rejected      = controller.mImpl->mInputFilter->Contains(Toolkit::InputFilter::Property::REJECTED, keyString);

        if(!accepted)
        {
          // The filtered key is set to empty.
          refinedKey = "";
          // Signal emits when the character to be inserted is filtered by the accepted filter.
          controller.mImpl->mEditableControlInterface->InputFiltered(Toolkit::InputFilter::Property::ACCEPTED);
        }
        if(rejected)
        {
          // The filtered key is set to empty.
          refinedKey = "";
          // Signal emits when the character to be inserted is filtered by the rejected filter.
          controller.mImpl->mEditableControlInterface->InputFiltered(Toolkit::InputFilter::Property::REJECTED);
        }
      }

      if(!refinedKey.empty())
      {
        // InputMethodContext is no longer handling key-events
        controller.mImpl->ClearPreEditFlag();

        TextUpdater::InsertText(controller, refinedKey, COMMIT);

        textChanged = true;

        // Will request for relayout.
        relayoutNeeded = true;
      }
    }

    if((controller.mImpl->mEventData->mState != EventData::INTERRUPTED) &&
       (controller.mImpl->mEventData->mState != EventData::INACTIVE) &&
       (!isNullKey) &&
       (Dali::DALI_KEY_SHIFT_LEFT != keyCode) &&
       (Dali::DALI_KEY_SHIFT_RIGHT != keyCode) &&
       (Dali::DALI_KEY_VOLUME_UP != keyCode) &&
       (Dali::DALI_KEY_VOLUME_DOWN != keyCode))
    {
      // Should not change the state if the key is the shift send by the InputMethodContext.
      // Otherwise, when the state is SELECTING the text controller can't send the right
      // surrounding info to the InputMethodContext.
      controller.mImpl->ChangeState(EventData::EDITING);

      // Will request for relayout.
      relayoutNeeded = true;
    }

    if(relayoutNeeded)
    {
      controller.mImpl->RequestRelayout();
    }
  }
  else if((NULL != controller.mImpl->mEventData) && (keyEvent.GetState() == KeyEvent::UP))
  {
    // Handles specific keys that require event propagation.
    if(Dali::DALI_KEY_BACK == keyEvent.GetKeyCode())
    {
      // Do nothing
      return false;
    }
  }

  if(textChanged &&
     (NULL != controller.mImpl->mEditableControlInterface))
  {
    // Do this last since it provides callbacks into application code
    controller.mImpl->mEditableControlInterface->TextChanged(false);
  }

  return true;
}

void Controller::EventHandler::AnchorEvent(Controller& controller, float x, float y)
{
  if(!controller.mImpl->mMarkupProcessorEnabled ||
     !controller.mImpl->mModel->mLogicalModel->mAnchors.Count() ||
     !controller.mImpl->IsShowingRealText())
  {
    return;
  }

  CharacterIndex cursorPosition = 0u;

  // Convert from control's coords to text's coords.
  const float xPosition = x - controller.mImpl->mModel->mScrollPosition.x;
  const float yPosition = y - controller.mImpl->mModel->mScrollPosition.y;

  // Whether to touch point hits on a glyph.
  bool matchedCharacter = false;
  cursorPosition        = Text::GetClosestCursorIndex(controller.mImpl->mModel->mVisualModel,
                                               controller.mImpl->mModel->mLogicalModel,
                                               controller.mImpl->mMetrics,
                                               xPosition,
                                               yPosition,
                                               CharacterHitTest::TAP,
                                               matchedCharacter);

  for(const auto& anchor : controller.mImpl->mModel->mLogicalModel->mAnchors)
  {
    // Anchor clicked if the calculated cursor position is within the range of anchor.
    if(cursorPosition >= anchor.startIndex && cursorPosition < anchor.endIndex)
    {
      if(controller.mImpl->mAnchorControlInterface && anchor.href)
      {
        std::string href(anchor.href);
        controller.mImpl->mAnchorControlInterface->AnchorClicked(href);
        break;
      }
    }
  }
}

void Controller::EventHandler::TapEvent(Controller& controller, unsigned int tapCount, float x, float y)
{
  DALI_ASSERT_DEBUG(controller.mImpl->mEventData && "Unexpected TapEvent");

  if(NULL != controller.mImpl->mEventData)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Concise, "TapEvent state:%d \n", controller.mImpl->mEventData->mState);
    EventData::State state(controller.mImpl->mEventData->mState);
    bool             relayoutNeeded(false); // to avoid unnecessary relayouts when tapping an empty text-field

    if(controller.mImpl->IsClipboardVisible())
    {
      if(EventData::INACTIVE == state || EventData::EDITING == state)
      {
        controller.mImpl->ChangeState(EventData::EDITING_WITH_GRAB_HANDLE);
      }
      relayoutNeeded = true;
    }
    else if(1u == tapCount)
    {
      if(EventData::EDITING_WITH_POPUP == state || EventData::EDITING_WITH_PASTE_POPUP == state)
      {
        controller.mImpl->ChangeState(EventData::EDITING_WITH_GRAB_HANDLE); // If Popup shown hide it here so can be shown again if required.
      }

      if(controller.mImpl->IsShowingRealText() && (EventData::INACTIVE != state))
      {
        controller.mImpl->ChangeState(EventData::EDITING_WITH_GRAB_HANDLE);
        relayoutNeeded = true;
      }
      else
      {
        if(controller.mImpl->IsShowingPlaceholderText() && !controller.mImpl->IsFocusedPlaceholderAvailable())
        {
          // Hide placeholder text
          TextUpdater::ResetText(controller);
        }

        if(EventData::INACTIVE == state)
        {
          controller.mImpl->ChangeState(EventData::EDITING);
        }
        else if(!controller.mImpl->IsClipboardEmpty())
        {
          controller.mImpl->ChangeState(EventData::EDITING_WITH_POPUP);
        }
        relayoutNeeded = true;
      }
    }
    else if(2u == tapCount)
    {
      if(controller.mImpl->mEventData->mSelectionEnabled &&
         controller.mImpl->IsShowingRealText())
      {
        relayoutNeeded                                       = true;
        controller.mImpl->mEventData->mIsLeftHandleSelected  = true;
        controller.mImpl->mEventData->mIsRightHandleSelected = true;
      }
    }

    // Handles & cursors must be repositioned after Relayout() i.e. after the Model has been updated
    if(relayoutNeeded)
    {
      Event event(Event::TAP_EVENT);
      event.p1.mUint  = tapCount;
      event.p2.mFloat = x;
      event.p3.mFloat = y;
      controller.mImpl->mEventData->mEventQueue.push_back(event);

      controller.mImpl->RequestRelayout();
    }
  }

  // Reset keyboard as tap event has occurred.
  controller.mImpl->ResetInputMethodContext();
}

void Controller::EventHandler::PanEvent(Controller& controller, GestureState state, const Vector2& displacement)
{
  DALI_ASSERT_DEBUG(controller.mImpl->mEventData && "Unexpected PanEvent");

  if(NULL != controller.mImpl->mEventData)
  {
    Event event(Event::PAN_EVENT);
    event.p1.mInt   = static_cast<int>(state);
    event.p2.mFloat = displacement.x;
    event.p3.mFloat = displacement.y;
    controller.mImpl->mEventData->mEventQueue.push_back(event);

    controller.mImpl->RequestRelayout();
  }
}

void Controller::EventHandler::LongPressEvent(Controller& controller, GestureState state, float x, float y)
{
  DALI_ASSERT_DEBUG(controller.mImpl->mEventData && "Unexpected LongPressEvent");

  if((state == GestureState::STARTED) &&
     (NULL != controller.mImpl->mEventData))
  {
    // The 1st long-press on inactive text-field is treated as tap
    if(EventData::INACTIVE == controller.mImpl->mEventData->mState)
    {
      controller.mImpl->ChangeState(EventData::EDITING);

      Event event(Event::TAP_EVENT);
      event.p1.mUint  = 1;
      event.p2.mFloat = x;
      event.p3.mFloat = y;
      controller.mImpl->mEventData->mEventQueue.push_back(event);

      controller.mImpl->RequestRelayout();
    }
    else if(!controller.mImpl->IsShowingRealText())
    {
      Event event(Event::LONG_PRESS_EVENT);
      event.p1.mInt   = static_cast<int>(state);
      event.p2.mFloat = x;
      event.p3.mFloat = y;
      controller.mImpl->mEventData->mEventQueue.push_back(event);
      controller.mImpl->RequestRelayout();
    }
    else if(!controller.mImpl->IsClipboardVisible())
    {
      // Reset the InputMethodContext to commit the pre-edit before selecting the text.
      controller.mImpl->ResetInputMethodContext();

      Event event(Event::LONG_PRESS_EVENT);
      event.p1.mInt   = static_cast<int>(state);
      event.p2.mFloat = x;
      event.p3.mFloat = y;
      controller.mImpl->mEventData->mEventQueue.push_back(event);
      controller.mImpl->RequestRelayout();

      controller.mImpl->mEventData->mIsLeftHandleSelected  = true;
      controller.mImpl->mEventData->mIsRightHandleSelected = true;
    }
  }
}

void Controller::EventHandler::SelectEvent(Controller& controller, float x, float y, SelectionType selectType)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::SelectEvent\n");

  if(NULL != controller.mImpl->mEventData)
  {
    if(selectType == SelectionType::ALL)
    {
      Event event(Event::SELECT_ALL);
      controller.mImpl->mEventData->mEventQueue.push_back(event);
    }
    else if(selectType == SelectionType::NONE)
    {
      Event event(Event::SELECT_NONE);
      controller.mImpl->mEventData->mEventQueue.push_back(event);
    }
    else
    {
      Event event(Event::SELECT);
      event.p2.mFloat = x;
      event.p3.mFloat = y;
      controller.mImpl->mEventData->mEventQueue.push_back(event);
    }

    controller.mImpl->mEventData->mCheckScrollAmount     = true;
    controller.mImpl->mEventData->mIsLeftHandleSelected  = true;
    controller.mImpl->mEventData->mIsRightHandleSelected = true;
    controller.mImpl->RequestRelayout();
  }
}

void Controller::EventHandler::SelectEvent(Controller& controller, const uint32_t start, const uint32_t end, SelectionType selectType)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::SelectEvent\n");

  if(NULL != controller.mImpl->mEventData)
  {
    if(selectType == SelectionType::RANGE)
    {
      Event event(Event::SELECT_RANGE);
      event.p2.mUint = start;
      event.p3.mUint = end;
      controller.mImpl->mEventData->mEventQueue.push_back(event);
    }

    controller.mImpl->mEventData->mCheckScrollAmount     = true;
    controller.mImpl->mEventData->mIsLeftHandleSelected  = true;
    controller.mImpl->mEventData->mIsRightHandleSelected = true;
    controller.mImpl->RequestRelayout();
  }
}

void Controller::EventHandler::ProcessModifyEvents(Controller& controller)
{
  Vector<ModifyEvent>& events = controller.mImpl->mModifyEvents;

  if(0u == events.Count())
  {
    // Nothing to do.
    return;
  }

  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_MODIFY_EVENTS");

  for(Vector<ModifyEvent>::ConstIterator it    = events.Begin(),
                                         endIt = events.End();
      it != endIt;
      ++it)
  {
    const ModifyEvent& event = *it;

    if(ModifyEvent::TEXT_REPLACED == event.type)
    {
      // A (single) replace event should come first, otherwise we wasted time processing NOOP events
      DALI_ASSERT_DEBUG(it == events.Begin() && "Unexpected TEXT_REPLACED event");

      TextReplacedEvent(controller);
    }
    else if(ModifyEvent::TEXT_INSERTED == event.type)
    {
      TextInsertedEvent(controller);
    }
    else if(ModifyEvent::TEXT_DELETED == event.type)
    {
      // Placeholder-text cannot be deleted
      if(!controller.mImpl->IsShowingPlaceholderText())
      {
        TextDeletedEvent(controller);
      }
    }
  }

  if(NULL != controller.mImpl->mEventData)
  {
    uint32_t oldStart, oldEnd;
    oldStart = controller.mImpl->mEventData->mLeftSelectionPosition;
    oldEnd   = controller.mImpl->mEventData->mRightSelectionPosition;

    // When the text is being modified, delay cursor blinking
    controller.mImpl->mEventData->mDecorator->DelayCursorBlink();

    // Update selection position after modifying the text
    controller.mImpl->mEventData->mLeftSelectionPosition  = controller.mImpl->mEventData->mPrimaryCursorPosition;
    controller.mImpl->mEventData->mRightSelectionPosition = controller.mImpl->mEventData->mPrimaryCursorPosition;

    if(controller.mImpl->mSelectableControlInterface != nullptr && controller.mImpl->mEventData->mState == EventData::SELECTING)
    {
      controller.mImpl->mSelectableControlInterface->SelectionChanged(oldStart, oldEnd, controller.mImpl->mEventData->mLeftSelectionPosition, controller.mImpl->mEventData->mRightSelectionPosition);
    }
  }

  // DISCARD temporary text
  events.Clear();
}

void Controller::EventHandler::TextReplacedEvent(Controller& controller)
{
  // The natural size needs to be re-calculated.
  controller.mImpl->mRecalculateNaturalSize = true;

  // The text direction needs to be updated.
  controller.mImpl->mUpdateTextDirection = true;

  // Apply modifications to the model
  controller.mImpl->mOperationsPending = ALL_OPERATIONS;
}

void Controller::EventHandler::TextInsertedEvent(Controller& controller)
{
  DALI_ASSERT_DEBUG(NULL != controller.mImpl->mEventData && "Unexpected TextInsertedEvent");

  if(NULL == controller.mImpl->mEventData)
  {
    return;
  }

  controller.mImpl->mEventData->mCheckScrollAmount = true;

  // The natural size needs to be re-calculated.
  controller.mImpl->mRecalculateNaturalSize = true;

  // The text direction needs to be updated.
  controller.mImpl->mUpdateTextDirection = true;

  // Apply modifications to the model; TODO - Optimize this
  controller.mImpl->mOperationsPending = ALL_OPERATIONS;
}

void Controller::EventHandler::TextDeletedEvent(Controller& controller)
{
  DALI_ASSERT_DEBUG(NULL != controller.mImpl->mEventData && "Unexpected TextDeletedEvent");

  if(NULL == controller.mImpl->mEventData)
  {
    return;
  }

  if(!controller.IsEditable()) return;

  controller.mImpl->mEventData->mCheckScrollAmount = true;

  // The natural size needs to be re-calculated.
  controller.mImpl->mRecalculateNaturalSize = true;

  // The text direction needs to be updated.
  controller.mImpl->mUpdateTextDirection = true;

  // Apply modifications to the model; TODO - Optimize this
  controller.mImpl->mOperationsPending = ALL_OPERATIONS;
}

bool Controller::EventHandler::DeleteEvent(Controller& controller, int keyCode)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::KeyEvent %p KeyCode : %d \n", &controller, keyCode);

  bool removed = false;

  if(NULL == controller.mImpl->mEventData)
  {
    return removed;
  }

  if(!controller.IsEditable()) return false;

  // InputMethodContext is no longer handling key-events
  controller.mImpl->ClearPreEditFlag();

  if(EventData::SELECTING == controller.mImpl->mEventData->mState)
  {
    removed = TextUpdater::RemoveSelectedText(controller);
  }
  else if((controller.mImpl->mEventData->mPrimaryCursorPosition > 0) && (keyCode == Dali::DALI_KEY_BACKSPACE))
  {
    // Remove the character before the current cursor position
    removed = TextUpdater::RemoveText(controller, -1, 1, UPDATE_INPUT_STYLE);
  }
  else if((controller.mImpl->mEventData->mPrimaryCursorPosition < controller.mImpl->mModel->mLogicalModel->mText.Count()) &&
          (keyCode == Dali::DevelKey::DALI_KEY_DELETE))
  {
    // Remove the character after the current cursor position
    removed = TextUpdater::RemoveText(controller, 0, 1, UPDATE_INPUT_STYLE);
  }

  if(removed)
  {
    if((0u != controller.mImpl->mModel->mLogicalModel->mText.Count()) ||
       !controller.mImpl->IsPlaceholderAvailable())
    {
      controller.mImpl->QueueModifyEvent(ModifyEvent::TEXT_DELETED);
    }
    else
    {
      PlaceholderHandler::ShowPlaceholderText(*controller.mImpl);
    }
    controller.mImpl->mEventData->mUpdateCursorPosition = true;
    controller.mImpl->mEventData->mScrollAfterDelete    = true;
  }

  return removed;
}

InputMethodContext::CallbackData Controller::EventHandler::OnInputMethodContextEvent(Controller& controller, InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent)
{
  // Whether the text needs to be relaid-out.
  bool requestRelayout = false;

  // Whether to retrieve the text and cursor position to be sent to the InputMethodContext.
  bool retrieveText   = false;
  bool retrieveCursor = false;

  switch(inputMethodContextEvent.eventName)
  {
    case InputMethodContext::COMMIT:
    {
      TextUpdater::InsertText(controller, inputMethodContextEvent.predictiveString, Text::Controller::COMMIT);
      requestRelayout = true;
      retrieveCursor  = true;
      break;
    }
    case InputMethodContext::PRE_EDIT:
    {
      TextUpdater::InsertText(controller, inputMethodContextEvent.predictiveString, Text::Controller::PRE_EDIT);
      requestRelayout = true;
      retrieveCursor  = true;
      break;
    }
    case InputMethodContext::DELETE_SURROUNDING:
    {
      const bool textDeleted = TextUpdater::RemoveText(controller,
                                                       inputMethodContextEvent.cursorOffset,
                                                       inputMethodContextEvent.numberOfChars,
                                                       DONT_UPDATE_INPUT_STYLE);

      if(textDeleted)
      {
        if((0u != controller.mImpl->mModel->mLogicalModel->mText.Count()) ||
           !controller.mImpl->IsPlaceholderAvailable())
        {
          controller.mImpl->QueueModifyEvent(ModifyEvent::TEXT_DELETED);
        }
        else
        {
          PlaceholderHandler::ShowPlaceholderText(*controller.mImpl);
        }
        controller.mImpl->mEventData->mUpdateCursorPosition = true;
        controller.mImpl->mEventData->mScrollAfterDelete    = true;

        requestRelayout = true;
      }
      break;
    }
    case InputMethodContext::GET_SURROUNDING:
    {
      retrieveText   = true;
      retrieveCursor = true;
      break;
    }
    case InputMethodContext::PRIVATE_COMMAND:
    {
      // PRIVATECOMMAND event is just for getting the private command message
      retrieveText   = true;
      retrieveCursor = true;
      break;
    }
    case InputMethodContext::SELECTION_SET:
    {
      uint32_t start = static_cast<uint32_t>(inputMethodContextEvent.startIndex);
      uint32_t end   = static_cast<uint32_t>(inputMethodContextEvent.endIndex);
      if(start == end)
      {
        controller.SetPrimaryCursorPosition(start, true);
      }
      else
      {
        controller.SelectText(start, end);
      }

      break;
    }
    case InputMethodContext::VOID:
    {
      // do nothing
      break;
    }
  } // end switch

  if(requestRelayout)
  {
    controller.mImpl->mOperationsPending = ALL_OPERATIONS;
    controller.mImpl->RequestRelayout();
  }

  std::string    text;
  CharacterIndex cursorPosition      = 0u;
  Length         numberOfWhiteSpaces = 0u;

  if(retrieveCursor)
  {
    numberOfWhiteSpaces = controller.mImpl->GetNumberOfWhiteSpaces(0u);

    cursorPosition = controller.mImpl->GetLogicalCursorPosition();

    if(cursorPosition < numberOfWhiteSpaces)
    {
      cursorPosition = 0u;
    }
    else
    {
      cursorPosition -= numberOfWhiteSpaces;
    }
  }

  if(retrieveText)
  {
    if(!controller.mImpl->IsShowingPlaceholderText())
    {
      // Retrieves the normal text string.
      controller.mImpl->GetText(numberOfWhiteSpaces, text);
    }
    else
    {
      // When the current text is Placeholder Text, the surrounding text should be empty string.
      // It means DALi should send empty string ("") to IME.
      text = "";
    }
  }

  InputMethodContext::CallbackData callbackData((retrieveText || retrieveCursor), cursorPosition, text, false);

  if(requestRelayout &&
     (NULL != controller.mImpl->mEditableControlInterface))
  {
    // Do this last since it provides callbacks into application code
    controller.mImpl->mEditableControlInterface->TextChanged(false);
  }

  return callbackData;
}

void Controller::EventHandler::PasteClipboardItemEvent(Controller& controller)
{
  // Retrieve the clipboard contents first
  ClipboardEventNotifier notifier(ClipboardEventNotifier::Get());
  std::string            stringToPaste(notifier.GetContent());

  // Commit the current pre-edit text; the contents of the clipboard should be appended
  controller.mImpl->ResetInputMethodContext();

  // Temporary disable hiding clipboard
  controller.mImpl->SetClipboardHideEnable(false);

  // Paste
  TextUpdater::PasteText(controller, stringToPaste);

  controller.mImpl->SetClipboardHideEnable(true);
}

void Controller::EventHandler::DecorationEvent(Controller& controller, HandleType handleType, HandleState state, float x, float y)
{
  DALI_ASSERT_DEBUG(controller.mImpl->mEventData && "Unexpected DecorationEvent");

  if(NULL != controller.mImpl->mEventData)
  {
    switch(handleType)
    {
      case GRAB_HANDLE:
      {
        Event event(Event::GRAB_HANDLE_EVENT);
        event.p1.mUint  = state;
        event.p2.mFloat = x;
        event.p3.mFloat = y;

        controller.mImpl->mEventData->mEventQueue.push_back(event);
        break;
      }
      case LEFT_SELECTION_HANDLE:
      {
        Event event(Event::LEFT_SELECTION_HANDLE_EVENT);
        event.p1.mUint  = state;
        event.p2.mFloat = x;
        event.p3.mFloat = y;

        controller.mImpl->mEventData->mEventQueue.push_back(event);
        break;
      }
      case RIGHT_SELECTION_HANDLE:
      {
        Event event(Event::RIGHT_SELECTION_HANDLE_EVENT);
        event.p1.mUint  = state;
        event.p2.mFloat = x;
        event.p3.mFloat = y;

        controller.mImpl->mEventData->mEventQueue.push_back(event);
        break;
      }
      case LEFT_SELECTION_HANDLE_MARKER:
      case RIGHT_SELECTION_HANDLE_MARKER:
      {
        // Markers do not move the handles.
        break;
      }
      case HANDLE_TYPE_COUNT:
      {
        DALI_ASSERT_DEBUG(!"Controller::HandleEvent. Unexpected handle type");
      }
    }

    controller.mImpl->RequestRelayout();
  }
}

void Controller::EventHandler::TextPopupButtonTouched(Controller& controller, Dali::Toolkit::TextSelectionPopup::Buttons button)
{
  if(NULL == controller.mImpl->mEventData)
  {
    return;
  }

  switch(button)
  {
    case Toolkit::TextSelectionPopup::CUT:
    {
      controller.CutText();
      break;
    }
    case Toolkit::TextSelectionPopup::COPY:
    {
      controller.CopyText();
      break;
    }
    case Toolkit::TextSelectionPopup::PASTE:
    {
      controller.PasteText();
      break;
    }
    case Toolkit::TextSelectionPopup::SELECT:
    {
      const Vector2& currentCursorPosition = controller.mImpl->mEventData->mDecorator->GetPosition(PRIMARY_CURSOR);

      if(controller.mImpl->mEventData->mSelectionEnabled)
      {
        // Creates a SELECT event.
        SelectEvent(controller, currentCursorPosition.x, currentCursorPosition.y, SelectionType::INTERACTIVE);
      }
      break;
    }
    case Toolkit::TextSelectionPopup::SELECT_ALL:
    {
      // Creates a SELECT_ALL event
      SelectEvent(controller, 0.f, 0.f, SelectionType::ALL);
      break;
    }
    case Toolkit::TextSelectionPopup::CLIPBOARD:
    {
      controller.mImpl->ShowClipboard();
      break;
    }
    case Toolkit::TextSelectionPopup::NONE:
    {
      // Nothing to do.
      break;
    }
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
