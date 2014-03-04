//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>

#include <dali-toolkit/dali-toolkit.h>

#include <dali/integration-api/events/key-event-integ.h>


#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Toolkit;

static void Startup();
static void Cleanup();

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

} // namespace

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}
namespace
{
static bool gHasEndSignalBeenReceived;
static bool gHasStartSignalBeenReceived;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}


// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliTextInputConstruction, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextInputDownCast, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextInputGetText, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextInputSetMaxCharacterLength, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextInputSetInitialText, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextInputAddChars, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextInputRemoveChars, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextInputEndSignalEmit, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextInputStartSignalEmit, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextInputExceedMaxCharactersInitial, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextInputExceedMaxCharacters, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextInputSetNumberOfLines, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextInputSetAndGetFadeBoundary, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextInputSetAndGetWidthExceedPolicy, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextInputSetAndGetHeightExceedPolicy, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextInputScroll, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}


// Positive test case for a method
static void UtcDaliTextInputConstruction()
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
static void UtcDaliTextInputDownCast()
{
  ToolkitTestApplication application;

  TextInput textInput = TextInput::New();

  tet_infoline("Testing Downcasting with a TextInput");
  DALI_TEST_EQUALS(true,downCastToTextInput(textInput), TEST_LOCATION); // downcast a TextInput

  Dali::TextActor badHandle = Dali::TextActor::New("test");

  tet_infoline("Testing Downcasting with the wrong actor");
  DALI_TEST_EQUALS(false, downCastToTextInput(badHandle), TEST_LOCATION); // downcast a TextActor to TextInput
}

// Positive test case for a method
static void UtcDaliTextInputGetText()
{
  ToolkitTestApplication application;

  tet_infoline("Testing GetText");

  const std::string teststring = "test";

  TextInput textInput = TextInput::New();  // create empty TextInput

  DALI_TEST_EQUALS("",textInput.GetText(), TEST_LOCATION); // Get text which should be empty

  textInput.SetInitialText(teststring);

  DALI_TEST_EQUALS(teststring,textInput.GetText(), TEST_LOCATION); // Get text which should be test string

}

// Positive test case for a method
static void UtcDaliTextInputSetMaxCharacterLength()
{
  ToolkitTestApplication application;

  tet_infoline("Testing Setting of max characters");

  const int maxChars = 4;
  const char* testChar  = "v";

  TextInput textInput = TextInput::New();  // create empty TextInput

  Stage::GetCurrent().Add(textInput);

  textInput.SetKeyInputFocus();
  textInput.SetMaxCharacterLength(maxChars);

  Integration::KeyEvent event(testChar, testChar, 0, 0, 0, Integration::KeyEvent::Down );

  std::string testString = "";
  // Send max number of characters
  for (int i=0; i < maxChars; i++)
    {
      application.ProcessEvent(event);
      testString.append(testChar);
    }

  DALI_TEST_EQUALS(testString,textInput.GetText(), TEST_LOCATION); // Get text which should be empty

  application.ProcessEvent(event); // try to append additional character

  DALI_TEST_EQUALS(testString,textInput.GetText(), TEST_LOCATION); // Get text which should be empty

  textInput.SetMaxCharacterLength(maxChars+1); // increment max characters by 1

  application.ProcessEvent(event); // append additional character
  testString.append(testChar);

  DALI_TEST_EQUALS(testString,textInput.GetText(), TEST_LOCATION); // Get text which should be empty
}

// Positive test case for a method
static void UtcDaliTextInputSetInitialText()
{
  ToolkitTestApplication application;

  tet_infoline("Testing Setting of Initial Text");

  const std::string teststring = "test";

  TextInput textInput = TextInput::New();  // create empty TextInput

  textInput.SetInitialText(teststring);

  DALI_TEST_EQUALS(teststring,textInput.GetText(), TEST_LOCATION); // Get text which should be empty
}

static void UtcDaliTextInputAddChars()
{
  ToolkitTestApplication application;

  tet_infoline("Testing Adding characters");

  TextInput textInput = TextInput::New();  // create empty TextInput

  Stage::GetCurrent().Add(textInput);

  textInput.SetKeyInputFocus();

  Integration::KeyEvent event("a", "a", 0, 0, 0, Integration::KeyEvent::Down);
  application.ProcessEvent(event);

  DALI_TEST_EQUALS("a",textInput.GetText(), TEST_LOCATION); // Get text which should be "a"

  Integration::KeyEvent event2("v", "v", 0, 0, 0, Integration::KeyEvent::Down);
  application.ProcessEvent(event2);

  DALI_TEST_EQUALS("av",textInput.GetText(), TEST_LOCATION); // Get text which should be "av"
}

static void UtcDaliTextInputRemoveChars()
{
  ToolkitTestApplication application;

  tet_infoline("Testing Removal of end characters");

  TextInput textInput = TextInput::New();  // create empty TextInput

  Stage::GetCurrent().Add(textInput);

  textInput.SetKeyInputFocus();

  Integration::KeyEvent event("a", "a", 0, 0, 0, Integration::KeyEvent::Down);
  application.ProcessEvent(event);

  DALI_TEST_EQUALS("a",textInput.GetText(), TEST_LOCATION); // Get text which should be "a"

  Integration::KeyEvent event2("BackSpace", "", 0, 0, 0, Integration::KeyEvent::Down);
  application.ProcessEvent(event2);

  DALI_TEST_EQUALS("",textInput.GetText(), TEST_LOCATION); // Get text which should be ""

  application.ProcessEvent(event);
  application.ProcessEvent(event);

  DALI_TEST_EQUALS("aa",textInput.GetText(), TEST_LOCATION); // Get text which should be "aa"

  application.ProcessEvent(event2);

  DALI_TEST_EQUALS("a",textInput.GetText(), TEST_LOCATION); // Get text which should be "a"
}

// Callback test function
void OnEndInput(TextInput textInput)
{
  gHasEndSignalBeenReceived = true;
}

static void UtcDaliTextInputEndSignalEmit()
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
}


// Callback test function
void OnStartInput(TextInput textInput)
{
  gHasStartSignalBeenReceived = true;
}

static void UtcDaliTextInputStartSignalEmit()
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

  textInput.SetEditable(true,Vector2(3.f,2.f));  // Set editable again

  DALI_TEST_EQUALS(true, gHasStartSignalBeenReceived, TEST_LOCATION);
}

static void UtcDaliTextInputExceedMaxCharactersInitial()
{
  ToolkitTestApplication application;

  tet_infoline("Testing Setting Initial Text obeys Max Character Limit");

  TextInput textInput = TextInput::New();  // create empty TextInput

  Stage::GetCurrent().Add(textInput);

  textInput.SetMaxCharacterLength(4);

  textInput.SetInitialText("TooBig");

  tet_printf( "Get text result : %s\n", textInput.GetText().c_str());

  DALI_TEST_EQUALS("TooB",textInput.GetText(), TEST_LOCATION); // Get text which should be only 4 characters
}


static void UtcDaliTextInputExceedMaxCharacters()
{
  ToolkitTestApplication application;

  tet_infoline("Testing Max characters is obeyed when inputting key events ");

  TextInput textInput = TextInput::New();  // create empty TextInput

  Stage::GetCurrent().Add(textInput);

  textInput.SetMaxCharacterLength(4);

  textInput.SetInitialText("");

  textInput.SetEditable(true);

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
}

static void UtcDaliTextInputSetNumberOfLines()
{
  ToolkitTestApplication application;

  tet_infoline("Ensuring API for setting and getting max number of lines is correct");

  TextInput textInput = TextInput::New();  // create empty TextInput

  unsigned int numberOfLines = 1;

  textInput.SetNumberOfLinesLimit( numberOfLines );

  DALI_TEST_EQUALS(numberOfLines ,textInput.GetNumberOfLinesLimit(),  TEST_LOCATION);
}

static void UtcDaliTextInputSetAndGetFadeBoundary()
{
  tet_infoline("UtcDaliTextViewSetAndGetFadeBoundary: ");

  ToolkitTestApplication application;

  TextView::FadeBoundary fadeBoundary( PixelSize( 0 ), PixelSize( 20 ), PixelSize( 0 ), PixelSize( 10 ) );

  TextInput textInput = TextInput::New();
  textInput.SetInitialText( "Hello world!" );

  textInput.SetFadeBoundary( fadeBoundary );

  TextView::FadeBoundary fadeBoundary2 = textInput.GetFadeBoundary();

  DALI_TEST_EQUALS( fadeBoundary.mLeft, fadeBoundary2.mLeft, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeBoundary.mRight, fadeBoundary2.mRight, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeBoundary.mTop, fadeBoundary2.mTop, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeBoundary.mBottom, fadeBoundary2.mBottom, TEST_LOCATION );
}

static void UtcDaliTextInputSetAndGetWidthExceedPolicy()
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
}

static void UtcDaliTextInputSetAndGetHeightExceedPolicy()
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
}

static void UtcDaliTextInputScroll()
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
}
