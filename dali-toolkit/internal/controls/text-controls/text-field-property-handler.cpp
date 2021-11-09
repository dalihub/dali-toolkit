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
 */

#include <dali-toolkit/internal/controls/text-controls/text-field-property-handler.h>

#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>
#include <dali-toolkit/devel-api/text/rendering-backend.h>

#include <dali-toolkit/internal/text/decorator/text-decorator.h>
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali-toolkit/internal/text/text-effects-style.h>
#include <dali-toolkit/internal/text/text-enumerations-impl.h>
#include <dali-toolkit/internal/text/text-font-style.h>
#include <dali-toolkit/public-api/text/text-enumerations.h>
#include <dali/integration-api/debug.h>

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gTextFieldLogFilter;
#endif

namespace Dali::Toolkit::Internal
{
const char* const TextField::PropertyHandler::IMAGE_MAP_FILENAME_STRING{"filename"};

/// Retrieves a filename from a value that is a Property::Map
std::string TextField::PropertyHandler::GetImageFileNameFromPropertyValue(const Property::Value& value)
{
  std::string          filename;
  const Property::Map* map = value.GetMap();
  if(map)
  {
    const Property::Value* filenameValue = map->Find(TextField::PropertyHandler::IMAGE_MAP_FILENAME_STRING);
    if(filenameValue)
    {
      filenameValue->Get(filename);
    }
  }
  return filename;
}

void TextField::PropertyHandler::SetProperty(Toolkit::TextField textField, Property::Index index, const Property::Value& value)
{
  TextField& impl(GetImpl(textField));
  DALI_ASSERT_DEBUG(impl.mController && "No text controller");
  DALI_ASSERT_DEBUG(impl.mDecorator && "No text decorator");

  switch(index)
  {
    case Toolkit::DevelTextField::Property::RENDERING_BACKEND:
    {
      int backend = value.Get<int>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField %p RENDERING_BACKEND %d\n", impl.mController.Get(), backend);

#ifndef ENABLE_VECTOR_BASED_TEXT_RENDERING
      if(DevelText::RENDERING_VECTOR_BASED == backend)
      {
        backend = TextAbstraction::BITMAP_GLYPH; // Fallback to bitmap-based rendering
      }
#endif
      if(impl.mRenderingBackend != backend)
      {
        impl.mRenderingBackend = backend;
        impl.mRenderer.Reset();

        // When using the vector-based rendering, the size of the GLyphs are different
        TextAbstraction::GlyphType glyphType = (DevelText::RENDERING_VECTOR_BASED == impl.mRenderingBackend) ? TextAbstraction::VECTOR_GLYPH : TextAbstraction::BITMAP_GLYPH;
        impl.mController->SetGlyphType(glyphType);
      }
      break;
    }
    case Toolkit::TextField::Property::TEXT:
    {
      const std::string& text = value.Get<std::string>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p TEXT %s\n", impl.mController.Get(), text.c_str());

      impl.mController->SetText(text);
      break;
    }

    case Toolkit::TextField::Property::PLACEHOLDER_TEXT:
    {
      const std::string& text = value.Get<std::string>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p PLACEHOLDER_TEXT %s\n", impl.mController.Get(), text.c_str());

      impl.mController->SetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_INACTIVE, text);
      break;
    }
    case Toolkit::TextField::Property::PLACEHOLDER_TEXT_FOCUSED:
    {
      const std::string& text = value.Get<std::string>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p PLACEHOLDER_TEXT_FOCUSED %s\n", impl.mController.Get(), text.c_str());

      impl.mController->SetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_ACTIVE, text);
      break;
    }
    case Toolkit::TextField::Property::FONT_FAMILY:
    {
      const std::string& fontFamily = value.Get<std::string>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p FONT_FAMILY %s\n", impl.mController.Get(), fontFamily.c_str());
      impl.mController->SetDefaultFontFamily(fontFamily);
      break;
    }
    case Toolkit::TextField::Property::FONT_STYLE:
    {
      SetFontStyleProperty(impl.mController, value, Text::FontStyle::DEFAULT);
      break;
    }
    case Toolkit::TextField::Property::POINT_SIZE:
    {
      const float pointSize = value.Get<float>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p POINT_SIZE %f\n", impl.mController.Get(), pointSize);

      if(!Equals(impl.mController->GetDefaultFontSize(Text::Controller::POINT_SIZE), pointSize))
      {
        impl.mController->SetDefaultFontSize(pointSize, Text::Controller::POINT_SIZE);
      }
      break;
    }
    case Toolkit::TextField::Property::MAX_LENGTH:
    {
      const int max = value.Get<int>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p MAX_LENGTH %d\n", impl.mController.Get(), max);

      impl.mController->SetMaximumNumberOfCharacters(max);
      break;
    }
    case Toolkit::TextField::Property::EXCEED_POLICY:
    {
      impl.mExceedPolicy = value.Get<int>();

      if(Dali::Toolkit::TextField::EXCEED_POLICY_CLIP == impl.mExceedPolicy)
      {
        impl.EnableClipping();
      }
      else
      {
        UnparentAndReset(impl.mStencil);
      }
      impl.RequestTextRelayout();
      break;
    }

    case Toolkit::TextField::Property::HORIZONTAL_ALIGNMENT:
    {
      Text::HorizontalAlignment::Type alignment(static_cast<Text::HorizontalAlignment::Type>(-1)); // Set to invalid value to ensure a valid mode does get set
      if(Text::GetHorizontalAlignmentEnumeration(value, alignment))
      {
        DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p HORIZONTAL_ALIGNMENT %d\n", impl.mController.Get(), alignment);
        impl.mController->SetHorizontalAlignment(alignment);
      }
      break;
    }
    case Toolkit::TextField::Property::VERTICAL_ALIGNMENT:
    {
      Toolkit::Text::VerticalAlignment::Type alignment(static_cast<Text::VerticalAlignment::Type>(-1)); // Set to invalid value to ensure a valid mode does get set
      if(Text::GetVerticalAlignmentEnumeration(value, alignment))
      {
        impl.mController->SetVerticalAlignment(alignment);
        DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p VERTICAL_ALIGNMENT %d\n", impl.mController.Get(), alignment);
      }
      break;
    }
    case Toolkit::TextField::Property::TEXT_COLOR:
    {
      const Vector4& textColor = value.Get<Vector4>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p TEXT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), textColor.r, textColor.g, textColor.b, textColor.a);

      if(impl.mController->GetDefaultColor() != textColor)
      {
        impl.mController->SetDefaultColor(textColor);
        impl.mController->SetInputColor(textColor);
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextField::Property::PLACEHOLDER_TEXT_COLOR:
    {
      const Vector4& textColor = value.Get<Vector4>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p PLACEHOLDER_TEXT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), textColor.r, textColor.g, textColor.b, textColor.a);

      if(impl.mController->GetPlaceholderTextColor() != textColor)
      {
        impl.mController->SetPlaceholderTextColor(textColor);
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextField::Property::PRIMARY_CURSOR_COLOR:
    {
      const Vector4& color = value.Get<Vector4>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p PRIMARY_CURSOR_COLOR %f,%f,%f,%f\n", impl.mController.Get(), color.r, color.g, color.b, color.a);

      impl.mDecorator->SetCursorColor(Toolkit::Text::PRIMARY_CURSOR, color);
      impl.RequestTextRelayout();
      break;
    }
    case Toolkit::TextField::Property::SECONDARY_CURSOR_COLOR:
    {
      const Vector4& color = value.Get<Vector4>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p SECONDARY_CURSOR_COLOR %f,%f,%f,%f\n", impl.mController.Get(), color.r, color.g, color.b, color.a);

      impl.mDecorator->SetCursorColor(Toolkit::Text::SECONDARY_CURSOR, color);
      impl.RequestTextRelayout();
      break;
    }
    case Toolkit::TextField::Property::ENABLE_CURSOR_BLINK:
    {
      const bool enable = value.Get<bool>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField %p ENABLE_CURSOR_BLINK %d\n", impl.mController.Get(), enable);

      impl.mController->SetEnableCursorBlink(enable);
      impl.RequestTextRelayout();
      break;
    }
    case Toolkit::TextField::Property::CURSOR_BLINK_INTERVAL:
    {
      const float interval = value.Get<float>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField %p CURSOR_BLINK_INTERVAL %f\n", impl.mController.Get(), interval);

      impl.mDecorator->SetCursorBlinkInterval(interval);
      break;
    }
    case Toolkit::TextField::Property::CURSOR_BLINK_DURATION:
    {
      const float duration = value.Get<float>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField %p CURSOR_BLINK_DURATION %f\n", impl.mController.Get(), duration);

      impl.mDecorator->SetCursorBlinkDuration(duration);
      break;
    }
    case Toolkit::TextField::Property::CURSOR_WIDTH:
    {
      const int width = value.Get<int>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField %p CURSOR_WIDTH %d\n", impl.mController.Get(), width);

      impl.mDecorator->SetCursorWidth(width);
      impl.mController->GetLayoutEngine().SetCursorWidth(width);
      break;
    }
    case Toolkit::TextField::Property::GRAB_HANDLE_IMAGE:
    {
      const std::string imageFileName = value.Get<std::string>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField %p GRAB_HANDLE_IMAGE %s\n", impl.mController.Get(), imageFileName.c_str());

      if(imageFileName.size())
      {
        impl.mDecorator->SetHandleImage(Toolkit::Text::GRAB_HANDLE, Toolkit::Text::HANDLE_IMAGE_RELEASED, imageFileName);
        impl.RequestTextRelayout();
      }
      break;
    }
    case Toolkit::TextField::Property::GRAB_HANDLE_PRESSED_IMAGE:
    {
      const std::string imageFileName = value.Get<std::string>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField %p GRAB_HANDLE_PRESSED_IMAGE %s\n", impl.mController.Get(), imageFileName.c_str());

      if(imageFileName.size())
      {
        impl.mDecorator->SetHandleImage(Toolkit::Text::GRAB_HANDLE, Toolkit::Text::HANDLE_IMAGE_PRESSED, imageFileName);
        impl.RequestTextRelayout();
      }
      break;
    }
    case Toolkit::TextField::Property::SCROLL_THRESHOLD:
    {
      const float threshold = value.Get<float>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField %p SCROLL_THRESHOLD %f\n", impl.mController.Get(), threshold);

      impl.mDecorator->SetScrollThreshold(threshold);
      break;
    }
    case Toolkit::TextField::Property::SCROLL_SPEED:
    {
      const float speed = value.Get<float>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField %p SCROLL_SPEED %f\n", impl.mController.Get(), speed);

      impl.mDecorator->SetScrollSpeed(speed);
      break;
    }
    case Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_LEFT:
    {
      const std::string filename = GetImageFileNameFromPropertyValue(value);

      if(filename.size())
      {
        impl.mDecorator->SetHandleImage(Toolkit::Text::LEFT_SELECTION_HANDLE, Toolkit::Text::HANDLE_IMAGE_RELEASED, filename);
        impl.RequestTextRelayout();
      }
      break;
    }
    case Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT:
    {
      const std::string filename = GetImageFileNameFromPropertyValue(value);

      if(filename.size())
      {
        impl.mDecorator->SetHandleImage(Toolkit::Text::RIGHT_SELECTION_HANDLE, Toolkit::Text::HANDLE_IMAGE_RELEASED, filename);
        impl.RequestTextRelayout();
      }
      break;
    }
    case Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT:
    {
      const std::string filename = GetImageFileNameFromPropertyValue(value);

      if(filename.size())
      {
        impl.mDecorator->SetHandleImage(Toolkit::Text::LEFT_SELECTION_HANDLE, Toolkit::Text::HANDLE_IMAGE_PRESSED, filename);
        impl.RequestTextRelayout();
      }
      break;
    }
    case Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT:
    {
      const std::string filename = GetImageFileNameFromPropertyValue(value);

      if(filename.size())
      {
        impl.mDecorator->SetHandleImage(Toolkit::Text::RIGHT_SELECTION_HANDLE, Toolkit::Text::HANDLE_IMAGE_PRESSED, filename);
        impl.RequestTextRelayout();
      }
      break;
    }
    case Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT:
    {
      const std::string filename = GetImageFileNameFromPropertyValue(value);

      if(filename.size())
      {
        impl.mDecorator->SetHandleImage(Toolkit::Text::LEFT_SELECTION_HANDLE_MARKER,
                                        Toolkit::Text::HANDLE_IMAGE_RELEASED,
                                        filename);
        impl.RequestTextRelayout();
      }
      break;
    }
    case Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT:
    {
      const std::string filename = GetImageFileNameFromPropertyValue(value);

      if(filename.size())
      {
        impl.mDecorator->SetHandleImage(Toolkit::Text::RIGHT_SELECTION_HANDLE_MARKER,
                                        Toolkit::Text::HANDLE_IMAGE_RELEASED,
                                        filename);
        impl.RequestTextRelayout();
      }
      break;
    }
    case Toolkit::TextField::Property::SELECTION_HIGHLIGHT_COLOR:
    {
      const Vector4 color = value.Get<Vector4>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p SELECTION_HIGHLIGHT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), color.r, color.g, color.b, color.a);

      impl.mDecorator->SetHighlightColor(color);
      impl.RequestTextRelayout();
      break;
    }
    case Toolkit::TextField::Property::DECORATION_BOUNDING_BOX:
    {
      const Rect<int>& box = value.Get<Rect<int> >();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p DECORATION_BOUNDING_BOX %d,%d %dx%d\n", impl.mController.Get(), box.x, box.y, box.width, box.height);

      impl.mDecorator->SetBoundingBox(box);
      impl.RequestTextRelayout();
      break;
    }
    case Toolkit::TextField::Property::INPUT_METHOD_SETTINGS:
    {
      const Property::Map* map = value.GetMap();
      if(map)
      {
        impl.mInputMethodOptions.ApplyProperty(*map);
      }
      impl.mController->SetInputModePassword(impl.mInputMethodOptions.IsPassword());

      Toolkit::Control control = Toolkit::KeyInputFocusManager::Get().GetCurrentFocusControl();
      if(control == textField)
      {
        impl.mInputMethodContext.ApplyOptions(impl.mInputMethodOptions);
      }
      break;
    }
    case Toolkit::TextField::Property::INPUT_COLOR:
    {
      const Vector4& inputColor = value.Get<Vector4>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p INPUT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), inputColor.r, inputColor.g, inputColor.b, inputColor.a);

      impl.mController->SetInputColor(inputColor);
      break;
    }
    case Toolkit::TextField::Property::ENABLE_MARKUP:
    {
      const bool enableMarkup = value.Get<bool>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p ENABLE_MARKUP %d\n", impl.mController.Get(), enableMarkup);

      impl.mController->SetMarkupProcessorEnabled(enableMarkup);
      break;
    }
    case Toolkit::TextField::Property::INPUT_FONT_FAMILY:
    {
      const std::string& fontFamily = value.Get<std::string>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p INPUT_FONT_FAMILY %s\n", impl.mController.Get(), fontFamily.c_str());
      impl.mController->SetInputFontFamily(fontFamily);
      break;
    }
    case Toolkit::TextField::Property::INPUT_FONT_STYLE:
    {
      SetFontStyleProperty(impl.mController, value, Text::FontStyle::INPUT);
      break;
    }
    case Toolkit::TextField::Property::INPUT_POINT_SIZE:
    {
      const float pointSize = value.Get<float>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p INPUT_POINT_SIZE %f\n", impl.mController.Get(), pointSize);
      impl.mController->SetInputFontPointSize(pointSize);
      break;
    }
    case Toolkit::TextField::Property::UNDERLINE:
    {
      const bool update = SetUnderlineProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
      if(update)
      {
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextField::Property::INPUT_UNDERLINE:
    {
      const bool update = SetUnderlineProperties(impl.mController, value, Text::EffectStyle::INPUT);
      if(update)
      {
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextField::Property::SHADOW:
    {
      const bool update = SetShadowProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
      if(update)
      {
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextField::Property::INPUT_SHADOW:
    {
      const bool update = SetShadowProperties(impl.mController, value, Text::EffectStyle::INPUT);
      if(update)
      {
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextField::Property::EMBOSS:
    {
      const bool update = SetEmbossProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
      if(update)
      {
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextField::Property::INPUT_EMBOSS:
    {
      const bool update = SetEmbossProperties(impl.mController, value, Text::EffectStyle::INPUT);
      if(update)
      {
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextField::Property::OUTLINE:
    {
      const bool update = SetOutlineProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
      if(update)
      {
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextField::Property::INPUT_OUTLINE:
    {
      const bool update = SetOutlineProperties(impl.mController, value, Text::EffectStyle::INPUT);
      if(update)
      {
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextField::Property::HIDDEN_INPUT_SETTINGS:
    {
      const Property::Map* map = value.GetMap();
      if(map)
      {
        impl.mController->SetHiddenInputOption(*map);
      }
      break;
    }
    case Toolkit::TextField::Property::PIXEL_SIZE:
    {
      const float pixelSize = value.Get<float>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p PIXEL_SIZE %f\n", impl.mController.Get(), pixelSize);

      if(!Equals(impl.mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE), pixelSize))
      {
        impl.mController->SetDefaultFontSize(pixelSize, Text::Controller::PIXEL_SIZE);
      }
      break;
    }
    case Toolkit::TextField::Property::ENABLE_SELECTION:
    {
      const bool enableSelection = value.Get<bool>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p ENABLE_SELECTION %d\n", impl.mController.Get(), enableSelection);
      impl.mController->SetSelectionEnabled(enableSelection);
      break;
    }
    case Toolkit::TextField::Property::PLACEHOLDER:
    {
      const Property::Map* map = value.GetMap();
      if(map)
      {
        impl.mController->SetPlaceholderProperty(*map);
      }
      break;
    }
    case Toolkit::TextField::Property::ELLIPSIS:
    {
      const bool ellipsis = value.Get<bool>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p ELLIPSIS %d\n", impl.mController.Get(), ellipsis);

      impl.mController->SetTextElideEnabled(ellipsis);
      break;
    }
    case Toolkit::DevelTextField::Property::ENABLE_SHIFT_SELECTION:
    {
      const bool shiftSelection = value.Get<bool>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p ENABLE_SHIFT_SELECTION %d\n", impl.mController.Get(), shiftSelection);

      impl.mController->SetShiftSelectionEnabled(shiftSelection);
      break;
    }
    case Toolkit::DevelTextField::Property::ENABLE_GRAB_HANDLE:
    {
      const bool grabHandleEnabled = value.Get<bool>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p ENABLE_GRAB_HANDLE %d\n", impl.mController.Get(), grabHandleEnabled);

      impl.mController->SetGrabHandleEnabled(grabHandleEnabled);
      break;
    }
    case Toolkit::DevelTextField::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION:
    {
      impl.mController->SetMatchLayoutDirection(value.Get<bool>() ? DevelText::MatchLayoutDirection::LOCALE : DevelText::MatchLayoutDirection::CONTENTS);
      break;
    }
    case Toolkit::DevelTextField::Property::ENABLE_GRAB_HANDLE_POPUP:
    {
      const bool grabHandlePopupEnabled = value.Get<bool>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p ENABLE_GRAB_HANDLE_POPUP %d\n", impl.mController.Get(), grabHandlePopupEnabled);

      impl.mController->SetGrabHandlePopupEnabled(grabHandlePopupEnabled);
      break;
    }
    case Toolkit::DevelTextField::Property::BACKGROUND:
    {
      const Vector4 backgroundColor = value.Get<Vector4>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p BACKGROUND %f,%f,%f,%f\n", impl.mController.Get(), backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);

      impl.mController->SetBackgroundEnabled(true);
      impl.mController->SetBackgroundColor(backgroundColor);
      break;
    }
    case Toolkit::DevelTextField::Property::SELECTED_TEXT_START:
    {
      uint32_t start = static_cast<uint32_t>(value.Get<int>());
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p SELECTED_TEXT_START %d\n", impl.mController.Get(), start);
      impl.SetTextSelectionRange(&start, nullptr);
      break;
    }
    case Toolkit::DevelTextField::Property::SELECTED_TEXT_END:
    {
      uint32_t end = static_cast<uint32_t>(value.Get<int>());
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p SELECTED_TEXT_END %d\n", impl.mController.Get(), end);
      impl.SetTextSelectionRange(nullptr, &end);
      break;
    }
    case Toolkit::DevelTextField::Property::ENABLE_EDITING:
    {
      const bool editable = value.Get<bool>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p ENABLE_EDITING %d\n", impl.mController.Get(), editable);
      impl.SetEditable(editable);
      break;
    }
    case Toolkit::DevelTextField::Property::FONT_SIZE_SCALE:
    {
      const float scale = value.Get<float>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p FONT_SIZE_SCALE %f\n", impl.mController.Get(), scale);

      if(!Equals(impl.mController->GetFontSizeScale(), scale))
      {
        impl.mController->SetFontSizeScale(scale);
      }
      break;
    }
    case Toolkit::DevelTextField::Property::PRIMARY_CURSOR_POSITION:
    {
      uint32_t position = static_cast<uint32_t>(value.Get<int>());
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p PRIMARY_CURSOR_POSITION %d\n", impl.mController.Get(), position);
      if(impl.mController->SetPrimaryCursorPosition(position, impl.HasKeyInputFocus()))
      {
        impl.SetKeyInputFocus();
      }
      break;
    }
    case Toolkit::DevelTextField::Property::GRAB_HANDLE_COLOR:
    {
      const Vector4 color = value.Get<Vector4>();
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p GRAB_HANDLE_COLOR %f,%f,%f,%f\n", impl.mController.Get(), color.r, color.g, color.b, color.a);

      impl.mDecorator->SetHandleColor(color);
      impl.RequestTextRelayout();
      break;
    }

    case Toolkit::DevelTextField::Property::INPUT_FILTER:
    {
      const Property::Map* map = value.GetMap();
      if(map)
      {
        impl.mController->SetInputFilterOption(*map);
      }
      break;
    }
    case Toolkit::DevelTextField::Property::ELLIPSIS_POSITION:
    {
      DevelText::EllipsisPosition::Type ellipsisPositionType(static_cast<DevelText::EllipsisPosition::Type>(-1)); // Set to invalid value to ensure a valid mode does get set
      if(Text::GetEllipsisPositionTypeEnumeration(value, ellipsisPositionType))
      {
        DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p EllipsisPosition::Type %d\n", impl.mController.Get(), ellipsisPositionType);
        impl.mController->SetEllipsisPosition(ellipsisPositionType);
      }
      break;
    }
  }
}

Property::Value TextField::PropertyHandler::GetProperty(Toolkit::TextField textField, Property::Index index)
{
  Property::Value value;
  TextField&      impl(GetImpl(textField));
  DALI_ASSERT_DEBUG(impl.mController && "No text controller");
  DALI_ASSERT_DEBUG(impl.mDecorator && "No text decorator");

  switch(index)
  {
    case Toolkit::DevelTextField::Property::RENDERING_BACKEND:
    {
      value = impl.mRenderingBackend;
      break;
    }
    case Toolkit::TextField::Property::TEXT:
    {
      std::string text;
      impl.mController->GetText(text);
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::General, "TextField %p returning text: %s\n", impl.mController.Get(), text.c_str());
      value = text;
      break;
    }
    case Toolkit::TextField::Property::PLACEHOLDER_TEXT:
    {
      std::string text;
      impl.mController->GetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_INACTIVE, text);
      value = text;
      break;
    }
    case Toolkit::TextField::Property::PLACEHOLDER_TEXT_FOCUSED:
    {
      std::string text;
      impl.mController->GetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_ACTIVE, text);
      value = text;
      break;
    }
    case Toolkit::TextField::Property::FONT_FAMILY:
    {
      value = impl.mController->GetDefaultFontFamily();
      break;
    }
    case Toolkit::TextField::Property::FONT_STYLE:
    {
      GetFontStyleProperty(impl.mController, value, Text::FontStyle::DEFAULT);
      break;
    }
    case Toolkit::TextField::Property::POINT_SIZE:
    {
      value = impl.mController->GetDefaultFontSize(Text::Controller::POINT_SIZE);
      break;
    }
    case Toolkit::TextField::Property::MAX_LENGTH:
    {
      value = impl.mController->GetMaximumNumberOfCharacters();
      break;
    }
    case Toolkit::TextField::Property::EXCEED_POLICY:
    {
      value = impl.mExceedPolicy;
      break;
    }
    case Toolkit::TextField::Property::HORIZONTAL_ALIGNMENT:
    {
      const char* name = Text::GetHorizontalAlignmentString(impl.mController->GetHorizontalAlignment());
      if(name)
      {
        value = std::string(name);
      }
      break;
    }
    case Toolkit::TextField::Property::VERTICAL_ALIGNMENT:
    {
      const char* name = Text::GetVerticalAlignmentString(impl.mController->GetVerticalAlignment());

      if(name)
      {
        value = std::string(name);
      }
      break;
    }
    case Toolkit::TextField::Property::TEXT_COLOR:
    {
      value = impl.mController->GetDefaultColor();
      break;
    }
    case Toolkit::TextField::Property::PLACEHOLDER_TEXT_COLOR:
    {
      value = impl.mController->GetPlaceholderTextColor();
      break;
    }
    case Toolkit::TextField::Property::PRIMARY_CURSOR_COLOR:
    {
      value = impl.mDecorator->GetColor(Text::PRIMARY_CURSOR);
      break;
    }
    case Toolkit::TextField::Property::SECONDARY_CURSOR_COLOR:
    {
      value = impl.mDecorator->GetColor(Text::SECONDARY_CURSOR);
      break;
    }
    case Toolkit::TextField::Property::ENABLE_CURSOR_BLINK:
    {
      value = impl.mController->GetEnableCursorBlink();
      break;
    }
    case Toolkit::TextField::Property::CURSOR_BLINK_INTERVAL:
    {
      value = impl.mDecorator->GetCursorBlinkInterval();
      break;
    }
    case Toolkit::TextField::Property::CURSOR_BLINK_DURATION:
    {
      value = impl.mDecorator->GetCursorBlinkDuration();
      break;
    }
    case Toolkit::TextField::Property::CURSOR_WIDTH:
    {
      value = impl.mDecorator->GetCursorWidth();
      break;
    }
    case Toolkit::TextField::Property::GRAB_HANDLE_IMAGE:
    {
      value = impl.mDecorator->GetHandleImage(Text::GRAB_HANDLE, Text::HANDLE_IMAGE_RELEASED);
      break;
    }
    case Toolkit::TextField::Property::GRAB_HANDLE_PRESSED_IMAGE:
    {
      value = impl.mDecorator->GetHandleImage(Text::GRAB_HANDLE, Text::HANDLE_IMAGE_PRESSED);
      break;
    }
    case Toolkit::TextField::Property::SCROLL_THRESHOLD:
    {
      value = impl.mDecorator->GetScrollThreshold();
      break;
    }
    case Toolkit::TextField::Property::SCROLL_SPEED:
    {
      value = impl.mDecorator->GetScrollSpeed();
      break;
    }
    case Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_LEFT:
    {
      impl.GetHandleImagePropertyValue(value, Text::LEFT_SELECTION_HANDLE, Text::HANDLE_IMAGE_RELEASED);
      break;
    }
    case Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT:
    {
      impl.GetHandleImagePropertyValue(value, Text::RIGHT_SELECTION_HANDLE, Text::HANDLE_IMAGE_RELEASED);
      break;
    }
    case Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT:
    {
      impl.GetHandleImagePropertyValue(value, Text::LEFT_SELECTION_HANDLE, Text::HANDLE_IMAGE_PRESSED);
      break;
    }
    case Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT:
    {
      impl.GetHandleImagePropertyValue(value, Text::RIGHT_SELECTION_HANDLE, Text::HANDLE_IMAGE_PRESSED);
      break;
    }
    case Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT:
    {
      impl.GetHandleImagePropertyValue(value, Text::LEFT_SELECTION_HANDLE_MARKER, Text::HANDLE_IMAGE_RELEASED);
      break;
    }
    case Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT:
    {
      impl.GetHandleImagePropertyValue(value, Text::RIGHT_SELECTION_HANDLE_MARKER, Text::HANDLE_IMAGE_RELEASED);
      break;
    }
    case Toolkit::TextField::Property::SELECTION_HIGHLIGHT_COLOR:
    {
      value = impl.mDecorator->GetHighlightColor();
      break;
    }
    case Toolkit::TextField::Property::DECORATION_BOUNDING_BOX:
    {
      Rect<int> boundingBox;
      impl.mDecorator->GetBoundingBox(boundingBox);
      value = boundingBox;
      break;
    }
    case Toolkit::TextField::Property::INPUT_METHOD_SETTINGS:
    {
      Property::Map map;
      impl.mInputMethodOptions.RetrieveProperty(map);
      value = map;
      break;
    }
    case Toolkit::TextField::Property::INPUT_COLOR:
    {
      value = impl.mController->GetInputColor();
      break;
    }
    case Toolkit::TextField::Property::ENABLE_MARKUP:
    {
      value = impl.mController->IsMarkupProcessorEnabled();
      break;
    }
    case Toolkit::TextField::Property::INPUT_FONT_FAMILY:
    {
      value = impl.mController->GetInputFontFamily();
      break;
    }
    case Toolkit::TextField::Property::INPUT_FONT_STYLE:
    {
      GetFontStyleProperty(impl.mController, value, Text::FontStyle::INPUT);
      break;
    }
    case Toolkit::TextField::Property::INPUT_POINT_SIZE:
    {
      value = impl.mController->GetInputFontPointSize();
      break;
    }
    case Toolkit::TextField::Property::UNDERLINE:
    {
      GetUnderlineProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
      break;
    }
    case Toolkit::TextField::Property::INPUT_UNDERLINE:
    {
      GetUnderlineProperties(impl.mController, value, Text::EffectStyle::INPUT);
      break;
    }
    case Toolkit::TextField::Property::SHADOW:
    {
      GetShadowProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
      break;
    }
    case Toolkit::TextField::Property::INPUT_SHADOW:
    {
      GetShadowProperties(impl.mController, value, Text::EffectStyle::INPUT);
      break;
    }
    case Toolkit::TextField::Property::EMBOSS:
    {
      GetEmbossProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
      break;
    }
    case Toolkit::TextField::Property::INPUT_EMBOSS:
    {
      GetEmbossProperties(impl.mController, value, Text::EffectStyle::INPUT);
      break;
    }
    case Toolkit::TextField::Property::OUTLINE:
    {
      GetOutlineProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
      break;
    }
    case Toolkit::TextField::Property::INPUT_OUTLINE:
    {
      GetOutlineProperties(impl.mController, value, Text::EffectStyle::INPUT);
      break;
    }
    case Toolkit::TextField::Property::HIDDEN_INPUT_SETTINGS:
    {
      Property::Map map;
      impl.mController->GetHiddenInputOption(map);
      value = map;
      break;
    }
    case Toolkit::TextField::Property::PIXEL_SIZE:
    {
      value = impl.mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE);
      break;
    }
    case Toolkit::TextField::Property::ENABLE_SELECTION:
    {
      value = impl.mController->IsSelectionEnabled();
      break;
    }
    case Toolkit::TextField::Property::PLACEHOLDER:
    {
      Property::Map map;
      impl.mController->GetPlaceholderProperty(map);
      value = map;
      break;
    }
    case Toolkit::TextField::Property::ELLIPSIS:
    {
      value = impl.mController->IsTextElideEnabled();
      break;
    }
    case Toolkit::DevelTextField::Property::ENABLE_SHIFT_SELECTION:
    {
      value = impl.mController->IsShiftSelectionEnabled();
      break;
    }
    case Toolkit::DevelTextField::Property::ENABLE_GRAB_HANDLE:
    {
      value = impl.mController->IsGrabHandleEnabled();
      break;
    }
    case Toolkit::DevelTextField::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION:
    {
      value = impl.mController->GetMatchLayoutDirection() != DevelText::MatchLayoutDirection::CONTENTS;
      break;
    }
    case Toolkit::DevelTextField::Property::ENABLE_GRAB_HANDLE_POPUP:
    {
      value = impl.mController->IsGrabHandlePopupEnabled();
      break;
    }
    case Toolkit::DevelTextField::Property::BACKGROUND:
    {
      value = impl.mController->GetBackgroundColor();
      break;
    }
    case Toolkit::DevelTextField::Property::SELECTED_TEXT:
    {
      value = impl.mController->GetSelectedText();
      break;
    }
    case Toolkit::DevelTextField::Property::SELECTED_TEXT_START:
    {
      Uint32Pair range = impl.GetTextSelectionRange();
      value            = static_cast<int>(range.first);
      break;
    }
    case Toolkit::DevelTextField::Property::SELECTED_TEXT_END:
    {
      Uint32Pair range = impl.GetTextSelectionRange();
      value            = static_cast<int>(range.second);
      break;
    }
    case Toolkit::DevelTextField::Property::ENABLE_EDITING:
    {
      value = impl.IsEditable();
      break;
    }
    case Toolkit::DevelTextField::Property::FONT_SIZE_SCALE:
    {
      value = impl.mController->GetFontSizeScale();
      break;
    }
    case Toolkit::DevelTextField::Property::PRIMARY_CURSOR_POSITION:
    {
      value = static_cast<int>(impl.mController->GetPrimaryCursorPosition());
      break;
    }
    case Toolkit::DevelTextField::Property::GRAB_HANDLE_COLOR:
    {
      value = impl.mDecorator->GetHandleColor();
      break;
    }
    case Toolkit::DevelTextField::Property::INPUT_FILTER:
    {
      Property::Map map;
      impl.mController->GetInputFilterOption(map);
      value = map;
      break;
    }
    case Toolkit::DevelTextField::Property::ELLIPSIS_POSITION:
    {
      value = impl.mController->GetEllipsisPosition();
      break;
    }
  } //switch
  return value;
}

} // namespace Dali::Toolkit::Internal
