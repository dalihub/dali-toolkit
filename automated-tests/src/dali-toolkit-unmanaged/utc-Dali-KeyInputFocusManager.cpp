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

#include "dummy-control.h"

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

void key_input_focus_manager_startup(void)
{
  test_return_value = TET_UNDEF;
}

void key_input_focus_manager_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliKeyInputFocusManagerGet(void)
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
  END_TEST;
}

int UtcDaliKeyInputFocusManagerSetFocus01(void)
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  tet_infoline(" Check that there is no focused control. Add a control to the stack. Check it is now the focused actor and receives KeyInputFocusGained signal");

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  Control focusedControl = manager.GetCurrentFocusControl();
  DALI_TEST_CHECK( ! focusedControl );

  DummyControl dummy = DummyControl::New(true);
  DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());
  dummy.SetSize(100.0f, 100.0f);
  stage.Add( dummy );
  DALI_TEST_CHECK( ! dummyImpl.keyInputFocusGained );

  manager.SetFocus( dummy );
  DALI_TEST_CHECK( dummy.HasKeyInputFocus()); // Also tests IsKeyboardListener() API
  DALI_TEST_CHECK( dummyImpl.keyInputFocusGained );

  focusedControl = manager.GetCurrentFocusControl();
  DALI_TEST_CHECK( focusedControl );
  DALI_TEST_CHECK( focusedControl == dummy );

  END_TEST;
}

int UtcDaliKeyInputFocusManagerSetFocus02(void)
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  tet_infoline(" Add a control to the stack. Check it is now the focused actor and receives KeyInputFocusGained signal. Add another control - check that the first control receives KeyInputFocusLost");

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  DummyControl dummy1 = DummyControl::New(true);
  DummyControlImplOverride& dummy1Impl = static_cast<DummyControlImplOverride&>(dummy1.GetImplementation());
  dummy1.SetSize(100.0f, 100.0f);
  stage.Add( dummy1 );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );

  stage.Add( dummy1 );

  manager.SetFocus( dummy1 );
  DALI_TEST_CHECK( dummy1.HasKeyInputFocus()); // Also tests IsKeyboardListener() API
  DALI_TEST_CHECK( dummy1Impl.keyInputFocusGained );
  dummy1Impl.keyInputFocusGained = false;

  DummyControl dummy2 = DummyControl::New(true);
  DummyControlImplOverride& dummy2Impl = static_cast<DummyControlImplOverride&>(dummy2.GetImplementation());
  dummy2.SetSize(100.0f, 100.0f);
  stage.Add( dummy2 );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );

  stage.Add( dummy2 );

  manager.SetFocus( dummy2 );
  DALI_TEST_CHECK( dummy2.HasKeyInputFocus()); // Also tests IsKeyboardListener() API
  DALI_TEST_CHECK( dummy2Impl.keyInputFocusGained );
  dummy1Impl.keyInputFocusGained = false;

  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( dummy1Impl.keyInputFocusLost );


  END_TEST;
}

int UtcDaliKeyInputFocusManagerGetCurrentFocusControl(void)
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  tet_infoline(" Add 2 controls, check they each get focused. Re-focus the first control - ensure it's now got focus (check signals)");

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  DummyControl dummy1 = DummyControl::New(true);
  DummyControlImplOverride& dummy1Impl = static_cast<DummyControlImplOverride&>(dummy1.GetImplementation());
  dummy1.SetSize(100.0f, 100.0f);
  stage.Add( dummy1 );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );

  DummyControl dummy2 = DummyControl::New(true);
  DummyControlImplOverride& dummy2Impl = static_cast<DummyControlImplOverride&>(dummy2.GetImplementation());
  dummy2.SetSize(100.0f, 100.0f);
  stage.Add( dummy2 );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusLost );

  manager.SetFocus(dummy1);
  DALI_TEST_CHECK( dummy1 == manager.GetCurrentFocusControl() );
  DALI_TEST_CHECK( dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );
  dummy1Impl.keyInputFocusGained = false;
  dummy1Impl.keyInputFocusLost = false;

  manager.SetFocus(dummy2);
  DALI_TEST_CHECK( dummy2 == manager.GetCurrentFocusControl() );
  DALI_TEST_CHECK( dummy1Impl.keyInputFocusLost );
  DALI_TEST_CHECK( dummy2Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusLost );
  // Reset signal received
  dummy1Impl.keyInputFocusGained = false;
  dummy1Impl.keyInputFocusLost = false;
  dummy2Impl.keyInputFocusGained = false;
  dummy2Impl.keyInputFocusLost = false;

  manager.SetFocus(dummy1);
  DALI_TEST_CHECK( dummy1 == manager.GetCurrentFocusControl());
  DALI_TEST_CHECK( dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( dummy2Impl.keyInputFocusLost );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusGained );
  END_TEST;
}

int UtcDaliKeyInputFocusManagerRemoveFocus01(void)
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  tet_infoline(" Add 3 focus controls. Test that removing the topmost informs the next control that it now has focus, Test that the bottommost control doesn't receive anything. ");

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  DummyControl dummy1 = DummyControl::New(true);
  DummyControlImplOverride& dummy1Impl = static_cast<DummyControlImplOverride&>(dummy1.GetImplementation());
  dummy1.SetSize(100.0f, 100.0f);
  stage.Add( dummy1 );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );

  DummyControl dummy2 = DummyControl::New(true);
  DummyControlImplOverride& dummy2Impl = static_cast<DummyControlImplOverride&>(dummy2.GetImplementation());
  dummy2.SetSize(100.0f, 100.0f);
  stage.Add( dummy2 );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusLost );

  manager.SetFocus(dummy1);
  DALI_TEST_CHECK(dummy1 == manager.GetCurrentFocusControl());

  manager.SetFocus(dummy2);
  DALI_TEST_CHECK(dummy2 == manager.GetCurrentFocusControl());
  dummy1Impl.keyInputFocusGained = false;
  dummy1Impl.keyInputFocusLost = false;
  dummy2Impl.keyInputFocusGained = false;
  dummy2Impl.keyInputFocusLost = false;

  manager.RemoveFocus(dummy2);
  DALI_TEST_CHECK(dummy1 == manager.GetCurrentFocusControl());
  DALI_TEST_CHECK( dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( dummy2Impl.keyInputFocusLost );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusGained );
  dummy1Impl.keyInputFocusGained = false;
  dummy1Impl.keyInputFocusLost = false;
  dummy2Impl.keyInputFocusGained = false;
  dummy2Impl.keyInputFocusLost = false;

  manager.RemoveFocus(dummy1);
  DALI_TEST_CHECK(Control() == manager.GetCurrentFocusControl());
  DALI_TEST_CHECK( dummy1Impl.keyInputFocusLost );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusLost );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusGained );

  END_TEST;
}

int UtcDaliKeyInputFocusManagerRemoveFocus02(void)
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  tet_infoline(" Add 3 focus controls. Test that removing the bottommost doesn't change the focused control" );

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  DummyControl dummy1 = DummyControl::New(true);
  DummyControlImplOverride& dummy1Impl = static_cast<DummyControlImplOverride&>(dummy1.GetImplementation());
  dummy1.SetSize(100.0f, 100.0f);
  stage.Add( dummy1 );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );

  DummyControl dummy2 = DummyControl::New(true);
  DummyControlImplOverride& dummy2Impl = static_cast<DummyControlImplOverride&>(dummy2.GetImplementation());
  dummy2.SetSize(100.0f, 100.0f);
  stage.Add( dummy2 );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusLost );

  DummyControl dummy3 = DummyControl::New(true);
  DummyControlImplOverride& dummy3Impl = static_cast<DummyControlImplOverride&>(dummy3.GetImplementation());
  dummy3.SetSize(100.0f, 100.0f);
  stage.Add( dummy3 );
  DALI_TEST_CHECK( ! dummy3Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy3Impl.keyInputFocusLost );

  manager.SetFocus(dummy1);
  DALI_TEST_CHECK(dummy1 == manager.GetCurrentFocusControl());

  manager.SetFocus(dummy2);
  DALI_TEST_CHECK(dummy2 == manager.GetCurrentFocusControl());

  manager.SetFocus(dummy3);
  DALI_TEST_CHECK(dummy3 == manager.GetCurrentFocusControl());
  dummy1Impl.keyInputFocusGained = false;
  dummy1Impl.keyInputFocusLost = false;
  dummy2Impl.keyInputFocusGained = false;
  dummy2Impl.keyInputFocusLost = false;
  dummy3Impl.keyInputFocusGained = false;
  dummy3Impl.keyInputFocusLost = false;

  manager.RemoveFocus(dummy1);
  DALI_TEST_CHECK(dummy3 == manager.GetCurrentFocusControl());
  DALI_TEST_CHECK( dummy1Impl.keyInputFocusLost );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusLost );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy3Impl.keyInputFocusLost );
  DALI_TEST_CHECK( ! dummy3Impl.keyInputFocusGained );
  dummy1Impl.keyInputFocusGained = false;
  dummy1Impl.keyInputFocusLost = false;
  dummy2Impl.keyInputFocusGained = false;
  dummy2Impl.keyInputFocusLost = false;
  dummy3Impl.keyInputFocusGained = false;
  dummy3Impl.keyInputFocusLost = false;

  END_TEST;
}

int UtcDaliKeyInputFocusManagerRemoveFocus03(void)
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  tet_infoline(" Add 2 focus controls. Remove each of them from the bottom up. Test that the stack is now empty. Add a new control - check that it correctly has focus" );

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  DummyControl dummy1 = DummyControl::New(true);
  DummyControlImplOverride& dummy1Impl = static_cast<DummyControlImplOverride&>(dummy1.GetImplementation());
  dummy1.SetSize(100.0f, 100.0f);
  stage.Add( dummy1 );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );

  DummyControl dummy2 = DummyControl::New(true);
  DummyControlImplOverride& dummy2Impl = static_cast<DummyControlImplOverride&>(dummy2.GetImplementation());
  dummy2.SetSize(100.0f, 100.0f);
  stage.Add( dummy2 );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusLost );

  manager.SetFocus(dummy1);
  DALI_TEST_CHECK(dummy1 == manager.GetCurrentFocusControl());

  manager.SetFocus(dummy2);
  DALI_TEST_CHECK(dummy2 == manager.GetCurrentFocusControl());

  dummy1Impl.keyInputFocusGained = false;
  dummy1Impl.keyInputFocusLost = false;
  dummy2Impl.keyInputFocusGained = false;
  dummy2Impl.keyInputFocusLost = false;

  manager.RemoveFocus(dummy1);
  DALI_TEST_CHECK(dummy2 == manager.GetCurrentFocusControl());
  DALI_TEST_CHECK( dummy1Impl.keyInputFocusLost );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusLost );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusGained );
  dummy1Impl.keyInputFocusGained = false;
  dummy1Impl.keyInputFocusLost = false;
  dummy2Impl.keyInputFocusGained = false;
  dummy2Impl.keyInputFocusLost = false;

  manager.RemoveFocus(dummy2);
  DALI_TEST_CHECK(Control() == manager.GetCurrentFocusControl());
  DALI_TEST_CHECK( dummy2Impl.keyInputFocusLost );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );
  dummy1Impl.keyInputFocusGained = false;
  dummy1Impl.keyInputFocusLost = false;
  dummy2Impl.keyInputFocusGained = false;
  dummy2Impl.keyInputFocusLost = false;

  PushButton pushButton1 = PushButton::New();
  stage.Add( pushButton1 );
  manager.SetFocus( pushButton1 );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusLost );
  DALI_TEST_CHECK( pushButton1 == manager.GetCurrentFocusControl());
  END_TEST;
}

int UtcDaliKeyInputFocusManagerRemoveFocus04(void)
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  tet_infoline(" Test what happens if the removed control is not on the focus stack");
  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  PushButton pushButton1 = PushButton::New();
  PushButton pushButton2 = PushButton::New();
  stage.Add( pushButton1 );
  stage.Add( pushButton2 );
  manager.SetFocus( pushButton1 );
  manager.SetFocus( pushButton2 );

  PushButton pushButton3 = PushButton::New();
  stage.Add( pushButton3 );
  manager.RemoveFocus( pushButton3 );
  DALI_TEST_CHECK( pushButton2 == manager.GetCurrentFocusControl());
  END_TEST;
}

int UtcDaliKeyInputFocusManagerDestroyObject01(void)
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  tet_infoline(" Add 2 controls to the stack. Unparent and destroy the topmost. Check that it is removed from the stack, and that the bottommost is correctly focused" );

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  DummyControl dummy1 = DummyControl::New(true);
  DummyControlImplOverride& dummy1Impl = static_cast<DummyControlImplOverride&>(dummy1.GetImplementation());
  dummy1.SetSize(100.0f, 100.0f);
  stage.Add( dummy1 );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );

  {
    DummyControl dummy2 = DummyControl::New(true);
    DummyControlImplOverride& dummy2Impl = static_cast<DummyControlImplOverride&>(dummy2.GetImplementation());
    dummy2.SetSize(100.0f, 100.0f);
    stage.Add( dummy2 );
    DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusGained );
    DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusLost );

    manager.SetFocus(dummy1);
    DALI_TEST_CHECK(dummy1 == manager.GetCurrentFocusControl());

    manager.SetFocus(dummy2);
    DALI_TEST_CHECK(dummy2 == manager.GetCurrentFocusControl());

    dummy1Impl.keyInputFocusGained = false;
    dummy1Impl.keyInputFocusLost = false;
    dummy2Impl.keyInputFocusGained = false;
    dummy2Impl.keyInputFocusLost = false;

    stage.Remove(dummy2);
  }

  DALI_TEST_CHECK(dummy1 == manager.GetCurrentFocusControl());
  DALI_TEST_CHECK( dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );
  END_TEST;
}

int UtcDaliKeyInputFocusManagerDestroyObject02(void)
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  tet_infoline(" Add 2 controls to the stack. Destroy a different actor entirely. Check that the stack is un-affected.");

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  DummyControl dummy1 = DummyControl::New(true);
  DummyControlImplOverride& dummy1Impl = static_cast<DummyControlImplOverride&>(dummy1.GetImplementation());
  dummy1.SetSize(100.0f, 100.0f);
  stage.Add( dummy1 );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );


  DummyControl dummy2 = DummyControl::New(true);
  DummyControlImplOverride& dummy2Impl = static_cast<DummyControlImplOverride&>(dummy2.GetImplementation());
  dummy2.SetSize(100.0f, 100.0f);
  stage.Add( dummy2 );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusLost );

  manager.SetFocus(dummy1);
  DALI_TEST_CHECK(dummy1 == manager.GetCurrentFocusControl());

  manager.SetFocus(dummy2);
  DALI_TEST_CHECK(dummy2 == manager.GetCurrentFocusControl());

  dummy1Impl.keyInputFocusGained = false;
  dummy1Impl.keyInputFocusLost = false;
  dummy2Impl.keyInputFocusGained = false;
  dummy2Impl.keyInputFocusLost = false;

  {
    BitmapImage image = CreateBitmapImage();
    ImageActor imageActor = ImageActor::New( image );
    stage.Add( imageActor );
    imageActor.SetSize(100, 100);

    application.SendNotification();
    application.Render();

    stage.Remove( imageActor );
  }

  DALI_TEST_CHECK( dummy2 == manager.GetCurrentFocusControl());
  END_TEST;
}

int UtcDaliKeyInputFocusManagerIsKeyboardListener(void)
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
  END_TEST;
}

int UtcDaliKeyInputFocusManagerSignalKeyInputFocusChanged(void)
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
  END_TEST;
}

int UtcDaliKeyInputFocusManagerSignalUnhandledKeyEvent(void)
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
  DALI_TEST_CHECK(event.state == static_cast<Integration::KeyEvent::State>(data.receivedKeyEvent.state) );

  data.Reset();

  Integration::KeyEvent event2("v", "v", 0, 0, 0, Integration::KeyEvent::Up);
  application.ProcessEvent(event2);

  DALI_TEST_CHECK(data.functorCalled);
  DALI_TEST_CHECK(event2.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK(event2.keyCode == data.receivedKeyEvent.keyCode);
  DALI_TEST_CHECK(event2.keyString == data.receivedKeyEvent.keyPressed );
  END_TEST;
}
