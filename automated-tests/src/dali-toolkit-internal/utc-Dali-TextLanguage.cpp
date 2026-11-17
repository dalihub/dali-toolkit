/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-editor-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-field-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>
#include <dali-toolkit/internal/controls/text-controls/text-label-impl.h>
#include <dali-toolkit/internal/text/controller/text-controller-impl.h>
#include <dali-toolkit/internal/text/text-editable-control-interface.h>
#include <dali/devel-api/object/type-info.h>
#include <dali/integration-api/adaptor-framework/input-method-context-integ.h>
#include <dali/integration-api/string-utils.h>
#include <string>
#include <type_traits>
#include <utility>

using namespace Dali;
using namespace Dali::Toolkit;

int UtcDaliTextLanguageGetterLifetimeP(void)
{
  static_assert(std::is_same_v<decltype(std::declval<const Text::Controller&>().GetTextLanguage()), std::string>);
  static_assert(std::is_same_v<decltype(std::declval<const Text::Controller::Impl&>().GetTextLanguage()), const std::string&>);
  ToolkitTestApplication application;
  std::string            maximum = "x";
  for(unsigned i = 0u; i < 127u; ++i) maximum += "-a";
  for(const auto& tag : {std::string("bg"), std::string("sr-cyrl-rs-x-long-snapshot"), maximum})
  {
    auto controller = Text::Controller::New();
    DALI_TEST_CHECK(controller->GetTextLanguage().empty());
    DALI_TEST_CHECK(!Text::Controller::Impl::GetImplementation(*controller).mTextLanguage);
    DALI_TEST_CHECK(controller->SetTextLanguage(tag == "bg" ? "BG" : tag));
    const std::string saved = controller->GetTextLanguage();
    // A const reference now extends the lifetime of the returned value.
    const auto& retained = controller->GetTextLanguage();
    DALI_TEST_EQUALS(saved, tag, TEST_LOCATION);
    DALI_TEST_CHECK(!controller->SetTextLanguage("bg_BG"));
    DALI_TEST_CHECK(!controller->SetTextLanguage(maximum + "a"));
    DALI_TEST_EQUALS(controller->GetTextLanguage(), tag, TEST_LOCATION);
    DALI_TEST_CHECK(controller->SetTextLanguage("ru"));
    DALI_TEST_EQUALS(retained, tag, TEST_LOCATION);
    DALI_TEST_CHECK(controller->SetTextLanguage(""));
    DALI_TEST_CHECK(controller->GetTextLanguage().empty());
    DALI_TEST_EQUALS(saved, tag, TEST_LOCATION);
    DALI_TEST_EQUALS(retained, tag, TEST_LOCATION);
    controller.Reset();
    DALI_TEST_EQUALS(saved, tag, TEST_LOCATION);
    DALI_TEST_EQUALS(retained, tag, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliTextLanguageWithoutEventDataP(void)
{
  ToolkitTestApplication application;
  auto                   controller = Text::Controller::New();
  auto&                  impl       = Text::Controller::Impl::GetImplementation(*controller);
  DALI_TEST_CHECK(!impl.mEventData);
  for(const std::string& tag : {std::string("bg"), std::string("ru"), std::string()})
  {
    impl.mOperationsPending = Text::Controller::NO_OPERATION;
    DALI_TEST_CHECK(controller->SetTextLanguage(tag));
    DALI_TEST_CHECK(!impl.mEventData);
    DALI_TEST_EQUALS(controller->GetTextLanguage(), tag, TEST_LOCATION);
    DALI_TEST_CHECK(impl.mOperationsPending & Text::Controller::SHAPE_TEXT);
    DALI_TEST_CHECK(!(impl.mOperationsPending & Text::Controller::VALIDATE_FONTS));
    DALI_TEST_CHECK(impl.mTextUpdateInfo.mFullRelayoutNeeded);
    impl.mOperationsPending = Text::Controller::NO_OPERATION;
    DALI_TEST_CHECK(controller->SetTextLanguage(tag));
    DALI_TEST_CHECK(!controller->SetTextLanguage("bg_BG"));
    DALI_TEST_CHECK(!impl.mEventData);
    DALI_TEST_EQUALS(impl.mOperationsPending, Text::Controller::NO_OPERATION, TEST_LOCATION);
    DALI_TEST_EQUALS(controller->GetTextLanguage(), tag, TEST_LOCATION);
  }
  DALI_TEST_CHECK(!impl.mTextLanguage);
  END_TEST;
}

int UtcDaliTextLanguagePropertyValueLifetimeP(void)
{
  ToolkitTestApplication application;
  std::string            maximum = "x";
  for(unsigned i = 0u; i < 127u; ++i) maximum += "-a";
  const Property::Index indices[] = {DevelTextLabel::Property::TEXT_LANGUAGE, DevelTextField::Property::TEXT_LANGUAGE, DevelTextEditor::Property::TEXT_LANGUAGE};
  Property::Value       saved[3];
  {
    Control controls[] = {TextLabel::New(), TextField::New(), TextEditor::New()};
    for(unsigned i = 0u; i < 3u; ++i)
    {
      auto control = controls[i];
      control.SetProperty(indices[i], Integration::ToPropertyValue(maximum));
      saved[i] = control.GetProperty(indices[i]);
      control.SetProperty(indices[i], "ru");
      DALI_TEST_EQUALS(Integration::ToStdString(saved[i]), maximum, TEST_LOCATION);
      control.SetProperty(indices[i], "");
      DALI_TEST_CHECK(control.GetProperty<Dali::String>(indices[i]).Empty());
      DALI_TEST_EQUALS(Integration::ToStdString(saved[i]), maximum, TEST_LOCATION);
    }
  }
  for(const auto& value : saved)
  {
    DALI_TEST_EQUALS(value.GetType(), Property::STRING, TEST_LOCATION);
    DALI_TEST_EQUALS(Integration::ToStdString(value), maximum, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliTextLanguageDevelPropertiesP(void)
{
  ToolkitTestApplication application;
  DALI_TEST_EQUALS(int(DevelTextLabel::Property::TEXT_LANGUAGE), 10001053, TEST_LOCATION);
  DALI_TEST_EQUALS(int(DevelTextField::Property::TEXT_LANGUAGE), 10001074, TEST_LOCATION);
  DALI_TEST_EQUALS(int(DevelTextEditor::Property::TEXT_LANGUAGE), 10001084, TEST_LOCATION);
  DALI_TEST_EQUALS(int(DevelTextLabel::Property::AUTO_SCROLL_DIRECTION), 10001052, TEST_LOCATION);
  DALI_TEST_EQUALS(int(DevelTextField::Property::ENABLE_CURSOR_INSET), 10001073, TEST_LOCATION);
  DALI_TEST_EQUALS(int(DevelTextEditor::Property::ENABLE_DEFERRED_RELAYOUT), 10001083, TEST_LOCATION);
  Control               controls[] = {TextLabel::New(), TextField::New(), TextEditor::New()};
  const Property::Index indices[]  = {DevelTextLabel::Property::TEXT_LANGUAGE, DevelTextField::Property::TEXT_LANGUAGE, DevelTextEditor::Property::TEXT_LANGUAGE};
  for(unsigned i = 0u; i < 3u; ++i)
  {
    auto       control = controls[i];
    const auto index   = indices[i];
    DALI_TEST_EQUALS(control.GetPropertyIndex("textLanguage"), index, TEST_LOCATION);
    DALI_TEST_EQUALS(control.GetPropertyName(index), Dali::String("textLanguage"), TEST_LOCATION);
    DALI_TEST_EQUALS(control.GetPropertyType(index), Property::STRING, TEST_LOCATION);
    DALI_TEST_CHECK(control.IsPropertyWritable(index));
    DALI_TEST_CHECK(!control.IsPropertyAnimatable(index));
    DALI_TEST_EQUALS(control.GetProperty<Dali::String>(index), Dali::String(), TEST_LOCATION);
    control.SetProperty(control.GetPropertyIndex("text"), "unchanged text");
    control.SetProperty(control.GetPropertyIndex("fontFamily"), "Test Sans");
    const auto direction = control.GetProperty(Actor::Property::LAYOUT_DIRECTION);
    control.SetProperty(index, "SR-Cyrl-RS");
    DALI_TEST_EQUALS(control.GetProperty<Dali::String>(index), Dali::String("sr-cyrl-rs"), TEST_LOCATION);
    for(const auto& invalid : {std::string("sr_RS"), std::string("sr\0RS", 5u), std::string(" bg")})
    {
      control.SetProperty(index, Dali::String(Dali::StringView(invalid.data(), invalid.size())));
      DALI_TEST_EQUALS(control.GetProperty<Dali::String>(index), Dali::String("sr-cyrl-rs"), TEST_LOCATION);
    }
    Property::Map properties;
    properties.Insert("textLanguage", "BG");
    control.SetProperties(properties);
    DALI_TEST_EQUALS(control.GetProperty<Dali::String>(index), Dali::String("bg"), TEST_LOCATION);
    Property::Map saved;
    saved.Insert("textLanguage", control.GetProperty(index));
    control.SetProperty(index, "und");
    DALI_TEST_EQUALS(control.GetProperty<Dali::String>(index), Dali::String("und"), TEST_LOCATION);
    control.SetProperties(saved);
    DALI_TEST_EQUALS(control.GetProperty<Dali::String>(index), Dali::String("bg"), TEST_LOCATION);
    control.SetProperty(index, "");
    DALI_TEST_EQUALS(control.GetProperty<Dali::String>(index), Dali::String(), TEST_LOCATION);
    DALI_TEST_EQUALS(control.GetProperty<Dali::String>(control.GetPropertyIndex("text")), Dali::String("unchanged text"), TEST_LOCATION);
    DALI_TEST_EQUALS(control.GetProperty<Dali::String>(control.GetPropertyIndex("fontFamily")), Dali::String("Test Sans"), TEST_LOCATION);
    DALI_TEST_CHECK(control.GetProperty(Actor::Property::LAYOUT_DIRECTION) == direction);
    TypeInfo type;
    DALI_TEST_CHECK(control.GetTypeInfo(type));
    DALI_TEST_EQUALS(Dali::String(type.GetPropertyName(index)), Dali::String("textLanguage"), TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliTextLanguageInvalidationP(void)
{
  ToolkitTestApplication application;
  auto                   controller = Text::Controller::New();
  controller->SetText("Cyrillic text");
  controller->Relayout(Size(400.f, 100.f));
  auto&       impl        = Text::Controller::Impl::GetImplementation(*controller);
  const auto  fontCount   = impl.mModel->mLogicalModel->mFontRuns.Count();
  const auto* fonts       = impl.mModel->mLogicalModel->mFontRuns.Begin();
  impl.mOperationsPending = Text::Controller::NO_OPERATION;
  DALI_TEST_CHECK(!impl.mTextLanguage);
  DALI_TEST_CHECK(controller->SetTextLanguage(""));
  DALI_TEST_CHECK(!controller->SetTextLanguage("bg_BG"));
  DALI_TEST_CHECK(!impl.mTextLanguage);
  DALI_TEST_EQUALS(impl.mOperationsPending, Text::Controller::NO_OPERATION, TEST_LOCATION);
  DALI_TEST_CHECK(controller->SetTextLanguage("BG"));
  DALI_TEST_CHECK(impl.mTextLanguage);
  DALI_TEST_CHECK(!(impl.mOperationsPending & Text::Controller::VALIDATE_FONTS));
  DALI_TEST_CHECK(impl.mOperationsPending & Text::Controller::SHAPE_TEXT);
  DALI_TEST_CHECK(impl.mOperationsPending & Text::Controller::COLOR);
  DALI_TEST_CHECK(impl.mRecalculateNaturalSize && impl.mRecalculateLayoutSize);
  controller->Relayout(Size(400.f, 100.f));
  DALI_TEST_EQUALS(impl.mModel->mLogicalModel->mFontRuns.Count(), fontCount, TEST_LOCATION);
  DALI_TEST_CHECK(impl.mModel->mLogicalModel->mFontRuns.Begin() == fonts);
  impl.mOperationsPending = Text::Controller::NO_OPERATION;
  DALI_TEST_CHECK(controller->SetTextLanguage("bg"));
  DALI_TEST_CHECK(controller->SetTextLanguage("BG"));
  DALI_TEST_CHECK(!controller->SetTextLanguage("bg_BG"));
  DALI_TEST_EQUALS(impl.mOperationsPending, Text::Controller::NO_OPERATION, TEST_LOCATION);
  DALI_TEST_CHECK(controller->SetTextLanguage(""));
  DALI_TEST_CHECK(!impl.mTextLanguage);
  END_TEST;
}

int UtcDaliTextLanguageManualPolicyP(void)
{
  ToolkitTestApplication application;
  auto                   label = TextLabel::New("text");
  label.SetProperty(Actor::Property::SIZE, Vector2(100.f, 50.f));
  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();
  const auto renderers = label.GetRendererCount();
  label.SetProperty(DevelTextLabel::Property::TEXT_LANGUAGE, "bg");
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(label.GetRendererCount(), renderers, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetProperty<bool>(DevelTextLabel::Property::NEED_REQUEST_ASYNC_RENDER));
  DALI_TEST_EQUALS(label.GetProperty<int>(DevelTextLabel::Property::RENDER_MODE), int(DevelTextLabel::Render::ASYNC_MANUAL), TEST_LOCATION);
  END_TEST;
}

namespace
{
// Re-enter from the same notification used by an editable control's signal.
struct LanguageOnEdit : Text::EditableControlInterface
{
  Text::ControllerPtr controller;
  bool                onInsert = false;
  void                TextInserted(unsigned, unsigned, const std::string&) override
  {
    if(onInsert) controller->SetTextLanguage("bg");
  }
  void TextDeleted(unsigned, unsigned, const std::string&) override
  {
  }
  void CursorPositionChanged(unsigned, unsigned) override
  {
  }
  void TextChanged(bool) override
  {
    if(!onInsert) controller->SetTextLanguage("sr");
  }
  void MaxLengthReached() override
  {
  }
  void InputStyleChanged(Text::InputStyle::Mask) override
  {
  }
  void InputFiltered(Toolkit::InputFilter::Property::Type) override
  {
  }
  void AddDecoration(Actor&, Text::DecorationType, bool) override
  {
  }
  void GetControlBackgroundColor(Vector4& color) const override
  {
    color = Color::WHITE;
  }
  bool IsEditable() const override
  {
    return true;
  }
  void SetEditable(bool) override
  {
  }
  std::string CopyText() override
  {
    return {};
  }
  std::string CutText() override
  {
    return {};
  }
  void PasteText() override
  {
  }
};
} //namespace

int UtcDaliTextLanguagePendingEditCallbackP(void)
{
  ToolkitTestApplication application;
  for(const auto& initial : {std::string(), std::string("ab\ncd")})
  {
    LanguageOnEdit control;
    auto           controller    = Text::Controller::New(nullptr, &control, nullptr, nullptr);
    control.controller           = controller;
    auto               decorator = Text::Decorator::New(*controller, *controller);
    InputMethodContext context   = Dali::Integration::InputMethodContext::New();
    controller->EnableTextInput(decorator, context);
    controller->SetMultiLineEnabled(true);
    controller->SetText(initial);
    controller->Relayout(Size(400.f, 100.f));
    auto& impl                              = Text::Controller::Impl::GetImplementation(*controller);
    impl.mEventData->mState                 = Text::EventData::EDITING;
    impl.mEventData->mPrimaryCursorPosition = initial.size();
    control.onInsert                        = true;
    auto insert                             = Dali::Integration::InputMethodContext::EventData(Dali::Integration::InputMethodContext::COMMIT, Dali::String("X"), 0, 1);
    controller->OnInputMethodContextEvent(context, insert);
    // Check the pending edit before shaping can read beyond the text buffer.
    const auto& update = impl.mTextUpdateInfo;
    DALI_TEST_EQUALS(update.mPreviousNumberOfCharacters + update.mNumberOfCharactersToAdd - update.mNumberOfCharactersToRemove,
                     impl.mModel->mLogicalModel->mText.Count(), TEST_LOCATION);
    controller->Relayout(Size(400.f, 100.f));
    std::string actual;
    controller->GetText(actual);
    DALI_TEST_EQUALS(actual, initial + "X", TEST_LOCATION);
    DALI_TEST_EQUALS(controller->GetTextLanguage(), std::string("bg"), TEST_LOCATION);
    control.onInsert = false;
    auto next        = Dali::Integration::InputMethodContext::EventData(Dali::Integration::InputMethodContext::COMMIT, Dali::String("Y"), 0, 1);
    controller->OnInputMethodContextEvent(context, next);
    controller->Relayout(Size(400.f, 100.f));
    controller->GetText(actual);
    DALI_TEST_EQUALS(actual, initial + "XY", TEST_LOCATION);
    DALI_TEST_EQUALS(controller->GetTextLanguage(), std::string("sr"), TEST_LOCATION);
    auto event = Dali::Integration::InputMethodContext::EventData(Dali::Integration::InputMethodContext::DELETE_SURROUNDING, Dali::String(), -1, 1);
    controller->OnInputMethodContextEvent(context, event);
    controller->SetTextLanguage("mk");
    controller->Relayout(Size(400.f, 100.f));
    controller->GetText(actual);
    DALI_TEST_EQUALS(actual, initial + "X", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliTextLanguagePreeditP(void)
{
  ToolkitTestApplication application;
  auto                   controller = Text::Controller::New();
  auto                   decorator  = Text::Decorator::New(*controller, *controller);
  InputMethodContext     context    = Dali::Integration::InputMethodContext::New();
  controller->EnableTextInput(decorator, context);
  controller->SetMultiLineEnabled(true);
  controller->SetText("ab");
  controller->Relayout(Size(400.f, 100.f));
  auto& impl                              = Text::Controller::Impl::GetImplementation(*controller);
  impl.mEventData->mState                 = Text::EventData::EDITING;
  impl.mEventData->mPrimaryCursorPosition = 2u;
  auto preedit                            = Dali::Integration::InputMethodContext::EventData(Dali::Integration::InputMethodContext::PRE_EDIT, Dali::String("xy"), 2, 2);
  controller->OnInputMethodContextEvent(context, preedit);
  DALI_TEST_CHECK(impl.mEventData->mPreEditFlag);
  const auto cursor = impl.mEventData->mPrimaryCursorPosition;
  controller->SetTextLanguage("bg");
  controller->Relayout(Size(400.f, 100.f));
  DALI_TEST_CHECK(impl.mEventData->mPreEditFlag);
  DALI_TEST_EQUALS(impl.mEventData->mPrimaryCursorPosition, cursor, TEST_LOCATION);
  std::string actual;
  controller->GetText(actual);
  DALI_TEST_EQUALS(actual, std::string("abxy"), TEST_LOCATION);
  auto commit = Dali::Integration::InputMethodContext::EventData(Dali::Integration::InputMethodContext::COMMIT, Dali::String("Z"), 0, 1);
  controller->OnInputMethodContextEvent(context, commit);
  controller->Relayout(Size(400.f, 100.f));
  controller->GetText(actual);
  DALI_TEST_EQUALS(actual, std::string("abZ"), TEST_LOCATION);
  DALI_TEST_CHECK(!impl.mEventData->mPreEditFlag);
  END_TEST;
}

int UtcDaliTextLanguageEditingStateP(void)
{
  ToolkitTestApplication application;
  auto                   controller = Text::Controller::New();
  auto                   decorator  = Text::Decorator::New(*controller, *controller);
  InputMethodContext     inputMethodContext;
  controller->EnableTextInput(decorator, inputMethodContext);
  controller->SetText("select words here");
  controller->Relayout(Size(400.f, 100.f));
  auto& impl                               = Text::Controller::Impl::GetImplementation(*controller);
  impl.mEventData->mState                  = Text::EventData::SELECTING;
  impl.mEventData->mPrimaryCursorPosition  = 3u;
  impl.mEventData->mLeftSelectionPosition  = 1u;
  impl.mEventData->mRightSelectionPosition = 5u;
  DALI_TEST_CHECK(controller->SetTextLanguage("bg"));
  DALI_TEST_CHECK(impl.mEventData->mUpdateLeftSelectionPosition);
  DALI_TEST_CHECK(impl.mEventData->mUpdateRightSelectionPosition);
  DALI_TEST_CHECK(impl.mEventData->mUpdateHighlightBox);
  controller->Relayout(Size(400.f, 100.f));
  DALI_TEST_EQUALS(impl.mEventData->mState, Text::EventData::SELECTING, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.mEventData->mPrimaryCursorPosition, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.mEventData->mLeftSelectionPosition, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.mEventData->mRightSelectionPosition, 5u, TEST_LOCATION);
  impl.mEventData->mState = Text::EventData::EDITING_WITH_GRAB_HANDLE;
  DALI_TEST_CHECK(controller->SetTextLanguage("sr"));
  DALI_TEST_CHECK(impl.mEventData->mUpdateGrabHandlePosition);
  DALI_TEST_CHECK(impl.mEventData->mScrollAfterUpdatePosition);
  controller->Relayout(Size(400.f, 100.f));
  DALI_TEST_EQUALS(decorator->GetPosition(Text::GRAB_HANDLE).x,
                   decorator->GetPosition(Text::PRIMARY_CURSOR).x, TEST_LOCATION);
  END_TEST;
}
