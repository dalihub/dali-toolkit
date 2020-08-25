/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>
#include <dali/devel-api/common/stage-devel.h>

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

    receivedKeyEvent.Reset();

  }

  bool functorCalled;
  KeyEvent receivedKeyEvent;
};

/**
 * Callback class for KeyEvent signal of control.
 */
class KeyEventCallback : public Dali::ConnectionTracker
{
public:
  /**
   * Constructor
   * @param[in]  returnValue  Set return value of KeyEvent callback.
   * */
  KeyEventCallback( bool consumed )
  : mConsumed( consumed ),
    mIsCalled( false )
  {
  }

  bool Callback( Control control, const KeyEvent& keyEvent )
  {
    mIsCalled = true;
    return mConsumed;
  }

  void Callback( const KeyEvent& keyEvent )
  {
    mIsCalled = true;
  }

  bool mConsumed;
  bool mIsCalled;
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
  Integration::Scene stage = application.GetScene();

  tet_infoline(" Check that there is no focused control. Set focus to control. Check it is now the focused actor and receives KeyInputFocusGained signal");

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  Control focusedControl = manager.GetCurrentFocusControl();
  DALI_TEST_CHECK( ! focusedControl );

  DummyControl dummy = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
  dummy.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
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
  Integration::Scene stage = application.GetScene();

  tet_infoline(" Set focus to control. Check it is now the focused actor and receives KeyInputFocusGained signal. Set focuse to another control - check that the first control receives KeyInputFocusLost");

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  DummyControl dummy1 = DummyControl::New(true);
  Impl::DummyControl& dummy1Impl = static_cast<Impl::DummyControl&>(dummy1.GetImplementation());
  dummy1.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  stage.Add( dummy1 );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );

  manager.SetFocus( dummy1 );
  DALI_TEST_CHECK( dummy1.HasKeyInputFocus()); // Also tests IsKeyboardListener() API
  DALI_TEST_CHECK( dummy1Impl.keyInputFocusGained );
  dummy1Impl.keyInputFocusGained = false;

  DummyControl dummy2 = DummyControl::New(true);
  Impl::DummyControl& dummy2Impl = static_cast<Impl::DummyControl&>(dummy2.GetImplementation());
  dummy2.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  stage.Add( dummy2 );
  DALI_TEST_CHECK( ! dummy2Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );

  manager.SetFocus( dummy2 );
  DALI_TEST_CHECK( dummy2.HasKeyInputFocus()); // Also tests IsKeyboardListener() API
  DALI_TEST_CHECK( dummy2Impl.keyInputFocusGained );
  dummy1Impl.keyInputFocusGained = false;

  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( dummy1Impl.keyInputFocusLost );

  END_TEST;
}

int UtcDaliKeyInputFocusManagerKeyEventPropagation01(void)
{

  ToolkitTestApplication application;
  Integration::Scene stage = application.GetScene();

  tet_infoline("Test KeyEvent propagation. If focused control doesn't consume KeyEvent, KeyEvent will be recursively delivered to the control and its parents, until the event is consumed or the stage is reached. In this case, KeyEvent is delivered to KeyboardFocusManager via Stage's KeyEventSignal");

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  KeyEventCallback stageCallback( false );
  stage.KeyEventSignal().Connect( &stageCallback, &KeyEventCallback::Callback );

  DummyControl dummy1 = DummyControl::New(true);
  dummy1.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  KeyEventCallback callback1( false );
  dummy1.KeyEventSignal().Connect( &callback1, &KeyEventCallback::Callback );
  stage.Add( dummy1 );

  DummyControl dummy2 = DummyControl::New(true);
  dummy2.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  KeyEventCallback callback2( false );
  dummy2.KeyEventSignal().Connect( &callback2, &KeyEventCallback::Callback );
  dummy1.Add( dummy2 );

  DummyControl dummy3 = DummyControl::New(true);
  Impl::DummyControl& dummy3Impl = static_cast<Impl::DummyControl&>(dummy3.GetImplementation());
  dummy3.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  KeyEventCallback callback3( false );
  dummy3.KeyEventSignal().Connect( &callback3, &KeyEventCallback::Callback );
  dummy2.Add( dummy3 );
  DALI_TEST_CHECK( ! dummy3Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy3Impl.keyInputFocusLost );

  manager.SetFocus( dummy3 );
  DALI_TEST_CHECK( dummy3Impl.keyInputFocusGained );

  Integration::KeyEvent event( "a", "", "a", 0, 0, 0, Integration::KeyEvent::Up, "", "", Device::Class::TOUCH, Device::Subclass::NONE );
  application.ProcessEvent(event);

  DALI_TEST_CHECK( callback1.mIsCalled );
  DALI_TEST_CHECK( callback2.mIsCalled );
  DALI_TEST_CHECK( callback3.mIsCalled );
  DALI_TEST_CHECK( stageCallback.mIsCalled );

  END_TEST;
}

int UtcDaliKeyInputFocusManagerKeyEventPropagation02(void)
{

  ToolkitTestApplication application;
  Integration::Scene stage = application.GetScene();

  tet_infoline("Test KeyEvent propagation. If focused control doesn't consume KeyEvent, KeyEvent will be recursively delivered to the control and its parents, until the event is consumed or the stage is reached. In this case, KeyEvent is delivered from dummy3 to dummy2");

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  DummyControl dummy1 = DummyControl::New(true);
  dummy1.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  KeyEventCallback callback1( false );
  dummy1.KeyEventSignal().Connect( &callback1, &KeyEventCallback::Callback );
  stage.Add( dummy1 );

  DummyControl dummy2 = DummyControl::New(true);
  dummy2.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  KeyEventCallback callback2( true );
  dummy2.KeyEventSignal().Connect( &callback2, &KeyEventCallback::Callback );
  dummy1.Add( dummy2 );

  DummyControl dummy3 = DummyControl::New(true);
  Impl::DummyControl& dummy3Impl = static_cast<Impl::DummyControl&>(dummy3.GetImplementation());
  dummy3.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  KeyEventCallback callback3( false );
  dummy3.KeyEventSignal().Connect( &callback3, &KeyEventCallback::Callback );
  dummy2.Add( dummy3 );
  DALI_TEST_CHECK( ! dummy3Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy3Impl.keyInputFocusLost );

  manager.SetFocus( dummy3 );
  DALI_TEST_CHECK( dummy3Impl.keyInputFocusGained );

  Integration::KeyEvent event( "a", "", "a", 0, 0, 0, Integration::KeyEvent::Up, "", "", Device::Class::TOUCH, Device::Subclass::NONE );
  application.ProcessEvent(event);

  DALI_TEST_CHECK( !callback1.mIsCalled );
  DALI_TEST_CHECK( callback2.mIsCalled );
  DALI_TEST_CHECK( callback3.mIsCalled );

  END_TEST;
}

int UtcDaliKeyInputFocusManagerGetCurrentFocusControl(void)
{
  ToolkitTestApplication application;
  Integration::Scene stage = application.GetScene();

  tet_infoline(" Add 2 controls, check they each get focused. Re-focus the first control - ensure it's now got focus (check signals)");

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  DummyControl dummy1 = DummyControl::New(true);
  Impl::DummyControl& dummy1Impl = static_cast<Impl::DummyControl&>(dummy1.GetImplementation());
  dummy1.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  stage.Add( dummy1 );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );

  DummyControl dummy2 = DummyControl::New(true);
  Impl::DummyControl& dummy2Impl = static_cast<Impl::DummyControl&>(dummy2.GetImplementation());
  dummy2.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
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

int UtcDaliKeyInputFocusManagerRemoveFocus(void)
{
  ToolkitTestApplication application;
  Integration::Scene stage = application.GetScene();

  tet_infoline(" Add focus controls. Test that removing focus from control which has focus. ");

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);

  DummyControl dummy1 = DummyControl::New(true);
  Impl::DummyControl& dummy1Impl = static_cast<Impl::DummyControl&>(dummy1.GetImplementation());
  dummy1.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  stage.Add( dummy1 );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusLost );

  manager.SetFocus(dummy1);
  DALI_TEST_CHECK(dummy1 == manager.GetCurrentFocusControl());
  dummy1Impl.keyInputFocusGained = false;
  dummy1Impl.keyInputFocusLost = false;

  manager.RemoveFocus(dummy1);
  DALI_TEST_CHECK(Control() == manager.GetCurrentFocusControl());
  DALI_TEST_CHECK( dummy1Impl.keyInputFocusLost );
  DALI_TEST_CHECK( ! dummy1Impl.keyInputFocusGained );

  END_TEST;
}

int UtcDaliKeyInputFocusManagerSignalKeyInputFocusChanged(void)
{
  ToolkitTestApplication application;
  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  Integration::Scene stage = application.GetScene();

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

  gainActor.Reset();
  lostActor.Reset();

  manager.SetFocus(pushButton2);

  DALI_TEST_CHECK( gainActor == pushButton2 );
  DALI_TEST_CHECK( lostActor == pushButton1 );

  gainActor.Reset();
  lostActor.Reset();

  // Removing the focus actor from the stage would also result in signal emission.
  stage.Remove( pushButton1 );
  stage.Remove( pushButton2 );

  DALI_TEST_CHECK( gainActor == Control() );
  DALI_TEST_CHECK( lostActor == Control() );
  END_TEST;
}

int UtcDaliKeyInputFocusManagerSignalKeyInputFocusChangedforNewWindow(void)
{
  ToolkitTestApplication application;
  KeyInputFocusManager manager = KeyInputFocusManager::Get();

  tet_infoline(" UtcDaliKeyInputFocusManagerSignalKeyInputFocusChanged");

  PushButton pushButton1 = PushButton::New();
  PushButton pushButton2 = PushButton::New();

  Window window = Window::New(PositionSize(0,0,0,0) ,"", false);
  DALI_TEST_CHECK( window );

  window.Add( pushButton1 );
  window.Add( pushButton2 );

  PushButton gainActor, lostActor;
  KeyInputFocusChangedCallback callback( gainActor, lostActor );
  manager.KeyInputFocusChangedSignal().Connect( &callback, &KeyInputFocusChangedCallback::Callback );

  manager.SetFocus(pushButton1);

  DALI_TEST_CHECK( gainActor == pushButton1 );
  DALI_TEST_CHECK( lostActor == Control() );

  gainActor.Reset();
  lostActor.Reset();

  manager.SetFocus(pushButton2);

  DALI_TEST_CHECK( gainActor == pushButton2 );
  DALI_TEST_CHECK( lostActor == pushButton1 );

  gainActor.Reset();
  lostActor.Reset();

  // Removing the focus actor from the window would also result in signal emission.
  window.Remove( pushButton1 );
  window.Remove( pushButton2 );
  DALI_TEST_CHECK( gainActor == Control() );
  DALI_TEST_CHECK( lostActor == Control() );

  window.Reset();
  END_TEST;
}
