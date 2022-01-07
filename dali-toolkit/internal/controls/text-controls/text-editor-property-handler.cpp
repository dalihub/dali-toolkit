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

#include <dali-toolkit/internal/controls/text-controls/common-text-utils.h>
#include <dali-toolkit/internal/controls/text-controls/text-editor-property-handler.h>

#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>

#include <dali-toolkit/internal/text/decorator/text-decorator.h>
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali-toolkit/internal/text/text-effects-style.h>
#include <dali-toolkit/internal/text/text-enumerations-impl.h>
#include <dali-toolkit/internal/text/text-font-style.h>
#include <dali-toolkit/public-api/text/text-enumerations.h>
#include <dali/integration-api/debug.h>

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gTextEditorLogFilter;
#endif

namespace Dali::Toolkit::Internal
{
const char* const TextEditor::PropertyHandler::IMAGE_MAP_FILENAME_STRING{"filename"};

/// Retrieves a filename from a value that is a Property::Map
std::string TextEditor::PropertyHandler::GetImageFileNameFromPropertyValue(const Property::Value& value)
{
  std::string          filename;
  const Property::Map* map = value.GetMap();
  if(map)
  {
    const Property::Value* filenameValue = map->Find(TextEditor::PropertyHandler::IMAGE_MAP_FILENAME_STRING);
    if(filenameValue)
    {
      filenameValue->Get(filename);
    }
  }
  return filename;
}

void TextEditor::PropertyHandler::SetProperty(Toolkit::TextEditor textEditor, Property::Index index, const Property::Value& value)
{
  TextEditor& impl(GetImpl(textEditor));
  DALI_ASSERT_DEBUG(impl.mController && "No text controller");
  DALI_ASSERT_DEBUG(impl.mDecorator && "No text decorator");

  switch(index)
  {
    case Toolkit::DevelTextEditor::Property::RENDERING_BACKEND:
    {
      int backend = value.Get<int>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor %p RENDERING_BACKEND %d\n", impl.mController.Get(), backend);

      if(impl.mRenderingBackend != backend)
      {
        impl.mRenderingBackend = backend;
        impl.mRenderer.Reset();
        impl.RequestTextRelayout();
      }
      break;
    }
    case Toolkit::TextEditor::Property::TEXT:
    {
      const std::string& text = value.Get<std::string>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p TEXT %s\n", impl.mController.Get(), text.c_str());

      impl.mController->SetText(text);
      break;
    }
    case Toolkit::TextEditor::Property::TEXT_COLOR:
    {
      const Vector4& textColor = value.Get<Vector4>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p TEXT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), textColor.r, textColor.g, textColor.b, textColor.a);

      if(impl.mController->GetDefaultColor() != textColor)
      {
        impl.mController->SetDefaultColor(textColor);
        impl.mController->SetInputColor(textColor);
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextEditor::Property::FONT_FAMILY:
    {
      const std::string& fontFamily = value.Get<std::string>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p FONT_FAMILY %s\n", impl.mController.Get(), fontFamily.c_str());
      impl.mController->SetDefaultFontFamily(fontFamily);
      break;
    }
    case Toolkit::TextEditor::Property::FONT_STYLE:
    {
      SetFontStyleProperty(impl.mController, value, Text::FontStyle::DEFAULT);
      break;
    }
    case Toolkit::TextEditor::Property::POINT_SIZE:
    {
      const float pointSize = value.Get<float>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p POINT_SIZE %f\n", impl.mController.Get(), pointSize);

      if(!Equals(impl.mController->GetDefaultFontSize(Text::Controller::POINT_SIZE), pointSize))
      {
        impl.mController->SetDefaultFontSize(pointSize, Text::Controller::POINT_SIZE);
      }
      break;
    }
    case Toolkit::TextEditor::Property::HORIZONTAL_ALIGNMENT:
    {
      Text::HorizontalAlignment::Type alignment(static_cast<Text::HorizontalAlignment::Type>(-1)); // Set to invalid value to ensure a valid mode does get set
      if(Text::GetHorizontalAlignmentEnumeration(value, alignment))
      {
        DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p HORIZONTAL_ALIGNMENT %d\n", impl.mController.Get(), alignment);
        impl.mController->SetHorizontalAlignment(alignment);
      }
      break;
    }
    case Toolkit::TextEditor::Property::SCROLL_THRESHOLD:
    {
      const float threshold = value.Get<float>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor %p SCROLL_THRESHOLD %f\n", impl.mController.Get(), threshold);

      impl.mDecorator->SetScrollThreshold(threshold);
      break;
    }
    case Toolkit::TextEditor::Property::SCROLL_SPEED:
    {
      const float speed = value.Get<float>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor %p SCROLL_SPEED %f\n", impl.mController.Get(), speed);

      impl.mDecorator->SetScrollSpeed(speed);
      break;
    }
    case Toolkit::TextEditor::Property::PRIMARY_CURSOR_COLOR:
    {
      const Vector4& color = value.Get<Vector4>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p PRIMARY_CURSOR_COLOR %f,%f,%f,%f\n", impl.mController.Get(), color.r, color.g, color.b, color.a);

      impl.mDecorator->SetCursorColor(Toolkit::Text::PRIMARY_CURSOR, color);
      impl.RequestTextRelayout();
      break;
    }
    case Toolkit::TextEditor::Property::SECONDARY_CURSOR_COLOR:
    {
      const Vector4& color = value.Get<Vector4>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p SECONDARY_CURSOR_COLOR %f,%f,%f,%f\n", impl.mController.Get(), color.r, color.g, color.b, color.a);

      impl.mDecorator->SetCursorColor(Toolkit::Text::SECONDARY_CURSOR, color);
      impl.RequestTextRelayout();
      break;
    }
    case Toolkit::TextEditor::Property::ENABLE_CURSOR_BLINK:
    {
      const bool enable = value.Get<bool>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor %p ENABLE_CURSOR_BLINK %d\n", impl.mController.Get(), enable);

      impl.mController->SetEnableCursorBlink(enable);
      impl.RequestTextRelayout();
      break;
    }
    case Toolkit::TextEditor::Property::CURSOR_BLINK_INTERVAL:
    {
      const float interval = value.Get<float>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor %p CURSOR_BLINK_INTERVAL %f\n", impl.mController.Get(), interval);

      impl.mDecorator->SetCursorBlinkInterval(interval);
      break;
    }
    case Toolkit::TextEditor::Property::CURSOR_BLINK_DURATION:
    {
      const float duration = value.Get<float>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor %p CURSOR_BLINK_DURATION %f\n", impl.mController.Get(), duration);

      impl.mDecorator->SetCursorBlinkDuration(duration);
      break;
    }
    case Toolkit::TextEditor::Property::CURSOR_WIDTH:
    {
      const int width = value.Get<int>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor %p CURSOR_WIDTH %d\n", impl.mController.Get(), width);

      impl.mDecorator->SetCursorWidth(width);
      impl.mController->GetLayoutEngine().SetCursorWidth(width);
      break;
    }
    case Toolkit::TextEditor::Property::GRAB_HANDLE_IMAGE:
    {
      const std::string imageFileName = value.Get<std::string>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor %p GRAB_HANDLE_IMAGE %s\n", impl.mController.Get(), imageFileName.c_str());

      if(imageFileName.size())
      {
        impl.mDecorator->SetHandleImage(Toolkit::Text::GRAB_HANDLE, Toolkit::Text::HANDLE_IMAGE_RELEASED, imageFileName);
        impl.RequestTextRelayout();
      }
      break;
    }
    case Toolkit::TextEditor::Property::GRAB_HANDLE_PRESSED_IMAGE:
    {
      const std::string imageFileName = value.Get<std::string>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor %p GRAB_HANDLE_PRESSED_IMAGE %s\n", impl.mController.Get(), imageFileName.c_str());

      if(imageFileName.size())
      {
        impl.mDecorator->SetHandleImage(Toolkit::Text::GRAB_HANDLE, Toolkit::Text::HANDLE_IMAGE_PRESSED, imageFileName);
        impl.RequestTextRelayout();
      }
      break;
    }
    case Toolkit::TextEditor::Property::SELECTION_HANDLE_IMAGE_LEFT:
    {
      const std::string filename = GetImageFileNameFromPropertyValue(value);

      if(filename.size())
      {
        impl.mDecorator->SetHandleImage(Toolkit::Text::LEFT_SELECTION_HANDLE, Toolkit::Text::HANDLE_IMAGE_RELEASED, filename);
        impl.RequestTextRelayout();
      }
      break;
    }
    case Toolkit::TextEditor::Property::SELECTION_HANDLE_IMAGE_RIGHT:
    {
      const std::string filename = GetImageFileNameFromPropertyValue(value);

      if(filename.size())
      {
        impl.mDecorator->SetHandleImage(Toolkit::Text::RIGHT_SELECTION_HANDLE, Toolkit::Text::HANDLE_IMAGE_RELEASED, filename);
        impl.RequestTextRelayout();
      }
      break;
    }
    case Toolkit::TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT:
    {
      const std::string filename = GetImageFileNameFromPropertyValue(value);

      if(filename.size())
      {
        impl.mDecorator->SetHandleImage(Toolkit::Text::LEFT_SELECTION_HANDLE, Toolkit::Text::HANDLE_IMAGE_PRESSED, filename);
        impl.RequestTextRelayout();
      }
      break;
    }
    case Toolkit::TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT:
    {
      const std::string filename = GetImageFileNameFromPropertyValue(value);

      if(filename.size())
      {
        impl.mDecorator->SetHandleImage(Toolkit::Text::RIGHT_SELECTION_HANDLE, Toolkit::Text::HANDLE_IMAGE_PRESSED, filename);
        impl.RequestTextRelayout();
      }
      break;
    }
    case Toolkit::TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT:
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
    case Toolkit::TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT:
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
    case Toolkit::TextEditor::Property::SELECTION_HIGHLIGHT_COLOR:
    {
      const Vector4 color = value.Get<Vector4>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p SELECTION_HIGHLIGHT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), color.r, color.g, color.b, color.a);

      impl.mDecorator->SetHighlightColor(color);
      impl.RequestTextRelayout();
      break;
    }
    case Toolkit::TextEditor::Property::DECORATION_BOUNDING_BOX:
    {
      const Rect<int>& box = value.Get<Rect<int> >();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p DECORATION_BOUNDING_BOX %d,%d %dx%d\n", impl.mController.Get(), box.x, box.y, box.width, box.height);

      impl.mDecorator->SetBoundingBox(box);
      impl.RequestTextRelayout();
      break;
    }
    case Toolkit::TextEditor::Property::ENABLE_MARKUP:
    {
      const bool enableMarkup = value.Get<bool>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p ENABLE_MARKUP %d\n", impl.mController.Get(), enableMarkup);

      impl.mController->SetMarkupProcessorEnabled(enableMarkup);
      CommonTextUtils::SynchronizeTextAnchorsInParent(textEditor, impl.mController, impl.mAnchorActors);
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_COLOR:
    {
      const Vector4& inputColor = value.Get<Vector4>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p INPUT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), inputColor.r, inputColor.g, inputColor.b, inputColor.a);

      impl.mController->SetInputColor(inputColor);
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_FONT_FAMILY:
    {
      const std::string& fontFamily = value.Get<std::string>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p INPUT_FONT_FAMILY %s\n", impl.mController.Get(), fontFamily.c_str());
      impl.mController->SetInputFontFamily(fontFamily);
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_FONT_STYLE:
    {
      SetFontStyleProperty(impl.mController, value, Text::FontStyle::INPUT);
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_POINT_SIZE:
    {
      const float pointSize = value.Get<float>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p INPUT_POINT_SIZE %f\n", impl.mController.Get(), pointSize);
      impl.mController->SetInputFontPointSize(pointSize);
      break;
    }
    case Toolkit::TextEditor::Property::LINE_SPACING:
    {
      const float lineSpacing = value.Get<float>();
      impl.mController->SetDefaultLineSpacing(lineSpacing);
      impl.mRenderer.Reset();
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_LINE_SPACING:
    {
      const float lineSpacing = value.Get<float>();
      impl.mController->SetInputLineSpacing(lineSpacing);
      impl.mRenderer.Reset();
      break;
    }
    case Toolkit::TextEditor::Property::UNDERLINE:
    {
      const bool update = SetUnderlineProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
      if(update)
      {
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_UNDERLINE:
    {
      const bool update = SetUnderlineProperties(impl.mController, value, Text::EffectStyle::INPUT);
      if(update)
      {
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextEditor::Property::SHADOW:
    {
      const bool update = SetShadowProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
      if(update)
      {
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_SHADOW:
    {
      const bool update = SetShadowProperties(impl.mController, value, Text::EffectStyle::INPUT);
      if(update)
      {
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextEditor::Property::EMBOSS:
    {
      const bool update = SetEmbossProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
      if(update)
      {
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_EMBOSS:
    {
      const bool update = SetEmbossProperties(impl.mController, value, Text::EffectStyle::INPUT);
      if(update)
      {
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextEditor::Property::OUTLINE:
    {
      const bool update = SetOutlineProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
      if(update)
      {
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_OUTLINE:
    {
      const bool update = SetOutlineProperties(impl.mController, value, Text::EffectStyle::INPUT);
      if(update)
      {
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextEditor::Property::SMOOTH_SCROLL:
    {
      const bool enable = value.Get<bool>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor SMOOTH_SCROLL %d\n", enable);

      impl.mScrollAnimationEnabled = enable;
      break;
    }
    case Toolkit::TextEditor::Property::SMOOTH_SCROLL_DURATION:
    {
      const float duration = value.Get<float>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor SMOOTH_SCROLL_DURATION %f\n", duration);

      impl.mScrollAnimationDuration = duration;
      if(impl.mTextVerticalScroller)
      {
        impl.mTextVerticalScroller->SetDuration(duration);
      }
      break;
    }
    case Toolkit::TextEditor::Property::ENABLE_SCROLL_BAR:
    {
      const bool enable = value.Get<bool>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor SHOW_SCROLL_BAR %d\n", enable);

      impl.mScrollBarEnabled = enable;
      break;
    }
    case Toolkit::TextEditor::Property::SCROLL_BAR_SHOW_DURATION:
    {
      const float duration = value.Get<float>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor SCROLL_BAR_SHOW_DURATION %f\n", duration);

      impl.mAnimationPeriod.delaySeconds = duration;
      break;
    }
    case Toolkit::TextEditor::Property::SCROLL_BAR_FADE_DURATION:
    {
      const float duration = value.Get<float>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor SCROLL_BAR_FADE_DURATION %f\n", duration);

      impl.mAnimationPeriod.durationSeconds = duration;
      break;
    }
    case Toolkit::TextEditor::Property::PIXEL_SIZE:
    {
      const float pixelSize = value.Get<float>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p PIXEL_SIZE %f\n", impl.mController.Get(), pixelSize);

      if(!Equals(impl.mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE), pixelSize))
      {
        impl.mController->SetDefaultFontSize(pixelSize, Text::Controller::PIXEL_SIZE);
      }
      break;
    }
    case Toolkit::DevelTextEditor::Property::PLACEHOLDER_TEXT:
    {
      const std::string& text = value.Get<std::string>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor::OnPropertySet %p PLACEHOLDER_TEXT %s\n", impl.mController.Get(), text.c_str());

      impl.mController->SetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_INACTIVE, text);
      break;
    }
    case Toolkit::DevelTextEditor::Property::PLACEHOLDER_TEXT_COLOR:
    {
      const Vector4& textColor = value.Get<Vector4>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p PLACEHOLDER_TEXT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), textColor.r, textColor.g, textColor.b, textColor.a);

      if(impl.mController->GetPlaceholderTextColor() != textColor)
      {
        impl.mController->SetPlaceholderTextColor(textColor);
        impl.mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextEditor::Property::ENABLE_SELECTION:
    {
      const bool enableSelection = value.Get<bool>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p ENABLE_SELECTION %d\n", impl.mController.Get(), enableSelection);
      impl.mController->SetSelectionEnabled(enableSelection);
      break;
    }
    case Toolkit::TextEditor::Property::PLACEHOLDER:
    {
      const Property::Map* map = value.GetMap();
      if(map)
      {
        impl.mController->SetPlaceholderProperty(*map);
      }
      break;
    }
    case Toolkit::TextEditor::Property::LINE_WRAP_MODE:
    {
      Text::LineWrap::Mode lineWrapMode(static_cast<Text::LineWrap::Mode>(-1)); // Set to invalid value to ensure a valid mode does get set
      if(Toolkit::Text::GetLineWrapModeEnumeration(value, lineWrapMode))
      {
        DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p LineWrap::MODE %d\n", impl.mController.Get(), lineWrapMode);
        impl.mController->SetLineWrapMode(lineWrapMode);
      }
      break;
    }
    case Toolkit::DevelTextEditor::Property::ENABLE_SHIFT_SELECTION:
    {
      const bool shiftSelection = value.Get<bool>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p ENABLE_SHIFT_SELECTION %d\n", impl.mController.Get(), shiftSelection);

      impl.mController->SetShiftSelectionEnabled(shiftSelection);
      break;
    }
    case Toolkit::DevelTextEditor::Property::ENABLE_GRAB_HANDLE:
    {
      const bool grabHandleEnabled = value.Get<bool>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p ENABLE_GRAB_HANDLE %d\n", impl.mController.Get(), grabHandleEnabled);

      impl.mController->SetGrabHandleEnabled(grabHandleEnabled);
      break;
    }
    case Toolkit::DevelTextEditor::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION:
    {
      impl.mController->SetMatchLayoutDirection(value.Get<bool>() ? DevelText::MatchLayoutDirection::LOCALE : DevelText::MatchLayoutDirection::CONTENTS);
      break;
    }
    case Toolkit::DevelTextEditor::Property::MAX_LENGTH:
    {
      const int max = value.Get<int>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p MAX_LENGTH %d\n", impl.mController.Get(), max);

      impl.mController->SetMaximumNumberOfCharacters(max);
      break;
    }
    case Toolkit::DevelTextEditor::Property::SELECTED_TEXT_START:
    {
      uint32_t start = static_cast<uint32_t>(value.Get<int>());
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p SELECTED_TEXT_START %d\n", impl.mController.Get(), start);
      impl.SetTextSelectionRange(&start, nullptr);
      break;
    }
    case Toolkit::DevelTextEditor::Property::SELECTED_TEXT_END:
    {
      uint32_t end = static_cast<uint32_t>(value.Get<int>());
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p SELECTED_TEXT_END %d\n", impl.mController.Get(), end);
      impl.SetTextSelectionRange(nullptr, &end);
      break;
    }
    case Toolkit::DevelTextEditor::Property::ENABLE_EDITING:
    {
      const bool editable = value.Get<bool>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p ENABLE_EDITING %d\n", impl.mController.Get(), editable);
      impl.SetEditable(editable);
      break;
    }
    case Toolkit::DevelTextEditor::Property::HORIZONTAL_SCROLL_POSITION:
    {
      float horizontalScroll = value.Get<float>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p HORIZONTAL_SCROLL_POSITION %d\n", impl.mController.Get(), horizontalScroll);
      if(horizontalScroll >= 0.0f)
      {
        impl.ScrollBy(Vector2(horizontalScroll - impl.GetHorizontalScrollPosition(), 0));
      }
      break;
    }
    case Toolkit::DevelTextEditor::Property::VERTICAL_SCROLL_POSITION:
    {
      float verticalScroll = value.Get<float>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p VERTICAL_SCROLL_POSITION %d\n", impl.mController.Get(), verticalScroll);
      if(verticalScroll >= 0.0f)
      {
        impl.ScrollBy(Vector2(0, verticalScroll - impl.GetVerticalScrollPosition()));
      }
      break;
    }
    case Toolkit::DevelTextEditor::Property::FONT_SIZE_SCALE:
    {
      const float scale = value.Get<float>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p FONT_SIZE_SCALE %f\n", impl.mController.Get(), scale);

      if(!Equals(impl.mController->GetFontSizeScale(), scale))
      {
        impl.mController->SetFontSizeScale(scale);
      }
      break;
    }
    case Toolkit::DevelTextEditor::Property::ENABLE_FONT_SIZE_SCALE:
    {
      const bool enableFontSizeScale = value.Get<bool>();
      if(!Equals(impl.mController->IsFontSizeScaleEnabled(), enableFontSizeScale))
      {
        impl.mController->SetFontSizeScaleEnabled(enableFontSizeScale);
      }
      break;
    }
    case Toolkit::DevelTextEditor::Property::PRIMARY_CURSOR_POSITION:
    {
      uint32_t position = static_cast<uint32_t>(value.Get<int>());
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p PRIMARY_CURSOR_POSITION %d\n", impl.mController.Get(), position);
      if(impl.mController->SetPrimaryCursorPosition(position, impl.HasKeyInputFocus()))
      {
        impl.SetKeyInputFocus();
      }
      break;
    }
    case Toolkit::DevelTextEditor::Property::GRAB_HANDLE_COLOR:
    {
      const Vector4 color = value.Get<Vector4>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p GRAB_HANDLE_COLOR %f,%f,%f,%f\n", impl.mController.Get(), color.r, color.g, color.b, color.a);

      impl.mDecorator->SetHandleColor(color);
      impl.RequestTextRelayout();
      break;
    }
    case Toolkit::DevelTextEditor::Property::ENABLE_GRAB_HANDLE_POPUP:
    {
      const bool grabHandlePopupEnabled = value.Get<bool>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p ENABLE_GRAB_HANDLE_POPUP %d\n", impl.mController.Get(), grabHandlePopupEnabled);

      impl.mController->SetGrabHandlePopupEnabled(grabHandlePopupEnabled);
      break;
    }
    case Toolkit::DevelTextEditor::Property::INPUT_METHOD_SETTINGS:
    {
      const Property::Map* map = value.GetMap();
      if(map)
      {
        impl.mInputMethodOptions.ApplyProperty(*map);
      }
      impl.mController->SetInputModePassword(impl.mInputMethodOptions.IsPassword());

      Toolkit::Control control = Toolkit::KeyInputFocusManager::Get().GetCurrentFocusControl();
      if(control == textEditor)
      {
        impl.mInputMethodContext.ApplyOptions(impl.mInputMethodOptions);
      }
      break;
    }
    case Toolkit::DevelTextEditor::Property::INPUT_FILTER:
    {
      const Property::Map* map = value.GetMap();
      if(map)
      {
        impl.mController->SetInputFilterOption(*map);
      }
      break;
    }
    case Toolkit::DevelTextEditor::Property::ELLIPSIS:
    {
      const bool ellipsis = value.Get<bool>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p ELLIPSIS %d\n", impl.mController.Get(), ellipsis);

      impl.mController->SetTextElideEnabled(ellipsis);
      break;
    }
    case Toolkit::DevelTextEditor::Property::ELLIPSIS_POSITION:
    {
      DevelText::EllipsisPosition::Type ellipsisPositionType(static_cast<DevelText::EllipsisPosition::Type>(-1)); // Set to invalid value to ensure a valid mode does get set
      if(Text::GetEllipsisPositionTypeEnumeration(value, ellipsisPositionType))
      {
        DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p EllipsisPosition::Type %d\n", impl.mController.Get(), ellipsisPositionType);
        impl.mController->SetEllipsisPosition(ellipsisPositionType);
      }
      break;
    }
    case Toolkit::DevelTextEditor::Property::MIN_LINE_SIZE:
    {
      const float minLineSize = value.Get<float>();
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor %p MIN_LINE_SIZE %f\n", impl.mController.Get(), minLineSize);

      impl.mController->SetDefaultLineSize(minLineSize);
      impl.mRenderer.Reset();
      break;
    }
  }
}

Property::Value TextEditor::PropertyHandler::GetProperty(Toolkit::TextEditor textEditor, Property::Index index)
{
  Property::Value value;
  TextEditor&     impl(GetImpl(textEditor));
  DALI_ASSERT_DEBUG(impl.mController && "No text controller");
  DALI_ASSERT_DEBUG(impl.mDecorator && "No text decorator");

  switch(index)
  {
    case Toolkit::DevelTextEditor::Property::RENDERING_BACKEND:
    {
      value = impl.mRenderingBackend;
      break;
    }
    case Toolkit::TextEditor::Property::TEXT:
    {
      std::string text;
      impl.mController->GetText(text);
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::General, "TextEditor %p returning text: %s\n", impl.mController.Get(), text.c_str());
      value = text;
      break;
    }
    case Toolkit::TextEditor::Property::TEXT_COLOR:
    {
      value = impl.mController->GetDefaultColor();
      break;
    }
    case Toolkit::TextEditor::Property::FONT_FAMILY:
    {
      value = impl.mController->GetDefaultFontFamily();
      break;
    }
    case Toolkit::TextEditor::Property::FONT_STYLE:
    {
      GetFontStyleProperty(impl.mController, value, Text::FontStyle::DEFAULT);
      break;
    }
    case Toolkit::TextEditor::Property::POINT_SIZE:
    {
      value = impl.mController->GetDefaultFontSize(Text::Controller::POINT_SIZE);
      break;
    }
    case Toolkit::TextEditor::Property::HORIZONTAL_ALIGNMENT:
    {
      const char* name = Text::GetHorizontalAlignmentString(impl.mController->GetHorizontalAlignment());
      if(name)
      {
        value = std::string(name);
      }
      break;
    }
    case Toolkit::TextEditor::Property::SCROLL_THRESHOLD:
    {
      value = impl.mDecorator->GetScrollThreshold();
      break;
    }
    case Toolkit::TextEditor::Property::SCROLL_SPEED:
    {
      value = impl.mDecorator->GetScrollSpeed();
      break;
    }
    case Toolkit::TextEditor::Property::PRIMARY_CURSOR_COLOR:
    {
      value = impl.mDecorator->GetColor(Text::PRIMARY_CURSOR);
      break;
    }
    case Toolkit::TextEditor::Property::SECONDARY_CURSOR_COLOR:
    {
      value = impl.mDecorator->GetColor(Text::SECONDARY_CURSOR);
      break;
    }
    case Toolkit::TextEditor::Property::ENABLE_CURSOR_BLINK:
    {
      value = impl.mController->GetEnableCursorBlink();
      break;
    }
    case Toolkit::TextEditor::Property::CURSOR_BLINK_INTERVAL:
    {
      value = impl.mDecorator->GetCursorBlinkInterval();
      break;
    }
    case Toolkit::TextEditor::Property::CURSOR_BLINK_DURATION:
    {
      value = impl.mDecorator->GetCursorBlinkDuration();
      break;
    }
    case Toolkit::TextEditor::Property::CURSOR_WIDTH:
    {
      value = impl.mDecorator->GetCursorWidth();
      break;
    }
    case Toolkit::TextEditor::Property::GRAB_HANDLE_IMAGE:
    {
      value = impl.mDecorator->GetHandleImage(Text::GRAB_HANDLE, Text::HANDLE_IMAGE_RELEASED);
      break;
    }
    case Toolkit::TextEditor::Property::GRAB_HANDLE_PRESSED_IMAGE:
    {
      value = impl.mDecorator->GetHandleImage(Text::GRAB_HANDLE, Text::HANDLE_IMAGE_PRESSED);
      break;
    }
    case Toolkit::TextEditor::Property::SELECTION_HANDLE_IMAGE_LEFT:
    {
      impl.GetHandleImagePropertyValue(value, Text::LEFT_SELECTION_HANDLE, Text::HANDLE_IMAGE_RELEASED);
      break;
    }
    case Toolkit::TextEditor::Property::SELECTION_HANDLE_IMAGE_RIGHT:
    {
      impl.GetHandleImagePropertyValue(value, Text::RIGHT_SELECTION_HANDLE, Text::HANDLE_IMAGE_RELEASED);
      break;
    }
    case Toolkit::TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT:
    {
      impl.GetHandleImagePropertyValue(value, Text::LEFT_SELECTION_HANDLE, Text::HANDLE_IMAGE_PRESSED);
      break;
    }
    case Toolkit::TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT:
    {
      impl.GetHandleImagePropertyValue(value, Text::RIGHT_SELECTION_HANDLE, Text::HANDLE_IMAGE_PRESSED);
      break;
    }
    case Toolkit::TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT:
    {
      impl.GetHandleImagePropertyValue(value, Text::LEFT_SELECTION_HANDLE_MARKER, Text::HANDLE_IMAGE_RELEASED);
      break;
    }
    case Toolkit::TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT:
    {
      impl.GetHandleImagePropertyValue(value, Text::RIGHT_SELECTION_HANDLE_MARKER, Text::HANDLE_IMAGE_RELEASED);
      break;
    }
    case Toolkit::TextEditor::Property::SELECTION_HIGHLIGHT_COLOR:
    {
      value = impl.mDecorator->GetHighlightColor();
      break;
    }
    case Toolkit::TextEditor::Property::DECORATION_BOUNDING_BOX:
    {
      Rect<int> boundingBox;
      impl.mDecorator->GetBoundingBox(boundingBox);
      value = boundingBox;
      break;
    }
    case Toolkit::TextEditor::Property::ENABLE_MARKUP:
    {
      value = impl.mController->IsMarkupProcessorEnabled();
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_COLOR:
    {
      value = impl.mController->GetInputColor();
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_FONT_FAMILY:
    {
      value = impl.mController->GetInputFontFamily();
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_FONT_STYLE:
    {
      GetFontStyleProperty(impl.mController, value, Text::FontStyle::INPUT);
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_POINT_SIZE:
    {
      value = impl.mController->GetInputFontPointSize();
      break;
    }
    case Toolkit::TextEditor::Property::LINE_SPACING:
    {
      value = impl.mController->GetDefaultLineSpacing();
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_LINE_SPACING:
    {
      value = impl.mController->GetInputLineSpacing();
      break;
    }
    case Toolkit::TextEditor::Property::UNDERLINE:
    {
      GetUnderlineProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_UNDERLINE:
    {
      GetUnderlineProperties(impl.mController, value, Text::EffectStyle::INPUT);
      break;
    }
    case Toolkit::TextEditor::Property::SHADOW:
    {
      GetShadowProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_SHADOW:
    {
      GetShadowProperties(impl.mController, value, Text::EffectStyle::INPUT);
      break;
    }
    case Toolkit::TextEditor::Property::EMBOSS:
    {
      GetEmbossProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_EMBOSS:
    {
      GetEmbossProperties(impl.mController, value, Text::EffectStyle::INPUT);
      break;
    }
    case Toolkit::TextEditor::Property::OUTLINE:
    {
      GetOutlineProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
      break;
    }
    case Toolkit::TextEditor::Property::INPUT_OUTLINE:
    {
      GetOutlineProperties(impl.mController, value, Text::EffectStyle::INPUT);
      break;
    }
    case Toolkit::TextEditor::Property::SMOOTH_SCROLL:
    {
      value = impl.mScrollAnimationEnabled;
      break;
    }
    case Toolkit::TextEditor::Property::SMOOTH_SCROLL_DURATION:
    {
      value = impl.mScrollAnimationDuration;
      break;
    }
    case Toolkit::TextEditor::Property::ENABLE_SCROLL_BAR:
    {
      value = impl.mScrollBarEnabled;
      break;
    }
    case Toolkit::TextEditor::Property::SCROLL_BAR_SHOW_DURATION:
    {
      value = impl.mAnimationPeriod.delaySeconds;
      break;
    }
    case Toolkit::TextEditor::Property::SCROLL_BAR_FADE_DURATION:
    {
      value = impl.mAnimationPeriod.durationSeconds;
      break;
    }
    case Toolkit::TextEditor::Property::PIXEL_SIZE:
    {
      value = impl.mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE);
      break;
    }
    case Toolkit::TextEditor::Property::LINE_COUNT:
    {
      float width = textEditor.GetProperty(Actor::Property::SIZE_WIDTH).Get<float>();
      value       = impl.mController->GetLineCount(width);
      break;
    }
    case Toolkit::DevelTextEditor::Property::PLACEHOLDER_TEXT:
    {
      std::string text;
      impl.mController->GetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_INACTIVE, text);
      value = text;
      break;
    }
    case Toolkit::DevelTextEditor::Property::PLACEHOLDER_TEXT_COLOR:
    {
      value = impl.mController->GetPlaceholderTextColor();
      break;
    }
    case Toolkit::TextEditor::Property::ENABLE_SELECTION:
    {
      value = impl.mController->IsSelectionEnabled();
      break;
    }
    case Toolkit::TextEditor::Property::PLACEHOLDER:
    {
      Property::Map map;
      impl.mController->GetPlaceholderProperty(map);
      value = map;
      break;
    }
    case Toolkit::TextEditor::Property::LINE_WRAP_MODE:
    {
      value = impl.mController->GetLineWrapMode();
      break;
    }
    case Toolkit::DevelTextEditor::Property::ENABLE_SHIFT_SELECTION:
    {
      value = impl.mController->IsShiftSelectionEnabled();
      break;
    }
    case Toolkit::DevelTextEditor::Property::ENABLE_GRAB_HANDLE:
    {
      value = impl.mController->IsGrabHandleEnabled();
      break;
    }
    case Toolkit::DevelTextEditor::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION:
    {
      value = impl.mController->GetMatchLayoutDirection() != DevelText::MatchLayoutDirection::CONTENTS;
      break;
    }
    case Toolkit::DevelTextEditor::Property::MAX_LENGTH:
    {
      value = impl.mController->GetMaximumNumberOfCharacters();
      break;
    }
    case Toolkit::DevelTextEditor::Property::SELECTED_TEXT:
    {
      value = impl.mController->GetSelectedText();
      break;
    }
    case Toolkit::DevelTextEditor::Property::SELECTED_TEXT_START:
    {
      Uint32Pair range = impl.GetTextSelectionRange();
      value            = static_cast<int>(range.first);
      break;
    }
    case Toolkit::DevelTextEditor::Property::SELECTED_TEXT_END:
    {
      Uint32Pair range = impl.GetTextSelectionRange();
      value            = static_cast<int>(range.second);
      break;
    }
    case Toolkit::DevelTextEditor::Property::ENABLE_EDITING:
    {
      value = impl.IsEditable();
      break;
    }
    case Toolkit::DevelTextEditor::Property::HORIZONTAL_SCROLL_POSITION:
    {
      value = impl.GetHorizontalScrollPosition();
      break;
    }
    case Toolkit::DevelTextEditor::Property::VERTICAL_SCROLL_POSITION:
    {
      value = impl.GetVerticalScrollPosition();
      break;
    }
    case Toolkit::DevelTextEditor::Property::FONT_SIZE_SCALE:
    {
      value = impl.mController->GetFontSizeScale();
      break;
    }
    case Toolkit::DevelTextEditor::Property::ENABLE_FONT_SIZE_SCALE:
    {
      value = impl.mController->IsFontSizeScaleEnabled();
      break;
    }
    case Toolkit::DevelTextEditor::Property::PRIMARY_CURSOR_POSITION:
    {
      value = static_cast<int>(impl.mController->GetPrimaryCursorPosition());
      break;
    }
    case Toolkit::DevelTextEditor::Property::GRAB_HANDLE_COLOR:
    {
      value = impl.mDecorator->GetHandleColor();
      break;
    }
    case Toolkit::DevelTextEditor::Property::ENABLE_GRAB_HANDLE_POPUP:
    {
      value = impl.mController->IsGrabHandlePopupEnabled();
      break;
    }
    case Toolkit::DevelTextEditor::Property::INPUT_METHOD_SETTINGS:
    {
      Property::Map map;
      impl.mInputMethodOptions.RetrieveProperty(map);
      value = map;
      break;
    }
    case Toolkit::DevelTextEditor::Property::INPUT_FILTER:
    {
      Property::Map map;
      impl.mController->GetInputFilterOption(map);
      value = map;
      break;
    }
    case Toolkit::DevelTextEditor::Property::ELLIPSIS:
    {
      value = impl.mController->IsTextElideEnabled();
      break;
    }
    case Toolkit::DevelTextEditor::Property::ELLIPSIS_POSITION:
    {
      value = impl.mController->GetEllipsisPosition();
      break;
    }
    case Toolkit::DevelTextEditor::Property::MIN_LINE_SIZE:
    {
      value = impl.mController->GetDefaultLineSize();
      break;
    }
  } //switch
  return value;
}

} // namespace Dali::Toolkit::Internal
