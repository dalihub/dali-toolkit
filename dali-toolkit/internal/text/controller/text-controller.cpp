/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/controller/text-controller.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <memory.h>
#include <cmath>
#include <limits>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali-toolkit/internal/text/controller/text-controller-background-actor.h>
#include <dali-toolkit/internal/text/controller/text-controller-event-handler.h>
#include <dali-toolkit/internal/text/controller/text-controller-impl.h>
#include <dali-toolkit/internal/text/controller/text-controller-input-font-handler.h>
#include <dali-toolkit/internal/text/controller/text-controller-input-properties.h>
#include <dali-toolkit/internal/text/controller/text-controller-placeholder-handler.h>
#include <dali-toolkit/internal/text/controller/text-controller-relayouter.h>
#include <dali-toolkit/internal/text/controller/text-controller-spannable-handler.h>
#include <dali-toolkit/internal/text/controller/text-controller-text-updater.h>
#include <dali-toolkit/internal/text/text-editable-control-interface.h>
#include <dali-toolkit/internal/text/text-geometry.h>
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

const char* EMPTY_STRING         = "";
const char* MIME_TYPE_TEXT_PLAIN = "text/plain;charset=utf-8";
const char* MIME_TYPE_HTML       = "application/xhtml+xml";

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
  static uint32_t horizontalDpi = 0u;
  static uint32_t verticalDpi   = 0u;

  // TODO : How can we know when fontClient DPI changed case?
  if(DALI_UNLIKELY(horizontalDpi == 0u))
  {
    Dali::TextAbstraction::FontClient fontClient = Dali::TextAbstraction::FontClient::Get();
    fontClient.GetDpi(horizontalDpi, verticalDpi);
  }
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

bool Controller::IsRemoveFrontInset() const
{
  return mImpl->mModel->mRemoveFrontInset;
}

void Controller::SetRemoveFrontInset(bool remove)
{
  mImpl->mModel->mRemoveFrontInset = remove;
}

bool Controller::IsRemoveBackInset() const
{
  return mImpl->mModel->mRemoveBackInset;
}

void Controller::SetRemoveBackInset(bool remove)
{
  mImpl->mModel->mRemoveBackInset = remove;
}

void Controller::ChangedLayoutDirection()
{
  mImpl->mIsLayoutDirectionChanged = true;
}

void Controller::ResetFontAndStyleData()
{
  mImpl->ResetFontAndStyleData();
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
  return mImpl->GetLayoutDirection(actor);
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

void Controller::SetCurrentLineSize(float lineSize)
{
  mImpl->mCurrentLineSize = lineSize;
}

float Controller::GetCurrentLineSize() const
{
  return mImpl->mCurrentLineSize;
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

void Controller::SetTextFitLineSize(float lineSize)
{
  mImpl->mTextFitLineSize = lineSize;
}

void Controller::SetTextFitArrayEnabled(bool enabled)
{
  mImpl->mTextFitArrayEnabled = enabled;
  mImpl->ClearFontData();
  mImpl->RequestRelayout();
}

bool Controller::IsTextFitArrayEnabled() const
{
  return mImpl->mTextFitArrayEnabled;
}

void Controller::SetTextFitArray(std::vector<Toolkit::DevelTextLabel::FitOption>& fitOptions)
{
  mImpl->mTextFitArray = fitOptions;
}

std::vector<Toolkit::DevelTextLabel::FitOption>& Controller::GetTextFitArray()
{
  return mImpl->mTextFitArray;
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
  mImpl->GetText(text);
}

Length Controller::GetNumberOfCharacters() const
{
  return mImpl->GetNumberOfCharacters();
}

void Controller::SetSpannedText(const Text::Spanned& spannedText)
{
  SpannableHandler::SetSpannedText(*this, spannedText);
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

  if(mImpl->mFontDefaults->mFontDescription.family != defaultFontFamily)
  {
    mImpl->mFontDefaults->mFontDescription.family = defaultFontFamily;
    DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::SetDefaultFontFamily %s\n", defaultFontFamily.c_str());
    mImpl->mFontDefaults->familyDefined = !defaultFontFamily.empty();

    // Update the cursor position if it's in editing mode
    UpdateCursorPosition(mImpl->mEventData);

    // Clear the font-specific data
    mImpl->ClearFontData();

    mImpl->RequestRelayout();
  }
}

std::string Controller::GetDefaultFontFamily() const
{
  return mImpl->mFontDefaults ? mImpl->mFontDefaults->mFontDescription.family : EMPTY_STRING;
}

void Controller::SetPlaceholderFontFamily(const std::string& placeholderTextFontFamily)
{
  PlaceholderHandler::SetPlaceholderFontFamily(*this, placeholderTextFontFamily);
}

std::string Controller::GetPlaceholderFontFamily() const
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

  // No relayout is required
  if(!mImpl->mFontSizeScaleEnabled) return;

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

void Controller::SetFontSizeScaleEnabled(bool enabled)
{
  mImpl->mFontSizeScaleEnabled = enabled;

  // Update the cursor position if it's in editing mode
  UpdateCursorPosition(mImpl->mEventData);

  // Clear the font-specific data
  mImpl->ClearFontData();

  mImpl->RequestRelayout();
}

bool Controller::IsFontSizeScaleEnabled() const
{
  return mImpl->mFontSizeScaleEnabled;
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

  if(mImpl->mEventData && EventData::INACTIVE != mImpl->mEventData->mState)
  {
    SetInputFontPointSize(fontSize, true);
  }
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

void Controller::SetAnchorColor(const Vector4& color)
{
  mImpl->SetAnchorColor(color);
}

const Vector4& Controller::GetAnchorColor() const
{
  return mImpl->GetAnchorColor();
}

void Controller::SetAnchorClickedColor(const Vector4& color)
{
  mImpl->SetAnchorClickedColor(color);
}

const Vector4& Controller::GetAnchorClickedColor() const
{
  return mImpl->GetAnchorClickedColor();
}

void Controller::SetDisabledColorOpacity(float opacity)
{
  mImpl->mDisabledColorOpacity = opacity;
}

float Controller::GetDisabledColorOpacity() const
{
  return mImpl->mDisabledColorOpacity;
}

void Controller::SetUserInteractionEnabled(bool enabled)
{
  mImpl->SetUserInteractionEnabled(enabled);
}

bool Controller::IsUserInteractionEnabled() const
{
  return mImpl->mIsUserInteractionEnabled;
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

void Controller::SetUnderlineType(Text::Underline::Type type)
{
  mImpl->mModel->mVisualModel->SetUnderlineType(type);

  mImpl->RequestRelayout();
}

Text::Underline::Type Controller::GetUnderlineType() const
{
  return mImpl->mModel->mVisualModel->GetUnderlineType();
}

void Controller::SetDashedUnderlineWidth(float width)
{
  mImpl->mModel->mVisualModel->SetDashedUnderlineWidth(width);

  mImpl->RequestRelayout();
}

float Controller::GetDashedUnderlineWidth() const
{
  return mImpl->mModel->mVisualModel->GetDashedUnderlineWidth();
}

void Controller::SetDashedUnderlineGap(float gap)
{
  mImpl->mModel->mVisualModel->SetDashedUnderlineGap(gap);

  mImpl->RequestRelayout();
}

float Controller::GetDashedUnderlineGap() const
{
  return mImpl->mModel->mVisualModel->GetDashedUnderlineGap();
}

void Controller::SetOutlineOffset(const Vector2& outlineOffset)
{
  mImpl->mModel->mVisualModel->SetOutlineOffset(outlineOffset);
  mImpl->RequestRelayout();
}

const Vector2& Controller::GetOutlineOffset() const
{
  return mImpl->mModel->mVisualModel->GetOutlineOffset();
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

void Controller::SetOutlineBlurRadius(const float& outlineBlurRadius)
{
  if(fabsf(GetOutlineBlurRadius() - outlineBlurRadius) > Math::MACHINE_EPSILON_1)
  {
    mImpl->mModel->mVisualModel->SetOutlineBlurRadius(outlineBlurRadius);
    mImpl->RequestRelayout();
  }
}

const float& Controller::GetOutlineBlurRadius() const
{
  return mImpl->mModel->mVisualModel->GetOutlineBlurRadius();
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

std::string Controller::GetDefaultEmbossProperties() const
{
  return mImpl->mEmbossDefaults ? mImpl->mEmbossDefaults->properties : EMPTY_STRING;
}

void Controller::SetDefaultOutlineProperties(const std::string& outlineProperties)
{
  EnsureCreated(mImpl->mOutlineDefaults);
  mImpl->mOutlineDefaults->properties = outlineProperties;
}

std::string Controller::GetDefaultOutlineProperties() const
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

bool Controller::SetRelativeLineSize(float relativeLineSize)
{
  return mImpl->SetRelativeLineSize(relativeLineSize);
}

float Controller::GetRelativeLineSize() const
{
  return mImpl->GetRelativeLineSize();
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

std::string Controller::GetInputFontFamily() const
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

void Controller::SetInputFontPointSize(float size, bool defaultFontSizeUpdated)
{
  InputFontHandler::SetInputFontPointSize(*this, size, defaultFontSizeUpdated);
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

std::string Controller::GetInputShadowProperties() const
{
  return InputProperties::GetInputShadowProperties(*this);
}

void Controller::SetInputUnderlineProperties(const std::string& underlineProperties)
{
  InputProperties::SetInputUnderlineProperties(*this, underlineProperties);
}

std::string Controller::GetInputUnderlineProperties() const
{
  return InputProperties::GetInputUnderlineProperties(*this);
}

void Controller::SetInputEmbossProperties(const std::string& embossProperties)
{
  InputProperties::SetInputEmbossProperties(*this, embossProperties);
}

std::string Controller::GetInputEmbossProperties() const
{
  return InputProperties::GetInputEmbossProperties(*this);
}

void Controller::SetInputOutlineProperties(const std::string& outlineProperties)
{
  InputProperties::SetInputOutlineProperties(*this, outlineProperties);
}

std::string Controller::GetInputOutlineProperties() const
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

void Controller::SetStrikethroughHeight(float height)
{
  mImpl->mModel->mVisualModel->SetStrikethroughHeight(height);

  mImpl->RequestRelayout();
}

float Controller::GetStrikethroughHeight() const
{
  return mImpl->mModel->mVisualModel->GetStrikethroughHeight();
}

void Controller::SetStrikethroughColor(const Vector4& color)
{
  mImpl->mModel->mVisualModel->SetStrikethroughColor(color);

  mImpl->RequestRelayout();
}

const Vector4& Controller::GetStrikethroughColor() const
{
  return mImpl->mModel->mVisualModel->GetStrikethroughColor();
}

void Controller::SetStrikethroughEnabled(bool enabled)
{
  mImpl->mModel->mVisualModel->SetStrikethroughEnabled(enabled);

  mImpl->RequestRelayout();
}

bool Controller::IsStrikethroughEnabled() const
{
  return mImpl->mModel->mVisualModel->IsStrikethroughEnabled();
}

void Controller::SetInputStrikethroughProperties(const std::string& strikethroughProperties)
{
  if(NULL != mImpl->mEventData)
  {
    mImpl->mEventData->mInputStyle.strikethroughProperties = strikethroughProperties;
  }
}

std::string Controller::GetInputStrikethroughProperties() const
{
  return (NULL != mImpl->mEventData) ? mImpl->mEventData->mInputStyle.strikethroughProperties : EMPTY_STRING;
}

bool Controller::IsStrikethroughSetByString()
{
  return mImpl->mStrikethroughSetByString;
}

void Controller::StrikethroughSetByString(bool setByString)
{
  mImpl->mStrikethroughSetByString = setByString;
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

void Controller::FitArrayPointSizeforLayout(Size layoutSize)
{
  Relayouter::FitArrayPointSizeforLayout(*this, layoutSize);
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

const ModelInterface* Controller::GetTextModel() const
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
  EventHandler::ProcessModifyEvents(*this);

  return mImpl->GetTextDirection();
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

void Controller::SetCharacterSpacing(float characterSpacing)
{
  mImpl->mModel->mVisualModel->SetCharacterSpacing(characterSpacing);

  mImpl->RelayoutAllCharacters();
  mImpl->RequestRelayout();
}

const float Controller::GetCharacterSpacing() const
{
  return mImpl->mModel->mVisualModel->GetCharacterSpacing();
}

void Controller::SetVisualTransformOffset(Vector2 offset)
{
  mImpl->mModel->mVisualTransformOffset = offset;
}

Controller::UpdateTextType Controller::Relayout(const Size& size, Dali::LayoutDirection::Type layoutDirection)
{
  return Relayouter::Relayout(*this, size, layoutDirection);
}

void Controller::RequestRelayout()
{
  mImpl->RequestRelayout();
}

Vector<Vector2> Controller::GetTextSize(CharacterIndex startIndex, CharacterIndex endIndex)
{
  Vector<Vector2> sizesList;
  Vector<Vector2> positionsList;

  GetTextGeometry(mImpl->mModel, startIndex, endIndex, sizesList, positionsList);
  return sizesList;
}

Vector<Vector2> Controller::GetTextPosition(CharacterIndex startIndex, CharacterIndex endIndex)
{
  Vector<Vector2> sizesList;
  Vector<Vector2> positionsList;

  GetTextGeometry(mImpl->mModel, startIndex, endIndex, sizesList, positionsList);
  return positionsList;
}

Rect<float> Controller::GetLineBoundingRectangle(const uint32_t lineIndex)
{
  return GetLineBoundingRect(mImpl->mModel, lineIndex);
}

Rect<float> Controller::GetCharacterBoundingRectangle(const uint32_t charIndex)
{
  return GetCharacterBoundingRect(mImpl->mModel, charIndex);
}

int Controller::GetCharacterIndexAtPosition(float visualX, float visualY)
{
  return GetCharIndexAtPosition(mImpl->mModel, visualX, visualY);
}

Rect<> Controller::GetTextBoundingRectangle(CharacterIndex startIndex, CharacterIndex endIndex)
{
  Vector<Vector2> sizeList;
  Vector<Vector2> positionList;

  GetTextGeometry(mImpl->mModel, startIndex, endIndex, sizeList, positionList);

  if(sizeList.Empty() || sizeList.Size() != positionList.Size())
  {
    return {0, 0, 0, 0};
  }

  auto controlWidth = mImpl->mModel->mVisualModel->mControlSize.width;
  auto minX         = positionList[0].x;
  auto minY         = positionList[0].y;
  auto maxRight     = positionList[0].x + sizeList[0].x;
  auto maxBottom    = positionList[0].y + sizeList[0].y;

  for(unsigned int i = 1; i < sizeList.Size(); i++)
  {
    minX      = std::min(minX, positionList[i].x);
    minY      = std::min(minY, positionList[i].y);
    maxRight  = std::max(maxRight, positionList[i].x + sizeList[i].x);
    maxBottom = std::max(maxBottom, positionList[i].y + sizeList[i].y);
  }

  if(minX < 0.0f)
  {
    minX = 0.0f;
  }

  if(maxRight > controlWidth)
  {
    maxRight = controlWidth;
  }

  return {minX, minY, maxRight - minX, maxBottom - minY};
}

bool Controller::IsInputStyleChangedSignalsQueueEmpty()
{
  return mImpl->IsInputStyleChangedSignalsQueueEmpty();
}

void Controller::RequestProcessInputStyleChangedSignals()
{
  if(Dali::Adaptor::IsAvailable() && !mImpl->mProcessorRegistered)
  {
    mImpl->mProcessorRegistered = true;
    Dali::Adaptor::Get().RegisterProcessor(*this, true);
  }
}

void Controller::OnIdleSignal()
{
  if(mImpl->mIdleCallback)
  {
    mImpl->mIdleCallback = NULL;

    mImpl->ProcessInputStyleChangedSignals();
  }
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

void Controller::SetTextSelectionRange(const uint32_t* start, const uint32_t* end)
{
  if(mImpl->mEventData)
  {
    mImpl->mEventData->mCheckScrollAmount     = true;
    mImpl->mEventData->mIsLeftHandleSelected  = true;
    mImpl->mEventData->mIsRightHandleSelected = true;
    mImpl->SetTextSelectionRange(start, end);
    mImpl->RequestRelayout();
    EventHandler::KeyboardFocusGainEvent(*this);
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
      EventHandler::KeyboardFocusGainEvent(*this);
      return true;
    }
  }
  return false;
}

void Controller::SelectWholeText()
{
  EventHandler::SelectEvent(*this, 0.f, 0.f, SelectionType::ALL);
}

void Controller::SelectNone()
{
  EventHandler::SelectEvent(*this, 0.f, 0.f, SelectionType::NONE);
}

void Controller::SelectText(const uint32_t start, const uint32_t end)
{
  EventHandler::SelectEvent(*this, start, end, SelectionType::RANGE);
}

string Controller::GetSelectedText() const
{
  return mImpl->GetSelectedText();
}

string Controller::CopyText()
{
  return mImpl->CopyText();
}

string Controller::CutText()
{
  return mImpl->CutText();
}

void Controller::PasteClipboardItemEvent(uint32_t id, const char* mimeType, const char* data)
{
  // Upon receiving the data, it is important to disconnect the signal
  // to avoid potential unintended pasting caused by subsequent requests.
  mImpl->mClipboard.DataReceivedSignal().Disconnect(this, &Controller::PasteClipboardItemEvent);

  // If the id is 0u, it is an invalid response.
  if(id == 0u)
  {
    return;
  }

  // text-controller allows only plain text type.
  if(!strncmp(mimeType, MIME_TYPE_TEXT_PLAIN, strlen(MIME_TYPE_TEXT_PLAIN)))
  {
    EventHandler::PasteClipboardItemEvent(*this, data);
  }
  else if(!strncmp(mimeType, MIME_TYPE_HTML, strlen(MIME_TYPE_HTML)))
  {
    // This does not mean that text controls can parse html.
    // This is temporary code, as text controls do not support html type data.
    // Simply remove the tags inside the angle brackets.
    // Once multiple types and data can be stored in the clipboard, this code should be removed.
    std::regex reg("<[^>]*>");
    std::string result = regex_replace(data, reg, "");

    EventHandler::PasteClipboardItemEvent(*this, result.c_str());
  }
}

void Controller::PasteText()
{
  if(mImpl->EnsureClipboardCreated())
  {
    // Connect the signal before calling GetData() of the clipboard.
    mImpl->mClipboard.DataReceivedSignal().Connect(this, &Controller::PasteClipboardItemEvent);

    // If there is no plain text type data on the clipboard, request html type data.
    std::string mimeType = mImpl->mClipboard.HasType(MIME_TYPE_TEXT_PLAIN) ? MIME_TYPE_TEXT_PLAIN : MIME_TYPE_HTML;

    // Request clipboard service to retrieve an item.
    uint id = mImpl->mClipboard.GetData(mimeType);
    if(id == 0u)
    {
      // If the return id is 0u, the signal is not emitted, we must disconnect signal here.
      mImpl->mClipboard.DataReceivedSignal().Disconnect(this, &Controller::PasteClipboardItemEvent);
    }
  }
}

InputMethodContext::CallbackData Controller::OnInputMethodContextEvent(InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent)
{
  return EventHandler::OnInputMethodContextEvent(*this, inputMethodContext, inputMethodContextEvent);
}

void Controller::GetTargetSize(Vector2& targetSize)
{
  targetSize = mImpl->mModel->mVisualModel->mControlSize;
}

void Controller::AddDecoration(Actor& actor, DecorationType type, bool needsClipping)
{
  if(mImpl->mEditableControlInterface)
  {
    mImpl->mEditableControlInterface->AddDecoration(actor, type, needsClipping);
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

bool Controller::IsScrollable(const Vector2& displacement)
{
  return mImpl->IsScrollable(displacement);
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
  return CreateControllerBackgroundActor(mImpl->mView, mImpl->mModel->mVisualModel, mImpl->mModel->mLogicalModel, mImpl->mShaderBackground);
}

void Controller::GetAnchorActors(std::vector<Toolkit::TextAnchor>& anchorActors)
{
  mImpl->GetAnchorActors(anchorActors);
}

int Controller::GetAnchorIndex(size_t characterOffset)
{
  return mImpl->GetAnchorIndex(characterOffset);
}

void Controller::Process(bool postProcess)
{
  if(Dali::Adaptor::IsAvailable() && mImpl->mProcessorRegistered)
  {
    Dali::Adaptor& adaptor = Dali::Adaptor::Get();

    adaptor.UnregisterProcessor(*this, true);
    mImpl->mProcessorRegistered = false;

    if(NULL == mImpl->mIdleCallback)
    {
      // @note: The callback manager takes the ownership of the callback object.
      mImpl->mIdleCallback = MakeCallback(this, &Controller::OnIdleSignal);
      if(DALI_UNLIKELY(!adaptor.AddIdle(mImpl->mIdleCallback, false)))
      {
        DALI_LOG_ERROR("Fail to add idle callback for text controller style changed signals queue. Skip these callbacks\n");

        // Clear queue forcely.
        if(mImpl->mEventData)
        {
          mImpl->mEventData->mInputStyleChangedQueue.Clear();
        }

        // Set the pointer to null as the callback manager deletes the callback even AddIdle failed.
        mImpl->mIdleCallback = NULL;
      }
    }
  }
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
  if(Dali::Adaptor::IsAvailable())
  {
    if(mImpl->mProcessorRegistered)
    {
      Dali::Adaptor::Get().UnregisterProcessor(*this, true);
    }
    if(mImpl->mIdleCallback)
    {
      Dali::Adaptor::Get().RemoveIdle(mImpl->mIdleCallback);
    }
  }
}

} // namespace Dali::Toolkit::Text
