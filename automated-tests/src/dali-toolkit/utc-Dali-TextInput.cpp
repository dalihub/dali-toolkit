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
#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/integration-api/events/key-event-integ.h>

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_text_input_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_text_input_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  Actor actor = Actor::DownCast(handle);

  if(actor)
  {
    TextInput handle = TextInput::DownCast(actor);
     if (handle)
     {
       gObjectCreatedCallBackCalled = true;
     }
  }
}

static bool gHasEndSignalBeenReceived;
static bool gHasStartSignalBeenReceived;

// Callback test function
void OnStartInput(TextInput textInput)
{
  gHasStartSignalBeenReceived = true;
}

// Callback test function
void OnEndInput(TextInput textInput)
{
  gHasEndSignalBeenReceived = true;
}

}

// Positive test case for a method
int UtcDaliTextInputConstruction(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing New constructor");

  TextInput textInput = TextInput::New();
  DALI_TEST_CHECK(textInput);

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect(&TestCallback);
  {
    TextInput textInput = TextInput::New();
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
  END_TEST;
}


static bool downCastToTextInput(Dali::Actor actor)
{
  TextInput handle = TextInput::DownCast(actor);
  if (handle)
  {
    tet_infoline("Downcasted to TextInput");
    return true;
  }
  else
  {
    tet_infoline("Did not downcast to TextInput");
    return false;
  }
}

// Positive test case for a method
int UtcDaliTextInputDownCast(void)
{
  ToolkitTestApplication application;

  TextInput textInput = TextInput::New();

  tet_infoline("Testing Downcasting with a TextInput");
  DALI_TEST_EQUALS(true,downCastToTextInput(textInput), TEST_LOCATION); // downcast a TextInput

  Dali::TextActor badHandle = Dali::TextActor::New("test");

  tet_infoline("Testing Downcasting with the wrong actor");
  DALI_TEST_EQUALS(false, downCastToTextInput(badHandle), TEST_LOCATION); // downcast a TextActor to TextInput
  END_TEST;
}

// Positive test case for a method
int UtcDaliTextInputGetText(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing GetText");

  const std::string teststring = "test";

  TextInput textInput = TextInput::New();  // create empty TextInput

  DALI_TEST_EQUALS("",textInput.GetText(), TEST_LOCATION); // Get text which should be empty

  textInput.SetInitialText(teststring);

  DALI_TEST_EQUALS(teststring,textInput.GetText(), TEST_LOCATION); // Get text which should be test string

  END_TEST;
}

int UtcDaliTextInputGetMarkupText(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing retrieval of Markup text after style set");

  const std::string markup = "<i>Text with italic style</i>" ;
  const std::string teststring = "Text with italic style";

  TextInput textInput = TextInput::New();

  tet_infoline("Set initial text");

  textInput.SetInitialText( teststring );

  tet_infoline("Check initial text");
  DALI_TEST_EQUALS( teststring,textInput.GetText(), TEST_LOCATION); // Get text which should be empty

  TextStyle style;
  style.SetItalics( true );

  tet_infoline("Apply style to TextInput");
  textInput.ApplyStyleToAll( style );

  tet_infoline("Retreive Markup Text");
  const std::string retreivedMarkupString = textInput.GetMarkupText();

  tet_infoline("Test Retreived text and Markup text match");
  DALI_TEST_EQUALS( retreivedMarkupString , retreivedMarkupString, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextInputSetMaxCharacterLength(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing Setting of max characters");

  const int maxChars = 4;
  const char* testChar  = "v";

  TextInput textInput = TextInput::New();  // create empty TextInput
  Stage::GetCurrent().Add(textInput);
  application.SendNotification();
  application.Render();

  textInput.SetMaxCharacterLength(maxChars);

  Integration::KeyEvent event(testChar, testChar, 0, 0, 0, Integration::KeyEvent::Down );

  std::string testString = "";

  tet_infoline("Starting editmode");
  textInput.SetEditable( true );

  tet_infoline("Sending Key Events");
  // Send max number of characters
  for (int i=0; i < maxChars; i++)
    {
      application.ProcessEvent(event);
      testString.append(testChar);
    }

  tet_printf( "Get text result : %s\n", textInput.GetText().c_str());

  DALI_TEST_EQUALS(testString, textInput.GetText(), TEST_LOCATION);

  tet_infoline("Sending Key Event which exceeds max characters");

  application.ProcessEvent(event); // try to append additional character

  DALI_TEST_EQUALS(testString,textInput.GetText(), TEST_LOCATION);

  tet_infoline("Increase max characters limit");

  textInput.SetMaxCharacterLength(maxChars+1); // increment max characters by 1

  tet_infoline("Send character again which should now fit");
  application.ProcessEvent(event); // append additional character
  testString.append(testChar);

  DALI_TEST_EQUALS(testString,textInput.GetText(), TEST_LOCATION);
  END_TEST;
}


int UtcDaliTextInputSetAndGetNumberOfLines(void)
{
  ToolkitTestApplication application;

  tet_infoline("Ensuring API for setting and getting max number of lines is correct");

  TextInput textInput = TextInput::New();  // create empty TextInput

  unsigned int numberOfLines = 1;

  textInput.SetNumberOfLinesLimit( numberOfLines );

  DALI_TEST_EQUALS(numberOfLines ,textInput.GetNumberOfLinesLimit(),  TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextInputGetNumberOfCharacters(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing Getting number of characters");

  const std::string initialString = "initial text";
  const std::string newInitialString = "initial text new";

  TextInput textInput = TextInput::New();  // create empty TextInput

  textInput.SetInitialText( initialString );

  tet_infoline("Testing TextInput contains correct number of characters ");

  DALI_TEST_EQUALS( initialString.size() , textInput.GetNumberOfCharacters(), TEST_LOCATION);

  tet_infoline("Testing TextInput contains correct number of characters second phase ");

  textInput.SetInitialText( newInitialString );

  DALI_TEST_EQUALS( newInitialString.size() , textInput.GetNumberOfCharacters(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextInputSetAndGetPlaceholderText(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing Setting of PlaceholderText");

  const std::string initialString = "initial text";
  const std::string placeholderString = "placeholder";

  TextInput textInput = TextInput::New();  // create empty TextInput

  tet_infoline("Testing TextInput is empty at creation ");

  DALI_TEST_EQUALS("",textInput.GetText(), TEST_LOCATION);

  tet_infoline("Set placeholder text");

  textInput.SetPlaceholderText( placeholderString );

  tet_infoline("Testing TextInput contains placeholder text");

  DALI_TEST_EQUALS( placeholderString , textInput.GetPlaceholderText(), TEST_LOCATION);

  tet_infoline("Set initial text which should replace placeholder text");

  textInput.SetInitialText( initialString );

  tet_infoline("Testing TextInput contains initial text when placeholder text set");

  DALI_TEST_EQUALS( initialString,textInput.GetText(), TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliTextInputSetInitialText(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing Setting of Initial Text");

  const std::string teststring = "test";

  TextInput textInput = TextInput::New();  // create empty TextInput

  tet_infoline("Testing TextInput is empty at creation ");

  DALI_TEST_EQUALS("",textInput.GetText(), TEST_LOCATION);

  tet_infoline("Set text to TextInput");

  textInput.SetInitialText(teststring);

  tet_infoline("Test TextInput contains set text");

  DALI_TEST_EQUALS(teststring,textInput.GetText(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextInputSetEditableAndIsEditable(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing SetEditable And IsEditable");

  const std::string initialString = "initial text";

  TextInput textInput = TextInput::New();  // create empty TextInput
  textInput.SetInitialText( initialString );

  Stage::GetCurrent().Add(textInput);
  application.SendNotification();
  application.Render();

  bool editableStateFalse ( false );
  bool editableStateTrue ( true );

  textInput.SetEditable ( editableStateFalse );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( editableStateFalse, textInput.IsEditable() , TEST_LOCATION);

  textInput.SetEditable ( editableStateTrue );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( editableStateTrue, textInput.IsEditable() , TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextInputSetEditOnTouch(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing SetEditOnTouch And IsEditOnTouch");

  TextInput textInput = TextInput::New();

  bool editableOnTouchOn ( true );
  bool editableOnTouchOff( false );

  tet_infoline("Testing SetEditOnTouch disabled");
  textInput.SetEditOnTouch ( editableOnTouchOff );
  DALI_TEST_EQUALS( editableOnTouchOff, textInput.IsEditOnTouch() , TEST_LOCATION);

  tet_infoline("Testing SetEditOnTouch enabled");
  textInput.SetEditOnTouch ( editableOnTouchOn );
  DALI_TEST_EQUALS( editableOnTouchOn, textInput.IsEditOnTouch() , TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextInputSetTextSelectable(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing SetTextSelectable and IsTextSelectable");

  const std::string initialString = "initial text";

  TextInput textInput = TextInput::New();
  textInput.SetInitialText( initialString );

  tet_infoline("Testing SetTextSelectable");
  textInput.SetTextSelectable();
  DALI_TEST_EQUALS( true, textInput.IsTextSelectable() , TEST_LOCATION);
  textInput.SetTextSelectable( false );
  DALI_TEST_EQUALS( false, textInput.IsTextSelectable() , TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextInputTextSelection(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing Text Selection");

  const std::string initialString = "initial text";

  TextInput textInput = TextInput::New();
  textInput.SetInitialText( initialString );

  Stage::GetCurrent().Add(textInput);
  application.SendNotification();
  application.Render();

  textInput.SetEditable( true );

  tet_infoline("Testing IsTextSelected negative");
  DALI_TEST_EQUALS( false, textInput.IsTextSelected(), TEST_LOCATION);

  textInput.SelectText(1,7);
  DALI_TEST_EQUALS( true, textInput.IsTextSelected(), TEST_LOCATION);

  textInput.DeSelectText();
  DALI_TEST_EQUALS( false, textInput.IsTextSelected(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextInputEnableGrabHandleAndIsGrabHandleEnabled(void)
{
  ToolkitTestApplication application;

  TextInput textInput = TextInput::New();

  bool grabHandleState = false;

  textInput.EnableGrabHandle( grabHandleState );

  DALI_TEST_EQUALS( grabHandleState, textInput.IsGrabHandleEnabled(), TEST_LOCATION);

  grabHandleState = true;
  textInput.EnableGrabHandle( grabHandleState );

  DALI_TEST_EQUALS( grabHandleState, textInput.IsGrabHandleEnabled(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextInputSetAndGetBoundingRectangle(void)
{
  ToolkitTestApplication application;

  TextInput textInput = TextInput::New();

  Stage::GetCurrent().Add(textInput);
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  const Rect<float> boundingRectangle( 100.0f, 100.0f, stageSize.width, stageSize.height );

  textInput.SetBoundingRectangle( boundingRectangle );

  const Rect<float> retreievedBoundingRectangle = textInput.GetBoundingRectangle();

  DALI_TEST_EQUALS( boundingRectangle.x, retreievedBoundingRectangle.x, TEST_LOCATION);
  DALI_TEST_EQUALS( boundingRectangle.y, retreievedBoundingRectangle.y, TEST_LOCATION);
  DALI_TEST_EQUALS( boundingRectangle.width, retreievedBoundingRectangle.width, TEST_LOCATION);
  DALI_TEST_EQUALS( boundingRectangle.height, retreievedBoundingRectangle.height, TEST_LOCATION);
  END_TEST;
}


int UtcDaliTextInputSetAndGetTextAlignment01(void)
{
  ToolkitTestApplication application;

  TextInput textInput = TextInput::New();
  Stage::GetCurrent().Add(textInput);
  application.SendNotification();
  application.Render();

  textInput.SetTextAlignment(static_cast<Alignment::Type>( Alignment::HorizontalCenter) );
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( static_cast<Alignment::Type>( Alignment::HorizontalCenter) & textInput.GetTextAlignment()) ;
  END_TEST;
}

int UtcDaliTextInputSetAndGetTextAlignment02(void)
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

int UtcDaliTextInputSetSortModifier(void)
{
  tet_infoline("Testing SetSortModifier does not cause TextInput failure");

  ToolkitTestApplication application;

  TextInput textInput = TextInput::New();

  const float offsetToUse = 1.5f;

  textInput.SetSortModifier( offsetToUse );

  DALI_TEST_CHECK( textInput );
  END_TEST;
}

int UtcDaliTextInputSetAndGetSnapshotModeEnabled(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing SetSnapshotModeEnabled and IsSnapshotModeEnabled");

  TextInput textInput = TextInput::New();  // create empty TextInput
  bool snapshotMode( true );
  textInput.SetSnapshotModeEnabled( snapshotMode );

  DALI_TEST_EQUALS( snapshotMode, textInput.IsSnapshotModeEnabled(), TEST_LOCATION);

  snapshotMode = false;
  textInput.SetSnapshotModeEnabled( snapshotMode );

  DALI_TEST_EQUALS( snapshotMode, textInput.IsSnapshotModeEnabled(), TEST_LOCATION);
  END_TEST;
}


int UtcDaliTextInputEndSignalEmit(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing Set editable false emits end signal");

  TextInput textInput = TextInput::New();  // create empty TextInput

  Stage::GetCurrent().Add(textInput);

  textInput.InputFinishedSignal().Connect( &OnEndInput );

  textInput.SetEditable(true) ;

  gHasEndSignalBeenReceived = false;

  textInput.SetEditable(false) ;

  DALI_TEST_EQUALS(true, gHasEndSignalBeenReceived, TEST_LOCATION);
  END_TEST;
}



int UtcDaliTextInputStartSignalEmit(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing SetEditable emits start signal");

  TextInput textInput = TextInput::New();  // create empty TextInput

  Stage::GetCurrent().Add(textInput);

  textInput.InputStartedSignal().Connect( &OnStartInput );

  gHasStartSignalBeenReceived = false;

  textInput.SetEditable(true);  // Set editable first time

  DALI_TEST_EQUALS(true, gHasStartSignalBeenReceived, TEST_LOCATION);

  gHasStartSignalBeenReceived = false;

  textInput.SetEditable(true); // Set editable second time, signal should not be sent again.

  DALI_TEST_EQUALS(false, gHasStartSignalBeenReceived, TEST_LOCATION);

  textInput.SetEditable(false);

  gHasStartSignalBeenReceived = false;

  textInput.SetEditable(true);  // Set editable again

  DALI_TEST_EQUALS(true, gHasStartSignalBeenReceived, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextInputExceedMaxCharacters(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing Max characters is obeyed when inputting key events ");

  TextInput textInput = TextInput::New();  // create empty TextInput

  Stage::GetCurrent().Add(textInput);
  textInput.SetMaxCharacterLength(4);
  textInput.SetInitialText("");
  textInput.SetEditable(true);

  application.SendNotification();
  application.Render();

  Integration::KeyEvent eventA("a", "a", 0, 0, 0, Integration::KeyEvent::Down );
  Integration::KeyEvent eventB("b", "b", 0, 0, 0, Integration::KeyEvent::Down );

  application.ProcessEvent(eventA);
  application.ProcessEvent(eventB);
  application.ProcessEvent(eventA);
  application.ProcessEvent(eventB);

  application.ProcessEvent(eventA);
  application.ProcessEvent(eventB);

  tet_printf( "Get text result : %s\n", textInput.GetText().c_str());

  DALI_TEST_EQUALS("abab",textInput.GetText(), TEST_LOCATION); // Get text which should be only 4 characters
  END_TEST;
}



int UtcDaliTextInputSetAndGetFadeBoundary(void)
{
  tet_infoline("UtcDaliTextViewSetAndGetFadeBoundary: ");

  ToolkitTestApplication application;

  TextView::FadeBoundary fadeBoundary( PixelSize( 0 ), PixelSize( 20 ), PixelSize( 0 ), PixelSize( 10 ) );

  TextInput textInput = TextInput::New();
  textInput.SetInitialText( "Hello world!" );

  Stage::GetCurrent().Add(textInput);
  application.SendNotification();
  application.Render();

  textInput.SetFadeBoundary( fadeBoundary );

  TextView::FadeBoundary fadeBoundary2 = textInput.GetFadeBoundary();

  DALI_TEST_EQUALS( fadeBoundary.mLeft, fadeBoundary2.mLeft, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeBoundary.mRight, fadeBoundary2.mRight, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeBoundary.mTop, fadeBoundary2.mTop, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeBoundary.mBottom, fadeBoundary2.mBottom, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTextInputSetAndGetWidthExceedPolicy(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextInputSetAndGetWidthExceedPolicy: ");

  const TextView::ExceedPolicy EXCEED_POLICIES[] = { TextView::Original, TextView::Fade, TextView::Split, TextView::ShrinkToFit };
  const unsigned int NUM_EXCEED_POLICIES = sizeof( EXCEED_POLICIES ) / sizeof( unsigned int );

  TextInput textInput = TextInput::New();
  textInput.SetInitialText( "Hello world!" );

  for( unsigned int epIndex = 0; epIndex < NUM_EXCEED_POLICIES; ++epIndex )
  {
    textInput.SetWidthExceedPolicy( EXCEED_POLICIES[epIndex] );

    DALI_TEST_EQUALS( textInput.GetWidthExceedPolicy(), EXCEED_POLICIES[epIndex], TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTextInputSetAndGetHeightExceedPolicy(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextInputSetAndGetHeightExceedPolicy: ");

  const TextView::ExceedPolicy EXCEED_POLICIES[] = { TextView::Original, TextView::Fade, TextView::ShrinkToFit };
  const unsigned int NUM_EXCEED_POLICIES = sizeof( EXCEED_POLICIES ) / sizeof( unsigned int );

  TextInput textInput = TextInput::New();
  textInput.SetInitialText( "Hello world!" );

  for( unsigned int epIndex = 0; epIndex < NUM_EXCEED_POLICIES; ++epIndex )
  {
    textInput.SetHeightExceedPolicy( EXCEED_POLICIES[epIndex] );

    DALI_TEST_EQUALS( textInput.GetHeightExceedPolicy(), EXCEED_POLICIES[epIndex], TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTextInputScroll(void)
{
  tet_infoline("UtcDaliTextInputScroll: ");
  ToolkitTestApplication application;

  // Avoids the frame buffer texture to throw an exception.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  TextInput view = TextInput::New();
  view.SetMultilinePolicy( TextView::SplitByNewLineChar );
  view.SetWidthExceedPolicy( TextView::Original );
  view.SetHeightExceedPolicy( TextView::Original );
  view.SetTextAlignment( static_cast<Toolkit::Alignment::Type>( Toolkit::Alignment::HorizontalCenter | Toolkit::Alignment::VerticalCenter ) );
  view.SetInitialText( "Hello world! This is a scroll test." );
  view.SetSize( 100.f, 100.f );
  view.SetSnapshotModeEnabled( false );

  Stage::GetCurrent().Add( view );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( !view.IsScrollEnabled() ); // Scroll should be disabled by default.

  view.SetScrollEnabled( true );

  DALI_TEST_CHECK( view.IsScrollEnabled() );
  DALI_TEST_CHECK( view.IsSnapshotModeEnabled() ); // Scroll should enable snapshot mode.

  view.SetScrollPosition( Vector2( 400.f, 400.f ) );

  application.SendNotification();
  application.Render();

  const Vector2& scrollPosition = view.GetScrollPosition();
  DALI_TEST_EQUALS( scrollPosition, Vector2( 149.153656f, 0.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  END_TEST;
}

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
  DALI_TEST_CHECK( !retreivedStyleAtCursor.IsItalicsEnabled() );

  tet_infoline("Set style before adding new character");
  style.SetItalics( true );
  textInput.SetActiveStyle( style );

  Integration::KeyEvent eventB("b", "b", 0, 0, 0, Integration::KeyEvent::Down );
  application.ProcessEvent(eventB);
  application.SendNotification();
  application.Render();

  tet_infoline("Confirm style at cursor is correct style");
  retreivedStyleAtCursor = textInput.GetStyleAtCursor();

  DALI_TEST_CHECK( retreivedStyleAtCursor.IsItalicsEnabled() );

  tet_infoline("Confirm style at cursor is not a style that was not set");
  DALI_TEST_CHECK( !retreivedStyleAtCursor.IsUnderlineEnabled() );

  tet_infoline("Confirm markup text is correct");
  DALI_TEST_EQUALS( styledString, textInput.GetMarkupText(), TEST_LOCATION);



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
