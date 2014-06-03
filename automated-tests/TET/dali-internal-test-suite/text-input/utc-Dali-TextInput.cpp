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
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit-test-suite-utils.h>

#include <dali/integration-api/events/key-event-integ.h>

// Internal includes
#include <dali-toolkit/internal/controls/text-input/text-input-impl.h>

using namespace Dali;
using namespace Dali::Toolkit;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
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
TEST_FUNCTION( UtcDaliInternalTextInputTextSelection, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextInputSetGetExceedEnabled, POSITIVE_TC_IDX );
// TEST_FUNCTION( UtcDaliTextInputMethod02, NEGATIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}


/**
 *  Test: Selected is replaced by new input text.
 **/
static void UtcDaliInternalTextInputTextSelection()
{
  const char* testChar  = "v";

  ToolkitTestApplication application;

  tet_infoline("Testing Text Selection with replace.");

  Toolkit::TextInput textInput = Toolkit::TextInput::New();

  DALI_TEST_CHECK(textInput);

  Stage::GetCurrent().Add(textInput);

  textInput.SetInitialText("Test String");

  std::string initialText = textInput.GetText();

  tet_printf("Set Initial text: %s\n", initialText.c_str() );

  textInput.SetKeyInputFocus();

  GetImpl(textInput).SelectText(0,11);

  tet_printf("Select all of Initial text\n");

  Integration::KeyEvent event(testChar, testChar, 0, 0, 0, Integration::KeyEvent::Down );

  application.ProcessEvent( event );

  tet_printf("Simulate pressing of a key: %s\n", testChar );

  std::string newText = textInput.GetText();

  tet_printf("Check current text (%s) is the new text \n", newText.c_str() );

  DALI_TEST_EQUALS("v",textInput.GetText(), TEST_LOCATION);
}

static void UtcDaliTextInputSetGetExceedEnabled()
{
  tet_infoline("UtcDaliTextInputSetGetExceedEnabled: ");

  ToolkitTestApplication application;

  Toolkit::TextInput textInput = Toolkit::TextInput::New();
  textInput.SetMultilinePolicy( Toolkit::TextView::SplitByWord );
  textInput.SetWidthExceedPolicy( Toolkit::TextView::Split );
  textInput.SetHeightExceedPolicy( Toolkit::TextView::Original );

  DALI_TEST_CHECK( textInput.GetExceedEnabled() );

  Toolkit::Internal::TextInput& textInputImpl = static_cast<Toolkit::Internal::TextInput&>( textInput.GetImplementation() );

  textInput.SetSize( 50.f, 50.f );
  textInput.SetExceedEnabled( false );

  DALI_TEST_CHECK( !textInput.GetExceedEnabled() );


  textInputImpl.InsertAt( Text("He"), 0 );

  DALI_TEST_EQUALS("He",textInput.GetText(), TEST_LOCATION);

  textInputImpl.InsertAt( Text("llo"), 2 );

  DALI_TEST_EQUALS("Hello",textInput.GetText(), TEST_LOCATION);

  textInputImpl.InsertAt( Text(" world! hello world hello world hello world"), 5 ); // Doesn't fit so is not added.

  DALI_TEST_EQUALS("Hello",textInput.GetText(), TEST_LOCATION);
}
