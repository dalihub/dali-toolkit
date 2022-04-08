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
 */

#include <dali-toolkit/internal/controls/text-controls/text-selection-popup-property-handler.h>
#include <dali/integration-api/debug.h>

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gLogFilter;
#endif

namespace Dali::Toolkit::Internal
{
void TextSelectionPopup::PropertyHandler::SetProperty(Toolkit::TextSelectionPopup selectionPopup, Property::Index index, const Property::Value& value)
{
  TextSelectionPopup& impl(GetImpl(selectionPopup));

  switch(index)
  {
    case Toolkit::TextSelectionPopup::Property::POPUP_MAX_SIZE:
    {
      impl.SetDimensionToCustomise(POPUP_MAXIMUM_SIZE, value.Get<Vector2>());
      break;
    }
    case Toolkit::TextSelectionPopup::Property::OPTION_MAX_SIZE:
    {
      impl.SetDimensionToCustomise(OPTION_MAXIMUM_SIZE, value.Get<Vector2>());
      break;
    }
    case Toolkit::TextSelectionPopup::Property::OPTION_MIN_SIZE:
    {
      impl.SetDimensionToCustomise(OPTION_MINIMUM_SIZE, value.Get<Vector2>());
      break;
    }
    case Toolkit::TextSelectionPopup::Property::OPTION_DIVIDER_SIZE:
    {
      impl.SetDimensionToCustomise(OPTION_DIVIDER_SIZE, value.Get<Vector2>());
      break;
    }
    case Toolkit::TextSelectionPopup::Property::OPTION_DIVIDER_PADDING:
    {
      Vector4 padding(value.Get<Vector4>());
      impl.SetOptionDividerPadding(Padding(padding.x, padding.y, padding.z, padding.w));
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_CLIPBOARD_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(Toolkit::TextSelectionPopup::CLIPBOARD, value.Get<std::string>());
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_CUT_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(Toolkit::TextSelectionPopup::CUT, value.Get<std::string>());
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_COPY_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(Toolkit::TextSelectionPopup::COPY, value.Get<std::string>());
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_PASTE_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(Toolkit::TextSelectionPopup::PASTE, value.Get<std::string>());
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_SELECT_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(Toolkit::TextSelectionPopup::SELECT, value.Get<std::string>());
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_SELECT_ALL_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(Toolkit::TextSelectionPopup::SELECT_ALL, value.Get<std::string>());
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_DIVIDER_COLOR:
    {
      impl.mDividerColor = value.Get<Vector4>();
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_ICON_COLOR:
    {
      impl.mIconColor = value.Get<Vector4>();
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_PRESSED_COLOR:
    {
      impl.mPressedColor = value.Get<Vector4>();
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_PRESSED_IMAGE:
    {
      impl.SetPressedImage(value.Get<std::string>());
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_FADE_IN_DURATION:
    {
      impl.mFadeInDuration = value.Get<float>();
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_FADE_OUT_DURATION:
    {
      impl.mFadeOutDuration = value.Get<float>();
      break;
    }
    case Toolkit::TextSelectionPopup::Property::BACKGROUND_BORDER:
    {
      Property::Map map = value.Get<Property::Map>();
      impl.CreateBackgroundBorder(map);
      break;
    }
  }
}

Property::Value TextSelectionPopup::PropertyHandler::GetProperty(Toolkit::TextSelectionPopup selectionPopup, Property::Index index)
{
  Property::Value value;
  TextSelectionPopup& impl(GetImpl(selectionPopup));

  switch(index)
  {
    case Toolkit::TextSelectionPopup::Property::POPUP_MAX_SIZE:
    {
      value = impl.GetDimensionToCustomise(POPUP_MAXIMUM_SIZE);
      break;
    }
    case Toolkit::TextSelectionPopup::Property::OPTION_MAX_SIZE:
    {
      value = impl.GetDimensionToCustomise(OPTION_MAXIMUM_SIZE);
      break;
    }
    case Toolkit::TextSelectionPopup::Property::OPTION_MIN_SIZE:
    {
      value = impl.GetDimensionToCustomise(OPTION_MINIMUM_SIZE);
      break;
    }
    case Toolkit::TextSelectionPopup::Property::OPTION_DIVIDER_SIZE:
    {
      value = impl.GetDimensionToCustomise(OPTION_DIVIDER_SIZE);
      break;
    }
    case Toolkit::TextSelectionPopup::Property::OPTION_DIVIDER_PADDING:
    {
      Padding padding = impl.GetOptionDividerPadding();
      value           = Vector4(padding.left, padding.right, padding.top, padding.bottom);
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_CLIPBOARD_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(Toolkit::TextSelectionPopup::CLIPBOARD);
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_CUT_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(Toolkit::TextSelectionPopup::CUT);
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_COPY_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(Toolkit::TextSelectionPopup::COPY);
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_PASTE_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(Toolkit::TextSelectionPopup::PASTE);
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_SELECT_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(Toolkit::TextSelectionPopup::SELECT);
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_SELECT_ALL_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(Toolkit::TextSelectionPopup::SELECT_ALL);
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_DIVIDER_COLOR:
    {
      value = impl.mDividerColor;
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_ICON_COLOR:
    {
      value = impl.mIconColor;
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_PRESSED_COLOR:
    {
      value = impl.mPressedColor;
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_PRESSED_IMAGE:
    {
      value = impl.GetPressedImage();
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_FADE_IN_DURATION:
    {
      value = impl.mFadeInDuration;
      break;
    }
    case Toolkit::TextSelectionPopup::Property::POPUP_FADE_OUT_DURATION:
    {
      value = impl.mFadeOutDuration;
      break;
    }
    case Toolkit::TextSelectionPopup::Property::BACKGROUND_BORDER:
    {
      Property::Map         map;
      Toolkit::Visual::Base visual = DevelControl::GetVisual(impl, Toolkit::TextSelectionPopup::Property::BACKGROUND_BORDER);
      if(visual)
      {
        visual.CreatePropertyMap(map);
      }
      value = map;
      break;
    }
  }

  return value;
}

} // namespace Dali::Toolkit::Internal
