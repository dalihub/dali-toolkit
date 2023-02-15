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
#include <dali-toolkit/internal/text/controller/text-controller-placeholder-handler.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/controller/text-controller-impl.h>
#include <dali-toolkit/internal/text/text-font-style.h>
#include <dali-toolkit/public-api/controls/text-controls/placeholder-properties.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

const char* EMPTY_STRING = "";

const char* const PLACEHOLDER_TEXT         = "text";
const char* const PLACEHOLDER_TEXT_FOCUSED = "textFocused";
const char* const PLACEHOLDER_COLOR        = "color";
const char* const PLACEHOLDER_FONT_FAMILY  = "fontFamily";
const char* const PLACEHOLDER_FONT_STYLE   = "fontStyle";
const char* const PLACEHOLDER_POINT_SIZE   = "pointSize";
const char* const PLACEHOLDER_PIXEL_SIZE   = "pixelSize";
const char* const PLACEHOLDER_ELLIPSIS     = "ellipsis";

} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Text
{
void Controller::PlaceholderHandler::SetPlaceholderTextElideEnabled(Controller& controller, bool enabled)
{
  controller.mImpl->mEventData->mIsPlaceholderElideEnabled = enabled;
  controller.mImpl->mEventData->mPlaceholderEllipsisFlag   = true;

  // Update placeholder if there is no text
  if(controller.mImpl->IsShowingPlaceholderText() ||
     (0u == controller.mImpl->mModel->mLogicalModel->mText.Count()))
  {
    ShowPlaceholderText(*controller.mImpl);
  }
}

bool Controller::PlaceholderHandler::IsPlaceholderTextElideEnabled(const Controller& controller)
{
  return controller.mImpl->mEventData->mIsPlaceholderElideEnabled;
}

void Controller::PlaceholderHandler::SetPlaceholderText(Controller& controller, PlaceholderType type, const std::string& text)
{
  if(NULL != controller.mImpl->mEventData)
  {
    if(PLACEHOLDER_TYPE_INACTIVE == type)
    {
      controller.mImpl->mEventData->mPlaceholderTextInactive = text;
    }
    else
    {
      controller.mImpl->mEventData->mPlaceholderTextActive = text;
    }

    // Update placeholder if there is no text
    if(controller.mImpl->IsShowingPlaceholderText() ||
       (0u == controller.mImpl->mModel->mLogicalModel->mText.Count()))
    {
      ShowPlaceholderText(*controller.mImpl);
    }
  }
}

void Controller::PlaceholderHandler::GetPlaceholderText(const Controller& controller, PlaceholderType type, std::string& text)
{
  if(NULL != controller.mImpl->mEventData)
  {
    if(PLACEHOLDER_TYPE_INACTIVE == type)
    {
      text = controller.mImpl->mEventData->mPlaceholderTextInactive;
    }
    else
    {
      text = controller.mImpl->mEventData->mPlaceholderTextActive;
    }
  }
}

void Controller::PlaceholderHandler::SetPlaceholderFontFamily(Controller& controller, const std::string& placeholderTextFontFamily)
{
  if(NULL != controller.mImpl->mEventData)
  {
    // if mPlaceholderFont is null, create an instance.
    CreatePlaceholderFont(controller);

    controller.mImpl->mEventData->mPlaceholderFont->mFontDescription.family = placeholderTextFontFamily;
    DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::SetPlaceholderFontFamily %s\n", placeholderTextFontFamily.c_str());
    controller.mImpl->mEventData->mPlaceholderFont->familyDefined = !placeholderTextFontFamily.empty();

    controller.mImpl->RequestRelayout();
  }
}

std::string Controller::PlaceholderHandler::GetPlaceholderFontFamily(const Controller& controller)
{
  if((NULL != controller.mImpl->mEventData) && (NULL != controller.mImpl->mEventData->mPlaceholderFont))
  {
    return controller.mImpl->mEventData->mPlaceholderFont->mFontDescription.family;
  }

  return EMPTY_STRING;
}

void Controller::PlaceholderHandler::SetPlaceholderTextFontWeight(Controller& controller, FontWeight weight)
{
  if(NULL != controller.mImpl->mEventData)
  {
    // if mPlaceholderFont is null, create an instance.
    CreatePlaceholderFont(controller);

    controller.mImpl->mEventData->mPlaceholderFont->mFontDescription.weight = weight;
    controller.mImpl->mEventData->mPlaceholderFont->weightDefined           = true;

    controller.mImpl->RequestRelayout();
  }
}

bool Controller::PlaceholderHandler::IsPlaceholderTextFontWeightDefined(const Controller& controller)
{
  if((NULL != controller.mImpl->mEventData) && (NULL != controller.mImpl->mEventData->mPlaceholderFont))
  {
    return controller.mImpl->mEventData->mPlaceholderFont->weightDefined;
  }
  return false;
}

FontWeight Controller::PlaceholderHandler::GetPlaceholderTextFontWeight(const Controller& controller)
{
  if((NULL != controller.mImpl->mEventData) && (NULL != controller.mImpl->mEventData->mPlaceholderFont))
  {
    return controller.mImpl->mEventData->mPlaceholderFont->mFontDescription.weight;
  }

  return TextAbstraction::FontWeight::NORMAL;
}

void Controller::PlaceholderHandler::SetPlaceholderTextFontWidth(Controller& controller, FontWidth width)
{
  if(NULL != controller.mImpl->mEventData)
  {
    // if mPlaceholderFont is null, create an instance.
    CreatePlaceholderFont(controller);

    controller.mImpl->mEventData->mPlaceholderFont->mFontDescription.width = width;
    controller.mImpl->mEventData->mPlaceholderFont->widthDefined           = true;

    controller.mImpl->RequestRelayout();
  }
}

bool Controller::PlaceholderHandler::IsPlaceholderTextFontWidthDefined(const Controller& controller)
{
  if((NULL != controller.mImpl->mEventData) && (NULL != controller.mImpl->mEventData->mPlaceholderFont))
  {
    return controller.mImpl->mEventData->mPlaceholderFont->widthDefined;
  }
  return false;
}

FontWidth Controller::PlaceholderHandler::GetPlaceholderTextFontWidth(const Controller& controller)
{
  if((NULL != controller.mImpl->mEventData) && (NULL != controller.mImpl->mEventData->mPlaceholderFont))
  {
    return controller.mImpl->mEventData->mPlaceholderFont->mFontDescription.width;
  }

  return TextAbstraction::FontWidth::NORMAL;
}

void Controller::PlaceholderHandler::SetPlaceholderTextFontSlant(Controller& controller, FontSlant slant)
{
  if(NULL != controller.mImpl->mEventData)
  {
    // if mPlaceholderFont is null, create an instance.
    CreatePlaceholderFont(controller);

    controller.mImpl->mEventData->mPlaceholderFont->mFontDescription.slant = slant;
    controller.mImpl->mEventData->mPlaceholderFont->slantDefined           = true;

    controller.mImpl->RequestRelayout();
  }
}

bool Controller::PlaceholderHandler::IsPlaceholderTextFontSlantDefined(const Controller& controller)
{
  if((NULL != controller.mImpl->mEventData) && (NULL != controller.mImpl->mEventData->mPlaceholderFont))
  {
    return controller.mImpl->mEventData->mPlaceholderFont->slantDefined;
  }
  return false;
}

FontSlant Controller::PlaceholderHandler::GetPlaceholderTextFontSlant(const Controller& controller)
{
  if((NULL != controller.mImpl->mEventData) && (NULL != controller.mImpl->mEventData->mPlaceholderFont))
  {
    return controller.mImpl->mEventData->mPlaceholderFont->mFontDescription.slant;
  }

  return TextAbstraction::FontSlant::NORMAL;
}

void Controller::PlaceholderHandler::SetPlaceholderTextFontSize(Controller& controller, float fontSize, FontSizeType type)
{
  if(NULL != controller.mImpl->mEventData)
  {
    // if mPlaceholderFont is null, create an instance.
    CreatePlaceholderFont(controller);

    switch(type)
    {
      case POINT_SIZE:
      {
        controller.mImpl->mEventData->mPlaceholderFont->mDefaultPointSize = fontSize;
        controller.mImpl->mEventData->mPlaceholderFont->sizeDefined       = true;
        controller.mImpl->mEventData->mIsPlaceholderPixelSize             = false; // Font size flag
        break;
      }
      case PIXEL_SIZE:
      {
        // Point size = Pixel size * 72.f / DPI
        unsigned int                horizontalDpi = 0u;
        unsigned int                verticalDpi   = 0u;
        TextAbstraction::FontClient fontClient    = TextAbstraction::FontClient::Get();
        fontClient.GetDpi(horizontalDpi, verticalDpi);

        controller.mImpl->mEventData->mPlaceholderFont->mDefaultPointSize = (fontSize * 72.f) / static_cast<float>(horizontalDpi);
        controller.mImpl->mEventData->mPlaceholderFont->sizeDefined       = true;
        controller.mImpl->mEventData->mIsPlaceholderPixelSize             = true; // Font size flag
        break;
      }
    }

    controller.mImpl->RequestRelayout();
  }
}

float Controller::PlaceholderHandler::GetPlaceholderTextFontSize(const Controller& controller, FontSizeType type)
{
  float value = 0.0f;
  if(NULL != controller.mImpl->mEventData)
  {
    switch(type)
    {
      case POINT_SIZE:
      {
        if(NULL != controller.mImpl->mEventData->mPlaceholderFont)
        {
          value = controller.mImpl->mEventData->mPlaceholderFont->mDefaultPointSize;
        }
        else
        {
          // If the placeholder text font size is not set, then return the default font size.
          value = controller.GetDefaultFontSize(POINT_SIZE);
        }
        break;
      }
      case PIXEL_SIZE:
      {
        if(NULL != controller.mImpl->mEventData->mPlaceholderFont)
        {
          // Pixel size = Point size * DPI / 72.f
          unsigned int                horizontalDpi = 0u;
          unsigned int                verticalDpi   = 0u;
          TextAbstraction::FontClient fontClient    = TextAbstraction::FontClient::Get();
          fontClient.GetDpi(horizontalDpi, verticalDpi);

          value = controller.mImpl->mEventData->mPlaceholderFont->mDefaultPointSize * static_cast<float>(horizontalDpi) / 72.f;
        }
        else
        {
          // If the placeholder text font size is not set, then return the default font size.
          value = controller.GetDefaultFontSize(PIXEL_SIZE);
        }
        break;
      }
    }
    return value;
  }

  return value;
}

void Controller::PlaceholderHandler::SetPlaceholderTextColor(Controller& controller, const Vector4& textColor)
{
  if(NULL != controller.mImpl->mEventData)
  {
    controller.mImpl->mEventData->mPlaceholderTextColor = textColor;
  }

  if(controller.mImpl->IsShowingPlaceholderText())
  {
    controller.mImpl->mModel->mVisualModel->SetTextColor(textColor);
    controller.mImpl->mModel->mLogicalModel->mColorRuns.Clear();
    controller.mImpl->mOperationsPending = static_cast<OperationsMask>(controller.mImpl->mOperationsPending | COLOR);
    controller.mImpl->RequestRelayout();
  }
}

const Vector4& Controller::PlaceholderHandler::GetPlaceholderTextColor(const Controller& controller)
{
  if(NULL != controller.mImpl->mEventData)
  {
    return controller.mImpl->mEventData->mPlaceholderTextColor;
  }

  return Color::BLACK;
}

void Controller::PlaceholderHandler::SetPlaceholderProperty(Controller& controller, const Property::Map& map)
{
  const Property::Map::SizeType count = map.Count();

  for(Property::Map::SizeType position = 0; position < count; ++position)
  {
    KeyValuePair     keyValue = map.GetKeyValue(position);
    Property::Key&   key      = keyValue.first;
    Property::Value& value    = keyValue.second;

    if(key == Toolkit::Text::PlaceHolder::Property::TEXT || key == PLACEHOLDER_TEXT)
    {
      std::string text = "";
      value.Get(text);
      SetPlaceholderText(controller, Controller::PLACEHOLDER_TYPE_INACTIVE, text);
    }
    else if(key == Toolkit::Text::PlaceHolder::Property::TEXT_FOCUSED || key == PLACEHOLDER_TEXT_FOCUSED)
    {
      std::string text = "";
      value.Get(text);
      SetPlaceholderText(controller, Controller::PLACEHOLDER_TYPE_ACTIVE, text);
    }
    else if(key == Toolkit::Text::PlaceHolder::Property::COLOR || key == PLACEHOLDER_COLOR)
    {
      Vector4 textColor;
      value.Get(textColor);
      if(GetPlaceholderTextColor(controller) != textColor)
      {
        SetPlaceholderTextColor(controller, textColor);
      }
    }
    else if(key == Toolkit::Text::PlaceHolder::Property::FONT_FAMILY || key == PLACEHOLDER_FONT_FAMILY)
    {
      std::string fontFamily = "";
      value.Get(fontFamily);
      SetPlaceholderFontFamily(controller, fontFamily);
    }
    else if(key == Toolkit::Text::PlaceHolder::Property::FONT_STYLE || key == PLACEHOLDER_FONT_STYLE)
    {
      SetFontStyleProperty(&controller, value, Text::FontStyle::PLACEHOLDER);
    }
    else if(key == Toolkit::Text::PlaceHolder::Property::POINT_SIZE || key == PLACEHOLDER_POINT_SIZE)
    {
      float pointSize;
      value.Get(pointSize);
      if(!Equals(GetPlaceholderTextFontSize(controller, Text::Controller::POINT_SIZE), pointSize))
      {
        SetPlaceholderTextFontSize(controller, pointSize, Text::Controller::POINT_SIZE);
      }
    }
    else if(key == Toolkit::Text::PlaceHolder::Property::PIXEL_SIZE || key == PLACEHOLDER_PIXEL_SIZE)
    {
      float pixelSize;
      value.Get(pixelSize);
      if(!Equals(GetPlaceholderTextFontSize(controller, Text::Controller::PIXEL_SIZE), pixelSize))
      {
        SetPlaceholderTextFontSize(controller, pixelSize, Text::Controller::PIXEL_SIZE);
      }
    }
    else if(key == Toolkit::Text::PlaceHolder::Property::ELLIPSIS || key == PLACEHOLDER_ELLIPSIS)
    {
      bool ellipsis;
      value.Get(ellipsis);
      SetPlaceholderTextElideEnabled(controller, ellipsis);
    }
  }
}

void Controller::PlaceholderHandler::GetPlaceholderProperty(Controller& controller, Property::Map& map)
{
  if(NULL != controller.mImpl->mEventData)
  {
    if(!controller.mImpl->mEventData->mPlaceholderTextActive.empty())
    {
      map[Text::PlaceHolder::Property::TEXT_FOCUSED] = controller.mImpl->mEventData->mPlaceholderTextActive;
    }
    if(!controller.mImpl->mEventData->mPlaceholderTextInactive.empty())
    {
      map[Text::PlaceHolder::Property::TEXT] = controller.mImpl->mEventData->mPlaceholderTextInactive;
    }

    map[Text::PlaceHolder::Property::COLOR]       = controller.mImpl->mEventData->mPlaceholderTextColor;
    map[Text::PlaceHolder::Property::FONT_FAMILY] = GetPlaceholderFontFamily(controller);

    Property::Value fontStyleMapGet;
    GetFontStyleProperty(&controller, fontStyleMapGet, Text::FontStyle::PLACEHOLDER);
    map[Text::PlaceHolder::Property::FONT_STYLE] = fontStyleMapGet;

    // Choose font size : POINT_SIZE or PIXEL_SIZE
    if(!controller.mImpl->mEventData->mIsPlaceholderPixelSize)
    {
      map[Text::PlaceHolder::Property::POINT_SIZE] = GetPlaceholderTextFontSize(controller, Text::Controller::POINT_SIZE);
    }
    else
    {
      map[Text::PlaceHolder::Property::PIXEL_SIZE] = GetPlaceholderTextFontSize(controller, Text::Controller::PIXEL_SIZE);
    }

    if(controller.mImpl->mEventData->mPlaceholderEllipsisFlag)
    {
      map[Text::PlaceHolder::Property::ELLIPSIS] = IsPlaceholderTextElideEnabled(controller);
    }
  }
}

void Controller::PlaceholderHandler::ShowPlaceholderText(Controller::Impl& impl)
{
  if(impl.IsPlaceholderAvailable())
  {
    EventData*& eventData = impl.mEventData;
    DALI_ASSERT_DEBUG(eventData && "No placeholder text available");

    if(NULL == eventData)
    {
      return;
    }

    eventData->mIsShowingPlaceholderText = true;

    // Disable handles when showing place-holder text
    DecoratorPtr& decorator = eventData->mDecorator;
    decorator->SetHandleActive(GRAB_HANDLE, false);
    decorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
    decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);

    const char* text(NULL);
    size_t      size(0);

    // TODO - Switch Placeholder text when changing state
    std::string& placeholderTextActive = eventData->mPlaceholderTextActive;
    if((EventData::INACTIVE != eventData->mState) &&
       (0u != placeholderTextActive.c_str()))
    {
      text = placeholderTextActive.c_str();
      size = placeholderTextActive.size();
    }
    else
    {
      std::string& placeholderTextInactive = eventData->mPlaceholderTextInactive;
      text                                 = placeholderTextInactive.c_str();
      size                                 = placeholderTextInactive.size();
    }

    TextUpdateInfo& textUpdateInfo             = impl.mTextUpdateInfo;
    textUpdateInfo.mCharacterIndex             = 0u;
    textUpdateInfo.mNumberOfCharactersToRemove = textUpdateInfo.mPreviousNumberOfCharacters;

    // Reset model for showing placeholder.
    ModelPtr&        model        = impl.mModel;
    LogicalModelPtr& logicalModel = model->mLogicalModel;
    logicalModel->mText.Clear();
    model->mVisualModel->SetTextColor(eventData->mPlaceholderTextColor);

    // Convert text into UTF-32
    Vector<Character>& utf32Characters = logicalModel->mText;
    utf32Characters.Resize(size);

    // This is a bit horrible but std::string returns a (signed) char*
    const uint8_t* utf8 = reinterpret_cast<const uint8_t*>(text);

    // Transform a text array encoded in utf8 into an array encoded in utf32.
    // It returns the actual number of characters.
    const Length characterCount = Utf8ToUtf32(utf8, size, utf32Characters.Begin());
    utf32Characters.Resize(characterCount);

    // The characters to be added.
    textUpdateInfo.mNumberOfCharactersToAdd = characterCount;

    // Reset the cursor position
    eventData->mPrimaryCursorPosition = 0;

    // The natural size needs to be re-calculated.
    impl.mRecalculateNaturalSize = true;

    // The text direction needs to be updated.
    impl.mUpdateTextDirection = true;

    // Apply modifications to the model
    impl.mOperationsPending = ALL_OPERATIONS;

    // Update the rest of the model during size negotiation
    impl.QueueModifyEvent(ModifyEvent::TEXT_REPLACED);
  }
}

void Controller::PlaceholderHandler::CreatePlaceholderFont(Controller& controller)
{
  if(nullptr == controller.mImpl->mEventData->mPlaceholderFont)
  {
    controller.mImpl->mEventData->mPlaceholderFont = std::unique_ptr<FontDefaults>(new FontDefaults());
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
