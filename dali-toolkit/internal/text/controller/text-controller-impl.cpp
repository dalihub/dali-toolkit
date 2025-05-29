/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/controller/text-controller-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/rendering/renderer.h>
#include <cmath>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-controls/common-text-utils.h>
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/controller/text-controller-impl-data-clearer.h>
#include <dali-toolkit/internal/text/controller/text-controller-impl-event-handler.h>
#include <dali-toolkit/internal/text/controller/text-controller-impl-model-updater.h>
#include <dali-toolkit/internal/text/controller/text-controller-placeholder-handler.h>
#include <dali-toolkit/internal/text/controller/text-controller-relayouter.h>
#include <dali-toolkit/internal/text/cursor-helper-functions.h>
#include <dali-toolkit/internal/text/glyph-metrics-helper.h>
#include <dali-toolkit/internal/text/text-control-interface.h>
#include <dali-toolkit/internal/text/text-editable-control-interface.h>
#include <dali-toolkit/internal/text/text-enumerations-impl.h>
#include <dali-toolkit/internal/text/text-run-container.h>
#include <dali-toolkit/internal/text/text-selection-handle-controller.h>
#include <dali-toolkit/internal/text/underlined-glyph-run.h>

using namespace Dali;

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

constexpr float MAX_FLOAT = std::numeric_limits<float>::max();

const char* EMPTY_STRING = "";

} // namespace

namespace Dali::Toolkit::Text
{
namespace
{
void SetDefaultInputStyle(InputStyle& inputStyle, const FontDefaults* const fontDefaults, const Vector4& textColor)
{
  // Sets the default text's color.
  inputStyle.textColor      = textColor;
  inputStyle.isDefaultColor = true;

  inputStyle.familyName.clear();
  inputStyle.weight = TextAbstraction::FontWeight::NORMAL;
  inputStyle.width  = TextAbstraction::FontWidth::NORMAL;
  inputStyle.slant  = TextAbstraction::FontSlant::NORMAL;
  inputStyle.size   = 0.f;

  inputStyle.lineSpacing = 0.f;

  inputStyle.underlineProperties.clear();
  inputStyle.shadowProperties.clear();
  inputStyle.embossProperties.clear();
  inputStyle.outlineProperties.clear();

  inputStyle.isFamilyDefined = false;
  inputStyle.isWeightDefined = false;
  inputStyle.isWidthDefined  = false;
  inputStyle.isSlantDefined  = false;
  inputStyle.isSizeDefined   = false;

  inputStyle.isLineSpacingDefined = false;

  inputStyle.isUnderlineDefined = false;
  inputStyle.isShadowDefined    = false;
  inputStyle.isEmbossDefined    = false;
  inputStyle.isOutlineDefined   = false;

  // Sets the default font's family name, weight, width, slant and size.
  if(fontDefaults)
  {
    if(fontDefaults->familyDefined)
    {
      inputStyle.familyName      = fontDefaults->mFontDescription.family;
      inputStyle.isFamilyDefined = true;
    }

    if(fontDefaults->weightDefined)
    {
      inputStyle.weight          = fontDefaults->mFontDescription.weight;
      inputStyle.isWeightDefined = true;
    }

    if(fontDefaults->widthDefined)
    {
      inputStyle.width          = fontDefaults->mFontDescription.width;
      inputStyle.isWidthDefined = true;
    }

    if(fontDefaults->slantDefined)
    {
      inputStyle.slant          = fontDefaults->mFontDescription.slant;
      inputStyle.isSlantDefined = true;
    }

    if(fontDefaults->sizeDefined)
    {
      inputStyle.size          = fontDefaults->mDefaultPointSize;
      inputStyle.isSizeDefined = true;
    }
  }
}

void ChangeTextControllerState(Controller::Impl& impl, EventData::State newState)
{
  EventData* eventData = impl.mEventData;

  if(nullptr == eventData)
  {
    // Nothing to do if there is no text input.
    return;
  }

  DecoratorPtr& decorator = eventData->mDecorator;
  if(!decorator)
  {
    // Nothing to do if there is no decorator.
    return;
  }

  DALI_LOG_INFO(gLogFilter, Debug::General, "ChangeState state:%d  newstate:%d\n", eventData->mState, newState);

  if(eventData->mState != newState)
  {
    eventData->mPreviousState = eventData->mState;
    eventData->mState         = newState;

    switch(eventData->mState)
    {
      case EventData::INACTIVE:
      {
        decorator->SetActiveCursor(ACTIVE_CURSOR_NONE);
        decorator->StopCursorBlink();
        decorator->SetHandleActive(GRAB_HANDLE, false);
        decorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
        decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);
        decorator->SetHighlightActive(false);
        decorator->SetPopupActive(false);
        eventData->mDecoratorUpdated = true;
        break;
      }

      case EventData::INTERRUPTED:
      {
        decorator->SetHandleActive(GRAB_HANDLE, false);
        decorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
        decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);
        decorator->SetHighlightActive(false);
        decorator->SetPopupActive(false);
        eventData->mDecoratorUpdated = true;
        break;
      }

      case EventData::SELECTING:
      {
        decorator->SetActiveCursor(ACTIVE_CURSOR_NONE);
        decorator->StopCursorBlink();
        decorator->SetHandleActive(GRAB_HANDLE, false);
        if(eventData->mGrabHandleEnabled)
        {
          decorator->SetHandleActive(LEFT_SELECTION_HANDLE, true);
          decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, true);
        }
        decorator->SetHighlightActive(true);
        if(eventData->mGrabHandlePopupEnabled)
        {
          impl.SetPopupButtons();
          decorator->SetPopupActive(true);
        }
        eventData->mDecoratorUpdated = true;
        break;
      }

      case EventData::EDITING:
      {
        decorator->SetActiveCursor(ACTIVE_CURSOR_PRIMARY);
        if(eventData->mCursorBlinkEnabled)
        {
          decorator->StartCursorBlink();
        }
        // Grab handle is not shown until a tap is received whilst EDITING
        decorator->SetHandleActive(GRAB_HANDLE, false);
        decorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
        decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);
        decorator->SetHighlightActive(false);
        if(eventData->mGrabHandlePopupEnabled)
        {
          decorator->SetPopupActive(false);
        }
        eventData->mDecoratorUpdated = true;
        break;
      }
      case EventData::EDITING_WITH_POPUP:
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "EDITING_WITH_POPUP \n", newState);

        decorator->SetActiveCursor(ACTIVE_CURSOR_PRIMARY);
        if(eventData->mCursorBlinkEnabled)
        {
          decorator->StartCursorBlink();
        }
        if(eventData->mSelectionEnabled)
        {
          decorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
          decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);
          decorator->SetHighlightActive(false);
        }
        else if(eventData->mGrabHandleEnabled)
        {
          decorator->SetHandleActive(GRAB_HANDLE, true);
        }
        if(eventData->mGrabHandlePopupEnabled)
        {
          impl.SetPopupButtons();
          decorator->SetPopupActive(true);
        }
        eventData->mDecoratorUpdated = true;
        break;
      }
      case EventData::EDITING_WITH_GRAB_HANDLE:
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "EDITING_WITH_GRAB_HANDLE \n", newState);

        decorator->SetActiveCursor(ACTIVE_CURSOR_PRIMARY);
        if(eventData->mCursorBlinkEnabled)
        {
          decorator->StartCursorBlink();
        }
        // Grab handle is not shown until a tap is received whilst EDITING
        if(eventData->mGrabHandleEnabled)
        {
          decorator->SetHandleActive(GRAB_HANDLE, true);
        }
        decorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
        decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);
        decorator->SetHighlightActive(false);
        if(eventData->mGrabHandlePopupEnabled)
        {
          decorator->SetPopupActive(false);
        }
        eventData->mDecoratorUpdated = true;
        break;
      }

      case EventData::SELECTION_HANDLE_PANNING:
      {
        decorator->SetActiveCursor(ACTIVE_CURSOR_NONE);
        decorator->StopCursorBlink();
        decorator->SetHandleActive(GRAB_HANDLE, false);
        if(eventData->mGrabHandleEnabled)
        {
          decorator->SetHandleActive(LEFT_SELECTION_HANDLE, true);
          decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, true);
        }
        decorator->SetHighlightActive(true);
        if(eventData->mGrabHandlePopupEnabled)
        {
          decorator->SetPopupActive(false);
        }
        eventData->mDecoratorUpdated = true;
        break;
      }

      case EventData::GRAB_HANDLE_PANNING:
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "GRAB_HANDLE_PANNING \n", newState);

        decorator->SetActiveCursor(ACTIVE_CURSOR_PRIMARY);
        if(eventData->mCursorBlinkEnabled)
        {
          decorator->StartCursorBlink();
        }
        if(eventData->mGrabHandleEnabled)
        {
          decorator->SetHandleActive(GRAB_HANDLE, true);
        }
        decorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
        decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);
        decorator->SetHighlightActive(false);
        if(eventData->mGrabHandlePopupEnabled)
        {
          decorator->SetPopupActive(false);
        }
        eventData->mDecoratorUpdated = true;
        break;
      }

      case EventData::EDITING_WITH_PASTE_POPUP:
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "EDITING_WITH_PASTE_POPUP \n", newState);

        decorator->SetActiveCursor(ACTIVE_CURSOR_PRIMARY);
        if(eventData->mCursorBlinkEnabled)
        {
          decorator->StartCursorBlink();
        }

        if(eventData->mGrabHandleEnabled)
        {
          decorator->SetHandleActive(GRAB_HANDLE, true);
        }
        decorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
        decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);
        decorator->SetHighlightActive(false);

        if(eventData->mGrabHandlePopupEnabled)
        {
          impl.SetPopupButtons();
          decorator->SetPopupActive(true);
        }
        eventData->mDecoratorUpdated = true;
        break;
      }

      case EventData::TEXT_PANNING:
      {
        decorator->SetActiveCursor(ACTIVE_CURSOR_NONE);
        decorator->StopCursorBlink();
        decorator->SetHandleActive(GRAB_HANDLE, false);
        if(eventData->mDecorator->IsHandleActive(LEFT_SELECTION_HANDLE) ||
           decorator->IsHandleActive(RIGHT_SELECTION_HANDLE))
        {
          decorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
          decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);
          decorator->SetHighlightActive(true);
        }

        if(eventData->mGrabHandlePopupEnabled)
        {
          decorator->SetPopupActive(false);
        }

        eventData->mDecoratorUpdated = true;
        break;
      }
    }
  }
}

void UpdateCursorPositionForAlignment(Controller::Impl& impl, bool needFullAlignment)
{
  EventData* eventData = impl.mEventData;

  // Set the flag to redo the alignment operation
  impl.mOperationsPending = static_cast<Controller::OperationsMask>(impl.mOperationsPending | Controller::OperationsMask::ALIGN);

  if(eventData)
  {
    // Note: mUpdateAlignment is currently only needed for horizontal alignment
    eventData->mUpdateAlignment = needFullAlignment;

    // Update the cursor if it's in editing mode
    if(EventData::IsEditingState(eventData->mState))
    {
      impl.ChangeState(EventData::EDITING);
      eventData->mUpdateCursorPosition = true;
    }
  }
}

} // unnamed Namespace

EventData::EventData(DecoratorPtr decorator, InputMethodContext& inputMethodContext)
: mDecorator(decorator),
  mInputMethodContext(inputMethodContext),
  mPlaceholderFont(nullptr),
  mPlaceholderTextActive(),
  mPlaceholderTextInactive(),
  mPlaceholderTextColor(0.8f, 0.8f, 0.8f, 0.8f), // This color has been published in the Public API (placeholder-properties.h).
  mEventQueue(),
  mInputStyleChangedQueue(),
  mPreviousState(INACTIVE),
  mState(INACTIVE),
  mPrimaryCursorPosition(0u),
  mLeftSelectionPosition(0u),
  mRightSelectionPosition(0u),
  mPreEditStartPosition(0u),
  mPreEditLength(0u),
  mCursorHookPositionX(0.f),
  mDoubleTapAction(Controller::NoTextTap::NO_ACTION),
  mLongPressAction(Controller::NoTextTap::SHOW_SELECTION_POPUP),
  mIsShowingPlaceholderText(false),
  mPreEditFlag(false),
  mDecoratorUpdated(false),
  mCursorBlinkEnabled(true),
  mGrabHandleEnabled(true),
  mGrabHandlePopupEnabled(true),
  mSelectionEnabled(true),
  mUpdateCursorHookPosition(false),
  mUpdateCursorPosition(false),
  mUpdateGrabHandlePosition(false),
  mUpdateLeftSelectionPosition(false),
  mUpdateRightSelectionPosition(false),
  mIsLeftHandleSelected(false),
  mIsRightHandleSelected(false),
  mUpdateHighlightBox(false),
  mScrollAfterUpdatePosition(false),
  mScrollAfterDelete(false),
  mAllTextSelected(false),
  mUpdateInputStyle(false),
  mPasswordInput(false),
  mCheckScrollAmount(false),
  mIsPlaceholderPixelSize(false),
  mIsPlaceholderElideEnabled(false),
  mPlaceholderEllipsisFlag(false),
  mShiftSelectionFlag(true),
  mUpdateAlignment(false),
  mEditingEnabled(true)
{
}

bool Controller::Impl::ProcessInputEvents()
{
  return ControllerImplEventHandler::ProcessInputEvents(*this);
}

void Controller::Impl::SetAnchorColor(const Vector4& color)
{
  mAnchorColor = color;
  UpdateAnchorColor();
}

const Vector4& Controller::Impl::GetAnchorColor() const
{
  return mAnchorColor;
}

void Controller::Impl::SetAnchorClickedColor(const Vector4& color)
{
  mAnchorClickedColor = color;
  UpdateAnchorColor();
}

const Vector4& Controller::Impl::GetAnchorClickedColor() const
{
  return mAnchorClickedColor;
}

void Controller::Impl::UpdateAnchorColor()
{
  if(!mAnchorControlInterface ||
     !mMarkupProcessorEnabled ||
     !mModel->mLogicalModel->mAnchors.Count() ||
     !IsShowingRealText())
  {
    return;
  }

  bool updateNeeded = false;

  // The anchor color & clicked color needs to be updated with the property's color.
  for(auto& anchor : mModel->mLogicalModel->mAnchors)
  {
    if(!anchor.isMarkupColorSet && !anchor.isClicked)
    {
      if(mModel->mLogicalModel->mColorRuns.Count() > anchor.colorRunIndex)
      {
        ColorRun& colorRun = *(mModel->mLogicalModel->mColorRuns.Begin() + anchor.colorRunIndex);
        colorRun.color     = mAnchorColor;
        updateNeeded       = true;
      }
      if(mModel->mLogicalModel->mUnderlinedCharacterRuns.Count() > anchor.underlinedCharacterRunIndex)
      {
        UnderlinedCharacterRun& underlineRun = *(mModel->mLogicalModel->mUnderlinedCharacterRuns.Begin() + anchor.underlinedCharacterRunIndex);
        underlineRun.properties.color        = mAnchorColor;
        updateNeeded                         = true;
      }
    }
    else if(!anchor.isMarkupClickedColorSet && anchor.isClicked)
    {
      if(mModel->mLogicalModel->mColorRuns.Count() > anchor.colorRunIndex)
      {
        ColorRun& colorRun = *(mModel->mLogicalModel->mColorRuns.Begin() + anchor.colorRunIndex);
        colorRun.color     = mAnchorClickedColor;
        updateNeeded       = true;
      }
      if(mModel->mLogicalModel->mUnderlinedCharacterRuns.Count() > anchor.underlinedCharacterRunIndex)
      {
        UnderlinedCharacterRun& underlineRun = *(mModel->mLogicalModel->mUnderlinedCharacterRuns.Begin() + anchor.underlinedCharacterRunIndex);
        underlineRun.properties.color        = mAnchorClickedColor;
        updateNeeded                         = true;
      }
    }
  }

  if(updateNeeded)
  {
    ClearFontData();
    mOperationsPending = static_cast<OperationsMask>(mOperationsPending | COLOR);
    RequestRelayout();
  }
}

void Controller::Impl::NotifyInputMethodContext()
{
  if(mEventData && mEventData->mInputMethodContext)
  {
    CharacterIndex cursorPosition = GetLogicalCursorPosition();
    mEventData->mInputMethodContext.SetCursorPosition(cursorPosition);
    mEventData->mInputMethodContext.NotifyCursorPosition();
  }
}

void Controller::Impl::NotifyInputMethodContextMultiLineStatus()
{
  if(mEventData && mEventData->mInputMethodContext)
  {
    Text::Layout::Engine::Type layout = mLayoutEngine.GetLayout();
    mEventData->mInputMethodContext.NotifyTextInputMultiLine(layout == Text::Layout::Engine::MULTI_LINE_BOX);
  }
}

CharacterIndex Controller::Impl::GetLogicalCursorPosition() const
{
  CharacterIndex cursorPosition = 0u;

  if(mEventData)
  {
    if((EventData::SELECTING == mEventData->mState) ||
       (EventData::SELECTION_HANDLE_PANNING == mEventData->mState))
    {
      cursorPosition = std::min(mEventData->mRightSelectionPosition, mEventData->mLeftSelectionPosition);
    }
    else
    {
      cursorPosition = mEventData->mPrimaryCursorPosition;
    }
  }

  return cursorPosition;
}

Length Controller::Impl::GetNumberOfWhiteSpaces(CharacterIndex index) const
{
  Length numberOfWhiteSpaces = 0u;

  // Get the buffer to the text.
  Character* utf32CharacterBuffer = mModel->mLogicalModel->mText.Begin();

  const Length totalNumberOfCharacters = mModel->mLogicalModel->mText.Count();
  for(; index < totalNumberOfCharacters; ++index, ++numberOfWhiteSpaces)
  {
    if(!TextAbstraction::IsWhiteSpace(*(utf32CharacterBuffer + index)))
    {
      break;
    }
  }

  return numberOfWhiteSpaces;
}

void Controller::Impl::GetText(std::string& text) const
{
  if(!IsShowingPlaceholderText())
  {
    // Retrieves the text string.
    GetText(0u, text);
  }
  else
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::GetText %p empty (but showing placeholder)\n", this);
  }
}

void Controller::Impl::GetText(CharacterIndex index, std::string& text) const
{
  // Get the total number of characters.
  Length numberOfCharacters = mModel->mLogicalModel->mText.Count();

  // Retrieve the text.
  if(0u != numberOfCharacters)
  {
    Utf32ToUtf8(mModel->mLogicalModel->mText.Begin() + index, numberOfCharacters - index, text);
  }
}

Dali::LayoutDirection::Type Controller::Impl::GetLayoutDirection(Dali::Actor& actor) const
{
  if(mModel->mMatchLayoutDirection == DevelText::MatchLayoutDirection::LOCALE ||
     (mModel->mMatchLayoutDirection == DevelText::MatchLayoutDirection::INHERIT && !mIsLayoutDirectionChanged))
  {
    Integration::SceneHolder sceneHolder = Integration::SceneHolder::Get(actor);
    return static_cast<Dali::LayoutDirection::Type>(sceneHolder ? sceneHolder.GetRootLayer().GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>() : LayoutDirection::LEFT_TO_RIGHT);
  }
  else
  {
    return static_cast<Dali::LayoutDirection::Type>(actor.GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());
  }
}

Toolkit::DevelText::TextDirection::Type Controller::Impl::GetTextDirection()
{
  if(mUpdateTextDirection)
  {
    // Operations that can be done only once until the text changes.
    const OperationsMask onlyOnceOperations = static_cast<OperationsMask>(CONVERT_TO_UTF32 |
                                                                          GET_SCRIPTS |
                                                                          VALIDATE_FONTS |
                                                                          GET_LINE_BREAKS |
                                                                          BIDI_INFO |
                                                                          SHAPE_TEXT |
                                                                          GET_GLYPH_METRICS);

    // Set the update info to relayout the whole text.
    mTextUpdateInfo.mParagraphCharacterIndex     = 0u;
    mTextUpdateInfo.mRequestedNumberOfCharacters = mModel->mLogicalModel->mText.Count();

    // Make sure the model is up-to-date before layouting
    UpdateModel(onlyOnceOperations);

    Vector3 naturalSize;
    Relayouter::DoRelayout(*this,
                           Size(MAX_FLOAT, MAX_FLOAT),
                           static_cast<OperationsMask>(onlyOnceOperations |
                                                       LAYOUT | REORDER | UPDATE_DIRECTION),
                           naturalSize.GetVectorXY());

    // Do not do again the only once operations.
    mOperationsPending = static_cast<OperationsMask>(mOperationsPending & ~onlyOnceOperations);

    // Clear the update info. This info will be set the next time the text is updated.
    mTextUpdateInfo.Clear();

    // FullRelayoutNeeded should be true because DoRelayout is MAX_FLOAT, MAX_FLOAT.
    mTextUpdateInfo.mFullRelayoutNeeded = true;

    mUpdateTextDirection = false;
  }

  return mIsTextDirectionRTL ? Toolkit::DevelText::TextDirection::RIGHT_TO_LEFT : Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT;
}

void Controller::Impl::CalculateTextUpdateIndices(Length& numberOfCharacters)
{
  mTextUpdateInfo.mParagraphCharacterIndex = 0u;
  mTextUpdateInfo.mStartGlyphIndex         = 0u;
  mTextUpdateInfo.mStartLineIndex          = 0u;
  numberOfCharacters                       = 0u;

  const Length numberOfParagraphs = mModel->mLogicalModel->mParagraphInfo.Count();
  if(0u == numberOfParagraphs)
  {
    mTextUpdateInfo.mParagraphCharacterIndex = 0u;
    numberOfCharacters                       = 0u;

    mTextUpdateInfo.mRequestedNumberOfCharacters = mTextUpdateInfo.mNumberOfCharactersToAdd - mTextUpdateInfo.mNumberOfCharactersToRemove;

    // Nothing else to do if there are no paragraphs.
    return;
  }

  // Find the paragraphs to be updated.
  Vector<ParagraphRunIndex> paragraphsToBeUpdated;
  if(mTextUpdateInfo.mCharacterIndex >= mTextUpdateInfo.mPreviousNumberOfCharacters)
  {
    // Text is being added at the end of the current text.
    if(mTextUpdateInfo.mIsLastCharacterNewParagraph)
    {
      // Text is being added in a new paragraph after the last character of the text.
      mTextUpdateInfo.mParagraphCharacterIndex     = mTextUpdateInfo.mPreviousNumberOfCharacters;
      numberOfCharacters                           = 0u;
      mTextUpdateInfo.mRequestedNumberOfCharacters = mTextUpdateInfo.mNumberOfCharactersToAdd - mTextUpdateInfo.mNumberOfCharactersToRemove;

      mTextUpdateInfo.mStartGlyphIndex = mModel->mVisualModel->mGlyphs.Count();
      mTextUpdateInfo.mStartLineIndex  = mModel->mVisualModel->mLines.Count() - 1u;

      // Nothing else to do;
      return;
    }

    paragraphsToBeUpdated.PushBack(numberOfParagraphs - 1u);
  }
  else
  {
    Length numberOfCharactersToUpdate = 0u;
    if(mTextUpdateInfo.mFullRelayoutNeeded)
    {
      numberOfCharactersToUpdate = mTextUpdateInfo.mPreviousNumberOfCharacters;
    }
    else
    {
      numberOfCharactersToUpdate = (mTextUpdateInfo.mNumberOfCharactersToRemove > 0u) ? mTextUpdateInfo.mNumberOfCharactersToRemove : 1u;
    }
    mModel->mLogicalModel->FindParagraphs(mTextUpdateInfo.mCharacterIndex,
                                          numberOfCharactersToUpdate,
                                          paragraphsToBeUpdated);
  }

  if(0u != paragraphsToBeUpdated.Count())
  {
    const ParagraphRunIndex firstParagraphIndex = *(paragraphsToBeUpdated.Begin());
    const ParagraphRun&     firstParagraph      = *(mModel->mLogicalModel->mParagraphInfo.Begin() + firstParagraphIndex);
    mTextUpdateInfo.mParagraphCharacterIndex    = firstParagraph.characterRun.characterIndex;

    ParagraphRunIndex   lastParagraphIndex = *(paragraphsToBeUpdated.End() - 1u);
    const ParagraphRun& lastParagraph      = *(mModel->mLogicalModel->mParagraphInfo.Begin() + lastParagraphIndex);

    if((mTextUpdateInfo.mNumberOfCharactersToRemove > 0u) &&                                           // Some character are removed.
       (lastParagraphIndex < numberOfParagraphs - 1u) &&                                               // There is a next paragraph.
       ((lastParagraph.characterRun.characterIndex + lastParagraph.characterRun.numberOfCharacters) == // The last removed character is the new paragraph character.
        (mTextUpdateInfo.mCharacterIndex + mTextUpdateInfo.mNumberOfCharactersToRemove)))
    {
      // The new paragraph character of the last updated paragraph has been removed so is going to be merged with the next one.
      const ParagraphRun& lastParagraph = *(mModel->mLogicalModel->mParagraphInfo.Begin() + lastParagraphIndex + 1u);

      numberOfCharacters = lastParagraph.characterRun.characterIndex + lastParagraph.characterRun.numberOfCharacters - mTextUpdateInfo.mParagraphCharacterIndex;
    }
    else
    {
      numberOfCharacters = lastParagraph.characterRun.characterIndex + lastParagraph.characterRun.numberOfCharacters - mTextUpdateInfo.mParagraphCharacterIndex;
    }
  }

  mTextUpdateInfo.mRequestedNumberOfCharacters = numberOfCharacters + mTextUpdateInfo.mNumberOfCharactersToAdd - mTextUpdateInfo.mNumberOfCharactersToRemove;
  mTextUpdateInfo.mStartGlyphIndex             = *(mModel->mVisualModel->mCharactersToGlyph.Begin() + mTextUpdateInfo.mParagraphCharacterIndex);
}

void Controller::Impl::ClearModelData(CharacterIndex startIndex, CharacterIndex endIndex, OperationsMask operations)
{
  ControllerImplDataClearer::ClearModelData(*this, startIndex, endIndex, operations);
}

bool Controller::Impl::UpdateModel(OperationsMask operationsRequired)
{
  return ControllerImplModelUpdater::Update(*this, operationsRequired);
}

void Controller::Impl::RetrieveDefaultInputStyle(InputStyle& inputStyle)
{
  SetDefaultInputStyle(inputStyle, mFontDefaults, mTextColor);
}

float Controller::Impl::GetDefaultFontLineHeight()
{
  FontId defaultFontId = 0u;
  if(nullptr == mFontDefaults)
  {
    TextAbstraction::FontDescription fontDescription;
    defaultFontId = mFontClient.GetFontId(fontDescription, TextAbstraction::FontClient::DEFAULT_POINT_SIZE * GetFontSizeScale());
  }
  else
  {
    defaultFontId = mFontDefaults->GetFontId(mFontClient, mFontDefaults->mDefaultPointSize * GetFontSizeScale());
  }

  Text::FontMetrics fontMetrics;
  mMetrics->GetFontMetrics(defaultFontId, fontMetrics);

  return (fontMetrics.ascender - fontMetrics.descender);
}

bool Controller::Impl::SetDefaultLineSpacing(float lineSpacing)
{
  if(std::fabs(lineSpacing - mLayoutEngine.GetDefaultLineSpacing()) > Math::MACHINE_EPSILON_1000)
  {
    mLayoutEngine.SetDefaultLineSpacing(lineSpacing);

    RelayoutAllCharacters();
    return true;
  }
  return false;
}

bool Controller::Impl::SetDefaultLineSize(float lineSize)
{
  if(std::fabs(lineSize - mLayoutEngine.GetDefaultLineSize()) > Math::MACHINE_EPSILON_1000)
  {
    mLayoutEngine.SetDefaultLineSize(lineSize);

    RelayoutAllCharacters();
    return true;
  }
  return false;
}

bool Controller::Impl::SetRelativeLineSize(float relativeLineSize)
{
  if(std::fabs(relativeLineSize - GetRelativeLineSize()) > Math::MACHINE_EPSILON_1000)
  {
    mLayoutEngine.SetRelativeLineSize(relativeLineSize);

    RelayoutAllCharacters();
    return true;
  }
  return false;
}

float Controller::Impl::GetRelativeLineSize()
{
  return mLayoutEngine.GetRelativeLineSize();
}

string Controller::Impl::GetSelectedText()
{
  string text;
  if(EventData::SELECTING == mEventData->mState)
  {
    RetrieveSelection(text, false);
  }
  return text;
}

string Controller::Impl::CopyText()
{
  string text;
  RetrieveSelection(text, false);
  SendSelectionToClipboard(false); // Text not modified

  mEventData->mUpdateCursorPosition = true;

  RequestRelayout(); // Cursor, Handles, Selection Highlight, Popup

  return text;
}

string Controller::Impl::CutText()
{
  string text;
  RetrieveSelection(text, false);

  if(!IsEditable())
  {
    return EMPTY_STRING;
  }

  SendSelectionToClipboard(true); // Synchronous call to modify text
  mOperationsPending = ALL_OPERATIONS;

  if((0u != mModel->mLogicalModel->mText.Count()) ||
     !IsPlaceholderAvailable())
  {
    QueueModifyEvent(ModifyEvent::TEXT_DELETED);
  }
  else
  {
    PlaceholderHandler::ShowPlaceholderText(*this);
  }

  mEventData->mUpdateCursorPosition = true;
  mEventData->mScrollAfterDelete    = true;

  RequestRelayout();

  if(nullptr != mEditableControlInterface)
  {
    mEditableControlInterface->TextChanged(true);
  }
  return text;
}

void Controller::Impl::SetTextSelectionRange(const uint32_t* pStart, const uint32_t* pEnd)
{
  if(nullptr == mEventData)
  {
    // Nothing to do if there is no text.
    return;
  }

  if(mEventData->mSelectionEnabled && (pStart || pEnd))
  {
    uint32_t length   = static_cast<uint32_t>(mModel->mLogicalModel->mText.Count());
    uint32_t oldStart = mEventData->mLeftSelectionPosition;
    uint32_t oldEnd   = mEventData->mRightSelectionPosition;

    if(pStart)
    {
      mEventData->mLeftSelectionPosition = std::min(*pStart, length);
    }
    if(pEnd)
    {
      mEventData->mRightSelectionPosition = std::min(*pEnd, length);
    }

    if(mEventData->mLeftSelectionPosition == mEventData->mRightSelectionPosition)
    {
      ChangeState(EventData::EDITING);
      mEventData->mPrimaryCursorPosition = mEventData->mLeftSelectionPosition = mEventData->mRightSelectionPosition;
      mEventData->mUpdateCursorPosition                                       = true;
    }
    else
    {
      ChangeState(EventData::SELECTING);
      mEventData->mUpdateHighlightBox           = true;
      mEventData->mUpdateLeftSelectionPosition  = true;
      mEventData->mUpdateRightSelectionPosition = true;
    }

    if(mSelectableControlInterface != nullptr)
    {
      mSelectableControlInterface->SelectionChanged(oldStart, oldEnd, mEventData->mLeftSelectionPosition, mEventData->mRightSelectionPosition);
    }
  }
}

CharacterIndex Controller::Impl::GetPrimaryCursorPosition() const
{
  if(nullptr == mEventData)
  {
    return 0;
  }
  return mEventData->mPrimaryCursorPosition;
}

bool Controller::Impl::SetPrimaryCursorPosition(CharacterIndex index, bool focused)
{
  if(nullptr == mEventData)
  {
    // Nothing to do if there is no text.
    return false;
  }

  if(mEventData->mPrimaryCursorPosition == index && mEventData->mState != EventData::SELECTING)
  {
    // Nothing for same cursor position.
    return false;
  }

  uint32_t length                    = static_cast<uint32_t>(mModel->mLogicalModel->mText.Count());
  uint32_t oldCursorPos              = mEventData->mPrimaryCursorPosition;
  mEventData->mPrimaryCursorPosition = std::min(index, length);
  // If there is no focus, only the value is updated.
  if(focused)
  {
    bool     wasInSelectingState = mEventData->mState == EventData::SELECTING;
    uint32_t oldStart            = mEventData->mLeftSelectionPosition;
    uint32_t oldEnd              = mEventData->mRightSelectionPosition;
    ChangeState(EventData::EDITING);
    mEventData->mLeftSelectionPosition = mEventData->mRightSelectionPosition = mEventData->mPrimaryCursorPosition;
    mEventData->mUpdateCursorPosition                                        = true;

    if(mSelectableControlInterface != nullptr && wasInSelectingState)
    {
      mSelectableControlInterface->SelectionChanged(oldStart, oldEnd, mEventData->mLeftSelectionPosition, mEventData->mRightSelectionPosition);
    }

    ScrollTextToMatchCursor();
  }

  if(nullptr != mEditableControlInterface)
  {
    mEditableControlInterface->CursorPositionChanged(oldCursorPos, mEventData->mPrimaryCursorPosition);
  }

  return true;
}

Uint32Pair Controller::Impl::GetTextSelectionRange() const
{
  Uint32Pair range;

  if(mEventData)
  {
    range.first  = mEventData->mLeftSelectionPosition;
    range.second = mEventData->mRightSelectionPosition;
  }

  return range;
}

bool Controller::Impl::IsEditable() const
{
  return mEventData && mEventData->mEditingEnabled;
}

void Controller::Impl::SetEditable(bool editable)
{
  if(mEventData)
  {
    mEventData->mEditingEnabled = editable;

    if(mEventData->mDecorator)
    {
      bool decoratorEditable = editable && mIsUserInteractionEnabled;
      mEventData->mDecorator->SetEditable(decoratorEditable);
    }
  }
}

void Controller::Impl::UpdateAfterFontChange(const std::string& newDefaultFont)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::UpdateAfterFontChange\n");

  if(!mFontDefaults->familyDefined) // If user defined font then should not update when system font changes
  {
    DALI_LOG_INFO(gLogFilter, Debug::Concise, "Controller::UpdateAfterFontChange newDefaultFont(%s)\n", newDefaultFont.c_str());
    mFontDefaults->mFontDescription.family = newDefaultFont;

    ClearFontData();

    RequestRelayout();
  }
}

void Controller::Impl::RetrieveSelection(std::string& selectedText, bool deleteAfterRetrieval)
{
  if(mEventData->mLeftSelectionPosition == mEventData->mRightSelectionPosition)
  {
    // Nothing to select if handles are in the same place.
    selectedText.clear();
    return;
  }

  const bool handlesCrossed = mEventData->mLeftSelectionPosition > mEventData->mRightSelectionPosition;

  //Get start and end position of selection
  const CharacterIndex startOfSelectedText  = handlesCrossed ? mEventData->mRightSelectionPosition : mEventData->mLeftSelectionPosition;
  const Length         lengthOfSelectedText = (handlesCrossed ? mEventData->mLeftSelectionPosition : mEventData->mRightSelectionPosition) - startOfSelectedText;

  Vector<Character>& utf32Characters    = mModel->mLogicalModel->mText;
  const Length       numberOfCharacters = utf32Characters.Count();

  // Validate the start and end selection points
  if((startOfSelectedText + lengthOfSelectedText) <= numberOfCharacters)
  {
    //Get text as a UTF8 string
    Utf32ToUtf8(&utf32Characters[startOfSelectedText], lengthOfSelectedText, selectedText);

    if(deleteAfterRetrieval) // Only delete text if copied successfully
    {
      // Keep a copy of the current input style.
      InputStyle currentInputStyle;
      currentInputStyle.Copy(mEventData->mInputStyle);

      // Set as input style the style of the first deleted character.
      mModel->mLogicalModel->RetrieveStyle(startOfSelectedText, mEventData->mInputStyle);

      // Compare if the input style has changed.
      const bool hasInputStyleChanged = !currentInputStyle.Equal(mEventData->mInputStyle);

      if(hasInputStyleChanged)
      {
        const InputStyle::Mask styleChangedMask = currentInputStyle.GetInputStyleChangeMask(mEventData->mInputStyle);
        // Queue the input style changed signal.
        mEventData->mInputStyleChangedQueue.PushBack(styleChangedMask);
      }

      mModel->mLogicalModel->UpdateTextStyleRuns(startOfSelectedText, -static_cast<int>(lengthOfSelectedText));

      // Mark the paragraphs to be updated.
      if(Layout::Engine::SINGLE_LINE_BOX == mLayoutEngine.GetLayout())
      {
        mTextUpdateInfo.mCharacterIndex             = 0;
        mTextUpdateInfo.mNumberOfCharactersToRemove = mTextUpdateInfo.mPreviousNumberOfCharacters;
        mTextUpdateInfo.mNumberOfCharactersToAdd    = mTextUpdateInfo.mPreviousNumberOfCharacters - lengthOfSelectedText;
        mTextUpdateInfo.mClearAll                   = true;
      }
      else
      {
        mTextUpdateInfo.mCharacterIndex             = startOfSelectedText;
        mTextUpdateInfo.mNumberOfCharactersToRemove = lengthOfSelectedText;
      }

      // Delete text between handles
      Vector<Character>::Iterator first = utf32Characters.Begin() + startOfSelectedText;
      Vector<Character>::Iterator last  = first + lengthOfSelectedText;
      utf32Characters.Erase(first, last);

      // Will show the cursor at the first character of the selection.
      mEventData->mPrimaryCursorPosition = handlesCrossed ? mEventData->mRightSelectionPosition : mEventData->mLeftSelectionPosition;
    }
    else
    {
      // Will show the cursor at the last character of the selection.
      mEventData->mPrimaryCursorPosition = handlesCrossed ? mEventData->mLeftSelectionPosition : mEventData->mRightSelectionPosition;
    }

    mEventData->mDecoratorUpdated = true;
  }
}

void Controller::Impl::SetSelection(int start, int end)
{
  uint32_t oldStart = mEventData->mLeftSelectionPosition;
  uint32_t oldEnd   = mEventData->mRightSelectionPosition;

  mEventData->mLeftSelectionPosition  = start;
  mEventData->mRightSelectionPosition = end;
  mEventData->mUpdateCursorPosition   = true;

  if(mSelectableControlInterface != nullptr)
  {
    mSelectableControlInterface->SelectionChanged(oldStart, oldEnd, start, end);
  }
}

std::pair<int, int> Controller::Impl::GetSelectionIndexes() const
{
  return {mEventData->mLeftSelectionPosition, mEventData->mRightSelectionPosition};
}

void Controller::Impl::ShowClipboard()
{
  if(EnsureClipboardCreated())
  {
    mClipboard.ShowClipboard();
  }
}

void Controller::Impl::HideClipboard()
{
  if(EnsureClipboardCreated() && mClipboardHideEnabled)
  {
    mClipboard.HideClipboard();
  }
}

void Controller::Impl::SetClipboardHideEnable(bool enable)
{
  mClipboardHideEnabled = enable;
}

bool Controller::Impl::CopyStringToClipboard(const std::string& source)
{
  //Send string to clipboard
  return (EnsureClipboardCreated() && mClipboard.SetItem(source));
}

void Controller::Impl::SendSelectionToClipboard(bool deleteAfterSending)
{
  std::string selectedText;
  RetrieveSelection(selectedText, deleteAfterSending);
  CopyStringToClipboard(selectedText);
  ChangeState(EventData::EDITING);
}

void Controller::Impl::RequestGetTextFromClipboard()
{
  if(EnsureClipboardCreated())
  {
    mClipboard.RequestItem();
  }
}

void Controller::Impl::RepositionSelectionHandles()
{
  SelectionHandleController::Reposition(*this);
}
void Controller::Impl::RepositionSelectionHandles(float visualX, float visualY, Controller::NoTextTap::Action action)
{
  SelectionHandleController::Reposition(*this, visualX, visualY, action);
}

void Controller::Impl::SetPopupButtons()
{
  /**
   *  Sets the Popup buttons to be shown depending on State.
   *
   *  If SELECTING :  CUT & COPY + ( PASTE & CLIPBOARD if content available to paste )
   *
   *  If EDITING_WITH_POPUP : SELECT & SELECT_ALL
   */

  bool                        isEditable    = IsEditable();
  TextSelectionPopup::Buttons buttonsToShow = TextSelectionPopup::NONE;

  if(EventData::SELECTING == mEventData->mState)
  {
    buttonsToShow = TextSelectionPopup::Buttons(TextSelectionPopup::COPY);
    if(isEditable)
    {
      buttonsToShow = TextSelectionPopup::Buttons(buttonsToShow | TextSelectionPopup::CUT);
    }

    if(!IsClipboardEmpty())
    {
      if(isEditable)
      {
        buttonsToShow = TextSelectionPopup::Buttons((buttonsToShow | TextSelectionPopup::PASTE));
      }
      buttonsToShow = TextSelectionPopup::Buttons((buttonsToShow | TextSelectionPopup::CLIPBOARD));
    }

    if(!mEventData->mAllTextSelected)
    {
      buttonsToShow = TextSelectionPopup::Buttons((buttonsToShow | TextSelectionPopup::SELECT_ALL));
    }
  }
  else if(EventData::EDITING_WITH_POPUP == mEventData->mState)
  {
    if(mModel->mLogicalModel->mText.Count() && !IsShowingPlaceholderText())
    {
      buttonsToShow = TextSelectionPopup::Buttons(TextSelectionPopup::SELECT | TextSelectionPopup::SELECT_ALL);
    }

    if(!IsClipboardEmpty())
    {
      if(isEditable)
      {
        buttonsToShow = TextSelectionPopup::Buttons((buttonsToShow | TextSelectionPopup::PASTE));
      }
      buttonsToShow = TextSelectionPopup::Buttons((buttonsToShow | TextSelectionPopup::CLIPBOARD));
    }
  }
  else if(EventData::EDITING_WITH_PASTE_POPUP == mEventData->mState)
  {
    if(!IsClipboardEmpty())
    {
      if(isEditable)
      {
        buttonsToShow = TextSelectionPopup::Buttons((buttonsToShow | TextSelectionPopup::PASTE));
      }
      buttonsToShow = TextSelectionPopup::Buttons((buttonsToShow | TextSelectionPopup::CLIPBOARD));
    }
  }

  mEventData->mDecorator->SetEnabledPopupButtons(buttonsToShow);
}

void Controller::Impl::ChangeState(EventData::State newState)
{
  ChangeTextControllerState(*this, newState);
}

void Controller::Impl::GetCursorPosition(CharacterIndex logical,
                                         CursorInfo&    cursorInfo)
{
  if(!IsShowingRealText())
  {
    // Do not want to use the place-holder text to set the cursor position.

    // Use the line's height of the font's family set to set the cursor's size.
    // If there is no font's family set, use the default font.
    // Use the current alignment to place the cursor at the beginning, center or end of the box.

    cursorInfo.lineOffset          = 0.f;
    cursorInfo.lineHeight          = GetDefaultFontLineHeight();
    cursorInfo.primaryCursorHeight = cursorInfo.lineHeight;

    bool isRTL = false;
    if(mModel->mMatchLayoutDirection != DevelText::MatchLayoutDirection::CONTENTS)
    {
      isRTL = mLayoutDirection == LayoutDirection::RIGHT_TO_LEFT;
    }

    switch(mModel->mHorizontalAlignment)
    {
      case Text::HorizontalAlignment::BEGIN:
      {
        if(isRTL)
        {
          cursorInfo.primaryPosition.x = mModel->mVisualModel->mControlSize.width - mEventData->mDecorator->GetCursorWidth();
        }
        else
        {
          cursorInfo.primaryPosition.x = 0.f;
        }
        break;
      }
      case Text::HorizontalAlignment::CENTER:
      {
        cursorInfo.primaryPosition.x = floorf(0.5f * mModel->mVisualModel->mControlSize.width);
        break;
      }
      case Text::HorizontalAlignment::END:
      {
        if(isRTL)
        {
          cursorInfo.primaryPosition.x = 0.f;
        }
        else
        {
          cursorInfo.primaryPosition.x = mModel->mVisualModel->mControlSize.width - mEventData->mDecorator->GetCursorWidth();
        }
        break;
      }
    }

    // Nothing else to do.
    return;
  }

  const bool                  isMultiLine = (Layout::Engine::MULTI_LINE_BOX == mLayoutEngine.GetLayout());
  GetCursorPositionParameters parameters;
  parameters.visualModel  = mModel->mVisualModel;
  parameters.logicalModel = mModel->mLogicalModel;
  parameters.metrics      = mMetrics;
  parameters.logical      = logical;
  parameters.isMultiline  = isMultiLine;

  float defaultFontLineHeight = GetDefaultFontLineHeight();

  Text::GetCursorPosition(parameters,
                          defaultFontLineHeight,
                          cursorInfo);

  // Adds Outline offset.
  const float outlineWidth = static_cast<float>(mModel->GetOutlineWidth());
  cursorInfo.primaryPosition.x += outlineWidth;
  cursorInfo.primaryPosition.y += outlineWidth;
  cursorInfo.secondaryPosition.x += outlineWidth;
  cursorInfo.secondaryPosition.y += outlineWidth;

  if(isMultiLine)
  {
    // If the text is editable and multi-line, the cursor position after a white space shouldn't exceed the boundaries of the text control.

    // Note the white spaces laid-out at the end of the line might exceed the boundaries of the control.
    // The reason is a wrapped line must not start with a white space so they are laid-out at the end of the line.

    if(0.f > cursorInfo.primaryPosition.x)
    {
      cursorInfo.primaryPosition.x = 0.f;
    }

    const float edgeWidth = mModel->mVisualModel->mControlSize.width - static_cast<float>(mEventData->mDecorator->GetCursorWidth());
    if(cursorInfo.primaryPosition.x > edgeWidth)
    {
      cursorInfo.primaryPosition.x = edgeWidth;
    }
  }
}

CharacterIndex Controller::Impl::CalculateNewCursorIndex(CharacterIndex index) const
{
  if(nullptr == mEventData)
  {
    // Nothing to do if there is no text input.
    return 0u;
  }

  CharacterIndex cursorIndex = mEventData->mPrimaryCursorPosition;

  const GlyphIndex* const charactersToGlyphBuffer  = mModel->mVisualModel->mCharactersToGlyph.Begin();
  const Length* const     charactersPerGlyphBuffer = mModel->mVisualModel->mCharactersPerGlyph.Begin();

  GlyphIndex glyphIndex         = *(charactersToGlyphBuffer + index);
  Length     numberOfCharacters = *(charactersPerGlyphBuffer + glyphIndex);

  if(numberOfCharacters > 1u)
  {
    const Script script = mModel->mLogicalModel->GetScript(index);
    if(HasLigatureMustBreak(script))
    {
      // Prevents to jump the whole Latin ligatures like fi, ff, or Arabic ﻻ, ...
      numberOfCharacters = 1u;
    }
  }
  else
  {
    while(0u == numberOfCharacters)
    {
      ++glyphIndex;
      numberOfCharacters = *(charactersPerGlyphBuffer + glyphIndex);
    }
  }

  if(index < mEventData->mPrimaryCursorPosition)
  {
    cursorIndex = cursorIndex < numberOfCharacters ? 0u : cursorIndex - numberOfCharacters;
  }
  else
  {
    Length textLength = mModel->mVisualModel->mCharactersToGlyph.Count();
    cursorIndex       = cursorIndex + numberOfCharacters > textLength ? textLength : cursorIndex + numberOfCharacters;
  }

  // Will update the cursor hook position.
  mEventData->mUpdateCursorHookPosition = true;

  return cursorIndex;
}

void Controller::Impl::UpdateCursorPosition(const CursorInfo& cursorInfo)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::UpdateCursorPosition %p\n", this);
  if(nullptr == mEventData)
  {
    // Nothing to do if there is no text input.
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::UpdateCursorPosition no event data\n");
    return;
  }

  const Vector2 cursorPosition = cursorInfo.primaryPosition + mModel->mScrollPosition;

  mEventData->mDecorator->SetGlyphOffset(PRIMARY_CURSOR, cursorInfo.glyphOffset);

  // Sets the cursor position.
  mEventData->mDecorator->SetPosition(PRIMARY_CURSOR,
                                      cursorPosition.x,
                                      cursorPosition.y,
                                      cursorInfo.primaryCursorHeight,
                                      cursorInfo.lineHeight);
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Primary cursor position: %f,%f\n", cursorPosition.x, cursorPosition.y);

  if(mEventData->mUpdateGrabHandlePosition)
  {
    // Sets the grab handle position.
    mEventData->mDecorator->SetPosition(GRAB_HANDLE,
                                        cursorPosition.x,
                                        cursorInfo.lineOffset + mModel->mScrollPosition.y,
                                        cursorInfo.lineHeight);
  }

  if(cursorInfo.isSecondaryCursor)
  {
    mEventData->mDecorator->SetPosition(SECONDARY_CURSOR,
                                        cursorInfo.secondaryPosition.x + mModel->mScrollPosition.x,
                                        cursorInfo.secondaryPosition.y + mModel->mScrollPosition.y,
                                        cursorInfo.secondaryCursorHeight,
                                        cursorInfo.lineHeight);
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Secondary cursor position: %f,%f\n", cursorInfo.secondaryPosition.x + mModel->mScrollPosition.x, cursorInfo.secondaryPosition.y + mModel->mScrollPosition.y);
  }

  // Set which cursors are active according the state.
  if(EventData::IsEditingState(mEventData->mState) || (EventData::GRAB_HANDLE_PANNING == mEventData->mState))
  {
    if(cursorInfo.isSecondaryCursor)
    {
      mEventData->mDecorator->SetActiveCursor(ACTIVE_CURSOR_BOTH);
    }
    else
    {
      mEventData->mDecorator->SetActiveCursor(ACTIVE_CURSOR_PRIMARY);
    }
  }
  else
  {
    mEventData->mDecorator->SetActiveCursor(ACTIVE_CURSOR_NONE);
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::UpdateCursorPosition\n");
}

void Controller::Impl::UpdateSelectionHandle(HandleType        handleType,
                                             const CursorInfo& cursorInfo)
{
  SelectionHandleController::Update(*this, handleType, cursorInfo);
}

void Controller::Impl::ClampHorizontalScroll(const Vector2& layoutSize)
{
  // Clamp between -space & -alignment offset.

  if(layoutSize.width > mModel->mVisualModel->mControlSize.width)
  {
    const float space         = (layoutSize.width - mModel->mVisualModel->mControlSize.width) + mModel->mAlignmentOffset;
    mModel->mScrollPosition.x = (mModel->mScrollPosition.x < -space) ? -space : mModel->mScrollPosition.x;
    mModel->mScrollPosition.x = (mModel->mScrollPosition.x > -mModel->mAlignmentOffset) ? -mModel->mAlignmentOffset : mModel->mScrollPosition.x;

    mEventData->mDecoratorUpdated = true;
  }
  else
  {
    mModel->mScrollPosition.x = 0.f;
  }
}

void Controller::Impl::ClampVerticalScroll(const Vector2& layoutSize)
{
  if(Layout::Engine::SINGLE_LINE_BOX == mLayoutEngine.GetLayout())
  {
    // Nothing to do if the text is single line.
    return;
  }

  // Clamp between -space & 0.
  if(layoutSize.height > mModel->mVisualModel->mControlSize.height)
  {
    const float space         = (layoutSize.height - mModel->mVisualModel->mControlSize.height);
    mModel->mScrollPosition.y = (mModel->mScrollPosition.y < -space) ? -space : mModel->mScrollPosition.y;
    mModel->mScrollPosition.y = (mModel->mScrollPosition.y > 0.f) ? 0.f : mModel->mScrollPosition.y;

    mEventData->mDecoratorUpdated = true;
  }
  else
  {
    mModel->mScrollPosition.y = 0.f;
  }
}

void Controller::Impl::ScrollToMakePositionVisible(const Vector2& position, float lineHeight)
{
  const float cursorWidth = mEventData->mDecorator ? static_cast<float>(mEventData->mDecorator->GetCursorWidth()) : 0.f;

  // position is in actor's coords.
  const float positionEndX = position.x + cursorWidth;
  const float positionEndY = position.y + lineHeight;

  // Transform the position to decorator coords.
  const float decoratorPositionBeginX = position.x + mModel->mScrollPosition.x;
  const float decoratorPositionEndX   = positionEndX + mModel->mScrollPosition.x;

  const float decoratorPositionBeginY = position.y + mModel->mScrollPosition.y;
  const float decoratorPositionEndY   = positionEndY + mModel->mScrollPosition.y;

  if(decoratorPositionBeginX < 0.f)
  {
    mModel->mScrollPosition.x = -position.x;
  }
  else if(decoratorPositionEndX > mModel->mVisualModel->mControlSize.width)
  {
    mModel->mScrollPosition.x = mModel->mVisualModel->mControlSize.width - positionEndX;
  }

  if(Layout::Engine::MULTI_LINE_BOX == mLayoutEngine.GetLayout())
  {
    if(decoratorPositionBeginY < 0.f)
    {
      mModel->mScrollPosition.y = -position.y;
    }
    else if(decoratorPositionEndY > mModel->mVisualModel->mControlSize.height)
    {
      mModel->mScrollPosition.y = mModel->mVisualModel->mControlSize.height - positionEndY;
    }
    else if(mModel->mLogicalModel->mText.Count() == 0u)
    {
      Relayouter::CalculateVerticalOffset(*this, mModel->mVisualModel->mControlSize);
    }
  }
}

void Controller::Impl::ScrollTextToMatchCursor(const CursorInfo& cursorInfo)
{
  // Get the current cursor position in decorator coords.
  const Vector2& currentCursorPosition = mEventData->mDecorator->GetPosition(PRIMARY_CURSOR);

  const LineIndex lineIndex = mModel->mVisualModel->GetLineOfCharacter(mEventData->mPrimaryCursorPosition);

  // Calculate the offset to match the cursor position before the character was deleted.
  mModel->mScrollPosition.x = currentCursorPosition.x - cursorInfo.primaryPosition.x;

  //If text control has more than two lines and current line index is not last, calculate scrollpositionY
  if(mModel->mVisualModel->mLines.Count() > 1u && lineIndex != mModel->mVisualModel->mLines.Count() - 1u)
  {
    const float currentCursorGlyphOffset = mEventData->mDecorator->GetGlyphOffset(PRIMARY_CURSOR);
    mModel->mScrollPosition.y            = currentCursorPosition.y - cursorInfo.lineOffset - currentCursorGlyphOffset;
  }

  ClampHorizontalScroll(mModel->mVisualModel->GetLayoutSize());
  ClampVerticalScroll(mModel->mVisualModel->GetLayoutSize());

  // Makes the new cursor position visible if needed.
  ScrollToMakePositionVisible(cursorInfo.primaryPosition, cursorInfo.lineHeight);
}

void Controller::Impl::ScrollTextToMatchCursor()
{
  CursorInfo cursorInfo;
  GetCursorPosition(mEventData->mPrimaryCursorPosition, cursorInfo);
  ScrollTextToMatchCursor(cursorInfo);
}

void Controller::Impl::RequestRelayout()
{
  if(nullptr != mControlInterface)
  {
    mControlInterface->RequestTextRelayout();
  }
}

void Controller::Impl::RelayoutAllCharacters()
{
  // relayout all characters
  mTextUpdateInfo.mCharacterIndex             = 0;
  mTextUpdateInfo.mNumberOfCharactersToRemove = mTextUpdateInfo.mPreviousNumberOfCharacters;
  mTextUpdateInfo.mNumberOfCharactersToAdd    = mModel->mLogicalModel->mText.Count();
  mOperationsPending                          = static_cast<OperationsMask>(mOperationsPending | LAYOUT);

  mTextUpdateInfo.mFullRelayoutNeeded = true;

  // Need to recalculate natural size
  mRecalculateNaturalSize = true;

  //remove selection
  if((mEventData != nullptr) && (mEventData->mState == EventData::SELECTING))
  {
    ChangeState(EventData::EDITING);
  }

  RequestRelayout();
}

bool Controller::Impl::IsInputStyleChangedSignalsQueueEmpty()
{
  return (NULL == mEventData) || (0u == mEventData->mInputStyleChangedQueue.Count());
}

void Controller::Impl::ProcessInputStyleChangedSignals()
{
  if(mEventData)
  {
    if(mEditableControlInterface)
    {
      // Emit the input style changed signal for each mask
      std::for_each(mEventData->mInputStyleChangedQueue.begin(),
                    mEventData->mInputStyleChangedQueue.end(),
                    [&](const auto mask) { mEditableControlInterface->InputStyleChanged(mask); });
    }

    mEventData->mInputStyleChangedQueue.Clear();
  }
}

void Controller::Impl::ScrollBy(Vector2 scroll)
{
  if(mEventData && (fabs(scroll.x) > Math::MACHINE_EPSILON_0 || fabs(scroll.y) > Math::MACHINE_EPSILON_0))
  {
    const Vector2& layoutSize    = mModel->mVisualModel->GetLayoutSize();
    const Vector2  currentScroll = mModel->mScrollPosition;

    scroll.x = -scroll.x;
    scroll.y = -scroll.y;

    if(fabs(scroll.x) > Math::MACHINE_EPSILON_0)
    {
      mModel->mScrollPosition.x += scroll.x;
      ClampHorizontalScroll(layoutSize);
    }

    if(fabs(scroll.y) > Math::MACHINE_EPSILON_0)
    {
      mModel->mScrollPosition.y += scroll.y;
      ClampVerticalScroll(layoutSize);
    }

    if(mModel->mScrollPosition != currentScroll)
    {
      mEventData->mDecorator->UpdatePositions(mModel->mScrollPosition - currentScroll);
      RequestRelayout();
    }
  }
}

bool Controller::Impl::IsScrollable(const Vector2& displacement)
{
  bool isScrollable = false;
  if(mEventData)
  {
    const bool isHorizontalScrollEnabled = mEventData->mDecorator->IsHorizontalScrollEnabled();
    const bool isVerticalScrollEnabled   = mEventData->mDecorator->IsVerticalScrollEnabled();
    if(isHorizontalScrollEnabled || isVerticalScrollEnabled)
    {
      const Vector2& targetSize     = mModel->mVisualModel->mControlSize;
      const Vector2& layoutSize     = mModel->mVisualModel->GetLayoutSize();
      const Vector2& scrollPosition = mModel->mScrollPosition;

      if(isHorizontalScrollEnabled)
      {
        const float displacementX = displacement.x;
        const float positionX     = scrollPosition.x + displacementX;
        if(layoutSize.width > targetSize.width && -positionX > 0.f && -positionX < layoutSize.width - targetSize.width)
        {
          isScrollable = true;
        }
      }

      if(isVerticalScrollEnabled)
      {
        const float displacementY = displacement.y;
        const float positionY     = scrollPosition.y + displacementY;
        if(layoutSize.height > targetSize.height && -positionY > 0 && -positionY < layoutSize.height - targetSize.height)
        {
          isScrollable = true;
        }
      }
    }
  }
  return isScrollable;
}

float Controller::Impl::GetHorizontalScrollPosition()
{
  // Scroll values are negative internally so we convert them to positive numbers
  return mEventData ? -mModel->mScrollPosition.x : 0.0f;
}

float Controller::Impl::GetVerticalScrollPosition()
{
  // Scroll values are negative internally so we convert them to positive numbers
  return mEventData ? -mModel->mScrollPosition.y : 0.0f;
}

Toolkit::TextAnchor Controller::Impl::CreateAnchorActor(Anchor anchor)
{
  auto actor = Toolkit::TextAnchor::New();
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  auto rect = Toolkit::Internal::CommonTextUtils::GetTextBoundingRectangle(mModel, anchor.startIndex, anchor.endIndex - 1);
  actor.SetProperty(Actor::Property::POSITION, Vector2(rect.x, rect.y));
  actor.SetProperty(Actor::Property::SIZE, Vector2(rect.width, rect.height));

  std::string anchorText;
  std::string anchorHref               = anchor.href ? anchor.href : "";
  Length      numberOfAnchorCharacters = anchor.endIndex - anchor.startIndex;
  if(numberOfAnchorCharacters > 0u && mModel->mLogicalModel->mText.Size() >= numberOfAnchorCharacters)
  {
    Utf32ToUtf8(mModel->mLogicalModel->mText.Begin() + anchor.startIndex, numberOfAnchorCharacters, anchorText);
  }
  DALI_LOG_INFO(gLogFilter, Debug::General, "CreateAnchorActor NAME:%s, URI:%s\n", anchorText.c_str(), anchorHref.c_str());

  actor.SetProperty(Actor::Property::NAME, anchorText);
  actor.SetProperty(Toolkit::TextAnchor::Property::URI, anchorHref);
  actor.SetProperty(Toolkit::TextAnchor::Property::START_CHARACTER_INDEX, static_cast<int>(anchor.startIndex));
  actor.SetProperty(Toolkit::TextAnchor::Property::END_CHARACTER_INDEX, static_cast<int>(anchor.endIndex));
  return actor;
}

void Controller::Impl::GetAnchorActors(std::vector<Toolkit::TextAnchor>& anchorActors)
{
  /* TODO: Now actors are created/destroyed in every "RenderText" function call. Even when we add just 1 character,
           we need to create and destroy potentially many actors. Some optimization can be considered here.
           Maybe a "dirty" flag in mLogicalModel? */
  anchorActors.clear();
  for(auto& anchor : mModel->mLogicalModel->mAnchors)
  {
    auto actor = CreateAnchorActor(anchor);
    anchorActors.push_back(actor);
  }
}

int32_t Controller::Impl::GetAnchorIndex(size_t characterOffset) const
{
  Vector<Anchor>::Iterator it = mModel->mLogicalModel->mAnchors.Begin();

  while(it != mModel->mLogicalModel->mAnchors.End() && (it->startIndex > characterOffset || it->endIndex <= characterOffset))
  {
    it++;
  }

  return it == mModel->mLogicalModel->mAnchors.End() ? -1 : it - mModel->mLogicalModel->mAnchors.Begin();
}

void Controller::Impl::CopyUnderlinedFromLogicalToVisualModels(bool shouldClearPreUnderlineRuns)
{
  //Underlined character runs for markup-processor
  const Vector<UnderlinedCharacterRun>& underlinedCharacterRuns = mModel->mLogicalModel->mUnderlinedCharacterRuns;
  const Vector<GlyphIndex>&             charactersToGlyph       = mModel->mVisualModel->mCharactersToGlyph;
  const Vector<Length>&                 glyphsPerCharacter      = mModel->mVisualModel->mGlyphsPerCharacter;

  if(shouldClearPreUnderlineRuns)
  {
    mModel->mVisualModel->mUnderlineRuns.Clear();
  }

  for(Vector<UnderlinedCharacterRun>::ConstIterator it = underlinedCharacterRuns.Begin(), endIt = underlinedCharacterRuns.End(); it != endIt; ++it)
  {
    CharacterIndex characterIndex     = it->characterRun.characterIndex;
    Length         numberOfCharacters = it->characterRun.numberOfCharacters;

    if(numberOfCharacters == 0)
    {
      continue;
    }

    // Create one run for all glyphs of all run's characters that has same properties
    // This enhance performance and reduce the needed memory to store glyphs-runs
    UnderlinedGlyphRun underlineGlyphRun;
    underlineGlyphRun.glyphRun.glyphIndex     = charactersToGlyph[characterIndex];
    underlineGlyphRun.glyphRun.numberOfGlyphs = glyphsPerCharacter[characterIndex];
    //Copy properties (attributes)
    underlineGlyphRun.properties = it->properties;

    for(Length index = 1u; index < numberOfCharacters; index++)
    {
      underlineGlyphRun.glyphRun.numberOfGlyphs += glyphsPerCharacter[characterIndex + index];
    }

    mModel->mVisualModel->mUnderlineRuns.PushBack(underlineGlyphRun);
  }
}

void Controller::Impl::CopyStrikethroughFromLogicalToVisualModels()
{
  //Strikethrough character runs from markup-processor
  const Vector<StrikethroughCharacterRun>& strikethroughCharacterRuns = mModel->mLogicalModel->mStrikethroughCharacterRuns;
  const Vector<GlyphIndex>&                charactersToGlyph          = mModel->mVisualModel->mCharactersToGlyph;
  const Vector<Length>&                    glyphsPerCharacter         = mModel->mVisualModel->mGlyphsPerCharacter;

  mModel->mVisualModel->mStrikethroughRuns.Clear();

  for(Vector<StrikethroughCharacterRun>::ConstIterator it = strikethroughCharacterRuns.Begin(), endIt = strikethroughCharacterRuns.End(); it != endIt; ++it)
  {
    CharacterIndex characterIndex     = it->characterRun.characterIndex;
    Length         numberOfCharacters = it->characterRun.numberOfCharacters;

    if(numberOfCharacters == 0)
    {
      continue;
    }

    StrikethroughGlyphRun strikethroughGlyphRun;
    strikethroughGlyphRun.properties              = it->properties;
    strikethroughGlyphRun.glyphRun.glyphIndex     = charactersToGlyph[characterIndex];
    strikethroughGlyphRun.glyphRun.numberOfGlyphs = glyphsPerCharacter[characterIndex];

    for(Length index = 1u; index < numberOfCharacters; index++)
    {
      strikethroughGlyphRun.glyphRun.numberOfGlyphs += glyphsPerCharacter[characterIndex + index];
    }

    mModel->mVisualModel->mStrikethroughRuns.PushBack(strikethroughGlyphRun);
  }
}

void Controller::Impl::CopyCharacterSpacingFromLogicalToVisualModels()
{
  //CharacterSpacing character runs from markup-processor
  const Vector<CharacterSpacingCharacterRun>& characterSpacingCharacterRuns = mModel->mLogicalModel->mCharacterSpacingCharacterRuns;
  const Vector<GlyphIndex>&                   charactersToGlyph             = mModel->mVisualModel->mCharactersToGlyph;
  const Vector<Length>&                       glyphsPerCharacter            = mModel->mVisualModel->mGlyphsPerCharacter;

  mModel->mVisualModel->mCharacterSpacingRuns.Clear();

  for(Vector<CharacterSpacingCharacterRun>::ConstIterator it = characterSpacingCharacterRuns.Begin(), endIt = characterSpacingCharacterRuns.End(); it != endIt; ++it)
  {
    const CharacterIndex& characterIndex     = it->characterRun.characterIndex;
    const Length&         numberOfCharacters = it->characterRun.numberOfCharacters;

    if(numberOfCharacters == 0)
    {
      continue;
    }

    CharacterSpacingGlyphRun characterSpacingGlyphRun;
    characterSpacingGlyphRun.value                   = it->value;
    characterSpacingGlyphRun.glyphRun.glyphIndex     = charactersToGlyph[characterIndex];
    characterSpacingGlyphRun.glyphRun.numberOfGlyphs = glyphsPerCharacter[characterIndex];

    for(Length index = 1u; index < numberOfCharacters; index++)
    {
      characterSpacingGlyphRun.glyphRun.numberOfGlyphs += glyphsPerCharacter[characterIndex + index];
    }

    mModel->mVisualModel->mCharacterSpacingRuns.PushBack(characterSpacingGlyphRun);
  }
}

void Controller::Impl::SetAutoScrollEnabled(bool enable)
{
  if(mLayoutEngine.GetLayout() == Layout::Engine::SINGLE_LINE_BOX)
  {
    mOperationsPending = static_cast<OperationsMask>(mOperationsPending |
                                                     LAYOUT |
                                                     ALIGN |
                                                     UPDATE_LAYOUT_SIZE |
                                                     REORDER);

    if(enable)
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::SetAutoScrollEnabled for SINGLE_LINE_BOX\n");
      mOperationsPending = static_cast<OperationsMask>(mOperationsPending | UPDATE_DIRECTION);
    }
    else
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::SetAutoScrollEnabled Disabling autoscroll\n");
    }

    mIsAutoScrollEnabled = enable;
    RequestRelayout();
  }
  else
  {
    DALI_LOG_WARNING("Attempted AutoScrolling on a non SINGLE_LINE_BOX, request ignored\n");
    mIsAutoScrollEnabled = false;
  }
}

void Controller::Impl::SetEnableCursorBlink(bool enable)
{
  DALI_ASSERT_DEBUG(NULL != mEventData && "TextInput disabled");

  if(mEventData)
  {
    mEventData->mCursorBlinkEnabled = enable;

    if(!enable && mEventData->mDecorator)
    {
      mEventData->mDecorator->StopCursorBlink();
    }
  }
}

void Controller::Impl::SetMultiLineEnabled(bool enable)
{
  const Layout::Engine::Type layout = enable ? Layout::Engine::MULTI_LINE_BOX : Layout::Engine::SINGLE_LINE_BOX;

  if(layout != mLayoutEngine.GetLayout())
  {
    // Set the layout type.
    mLayoutEngine.SetLayout(layout);

    // Set the flags to redo the layout operations
    const OperationsMask layoutOperations = static_cast<OperationsMask>(LAYOUT |
                                                                        UPDATE_LAYOUT_SIZE |
                                                                        ALIGN |
                                                                        REORDER);

    mTextUpdateInfo.mFullRelayoutNeeded = true;
    mOperationsPending                  = static_cast<OperationsMask>(mOperationsPending | layoutOperations);

    // Need to recalculate natural size
    mRecalculateNaturalSize = true;

    RequestRelayout();
  }
}

void Controller::Impl::SetHorizontalAlignment(Text::HorizontalAlignment::Type alignment)
{
  if(alignment != mModel->mHorizontalAlignment)
  {
    // Set the alignment.
    mModel->mHorizontalAlignment = alignment;
    UpdateCursorPositionForAlignment(*this, true);
    RequestRelayout();
  }
}

void Controller::Impl::SetVerticalAlignment(VerticalAlignment::Type alignment)
{
  if(alignment != mModel->mVerticalAlignment)
  {
    // Set the alignment.
    mModel->mVerticalAlignment = alignment;
    UpdateCursorPositionForAlignment(*this, false);
    RequestRelayout();
  }
}

void Controller::Impl::SetLineWrapMode(Text::LineWrap::Mode lineWrapMode)
{
  if(lineWrapMode != mModel->mLineWrapMode)
  {
    // Update Text layout for applying wrap mode
    mOperationsPending = static_cast<OperationsMask>(mOperationsPending |
                                                     ALIGN |
                                                     LAYOUT |
                                                     UPDATE_LAYOUT_SIZE |
                                                     REORDER);

    if((mModel->mLineWrapMode == (Text::LineWrap::Mode)DevelText::LineWrap::HYPHENATION) || (lineWrapMode == (Text::LineWrap::Mode)DevelText::LineWrap::HYPHENATION) ||
       (mModel->mLineWrapMode == (Text::LineWrap::Mode)DevelText::LineWrap::MIXED) || (lineWrapMode == (Text::LineWrap::Mode)DevelText::LineWrap::MIXED)) // hyphen is treated as line break
    {
      mOperationsPending = static_cast<OperationsMask>(mOperationsPending | GET_LINE_BREAKS);
    }

    // Set the text wrap mode.
    mModel->mLineWrapMode = lineWrapMode;

    mTextUpdateInfo.mCharacterIndex             = 0u;
    mTextUpdateInfo.mNumberOfCharactersToRemove = mTextUpdateInfo.mPreviousNumberOfCharacters;
    mTextUpdateInfo.mNumberOfCharactersToAdd    = mModel->mLogicalModel->mText.Count();

    // Request relayout
    RequestRelayout();
  }
}

void Controller::Impl::SetDefaultColor(const Vector4& color)
{
  mTextColor = color;

  if(!IsShowingPlaceholderText())
  {
    mModel->mVisualModel->SetTextColor(color);
    mOperationsPending = static_cast<OperationsMask>(mOperationsPending | COLOR);
    RequestRelayout();
  }
}

void Controller::Impl::SetUserInteractionEnabled(bool enabled)
{
  mIsUserInteractionEnabled = enabled;

  if(mEventData && mEventData->mDecorator)
  {
    bool editable = mEventData->mEditingEnabled && enabled;
    mEventData->mDecorator->SetEditable(editable);
  }
}

void Controller::Impl::ClearFontData()
{
  if(mFontDefaults)
  {
    mFontDefaults->mFontId = 0u; // Remove old font ID
  }

  // Set flags to update the model.
  mTextUpdateInfo.mCharacterIndex             = 0u;
  mTextUpdateInfo.mNumberOfCharactersToRemove = mTextUpdateInfo.mPreviousNumberOfCharacters;
  mTextUpdateInfo.mNumberOfCharactersToAdd    = mModel->mLogicalModel->mText.Count();

  mTextUpdateInfo.mClearAll           = true;
  mTextUpdateInfo.mFullRelayoutNeeded = true;
  mRecalculateNaturalSize             = true;

  mOperationsPending = static_cast<OperationsMask>(mOperationsPending |
                                                   VALIDATE_FONTS |
                                                   SHAPE_TEXT |
                                                   BIDI_INFO |
                                                   GET_GLYPH_METRICS |
                                                   LAYOUT |
                                                   UPDATE_LAYOUT_SIZE |
                                                   REORDER |
                                                   ALIGN);
}

void Controller::Impl::ClearStyleData()
{
  mModel->mLogicalModel->mColorRuns.Clear();
  mModel->mLogicalModel->mBackgroundColorRuns.Clear();
  mModel->mLogicalModel->ClearFontDescriptionRuns();
  mModel->mLogicalModel->ClearStrikethroughRuns();
  mModel->mLogicalModel->ClearUnderlineRuns();
}

void Controller::Impl::ResetScrollPosition()
{
  if(mEventData)
  {
    // Reset the scroll position.
    mModel->mScrollPosition                = Vector2::ZERO;
    mEventData->mScrollAfterUpdatePosition = true;
  }
}

} // namespace Dali::Toolkit::Text
