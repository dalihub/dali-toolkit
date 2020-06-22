/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <iostream>

#include <stdlib.h>
#include <limits>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <toolkit-text-utils.h>
#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali-toolkit/internal/text/text-control-interface.h>
#include <dali-toolkit/internal/text/text-editable-control-interface.h>
#include <dali-toolkit/internal/text/text-controller-impl.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

namespace
{

const char* const OPTION_SELECT_ALL("option-select_all");   // "Select All" popup option.
const char* const OPTION_CUT("optionCut");                  // "Cut" popup option.
const char* const OPTION_COPY("optionCopy");                // "Copy" popup option.
const char* const OPTION_PASTE("optionPaste");              // "Paste" popup option.
const char* const OPTION_CLIPBOARD("optionClipboard");      // "Clipboard" popup option.

const Size CONTROL_SIZE( 300.f, 60.f );

std::string gClipboardText;
void ContentSelectedCallback( ClipboardEventNotifier& notifier )
{
  gClipboardText = notifier.GetContent();
}

// Generate a KeyEvent to send to Core.
Dali::KeyEvent GenerateKey( const std::string& keyName,
                            const std::string& keyString,
                            int keyCode,
                            int keyModifier,
                            unsigned long timeStamp,
                            const Dali::KeyEvent::State& keyState )
{
  return Dali::KeyEvent( keyName,
                         keyString,
                         keyCode,
                         keyModifier,
                         timeStamp,
                         keyState );
}

} // namespace

int UtcDaliTextController(void)
{
  tet_infoline(" UtcDaliTextController");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();
  DALI_TEST_CHECK( controller );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextControllerSetGetScrollEnabled(void)
{
  tet_infoline(" UtcDaliTextControllerSetGetScrollEnabled");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();
  DALI_TEST_CHECK( controller );

  // Configures the text controller similarly to the text-editor.
  ConfigureTextEditor( controller );

  DALI_TEST_CHECK( !controller->IsHorizontalScrollEnabled() );
  DALI_TEST_CHECK( controller->IsVerticalScrollEnabled() );

  // Configures the text controller similarly to the text-field.
  ConfigureTextField( controller );

  DALI_TEST_CHECK( controller->IsHorizontalScrollEnabled() );
  DALI_TEST_CHECK( !controller->IsVerticalScrollEnabled() );

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel( controller );

  DALI_TEST_CHECK( !controller->IsHorizontalScrollEnabled() );
  DALI_TEST_CHECK( !controller->IsVerticalScrollEnabled() );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextControllerSetIsTextElide(void)
{
  tet_infoline(" UtcDaliTextControllerSetIsTextElide");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();
  DALI_TEST_CHECK( controller );

  // Configures the text controller similarly to the text-editor.
  ConfigureTextEditor( controller );
  DALI_TEST_EQUALS( false, controller->IsTextElideEnabled(), TEST_LOCATION );

  controller->SetTextElideEnabled( true );
  DALI_TEST_EQUALS( true, controller->IsTextElideEnabled(), TEST_LOCATION );

  // Configures the text controller similarly to the text-field.
  ConfigureTextField( controller );
  DALI_TEST_EQUALS( false, controller->IsTextElideEnabled(), TEST_LOCATION );

  controller->SetTextElideEnabled( true );
  DALI_TEST_EQUALS( true, controller->IsTextElideEnabled(), TEST_LOCATION );

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel( controller );
  DALI_TEST_EQUALS( true, controller->IsTextElideEnabled(), TEST_LOCATION );

  controller->SetTextElideEnabled( false );
  DALI_TEST_EQUALS( false, controller->IsTextElideEnabled(), TEST_LOCATION );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextControllerEnableCursorBlinking(void)
{
  tet_infoline(" UtcDaliTextControllerEnableCursorBlinking");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();
  DALI_TEST_CHECK( controller );

  // There is no text input enabled.
  DALI_TEST_CHECK( !controller->GetEnableCursorBlink() );

  // Enable the text input.
  // Creates a decorator.
  Text::DecoratorPtr decorator = Text::Decorator::New( *controller,
                                                       *controller );
  InputMethodContext inputMethodContext = InputMethodContext::New();
  // Enables the text input.
  controller->EnableTextInput( decorator, inputMethodContext );

  // Enables the cursor blink.
  controller->SetEnableCursorBlink( true );

  DALI_TEST_CHECK( controller->GetEnableCursorBlink() );

  // Disables the cursor blink.
  controller->SetEnableCursorBlink( false );

  DALI_TEST_CHECK( !controller->GetEnableCursorBlink() );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextControllerImfEvent(void)
{
  tet_infoline(" UtcDaliTextController");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  std::string text;
  InputMethodContext::EventData imfEvent;

  DALI_TEST_CHECK( controller );

  // Enable the text input.
  // Creates a decorator.
  Text::DecoratorPtr decorator = Text::Decorator::New( *controller,
                                                       *controller );

  InputMethodContext inputMethodContext = InputMethodContext::New();
  // Enables the text input.
  controller->EnableTextInput( decorator, inputMethodContext );

  // Set the placeholder text.
  controller->SetPlaceholderText( Controller::PLACEHOLDER_TYPE_INACTIVE, "Hello Dali" );

  // For coverage.
  imfEvent = InputMethodContext::EventData( InputMethodContext::GET_SURROUNDING, "", 0, 0 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );

  // Send VOID event.
  imfEvent = InputMethodContext::EventData( InputMethodContext::VOID, "", 0, 0 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );

  controller->GetText( text );
  DALI_TEST_CHECK( text.empty() );

  imfEvent = InputMethodContext::EventData( InputMethodContext::COMMIT, "Hello ", 0, 6 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );
  controller->GetNaturalSize();

  // Check 'Delete All' key which means the input panel send a big range
  imfEvent = InputMethodContext::EventData( InputMethodContext::DELETE_SURROUNDING, "", -100, 100 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );
  controller->GetNaturalSize();

  controller->GetText( text );
  DALI_TEST_EQUALS( "", text, TEST_LOCATION );

  // Send COMMIT event.
  imfEvent = InputMethodContext::EventData( InputMethodContext::COMMIT, "Hello ", 0, 6 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );

  // Force to update the model.
  controller->GetNaturalSize();

  controller->GetText( text );
  DALI_TEST_EQUALS( "Hello ", text, TEST_LOCATION );

  // Send PRE_EDIT event
  imfEvent = InputMethodContext::EventData( InputMethodContext::PRE_EDIT, "w", 6, 1 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );

  // Force to update the model.
  controller->GetNaturalSize();

  controller->GetText( text );
  DALI_TEST_EQUALS( "Hello w", text, TEST_LOCATION );

  // Send DELETE_SURROUNDING event
  imfEvent = InputMethodContext::EventData( InputMethodContext::DELETE_SURROUNDING, "", -1, 1 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );

  // Force to update the model.
  controller->GetNaturalSize();

  controller->GetText( text );
  DALI_TEST_EQUALS( "Hello ", text, TEST_LOCATION );

  // for coverage
  inputMethodContext.SetPreeditStyle( InputMethodContext::PreeditStyle::UNDERLINE );

  // Send PRE_EDIT event
  imfEvent = InputMethodContext::EventData( InputMethodContext::PRE_EDIT, "wo", 6, 2 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );

  // Force to update the model.
  controller->GetNaturalSize();

  controller->GetText( text );
  DALI_TEST_EQUALS( "Hello wo", text, TEST_LOCATION );

  // Send GET_SURROUNDING event
  imfEvent = InputMethodContext::EventData( InputMethodContext::GET_SURROUNDING, "", 0, 0 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );

  controller->GetText( text );
  DALI_TEST_EQUALS( "Hello wo", text, TEST_LOCATION );

  // Send PRIVATE_COMMAND event
  imfEvent = InputMethodContext::EventData( InputMethodContext::PRIVATE_COMMAND, "", 0, 0 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );

  controller->GetText( text );
  DALI_TEST_EQUALS( "Hello wo", text, TEST_LOCATION );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextControllerImfPreeditStyle(void)
{
  tet_infoline(" UtcDaliTextControllerImfPreeditStyle");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  std::string text;
  InputMethodContext::EventData imfEvent;

  DALI_TEST_CHECK( controller );

  // Configures the text controller similarly to the text-field.
  ConfigureTextField( controller );

  InputMethodContext inputMethodContext = InputMethodContext::New();

  // Send COMMIT event.
  imfEvent = InputMethodContext::EventData( InputMethodContext::COMMIT, "Hello ", 0, 6 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );

  // Force to update the model.
  controller->GetNaturalSize();

  controller->GetText( text );
  DALI_TEST_EQUALS( "Hello ", text, TEST_LOCATION );

  // Send PRE_EDIT event
  imfEvent = InputMethodContext::EventData( InputMethodContext::PRE_EDIT, "w", 6, 1 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );
  inputMethodContext.SetPreeditStyle( InputMethodContext::PreeditStyle::NONE );
  controller->GetNaturalSize();

  controller->GetText( text );
  DALI_TEST_EQUALS( "Hello w", text, TEST_LOCATION );

  imfEvent = InputMethodContext::EventData( InputMethodContext::PRE_EDIT, "w", 6, 1 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );

  // Set the preedit style as REVERSE
  inputMethodContext.SetPreeditStyle( InputMethodContext::PreeditStyle::REVERSE );
  controller->GetNaturalSize();

  controller->GetText( text );
  DALI_TEST_EQUALS( "Hello w", text, TEST_LOCATION );

  imfEvent = InputMethodContext::EventData( InputMethodContext::PRE_EDIT, "w", 6, 1 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );

  // Set the preedit style as HIGHLIGHT
  inputMethodContext.SetPreeditStyle( InputMethodContext::PreeditStyle::HIGHLIGHT );
  controller->GetNaturalSize();

  controller->GetText( text );
  DALI_TEST_EQUALS( "Hello w", text, TEST_LOCATION );

  imfEvent = InputMethodContext::EventData( InputMethodContext::PRE_EDIT, "w", 6, 1 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );

  // Set the preedit style as CUSTOM_PLATFORM_STYLE_1
  inputMethodContext.SetPreeditStyle( InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_1 );
  controller->GetNaturalSize();

  controller->GetText( text );
  DALI_TEST_EQUALS( "Hello w", text, TEST_LOCATION );

  imfEvent = InputMethodContext::EventData( InputMethodContext::PRE_EDIT, "w", 6, 1 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );

  // Set the preedit style as CUSTOM_PLATFORM_STYLE_2
  inputMethodContext.SetPreeditStyle( InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_2 );
  controller->GetNaturalSize();

  controller->GetText( text );
  DALI_TEST_EQUALS( "Hello w", text, TEST_LOCATION );

  imfEvent = InputMethodContext::EventData( InputMethodContext::PRE_EDIT, "w", 6, 1 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );

  // Set the preedit style as CUSTOM_PLATFORM_STYLE_3
  inputMethodContext.SetPreeditStyle( InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_3 );
  controller->GetNaturalSize();

  controller->GetText( text );
  DALI_TEST_EQUALS( "Hello w", text, TEST_LOCATION );

  imfEvent = InputMethodContext::EventData( InputMethodContext::PRE_EDIT, "w", 6, 1 );
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );

  // Set the preedit style as CUSTOM_PLATFORM_STYLE_4
  inputMethodContext.SetPreeditStyle( InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_4 );
  controller->GetNaturalSize();

  controller->GetText( text );
  DALI_TEST_EQUALS( "Hello w", text, TEST_LOCATION );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextControllerTextPopupButtonTouched(void)
{
  tet_infoline(" UtcDaliTextControllerTextPopupButtonTouched");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  DALI_TEST_CHECK( controller );

  std::string text;
  PushButton button;
  Property::Map attributes;

  // Enable the text input.
  // Creates a decorator.
  Text::DecoratorPtr decorator = Text::Decorator::New( *controller,
                                                       *controller );

  InputMethodContext inputMethodContext = InputMethodContext::New();
  // Enables the text input.
  controller->EnableTextInput( decorator, inputMethodContext );

  // Creates the text's popup.
  TextSelectionPopupCallbackInterface& callbackInterface = *controller;
  TextSelectionPopup textPopup = TextSelectionPopup::New( &callbackInterface );

  Toolkit::TextSelectionPopup::Buttons buttonsToEnable = static_cast<Toolkit::TextSelectionPopup::Buttons>( TextSelectionPopup::CUT        |
                                                                                                            TextSelectionPopup::COPY       |
                                                                                                            TextSelectionPopup::PASTE      |
                                                                                                            TextSelectionPopup::SELECT     |
                                                                                                            TextSelectionPopup::SELECT_ALL |
                                                                                                            TextSelectionPopup::CLIPBOARD );

  textPopup.EnableButtons( buttonsToEnable );
  Stage::GetCurrent().Add( textPopup );
  textPopup.ShowPopup();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Sets some text.
  controller->SetText( "Hello world" );

  // Select the whole text.
  button = PushButton::DownCast( textPopup.FindChildByName( OPTION_SELECT_ALL ) );
  DALI_TEST_CHECK( button );

  button.DoAction( "buttonClick", attributes );

  // Call relayout to process the input events.
  controller->Relayout( CONTROL_SIZE );

  // Cut the text.
  button = PushButton::DownCast( textPopup.FindChildByName( OPTION_CUT ) );
  DALI_TEST_CHECK( button );

  button.DoAction( "buttonClick", attributes );

  // Force to update the model.
  controller->GetNaturalSize();

  controller->GetText( text );
  DALI_TEST_CHECK( text.empty() );

  // Set text again.
  controller->SetText( "Hello world" );

  // Select the whole text.
  button = PushButton::DownCast( textPopup.FindChildByName( OPTION_SELECT_ALL ) );
  DALI_TEST_CHECK( button );

  button.DoAction( "buttonClick", attributes );

  // Call relayout to process the input events.
  controller->Relayout( CONTROL_SIZE );

  // Copy to the clipboard.
  button = PushButton::DownCast( textPopup.FindChildByName( OPTION_COPY ) );
  DALI_TEST_CHECK( button );

  button.DoAction( "buttonClick", attributes );

  // Call relayout to process the input events.
  controller->Relayout( CONTROL_SIZE );

  // Cut the text.
  button = PushButton::DownCast( textPopup.FindChildByName( OPTION_CUT ) );
  DALI_TEST_CHECK( button );

  button.DoAction( "buttonClick", attributes );

  // Force to update the model.
  controller->GetNaturalSize();

  controller->GetText( text );
  DALI_TEST_CHECK( text.empty() );

  ClipboardEventNotifier clipboardEventNotifier = ClipboardEventNotifier::Get();
  clipboardEventNotifier.ContentSelectedSignal().Connect( &ContentSelectedCallback );

  // Paste the text.
  button = PushButton::DownCast( textPopup.FindChildByName( OPTION_PASTE ) );
  DALI_TEST_CHECK( button );

  button.DoAction( "buttonClick", attributes );

  // Call relayout to process the input events.
  controller->Relayout( CONTROL_SIZE );

  DALI_TEST_EQUALS( "Hello world", gClipboardText, TEST_LOCATION );

  // Show the clipboard.
  button = PushButton::DownCast( textPopup.FindChildByName( OPTION_CLIPBOARD ) );
  DALI_TEST_CHECK( button );

  button.DoAction( "buttonClick", attributes );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextControllerGetInputShadowProperty(void)
{
  tet_infoline(" UtcDaliTextControllerGetInputShadowProperty");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  DALI_TEST_CHECK( controller );

  const std::string& shadowProperties = controller->GetInputShadowProperties();

  DALI_TEST_CHECK( shadowProperties.empty() );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextControllerGetInputUnderlineProperty(void)
{
  tet_infoline(" UtcDaliTextControllerGetInputUnderlineProperty");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  DALI_TEST_CHECK( controller );

  const std::string& underlineProperties = controller->GetInputUnderlineProperties();

  DALI_TEST_CHECK( underlineProperties.empty() );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextControllerSetGetAutoScrollEnabled(void)
{
  tet_infoline(" UtcDaliTextControllerSetGetAutoScrollEnabled");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  DALI_TEST_CHECK( controller );

  DALI_TEST_CHECK( !controller->IsAutoScrollEnabled() );

  // The auto scrolling shouldn't be enabled if the multi-line is enabled.

  // Enable multi-line.
  controller->SetMultiLineEnabled( true );

  // Enable text scrolling.
  controller->SetAutoScrollEnabled( true );

  DALI_TEST_CHECK( !controller->IsAutoScrollEnabled() );

  // Disable multi-line.
  controller->SetMultiLineEnabled( false );

  // Enable text scrolling.
  controller->SetAutoScrollEnabled( true );

  // Should be ebabled now.
  DALI_TEST_CHECK( controller->IsAutoScrollEnabled() );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextControllerSetGetCheckProperty(void)
{
  tet_infoline(" UtcDaliTextControllerSetGetCheckProperty");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  DALI_TEST_CHECK( controller );

  // Enable the text input.
  // Creates a decorator.
  Text::DecoratorPtr decorator = Text::Decorator::New( *controller, *controller );

  InputMethodContext inputMethodContext = InputMethodContext::New();
  // Enables the text input.
  controller->EnableTextInput( decorator, inputMethodContext );

  DALI_TEST_CHECK( !controller->IsInputModePassword() );

  // Set the text input to password.
  controller->SetInputModePassword( true );

  DALI_TEST_CHECK( controller->IsInputModePassword() );

  // Unset the text input to password.
  controller->SetInputModePassword( false );

  DALI_TEST_CHECK( !controller->IsInputModePassword() );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextControllerSetGetTapLongPressAction(void)
{
  tet_infoline(" UtcDaliTextControllerSetGetTapLongPressAction");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  DALI_TEST_CHECK( controller );

  // Test first with no decorator.

  DALI_TEST_EQUALS( Controller::NoTextTap::NO_ACTION, controller->GetNoTextDoubleTapAction(), TEST_LOCATION );
  controller->SetNoTextDoubleTapAction( Controller::NoTextTap::HIGHLIGHT );
  DALI_TEST_EQUALS( Controller::NoTextTap::NO_ACTION, controller->GetNoTextDoubleTapAction(), TEST_LOCATION );

  DALI_TEST_EQUALS( Controller::NoTextTap::NO_ACTION, controller->GetNoTextLongPressAction(), TEST_LOCATION );
  controller->SetNoTextLongPressAction( Controller::NoTextTap::HIGHLIGHT );
  DALI_TEST_EQUALS( Controller::NoTextTap::NO_ACTION, controller->GetNoTextLongPressAction(), TEST_LOCATION );

  // Add a decorator and re-test.

  // Creates a decorator.
  Text::DecoratorPtr decorator = Text::Decorator::New( *controller, *controller );

  InputMethodContext inputMethodContext = InputMethodContext::New();
  // Enables the text input.
  controller->EnableTextInput( decorator, inputMethodContext );

  DALI_TEST_EQUALS( Controller::NoTextTap::NO_ACTION, controller->GetNoTextDoubleTapAction(), TEST_LOCATION );
  controller->SetNoTextDoubleTapAction( Controller::NoTextTap::HIGHLIGHT );
  DALI_TEST_EQUALS( Controller::NoTextTap::HIGHLIGHT, controller->GetNoTextDoubleTapAction(), TEST_LOCATION );

  DALI_TEST_EQUALS( Controller::NoTextTap::SHOW_SELECTION_POPUP, controller->GetNoTextLongPressAction(), TEST_LOCATION ); // The default is SHOW_SELECTION_POPUP
  controller->SetNoTextLongPressAction( Controller::NoTextTap::HIGHLIGHT );
  DALI_TEST_EQUALS( Controller::NoTextTap::HIGHLIGHT, controller->GetNoTextLongPressAction(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextControllerSetGetLineSpacingProperty(void)
{
  tet_infoline(" UtcDaliTextControllerSetGetLineSpacingProperty");
  ToolkitTestApplication application;

  const Size size( Dali::Stage::GetCurrent().GetSize() );

  // single line text
  const std::string textSingle("A Quick Brown Fox Jumps Over The Lazy Dog");

  // multi-line text
  const std::string textMulti("A Quick Brown\nFox Jumps Over\nThe Lazy Dog");

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  ConfigureTextLabel(controller);

  // single line, line spacing = 0px
  {
    const float EXPECTED_SPACING = 0.0f;
    const Vector2 EXPECTED_LAYOUT_SIZE( 360.0f, 19.0f);
    const Vector3 EXPECTED_NATURAL_SIZE( 360.0f, 20.0f, 0.0f );

    controller->SetText(textSingle);
    controller->Relayout(size);
    controller->SetMultiLineEnabled( false );

    Vector3 naturalSize  = controller->GetNaturalSize();
    Vector2 layoutSize   = controller->GetTextModel()->GetLayoutSize();
    float lineSpacing0 = controller->GetDefaultLineSpacing();

    DALI_TEST_EQUALS( EXPECTED_SPACING, lineSpacing0, TEST_LOCATION );
    DALI_TEST_EQUALS( EXPECTED_LAYOUT_SIZE, layoutSize, TEST_LOCATION );
    DALI_TEST_EQUALS( EXPECTED_NATURAL_SIZE, naturalSize, TEST_LOCATION );
  }

  // single line, line spacing = 20px
  {
    const float EXPECTED_SPACING = 20.0f;
    const Vector2 EXPECTED_LAYOUT_SIZE( 360.0f, 19.0f );
    const Vector3 EXPECTED_NATURAL_SIZE( 360.0f, 40.0f, 0.0f );

    controller->SetText(textSingle);
    controller->Relayout(size);
    controller->SetDefaultLineSpacing( 20 );
    controller->SetMultiLineEnabled( false );

    Vector3 naturalSize  = controller->GetNaturalSize();
    Vector2 layoutSize   = controller->GetTextModel()->GetLayoutSize();
    float lineSpacing0 = controller->GetDefaultLineSpacing();

    DALI_TEST_EQUALS( EXPECTED_SPACING, lineSpacing0, TEST_LOCATION );
    DALI_TEST_EQUALS( EXPECTED_LAYOUT_SIZE, layoutSize, TEST_LOCATION );
    DALI_TEST_EQUALS( EXPECTED_NATURAL_SIZE, naturalSize, TEST_LOCATION );
  }

  // multi-line, line spacing = 0px
  {
    const float EXPECTED_SPACING = 0.0f;
    const Vector2 EXPECTED_LAYOUT_SIZE( 350.0f, 39.0f );
    const Vector3 EXPECTED_NATURAL_SIZE( 124.0f, 58.0f, 0.0f );

    controller->SetText(textMulti);
    controller->Relayout(size);
    controller->SetMultiLineEnabled( true );
    controller->SetDefaultLineSpacing( 0 );

    Vector3 naturalSize  = controller->GetNaturalSize();
    Vector2 layoutSize   = controller->GetTextModel()->GetLayoutSize();
    float lineSpacing0 = controller->GetDefaultLineSpacing();

    DALI_TEST_EQUALS( EXPECTED_SPACING, lineSpacing0, TEST_LOCATION );
    DALI_TEST_EQUALS( EXPECTED_LAYOUT_SIZE, layoutSize, TEST_LOCATION );
    DALI_TEST_EQUALS( EXPECTED_NATURAL_SIZE, naturalSize, TEST_LOCATION );
  }

  // multi-line, line spacing = 20px
  {
    const float EXPECTED_SPACING = 20.0f;
    const Vector2 EXPECTED_LAYOUT_SIZE( 124.0f, 57.0f );
    const Vector3 EXPECTED_NATURAL_SIZE( 124.0f, 118.0f, 0.0f );

    controller->SetText(textMulti);
    controller->Relayout(size);
    controller->SetMultiLineEnabled( true );
    controller->SetDefaultLineSpacing( 20 );

    Vector3 naturalSize  = controller->GetNaturalSize();
    Vector2 layoutSize   = controller->GetTextModel()->GetLayoutSize();
    float lineSpacing0 = controller->GetDefaultLineSpacing();

    DALI_TEST_EQUALS( EXPECTED_SPACING, lineSpacing0, TEST_LOCATION );
    DALI_TEST_EQUALS( EXPECTED_LAYOUT_SIZE, layoutSize, TEST_LOCATION );
    DALI_TEST_EQUALS( EXPECTED_NATURAL_SIZE, naturalSize, TEST_LOCATION );
  }

  // multi-line, line spacing = 30px
  {
    const float EXPECTED_SPACING = 30.0f;
    const Vector2 EXPECTED_LAYOUT_SIZE( 124.0f, 117.0f );
    const Vector3 EXPECTED_NATURAL_SIZE( 124.0f, 148.0f, 0.0f );

    controller->SetText(textMulti);
    controller->Relayout(size);
    controller->SetMultiLineEnabled( true );
    controller->SetDefaultLineSpacing( 30 );

    Vector3 naturalSize  = controller->GetNaturalSize();
    Vector2 layoutSize   = controller->GetTextModel()->GetLayoutSize();
    float lineSpacing0 = controller->GetDefaultLineSpacing();

    DALI_TEST_EQUALS( EXPECTED_SPACING, lineSpacing0, TEST_LOCATION );
    DALI_TEST_EQUALS( EXPECTED_LAYOUT_SIZE, layoutSize, TEST_LOCATION );
    DALI_TEST_EQUALS( EXPECTED_NATURAL_SIZE, naturalSize, TEST_LOCATION );
  }


  END_TEST;

}

int UtcDaliTextControllerCheckBufferIndices(void)
{
  tet_infoline(" UtcDaliTextControllerCheckBufferIndices");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  ConfigureTextLabel(controller);

  // Set the text
  const std::string text("A Quick Brown Fox Jumps Over The Lazy Dog");
  controller->SetText(text);

  // Get the implementation of the text controller
  Controller::Impl& mImpl = Controller::Impl::GetImplementation( *controller.Get() );

  // Tweak some parameters to make the indices to access the text buffer invalid
  mImpl.mTextUpdateInfo.mNumberOfCharactersToAdd = mImpl.mModel->mLogicalModel->mText.Count() * 10u;
  mImpl.mTextUpdateInfo.mNumberOfCharactersToRemove = 0u;
  mImpl.mTextUpdateInfo.mPreviousNumberOfCharacters = 0u;
  mImpl.mOperationsPending = Controller::ALL_OPERATIONS;

  // Perform a relayout
  const Size size( Dali::Stage::GetCurrent().GetSize() );
  controller->Relayout(size);

  tet_result(TET_PASS);

  END_TEST;
}

int UtcDaliTextControllerCheckInputColorChanged(void)
{
  tet_infoline(" UtcDaliTextControllerCheckInputColorChanged");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  ConfigureTextLabel(controller);

  // Enable the text input.
  // Creates a decorator.
  Text::DecoratorPtr decorator = Text::Decorator::New( *controller,
                                                       *controller );
  InputMethodContext inputMethodContext = InputMethodContext::New();
  // Enables the text input.
  controller->EnableTextInput( decorator, inputMethodContext );

  // Set the text
  const std::string text("Hello World!");
  controller->SetText(text);

  const Vector4 inputColor( 0.0f, 0.0f, 0.0f, 1.0f );
  controller->SetInputColor( inputColor );

  // Get the implementation of the text controller
  Controller::Impl& mImpl = Controller::Impl::GetImplementation( *controller.Get() );

  //  Reset operation
  mImpl.mOperationsPending = Controller::NO_OPERATION;

  // simulate a key event.
  controller->KeyEvent( GenerateKey( "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Dali::KeyEvent::Down ) );

  // change the input color
  const Vector4 newInputColor( 1.0f, 0.0f, 0.0f, 1.0f );
  controller->SetInputColor( newInputColor );

  // Check if relayout is requested or not when event state is INACTIVE.
  DALI_TEST_EQUALS( EventData::INACTIVE, mImpl.mEventData->mState, TEST_LOCATION );
  DALI_TEST_EQUALS( Controller::COLOR, static_cast<Controller::OperationsMask>( mImpl.mOperationsPending & Controller::COLOR ), TEST_LOCATION );

  // Perform a relayout
  const Size size( Dali::Stage::GetCurrent().GetSize() );
  controller->Relayout(size);

  tet_result(TET_PASS);

  END_TEST;
}

int UtcDaliTextControllerCheckInputFontFamilyChanged(void)
{
  tet_infoline(" UtcDaliTextControllerCheckInputFontFamilyChanged");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  ConfigureTextLabel(controller);

  // Enable the text input.
  // Creates a decorator.
  Text::DecoratorPtr decorator = Text::Decorator::New( *controller,
                                                       *controller );
  InputMethodContext inputMethodContext = InputMethodContext::New();
  // Enables the text input.
  controller->EnableTextInput( decorator, inputMethodContext );

  // Set the text and font family
  const std::string text("Hello World!");
  controller->SetText(text);
  controller->SetInputFontFamily("SamsungOneUI_200");

  // Get the implementation of the text controller
  Controller::Impl& mImpl = Controller::Impl::GetImplementation( *controller.Get() );

  //  Reset operation
  mImpl.mOperationsPending = Controller::NO_OPERATION;

  // simulate a key event.
  controller->KeyEvent( GenerateKey( "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Dali::KeyEvent::Down ) );

  // change the input font family
  controller->SetInputFontFamily("SamsungOneUI_300");

  // Check if relayout is requested or not when event state is INACTIVE.
  DALI_TEST_EQUALS( EventData::INACTIVE, mImpl.mEventData->mState, TEST_LOCATION );
  DALI_TEST_EQUALS( Controller::VALIDATE_FONTS, static_cast<Controller::OperationsMask>( mImpl.mOperationsPending & Controller::VALIDATE_FONTS ),
                    TEST_LOCATION );

  // Perform a relayout
  const Size size( Dali::Stage::GetCurrent().GetSize() );
  controller->Relayout(size);

  tet_result(TET_PASS);

  END_TEST;
}

int UtcDaliTextControllerCheckInputFontWeightChanged(void)
{
  tet_infoline(" UtcDaliTextControllerCheckInputFontWeightChanged");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  ConfigureTextLabel(controller);

  // Enable the text input.
  // Creates a decorator.
  Text::DecoratorPtr decorator = Text::Decorator::New( *controller,
                                                       *controller );
  InputMethodContext inputMethodContext = InputMethodContext::New();
  // Enables the text input.
  controller->EnableTextInput( decorator, inputMethodContext );

  // Set the text
  const std::string text("Hello World!");
  controller->SetText(text);
  controller->SetInputFontWeight( TextAbstraction::FontWeight::NORMAL );

  // Get the implementation of the text controller
  Controller::Impl& mImpl = Controller::Impl::GetImplementation( *controller.Get() );

  // Reset operation
  mImpl.mOperationsPending = Controller::NO_OPERATION;

  // simulate a key event.
  controller->KeyEvent( GenerateKey( "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Dali::KeyEvent::Down ) );

  // change the input font weight
  controller->SetInputFontWeight( TextAbstraction::FontWeight::BOLD );

  // Check if relayout is requested or not when event state is INACTIVE.
  DALI_TEST_EQUALS( EventData::INACTIVE, mImpl.mEventData->mState, TEST_LOCATION );
  DALI_TEST_EQUALS( Controller::VALIDATE_FONTS, static_cast<Controller::OperationsMask>( mImpl.mOperationsPending & Controller::VALIDATE_FONTS ),
                    TEST_LOCATION );

  // Perform a relayout
  const Size size( Dali::Stage::GetCurrent().GetSize() );
  controller->Relayout(size);

  tet_result(TET_PASS);

  END_TEST;
}

int UtcDaliTextControllerCheckInputFontWidthChanged(void)
{
  tet_infoline(" UtcDaliTextControllerCheckInputFontWidthChanged");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  ConfigureTextLabel(controller);

  // Enable the text input.
  // Creates a decorator.
  Text::DecoratorPtr decorator = Text::Decorator::New( *controller,
                                                       *controller );
  InputMethodContext inputMethodContext = InputMethodContext::New();
  // Enables the text input.
  controller->EnableTextInput( decorator, inputMethodContext );

  // Set the text
  const std::string text("Hello World!");
  controller->SetText(text);
  controller->SetInputFontWidth( TextAbstraction::FontWidth::NORMAL );

  // Get the implementation of the text controller
  Controller::Impl& mImpl = Controller::Impl::GetImplementation( *controller.Get() );

  // Reset operation
  mImpl.mOperationsPending = Controller::NO_OPERATION;

  // simulate a key event.
  controller->KeyEvent( GenerateKey( "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Dali::KeyEvent::Down ) );

  // change the input font width
  controller->SetInputFontWidth( TextAbstraction::FontWidth::EXPANDED );

  // Check if relayout is requested or not when event state is INACTIVE.
  DALI_TEST_EQUALS( EventData::INACTIVE, mImpl.mEventData->mState, TEST_LOCATION );
  DALI_TEST_EQUALS( Controller::VALIDATE_FONTS, static_cast<Controller::OperationsMask>( mImpl.mOperationsPending & Controller::VALIDATE_FONTS ),
                    TEST_LOCATION );

  // Perform a relayout
  const Size size( Dali::Stage::GetCurrent().GetSize() );
  controller->Relayout(size);

  tet_result(TET_PASS);

  END_TEST;
}

int UtcDaliTextControllerCheckInputFontSlantChanged(void)
{
  tet_infoline(" UtcDaliTextControllerCheckInputFontSlantChanged");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  ConfigureTextLabel(controller);

  // Enable the text input.
  // Creates a decorator.
  Text::DecoratorPtr decorator = Text::Decorator::New( *controller,
                                                       *controller );
  InputMethodContext inputMethodContext = InputMethodContext::New();
  // Enables the text input.
  controller->EnableTextInput( decorator, inputMethodContext );

  // Set the text
  const std::string text("Hello World!");
  controller->SetText(text);
  controller->SetInputFontSlant( TextAbstraction::FontSlant::NORMAL );

  // Get the implementation of the text controller
  Controller::Impl& mImpl = Controller::Impl::GetImplementation( *controller.Get() );

  //  Reset operation
  mImpl.mOperationsPending = Controller::NO_OPERATION;

  // simulate a key event.
  controller->KeyEvent( GenerateKey( "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Dali::KeyEvent::Down ) );

  // change the input font slant
  controller->SetInputFontSlant( TextAbstraction::FontSlant::ROMAN );

  // Check if relayout is requested or not when event state is INACTIVE.
  DALI_TEST_EQUALS( EventData::INACTIVE, mImpl.mEventData->mState, TEST_LOCATION );
  DALI_TEST_EQUALS( Controller::VALIDATE_FONTS, static_cast<Controller::OperationsMask>( mImpl.mOperationsPending & Controller::VALIDATE_FONTS ),
                    TEST_LOCATION );

  // Perform a relayout
  const Size size( Dali::Stage::GetCurrent().GetSize() );
  controller->Relayout(size);

  tet_result(TET_PASS);

  END_TEST;
}

int UtcDaliTextControllerCheckInputFontPointSizeChanged(void)
{
  tet_infoline(" UtcDaliTextControllerCheckInputFontPointSizeChanged");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  ConfigureTextLabel(controller);

  // Enable the text input.
  // Creates a decorator.
  Text::DecoratorPtr decorator = Text::Decorator::New( *controller,
                                                       *controller );
  InputMethodContext inputMethodContext = InputMethodContext::New();
  // Enables the text input.
  controller->EnableTextInput( decorator, inputMethodContext );

  // Set the text
  const std::string text("Hello World!");
  controller->SetText(text);
  controller->SetInputFontPointSize( 1.0f );

  // Get the implementation of the text controller
  Controller::Impl& mImpl = Controller::Impl::GetImplementation( *controller.Get() );

  //  Reset operation
  mImpl.mOperationsPending = Controller::NO_OPERATION;

  // simulate a key event.
  controller->KeyEvent( GenerateKey( "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Dali::KeyEvent::Down ) );

  // change the input font point size
  controller->SetInputFontPointSize( 1.2f );

  // Check if relayout is requested or not when event state is INACTIVE.
  DALI_TEST_EQUALS( EventData::INACTIVE, mImpl.mEventData->mState, TEST_LOCATION );
  DALI_TEST_EQUALS( Controller::VALIDATE_FONTS, static_cast<Controller::OperationsMask>( mImpl.mOperationsPending & Controller::VALIDATE_FONTS ),
                    TEST_LOCATION );

  // Perform a relayout
  const Size size( Dali::Stage::GetCurrent().GetSize() );
  controller->Relayout(size);

  tet_result(TET_PASS);

  END_TEST;
}

int UtcDaliTextControllerSelectEvent(void)
{
  tet_infoline(" UtcDaliTextControllerSelectEvent");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Configures the text controller similarly to the text-field.
  ConfigureTextField( controller );

  // Set the text
  const std::string text("Hello World!");
  controller->SetText( text );

  // Select the whole text.
  controller->SelectEvent( 0.f, 0.f, SelectionType::INTERACTIVE );

  // Perform a relayout
  const Size size( Dali::Stage::GetCurrent().GetSize() );
  controller->Relayout(size);

  // Get the implementation of the text controller
  Controller::Impl& mImpl = Controller::Impl::GetImplementation( *controller.Get() );

  // Check if the whole text is selected or not.
  std::string retrieved_text;
  mImpl.RetrieveSelection( retrieved_text, false );
  DALI_TEST_EQUALS( "Hello", retrieved_text, TEST_LOCATION );

  // Select the whole text.
  controller->SelectEvent( 0.f, 0.f, SelectionType::ALL );

  // Perform a relayout
  controller->Relayout( size );

  mImpl.RetrieveSelection( retrieved_text, false );
  DALI_TEST_EQUALS( text, retrieved_text, TEST_LOCATION );

  END_TEST;
}


int UtcDaliTextControllerRemoveTextChangeEventData(void)
{
  tet_infoline(" UtcDaliTextControllerRemoveTextChangeEventData");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  ConfigureTextField( controller );

  // Set the text
  const std::string text( "Hello World!" );
  controller->SetText( text );
  controller->SetInputFontPointSize( 1.0f );

  // Get the implementation of the text controller
  Controller::Impl& mImpl = Controller::Impl::GetImplementation( *controller.Get() );

  DALI_TEST_EQUALS( EventData::INACTIVE, mImpl.mEventData->mState, TEST_LOCATION );

  // Send DELETE_SURROUNDING event
  InputMethodContext::EventData imfEvent = InputMethodContext::EventData( InputMethodContext::DELETE_SURROUNDING, "", -1, 1 );
  InputMethodContext inputMethodContext = InputMethodContext::New();
  controller->OnInputMethodContextEvent( inputMethodContext, imfEvent );

  // Force to update the model.
  controller->GetNaturalSize();

  // Simulate a key event to delete text
  controller->KeyEvent( GenerateKey( "", "", DALI_KEY_BACKSPACE, 0, 0, Dali::KeyEvent::Down ) );

  DALI_TEST_EQUALS( EventData::EDITING, mImpl.mEventData->mState, TEST_LOCATION );

  // Perform a relayout
  const Size size( Dali::Stage::GetCurrent().GetSize() );
  controller->Relayout( size );

  tet_result(TET_PASS);

  END_TEST;
}

int UtcDaliTextControllerCheckInputFontPointSizeUpdated(void)
{
  tet_infoline(" UtcDaliTextControllerCheckInputFontPointSizeUpdated");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  ConfigureTextField(controller);

  // Set the text
  const std::string text("Hello World!");
  controller->SetText( text );
  controller->SetInputFontPointSize( 1.0f );
  controller->KeyboardFocusGainEvent();

  application.SendNotification();
  application.Render();

  // Perform a relayout
  const Size size( Dali::Stage::GetCurrent().GetSize() );
  controller->Relayout(size);

  // simulate a key event.
  controller->KeyEvent( GenerateKey( "a", "a", 38, 0, 0, Dali::KeyEvent::Down ) );

  // change the input font point size
  controller->SetInputFontPointSize( 20.f );

  application.SendNotification();
  application.Render();

  // Perform a relayout
  controller->Relayout(size);

  tet_result(TET_PASS);

  END_TEST;
}
