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
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/integration-api/debug.h>
#include <memory.h>
#include <cmath>
#include <limits>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali-toolkit/internal/text/text-controller-event-handler.h>
#include <dali-toolkit/internal/text/text-controller-impl.h>
#include <dali-toolkit/internal/text/text-controller-input-font-handler.h>
#include <dali-toolkit/internal/text/text-controller-input-properties.h>
#include <dali-toolkit/internal/text/text-controller-placeholder-handler.h>
#include <dali-toolkit/internal/text/text-controller-relayouter.h>
#include <dali-toolkit/internal/text/text-controller-text-updater.h>
#include <dali-toolkit/internal/text/text-editable-control-interface.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

constexpr float MAX_FLOAT = std::numeric_limits<float>::max();

const std::string EMPTY_STRING("");

template<typename Type>
void EnsureCreated(Type*& object)
{
  if(!object)
  {
    object = new Type();
  }
}

template<typename Type>
void EnsureCreated(std::unique_ptr<Type>& object)
{
  if(!object)
  {
    object = std::unique_ptr<Type>(new Type());
  }
}

template<typename Type, typename Arg1>
void EnsureCreated(Type*& object, Arg1 arg1)
{
  if(!object)
  {
    object = new Type(arg1);
  }
}

template<typename Type, typename Arg1, typename Arg2>
void EnsureCreated(Type*& object, Arg1 arg1, Arg2 arg2)
{
  if(!object)
  {
    object = new Type(arg1, arg2);
  }
}

float GetDpi()
{
  unsigned int                      horizontalDpi = 0u;
  unsigned int                      verticalDpi   = 0u;
  Dali::TextAbstraction::FontClient fontClient    = Dali::TextAbstraction::FontClient::Get();
  fontClient.GetDpi(horizontalDpi, verticalDpi);
  return static_cast<float>(horizontalDpi);
}

float ConvertPixelToPoint(float pixel)
{
  return pixel * 72.0f / GetDpi();
}

float ConvertPointToPixel(float point)
{
  // Pixel size = Point size * DPI / 72.f
  return point * GetDpi() / 72.0f;
}

void UpdateCursorPosition(Dali::Toolkit::Text::EventData* eventData)
{
  if(eventData && Dali::Toolkit::Text::EventData::IsEditingState(eventData->mState))
  {
    // Update the cursor position if it's in editing mode
    eventData->mDecoratorUpdated     = true;
    eventData->mUpdateCursorPosition = true; // Cursor position should be updated when the font size is updated.
  }
}

} // namespace

namespace Dali::Toolkit::Text
{

void Controller::EnableTextInput(DecoratorPtr decorator, InputMethodContext& inputMethodContext)
{
  if(!decorator)
  {
    delete mImpl->mEventData;
    mImpl->mEventData = NULL;

    // Nothing else to do.
    return;
  }

  EnsureCreated(mImpl->mEventData, decorator, inputMethodContext);
}

void Controller::SetGlyphType(TextAbstraction::GlyphType glyphType)
{
  // Metrics for bitmap & vector based glyphs are different
  mImpl->mMetrics->SetGlyphType(glyphType);

  // Clear the font-specific data
  mImpl->ClearFontData();

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

  mImpl->mModel->mVisualModel->SetMarkupProcessorEnabled(enable);
}

bool Controller::IsMarkupProcessorEnabled() const
{
  return mImpl->mMarkupProcessorEnabled;
}

bool Controller::HasAnchors() const
{
  return (mImpl->mMarkupProcessorEnabled && mImpl->mModel->mLogicalModel->mAnchors.Count() && mImpl->IsShowingRealText());
}

void Controller::SetAutoScrollEnabled(bool enable)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::SetAutoScrollEnabled[%s] SingleBox[%s]-> [%p]\n", (enable) ? "true" : "false", (mImpl->mLayoutEngine.GetLayout() == Layout::Engine::SINGLE_LINE_BOX) ? "true" : "false", this);
  mImpl->SetAutoScrollEnabled(enable);
}

void Controller::SetAutoScrollMaxTextureExceeded(bool exceed)
{
  mImpl->mIsAutoScrollMaxTextureExceeded = exceed;
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
  if(mImpl->mModel->mVisualModel && (0u != mImpl->mModel->mVisualModel->mLines.Count()))
  {
    offset = (*mImpl->mModel->mVisualModel->mLines.Begin()).alignmentOffset;
  }
  return offset;
}

void Controller::SetHorizontalScrollEnabled(bool enable)
{
  if(mImpl->mEventData && mImpl->mEventData->mDecorator)
  {
    mImpl->mEventData->mDecorator->SetHorizontalScrollEnabled(enable);
  }
}

bool Controller::IsHorizontalScrollEnabled() const
{
  return mImpl->mEventData && mImpl->mEventData->mDecorator && mImpl->mEventData->mDecorator->IsHorizontalScrollEnabled();
}

void Controller::SetVerticalScrollEnabled(bool enable)
{
  if(mImpl->mEventData && mImpl->mEventData->mDecorator)
  {
    mImpl->mEventData->mDecorator->SetVerticalScrollEnabled(enable);
  }
}

bool Controller::IsVerticalScrollEnabled() const
{
  return mImpl->mEventData && mImpl->mEventData->mDecorator && mImpl->mEventData->mDecorator->IsVerticalScrollEnabled();
}

void Controller::SetSmoothHandlePanEnabled(bool enable)
{
  if(mImpl->mEventData && mImpl->mEventData->mDecorator)
  {
    mImpl->mEventData->mDecorator->SetSmoothHandlePanEnabled(enable);
  }
}

bool Controller::IsSmoothHandlePanEnabled() const
{
  return mImpl->mEventData && mImpl->mEventData->mDecorator && mImpl->mEventData->mDecorator->IsSmoothHandlePanEnabled();
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
  mImpl->SetEnableCursorBlink(enable);
}

bool Controller::GetEnableCursorBlink() const
{
  return mImpl->mEventData && mImpl->mEventData->mCursorBlinkEnabled;
}

void Controller::SetMultiLineEnabled(bool enable)
{
  mImpl->SetMultiLineEnabled(enable);
}

bool Controller::IsMultiLineEnabled() const
{
  return Layout::Engine::MULTI_LINE_BOX == mImpl->mLayoutEngine.GetLayout();
}

void Controller::SetHorizontalAlignment(Text::HorizontalAlignment::Type alignment)
{
  mImpl->SetHorizontalAlignment(alignment);
}

Text::HorizontalAlignment::Type Controller::GetHorizontalAlignment() const
{
  return mImpl->mModel->mHorizontalAlignment;
}

void Controller::SetVerticalAlignment(VerticalAlignment::Type alignment)
{
  mImpl->SetVerticalAlignment(alignment);
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

void Controller::ChangedLayoutDirection()
{
  mImpl->mIsLayoutDirectionChanged = true;
}

void Controller::SetMatchLayoutDirection(DevelText::MatchLayoutDirection type)
{
  mImpl->mModel->mMatchLayoutDirection = type;
}

DevelText::MatchLayoutDirection Controller::GetMatchLayoutDirection() const
{
  return mImpl->mModel->mMatchLayoutDirection;
}

void Controller::SetLayoutDirection(Dali::LayoutDirection::Type layoutDirection)
{
  mImpl->mLayoutDirection = layoutDirection;
}

Dali::LayoutDirection::Type Controller::GetLayoutDirection(Dali::Actor& actor) const
{
  if(mImpl->mModel->mMatchLayoutDirection == DevelText::MatchLayoutDirection::LOCALE ||
     (mImpl->mModel->mMatchLayoutDirection == DevelText::MatchLayoutDirection::INHERIT && !mImpl->mIsLayoutDirectionChanged))
  {
    return static_cast<Dali::LayoutDirection::Type>(DevelWindow::Get(actor).GetRootLayer().GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());
  }
  else
  {
    return static_cast<Dali::LayoutDirection::Type>(actor.GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());
  }
}

bool Controller::IsShowingRealText() const
{
  return mImpl->IsShowingRealText();
}

void Controller::SetLineWrapMode(Text::LineWrap::Mode lineWrapMode)
{
  mImpl->SetLineWrapMode(lineWrapMode);
}

Text::LineWrap::Mode Controller::GetLineWrapMode() const
{
  return mImpl->mModel->mLineWrapMode;
}

void Controller::SetTextElideEnabled(bool enabled)
{
  mImpl->mModel->mElideEnabled = enabled;
  mImpl->mModel->mVisualModel->SetTextElideEnabled(enabled);
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

void Controller::SetTextFitChanged(bool changed)
{
  mImpl->mTextFitChanged = changed;
}

bool Controller::IsTextFitChanged() const
{
  return mImpl->mTextFitChanged;
}

void Controller::SetTextFitMinSize(float minSize, FontSizeType type)
{
  mImpl->mTextFitMinSize = (type == POINT_SIZE) ? minSize : ConvertPixelToPoint(minSize);
}

float Controller::GetTextFitMinSize() const
{
  return mImpl->mTextFitMinSize;
}

void Controller::SetTextFitMaxSize(float maxSize, FontSizeType type)
{
  mImpl->mTextFitMaxSize = (type == POINT_SIZE) ? maxSize : ConvertPixelToPoint(maxSize);
}

float Controller::GetTextFitMaxSize() const
{
  return mImpl->mTextFitMaxSize;
}

void Controller::SetTextFitStepSize(float step, FontSizeType type)
{
  mImpl->mTextFitStepSize = (type == POINT_SIZE) ? step : ConvertPixelToPoint(step);
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

float Controller::GetTextFitPointSize() const
{
  return mImpl->mFontDefaults ? mImpl->mFontDefaults->mFitPointSize : 0.0f;
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

void Controller::SetText(const std::string& text)
{
  TextUpdater::SetText(*this, text);
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
  mImpl->UpdateAfterFontChange(newDefaultFont);
}

void Controller::RetrieveSelection(std::string& selectedText) const
{
  mImpl->RetrieveSelection(selectedText, false);
}

void Controller::SetSelection(int start, int end)
{
  mImpl->SetSelection(start, end);
}

std::pair<int, int> Controller::GetSelectionIndexes() const
{
  return mImpl->GetSelectionIndexes();
}

void Controller::CopyStringToClipboard(const std::string& source)
{
  mImpl->CopyStringToClipboard(source);
}

void Controller::SendSelectionToClipboard(bool deleteAfterSending)
{
  mImpl->SendSelectionToClipboard(deleteAfterSending);
}

void Controller::SetDefaultFontFamily(const std::string& defaultFontFamily)
{
  EnsureCreated(mImpl->mFontDefaults);

  mImpl->mFontDefaults->mFontDescription.family = defaultFontFamily;
  DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::SetDefaultFontFamily %s\n", defaultFontFamily.c_str());
  mImpl->mFontDefaults->familyDefined = !defaultFontFamily.empty();

  // Update the cursor position if it's in editing mode
  UpdateCursorPosition(mImpl->mEventData);

  // Clear the font-specific data
  mImpl->ClearFontData();

  mImpl->RequestRelayout();
}

const std::string& Controller::GetDefaultFontFamily() const
{
  return mImpl->mFontDefaults ? mImpl->mFontDefaults->mFontDescription.family : EMPTY_STRING;
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
  EnsureCreated(mImpl->mFontDefaults);

  mImpl->mFontDefaults->mFontDescription.weight = weight;
  mImpl->mFontDefaults->weightDefined           = true;

  // Update the cursor position if it's in editing mode
  UpdateCursorPosition(mImpl->mEventData);

  // Clear the font-specific data
  mImpl->ClearFontData();

  mImpl->RequestRelayout();
}

bool Controller::IsDefaultFontWeightDefined() const
{
  return mImpl->mFontDefaults && mImpl->mFontDefaults->weightDefined;
}

FontWeight Controller::GetDefaultFontWeight() const
{
  return mImpl->mFontDefaults ? mImpl->mFontDefaults->mFontDescription.weight : TextAbstraction::FontWeight::NORMAL;
}

void Controller::SetPlaceholderTextFontWeight(FontWeight weight)
{
  PlaceholderHandler::SetPlaceholderTextFontWeight(*this, weight);
}

bool Controller::IsPlaceholderTextFontWeightDefined() const
{
  return PlaceholderHandler::IsPlaceholderTextFontWeightDefined(*this);
}

FontWeight Controller::GetPlaceholderTextFontWeight() const
{
  return PlaceholderHandler::GetPlaceholderTextFontWeight(*this);
}

void Controller::SetDefaultFontWidth(FontWidth width)
{
  EnsureCreated(mImpl->mFontDefaults);

  mImpl->mFontDefaults->mFontDescription.width = width;
  mImpl->mFontDefaults->widthDefined           = true;

  // Update the cursor position if it's in editing mode
  UpdateCursorPosition(mImpl->mEventData);

  // Clear the font-specific data
  mImpl->ClearFontData();

  mImpl->RequestRelayout();
}

bool Controller::IsDefaultFontWidthDefined() const
{
  return mImpl->mFontDefaults && mImpl->mFontDefaults->widthDefined;
}

FontWidth Controller::GetDefaultFontWidth() const
{
  return mImpl->mFontDefaults ? mImpl->mFontDefaults->mFontDescription.width : TextAbstraction::FontWidth::NORMAL;
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
  EnsureCreated(mImpl->mFontDefaults);

  mImpl->mFontDefaults->mFontDescription.slant = slant;
  mImpl->mFontDefaults->slantDefined           = true;

  // Update the cursor position if it's in editing mode
  UpdateCursorPosition(mImpl->mEventData);

  // Clear the font-specific data
  mImpl->ClearFontData();

  mImpl->RequestRelayout();
}

bool Controller::IsDefaultFontSlantDefined() const
{
  return mImpl->mFontDefaults && mImpl->mFontDefaults->slantDefined;
}

FontSlant Controller::GetDefaultFontSlant() const
{
  return mImpl->mFontDefaults ? mImpl->mFontDefaults->mFontDescription.slant : TextAbstraction::FontSlant::NORMAL;
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

void Controller::SetFontSizeScale(float scale)
{
  mImpl->mFontSizeScale = scale;

  // Update the cursor position if it's in editing mode
  UpdateCursorPosition(mImpl->mEventData);

  // Clear the font-specific data
  mImpl->ClearFontData();

  mImpl->RequestRelayout();
}

float Controller::GetFontSizeScale() const
{
  return mImpl->mFontDefaults ? mImpl->mFontSizeScale : 1.0f;
}

void Controller::SetDefaultFontSize(float fontSize, FontSizeType type)
{
  EnsureCreated(mImpl->mFontDefaults);

  mImpl->mFontDefaults->mDefaultPointSize = (type == POINT_SIZE) ? fontSize : ConvertPixelToPoint(fontSize);
  mImpl->mFontDefaults->sizeDefined       = true;

  // Update the cursor position if it's in editing mode
  UpdateCursorPosition(mImpl->mEventData);

  // Clear the font-specific data
  mImpl->ClearFontData();

  mImpl->RequestRelayout();
}

float Controller::GetDefaultFontSize(FontSizeType type) const
{
  if(mImpl->mFontDefaults)
  {
    return (type == POINT_SIZE) ? mImpl->mFontDefaults->mDefaultPointSize : ConvertPointToPixel(mImpl->mFontDefaults->mDefaultPointSize);
  }
  return 0.0f;
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
  mImpl->SetDefaultColor(color);
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
  EnsureCreated(mImpl->mEmbossDefaults);
  mImpl->mEmbossDefaults->properties = embossProperties;
}

const std::string& Controller::GetDefaultEmbossProperties() const
{
  return mImpl->mEmbossDefaults ? mImpl->mEmbossDefaults->properties : EMPTY_STRING;
}

void Controller::SetDefaultOutlineProperties(const std::string& outlineProperties)
{
  EnsureCreated(mImpl->mOutlineDefaults);
  mImpl->mOutlineDefaults->properties = outlineProperties;
}

const std::string& Controller::GetDefaultOutlineProperties() const
{
  return mImpl->mOutlineDefaults ? mImpl->mOutlineDefaults->properties : EMPTY_STRING;
}

bool Controller::SetDefaultLineSpacing(float lineSpacing)
{
  return mImpl->SetDefaultLineSpacing(lineSpacing);
}

float Controller::GetDefaultLineSpacing() const
{
  return mImpl->mLayoutEngine.GetDefaultLineSpacing();
}

bool Controller::SetDefaultLineSize(float lineSize)
{
  return mImpl->SetDefaultLineSize(lineSize);
}

float Controller::GetDefaultLineSize() const
{
  return mImpl->mLayoutEngine.GetDefaultLineSize();
}

void Controller::SetInputColor(const Vector4& color)
{
  InputProperties::SetInputColor(*this, color);
}

const Vector4& Controller::GetInputColor() const
{
  return InputProperties::GetInputColor(*this);
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
  InputProperties::SetInputLineSpacing(*this, lineSpacing);
}

float Controller::GetInputLineSpacing() const
{
  return InputProperties::GetInputLineSpacing(*this);
}

void Controller::SetInputShadowProperties(const std::string& shadowProperties)
{
  InputProperties::SetInputShadowProperties(*this, shadowProperties);
}

const std::string& Controller::GetInputShadowProperties() const
{
  return InputProperties::GetInputShadowProperties(*this);
}

void Controller::SetInputUnderlineProperties(const std::string& underlineProperties)
{
  InputProperties::SetInputUnderlineProperties(*this, underlineProperties);
}

const std::string& Controller::GetInputUnderlineProperties() const
{
  return InputProperties::GetInputUnderlineProperties(*this);
}

void Controller::SetInputEmbossProperties(const std::string& embossProperties)
{
  InputProperties::SetInputEmbossProperties(*this, embossProperties);
}

const std::string& Controller::GetInputEmbossProperties() const
{
  return InputProperties::GetInputEmbossProperties(*this);
}

void Controller::SetInputOutlineProperties(const std::string& outlineProperties)
{
  InputProperties::SetInputOutlineProperties(*this, outlineProperties);
}

const std::string& Controller::GetInputOutlineProperties() const
{
  return InputProperties::GetInputOutlineProperties(*this);
}

void Controller::SetInputModePassword(bool passwordInput)
{
  InputProperties::SetInputModePassword(*this, passwordInput);
}

bool Controller::IsInputModePassword()
{
  return InputProperties::IsInputModePassword(*this);
}

void Controller::SetNoTextDoubleTapAction(NoTextTap::Action action)
{
  if(mImpl->mEventData)
  {
    mImpl->mEventData->mDoubleTapAction = action;
  }
}

Controller::NoTextTap::Action Controller::GetNoTextDoubleTapAction() const
{
  return mImpl->mEventData ? mImpl->mEventData->mDoubleTapAction : NoTextTap::NO_ACTION;
}

void Controller::SetNoTextLongPressAction(NoTextTap::Action action)
{
  if(mImpl->mEventData)
  {
    mImpl->mEventData->mLongPressAction = action;
  }
}

Controller::NoTextTap::Action Controller::GetNoTextLongPressAction() const
{
  return mImpl->mEventData ? mImpl->mEventData->mLongPressAction : NoTextTap::NO_ACTION;
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
  return Relayouter::GetNaturalSize(*this);
}

bool Controller::CheckForTextFit(float pointSize, Size& layoutSize)
{
  return Relayouter::CheckForTextFit(*this, pointSize, layoutSize);
}

void Controller::FitPointSizeforLayout(Size layoutSize)
{
  Relayouter::FitPointSizeforLayout(*this, layoutSize);
}

float Controller::GetHeightForWidth(float width)
{
  return Relayouter::GetHeightForWidth(*this, width);
}

int Controller::GetLineCount(float width)
{
  GetHeightForWidth(width);
  return mImpl->mModel->GetNumberOfLines();
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
  EnsureCreated<HiddenText, Controller*>(mImpl->mHiddenInput, this);
  mImpl->mHiddenInput->SetProperties(options);
}

void Controller::GetHiddenInputOption(Property::Map& options)
{
  if(mImpl->mHiddenInput)
  {
    mImpl->mHiddenInput->GetProperties(options);
  }
}

void Controller::SetInputFilterOption(const Property::Map& options)
{
  EnsureCreated(mImpl->mInputFilter);
  mImpl->mInputFilter->SetProperties(options);
}

void Controller::GetInputFilterOption(Property::Map& options)
{
  if(mImpl->mInputFilter)
  {
    mImpl->mInputFilter->GetProperties(options);
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

Toolkit::DevelText::EllipsisPosition::Type Controller::GetEllipsisPosition() const
{
  return mImpl->mModel->GetEllipsisPosition();
}

void Controller::SetEllipsisPosition(Toolkit::DevelText::EllipsisPosition::Type ellipsisPosition)
{
  mImpl->mModel->mEllipsisPosition = ellipsisPosition;
  mImpl->mModel->mVisualModel->SetEllipsisPosition(ellipsisPosition);
}

Controller::UpdateTextType Controller::Relayout(const Size& size, Dali::LayoutDirection::Type layoutDirection)
{
  return Relayouter::Relayout(*this, size, layoutDirection);
}

void Controller::RequestRelayout()
{
  mImpl->RequestRelayout();
}

bool Controller::IsInputStyleChangedSignalsQueueEmpty()
{
  return mImpl->IsInputStyleChangedSignalsQueueEmpty();
}

void Controller::ProcessInputStyleChangedSignals()
{
  mImpl->ProcessInputStyleChangedSignals();
}

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

void Controller::AnchorEvent(float x, float y)
{
  EventHandler::AnchorEvent(*this, x, y);
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

void Controller::SelectEvent(const uint32_t start, const uint32_t end, SelectionType selectType)
{
  EventHandler::SelectEvent(*this, start, end, selectType);
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

CharacterIndex Controller::GetPrimaryCursorPosition() const
{
  return mImpl->GetPrimaryCursorPosition();
}

bool Controller::SetPrimaryCursorPosition(CharacterIndex index, bool focused)
{
  if(mImpl->mEventData)
  {
    mImpl->mEventData->mCheckScrollAmount     = true;
    mImpl->mEventData->mIsLeftHandleSelected  = true;
    mImpl->mEventData->mIsRightHandleSelected = true;
    mImpl->mEventData->mCheckScrollAmount     = true;
    if(mImpl->SetPrimaryCursorPosition(index, focused) && focused)
    {
      KeyboardFocusGainEvent();
      return true;
    }
  }
  return false;
}

void Controller::SelectWholeText()
{
  SelectEvent(0.f, 0.f, SelectionType::ALL);
}

void Controller::SelectNone()
{
  SelectEvent(0.f, 0.f, SelectionType::NONE);
}

void Controller::SelectText(const uint32_t start, const uint32_t end)
{
  SelectEvent(start, end, SelectionType::RANGE);
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

string Controller::CopyText()
{
  string text;
  mImpl->RetrieveSelection(text, false);
  mImpl->SendSelectionToClipboard(false); // Text not modified

  mImpl->mEventData->mUpdateCursorPosition = true;

  mImpl->RequestRelayout(); // Cursor, Handles, Selection Highlight, Popup

  return text;
}

string Controller::CutText()
{
  string text;
  mImpl->RetrieveSelection(text, false);

  if(!IsEditable())
  {
    return EMPTY_STRING;
  }

  mImpl->SendSelectionToClipboard(true); // Synchronous call to modify text
  mImpl->mOperationsPending = ALL_OPERATIONS;

  if((0u != mImpl->mModel->mLogicalModel->mText.Count()) ||
     !mImpl->IsPlaceholderAvailable())
  {
    mImpl->QueueModifyEvent(ModifyEvent::TEXT_DELETED);
  }
  else
  {
    ShowPlaceholderText();
  }

  mImpl->mEventData->mUpdateCursorPosition = true;
  mImpl->mEventData->mScrollAfterDelete    = true;

  mImpl->RequestRelayout();

  if(nullptr != mImpl->mEditableControlInterface)
  {
    mImpl->mEditableControlInterface->TextChanged(true);
  }
  return text;
}

void Controller::PasteText()
{
  mImpl->RequestGetTextFromClipboard(); // Request clipboard service to retrieve an item
}

InputMethodContext::CallbackData Controller::OnInputMethodContextEvent(InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent)
{
  return EventHandler::OnInputMethodContextEvent(*this, inputMethodContext, inputMethodContextEvent);
}

void Controller::PasteClipboardItemEvent()
{
  EventHandler::PasteClipboardItemEvent(*this);
}

void Controller::GetTargetSize(Vector2& targetSize)
{
  targetSize = mImpl->mModel->mVisualModel->mControlSize;
}

void Controller::AddDecoration(Actor& actor, bool needsClipping)
{
  if(mImpl->mEditableControlInterface)
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
}

void Controller::ScrollBy(Vector2 scroll)
{
  mImpl->ScrollBy(scroll);
}

float Controller::GetHorizontalScrollPosition()
{
  return mImpl->GetHorizontalScrollPosition();
}

float Controller::GetVerticalScrollPosition()
{
  return mImpl->GetVerticalScrollPosition();
}

void Controller::DecorationEvent(HandleType handleType, HandleState state, float x, float y)
{
  EventHandler::DecorationEvent(*this, handleType, state, x, y);
}

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

void Controller::InsertText(const std::string& text, Controller::InsertType type)
{
  TextUpdater::InsertText(*this, text, type);
}

void Controller::PasteText(const std::string& stringToPaste)
{
  TextUpdater::PasteText(*this, stringToPaste);
}

bool Controller::RemoveText(int cursorOffset, int numberOfCharacters, UpdateInputStyleType type)
{
  return TextUpdater::RemoveText(*this, cursorOffset, numberOfCharacters, type);
}

bool Controller::RemoveSelectedText()
{
  return TextUpdater::RemoveSelectedText(*this);
}

void Controller::InsertTextAnchor(int numberOfCharacters, CharacterIndex previousCursorIndex)
{
  TextUpdater::InsertTextAnchor(*this, numberOfCharacters, previousCursorIndex);
}

void Controller::RemoveTextAnchor(int cursorOffset, int numberOfCharacters, CharacterIndex previousCursorIndex)
{
  TextUpdater::RemoveTextAnchor(*this, cursorOffset, numberOfCharacters, previousCursorIndex);
}

bool Controller::DoRelayout(const Size& size, OperationsMask operationsRequired, Size& layoutSize)
{
  return Relayouter::DoRelayout(*this, size, operationsRequired, layoutSize);
}

void Controller::CalculateVerticalOffset(const Size& controlSize)
{
  Relayouter::CalculateVerticalOffset(*this, controlSize);
}

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
  TextUpdater::ResetText(*this);
}

void Controller::ShowPlaceholderText()
{
  PlaceholderHandler::ShowPlaceholderText(*this);
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

CharacterIndex Controller::GetCursorPosition()
{
  return mImpl->mEventData ? mImpl->mEventData->mPrimaryCursorPosition : 0;
}

void Controller::SetControlInterface(ControlInterface* controlInterface)
{
  mImpl->mControlInterface = controlInterface;
}

void Controller::SetAnchorControlInterface(AnchorControlInterface* anchorControlInterface)
{
  mImpl->mAnchorControlInterface = anchorControlInterface;
}

bool Controller::ShouldClearFocusOnEscape() const
{
  return mImpl->mShouldClearFocusOnEscape;
}

Actor Controller::CreateBackgroundActor()
{
  return mImpl->CreateBackgroundActor();
}

Controller::Controller(ControlInterface*           controlInterface,
                       EditableControlInterface*   editableControlInterface,
                       SelectableControlInterface* selectableControlInterface,
                       AnchorControlInterface*     anchorControlInterface)
: mImpl(new Controller::Impl(controlInterface, editableControlInterface, selectableControlInterface, anchorControlInterface))
{
}

Controller::~Controller()
{
  delete mImpl;
}

} // namespace Dali::Toolkit::Text
