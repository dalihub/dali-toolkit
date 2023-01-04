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
#include <dali-toolkit/internal/text/text-controller.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/tts-player.h>
#include <memory.h>
#include <cmath>
#include <limits>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/layouts/layout-parameters.h>
#include <dali-toolkit/internal/text/markup-processor.h>
#include <dali-toolkit/internal/text/text-controller-event-handler.h>
#include <dali-toolkit/internal/text/text-controller-impl.h>
#include <dali-toolkit/internal/text/text-controller-input-font-handler.h>
#include <dali-toolkit/internal/text/text-controller-placeholder-handler.h>
#include <dali-toolkit/internal/text/text-editable-control-interface.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

const float MAX_FLOAT = std::numeric_limits<float>::max();

const std::string EMPTY_STRING("");

float ConvertToEven(float value)
{
  int intValue(static_cast<int>(value));
  return static_cast<float>(intValue + (intValue & 1));
}

int ConvertPixelToPint(float pixel)
{
  unsigned int                      horizontalDpi = 0u;
  unsigned int                      verticalDpi   = 0u;
  Dali::TextAbstraction::FontClient fontClient    = Dali::TextAbstraction::FontClient::Get();
  fontClient.GetDpi(horizontalDpi, verticalDpi);

  return (pixel * 72.f) / static_cast<float>(horizontalDpi);
}

} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Text
{
// public : Constructor.

ControllerPtr Controller::New()
{
  return ControllerPtr(new Controller());
}

ControllerPtr Controller::New(ControlInterface* controlInterface)
{
  return ControllerPtr(new Controller(controlInterface));
}

ControllerPtr Controller::New(ControlInterface*           controlInterface,
                              EditableControlInterface*   editableControlInterface,
                              SelectableControlInterface* selectableControlInterface)
{
  return ControllerPtr(new Controller(controlInterface,
                                      editableControlInterface,
                                      selectableControlInterface));
}

// public : Configure the text controller.

void Controller::EnableTextInput(DecoratorPtr decorator, InputMethodContext& inputMethodContext)
{
  if(!decorator)
  {
    delete mImpl->mEventData;
    mImpl->mEventData = NULL;

    // Nothing else to do.
    return;
  }

  if(NULL == mImpl->mEventData)
  {
    mImpl->mEventData = new EventData(decorator, inputMethodContext);
  }
}

void Controller::SetGlyphType(TextAbstraction::GlyphType glyphType)
{
  // Metrics for bitmap & vector based glyphs are different
  mImpl->mMetrics->SetGlyphType(glyphType);

  // Clear the font-specific data
  ClearFontData();

  mImpl->RequestRelayout();
}

void Controller::SetMarkupProcessorEnabled(bool enable)
{
  if(enable != mImpl->mMarkupProcessorEnabled)
  {
    //If Text was already set, call the SetText again for enabling or disabling markup
    mImpl->mMarkupProcessorEnabled = enable;
    std::string text;
    GetText(text);
    SetText(text);
  }
}

bool Controller::IsMarkupProcessorEnabled() const
{
  return mImpl->mMarkupProcessorEnabled;
}

void Controller::SetAutoScrollEnabled(bool enable)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::SetAutoScrollEnabled[%s] SingleBox[%s]-> [%p]\n", (enable) ? "true" : "false", (mImpl->mLayoutEngine.GetLayout() == Layout::Engine::SINGLE_LINE_BOX) ? "true" : "false", this);

  if(mImpl->mLayoutEngine.GetLayout() == Layout::Engine::SINGLE_LINE_BOX)
  {
    if(enable)
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::SetAutoScrollEnabled for SINGLE_LINE_BOX\n");
      mImpl->mOperationsPending = static_cast<OperationsMask>(mImpl->mOperationsPending |
                                                              LAYOUT |
                                                              ALIGN |
                                                              UPDATE_LAYOUT_SIZE |
                                                              UPDATE_DIRECTION |
                                                              REORDER);
    }
    else
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::SetAutoScrollEnabled Disabling autoscroll\n");
      mImpl->mOperationsPending = static_cast<OperationsMask>(mImpl->mOperationsPending |
                                                              LAYOUT |
                                                              ALIGN |
                                                              UPDATE_LAYOUT_SIZE |
                                                              REORDER);
    }

    mImpl->mIsAutoScrollEnabled = enable;
    mImpl->RequestRelayout();
  }
  else
  {
    DALI_LOG_WARNING("Attempted AutoScrolling on a non SINGLE_LINE_BOX, request ignored\n");
    mImpl->mIsAutoScrollEnabled = false;
  }
}

bool Controller::IsAutoScrollEnabled() const
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::IsAutoScrollEnabled[%s]\n", mImpl->mIsAutoScrollEnabled ? "true" : "false");

  return mImpl->mIsAutoScrollEnabled;
}

CharacterDirection Controller::GetAutoScrollDirection() const
{
  return mImpl->mIsTextDirectionRTL;
}

float Controller::GetAutoScrollLineAlignment() const
{
  float offset = 0.f;

  if(mImpl->mModel->mVisualModel &&
     (0u != mImpl->mModel->mVisualModel->mLines.Count()))
  {
    offset = (*mImpl->mModel->mVisualModel->mLines.Begin()).alignmentOffset;
  }

  return offset;
}

void Controller::SetHorizontalScrollEnabled(bool enable)
{
  if((NULL != mImpl->mEventData) &&
     mImpl->mEventData->mDecorator)
  {
    mImpl->mEventData->mDecorator->SetHorizontalScrollEnabled(enable);
  }
}
bool Controller::IsHorizontalScrollEnabled() const
{
  if((NULL != mImpl->mEventData) &&
     mImpl->mEventData->mDecorator)
  {
    return mImpl->mEventData->mDecorator->IsHorizontalScrollEnabled();
  }

  return false;
}

void Controller::SetVerticalScrollEnabled(bool enable)
{
  if((NULL != mImpl->mEventData) &&
     mImpl->mEventData->mDecorator)
  {
    if(mImpl->mEventData->mDecorator)
    {
      mImpl->mEventData->mDecorator->SetVerticalScrollEnabled(enable);
    }
  }
}

bool Controller::IsVerticalScrollEnabled() const
{
  if((NULL != mImpl->mEventData) &&
     mImpl->mEventData->mDecorator)
  {
    return mImpl->mEventData->mDecorator->IsVerticalScrollEnabled();
  }

  return false;
}

void Controller::SetSmoothHandlePanEnabled(bool enable)
{
  if((NULL != mImpl->mEventData) &&
     mImpl->mEventData->mDecorator)
  {
    mImpl->mEventData->mDecorator->SetSmoothHandlePanEnabled(enable);
  }
}

bool Controller::IsSmoothHandlePanEnabled() const
{
  if((NULL != mImpl->mEventData) &&
     mImpl->mEventData->mDecorator)
  {
    return mImpl->mEventData->mDecorator->IsSmoothHandlePanEnabled();
  }

  return false;
}

void Controller::SetMaximumNumberOfCharacters(Length maxCharacters)
{
  mImpl->mMaximumNumberOfCharacters = maxCharacters;
}

int Controller::GetMaximumNumberOfCharacters()
{
  return mImpl->mMaximumNumberOfCharacters;
}

void Controller::SetEnableCursorBlink(bool enable)
{
  DALI_ASSERT_DEBUG(NULL != mImpl->mEventData && "TextInput disabled");

  if(NULL != mImpl->mEventData)
  {
    mImpl->mEventData->mCursorBlinkEnabled = enable;

    if(!enable &&
       mImpl->mEventData->mDecorator)
    {
      mImpl->mEventData->mDecorator->StopCursorBlink();
    }
  }
}

bool Controller::GetEnableCursorBlink() const
{
  if(NULL != mImpl->mEventData)
  {
    return mImpl->mEventData->mCursorBlinkEnabled;
  }

  return false;
}

void Controller::SetMultiLineEnabled(bool enable)
{
  const Layout::Engine::Type layout = enable ? Layout::Engine::MULTI_LINE_BOX : Layout::Engine::SINGLE_LINE_BOX;

  if(layout != mImpl->mLayoutEngine.GetLayout())
  {
    // Set the layout type.
    mImpl->mLayoutEngine.SetLayout(layout);

    // Set the flags to redo the layout operations
    const OperationsMask layoutOperations = static_cast<OperationsMask>(LAYOUT |
                                                                        UPDATE_LAYOUT_SIZE |
                                                                        ALIGN |
                                                                        REORDER);

    mImpl->mTextUpdateInfo.mFullRelayoutNeeded = true;
    mImpl->mOperationsPending                  = static_cast<OperationsMask>(mImpl->mOperationsPending | layoutOperations);

    // Need to recalculate natural size
    mImpl->mRecalculateNaturalSize = true;

    mImpl->RequestRelayout();
  }
}

bool Controller::IsMultiLineEnabled() const
{
  return Layout::Engine::MULTI_LINE_BOX == mImpl->mLayoutEngine.GetLayout();
}

void Controller::SetHorizontalAlignment(Text::HorizontalAlignment::Type alignment)
{
  if(alignment != mImpl->mModel->mHorizontalAlignment)
  {
    // Set the alignment.
    mImpl->mModel->mHorizontalAlignment = alignment;

    // Set the flag to redo the alignment operation.
    mImpl->mOperationsPending = static_cast<OperationsMask>(mImpl->mOperationsPending | ALIGN);

    if(mImpl->mEventData)
    {
      mImpl->mEventData->mUpdateAlignment = true;

      // Update the cursor if it's in editing mode
      if(EventData::IsEditingState(mImpl->mEventData->mState))
      {
        mImpl->ChangeState(EventData::EDITING);
        mImpl->mEventData->mUpdateCursorPosition = true;
      }
    }

    mImpl->RequestRelayout();
  }
}

Text::HorizontalAlignment::Type Controller::GetHorizontalAlignment() const
{
  return mImpl->mModel->mHorizontalAlignment;
}

void Controller::SetVerticalAlignment(VerticalAlignment::Type alignment)
{
  if(alignment != mImpl->mModel->mVerticalAlignment)
  {
    // Set the alignment.
    mImpl->mModel->mVerticalAlignment = alignment;

    mImpl->mOperationsPending = static_cast<OperationsMask>(mImpl->mOperationsPending | ALIGN);

    mImpl->RequestRelayout();
  }
}

VerticalAlignment::Type Controller::GetVerticalAlignment() const
{
  return mImpl->mModel->mVerticalAlignment;
}

bool Controller::IsIgnoreSpacesAfterText() const
{
  return mImpl->mModel->mIgnoreSpacesAfterText;
}

void Controller::SetIgnoreSpacesAfterText(bool ignore)
{
  mImpl->mModel->mIgnoreSpacesAfterText = ignore;
}

bool Controller::IsMatchSystemLanguageDirection() const
{
  return mImpl->mModel->mMatchSystemLanguageDirection;
}

void Controller::SetMatchSystemLanguageDirection(bool match)
{
  mImpl->mModel->mMatchSystemLanguageDirection = match;
}

void Controller::SetLayoutDirection(Dali::LayoutDirection::Type layoutDirection)
{
  mImpl->mLayoutDirection = layoutDirection;
}

bool Controller::IsShowingRealText() const
{
  return mImpl->IsShowingRealText();
}

void Controller::SetLineWrapMode(Text::LineWrap::Mode lineWrapMode)
{
  if(lineWrapMode != mImpl->mModel->mLineWrapMode)
  {
    // Set the text wrap mode.
    mImpl->mModel->mLineWrapMode = lineWrapMode;

    // Update Text layout for applying wrap mode
    mImpl->mOperationsPending                          = static_cast<OperationsMask>(mImpl->mOperationsPending |
                                                            ALIGN |
                                                            LAYOUT |
                                                            UPDATE_LAYOUT_SIZE |
                                                            REORDER);
    mImpl->mTextUpdateInfo.mCharacterIndex             = 0u;
    mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;
    mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd    = mImpl->mModel->mLogicalModel->mText.Count();

    // Request relayout
    mImpl->RequestRelayout();
  }
}

Text::LineWrap::Mode Controller::GetLineWrapMode() const
{
  return mImpl->mModel->mLineWrapMode;
}

void Controller::SetTextElideEnabled(bool enabled)
{
  mImpl->mModel->mElideEnabled = enabled;
}

bool Controller::IsTextElideEnabled() const
{
  return mImpl->mModel->mElideEnabled;
}

void Controller::SetTextFitEnabled(bool enabled)
{
  mImpl->mTextFitEnabled = enabled;
}

bool Controller::IsTextFitEnabled() const
{
  return mImpl->mTextFitEnabled;
}

void Controller::SetTextFitMinSize(float minSize, FontSizeType type)
{
  switch(type)
  {
    case POINT_SIZE:
    {
      mImpl->mTextFitMinSize = minSize;
      break;
    }
    case PIXEL_SIZE:
    {
      mImpl->mTextFitMinSize = ConvertPixelToPint(minSize);
      break;
    }
  }
}

float Controller::GetTextFitMinSize() const
{
  return mImpl->mTextFitMinSize;
}

void Controller::SetTextFitMaxSize(float maxSize, FontSizeType type)
{
  switch(type)
  {
    case POINT_SIZE:
    {
      mImpl->mTextFitMaxSize = maxSize;
      break;
    }
    case PIXEL_SIZE:
    {
      mImpl->mTextFitMaxSize = ConvertPixelToPint(maxSize);
      break;
    }
  }
}

float Controller::GetTextFitMaxSize() const
{
  return mImpl->mTextFitMaxSize;
}

void Controller::SetTextFitStepSize(float step, FontSizeType type)
{
  switch(type)
  {
    case POINT_SIZE:
    {
      mImpl->mTextFitStepSize = step;
      break;
    }
    case PIXEL_SIZE:
    {
      mImpl->mTextFitStepSize = ConvertPixelToPint(step);
      break;
    }
  }
}

float Controller::GetTextFitStepSize() const
{
  return mImpl->mTextFitStepSize;
}

void Controller::SetTextFitContentSize(Vector2 size)
{
  mImpl->mTextFitContentSize = size;
}

Vector2 Controller::GetTextFitContentSize() const
{
  return mImpl->mTextFitContentSize;
}

void Controller::SetPlaceholderTextElideEnabled(bool enabled)
{
  PlaceholderHandler::SetPlaceholderTextElideEnabled(*this, enabled);
}

bool Controller::IsPlaceholderTextElideEnabled() const
{
  return PlaceholderHandler::IsPlaceholderTextElideEnabled(*this);
}

void Controller::SetSelectionEnabled(bool enabled)
{
  mImpl->mEventData->mSelectionEnabled = enabled;
}

bool Controller::IsSelectionEnabled() const
{
  return mImpl->mEventData->mSelectionEnabled;
}

void Controller::SetShiftSelectionEnabled(bool enabled)
{
  mImpl->mEventData->mShiftSelectionFlag = enabled;
}

bool Controller::IsShiftSelectionEnabled() const
{
  return mImpl->mEventData->mShiftSelectionFlag;
}

void Controller::SetGrabHandleEnabled(bool enabled)
{
  mImpl->mEventData->mGrabHandleEnabled = enabled;
}

bool Controller::IsGrabHandleEnabled() const
{
  return mImpl->mEventData->mGrabHandleEnabled;
}

void Controller::SetGrabHandlePopupEnabled(bool enabled)
{
  mImpl->mEventData->mGrabHandlePopupEnabled = enabled;
}

bool Controller::IsGrabHandlePopupEnabled() const
{
  return mImpl->mEventData->mGrabHandlePopupEnabled;
}

// public : Update

void Controller::SetText(const std::string& text)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::SetText\n");

  // Reset keyboard as text changed
  mImpl->ResetInputMethodContext();

  // Remove the previously set text and style.
  ResetText();

  // Remove the style.
  ClearStyleData();

  CharacterIndex lastCursorIndex = 0u;

  if(NULL != mImpl->mEventData)
  {
    // If popup shown then hide it by switching to Editing state
    if((EventData::SELECTING == mImpl->mEventData->mState) ||
       (EventData::EDITING_WITH_POPUP == mImpl->mEventData->mState) ||
       (EventData::EDITING_WITH_GRAB_HANDLE == mImpl->mEventData->mState) ||
       (EventData::EDITING_WITH_PASTE_POPUP == mImpl->mEventData->mState))
    {
      mImpl->ChangeState(EventData::EDITING);
    }
  }

  if(!text.empty())
  {
    mImpl->mModel->mVisualModel->SetTextColor(mImpl->mTextColor);

    MarkupProcessData markupProcessData(mImpl->mModel->mLogicalModel->mColorRuns,
                                        mImpl->mModel->mLogicalModel->mFontDescriptionRuns,
                                        mImpl->mModel->mLogicalModel->mEmbeddedItems);

    Length         textSize = 0u;
    const uint8_t* utf8     = NULL;
    if(mImpl->mMarkupProcessorEnabled)
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
    Vector<Character>& utf32Characters = mImpl->mModel->mLogicalModel->mText;
    utf32Characters.Resize(textSize);

    // Transform a text array encoded in utf8 into an array encoded in utf32.
    // It returns the actual number of characters.
    Length characterCount = Utf8ToUtf32(utf8, textSize, utf32Characters.Begin());
    utf32Characters.Resize(characterCount);

    DALI_ASSERT_DEBUG(textSize >= characterCount && "Invalid UTF32 conversion length");
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::SetText %p UTF8 size %d, UTF32 size %d\n", this, textSize, mImpl->mModel->mLogicalModel->mText.Count());

    // The characters to be added.
    mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd = mImpl->mModel->mLogicalModel->mText.Count();

    // To reset the cursor position
    lastCursorIndex = characterCount;

    // Update the rest of the model during size negotiation
    mImpl->QueueModifyEvent(ModifyEvent::TEXT_REPLACED);

    // The natural size needs to be re-calculated.
    mImpl->mRecalculateNaturalSize = true;

    // The text direction needs to be updated.
    mImpl->mUpdateTextDirection = true;

    // Apply modifications to the model
    mImpl->mOperationsPending = ALL_OPERATIONS;
  }
  else
  {
    ShowPlaceholderText();
  }

  // Resets the cursor position.
  ResetCursorPosition(lastCursorIndex);

  // Scrolls the text to make the cursor visible.
  ResetScrollPosition();

  mImpl->RequestRelayout();

  if(NULL != mImpl->mEventData)
  {
    // Cancel previously queued events
    mImpl->mEventData->mEventQueue.clear();
  }

  // Do this last since it provides callbacks into application code.
  if(NULL != mImpl->mEditableControlInterface)
  {
    mImpl->mEditableControlInterface->TextChanged(true);
  }
}

void Controller::GetText(std::string& text) const
{
  if(!mImpl->IsShowingPlaceholderText())
  {
    // Retrieves the text string.
    mImpl->GetText(0u, text);
  }
  else
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::GetText %p empty (but showing placeholder)\n", this);
  }
}

void Controller::SetPlaceholderText(PlaceholderType type, const std::string& text)
{
  PlaceholderHandler::SetPlaceholderText(*this, type, text);
}

void Controller::GetPlaceholderText(PlaceholderType type, std::string& text) const
{
  PlaceholderHandler::GetPlaceholderText(*this, type, text);
}

void Controller::UpdateAfterFontChange(const std::string& newDefaultFont)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::UpdateAfterFontChange\n");

  if(!mImpl->mFontDefaults->familyDefined) // If user defined font then should not update when system font changes
  {
    DALI_LOG_INFO(gLogFilter, Debug::Concise, "Controller::UpdateAfterFontChange newDefaultFont(%s)\n", newDefaultFont.c_str());
    mImpl->mFontDefaults->mFontDescription.family = newDefaultFont;

    ClearFontData();

    mImpl->RequestRelayout();
  }
}

// public : Default style & Input style

void Controller::SetDefaultFontFamily(const std::string& defaultFontFamily)
{
  if(NULL == mImpl->mFontDefaults)
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  mImpl->mFontDefaults->mFontDescription.family = defaultFontFamily;
  DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::SetDefaultFontFamily %s\n", defaultFontFamily.c_str());
  mImpl->mFontDefaults->familyDefined = !defaultFontFamily.empty();

  if(mImpl->mEventData)
  {
    // Update the cursor position if it's in editing mode
    if(EventData::IsEditingState(mImpl->mEventData->mState))
    {
      mImpl->mEventData->mDecoratorUpdated     = true;
      mImpl->mEventData->mUpdateCursorPosition = true; // Cursor position should be updated when the font family is updated.
    }
  }

  // Clear the font-specific data
  ClearFontData();

  mImpl->RequestRelayout();
}

const std::string& Controller::GetDefaultFontFamily() const
{
  if(NULL != mImpl->mFontDefaults)
  {
    return mImpl->mFontDefaults->mFontDescription.family;
  }

  return EMPTY_STRING;
}

void Controller::SetPlaceholderFontFamily(const std::string& placeholderTextFontFamily)
{
  PlaceholderHandler::SetPlaceholderFontFamily(*this, placeholderTextFontFamily);
}

const std::string& Controller::GetPlaceholderFontFamily() const
{
  return PlaceholderHandler::GetPlaceholderFontFamily(*this);
}

void Controller::SetDefaultFontWeight(FontWeight weight)
{
  if(NULL == mImpl->mFontDefaults)
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  mImpl->mFontDefaults->mFontDescription.weight = weight;
  mImpl->mFontDefaults->weightDefined           = true;

  if(mImpl->mEventData)
  {
    // Update the cursor position if it's in editing mode
    if(EventData::IsEditingState(mImpl->mEventData->mState))
    {
      mImpl->mEventData->mDecoratorUpdated     = true;
      mImpl->mEventData->mUpdateCursorPosition = true; // Cursor position should be updated when the font weight is updated.
    }
  }

  // Clear the font-specific data
  ClearFontData();

  mImpl->RequestRelayout();
}

bool Controller::IsDefaultFontWeightDefined() const
{
  if(NULL != mImpl->mFontDefaults)
  {
    return mImpl->mFontDefaults->weightDefined;
  }

  return false;
}

FontWeight Controller::GetDefaultFontWeight() const
{
  if(NULL != mImpl->mFontDefaults)
  {
    return mImpl->mFontDefaults->mFontDescription.weight;
  }

  return TextAbstraction::FontWeight::NORMAL;
}

void Controller::SetPlaceholderTextFontWeight(FontWeight weight)
{
  PlaceholderHandler::SetPlaceholderTextFontWeight(*this, weight);
}

bool Controller::IsPlaceholderTextFontWeightDefined() const
{
  return PlaceholderHandler::IsPlaceholderTextFontWeightDefined(*this);
  ;
}

FontWeight Controller::GetPlaceholderTextFontWeight() const
{
  return PlaceholderHandler::GetPlaceholderTextFontWeight(*this);
}

void Controller::SetDefaultFontWidth(FontWidth width)
{
  if(NULL == mImpl->mFontDefaults)
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  mImpl->mFontDefaults->mFontDescription.width = width;
  mImpl->mFontDefaults->widthDefined           = true;

  if(mImpl->mEventData)
  {
    // Update the cursor position if it's in editing mode
    if(EventData::IsEditingState(mImpl->mEventData->mState))
    {
      mImpl->mEventData->mDecoratorUpdated     = true;
      mImpl->mEventData->mUpdateCursorPosition = true; // Cursor position should be updated when the font width is updated.
    }
  }

  // Clear the font-specific data
  ClearFontData();

  mImpl->RequestRelayout();
}

bool Controller::IsDefaultFontWidthDefined() const
{
  if(NULL != mImpl->mFontDefaults)
  {
    return mImpl->mFontDefaults->widthDefined;
  }

  return false;
}

FontWidth Controller::GetDefaultFontWidth() const
{
  if(NULL != mImpl->mFontDefaults)
  {
    return mImpl->mFontDefaults->mFontDescription.width;
  }

  return TextAbstraction::FontWidth::NORMAL;
}

void Controller::SetPlaceholderTextFontWidth(FontWidth width)
{
  PlaceholderHandler::SetPlaceholderTextFontWidth(*this, width);
}

bool Controller::IsPlaceholderTextFontWidthDefined() const
{
  return PlaceholderHandler::IsPlaceholderTextFontWidthDefined(*this);
}

FontWidth Controller::GetPlaceholderTextFontWidth() const
{
  return PlaceholderHandler::GetPlaceholderTextFontWidth(*this);
}

void Controller::SetDefaultFontSlant(FontSlant slant)
{
  if(NULL == mImpl->mFontDefaults)
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  mImpl->mFontDefaults->mFontDescription.slant = slant;
  mImpl->mFontDefaults->slantDefined           = true;

  if(mImpl->mEventData)
  {
    // Update the cursor position if it's in editing mode
    if(EventData::IsEditingState(mImpl->mEventData->mState))
    {
      mImpl->mEventData->mDecoratorUpdated     = true;
      mImpl->mEventData->mUpdateCursorPosition = true; // Cursor position should be updated when the font slant is updated.
    }
  }

  // Clear the font-specific data
  ClearFontData();

  mImpl->RequestRelayout();
}

bool Controller::IsDefaultFontSlantDefined() const
{
  if(NULL != mImpl->mFontDefaults)
  {
    return mImpl->mFontDefaults->slantDefined;
  }
  return false;
}

FontSlant Controller::GetDefaultFontSlant() const
{
  if(NULL != mImpl->mFontDefaults)
  {
    return mImpl->mFontDefaults->mFontDescription.slant;
  }

  return TextAbstraction::FontSlant::NORMAL;
}

void Controller::SetPlaceholderTextFontSlant(FontSlant slant)
{
  PlaceholderHandler::SetPlaceholderTextFontSlant(*this, slant);
}

bool Controller::IsPlaceholderTextFontSlantDefined() const
{
  return PlaceholderHandler::IsPlaceholderTextFontSlantDefined(*this);
}

FontSlant Controller::GetPlaceholderTextFontSlant() const
{
  return PlaceholderHandler::GetPlaceholderTextFontSlant(*this);
}

void Controller::SetDefaultFontSize(float fontSize, FontSizeType type)
{
  if(NULL == mImpl->mFontDefaults)
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  switch(type)
  {
    case POINT_SIZE:
    {
      mImpl->mFontDefaults->mDefaultPointSize = fontSize;
      mImpl->mFontDefaults->sizeDefined       = true;
      break;
    }
    case PIXEL_SIZE:
    {
      // Point size = Pixel size * 72.f / DPI
      unsigned int                horizontalDpi = 0u;
      unsigned int                verticalDpi   = 0u;
      TextAbstraction::FontClient fontClient    = TextAbstraction::FontClient::Get();
      fontClient.GetDpi(horizontalDpi, verticalDpi);

      mImpl->mFontDefaults->mDefaultPointSize = (fontSize * 72.f) / static_cast<float>(horizontalDpi);
      mImpl->mFontDefaults->sizeDefined       = true;
      break;
    }
  }

  if(mImpl->mEventData)
  {
    // Update the cursor position if it's in editing mode
    if(EventData::IsEditingState(mImpl->mEventData->mState))
    {
      mImpl->mEventData->mDecoratorUpdated     = true;
      mImpl->mEventData->mUpdateCursorPosition = true; // Cursor position should be updated when the font size is updated.
    }
  }

  // Clear the font-specific data
  ClearFontData();

  mImpl->RequestRelayout();
}

float Controller::GetDefaultFontSize(FontSizeType type) const
{
  float value = 0.0f;
  if(NULL != mImpl->mFontDefaults)
  {
    switch(type)
    {
      case POINT_SIZE:
      {
        value = mImpl->mFontDefaults->mDefaultPointSize;
        break;
      }
      case PIXEL_SIZE:
      {
        // Pixel size = Point size * DPI / 72.f
        unsigned int                horizontalDpi = 0u;
        unsigned int                verticalDpi   = 0u;
        TextAbstraction::FontClient fontClient    = TextAbstraction::FontClient::Get();
        fontClient.GetDpi(horizontalDpi, verticalDpi);

        value = mImpl->mFontDefaults->mDefaultPointSize * static_cast<float>(horizontalDpi) / 72.f;
        break;
      }
    }
    return value;
  }

  return value;
}

void Controller::SetPlaceholderTextFontSize(float fontSize, FontSizeType type)
{
  PlaceholderHandler::SetPlaceholderTextFontSize(*this, fontSize, type);
}

float Controller::GetPlaceholderTextFontSize(FontSizeType type) const
{
  return PlaceholderHandler::GetPlaceholderTextFontSize(*this, type);
}

void Controller::SetDefaultColor(const Vector4& color)
{
  mImpl->mTextColor = color;

  if(!mImpl->IsShowingPlaceholderText())
  {
    mImpl->mModel->mVisualModel->SetTextColor(color);

    mImpl->mModel->mLogicalModel->mColorRuns.Clear();

    mImpl->mOperationsPending = static_cast<OperationsMask>(mImpl->mOperationsPending | COLOR);

    mImpl->RequestRelayout();
  }
}

const Vector4& Controller::GetDefaultColor() const
{
  return mImpl->mTextColor;
}

void Controller::SetPlaceholderTextColor(const Vector4& textColor)
{
  PlaceholderHandler::SetPlaceholderTextColor(*this, textColor);
}

const Vector4& Controller::GetPlaceholderTextColor() const
{
  return PlaceholderHandler::GetPlaceholderTextColor(*this);
}

void Controller::SetShadowOffset(const Vector2& shadowOffset)
{
  mImpl->mModel->mVisualModel->SetShadowOffset(shadowOffset);

  mImpl->RequestRelayout();
}

const Vector2& Controller::GetShadowOffset() const
{
  return mImpl->mModel->mVisualModel->GetShadowOffset();
}

void Controller::SetShadowColor(const Vector4& shadowColor)
{
  mImpl->mModel->mVisualModel->SetShadowColor(shadowColor);

  mImpl->RequestRelayout();
}

const Vector4& Controller::GetShadowColor() const
{
  return mImpl->mModel->mVisualModel->GetShadowColor();
}

void Controller::SetShadowBlurRadius(const float& shadowBlurRadius)
{
  if(fabsf(GetShadowBlurRadius() - shadowBlurRadius) > Math::MACHINE_EPSILON_1)
  {
    mImpl->mModel->mVisualModel->SetShadowBlurRadius(shadowBlurRadius);

    mImpl->RequestRelayout();
  }
}

const float& Controller::GetShadowBlurRadius() const
{
  return mImpl->mModel->mVisualModel->GetShadowBlurRadius();
}

void Controller::SetUnderlineColor(const Vector4& color)
{
  mImpl->mModel->mVisualModel->SetUnderlineColor(color);

  mImpl->RequestRelayout();
}

const Vector4& Controller::GetUnderlineColor() const
{
  return mImpl->mModel->mVisualModel->GetUnderlineColor();
}

void Controller::SetUnderlineEnabled(bool enabled)
{
  mImpl->mModel->mVisualModel->SetUnderlineEnabled(enabled);

  mImpl->RequestRelayout();
}

bool Controller::IsUnderlineEnabled() const
{
  return mImpl->mModel->mVisualModel->IsUnderlineEnabled();
}

void Controller::SetUnderlineHeight(float height)
{
  mImpl->mModel->mVisualModel->SetUnderlineHeight(height);

  mImpl->RequestRelayout();
}

float Controller::GetUnderlineHeight() const
{
  return mImpl->mModel->mVisualModel->GetUnderlineHeight();
}

void Controller::SetOutlineColor(const Vector4& color)
{
  mImpl->mModel->mVisualModel->SetOutlineColor(color);

  mImpl->RequestRelayout();
}

const Vector4& Controller::GetOutlineColor() const
{
  return mImpl->mModel->mVisualModel->GetOutlineColor();
}

void Controller::SetOutlineWidth(uint16_t width)
{
  mImpl->mModel->mVisualModel->SetOutlineWidth(width);

  mImpl->RequestRelayout();
}

uint16_t Controller::GetOutlineWidth() const
{
  return mImpl->mModel->mVisualModel->GetOutlineWidth();
}

void Controller::SetBackgroundColor(const Vector4& color)
{
  mImpl->mModel->mVisualModel->SetBackgroundColor(color);

  mImpl->RequestRelayout();
}

const Vector4& Controller::GetBackgroundColor() const
{
  return mImpl->mModel->mVisualModel->GetBackgroundColor();
}

void Controller::SetBackgroundEnabled(bool enabled)
{
  mImpl->mModel->mVisualModel->SetBackgroundEnabled(enabled);

  mImpl->RequestRelayout();
}

bool Controller::IsBackgroundEnabled() const
{
  return mImpl->mModel->mVisualModel->IsBackgroundEnabled();
}

void Controller::SetDefaultEmbossProperties(const std::string& embossProperties)
{
  if(NULL == mImpl->mEmbossDefaults)
  {
    mImpl->mEmbossDefaults = new EmbossDefaults();
  }

  mImpl->mEmbossDefaults->properties = embossProperties;
}

const std::string& Controller::GetDefaultEmbossProperties() const
{
  if(NULL != mImpl->mEmbossDefaults)
  {
    return mImpl->mEmbossDefaults->properties;
  }

  return EMPTY_STRING;
}

void Controller::SetDefaultOutlineProperties(const std::string& outlineProperties)
{
  if(NULL == mImpl->mOutlineDefaults)
  {
    mImpl->mOutlineDefaults = new OutlineDefaults();
  }

  mImpl->mOutlineDefaults->properties = outlineProperties;
}

const std::string& Controller::GetDefaultOutlineProperties() const
{
  if(NULL != mImpl->mOutlineDefaults)
  {
    return mImpl->mOutlineDefaults->properties;
  }

  return EMPTY_STRING;
}

bool Controller::SetDefaultLineSpacing(float lineSpacing)
{
  if(std::fabs(lineSpacing - mImpl->mLayoutEngine.GetDefaultLineSpacing()) > Math::MACHINE_EPSILON_1000)
  {
    mImpl->mLayoutEngine.SetDefaultLineSpacing(lineSpacing);
    mImpl->mRecalculateNaturalSize = true;
    return true;
  }
  return false;
}

float Controller::GetDefaultLineSpacing() const
{
  return mImpl->mLayoutEngine.GetDefaultLineSpacing();
}

bool Controller::SetDefaultLineSize(float lineSize)
{
  if(std::fabs(lineSize - mImpl->mLayoutEngine.GetDefaultLineSize()) > Math::MACHINE_EPSILON_1000)
  {
    mImpl->mLayoutEngine.SetDefaultLineSize(lineSize);
    mImpl->mRecalculateNaturalSize = true;
    return true;
  }
  return false;
}

float Controller::GetDefaultLineSize() const
{
  return mImpl->mLayoutEngine.GetDefaultLineSize();
}

void Controller::SetInputColor(const Vector4& color)
{
  if(NULL != mImpl->mEventData)
  {
    mImpl->mEventData->mInputStyle.textColor      = color;
    mImpl->mEventData->mInputStyle.isDefaultColor = false;

    if(EventData::SELECTING == mImpl->mEventData->mState || EventData::EDITING == mImpl->mEventData->mState || EventData::INACTIVE == mImpl->mEventData->mState)
    {
      if(EventData::SELECTING == mImpl->mEventData->mState)
      {
        const bool handlesCrossed = mImpl->mEventData->mLeftSelectionPosition > mImpl->mEventData->mRightSelectionPosition;

        // Get start and end position of selection
        const CharacterIndex startOfSelectedText  = handlesCrossed ? mImpl->mEventData->mRightSelectionPosition : mImpl->mEventData->mLeftSelectionPosition;
        const Length         lengthOfSelectedText = (handlesCrossed ? mImpl->mEventData->mLeftSelectionPosition : mImpl->mEventData->mRightSelectionPosition) - startOfSelectedText;

        // Add the color run.
        const VectorBase::SizeType numberOfRuns = mImpl->mModel->mLogicalModel->mColorRuns.Count();
        mImpl->mModel->mLogicalModel->mColorRuns.Resize(numberOfRuns + 1u);

        ColorRun& colorRun                       = *(mImpl->mModel->mLogicalModel->mColorRuns.Begin() + numberOfRuns);
        colorRun.color                           = color;
        colorRun.characterRun.characterIndex     = startOfSelectedText;
        colorRun.characterRun.numberOfCharacters = lengthOfSelectedText;

        mImpl->mTextUpdateInfo.mCharacterIndex             = startOfSelectedText;
        mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = lengthOfSelectedText;
        mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd    = lengthOfSelectedText;
      }
      else
      {
        mImpl->mTextUpdateInfo.mCharacterIndex             = 0;
        mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;
        mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd    = mImpl->mModel->mLogicalModel->mText.Count();
      }

      // Request to relayout.
      mImpl->mOperationsPending = static_cast<OperationsMask>(mImpl->mOperationsPending | COLOR);
      mImpl->RequestRelayout();
    }
  }
}

const Vector4& Controller::GetInputColor() const
{
  if(NULL != mImpl->mEventData)
  {
    return mImpl->mEventData->mInputStyle.textColor;
  }

  // Return the default text's color if there is no EventData.
  return mImpl->mTextColor;
}

void Controller::SetInputFontFamily(const std::string& fontFamily)
{
  InputFontHandler::SetInputFontFamily(*this, fontFamily);
}

const std::string& Controller::GetInputFontFamily() const
{
  return InputFontHandler::GetInputFontFamily(*this);
}

void Controller::SetInputFontWeight(FontWeight weight)
{
  InputFontHandler::SetInputFontWeight(*this, weight);
}

bool Controller::IsInputFontWeightDefined() const
{
  return InputFontHandler::IsInputFontWeightDefined(*this);
}

FontWeight Controller::GetInputFontWeight() const
{
  return InputFontHandler::GetInputFontWeight(*this);
}

void Controller::SetInputFontWidth(FontWidth width)
{
  InputFontHandler::SetInputFontWidth(*this, width);
}

bool Controller::IsInputFontWidthDefined() const
{
  return InputFontHandler::IsInputFontWidthDefined(*this);
}

FontWidth Controller::GetInputFontWidth() const
{
  return InputFontHandler::GetInputFontWidth(*this);
}

void Controller::SetInputFontSlant(FontSlant slant)
{
  InputFontHandler::SetInputFontSlant(*this, slant);
}

bool Controller::IsInputFontSlantDefined() const
{
  return InputFontHandler::IsInputFontSlantDefined(*this);
}

FontSlant Controller::GetInputFontSlant() const
{
  return InputFontHandler::GetInputFontSlant(*this);
}

void Controller::SetInputFontPointSize(float size)
{
  InputFontHandler::SetInputFontPointSize(*this, size);
}

float Controller::GetInputFontPointSize() const
{
  return InputFontHandler::GetInputFontPointSize(*this);
}

void Controller::SetInputLineSpacing(float lineSpacing)
{
  if(NULL != mImpl->mEventData)
  {
    mImpl->mEventData->mInputStyle.lineSpacing          = lineSpacing;
    mImpl->mEventData->mInputStyle.isLineSpacingDefined = true;
  }
}

float Controller::GetInputLineSpacing() const
{
  if(NULL != mImpl->mEventData)
  {
    return mImpl->mEventData->mInputStyle.lineSpacing;
  }

  return 0.f;
}

void Controller::SetInputShadowProperties(const std::string& shadowProperties)
{
  if(NULL != mImpl->mEventData)
  {
    mImpl->mEventData->mInputStyle.shadowProperties = shadowProperties;
  }
}

const std::string& Controller::GetInputShadowProperties() const
{
  if(NULL != mImpl->mEventData)
  {
    return mImpl->mEventData->mInputStyle.shadowProperties;
  }

  return EMPTY_STRING;
}

void Controller::SetInputUnderlineProperties(const std::string& underlineProperties)
{
  if(NULL != mImpl->mEventData)
  {
    mImpl->mEventData->mInputStyle.underlineProperties = underlineProperties;
  }
}

const std::string& Controller::GetInputUnderlineProperties() const
{
  if(NULL != mImpl->mEventData)
  {
    return mImpl->mEventData->mInputStyle.underlineProperties;
  }

  return EMPTY_STRING;
}

void Controller::SetInputEmbossProperties(const std::string& embossProperties)
{
  if(NULL != mImpl->mEventData)
  {
    mImpl->mEventData->mInputStyle.embossProperties = embossProperties;
  }
}

const std::string& Controller::GetInputEmbossProperties() const
{
  if(NULL != mImpl->mEventData)
  {
    return mImpl->mEventData->mInputStyle.embossProperties;
  }

  return GetDefaultEmbossProperties();
}

void Controller::SetInputOutlineProperties(const std::string& outlineProperties)
{
  if(NULL != mImpl->mEventData)
  {
    mImpl->mEventData->mInputStyle.outlineProperties = outlineProperties;
  }
}

const std::string& Controller::GetInputOutlineProperties() const
{
  if(NULL != mImpl->mEventData)
  {
    return mImpl->mEventData->mInputStyle.outlineProperties;
  }

  return GetDefaultOutlineProperties();
}

void Controller::SetInputModePassword(bool passwordInput)
{
  if(NULL != mImpl->mEventData)
  {
    mImpl->mEventData->mPasswordInput = passwordInput;
  }
}

bool Controller::IsInputModePassword()
{
  if(NULL != mImpl->mEventData)
  {
    return mImpl->mEventData->mPasswordInput;
  }
  return false;
}

void Controller::SetNoTextDoubleTapAction(NoTextTap::Action action)
{
  if(NULL != mImpl->mEventData)
  {
    mImpl->mEventData->mDoubleTapAction = action;
  }
}

Controller::NoTextTap::Action Controller::GetNoTextDoubleTapAction() const
{
  NoTextTap::Action action = NoTextTap::NO_ACTION;

  if(NULL != mImpl->mEventData)
  {
    action = mImpl->mEventData->mDoubleTapAction;
  }

  return action;
}

void Controller::SetNoTextLongPressAction(NoTextTap::Action action)
{
  if(NULL != mImpl->mEventData)
  {
    mImpl->mEventData->mLongPressAction = action;
  }
}

Controller::NoTextTap::Action Controller::GetNoTextLongPressAction() const
{
  NoTextTap::Action action = NoTextTap::NO_ACTION;

  if(NULL != mImpl->mEventData)
  {
    action = mImpl->mEventData->mLongPressAction;
  }

  return action;
}

bool Controller::IsUnderlineSetByString()
{
  return mImpl->mUnderlineSetByString;
}

void Controller::UnderlineSetByString(bool setByString)
{
  mImpl->mUnderlineSetByString = setByString;
}

bool Controller::IsShadowSetByString()
{
  return mImpl->mShadowSetByString;
}

void Controller::ShadowSetByString(bool setByString)
{
  mImpl->mShadowSetByString = setByString;
}

bool Controller::IsOutlineSetByString()
{
  return mImpl->mOutlineSetByString;
}

void Controller::OutlineSetByString(bool setByString)
{
  mImpl->mOutlineSetByString = setByString;
}

bool Controller::IsFontStyleSetByString()
{
  return mImpl->mFontStyleSetByString;
}

void Controller::FontStyleSetByString(bool setByString)
{
  mImpl->mFontStyleSetByString = setByString;
}

// public : Queries & retrieves.

Layout::Engine& Controller::GetLayoutEngine()
{
  return mImpl->mLayoutEngine;
}

View& Controller::GetView()
{
  return mImpl->mView;
}

Vector3 Controller::GetNaturalSize()
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::GetNaturalSize\n");
  Vector3 naturalSize;

  // Make sure the model is up-to-date before layouting
  ProcessModifyEvents();

  if(mImpl->mRecalculateNaturalSize)
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
    mImpl->mTextUpdateInfo.mParagraphCharacterIndex     = 0u;
    mImpl->mTextUpdateInfo.mRequestedNumberOfCharacters = mImpl->mModel->mLogicalModel->mText.Count();

    // Make sure the model is up-to-date before layouting
    mImpl->UpdateModel(onlyOnceOperations);

    // Layout the text for the new width.
    mImpl->mOperationsPending = static_cast<OperationsMask>(mImpl->mOperationsPending | LAYOUT | REORDER);

    // Store the actual control's size to restore later.
    const Size actualControlSize = mImpl->mModel->mVisualModel->mControlSize;

    DoRelayout(Size(MAX_FLOAT, MAX_FLOAT),
               static_cast<OperationsMask>(onlyOnceOperations |
                                           LAYOUT | REORDER),
               naturalSize.GetVectorXY());

    // Do not do again the only once operations.
    mImpl->mOperationsPending = static_cast<OperationsMask>(mImpl->mOperationsPending & ~onlyOnceOperations);

    // Do the size related operations again.
    const OperationsMask sizeOperations = static_cast<OperationsMask>(LAYOUT |
                                                                      ALIGN |
                                                                      REORDER);
    mImpl->mOperationsPending           = static_cast<OperationsMask>(mImpl->mOperationsPending | sizeOperations);

    // Stores the natural size to avoid recalculate it again
    // unless the text/style changes.
    mImpl->mModel->mVisualModel->SetNaturalSize(naturalSize.GetVectorXY());

    mImpl->mRecalculateNaturalSize = false;

    // Clear the update info. This info will be set the next time the text is updated.
    mImpl->mTextUpdateInfo.Clear();
    mImpl->mTextUpdateInfo.mClearAll = true;

    // Restore the actual control's size.
    mImpl->mModel->mVisualModel->mControlSize = actualControlSize;

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::GetNaturalSize calculated %f,%f,%f\n", naturalSize.x, naturalSize.y, naturalSize.z);
  }
  else
  {
    naturalSize = mImpl->mModel->mVisualModel->GetNaturalSize();

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::GetNaturalSize cached %f,%f,%f\n", naturalSize.x, naturalSize.y, naturalSize.z);
  }

  naturalSize.x = ConvertToEven(naturalSize.x);
  naturalSize.y = ConvertToEven(naturalSize.y);

  return naturalSize;
}

bool Controller::CheckForTextFit(float pointSize, Size& layoutSize)
{
  Size textSize;
  mImpl->mFontDefaults->mFitPointSize = pointSize;
  mImpl->mFontDefaults->sizeDefined   = true;
  ClearFontData();

  // Operations that can be done only once until the text changes.
  const OperationsMask onlyOnceOperations = static_cast<OperationsMask>(CONVERT_TO_UTF32 |
                                                                        GET_SCRIPTS |
                                                                        VALIDATE_FONTS |
                                                                        GET_LINE_BREAKS |
                                                                        BIDI_INFO |
                                                                        SHAPE_TEXT |
                                                                        GET_GLYPH_METRICS);

  mImpl->mTextUpdateInfo.mParagraphCharacterIndex     = 0u;
  mImpl->mTextUpdateInfo.mRequestedNumberOfCharacters = mImpl->mModel->mLogicalModel->mText.Count();

  // Make sure the model is up-to-date before layouting
  mImpl->UpdateModel(onlyOnceOperations);

  DoRelayout(Size(layoutSize.width, MAX_FLOAT),
             static_cast<OperationsMask>(onlyOnceOperations | LAYOUT),
             textSize);

  // Clear the update info. This info will be set the next time the text is updated.
  mImpl->mTextUpdateInfo.Clear();
  mImpl->mTextUpdateInfo.mClearAll = true;

  if(textSize.width > layoutSize.width || textSize.height > layoutSize.height)
  {
    return false;
  }
  return true;
}

void Controller::FitPointSizeforLayout(Size layoutSize)
{
  const OperationsMask operations = mImpl->mOperationsPending;
  if(NO_OPERATION != (UPDATE_LAYOUT_SIZE & operations) || mImpl->mTextFitContentSize != layoutSize)
  {
    bool  actualellipsis = mImpl->mModel->mElideEnabled;
    float minPointSize   = mImpl->mTextFitMinSize;
    float maxPointSize   = mImpl->mTextFitMaxSize;
    float pointInterval  = mImpl->mTextFitStepSize;

    mImpl->mModel->mElideEnabled = false;
    Vector<float> pointSizeArray;

    // check zero value
    if(pointInterval < 1.f)
    {
      mImpl->mTextFitStepSize = pointInterval = 1.0f;
    }

    pointSizeArray.Reserve(static_cast<unsigned int>(ceil((maxPointSize - minPointSize) / pointInterval)));

    for(float i = minPointSize; i < maxPointSize; i += pointInterval)
    {
      pointSizeArray.PushBack(i);
    }

    pointSizeArray.PushBack(maxPointSize);

    int bestSizeIndex = 0;
    int min           = bestSizeIndex + 1;
    int max           = pointSizeArray.Size() - 1;
    while(min <= max)
    {
      int destI = (min + max) / 2;

      if(CheckForTextFit(pointSizeArray[destI], layoutSize))
      {
        bestSizeIndex = min;
        min           = destI + 1;
      }
      else
      {
        max           = destI - 1;
        bestSizeIndex = max;
      }
    }

    mImpl->mModel->mElideEnabled        = actualellipsis;
    mImpl->mFontDefaults->mFitPointSize = pointSizeArray[bestSizeIndex];
    mImpl->mFontDefaults->sizeDefined   = true;
    ClearFontData();
  }
}

float Controller::GetHeightForWidth(float width)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::GetHeightForWidth %p width %f\n", this, width);
  // Make sure the model is up-to-date before layouting
  ProcessModifyEvents();

  Size layoutSize;
  if(fabsf(width - mImpl->mModel->mVisualModel->mControlSize.width) > Math::MACHINE_EPSILON_1000 ||
     mImpl->mTextUpdateInfo.mFullRelayoutNeeded ||
     mImpl->mTextUpdateInfo.mClearAll)
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
    mImpl->mTextUpdateInfo.mParagraphCharacterIndex     = 0u;
    mImpl->mTextUpdateInfo.mRequestedNumberOfCharacters = mImpl->mModel->mLogicalModel->mText.Count();

    // Make sure the model is up-to-date before layouting
    mImpl->UpdateModel(onlyOnceOperations);

    // Layout the text for the new width.
    mImpl->mOperationsPending = static_cast<OperationsMask>(mImpl->mOperationsPending | LAYOUT);

    // Store the actual control's width.
    const float actualControlWidth = mImpl->mModel->mVisualModel->mControlSize.width;

    DoRelayout(Size(width, MAX_FLOAT),
               static_cast<OperationsMask>(onlyOnceOperations |
                                           LAYOUT),
               layoutSize);

    // Do not do again the only once operations.
    mImpl->mOperationsPending = static_cast<OperationsMask>(mImpl->mOperationsPending & ~onlyOnceOperations);

    // Do the size related operations again.
    const OperationsMask sizeOperations = static_cast<OperationsMask>(LAYOUT |
                                                                      ALIGN |
                                                                      REORDER);

    mImpl->mOperationsPending = static_cast<OperationsMask>(mImpl->mOperationsPending | sizeOperations);

    // Clear the update info. This info will be set the next time the text is updated.
    mImpl->mTextUpdateInfo.Clear();
    mImpl->mTextUpdateInfo.mClearAll = true;

    // Restore the actual control's width.
    mImpl->mModel->mVisualModel->mControlSize.width = actualControlWidth;

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::GetHeightForWidth calculated %f\n", layoutSize.height);
  }
  else
  {
    layoutSize = mImpl->mModel->mVisualModel->GetLayoutSize();
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::GetHeightForWidth cached %f\n", layoutSize.height);
  }

  return layoutSize.height;
}

int Controller::GetLineCount(float width)
{
  GetHeightForWidth(width);
  int numberofLines = mImpl->mModel->GetNumberOfLines();
  return numberofLines;
}

const ModelInterface* const Controller::GetTextModel() const
{
  return mImpl->mModel.Get();
}

float Controller::GetScrollAmountByUserInput()
{
  float scrollAmount = 0.0f;

  if(NULL != mImpl->mEventData && mImpl->mEventData->mCheckScrollAmount)
  {
    scrollAmount                          = mImpl->mModel->mScrollPosition.y - mImpl->mModel->mScrollPositionLast.y;
    mImpl->mEventData->mCheckScrollAmount = false;
  }
  return scrollAmount;
}

bool Controller::GetTextScrollInfo(float& scrollPosition, float& controlHeight, float& layoutHeight)
{
  const Vector2& layout = mImpl->mModel->mVisualModel->GetLayoutSize();
  bool           isScrolled;

  controlHeight  = mImpl->mModel->mVisualModel->mControlSize.height;
  layoutHeight   = layout.height;
  scrollPosition = mImpl->mModel->mScrollPosition.y;
  isScrolled     = !Equals(mImpl->mModel->mScrollPosition.y, mImpl->mModel->mScrollPositionLast.y, Math::MACHINE_EPSILON_1);
  return isScrolled;
}

void Controller::SetHiddenInputOption(const Property::Map& options)
{
  if(NULL == mImpl->mHiddenInput)
  {
    mImpl->mHiddenInput = new HiddenText(this);
  }
  mImpl->mHiddenInput->SetProperties(options);
}

void Controller::GetHiddenInputOption(Property::Map& options)
{
  if(NULL != mImpl->mHiddenInput)
  {
    mImpl->mHiddenInput->GetProperties(options);
  }
}

void Controller::SetPlaceholderProperty(const Property::Map& map)
{
  PlaceholderHandler::SetPlaceholderProperty(*this, map);
}

void Controller::GetPlaceholderProperty(Property::Map& map)
{
  PlaceholderHandler::GetPlaceholderProperty(*this, map);
}

Toolkit::DevelText::TextDirection::Type Controller::GetTextDirection()
{
  // Make sure the model is up-to-date before layouting
  ProcessModifyEvents();

  if(mImpl->mUpdateTextDirection)
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
    mImpl->mTextUpdateInfo.mParagraphCharacterIndex     = 0u;
    mImpl->mTextUpdateInfo.mRequestedNumberOfCharacters = mImpl->mModel->mLogicalModel->mText.Count();

    // Make sure the model is up-to-date before layouting
    mImpl->UpdateModel(onlyOnceOperations);

    Vector3 naturalSize;
    DoRelayout(Size(MAX_FLOAT, MAX_FLOAT),
               static_cast<OperationsMask>(onlyOnceOperations |
                                           LAYOUT | REORDER | UPDATE_DIRECTION),
               naturalSize.GetVectorXY());

    // Do not do again the only once operations.
    mImpl->mOperationsPending = static_cast<OperationsMask>(mImpl->mOperationsPending & ~onlyOnceOperations);

    // Clear the update info. This info will be set the next time the text is updated.
    mImpl->mTextUpdateInfo.Clear();

    // FullRelayoutNeeded should be true because DoRelayout is MAX_FLOAT, MAX_FLOAT.
    mImpl->mTextUpdateInfo.mFullRelayoutNeeded = true;

    mImpl->mUpdateTextDirection = false;
  }

  return mImpl->mIsTextDirectionRTL ? Toolkit::DevelText::TextDirection::RIGHT_TO_LEFT : Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT;
}

Toolkit::DevelText::VerticalLineAlignment::Type Controller::GetVerticalLineAlignment() const
{
  return mImpl->mModel->GetVerticalLineAlignment();
}

void Controller::SetVerticalLineAlignment(Toolkit::DevelText::VerticalLineAlignment::Type alignment)
{
  mImpl->mModel->mVerticalLineAlignment = alignment;
}

// public : Relayout.

Controller::UpdateTextType Controller::Relayout(const Size& size, Dali::LayoutDirection::Type layoutDirection)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::Relayout %p size %f,%f, autoScroll[%s]\n", this, size.width, size.height, mImpl->mIsAutoScrollEnabled ? "true" : "false");

  UpdateTextType updateTextType = NONE_UPDATED;

  if((size.width < Math::MACHINE_EPSILON_1000) || (size.height < Math::MACHINE_EPSILON_1000))
  {
    if(0u != mImpl->mModel->mVisualModel->mGlyphPositions.Count())
    {
      mImpl->mModel->mVisualModel->mGlyphPositions.Clear();
      updateTextType = MODEL_UPDATED;
    }

    // Clear the update info. This info will be set the next time the text is updated.
    mImpl->mTextUpdateInfo.Clear();

    // Not worth to relayout if width or height is equal to zero.
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::Relayout (skipped)\n");

    return updateTextType;
  }

  // Whether a new size has been set.
  const bool newSize = (size != mImpl->mModel->mVisualModel->mControlSize);

  if(newSize)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "new size (previous size %f,%f)\n", mImpl->mModel->mVisualModel->mControlSize.width, mImpl->mModel->mVisualModel->mControlSize.height);

    if((0 == mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd) &&
       (0 == mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters) &&
       ((mImpl->mModel->mVisualModel->mControlSize.width < Math::MACHINE_EPSILON_1000) || (mImpl->mModel->mVisualModel->mControlSize.height < Math::MACHINE_EPSILON_1000)))
    {
      mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd = mImpl->mModel->mLogicalModel->mText.Count();
    }

    // Layout operations that need to be done if the size changes.
    mImpl->mOperationsPending = static_cast<OperationsMask>(mImpl->mOperationsPending |
                                                            LAYOUT |
                                                            ALIGN |
                                                            UPDATE_LAYOUT_SIZE |
                                                            REORDER);
    // Set the update info to relayout the whole text.
    mImpl->mTextUpdateInfo.mFullRelayoutNeeded = true;
    mImpl->mTextUpdateInfo.mCharacterIndex     = 0u;

    // Store the size used to layout the text.
    mImpl->mModel->mVisualModel->mControlSize = size;
  }

  // Whether there are modify events.
  if(0u != mImpl->mModifyEvents.Count())
  {
    // Style operations that need to be done if the text is modified.
    mImpl->mOperationsPending = static_cast<OperationsMask>(mImpl->mOperationsPending |
                                                            COLOR);
  }

  // Set the update info to elide the text.
  if(mImpl->mModel->mElideEnabled ||
     ((NULL != mImpl->mEventData) && mImpl->mEventData->mIsPlaceholderElideEnabled))
  {
    // Update Text layout for applying elided
    mImpl->mOperationsPending                  = static_cast<OperationsMask>(mImpl->mOperationsPending |
                                                            ALIGN |
                                                            LAYOUT |
                                                            UPDATE_LAYOUT_SIZE |
                                                            REORDER);
    mImpl->mTextUpdateInfo.mFullRelayoutNeeded = true;
    mImpl->mTextUpdateInfo.mCharacterIndex     = 0u;
  }

  if(mImpl->mModel->mMatchSystemLanguageDirection && mImpl->mLayoutDirection != layoutDirection)
  {
    // Clear the update info. This info will be set the next time the text is updated.
    mImpl->mTextUpdateInfo.mClearAll = true;
    // Apply modifications to the model
    // Shape the text again is needed because characters like '()[]{}' have to be mirrored and the glyphs generated again.
    mImpl->mOperationsPending = static_cast<OperationsMask>(mImpl->mOperationsPending |
                                                            GET_GLYPH_METRICS |
                                                            SHAPE_TEXT |
                                                            UPDATE_DIRECTION |
                                                            LAYOUT |
                                                            BIDI_INFO |
                                                            REORDER);
    mImpl->mLayoutDirection   = layoutDirection;
  }

  // Make sure the model is up-to-date before layouting.
  ProcessModifyEvents();
  bool updated = mImpl->UpdateModel(mImpl->mOperationsPending);

  // Layout the text.
  Size layoutSize;
  updated = DoRelayout(size,
                       mImpl->mOperationsPending,
                       layoutSize) ||
            updated;

  if(updated)
  {
    updateTextType = MODEL_UPDATED;
  }

  // Do not re-do any operation until something changes.
  mImpl->mOperationsPending          = NO_OPERATION;
  mImpl->mModel->mScrollPositionLast = mImpl->mModel->mScrollPosition;

  // Whether the text control is editable
  const bool isEditable = NULL != mImpl->mEventData;

  // Keep the current offset as it will be used to update the decorator's positions (if the size changes).
  Vector2 offset;
  if(newSize && isEditable)
  {
    offset = mImpl->mModel->mScrollPosition;
  }

  if(!isEditable || !IsMultiLineEnabled())
  {
    // After doing the text layout, the vertical offset to place the actor in the desired position can be calculated.
    CalculateVerticalOffset(size);
  }

  if(isEditable)
  {
    if(newSize)
    {
      // If there is a new size, the scroll position needs to be clamped.
      mImpl->ClampHorizontalScroll(layoutSize);

      // Update the decorator's positions is needed if there is a new size.
      mImpl->mEventData->mDecorator->UpdatePositions(mImpl->mModel->mScrollPosition - offset);
    }

    // Move the cursor, grab handle etc.
    if(mImpl->ProcessInputEvents())
    {
      updateTextType = static_cast<UpdateTextType>(updateTextType | DECORATOR_UPDATED);
    }
  }

  // Clear the update info. This info will be set the next time the text is updated.
  mImpl->mTextUpdateInfo.Clear();
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::Relayout\n");

  return updateTextType;
}

void Controller::RequestRelayout()
{
  mImpl->RequestRelayout();
}

// public : Input style change signals.

bool Controller::IsInputStyleChangedSignalsQueueEmpty()
{
  return (NULL == mImpl->mEventData) || (0u == mImpl->mEventData->mInputStyleChangedQueue.Count());
}

void Controller::ProcessInputStyleChangedSignals()
{
  if(NULL == mImpl->mEventData)
  {
    // Nothing to do.
    return;
  }

  for(Vector<InputStyle::Mask>::ConstIterator it    = mImpl->mEventData->mInputStyleChangedQueue.Begin(),
                                              endIt = mImpl->mEventData->mInputStyleChangedQueue.End();
      it != endIt;
      ++it)
  {
    const InputStyle::Mask mask = *it;

    if(NULL != mImpl->mEditableControlInterface)
    {
      // Emit the input style changed signal.
      mImpl->mEditableControlInterface->InputStyleChanged(mask);
    }
  }

  mImpl->mEventData->mInputStyleChangedQueue.Clear();
}

// public : Text-input Event Queuing.

void Controller::KeyboardFocusGainEvent()
{
  EventHandler::KeyboardFocusGainEvent(*this);
}

void Controller::KeyboardFocusLostEvent()
{
  EventHandler::KeyboardFocusLostEvent(*this);
}

bool Controller::KeyEvent(const Dali::KeyEvent& keyEvent)
{
  return EventHandler::KeyEvent(*this, keyEvent);
}

void Controller::TapEvent(unsigned int tapCount, float x, float y)
{
  EventHandler::TapEvent(*this, tapCount, x, y);
}

void Controller::PanEvent(GestureState state, const Vector2& displacement)
{
  EventHandler::PanEvent(*this, state, displacement);
}

void Controller::LongPressEvent(GestureState state, float x, float y)
{
  EventHandler::LongPressEvent(*this, state, x, y);
}

void Controller::SelectEvent(float x, float y, SelectionType selectType)
{
  EventHandler::SelectEvent(*this, x, y, selectType);
}

void Controller::SetTextSelectionRange(const uint32_t* start, const uint32_t* end)
{
  if(mImpl->mEventData)
  {
    mImpl->mEventData->mCheckScrollAmount     = true;
    mImpl->mEventData->mIsLeftHandleSelected  = true;
    mImpl->mEventData->mIsRightHandleSelected = true;
    mImpl->SetTextSelectionRange(start, end);
    mImpl->RequestRelayout();
    KeyboardFocusGainEvent();
  }
}

Uint32Pair Controller::GetTextSelectionRange() const
{
  return mImpl->GetTextSelectionRange();
}

void Controller::SelectWholeText()
{
  SelectEvent(0.f, 0.f, SelectionType::ALL);
}

void Controller::SelectNone()
{
  SelectEvent(0.f, 0.f, SelectionType::NONE);
}

string Controller::GetSelectedText() const
{
  string text;
  if(EventData::SELECTING == mImpl->mEventData->mState)
  {
    mImpl->RetrieveSelection(text, false);
  }
  return text;
}

InputMethodContext::CallbackData Controller::OnInputMethodContextEvent(InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent)
{
  return EventHandler::OnInputMethodContextEvent(*this, inputMethodContext, inputMethodContextEvent);
}

void Controller::PasteClipboardItemEvent()
{
  EventHandler::PasteClipboardItemEvent(*this);
}

// protected : Inherit from Text::Decorator::ControllerInterface.

void Controller::GetTargetSize(Vector2& targetSize)
{
  targetSize = mImpl->mModel->mVisualModel->mControlSize;
}

void Controller::AddDecoration(Actor& actor, bool needsClipping)
{
  if(NULL != mImpl->mEditableControlInterface)
  {
    mImpl->mEditableControlInterface->AddDecoration(actor, needsClipping);
  }
}

bool Controller::IsEditable() const
{
  return mImpl->IsEditable();
}

void Controller::SetEditable(bool editable)
{
  mImpl->SetEditable(editable);
  if(mImpl->mEventData && mImpl->mEventData->mDecorator)
  {
    mImpl->mEventData->mDecorator->SetEditable(editable);
  }
}

void Controller::DecorationEvent(HandleType handleType, HandleState state, float x, float y)
{
  EventHandler::DecorationEvent(*this, handleType, state, x, y);
}

// protected : Inherit from TextSelectionPopup::TextPopupButtonCallbackInterface.

void Controller::TextPopupButtonTouched(Dali::Toolkit::TextSelectionPopup::Buttons button)
{
  EventHandler::TextPopupButtonTouched(*this, button);
}

void Controller::DisplayTimeExpired()
{
  mImpl->mEventData->mUpdateCursorPosition = true;
  // Apply modifications to the model
  mImpl->mOperationsPending = ALL_OPERATIONS;

  mImpl->RequestRelayout();
}

// private : Update.

void Controller::InsertText(const std::string& text, Controller::InsertType type)
{
  bool removedPrevious  = false;
  bool removedSelected  = false;
  bool maxLengthReached = false;

  DALI_ASSERT_DEBUG(NULL != mImpl->mEventData && "Unexpected InsertText")

  if(NULL == mImpl->mEventData)
  {
    return;
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::InsertText %p %s (%s) mPrimaryCursorPosition %d mPreEditFlag %d mPreEditStartPosition %d mPreEditLength %d\n", this, text.c_str(), (COMMIT == type ? "COMMIT" : "PRE_EDIT"), mImpl->mEventData->mPrimaryCursorPosition, mImpl->mEventData->mPreEditFlag, mImpl->mEventData->mPreEditStartPosition, mImpl->mEventData->mPreEditLength);

  // Play the input text with the TTS player.
  Dali::TtsPlayer player = Dali::TtsPlayer::Get(Dali::TtsPlayer::SCREEN_READER);
  if (player)
  {
    player.Play(text);
  }

  // TODO: At the moment the underline runs are only for pre-edit.
  mImpl->mModel->mVisualModel->mUnderlineRuns.Clear();

  // Remove the previous InputMethodContext pre-edit.
  if(mImpl->mEventData->mPreEditFlag && (0u != mImpl->mEventData->mPreEditLength))
  {
    removedPrevious = RemoveText(-static_cast<int>(mImpl->mEventData->mPrimaryCursorPosition - mImpl->mEventData->mPreEditStartPosition),
                                 mImpl->mEventData->mPreEditLength,
                                 DONT_UPDATE_INPUT_STYLE);

    mImpl->mEventData->mPrimaryCursorPosition = mImpl->mEventData->mPreEditStartPosition;
    mImpl->mEventData->mPreEditLength         = 0u;
  }
  else
  {
    // Remove the previous Selection.
    removedSelected = RemoveSelectedText();
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
    if(mImpl->IsShowingPlaceholderText())
    {
      ResetText();
    }

    mImpl->ChangeState(EventData::EDITING);

    // Handle the InputMethodContext (predicitive text) state changes
    if(COMMIT == type)
    {
      // InputMethodContext is no longer handling key-events
      mImpl->ClearPreEditFlag();
    }
    else // PRE_EDIT
    {
      if(!mImpl->mEventData->mPreEditFlag)
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Entered PreEdit state\n");

        // Record the start of the pre-edit text
        mImpl->mEventData->mPreEditStartPosition = mImpl->mEventData->mPrimaryCursorPosition;
      }

      mImpl->mEventData->mPreEditLength = utf32Characters.Count();
      mImpl->mEventData->mPreEditFlag   = true;

      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "mPreEditStartPosition %d mPreEditLength %d\n", mImpl->mEventData->mPreEditStartPosition, mImpl->mEventData->mPreEditLength);
    }

    const Length numberOfCharactersInModel = mImpl->mModel->mLogicalModel->mText.Count();

    // Restrict new text to fit within Maximum characters setting.
    Length maxSizeOfNewText = std::min((mImpl->mMaximumNumberOfCharacters - numberOfCharactersInModel), characterCount);
    maxLengthReached        = (characterCount > maxSizeOfNewText);

    // The cursor position.
    CharacterIndex& cursorIndex = mImpl->mEventData->mPrimaryCursorPosition;

    // Update the text's style.

    // Updates the text style runs by adding characters.
    mImpl->mModel->mLogicalModel->UpdateTextStyleRuns(cursorIndex, maxSizeOfNewText);

    // Get the character index from the cursor index.
    const CharacterIndex styleIndex = (cursorIndex > 0u) ? cursorIndex - 1u : 0u;

    // Retrieve the text's style for the given index.
    InputStyle style;
    mImpl->RetrieveDefaultInputStyle(style);
    mImpl->mModel->mLogicalModel->RetrieveStyle(styleIndex, style);

    // Whether to add a new text color run.
    const bool addColorRun = (style.textColor != mImpl->mEventData->mInputStyle.textColor) && !mImpl->mEventData->mInputStyle.isDefaultColor;

    // Whether to add a new font run.
    const bool addFontNameRun   = (style.familyName != mImpl->mEventData->mInputStyle.familyName) && mImpl->mEventData->mInputStyle.isFamilyDefined;
    const bool addFontWeightRun = (style.weight != mImpl->mEventData->mInputStyle.weight) && mImpl->mEventData->mInputStyle.isWeightDefined;
    const bool addFontWidthRun  = (style.width != mImpl->mEventData->mInputStyle.width) && mImpl->mEventData->mInputStyle.isWidthDefined;
    const bool addFontSlantRun  = (style.slant != mImpl->mEventData->mInputStyle.slant) && mImpl->mEventData->mInputStyle.isSlantDefined;
    const bool addFontSizeRun   = (style.size != mImpl->mEventData->mInputStyle.size) && mImpl->mEventData->mInputStyle.isSizeDefined;

    // Add style runs.
    if(addColorRun)
    {
      const VectorBase::SizeType numberOfRuns = mImpl->mModel->mLogicalModel->mColorRuns.Count();
      mImpl->mModel->mLogicalModel->mColorRuns.Resize(numberOfRuns + 1u);

      ColorRun& colorRun                       = *(mImpl->mModel->mLogicalModel->mColorRuns.Begin() + numberOfRuns);
      colorRun.color                           = mImpl->mEventData->mInputStyle.textColor;
      colorRun.characterRun.characterIndex     = cursorIndex;
      colorRun.characterRun.numberOfCharacters = maxSizeOfNewText;
    }

    if(addFontNameRun ||
       addFontWeightRun ||
       addFontWidthRun ||
       addFontSlantRun ||
       addFontSizeRun)
    {
      const VectorBase::SizeType numberOfRuns = mImpl->mModel->mLogicalModel->mFontDescriptionRuns.Count();
      mImpl->mModel->mLogicalModel->mFontDescriptionRuns.Resize(numberOfRuns + 1u);

      FontDescriptionRun& fontDescriptionRun = *(mImpl->mModel->mLogicalModel->mFontDescriptionRuns.Begin() + numberOfRuns);

      if(addFontNameRun)
      {
        fontDescriptionRun.familyLength = mImpl->mEventData->mInputStyle.familyName.size();
        fontDescriptionRun.familyName   = new char[fontDescriptionRun.familyLength];
        memcpy(fontDescriptionRun.familyName, mImpl->mEventData->mInputStyle.familyName.c_str(), fontDescriptionRun.familyLength);
        fontDescriptionRun.familyDefined = true;

        // The memory allocated for the font family name is freed when the font description is removed from the logical model.
      }

      if(addFontWeightRun)
      {
        fontDescriptionRun.weight        = mImpl->mEventData->mInputStyle.weight;
        fontDescriptionRun.weightDefined = true;
      }

      if(addFontWidthRun)
      {
        fontDescriptionRun.width        = mImpl->mEventData->mInputStyle.width;
        fontDescriptionRun.widthDefined = true;
      }

      if(addFontSlantRun)
      {
        fontDescriptionRun.slant        = mImpl->mEventData->mInputStyle.slant;
        fontDescriptionRun.slantDefined = true;
      }

      if(addFontSizeRun)
      {
        fontDescriptionRun.size        = static_cast<PointSize26Dot6>(mImpl->mEventData->mInputStyle.size * 64.f);
        fontDescriptionRun.sizeDefined = true;
      }

      fontDescriptionRun.characterRun.characterIndex     = cursorIndex;
      fontDescriptionRun.characterRun.numberOfCharacters = maxSizeOfNewText;
    }

    // Insert at current cursor position.
    Vector<Character>& modifyText = mImpl->mModel->mLogicalModel->mText;

    if(cursorIndex < numberOfCharactersInModel)
    {
      modifyText.Insert(modifyText.Begin() + cursorIndex, utf32Characters.Begin(), utf32Characters.Begin() + maxSizeOfNewText);
    }
    else
    {
      modifyText.Insert(modifyText.End(), utf32Characters.Begin(), utf32Characters.Begin() + maxSizeOfNewText);
    }

    // Mark the first paragraph to be updated.
    if(Layout::Engine::SINGLE_LINE_BOX == mImpl->mLayoutEngine.GetLayout())
    {
      mImpl->mTextUpdateInfo.mCharacterIndex             = 0;
      mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;
      mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd    = numberOfCharactersInModel + maxSizeOfNewText;
      mImpl->mTextUpdateInfo.mClearAll                   = true;
    }
    else
    {
      mImpl->mTextUpdateInfo.mCharacterIndex = std::min(cursorIndex, mImpl->mTextUpdateInfo.mCharacterIndex);
      mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd += maxSizeOfNewText;
    }

    // Update the cursor index.
    cursorIndex += maxSizeOfNewText;

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Inserted %d characters, new size %d new cursor %d\n", maxSizeOfNewText, mImpl->mModel->mLogicalModel->mText.Count(), mImpl->mEventData->mPrimaryCursorPosition);
  }

  if((0u == mImpl->mModel->mLogicalModel->mText.Count()) &&
     mImpl->IsPlaceholderAvailable())
  {
    // Show place-holder if empty after removing the pre-edit text
    ShowPlaceholderText();
    mImpl->mEventData->mUpdateCursorPosition = true;
    mImpl->ClearPreEditFlag();
  }
  else if(removedPrevious ||
          removedSelected ||
          (0 != utf32Characters.Count()))
  {
    // Queue an inserted event
    mImpl->QueueModifyEvent(ModifyEvent::TEXT_INSERTED);

    mImpl->mEventData->mUpdateCursorPosition = true;
    if(removedSelected)
    {
      mImpl->mEventData->mScrollAfterDelete = true;
    }
    else
    {
      mImpl->mEventData->mScrollAfterUpdatePosition = true;
    }
  }

  if(maxLengthReached)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "MaxLengthReached (%d)\n", mImpl->mModel->mLogicalModel->mText.Count());

    mImpl->ResetInputMethodContext();

    if(NULL != mImpl->mEditableControlInterface)
    {
      // Do this last since it provides callbacks into application code
      mImpl->mEditableControlInterface->MaxLengthReached();
    }
  }
}

void Controller::PasteText(const std::string& stringToPaste)
{
  InsertText(stringToPaste, Text::Controller::COMMIT);
  mImpl->ChangeState(EventData::EDITING);
  mImpl->RequestRelayout();

  if(NULL != mImpl->mEditableControlInterface)
  {
    // Do this last since it provides callbacks into application code
    mImpl->mEditableControlInterface->TextChanged(true);
  }
}

bool Controller::RemoveText(int                  cursorOffset,
                            int                  numberOfCharacters,
                            UpdateInputStyleType type)
{
  bool removed = false;
  bool removeAll = false;

  if(NULL == mImpl->mEventData)
  {
    return removed;
  }

  DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::RemoveText %p mText.Count() %d cursor %d cursorOffset %d numberOfCharacters %d\n", this, mImpl->mModel->mLogicalModel->mText.Count(), mImpl->mEventData->mPrimaryCursorPosition, cursorOffset, numberOfCharacters);

  if(!mImpl->IsShowingPlaceholderText())
  {
    // Delete at current cursor position
    Vector<Character>& currentText    = mImpl->mModel->mLogicalModel->mText;
    CharacterIndex&    oldCursorIndex = mImpl->mEventData->mPrimaryCursorPosition;

    CharacterIndex cursorIndex = 0;

    // Validate the cursor position & number of characters
    if((static_cast<int>(mImpl->mEventData->mPrimaryCursorPosition) + cursorOffset) >= 0)
    {
      cursorIndex = mImpl->mEventData->mPrimaryCursorPosition + cursorOffset;
    }

    if((cursorIndex + numberOfCharacters) > currentText.Count())
    {
      numberOfCharacters = currentText.Count() - cursorIndex;
    }

    if((cursorIndex == 0) && (currentText.Count() - numberOfCharacters == 0))
    {
      removeAll = true;
    }

    if(mImpl->mEventData->mPreEditFlag || removeAll || // If the preedit flag is enabled, it means two (or more) of them came together i.e. when two keys have been pressed at the same time.
       ((cursorIndex + numberOfCharacters) <= mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters))
    {
      // Mark the paragraphs to be updated.
      if(Layout::Engine::SINGLE_LINE_BOX == mImpl->mLayoutEngine.GetLayout())
      {
        mImpl->mTextUpdateInfo.mCharacterIndex             = 0;
        mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;
        mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd    = mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters - numberOfCharacters;
        mImpl->mTextUpdateInfo.mClearAll                   = true;
      }
      else
      {
        mImpl->mTextUpdateInfo.mCharacterIndex = std::min(cursorIndex, mImpl->mTextUpdateInfo.mCharacterIndex);
        mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove += numberOfCharacters;
      }

      // Update the input style and remove the text's style before removing the text.

      if(UPDATE_INPUT_STYLE == type)
      {
        // Keep a copy of the current input style.
        InputStyle currentInputStyle;
        currentInputStyle.Copy(mImpl->mEventData->mInputStyle);

        // Set first the default input style.
        mImpl->RetrieveDefaultInputStyle(mImpl->mEventData->mInputStyle);

        // Update the input style.
        mImpl->mModel->mLogicalModel->RetrieveStyle(cursorIndex, mImpl->mEventData->mInputStyle);

        // Compare if the input style has changed.
        const bool hasInputStyleChanged = !currentInputStyle.Equal(mImpl->mEventData->mInputStyle);

        if(hasInputStyleChanged)
        {
          const InputStyle::Mask styleChangedMask = currentInputStyle.GetInputStyleChangeMask(mImpl->mEventData->mInputStyle);
          // Queue the input style changed signal.
          mImpl->mEventData->mInputStyleChangedQueue.PushBack(styleChangedMask);
        }
      }

      // If the number of current text and the number of characters to be deleted are same,
      // it means all texts should be removed and all Preedit variables should be initialized.
      if(removeAll)
      {
        mImpl->ClearPreEditFlag();
        mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd = 0;
      }

      // Updates the text style runs by removing characters. Runs with no characters are removed.
      mImpl->mModel->mLogicalModel->UpdateTextStyleRuns(cursorIndex, -numberOfCharacters);

      // Remove the characters.
      Vector<Character>::Iterator first = currentText.Begin() + cursorIndex;
      Vector<Character>::Iterator last  = first + numberOfCharacters;

      currentText.Erase(first, last);

      // Cursor position retreat
      oldCursorIndex = cursorIndex;

      mImpl->mEventData->mScrollAfterDelete = true;

      if(EventData::INACTIVE == mImpl->mEventData->mState)
      {
        mImpl->ChangeState(EventData::EDITING);
      }

      DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::RemoveText %p removed %d\n", this, numberOfCharacters);
      removeAll = false;
      removed = true;
    }
  }

  return removed;
}

bool Controller::RemoveSelectedText()
{
  bool textRemoved(false);

  if(EventData::SELECTING == mImpl->mEventData->mState)
  {
    std::string removedString;
    mImpl->RetrieveSelection(removedString, true);

    if(!removedString.empty())
    {
      textRemoved = true;
      mImpl->ChangeState(EventData::EDITING);
    }
  }

  return textRemoved;
}

// private : Relayout.

bool Controller::DoRelayout(const Size&    size,
                            OperationsMask operationsRequired,
                            Size&          layoutSize)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::DoRelayout %p size %f,%f\n", this, size.width, size.height);
  bool viewUpdated(false);

  // Calculate the operations to be done.
  const OperationsMask operations = static_cast<OperationsMask>(mImpl->mOperationsPending & operationsRequired);

  const CharacterIndex startIndex                  = mImpl->mTextUpdateInfo.mParagraphCharacterIndex;
  const Length         requestedNumberOfCharacters = mImpl->mTextUpdateInfo.mRequestedNumberOfCharacters;

  // Get the current layout size.
  layoutSize = mImpl->mModel->mVisualModel->GetLayoutSize();

  if(NO_OPERATION != (LAYOUT & operations))
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::DoRelayout LAYOUT & operations\n");

    // Some vectors with data needed to layout and reorder may be void
    // after the first time the text has been laid out.
    // Fill the vectors again.

    // Calculate the number of glyphs to layout.
    const Vector<GlyphIndex>& charactersToGlyph        = mImpl->mModel->mVisualModel->mCharactersToGlyph;
    const Vector<Length>&     glyphsPerCharacter       = mImpl->mModel->mVisualModel->mGlyphsPerCharacter;
    const GlyphIndex* const   charactersToGlyphBuffer  = charactersToGlyph.Begin();
    const Length* const       glyphsPerCharacterBuffer = glyphsPerCharacter.Begin();

    const CharacterIndex lastIndex       = startIndex + ((requestedNumberOfCharacters > 0u) ? requestedNumberOfCharacters - 1u : 0u);
    const GlyphIndex     startGlyphIndex = mImpl->mTextUpdateInfo.mStartGlyphIndex;

    // Make sure the index is not out of bound
    if(charactersToGlyph.Count() != glyphsPerCharacter.Count() ||
       requestedNumberOfCharacters > charactersToGlyph.Count() ||
       (lastIndex > charactersToGlyph.Count() && charactersToGlyph.Count() > 0u))
    {
      std::string currentText;
      GetText(currentText);

      DALI_LOG_ERROR("Controller::DoRelayout: Attempting to access invalid buffer\n");
      DALI_LOG_ERROR("Current text is: %s\n", currentText.c_str());
      DALI_LOG_ERROR("startIndex: %u, lastIndex: %u, requestedNumberOfCharacters: %u, charactersToGlyph.Count = %lu, glyphsPerCharacter.Count = %lu\n", startIndex, lastIndex, requestedNumberOfCharacters, charactersToGlyph.Count(), glyphsPerCharacter.Count());

      return false;
    }

    const Length numberOfGlyphs      = (requestedNumberOfCharacters > 0u) ? *(charactersToGlyphBuffer + lastIndex) + *(glyphsPerCharacterBuffer + lastIndex) - startGlyphIndex : 0u;
    const Length totalNumberOfGlyphs = mImpl->mModel->mVisualModel->mGlyphs.Count();

    if(0u == totalNumberOfGlyphs)
    {
      if(NO_OPERATION != (UPDATE_LAYOUT_SIZE & operations))
      {
        mImpl->mModel->mVisualModel->SetLayoutSize(Size::ZERO);
      }

      // Nothing else to do if there is no glyphs.
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::DoRelayout no glyphs, view updated true\n");
      return true;
    }

    // Set the layout parameters.
    Layout::Parameters layoutParameters(size,
                                        mImpl->mModel);

    // Resize the vector of positions to have the same size than the vector of glyphs.
    Vector<Vector2>& glyphPositions = mImpl->mModel->mVisualModel->mGlyphPositions;
    glyphPositions.Resize(totalNumberOfGlyphs);

    // Whether the last character is a new paragraph character.
    const Character* const textBuffer                   = mImpl->mModel->mLogicalModel->mText.Begin();
    mImpl->mTextUpdateInfo.mIsLastCharacterNewParagraph = TextAbstraction::IsNewParagraph(*(textBuffer + (mImpl->mModel->mLogicalModel->mText.Count() - 1u)));
    layoutParameters.isLastNewParagraph                 = mImpl->mTextUpdateInfo.mIsLastCharacterNewParagraph;

    // The initial glyph and the number of glyphs to layout.
    layoutParameters.startGlyphIndex        = startGlyphIndex;
    layoutParameters.numberOfGlyphs         = numberOfGlyphs;
    layoutParameters.startLineIndex         = mImpl->mTextUpdateInfo.mStartLineIndex;
    layoutParameters.estimatedNumberOfLines = mImpl->mTextUpdateInfo.mEstimatedNumberOfLines;

    // Update the ellipsis
    bool elideTextEnabled = mImpl->mModel->mElideEnabled;

    if(NULL != mImpl->mEventData)
    {
      if(mImpl->mEventData->mPlaceholderEllipsisFlag && mImpl->IsShowingPlaceholderText())
      {
        elideTextEnabled = mImpl->mEventData->mIsPlaceholderElideEnabled;
      }
      else if(EventData::INACTIVE != mImpl->mEventData->mState)
      {
        // Disable ellipsis when editing
        elideTextEnabled = false;
      }

      // Reset the scroll position in inactive state
      if(elideTextEnabled && (mImpl->mEventData->mState == EventData::INACTIVE))
      {
        ResetScrollPosition();
      }
    }

    // Update the visual model.
    bool isAutoScrollEnabled = mImpl->mIsAutoScrollEnabled;
    Size newLayoutSize;
    viewUpdated                 = mImpl->mLayoutEngine.LayoutText(layoutParameters,
                                                  newLayoutSize,
                                                  elideTextEnabled,
                                                  isAutoScrollEnabled);
    mImpl->mIsAutoScrollEnabled = isAutoScrollEnabled;

    viewUpdated = viewUpdated || (newLayoutSize != layoutSize);

    if(viewUpdated)
    {
      layoutSize = newLayoutSize;

      if(NO_OPERATION != (UPDATE_DIRECTION & operations))
      {
        mImpl->mIsTextDirectionRTL = false;
      }

      if((NO_OPERATION != (UPDATE_DIRECTION & operations)) && !mImpl->mModel->mVisualModel->mLines.Empty())
      {
        mImpl->mIsTextDirectionRTL = mImpl->mModel->mVisualModel->mLines[0u].direction;
      }

      // Sets the layout size.
      if(NO_OPERATION != (UPDATE_LAYOUT_SIZE & operations))
      {
        mImpl->mModel->mVisualModel->SetLayoutSize(layoutSize);
      }
    } // view updated
  }

  if(NO_OPERATION != (ALIGN & operations))
  {
    // The laid-out lines.
    Vector<LineRun>& lines = mImpl->mModel->mVisualModel->mLines;

    CharacterIndex alignStartIndex                  = startIndex;
    Length         alignRequestedNumberOfCharacters = requestedNumberOfCharacters;

    // the whole text needs to be full aligned.
    // If you do not do a full aligned, only the last line of the multiline input is aligned.
    if(mImpl->mEventData && mImpl->mEventData->mUpdateAlignment)
    {
      alignStartIndex                     = 0u;
      alignRequestedNumberOfCharacters    = mImpl->mModel->mLogicalModel->mText.Count();
      mImpl->mEventData->mUpdateAlignment = false;
    }

    // Need to align with the control's size as the text may contain lines
    // starting either with left to right text or right to left.
    mImpl->mLayoutEngine.Align(size,
                               alignStartIndex,
                               alignRequestedNumberOfCharacters,
                               mImpl->mModel->mHorizontalAlignment,
                               lines,
                               mImpl->mModel->mAlignmentOffset,
                               mImpl->mLayoutDirection,
                               mImpl->mModel->mMatchSystemLanguageDirection);

    viewUpdated = true;
  }
#if defined(DEBUG_ENABLED)
  std::string currentText;
  GetText(currentText);
  DALI_LOG_INFO(gLogFilter, Debug::Concise, "Controller::DoRelayout [%p] mImpl->mIsTextDirectionRTL[%s] [%s]\n", this, (mImpl->mIsTextDirectionRTL) ? "true" : "false", currentText.c_str());
#endif
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::DoRelayout, view updated %s\n", (viewUpdated ? "true" : "false"));
  return viewUpdated;
}

void Controller::CalculateVerticalOffset(const Size& controlSize)
{
  ModelPtr&       model                 = mImpl->mModel;
  VisualModelPtr& visualModel           = model->mVisualModel;
  Size            layoutSize            = mImpl->mModel->mVisualModel->GetLayoutSize();
  Size            oldLayoutSize         = layoutSize;
  float           offsetY               = 0.f;
  bool            needRecalc            = false;
  float           defaultFontLineHeight = mImpl->GetDefaultFontLineHeight();

  if(fabsf(layoutSize.height) < Math::MACHINE_EPSILON_1000)
  {
    // Get the line height of the default font.
    layoutSize.height = defaultFontLineHeight;
  }

  // Whether the text control is editable
  const bool isEditable = NULL != mImpl->mEventData;
  if(isEditable && layoutSize.height != defaultFontLineHeight)
  {
    // This code prevents the wrong positioning of cursor when the layout size is bigger/smaller than defaultFontLineHeight.
    // This situation occurs when the size of placeholder text is different from the default text.
    layoutSize.height = defaultFontLineHeight;
    needRecalc        = true;
  }

  switch(mImpl->mModel->mVerticalAlignment)
  {
    case VerticalAlignment::TOP:
    {
      mImpl->mModel->mScrollPosition.y = 0.f;
      offsetY                          = 0.f;
      break;
    }
    case VerticalAlignment::CENTER:
    {
      mImpl->mModel->mScrollPosition.y = floorf(0.5f * (controlSize.height - layoutSize.height)); // try to avoid pixel alignment.
      if(needRecalc) offsetY = floorf(0.5f * (layoutSize.height - oldLayoutSize.height));
      break;
    }
    case VerticalAlignment::BOTTOM:
    {
      mImpl->mModel->mScrollPosition.y = controlSize.height - layoutSize.height;
      if(needRecalc) offsetY = layoutSize.height - oldLayoutSize.height;
      break;
    }
  }

  if(needRecalc)
  {
    // Update glyphPositions according to recalculation.
    const Length     positionCount  = visualModel->mGlyphPositions.Count();
    Vector<Vector2>& glyphPositions = visualModel->mGlyphPositions;
    for(Length index = 0u; index < positionCount; index++)
    {
      glyphPositions[index].y += offsetY;
    }
  }
}

// private : Events.

void Controller::ProcessModifyEvents()
{
  EventHandler::ProcessModifyEvents(*this);
}

void Controller::TextReplacedEvent()
{
  EventHandler::TextReplacedEvent(*this);
}

void Controller::TextInsertedEvent()
{
  EventHandler::TextInsertedEvent(*this);
}

void Controller::TextDeletedEvent()
{
  EventHandler::TextDeletedEvent(*this);
}

bool Controller::DeleteEvent(int keyCode)
{
  return EventHandler::DeleteEvent(*this, keyCode);
}

// private : Helpers.

void Controller::ResetText()
{
  // Reset buffers.
  mImpl->mModel->mLogicalModel->mText.Clear();

  // Reset the embedded images buffer.
  mImpl->mModel->mLogicalModel->ClearEmbeddedImages();

  // We have cleared everything including the placeholder-text
  mImpl->PlaceholderCleared();

  mImpl->mTextUpdateInfo.mCharacterIndex             = 0u;
  mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;
  mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd    = 0u;

  // Clear any previous text.
  mImpl->mTextUpdateInfo.mClearAll = true;

  // The natural size needs to be re-calculated.
  mImpl->mRecalculateNaturalSize = true;

  // The text direction needs to be updated.
  mImpl->mUpdateTextDirection = true;

  // Apply modifications to the model
  mImpl->mOperationsPending = ALL_OPERATIONS;
}

void Controller::ShowPlaceholderText()
{
  if(mImpl->IsPlaceholderAvailable())
  {
    DALI_ASSERT_DEBUG(mImpl->mEventData && "No placeholder text available");

    if(NULL == mImpl->mEventData)
    {
      return;
    }

    mImpl->mEventData->mIsShowingPlaceholderText = true;

    // Disable handles when showing place-holder text
    mImpl->mEventData->mDecorator->SetHandleActive(GRAB_HANDLE, false);
    mImpl->mEventData->mDecorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
    mImpl->mEventData->mDecorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);

    const char* text(NULL);
    size_t      size(0);

    // TODO - Switch Placeholder text when changing state
    if((EventData::INACTIVE != mImpl->mEventData->mState) &&
       (0u != mImpl->mEventData->mPlaceholderTextActive.c_str()))
    {
      text = mImpl->mEventData->mPlaceholderTextActive.c_str();
      size = mImpl->mEventData->mPlaceholderTextActive.size();
    }
    else
    {
      text = mImpl->mEventData->mPlaceholderTextInactive.c_str();
      size = mImpl->mEventData->mPlaceholderTextInactive.size();
    }

    mImpl->mTextUpdateInfo.mCharacterIndex             = 0u;
    mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;

    // Reset model for showing placeholder.
    mImpl->mModel->mLogicalModel->mText.Clear();
    mImpl->mModel->mVisualModel->SetTextColor(mImpl->mEventData->mPlaceholderTextColor);

    // Convert text into UTF-32
    Vector<Character>& utf32Characters = mImpl->mModel->mLogicalModel->mText;
    utf32Characters.Resize(size);

    // This is a bit horrible but std::string returns a (signed) char*
    const uint8_t* utf8 = reinterpret_cast<const uint8_t*>(text);

    // Transform a text array encoded in utf8 into an array encoded in utf32.
    // It returns the actual number of characters.
    const Length characterCount = Utf8ToUtf32(utf8, size, utf32Characters.Begin());
    utf32Characters.Resize(characterCount);

    // The characters to be added.
    mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd = characterCount;

    // Reset the cursor position
    mImpl->mEventData->mPrimaryCursorPosition = 0;

    // The natural size needs to be re-calculated.
    mImpl->mRecalculateNaturalSize = true;

    // The text direction needs to be updated.
    mImpl->mUpdateTextDirection = true;

    // Apply modifications to the model
    mImpl->mOperationsPending = ALL_OPERATIONS;

    // Update the rest of the model during size negotiation
    mImpl->QueueModifyEvent(ModifyEvent::TEXT_REPLACED);
  }
}

void Controller::ClearFontData()
{
  if(mImpl->mFontDefaults)
  {
    mImpl->mFontDefaults->mFontId = 0u; // Remove old font ID
  }

  // Set flags to update the model.
  mImpl->mTextUpdateInfo.mCharacterIndex             = 0u;
  mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;
  mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd    = mImpl->mModel->mLogicalModel->mText.Count();

  mImpl->mTextUpdateInfo.mClearAll           = true;
  mImpl->mTextUpdateInfo.mFullRelayoutNeeded = true;
  mImpl->mRecalculateNaturalSize             = true;

  mImpl->mOperationsPending = static_cast<OperationsMask>(mImpl->mOperationsPending |
                                                          VALIDATE_FONTS |
                                                          SHAPE_TEXT |
                                                          BIDI_INFO |
                                                          GET_GLYPH_METRICS |
                                                          LAYOUT |
                                                          UPDATE_LAYOUT_SIZE |
                                                          REORDER |
                                                          ALIGN);
}

void Controller::ClearStyleData()
{
  mImpl->mModel->mLogicalModel->mColorRuns.Clear();
  mImpl->mModel->mLogicalModel->ClearFontDescriptionRuns();
}

void Controller::ResetCursorPosition(CharacterIndex cursorIndex)
{
  // Reset the cursor position
  if(NULL != mImpl->mEventData)
  {
    mImpl->mEventData->mPrimaryCursorPosition = cursorIndex;

    // Update the cursor if it's in editing mode.
    if(EventData::IsEditingState(mImpl->mEventData->mState))
    {
      mImpl->mEventData->mUpdateCursorPosition = true;
    }
  }
}

void Controller::ResetScrollPosition()
{
  if(NULL != mImpl->mEventData)
  {
    // Reset the scroll position.
    mImpl->mModel->mScrollPosition                = Vector2::ZERO;
    mImpl->mEventData->mScrollAfterUpdatePosition = true;
  }
}

void Controller::SetControlInterface(ControlInterface* controlInterface)
{
  mImpl->mControlInterface = controlInterface;
}

bool Controller::ShouldClearFocusOnEscape() const
{
  return mImpl->mShouldClearFocusOnEscape;
}

Actor Controller::CreateBackgroundActor()
{
  return mImpl->CreateBackgroundActor();
}

// private : Private contructors & copy operator.

Controller::Controller()
: Controller(nullptr, nullptr, nullptr)
{
}

Controller::Controller(ControlInterface* controlInterface)
: Controller(controlInterface, nullptr, nullptr)
{
}

Controller::Controller(ControlInterface*           controlInterface,
                       EditableControlInterface*   editableControlInterface,
                       SelectableControlInterface* selectableControlInterface)
: mImpl(new Controller::Impl(controlInterface, editableControlInterface, selectableControlInterface))
{
}

// The copy constructor and operator are left unimplemented.

// protected : Destructor.

Controller::~Controller()
{
  delete mImpl;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
