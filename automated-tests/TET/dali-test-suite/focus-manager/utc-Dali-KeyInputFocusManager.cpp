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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali/integration-api/events/key-event-integ.h>

#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Toolkit;

namespace
{

/**
 * Callback class for KeyInputFocusChanged signal.
 */
class KeyInputFocusChangedCallback : public Dali::ConnectionTracker
{
public:
  /**
   * Constructor
   * @param[in]  gainActor  Ref to the actor that should be set as the one that gains key input focus.
   * @param[in]  lostActor  Ref to the actor that should be set as the one that loses key input focus.
   */
  KeyInputFocusChangedCallback( Control& gainActor, Control& lostActor )
  : mActorGain( gainActor ),
    mActorLost( lostActor )
  {
  }

  void Callback( Control gainingActor, Control lostActor )
  {
    mActorGain = gainingActor;
    mActorLost = lostActor;
  }

  Control& mActorGain;
  Control& mActorLost;
};

// Stores data that is populated in the callback and will be read by the TET cases
struct SignalData
{
  SignalData()
  : functorCalled(false)
  {}

  void Reset()
  {
    functorCalled = false;

    receivedKeyEvent.keyModifier = 0;
    receivedKeyEvent.keyPressedName.clear();
    receivedKeyEvent.keyPressed.clear();

  }

  bool functorCalled;
  KeyEvent receivedKeyEvent;
};

/**
 * Callback class to test SignalUnhandledKeyEvent signal
 */
class SignalUnhandledKeyEventCallback : public Dali::ConnectionTracker
{
public:
  SignalUnhandledKeyEventCallback( SignalData& data ) : mSignalData( data ) { }

  void Callback(const KeyEvent& event)
  {
    mSignalData.functorCalled = true;
    mSignalData.receivedKeyEvent = event;
  }

  SignalData& mSignalData;
};

} // namespace

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
TEST_FUNCTION( UtcDaliKeyInputFocusManagerGet,                        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyInputFocusManagerSetFocus,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyInputFocusManagerGetCurrentFocusControl,     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyInputFocusManagerRemoveFocus,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyInputFocusManagerIsKeyboardListener,         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyInputFocusManagerSignalKeyInputFocusChanged, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyInputFocusManagerSignalUnhandledKeyEvent,    POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliKeyInputFocusManagerGet()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyInputFocusManagerGet");

  KeyInputFocusManager manager;
  {
    manager = KeyInputFocusManager::Get();
    DALI_TEST_CHECK(manager);
  }

  KeyInputFocusManager newManager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(newManager);

  // Check that focus manager is a singleton
  DALI_TEST_CHECK(manager == newManager);
}

static void UtcDaliKeyInputFocusManagerSetFocus()
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  tet_infoline(" UtcDaliKeyInputFocusManagerSetFocus");

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  PushButton pushButton1 = PushButton::New();
  stage.Add( pushButton1 );

  manager.SetFocus(pushButton1);
  DALI_TEST_CHECK(pushButton1.HasKeyInputFocus());
}

static void UtcDaliKeyInputFocusManagerGetCurrentFocusControl()
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  tet_infoline(" UtcDaliKeyInputFocusManagerGetCurrentFocusControl");

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  PushButton pushButton1 = PushButton::New();
  PushButton pushButton2 = PushButton::New();
  stage.Add( pushButton1 );
  stage.Add( pushButton2 );

  manager.SetFocus(pushButton1);
  DALI_TEST_CHECK(pushButton1 == manager.GetCurrentFocusControl());

  manager.SetFocus(pushButton2);
  DALI_TEST_CHECK(pushButton2 == manager.GetCurrentFocusControl());

  manager.SetFocus(pushButton1);
  DALI_TEST_CHECK(pushButton1 == manager.GetCurrentFocusControl());
}

static void UtcDaliKeyInputFocusManagerRemoveFocus()
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  tet_infoline(" UtcDaliKeyInputFocusManagerRemoveFocus");

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  PushButton pushButton1 = PushButton::New();
  PushButton pushButton2 = PushButton::New();
  stage.Add( pushButton1 );
  stage.Add( pushButton2 );

  manager.SetFocus(pushButton1);
  DALI_TEST_CHECK(pushButton1 == manager.GetCurrentFocusControl());

  manager.SetFocus(pushButton2);
  DALI_TEST_CHECK(pushButton2 == manager.GetCurrentFocusControl());

  manager.RemoveFocus(pushButton2);
  DALI_TEST_CHECK(pushButton1 == manager.GetCurrentFocusControl());

  manager.RemoveFocus(pushButton1);
  DALI_TEST_CHECK(Control() == manager.GetCurrentFocusControl());
}

static void UtcDaliKeyInputFocusManagerIsKeyboardListener()
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  tet_infoline(" UtcDaliKeyInputFocusManagerIsKeyboardListener");

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  PushButton pushButton1 = PushButton::New();
  PushButton pushButton2 = PushButton::New();
  stage.Add( pushButton1 );
  stage.Add( pushButton2 );

  manager.SetFocus(pushButton1);
  DALI_TEST_CHECK(pushButton1 == manager.GetCurrentFocusControl());

  manager.SetFocus(pushButton2);
  DALI_TEST_CHECK(pushButton2 == manager.GetCurrentFocusControl());

  DALI_TEST_CHECK(manager.IsKeyboardListener(pushButton1));
  DALI_TEST_CHECK(manager.IsKeyboardListener(pushButton2));

  manager.RemoveFocus(pushButton2);
  DALI_TEST_CHECK(!manager.IsKeyboardListener(pushButton2));

  manager.RemoveFocus(pushButton1);
  DALI_TEST_CHECK(!manager.IsKeyboardListener(pushButton1));

  manager.SetFocus(pushButton2);
  DALI_TEST_CHECK(manager.IsKeyboardListener(pushButton2));
  pushButton2.ClearKeyInputFocus();
  DALI_TEST_CHECK(!manager.IsKeyboardListener(pushButton2));
}

static void UtcDaliKeyInputFocusManagerSignalKeyInputFocusChanged()
{
  ToolkitTestApplication application;
  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  Stage stage = Stage::GetCurrent();

  tet_infoline(" UtcDaliKeyInputFocusManagerSignalKeyInputFocusChanged");

  PushButton pushButton1 = PushButton::New();
  PushButton pushButton2 = PushButton::New();

  stage.Add( pushButton1 );
  stage.Add( pushButton2 );

  PushButton gainActor, lostActor;
  KeyInputFocusChangedCallback callback( gainActor, lostActor );
  manager.KeyInputFocusChangedSignal().Connect( &callback, &KeyInputFocusChangedCallback::Callback );

  manager.SetFocus(pushButton1);

  DALI_TEST_CHECK( gainActor == pushButton1 );
  DALI_TEST_CHECK( lostActor == Control() );

  gainActor = lostActor = NULL;

  manager.SetFocus(pushButton2);

  DALI_TEST_CHECK( gainActor == pushButton2 );
  DALI_TEST_CHECK( lostActor == pushButton1 );

  gainActor = lostActor = NULL;

  // Removing the focus actor from the stage would also result in signal emission.
  stage.Remove( pushButton1 );
  stage.Remove( pushButton2 );

  DALI_TEST_CHECK( gainActor == Control() );
  DALI_TEST_CHECK( lostActor == Control() );
}

static void UtcDaliKeyInputFocusManagerSignalUnhandledKeyEvent()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliKeyInputFocusManagerSignalUnhandledKeyEvent");

  SignalData data;
  SignalUnhandledKeyEventCallback callback( data );

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  manager.UnhandledKeyEventSignal().Connect( &callback, &SignalUnhandledKeyEventCallback::Callback );


  Integration::KeyEvent event("a", "a", 0, 0, 0, Integration::KeyEvent::Up);
  application.ProcessEvent(event);

  DALI_TEST_CHECK(data.functorCalled);
  DALI_TEST_CHECK(event.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK(event.keyCode == data.receivedKeyEvent.keyCode);
  DALI_TEST_CHECK(event.keyString == data.receivedKeyEvent.keyPressed );
  DALI_TEST_CHECK(event.state == data.receivedKeyEvent.state );

  data.Reset();

  Integration::KeyEvent event2("v", "v", 0, 0, 0, Integration::KeyEvent::Up);
  application.ProcessEvent(event2);

  DALI_TEST_CHECK(data.functorCalled);
  DALI_TEST_CHECK(event2.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK(event2.keyCode == data.receivedKeyEvent.keyCode);
  DALI_TEST_CHECK(event2.keyString == data.receivedKeyEvent.keyPressed );
}
