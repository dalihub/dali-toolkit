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
#include <dali-toolkit/internal/text/text-controller-impl-event-handler.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/key.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/cursor-helper-functions.h>
#include <dali-toolkit/internal/text/text-editable-control-interface.h>

using namespace Dali;

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
bool ControllerImplEventHandler::ProcessInputEvents(Controller::Impl& impl)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::ProcessInputEvents\n");

  EventData*& eventData = impl.mEventData;
  if(NULL == eventData)
  {
    // Nothing to do if there is no text input.
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::ProcessInputEvents no event data\n");
    return false;
  }

  unsigned int oldPos = eventData->mPrimaryCursorPosition;

  if(eventData->mDecorator)
  {
    for(std::vector<Event>::iterator iter = eventData->mEventQueue.begin();
        iter != eventData->mEventQueue.end();
        ++iter)
    {
      switch(iter->type)
      {
        case Event::CURSOR_KEY_EVENT:
        {
          OnCursorKeyEvent(impl, *iter);
          break;
        }
        case Event::TAP_EVENT:
        {
          OnTapEvent(impl, *iter);
          break;
        }
        case Event::LONG_PRESS_EVENT:
        {
          OnLongPressEvent(impl, *iter);
          break;
        }
        case Event::PAN_EVENT:
        {
          OnPanEvent(impl, *iter);
          break;
        }
        case Event::GRAB_HANDLE_EVENT:
        case Event::LEFT_SELECTION_HANDLE_EVENT:
        case Event::RIGHT_SELECTION_HANDLE_EVENT: // Fall through
        {
          OnHandleEvent(impl, *iter);
          break;
        }
        case Event::SELECT:
        {
          OnSelectEvent(impl, *iter);
          break;
        }
        case Event::SELECT_ALL:
        {
          OnSelectAllEvent(impl);
          break;
        }
        case Event::SELECT_NONE:
        {
          OnSelectNoneEvent(impl);
          break;
        }
        case Event::SELECT_RANGE:
        {
          OnSelectRangeEvent(impl, *iter);
          break;
        }
      }
    }
  }

  if(eventData->mUpdateCursorPosition ||
     eventData->mUpdateHighlightBox)
  {
    impl.NotifyInputMethodContext();
  }

  // The cursor must also be repositioned after inserts into the model
  if(eventData->mUpdateCursorPosition)
  {
    // Updates the cursor position and scrolls the text to make it visible.
    CursorInfo cursorInfo;

    // Calculate the cursor position from the new cursor index.
    impl.GetCursorPosition(eventData->mPrimaryCursorPosition, cursorInfo);

    //only emit the event if the cursor is moved in current function.
    if(nullptr != impl.mEditableControlInterface && eventData->mEventQueue.size() > 0)
    {
      impl.mEditableControlInterface->CursorPositionChanged(oldPos, eventData->mPrimaryCursorPosition);
    }

    if(eventData->mUpdateCursorHookPosition)
    {
      // Update the cursor hook position. Used to move the cursor with the keys 'up' and 'down'.
      eventData->mCursorHookPositionX      = cursorInfo.primaryPosition.x;
      eventData->mUpdateCursorHookPosition = false;
    }

    // Scroll first the text after delete ...
    if(eventData->mScrollAfterDelete)
    {
      impl.ScrollTextToMatchCursor(cursorInfo);
    }

    // ... then, text can be scrolled to make the cursor visible.
    if(eventData->mScrollAfterUpdatePosition)
    {
      const Vector2 currentCursorPosition(cursorInfo.primaryPosition.x, cursorInfo.lineOffset);
      impl.ScrollToMakePositionVisible(currentCursorPosition, cursorInfo.lineHeight);
    }
    eventData->mScrollAfterUpdatePosition = false;
    eventData->mScrollAfterDelete         = false;

    impl.UpdateCursorPosition(cursorInfo);

    eventData->mDecoratorUpdated         = true;
    eventData->mUpdateCursorPosition     = false;
    eventData->mUpdateGrabHandlePosition = false;
  }

  if(eventData->mUpdateHighlightBox ||
     eventData->mUpdateLeftSelectionPosition ||
     eventData->mUpdateRightSelectionPosition)
  {
    CursorInfo leftHandleInfo;
    CursorInfo rightHandleInfo;

    if(eventData->mUpdateHighlightBox)
    {
      impl.GetCursorPosition(eventData->mLeftSelectionPosition, leftHandleInfo);

      impl.GetCursorPosition(eventData->mRightSelectionPosition, rightHandleInfo);

      if(eventData->mScrollAfterUpdatePosition && (eventData->mIsLeftHandleSelected ? eventData->mUpdateLeftSelectionPosition : eventData->mUpdateRightSelectionPosition))
      {
        if(eventData->mIsLeftHandleSelected && eventData->mIsRightHandleSelected)
        {
          CursorInfo& infoLeft = leftHandleInfo;

          const Vector2 currentCursorPositionLeft(infoLeft.primaryPosition.x, infoLeft.lineOffset);
          impl.ScrollToMakePositionVisible(currentCursorPositionLeft, infoLeft.lineHeight);

          CursorInfo& infoRight = rightHandleInfo;

          const Vector2 currentCursorPositionRight(infoRight.primaryPosition.x, infoRight.lineOffset);
          impl.ScrollToMakePositionVisible(currentCursorPositionRight, infoRight.lineHeight);
        }
        else
        {
          CursorInfo& info = eventData->mIsLeftHandleSelected ? leftHandleInfo : rightHandleInfo;

          const Vector2 currentCursorPosition(info.primaryPosition.x, info.lineOffset);
          impl.ScrollToMakePositionVisible(currentCursorPosition, info.lineHeight);
        }
      }
    }

    if(eventData->mUpdateLeftSelectionPosition)
    {
      impl.UpdateSelectionHandle(LEFT_SELECTION_HANDLE, leftHandleInfo);

      impl.SetPopupButtons();
      eventData->mDecoratorUpdated            = true;
      eventData->mUpdateLeftSelectionPosition = false;
    }

    if(eventData->mUpdateRightSelectionPosition)
    {
      impl.UpdateSelectionHandle(RIGHT_SELECTION_HANDLE, rightHandleInfo);

      impl.SetPopupButtons();
      eventData->mDecoratorUpdated             = true;
      eventData->mUpdateRightSelectionPosition = false;
    }

    if(eventData->mUpdateHighlightBox)
    {
      impl.RepositionSelectionHandles();

      eventData->mUpdateLeftSelectionPosition  = false;
      eventData->mUpdateRightSelectionPosition = false;
      eventData->mUpdateHighlightBox           = false;
      eventData->mIsLeftHandleSelected         = false;
      eventData->mIsRightHandleSelected        = false;
    }

    eventData->mScrollAfterUpdatePosition = false;
  }

  if(eventData->mUpdateInputStyle)
  {
    // Keep a copy of the current input style.
    InputStyle currentInputStyle;
    currentInputStyle.Copy(eventData->mInputStyle);

    // Set the default style first.
    impl.RetrieveDefaultInputStyle(eventData->mInputStyle);

    // Get the character index from the cursor index.
    const CharacterIndex styleIndex = (eventData->mPrimaryCursorPosition > 0u) ? eventData->mPrimaryCursorPosition - 1u : 0u;

    // Retrieve the style from the style runs stored in the logical model.
    impl.mModel->mLogicalModel->RetrieveStyle(styleIndex, eventData->mInputStyle);

    // Compare if the input style has changed.
    const bool hasInputStyleChanged = !currentInputStyle.Equal(eventData->mInputStyle);

    if(hasInputStyleChanged)
    {
      const InputStyle::Mask styleChangedMask = currentInputStyle.GetInputStyleChangeMask(eventData->mInputStyle);
      // Queue the input style changed signal.
      eventData->mInputStyleChangedQueue.PushBack(styleChangedMask);
    }

    eventData->mUpdateInputStyle = false;
  }

  eventData->mEventQueue.clear();

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::ProcessInputEvents\n");

  const bool decoratorUpdated  = eventData->mDecoratorUpdated;
  eventData->mDecoratorUpdated = false;

  return decoratorUpdated;
}

void ControllerImplEventHandler::OnCursorKeyEvent(Controller::Impl& impl, const Event& event)
{
  if(NULL == impl.mEventData || !impl.IsShowingRealText())
  {
    // Nothing to do if there is no text input.
    return;
  }

  int              keyCode         = event.p1.mInt;
  bool             isShiftModifier = event.p2.mBool;
  EventData&       eventData       = *impl.mEventData;
  ModelPtr&        model           = impl.mModel;
  LogicalModelPtr& logicalModel    = model->mLogicalModel;
  VisualModelPtr&  visualModel     = model->mVisualModel;
  uint32_t         oldSelStart     = eventData.mLeftSelectionPosition;
  uint32_t         oldSelEnd       = eventData.mRightSelectionPosition;

  CharacterIndex& primaryCursorPosition         = eventData.mPrimaryCursorPosition;
  CharacterIndex  previousPrimaryCursorPosition = primaryCursorPosition;

  if(Dali::DALI_KEY_CURSOR_LEFT == keyCode)
  {
    if(primaryCursorPosition > 0u)
    {
      if(!isShiftModifier && eventData.mDecorator->IsHighlightVisible())
      {
        primaryCursorPosition = std::min(eventData.mLeftSelectionPosition, eventData.mRightSelectionPosition);
      }
      else
      {
        primaryCursorPosition = impl.CalculateNewCursorIndex(primaryCursorPosition - 1u);
      }
    }
  }
  else if(Dali::DALI_KEY_CURSOR_RIGHT == keyCode)
  {
    if(logicalModel->mText.Count() > primaryCursorPosition)
    {
      if(!isShiftModifier && eventData.mDecorator->IsHighlightVisible())
      {
        primaryCursorPosition = std::max(eventData.mLeftSelectionPosition, eventData.mRightSelectionPosition);
      }
      else
      {
        primaryCursorPosition = impl.CalculateNewCursorIndex(primaryCursorPosition);
      }
    }
  }
  else if(Dali::DALI_KEY_CURSOR_UP == keyCode && !isShiftModifier)
  {
    // Ignore Shift-Up for text selection for now.

    // Get first the line index of the current cursor position index.
    CharacterIndex characterIndex = 0u;

    if(primaryCursorPosition > 0u)
    {
      characterIndex = primaryCursorPosition - 1u;
    }

    const LineIndex lineIndex         = visualModel->GetLineOfCharacter(characterIndex);
    const LineIndex previousLineIndex = (lineIndex > 0 ? lineIndex - 1u : lineIndex);
    const LineIndex lastLineIndex     = (visualModel->mLines.Size() > 0 ? visualModel->mLines.Size() - 1u : 0);
    const bool      isLastLine        = (previousLineIndex == lastLineIndex);

    // Retrieve the cursor position info.
    CursorInfo cursorInfo;
    impl.GetCursorPosition(primaryCursorPosition,
                           cursorInfo);

    // Get the line above.
    const LineRun& line = *(visualModel->mLines.Begin() + previousLineIndex);

    // Get the next hit 'y' point.
    const float hitPointY = cursorInfo.lineOffset - 0.5f * GetLineHeight(line, isLastLine);

    // Use the cursor hook position 'x' and the next hit 'y' position to calculate the new cursor index.
    bool matchedCharacter = false;
    primaryCursorPosition = Text::GetClosestCursorIndex(visualModel,
                                                        logicalModel,
                                                        impl.mMetrics,
                                                        eventData.mCursorHookPositionX,
                                                        hitPointY,
                                                        CharacterHitTest::TAP,
                                                        matchedCharacter);
  }
  else if(Dali::DALI_KEY_CURSOR_DOWN == keyCode && !isShiftModifier)
  {
    // Ignore Shift-Down for text selection for now.

    // Get first the line index of the current cursor position index.
    CharacterIndex characterIndex = 0u;

    if(primaryCursorPosition > 0u)
    {
      characterIndex = primaryCursorPosition - 1u;
    }

    const LineIndex lineIndex = visualModel->GetLineOfCharacter(characterIndex);

    if(lineIndex + 1u < visualModel->mLines.Count())
    {
      // Retrieve the cursor position info.
      CursorInfo cursorInfo;
      impl.GetCursorPosition(primaryCursorPosition, cursorInfo);

      // Get the line below.
      const LineRun& line = *(visualModel->mLines.Begin() + lineIndex + 1u);

      // Get last line index
      const LineIndex lastLineIndex = (visualModel->mLines.Size() > 0 ? visualModel->mLines.Size() - 1u : 0);
      const bool      isLastLine    = (lineIndex + 1u == lastLineIndex);

      // Get the next hit 'y' point.
      const float hitPointY = cursorInfo.lineOffset + cursorInfo.lineHeight + 0.5f * GetLineHeight(line, isLastLine);

      // Use the cursor hook position 'x' and the next hit 'y' position to calculate the new cursor index.
      bool matchedCharacter = false;
      primaryCursorPosition = Text::GetClosestCursorIndex(visualModel,
                                                          logicalModel,
                                                          impl.mMetrics,
                                                          eventData.mCursorHookPositionX,
                                                          hitPointY,
                                                          CharacterHitTest::TAP,
                                                          matchedCharacter);
    }
  }

  if(!isShiftModifier && eventData.mState != EventData::SELECTING)
  {
    // Update selection position after moving the cursor
    eventData.mLeftSelectionPosition  = primaryCursorPosition;
    eventData.mRightSelectionPosition = primaryCursorPosition;

    if(impl.mSelectableControlInterface != nullptr && eventData.mDecorator->IsHighlightVisible())
    {
      impl.mSelectableControlInterface->SelectionChanged(oldSelStart, oldSelEnd, eventData.mLeftSelectionPosition, eventData.mRightSelectionPosition);
    }
  }

  if(isShiftModifier && impl.IsShowingRealText() && eventData.mShiftSelectionFlag)
  {
    // Handle text selection
    bool selecting = false;

    if(Dali::DALI_KEY_CURSOR_LEFT == keyCode || Dali::DALI_KEY_CURSOR_RIGHT == keyCode)
    {
      // Shift-Left/Right to select the text
      int cursorPositionDelta = primaryCursorPosition - previousPrimaryCursorPosition;
      if(cursorPositionDelta > 0 || eventData.mRightSelectionPosition > 0u) // Check the boundary
      {
        eventData.mRightSelectionPosition += cursorPositionDelta;
        eventData.mPrimaryCursorPosition = eventData.mRightSelectionPosition;

        if(impl.mSelectableControlInterface != nullptr)
        {
          impl.mSelectableControlInterface->SelectionChanged(oldSelStart, oldSelEnd, eventData.mLeftSelectionPosition, eventData.mRightSelectionPosition);
        }
      }

      if(impl.mSelectableControlInterface != nullptr && eventData.mLeftSelectionPosition == eventData.mRightSelectionPosition)
      {
        // If left selection position and right selection position are the same, the selection is canceled.
        selecting = false;
      }
      else
      {
        selecting = true;
      }
    }
    else if(eventData.mLeftSelectionPosition != eventData.mRightSelectionPosition)
    {
      // Show no grab handles and text highlight if Shift-Up/Down pressed but no selected text
      selecting = true;
    }

    if(selecting)
    {
      // Notify the cursor position to the InputMethodContext.
      if(eventData.mInputMethodContext)
      {
        eventData.mInputMethodContext.SetCursorPosition(primaryCursorPosition);
        eventData.mInputMethodContext.NotifyCursorPosition();
      }

      impl.ChangeState(EventData::SELECTING);

      eventData.mUpdateLeftSelectionPosition  = true;
      eventData.mUpdateRightSelectionPosition = true;
      eventData.mUpdateGrabHandlePosition     = true;
      eventData.mUpdateHighlightBox           = true;

      // Hide the text selection popup if select the text using keyboard instead of moving grab handles
      if(eventData.mGrabHandlePopupEnabled)
      {
        eventData.mDecorator->SetPopupActive(false);
      }
    }
    else
    {
      // If no selection, set a normal cursor.
      impl.ChangeState(EventData::EDITING);
      eventData.mUpdateCursorPosition = true;
    }
  }
  else
  {
    // Handle normal cursor move
    impl.ChangeState(EventData::EDITING);
    eventData.mUpdateCursorPosition = true;
  }

  eventData.mUpdateInputStyle          = true;
  eventData.mScrollAfterUpdatePosition = true;
}

void ControllerImplEventHandler::OnTapEvent(Controller::Impl& impl, const Event& event)
{
  if(impl.mEventData)
  {
    const unsigned int tapCount     = event.p1.mUint;
    EventData&         eventData    = *impl.mEventData;
    ModelPtr&          model        = impl.mModel;
    LogicalModelPtr&   logicalModel = model->mLogicalModel;
    VisualModelPtr&    visualModel  = model->mVisualModel;

    if(1u == tapCount)
    {
      if(impl.IsShowingRealText())
      {
        // Convert from control's coords to text's coords.
        const float xPosition   = event.p2.mFloat - model->mScrollPosition.x;
        const float yPosition   = event.p3.mFloat - model->mScrollPosition.y;
        uint32_t    oldSelStart = eventData.mLeftSelectionPosition;
        uint32_t    oldSelEnd   = eventData.mRightSelectionPosition;

        // Keep the tap 'x' position. Used to move the cursor.
        eventData.mCursorHookPositionX = xPosition;

        // Whether to touch point hits on a glyph.
        bool matchedCharacter            = false;
        eventData.mPrimaryCursorPosition = Text::GetClosestCursorIndex(visualModel,
                                                                       logicalModel,
                                                                       impl.mMetrics,
                                                                       xPosition,
                                                                       yPosition,
                                                                       CharacterHitTest::TAP,
                                                                       matchedCharacter);

        if(impl.mSelectableControlInterface != nullptr && eventData.mDecorator->IsHighlightVisible())
        {
          impl.mSelectableControlInterface->SelectionChanged(oldSelStart, oldSelEnd, eventData.mPrimaryCursorPosition, eventData.mPrimaryCursorPosition);
        }

        // When the cursor position is changing, delay cursor blinking
        eventData.mDecorator->DelayCursorBlink();
      }
      else
      {
        eventData.mPrimaryCursorPosition = 0u;
      }

      // Update selection position after tapping
      eventData.mLeftSelectionPosition  = eventData.mPrimaryCursorPosition;
      eventData.mRightSelectionPosition = eventData.mPrimaryCursorPosition;

      eventData.mUpdateCursorPosition      = true;
      eventData.mUpdateGrabHandlePosition  = true;
      eventData.mScrollAfterUpdatePosition = true;
      eventData.mUpdateInputStyle          = true;

      // Notify the cursor position to the InputMethodContext.
      if(eventData.mInputMethodContext)
      {
        eventData.mInputMethodContext.SetCursorPosition(eventData.mPrimaryCursorPosition);
        eventData.mInputMethodContext.NotifyCursorPosition();
      }
    }
    else if(2u == tapCount)
    {
      if(eventData.mSelectionEnabled)
      {
        // Convert from control's coords to text's coords.
        const float xPosition = event.p2.mFloat - model->mScrollPosition.x;
        const float yPosition = event.p3.mFloat - model->mScrollPosition.y;

        // Calculates the logical position from the x,y coords.
        impl.RepositionSelectionHandles(xPosition, yPosition, eventData.mDoubleTapAction);
      }
    }
  }
}

void ControllerImplEventHandler::OnPanEvent(Controller::Impl& impl, const Event& event)
{
  if(impl.mEventData)
  {
    EventData&    eventData = *impl.mEventData;
    DecoratorPtr& decorator = eventData.mDecorator;

    const bool isHorizontalScrollEnabled = decorator->IsHorizontalScrollEnabled();
    const bool isVerticalScrollEnabled   = decorator->IsVerticalScrollEnabled();

    if(!isHorizontalScrollEnabled && !isVerticalScrollEnabled)
    {
      // Nothing to do if scrolling is not enabled.
      return;
    }

    const GestureState state = static_cast<GestureState>(event.p1.mInt);
    switch(state)
    {
      case GestureState::STARTED:
      {
        // Will remove the cursor, handles or text's popup, ...
        impl.ChangeState(EventData::TEXT_PANNING);
        break;
      }
      case GestureState::CONTINUING:
      {
        ModelPtr& model = impl.mModel;

        const Vector2& layoutSize     = model->mVisualModel->GetLayoutSize();
        Vector2&       scrollPosition = model->mScrollPosition;
        const Vector2  currentScroll  = scrollPosition;

        if(isHorizontalScrollEnabled)
        {
          const float displacementX = event.p2.mFloat;
          scrollPosition.x += displacementX;

          impl.ClampHorizontalScroll(layoutSize);
        }

        if(isVerticalScrollEnabled)
        {
          const float displacementY = event.p3.mFloat;
          scrollPosition.y += displacementY;

          impl.ClampVerticalScroll(layoutSize);
        }

        decorator->UpdatePositions(scrollPosition - currentScroll);
        break;
      }
      case GestureState::FINISHED:
      case GestureState::CANCELLED: // FALLTHROUGH
      {
        // Will go back to the previous state to show the cursor, handles, the text's popup, ...
        impl.ChangeState(eventData.mPreviousState);
        break;
      }
      default:
        break;
    }
  }
}

void ControllerImplEventHandler::OnLongPressEvent(Controller::Impl& impl, const Event& event)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::OnLongPressEvent\n");

  if(impl.mEventData)
  {
    EventData& eventData = *impl.mEventData;

    if(!impl.IsShowingRealText() && (EventData::EDITING == eventData.mState))
    {
      impl.ChangeState(EventData::EDITING_WITH_POPUP);
      eventData.mDecoratorUpdated = true;
      eventData.mUpdateInputStyle = true;
    }
    else
    {
      if(eventData.mSelectionEnabled)
      {
        ModelPtr& model = impl.mModel;

        // Convert from control's coords to text's coords.
        const float xPosition = event.p2.mFloat - model->mScrollPosition.x;
        const float yPosition = event.p3.mFloat - model->mScrollPosition.y;

        // Calculates the logical position from the x,y coords.
        impl.RepositionSelectionHandles(xPosition, yPosition, eventData.mLongPressAction);
      }
    }
  }
}

void ControllerImplEventHandler::OnHandleEvent(Controller::Impl& impl, const Event& event)
{
  if(impl.mEventData)
  {
    const unsigned int state                    = event.p1.mUint;
    const bool         handleStopScrolling      = (HANDLE_STOP_SCROLLING == state);
    const bool         isSmoothHandlePanEnabled = impl.mEventData->mDecorator->IsSmoothHandlePanEnabled();

    if(HANDLE_PRESSED == state)
    {
      OnHandlePressed(impl, event, isSmoothHandlePanEnabled);
    } // end ( HANDLE_PRESSED == state )
    else if((HANDLE_RELEASED == state) ||
            handleStopScrolling)
    {
      OnHandleReleased(impl, event, isSmoothHandlePanEnabled, handleStopScrolling);
    } // end ( ( HANDLE_RELEASED == state ) || ( HANDLE_STOP_SCROLLING == state ) )
    else if(HANDLE_SCROLLING == state)
    {
      OnHandleScrolling(impl, event, isSmoothHandlePanEnabled);
    } // end ( HANDLE_SCROLLING == state )
  }
}

void ControllerImplEventHandler::OnSelectEvent(Controller::Impl& impl, const Event& event)
{
  if(impl.mEventData && impl.mEventData->mSelectionEnabled)
  {
    ModelPtr&      model          = impl.mModel;
    const Vector2& scrollPosition = model->mScrollPosition;

    // Convert from control's coords to text's coords.
    const float xPosition = event.p2.mFloat - scrollPosition.x;
    const float yPosition = event.p3.mFloat - scrollPosition.y;

    // Calculates the logical position from the x,y coords.
    impl.RepositionSelectionHandles(xPosition, yPosition, Controller::NoTextTap::HIGHLIGHT);
  }
}

void ControllerImplEventHandler::OnSelectAllEvent(Controller::Impl& impl)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "OnSelectAllEvent mEventData->mSelectionEnabled%s \n", impl.mEventData->mSelectionEnabled ? "true" : "false");

  if(impl.mEventData)
  {
    EventData& eventData = *impl.mEventData;
    if(eventData.mSelectionEnabled && eventData.mState != EventData::INACTIVE)
    {
      ModelPtr&      model          = impl.mModel;
      const Vector2& scrollPosition = model->mScrollPosition;

      // Calculates the logical position from the start.
      impl.RepositionSelectionHandles(0.f - scrollPosition.x,
                                      0.f - scrollPosition.y,
                                      Controller::NoTextTap::HIGHLIGHT);

      uint32_t oldStart = eventData.mLeftSelectionPosition;
      uint32_t oldEnd   = eventData.mRightSelectionPosition;

      eventData.mLeftSelectionPosition  = 0u;
      eventData.mRightSelectionPosition = model->mLogicalModel->mText.Count();
      eventData.mPrimaryCursorPosition  = eventData.mRightSelectionPosition;

      if(impl.mSelectableControlInterface != nullptr)
      {
        impl.mSelectableControlInterface->SelectionChanged(oldStart, oldEnd, eventData.mLeftSelectionPosition, eventData.mRightSelectionPosition);
      }
    }
  }
}

void ControllerImplEventHandler::OnSelectNoneEvent(Controller::Impl& impl)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "OnSelectNoneEvent mEventData->mSelectionEnabled%s \n", impl.mEventData->mSelectionEnabled ? "true" : "false");

  if(impl.mEventData)
  {
    EventData& eventData = *impl.mEventData;
    if(eventData.mSelectionEnabled && eventData.mState == EventData::SELECTING)
    {
      uint32_t oldStart = eventData.mLeftSelectionPosition;
      uint32_t oldEnd   = eventData.mRightSelectionPosition;

      eventData.mLeftSelectionPosition = eventData.mRightSelectionPosition = eventData.mPrimaryCursorPosition;
      impl.ChangeState(EventData::EDITING);
      eventData.mUpdateCursorPosition      = true;
      eventData.mUpdateInputStyle          = true;
      eventData.mScrollAfterUpdatePosition = true;

      if(impl.mSelectableControlInterface != nullptr)
      {
        impl.mSelectableControlInterface->SelectionChanged(oldStart, oldEnd, eventData.mLeftSelectionPosition, eventData.mRightSelectionPosition);
      }
    }
  }
}

void ControllerImplEventHandler::OnSelectRangeEvent(Controller::Impl& impl, const Event& event)
{
  if(impl.mEventData && impl.mEventData->mSelectionEnabled && impl.mEventData->mState != EventData::INACTIVE)
  {
    ModelPtr&      model          = impl.mModel;
    const Vector2& scrollPosition = model->mScrollPosition;

    // Calculate the selection index.
    const uint32_t length = static_cast<uint32_t>(model->mLogicalModel->mText.Count());
    const uint32_t start  = std::min(event.p2.mUint, length);
    const uint32_t end    = std::min(event.p3.mUint, length);

    if(start != end)
    {
      uint32_t oldStart = impl.mEventData->mLeftSelectionPosition;
      uint32_t oldEnd   = impl.mEventData->mRightSelectionPosition;

      // Calculates the logical position from the x,y coords.
      impl.RepositionSelectionHandles(0.f - scrollPosition.x, 0.f - scrollPosition.y, Controller::NoTextTap::HIGHLIGHT);

      impl.mEventData->mLeftSelectionPosition  = start;
      impl.mEventData->mRightSelectionPosition = end;
      impl.mEventData->mPrimaryCursorPosition  = end;

      if(impl.mSelectableControlInterface != nullptr)
      {
        impl.mSelectableControlInterface->SelectionChanged(oldStart, oldEnd, start, end);
      }
    }
  }
}

void ControllerImplEventHandler::OnHandlePressed(Controller::Impl& impl, const Event& event, const bool isSmoothHandlePanEnabled)
{
  ModelPtr&      model          = impl.mModel;
  const Vector2& scrollPosition = model->mScrollPosition;

  // Convert from decorator's coords to text's coords.
  const float xPosition = event.p2.mFloat - scrollPosition.x;
  const float yPosition = event.p3.mFloat - scrollPosition.y;

  // Need to calculate the handle's new position.
  bool                 matchedCharacter  = false;
  const CharacterIndex handleNewPosition = Text::GetClosestCursorIndex(model->mVisualModel,
                                                                       model->mLogicalModel,
                                                                       impl.mMetrics,
                                                                       xPosition,
                                                                       yPosition,
                                                                       CharacterHitTest::SCROLL,
                                                                       matchedCharacter);

  EventData& eventData = *impl.mEventData;
  uint32_t   oldStart  = eventData.mLeftSelectionPosition;
  uint32_t   oldEnd    = eventData.mRightSelectionPosition;

  if(Event::GRAB_HANDLE_EVENT == event.type)
  {
    impl.ChangeState(EventData::GRAB_HANDLE_PANNING);

    if(handleNewPosition != eventData.mPrimaryCursorPosition)
    {
      // Updates the cursor position if the handle's new position is different than the current one.
      eventData.mUpdateCursorPosition = true;
      // Does not update the grab handle position if the smooth panning is enabled. (The decorator does it smooth).
      eventData.mUpdateGrabHandlePosition = !isSmoothHandlePanEnabled;
      eventData.mPrimaryCursorPosition    = handleNewPosition;
    }

    // Updates the decorator if the soft handle panning is enabled. It triggers a relayout in the decorator and the new position of the handle is set.
    eventData.mDecoratorUpdated = isSmoothHandlePanEnabled;
  }
  else if(Event::LEFT_SELECTION_HANDLE_EVENT == event.type)
  {
    impl.ChangeState(EventData::SELECTION_HANDLE_PANNING);

    if((handleNewPosition != eventData.mLeftSelectionPosition) &&
       (handleNewPosition != eventData.mRightSelectionPosition))
    {
      // Updates the highlight box if the handle's new position is different than the current one.
      eventData.mUpdateHighlightBox = true;
      // Does not update the selection handle position if the smooth panning is enabled. (The decorator does it smooth).
      eventData.mUpdateLeftSelectionPosition = !isSmoothHandlePanEnabled;
      eventData.mLeftSelectionPosition       = handleNewPosition;
    }

    // Updates the decorator if the soft handle panning is enabled. It triggers a relayout in the decorator and the new position of the handle is set.
    eventData.mDecoratorUpdated = isSmoothHandlePanEnabled;

    // Will define the order to scroll the text to match the handle position.
    eventData.mIsLeftHandleSelected  = true;
    eventData.mIsRightHandleSelected = false;
  }
  else if(Event::RIGHT_SELECTION_HANDLE_EVENT == event.type)
  {
    impl.ChangeState(EventData::SELECTION_HANDLE_PANNING);

    if((handleNewPosition != eventData.mRightSelectionPosition) &&
       (handleNewPosition != eventData.mLeftSelectionPosition))
    {
      // Updates the highlight box if the handle's new position is different than the current one.
      eventData.mUpdateHighlightBox = true;
      // Does not update the selection handle position if the smooth panning is enabled. (The decorator does it smooth).
      eventData.mUpdateRightSelectionPosition = !isSmoothHandlePanEnabled;
      eventData.mRightSelectionPosition       = handleNewPosition;
    }

    // Updates the decorator if the soft handle panning is enabled. It triggers a relayout in the decorator and the new position of the handle is set.
    eventData.mDecoratorUpdated = isSmoothHandlePanEnabled;

    // Will define the order to scroll the text to match the handle position.
    eventData.mIsLeftHandleSelected  = false;
    eventData.mIsRightHandleSelected = true;
  }

  if((impl.mSelectableControlInterface != nullptr) && ((oldStart != eventData.mLeftSelectionPosition) || (oldEnd != eventData.mRightSelectionPosition)))
  {
    impl.mSelectableControlInterface->SelectionChanged(oldStart, oldEnd, eventData.mLeftSelectionPosition, eventData.mRightSelectionPosition);
  }
}

void ControllerImplEventHandler::OnHandleReleased(Controller::Impl& impl, const Event& event, const bool isSmoothHandlePanEnabled, const bool handleStopScrolling)
{
  CharacterIndex handlePosition = 0u;
  if(handleStopScrolling || isSmoothHandlePanEnabled)
  {
    ModelPtr&      model          = impl.mModel;
    const Vector2& scrollPosition = model->mScrollPosition;

    // Convert from decorator's coords to text's coords.
    const float xPosition = event.p2.mFloat - scrollPosition.x;
    const float yPosition = event.p3.mFloat - scrollPosition.y;

    bool matchedCharacter = false;
    handlePosition        = Text::GetClosestCursorIndex(model->mVisualModel,
                                                 model->mLogicalModel,
                                                 impl.mMetrics,
                                                 xPosition,
                                                 yPosition,
                                                 CharacterHitTest::SCROLL,
                                                 matchedCharacter);
  }

  EventData& eventData = *impl.mEventData;
  uint32_t   oldStart  = eventData.mLeftSelectionPosition;
  uint32_t   oldEnd    = eventData.mRightSelectionPosition;

  if(Event::GRAB_HANDLE_EVENT == event.type)
  {
    eventData.mUpdateCursorPosition     = true;
    eventData.mUpdateGrabHandlePosition = true;
    eventData.mUpdateInputStyle         = true;

    if(!impl.IsClipboardEmpty())
    {
      impl.ChangeState(EventData::EDITING_WITH_PASTE_POPUP); // Moving grabhandle will show Paste Popup
    }

    if(handleStopScrolling || isSmoothHandlePanEnabled)
    {
      eventData.mScrollAfterUpdatePosition = true;
      eventData.mPrimaryCursorPosition     = handlePosition;
    }
  }
  else if(Event::LEFT_SELECTION_HANDLE_EVENT == event.type)
  {
    impl.ChangeState(EventData::SELECTING);

    eventData.mUpdateHighlightBox           = true;
    eventData.mUpdateLeftSelectionPosition  = true;
    eventData.mUpdateRightSelectionPosition = true;

    if(handleStopScrolling || isSmoothHandlePanEnabled)
    {
      eventData.mScrollAfterUpdatePosition = true;

      if((handlePosition != eventData.mRightSelectionPosition) &&
         (handlePosition != eventData.mLeftSelectionPosition))
      {
        eventData.mLeftSelectionPosition = handlePosition;
      }
    }
  }
  else if(Event::RIGHT_SELECTION_HANDLE_EVENT == event.type)
  {
    impl.ChangeState(EventData::SELECTING);

    eventData.mUpdateHighlightBox           = true;
    eventData.mUpdateRightSelectionPosition = true;
    eventData.mUpdateLeftSelectionPosition  = true;

    if(handleStopScrolling || isSmoothHandlePanEnabled)
    {
      eventData.mScrollAfterUpdatePosition = true;
      if((handlePosition != eventData.mRightSelectionPosition) &&
         (handlePosition != eventData.mLeftSelectionPosition))
      {
        eventData.mRightSelectionPosition = handlePosition;
      }
    }
  }

  if((impl.mSelectableControlInterface != nullptr) && ((oldStart != eventData.mLeftSelectionPosition) || (oldEnd != eventData.mRightSelectionPosition)))
  {
    impl.mSelectableControlInterface->SelectionChanged(oldStart, oldEnd, eventData.mLeftSelectionPosition, eventData.mRightSelectionPosition);
  }

  eventData.mDecoratorUpdated = true;
}

void ControllerImplEventHandler::OnHandleScrolling(Controller::Impl& impl, const Event& event, const bool isSmoothHandlePanEnabled)
{
  ModelPtr&       model          = impl.mModel;
  Vector2&        scrollPosition = model->mScrollPosition;
  VisualModelPtr& visualModel    = model->mVisualModel;

  const float    xSpeed                = event.p2.mFloat;
  const float    ySpeed                = event.p3.mFloat;
  const Vector2& layoutSize            = visualModel->GetLayoutSize();
  const Vector2  currentScrollPosition = scrollPosition;

  scrollPosition.x += xSpeed;
  scrollPosition.y += ySpeed;

  impl.ClampHorizontalScroll(layoutSize);
  impl.ClampVerticalScroll(layoutSize);

  EventData&    eventData = *impl.mEventData;
  DecoratorPtr& decorator = eventData.mDecorator;

  bool endOfScroll = false;
  if(Vector2::ZERO == (currentScrollPosition - scrollPosition))
  {
    // Notify the decorator there is no more text to scroll.
    // The decorator won't send more scroll events.
    decorator->NotifyEndOfScroll();
    // Still need to set the position of the handle.
    endOfScroll = true;
  }

  // Set the position of the handle.
  const bool scrollRightDirection      = xSpeed > 0.f;
  const bool scrollBottomDirection     = ySpeed > 0.f;
  const bool leftSelectionHandleEvent  = Event::LEFT_SELECTION_HANDLE_EVENT == event.type;
  const bool rightSelectionHandleEvent = Event::RIGHT_SELECTION_HANDLE_EVENT == event.type;

  if(Event::GRAB_HANDLE_EVENT == event.type)
  {
    impl.ChangeState(EventData::GRAB_HANDLE_PANNING);

    // Get the grab handle position in decorator coords.
    Vector2 position = decorator->GetPosition(GRAB_HANDLE);

    if(decorator->IsHorizontalScrollEnabled())
    {
      // Position the grag handle close to either the left or right edge.
      position.x = scrollRightDirection ? 0.f : visualModel->mControlSize.width;
    }

    if(decorator->IsVerticalScrollEnabled())
    {
      position.x = eventData.mCursorHookPositionX;

      // Position the grag handle close to either the top or bottom edge.
      position.y = scrollBottomDirection ? 0.f : visualModel->mControlSize.height;
    }

    // Get the new handle position.
    // The grab handle's position is in decorator's coords. Need to transforms to text's coords.
    bool                 matchedCharacter = false;
    const CharacterIndex handlePosition   = Text::GetClosestCursorIndex(visualModel,
                                                                      impl.mModel->mLogicalModel,
                                                                      impl.mMetrics,
                                                                      position.x - scrollPosition.x,
                                                                      position.y - scrollPosition.y,
                                                                      CharacterHitTest::SCROLL,
                                                                      matchedCharacter);

    if(eventData.mPrimaryCursorPosition != handlePosition)
    {
      eventData.mUpdateCursorPosition      = true;
      eventData.mUpdateGrabHandlePosition  = !isSmoothHandlePanEnabled;
      eventData.mScrollAfterUpdatePosition = true;
      eventData.mPrimaryCursorPosition     = handlePosition;
    }
    eventData.mUpdateInputStyle = eventData.mUpdateCursorPosition;

    // Updates the decorator if the soft handle panning is enabled.
    eventData.mDecoratorUpdated = isSmoothHandlePanEnabled;
  }
  else if(leftSelectionHandleEvent || rightSelectionHandleEvent)
  {
    impl.ChangeState(EventData::SELECTION_HANDLE_PANNING);

    // Get the selection handle position in decorator coords.
    Vector2 position = decorator->GetPosition(leftSelectionHandleEvent ? Text::LEFT_SELECTION_HANDLE : Text::RIGHT_SELECTION_HANDLE);

    if(decorator->IsHorizontalScrollEnabled())
    {
      // Position the selection handle close to either the left or right edge.
      position.x = scrollRightDirection ? 0.f : visualModel->mControlSize.width;
    }

    if(decorator->IsVerticalScrollEnabled())
    {
      position.x = eventData.mCursorHookPositionX;

      // Position the grag handle close to either the top or bottom edge.
      position.y = scrollBottomDirection ? 0.f : visualModel->mControlSize.height;
    }

    // Get the new handle position.
    // The selection handle's position is in decorator's coords. Need to transform to text's coords.
    bool                 matchedCharacter = false;
    const CharacterIndex handlePosition   = Text::GetClosestCursorIndex(visualModel,
                                                                      impl.mModel->mLogicalModel,
                                                                      impl.mMetrics,
                                                                      position.x - scrollPosition.x,
                                                                      position.y - scrollPosition.y,
                                                                      CharacterHitTest::SCROLL,
                                                                      matchedCharacter);
    uint32_t             oldStart         = eventData.mLeftSelectionPosition;
    uint32_t             oldEnd           = eventData.mRightSelectionPosition;

    if(leftSelectionHandleEvent)
    {
      const bool differentHandles = (eventData.mLeftSelectionPosition != handlePosition) && (eventData.mRightSelectionPosition != handlePosition);

      if(differentHandles || endOfScroll)
      {
        eventData.mUpdateHighlightBox           = true;
        eventData.mUpdateLeftSelectionPosition  = !isSmoothHandlePanEnabled;
        eventData.mUpdateRightSelectionPosition = isSmoothHandlePanEnabled;
        eventData.mLeftSelectionPosition        = handlePosition;
      }
    }
    else
    {
      const bool differentHandles = (eventData.mRightSelectionPosition != handlePosition) && (eventData.mLeftSelectionPosition != handlePosition);
      if(differentHandles || endOfScroll)
      {
        eventData.mUpdateHighlightBox           = true;
        eventData.mUpdateRightSelectionPosition = !isSmoothHandlePanEnabled;
        eventData.mUpdateLeftSelectionPosition  = isSmoothHandlePanEnabled;
        eventData.mRightSelectionPosition       = handlePosition;
      }
    }

    if(eventData.mUpdateLeftSelectionPosition || eventData.mUpdateRightSelectionPosition)
    {
      impl.RepositionSelectionHandles();

      eventData.mScrollAfterUpdatePosition = !isSmoothHandlePanEnabled;

      if(impl.mSelectableControlInterface != nullptr)
      {
        impl.mSelectableControlInterface->SelectionChanged(oldStart, oldEnd, eventData.mLeftSelectionPosition, eventData.mRightSelectionPosition);
      }
    }
  }
  eventData.mDecoratorUpdated = true;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
