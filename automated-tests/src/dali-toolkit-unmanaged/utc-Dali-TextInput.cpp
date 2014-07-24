/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/integration-api/events/key-event-integ.h>

using namespace Dali;
using namespace Toolkit;


int UtcDaliTextInputSetActiveStyle(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing Setting of Style to newly added text");

  TextInput textInput = TextInput::New();  // create empty TextInput

  Stage::GetCurrent().Add(textInput);

  const std::string styledString = "Test String<i>ab</i>" ;
  const std::string plainString = "Test String";
  textInput.SetInitialText( plainString );

  application.SendNotification();
  application.Render();

  textInput.SetEditable(true);

  std::string retreivedMarkupString = textInput.GetMarkupText();

  tet_infoline("Confirm markup text is a plain string ");
  DALI_TEST_EQUALS( plainString,textInput.GetText(), TEST_LOCATION);

  TextStyle style;
  style.SetItalics( true );

  tet_infoline("Apply style to TextInput");
  textInput.SetActiveStyle( style );

  Integration::KeyEvent eventA("a", "a", 0, 0, 0, Integration::KeyEvent::Down );
  Integration::KeyEvent eventB("b", "b", 0, 0, 0, Integration::KeyEvent::Down );

  application.SendNotification();
  application.Render();

  application.ProcessEvent(eventA);
  application.SendNotification();
  application.Render();

  application.ProcessEvent(eventB);
  application.SendNotification();
  application.Render();

  retreivedMarkupString = textInput.GetMarkupText();

  DALI_TEST_EQUALS( styledString, retreivedMarkupString, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextInputApplyStyleToSelectedText(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing application of style to selected text ");

  TextInput textInput = TextInput::New();  // create empty TextInput

  Stage::GetCurrent().Add(textInput);

  const std::string styledString = "Test <i>String</i> to style";
  const std::string plainString = "Test String to style";
  textInput.SetInitialText( plainString );

  application.SendNotification();
  application.Render();

  textInput.SetEditable(true);

  std::string retreivedMarkupString = textInput.GetMarkupText();

  tet_infoline("Confirm markup text is a plain string ");
  DALI_TEST_EQUALS( plainString,textInput.GetText(), TEST_LOCATION);

  TextStyle style;
  style.SetItalics( true );

  textInput.SelectText( 5, 11 );

  tet_infoline("Apply style to selected text");
  textInput.ApplyStyle( style );

  application.Render();

  retreivedMarkupString = textInput.GetMarkupText();

  DALI_TEST_EQUALS( styledString, retreivedMarkupString, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextInputApplyStyleToAll(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing application of style to all text ");

  TextInput textInput = TextInput::New();  // create empty TextInput

  Stage::GetCurrent().Add(textInput);

  const std::string styledString = "<i>Test String to style</i>";
  const std::string plainString = "Test String to style";
  textInput.SetInitialText( plainString );

  application.SendNotification();
  application.Render();

  textInput.SetEditable(true);

  std::string retreivedMarkupString = textInput.GetMarkupText();

  tet_infoline("Confirm markup text is a plain string ");
  DALI_TEST_EQUALS( plainString,textInput.GetText(), TEST_LOCATION);

  TextStyle style;
  style.SetItalics( true );

  tet_infoline("Apply style to all text");
  textInput.ApplyStyleToAll( style );

  application.Render();

  retreivedMarkupString = textInput.GetMarkupText();

  DALI_TEST_EQUALS( styledString, retreivedMarkupString, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextInputGetStyleAtCursor(void)
{
  ToolkitTestApplication application;

  tet_infoline("Test getting style at cursor");

  TextInput textInput = TextInput::New();  // create empty TextInput

  Stage::GetCurrent().Add(textInput);

  const std::string styledString = "Test Stringa<i>b</i>" ;
  const std::string plainString = "Test String";
  textInput.SetInitialText( plainString );

  application.SendNotification();
  application.Render();

  textInput.SetEditable(true);

  tet_infoline("Confirm style at cursor is default(plain)");
  TextStyle style;
  Integration::KeyEvent eventA("a", "a", 0, 0, 0, Integration::KeyEvent::Down );
  application.ProcessEvent(eventA);
  application.SendNotification();
  application.Render();

  TextStyle retreivedStyleAtCursor = textInput.GetStyleAtCursor();

  DALI_TEST_CHECK( style == retreivedStyleAtCursor );
  DALI_TEST_CHECK( !retreivedStyleAtCursor.GetItalics() );

  tet_infoline("Set style before adding new character");
  style.SetItalics( true );
  textInput.SetActiveStyle( style );

  Integration::KeyEvent eventB("b", "b", 0, 0, 0, Integration::KeyEvent::Down );
  application.ProcessEvent(eventB);
  application.SendNotification();
  application.Render();

  tet_infoline("Confirm style at cursor is correct style");
  retreivedStyleAtCursor = textInput.GetStyleAtCursor();

  DALI_TEST_CHECK( retreivedStyleAtCursor.GetItalics() );

  tet_infoline("Confirm style at cursor is not a style that was not set");
  DALI_TEST_CHECK( !retreivedStyleAtCursor.GetUnderline() );

  tet_infoline("Confirm markup text is correct");
  DALI_TEST_EQUALS( styledString, textInput.GetMarkupText(), TEST_LOCATION);



  END_TEST;
}

int UtcDaliTextInputSetAndGetTextAlignment(void)
{
  ToolkitTestApplication application;

  TextInput textInput = TextInput::New();
  textInput.SetTextAlignment(static_cast<Alignment::Type>( Alignment::HorizontalCenter) );

  bool result = ( textInput.GetTextAlignment() & Alignment::HorizontalCenter ) ;

  DALI_TEST_CHECK( result );

  result = ( textInput.GetTextAlignment() & Alignment::HorizontalRight );

  DALI_TEST_CHECK( !result );
  END_TEST;
}

int UtcDaliTextInputSetAndGetMultilinePolicy(void)
{
  ToolkitTestApplication application;

  const TextView::MultilinePolicy MULTILINE_POLICIES[] = { TextView::SplitByNewLineChar, TextView::SplitByWord, TextView::SplitByChar };
  const unsigned int NUM_MULTILINE_POLICIES = sizeof( MULTILINE_POLICIES ) / sizeof( unsigned int );

  TextInput textInput = TextInput::New();
  Stage::GetCurrent().Add(textInput);
  textInput.SetInitialText( "Hello world!" );

  for( unsigned int epIndex = 0; epIndex < NUM_MULTILINE_POLICIES; ++epIndex )
  {
    textInput.SetMultilinePolicy( MULTILINE_POLICIES[epIndex] );

    DALI_TEST_EQUALS( textInput.GetMultilinePolicy(), MULTILINE_POLICIES[epIndex], TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTextInputSetAndGetExceedEnabled(void)
{
  ToolkitTestApplication application;

  const TextView::ExceedPolicy EXCEED_POLICIES[] = { TextView::Original, TextView::Fade, TextView::Split, TextView::ShrinkToFit };
  const unsigned int NUM_EXCEED_POLICIES = sizeof( EXCEED_POLICIES ) / sizeof( unsigned int );

  TextInput textInput = TextInput::New();
  Stage::GetCurrent().Add(textInput);
  textInput.SetInitialText( "Hello world!" );

  for( unsigned int epIndex = 0; epIndex < NUM_EXCEED_POLICIES; ++epIndex )
  {
    textInput.SetWidthExceedPolicy( EXCEED_POLICIES[epIndex] );

    DALI_TEST_EQUALS( textInput.GetWidthExceedPolicy(), EXCEED_POLICIES[epIndex], TEST_LOCATION );
  }
  END_TEST;
}
