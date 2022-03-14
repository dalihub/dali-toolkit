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
 *
 */

#include <stdlib.h>
#include <iostream>

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <dali-toolkit/devel-api/focus-manager/keyboard-focus-manager-devel.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/wheel-event-integ.h>

using namespace Dali;
using namespace Dali::Toolkit;

void utc_dali_toolkit_keyboard_focus_manager_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_keyboard_focus_manager_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const std::string DEFAULT_DEVICE_NAME("hwKeyboard");

// Functors to test whether GetNextFocusableActor() method of CustomAlgorithmInterface is called when the keyboard focus is about to change
class CustomAlgorithm : public Dali::Toolkit::DevelKeyboardFocusManager::CustomAlgorithmInterface
{
public:
  CustomAlgorithm(bool& interfaceVerified)
  : mInterfaceVerified(interfaceVerified),
    mCurrentFocusedActor(),
    mProposedActorToFocus(),
    mDirection(Control::KeyboardFocus::LEFT)
  {
  }

  Actor GetNextFocusableActor(Actor currentFocusedActor, Actor proposedActorToFocus, Control::KeyboardFocus::Direction direction)
  {
    tet_infoline("Verifying CustomAlgorithm()");

    mInterfaceVerified = true;

    mCurrentFocusedActor  = currentFocusedActor;
    mProposedActorToFocus = proposedActorToFocus;
    mDirection            = direction;

    return mProposedActorToFocus;
  }

  void Reset()
  {
    mInterfaceVerified    = false;
    mCurrentFocusedActor  = Actor();
    mProposedActorToFocus = Actor();
    mDirection            = Control::KeyboardFocus::LEFT;
  }

  bool&                             mInterfaceVerified;
  Actor                             mCurrentFocusedActor;
  Actor                             mProposedActorToFocus;
  Control::KeyboardFocus::Direction mDirection;
};

// Functors to test whether PreFocusChange signal is emitted when the keyboard focus is about to change
class PreFocusChangeCallback : public Dali::ConnectionTracker
{
public:
  PreFocusChangeCallback(bool& signalReceived)
  : mSignalVerified(signalReceived),
    mCurrentFocusedActor(),
    mProposedActorToFocus(),
    mDirection(Control::KeyboardFocus::LEFT)
  {
  }

  Actor Callback(Actor currentFocusedActor, Actor proposedActorToFocus, Control::KeyboardFocus::Direction direction)
  {
    tet_infoline("Verifying PreFocusChangeCallback()");

    mSignalVerified = true;

    mCurrentFocusedActor  = currentFocusedActor;
    mProposedActorToFocus = proposedActorToFocus;
    mDirection            = direction;

    return mProposedActorToFocus;
  }

  void Reset()
  {
    mSignalVerified       = false;
    mCurrentFocusedActor  = Actor();
    mProposedActorToFocus = Actor();
    mDirection            = Control::KeyboardFocus::LEFT;
  }

  bool&                             mSignalVerified;
  Actor                             mCurrentFocusedActor;
  Actor                             mProposedActorToFocus;
  Control::KeyboardFocus::Direction mDirection;
};

// Functors to test whether focus changed signal is emitted when the keyboard focus is changed
class FocusChangedCallback : public Dali::ConnectionTracker
{
public:
  FocusChangedCallback(bool& signalReceived)
  : mSignalVerified(signalReceived),
    mOriginalFocusedActor(),
    mCurrentFocusedActor()
  {
  }

  void Callback(Actor originalFocusedActor, Actor currentFocusedActor)
  {
    tet_infoline("Verifying FocusChangedCallback()");

    if(originalFocusedActor == mCurrentFocusedActor)
    {
      mSignalVerified = true;
    }

    mOriginalFocusedActor = originalFocusedActor;
    mCurrentFocusedActor  = currentFocusedActor;
  }

  void Reset()
  {
    mSignalVerified = false;
  }

  bool& mSignalVerified;
  Actor mOriginalFocusedActor;
  Actor mCurrentFocusedActor;
};

// Functors to test whether focus group changed signal is emitted when the keyboard focus group is changed
class FocusGroupChangedCallback : public Dali::ConnectionTracker
{
public:
  FocusGroupChangedCallback(bool& signalReceived)
  : mSignalVerified(signalReceived),
    mCurrentFocusedActor(),
    mForward(true)
  {
  }

  void Callback(Actor currentFocusedActor, bool forward)
  {
    tet_infoline("Verifying FocusGroupChangedCallback()");

    mSignalVerified = true;

    mCurrentFocusedActor = currentFocusedActor;
    mForward             = forward;
  }

  void Reset()
  {
    mSignalVerified = false;
  }

  bool& mSignalVerified;
  Actor mCurrentFocusedActor;
  bool  mForward;
};

// Functors to test whether focused actor activated signal is emitted when the focused actor is activated
class FocusedActorActivatedCallback : public Dali::ConnectionTracker
{
public:
  FocusedActorActivatedCallback(bool& signalReceived)
  : mSignalVerified(signalReceived),
    mActivatedActor()
  {
  }

  void Callback(Actor activatedActor)
  {
    tet_infoline("Verifying FocusedActorActivatedCallback()");

    mSignalVerified = true;

    mActivatedActor = activatedActor;
  }

  void Reset()
  {
    mSignalVerified = false;
  }

  bool& mSignalVerified;
  Actor mActivatedActor;
};

class KeyEventCallback : public Dali::ConnectionTracker
{
public:
  /**
   * Constructor
   * @param[in]  returnValue  Set return value of KeyEvent callback.
   * */
  KeyEventCallback(bool consumed)
  : mConsumed(consumed),
    mIsCalled(false)
  {
  }

  bool Callback(Control control, const KeyEvent& keyEvent)
  {
    mIsCalled = true;
    return mConsumed;
  }

  void Callback(const KeyEvent& keyEvent)
  {
    mIsCalled = true;
  }

  bool mConsumed;
  bool mIsCalled;
};

class WheelEventCallback : public Dali::ConnectionTracker
{
public:
  /**
   * Constructor
   * @param[in]  returnValue  Set return value of WheelEvent callback.
   * */
  WheelEventCallback(bool consumed)
  : mConsumed(consumed),
    mIsCalled(false)
  {
  }

  bool Callback(Actor actor, const WheelEvent& wheelEvent)
  {
    mIsCalled = true;
    return mConsumed;
  }

  void Callback(const WheelEvent& wheelEvent)
  {
    mIsCalled = true;
  }

  bool mConsumed;
  bool mIsCalled;
};

// Used to connect to signals via the ConnectSignal Handle method
struct CallbackFunctor
{
  CallbackFunctor()
  {
  }

  void operator()()
  {
  }
};

} // namespace

int UtcDaliKeyboardFocusManagerGet(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardKeyboardFocusManagerGet");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo("KeyboardFocusManager");
  DALI_TEST_CHECK(type);
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK(handle);

  KeyboardFocusManager manager;

  manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  KeyboardFocusManager newManager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(newManager);

  // Check that focus manager is a singleton
  DALI_TEST_CHECK(manager == newManager);
  END_TEST;
}

int UtcDaliKeyboardFocusManagerSetAndGetCurrentFocusActor(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerSetAndGetCurrentFocusActor");

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Create the first actor and add it to the stage
  Actor first = Actor::New();
  first.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(first);

  // Create the second actor and add it to the stage
  Actor second = Actor::New();
  second.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(second);

  // Create the third actor but don't add it to the stage
  Actor third = Actor::New();

  // Check that no actor is being focused yet.
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());

  // Check that it will fail to set focus on an invalid actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(Actor()) == false);

  // Check that the focus is set on the first actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);

  // Check that the focus is set on the second actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(second) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);

  // Check that it will fail to set focus on the third actor as it's not in the stage
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(third) == false);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);

  // Add the third actor to the stage
  application.GetScene().Add(third);

  // Check that it will fail to set focus on the third actor as it's not focusable
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(third) == false);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);

  // Make the third actor focusable
  third.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  // Check that the focus is successfully moved to the third actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(third) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
  END_TEST;
}

int UtcDaliKeyboardFocusManagerMoveFocus(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerMoveFocus");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo("KeyboardFocusManager");
  DALI_TEST_CHECK(type);
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK(handle);

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  bool                   preFocusChangeSignalVerified = false;
  PreFocusChangeCallback preFocusChangeCallback(preFocusChangeSignalVerified);
  manager.PreFocusChangeSignal().Connect(&preFocusChangeCallback, &PreFocusChangeCallback::Callback);

  bool                 focusChangedSignalVerified = false;
  FocusChangedCallback focusChangedCallback(focusChangedSignalVerified);
  manager.FocusChangedSignal().Connect(&focusChangedCallback, &FocusChangedCallback::Callback);

  // Create the first actor and add it to the stage
  Actor first = Actor::New();
  first.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(first);

  // Create the second actor and add it to the stage
  Actor second = Actor::New();
  second.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(second);

  // Move the focus to the right
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == false);

  // Because no layout control in the stage and no actor is focused, it should emit the PreFocusChange signal
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mDirection == Control::KeyboardFocus::RIGHT);
  preFocusChangeCallback.Reset();

  // Check that the focus is set on the first actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == Actor());
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == first);
  focusChangedCallback.Reset();

  // Move the focus towards right
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == false);

  // Because no layout control in the stage and the first actor is focused, it should emit the PreFocusChange signal
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == first);
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mDirection == Control::KeyboardFocus::RIGHT);
  preFocusChangeCallback.Reset();

  // Check that the focus is set on the second actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(second) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == first);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == second);
  focusChangedCallback.Reset();

  // Move the focus towards up
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::UP) == false);

  // Because no layout control in the stage and no actor is focused, it should emit the PreFocusChange signal
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == second);
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mDirection == Control::KeyboardFocus::UP);
  preFocusChangeCallback.Reset();
  DALI_TEST_CHECK(!focusChangedCallback.mSignalVerified);

  // Create a 2x2 table view and try to move focus inside it
  TableView tableView = TableView::New(2, 2);
  application.GetScene().Add(tableView);

  // Create the third actor
  Actor third = Actor::New();
  third.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  // Create the fourth actor
  Actor fourth = Actor::New();
  fourth.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  // Add the four children to table view
  tableView.AddChild(first, TableView::CellPosition(0, 0));
  tableView.AddChild(second, TableView::CellPosition(0, 1));
  tableView.AddChild(third, TableView::CellPosition(1, 0));
  tableView.AddChild(fourth, TableView::CellPosition(1, 1));

  // Set the focus to the first actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == second);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == first);
  focusChangedCallback.Reset();

  // Move the focus towards right
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == first);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == second);
  focusChangedCallback.Reset();

  // Move the focus towards down
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::DOWN) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == fourth);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == second);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == fourth);
  focusChangedCallback.Reset();

  // Move the focus towards left
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::LEFT) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == fourth);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == third);
  focusChangedCallback.Reset();

  // Move the focus towards up
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::UP) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == third);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == first);
  focusChangedCallback.Reset();

  // Move the focus towards left. The focus move will fail as no way to move it upwards
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::LEFT) == false);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == first);
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mDirection == Control::KeyboardFocus::LEFT);
  preFocusChangeCallback.Reset();
  DALI_TEST_CHECK(!focusChangedCallback.mSignalVerified);

  // Enable the loop
  manager.SetFocusGroupLoop(true);
  DALI_TEST_CHECK(manager.GetFocusGroupLoop() == true);

  // Move the focus towards left again. The focus should move to the fourth actor.
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::LEFT) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == fourth);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == first);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == fourth);
  focusChangedCallback.Reset();
  END_TEST;
}

int UtcDaliKeyboardFocusManagerCustomAlgorithmMoveFocus(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerCustomAlgorithmMoveFocus");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo("KeyboardFocusManager");
  DALI_TEST_CHECK(type);
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK(handle);

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  bool                   preFocusChangeSignalVerified = false;
  PreFocusChangeCallback preFocusChangeCallback(preFocusChangeSignalVerified);
  manager.PreFocusChangeSignal().Connect(&preFocusChangeCallback, &PreFocusChangeCallback::Callback);

  bool                 focusChangedSignalVerified = false;
  FocusChangedCallback focusChangedCallback(focusChangedSignalVerified);
  manager.FocusChangedSignal().Connect(&focusChangedCallback, &FocusChangedCallback::Callback);

  // Create the first actor and add it to the stage
  Actor first = Actor::New();
  first.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(first);

  // Create the second actor and add it to the stage
  Actor second = Actor::New();
  second.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(second);

  // Move the focus to the right
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == false);

  // Because no layout control in the stage and no actor is focused, it should emit the PreFocusChange signal
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mDirection == Control::KeyboardFocus::RIGHT);
  preFocusChangeCallback.Reset();

  bool            customAlgorithmInterfaceVerified = false;
  CustomAlgorithm customAlgorithm(customAlgorithmInterfaceVerified);
  Toolkit::DevelKeyboardFocusManager::SetCustomAlgorithm(manager, customAlgorithm);

  // Move the focus towards right
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == false);

  // Because no layout control in the stage and the first actor is focused, it should invoke CustomAlgorithm
  DALI_TEST_CHECK(customAlgorithm.mInterfaceVerified);
  DALI_TEST_CHECK(customAlgorithm.mCurrentFocusedActor == Actor());
  DALI_TEST_CHECK(customAlgorithm.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(customAlgorithm.mDirection == Control::KeyboardFocus::RIGHT);
  customAlgorithm.Reset();

  // Check that the focus is set on the first actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == Actor());
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == first);
  focusChangedCallback.Reset();

  // Move the focus towards right
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == false);

  // Because no layout control in the stage and the first actor is focused, it should invoke CustomAlgorithm
  DALI_TEST_CHECK(customAlgorithm.mInterfaceVerified);
  DALI_TEST_CHECK(customAlgorithm.mCurrentFocusedActor == first);
  DALI_TEST_CHECK(customAlgorithm.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(customAlgorithm.mDirection == Control::KeyboardFocus::RIGHT);
  customAlgorithm.Reset();

  // Check that the focus is set on the second actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(second) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == first);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == second);
  focusChangedCallback.Reset();

  // Move the focus towards up
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::UP) == false);

  // Because no layout control in the stage and no actor is focused, it should invoke CustomAlgorithm
  DALI_TEST_CHECK(customAlgorithm.mInterfaceVerified);
  DALI_TEST_CHECK(customAlgorithm.mCurrentFocusedActor == second);
  DALI_TEST_CHECK(customAlgorithm.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(customAlgorithm.mDirection == Control::KeyboardFocus::UP);
  customAlgorithm.Reset();
  DALI_TEST_CHECK(!focusChangedCallback.mSignalVerified);

  END_TEST;
}
int UtcDaliKeyboardFocusManagerFocusablePropertiesMoveFocus(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerCustomAlgorithmMoveFocus");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo("KeyboardFocusManager");
  DALI_TEST_CHECK(type);
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK(handle);

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  bool                 focusChangedSignalVerified = false;
  FocusChangedCallback focusChangedCallback(focusChangedSignalVerified);
  manager.FocusChangedSignal().Connect(&focusChangedCallback, &FocusChangedCallback::Callback);

  PushButton button1 = PushButton::New();
  PushButton button2 = PushButton::New();
  button1.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  button2.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(button1);
  application.GetScene().Add(button2);

  // Set the focus to the button1
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(button1) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == button1);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == Actor());
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button1);
  focusChangedCallback.Reset();

  // set the navigation properties of button1
  button1.SetProperty(Toolkit::DevelControl::Property::LEFT_FOCUSABLE_ACTOR_ID, Property::Value((int)button2.GetProperty<int>(Actor::Property::ID)));
  button1.SetProperty(Toolkit::DevelControl::Property::RIGHT_FOCUSABLE_ACTOR_ID, Property::Value((int)button2.GetProperty<int>(Actor::Property::ID)));
  button1.SetProperty(Toolkit::DevelControl::Property::UP_FOCUSABLE_ACTOR_ID, Property::Value((int)button2.GetProperty<int>(Actor::Property::ID)));
  button1.SetProperty(Toolkit::DevelControl::Property::DOWN_FOCUSABLE_ACTOR_ID, Property::Value((int)button2.GetProperty<int>(Actor::Property::ID)));

  // set the navigation properties of button2
  button2.SetProperty(Toolkit::DevelControl::Property::LEFT_FOCUSABLE_ACTOR_ID, Property::Value((int)button1.GetProperty<int>(Actor::Property::ID)));
  button2.SetProperty(Toolkit::DevelControl::Property::RIGHT_FOCUSABLE_ACTOR_ID, Property::Value((int)button1.GetProperty<int>(Actor::Property::ID)));
  button2.SetProperty(Toolkit::DevelControl::Property::UP_FOCUSABLE_ACTOR_ID, Property::Value((int)button1.GetProperty<int>(Actor::Property::ID)));
  button2.SetProperty(Toolkit::DevelControl::Property::DOWN_FOCUSABLE_ACTOR_ID, Property::Value((int)button1.GetProperty<int>(Actor::Property::ID)));

  // Move the focus towards left
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::LEFT) == true);

  // Confirm whether focus is moved to button2
  DALI_TEST_EQUALS(button2.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == button1);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button2);
  focusChangedCallback.Reset();

  // Move the focus towards right
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == true);

  // Confirm whether focus is moved to button1
  DALI_TEST_EQUALS(button1.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == button2);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button1);
  focusChangedCallback.Reset();

  // Move the focus towards up
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::UP) == true);

  // Confirm whether focus is moved to button2
  DALI_TEST_EQUALS(button2.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == button1);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button2);
  focusChangedCallback.Reset();

  // Move the focus towards down
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::DOWN) == true);

  // Confirm whether focus is moved to button1
  DALI_TEST_EQUALS(button1.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == button2);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button1);
  focusChangedCallback.Reset();

  // Create a 1x1 table view and try to move focus inside it
  TableView tableView = TableView::New(1, 1);
  application.GetScene().Add(tableView);

  PushButton button = PushButton::New();
  button.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  tableView.AddChild(button, TableView::CellPosition(0, 0));

  // set the navigation properties of button3
  button.SetProperty(Toolkit::DevelControl::Property::LEFT_FOCUSABLE_ACTOR_ID, Property::Value((int)button1.GetProperty<int>(Actor::Property::ID)));

  // Set the focus to the button
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(button) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == button);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == button1);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button);
  focusChangedCallback.Reset();

  // Move the focus towards left
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::LEFT) == true);

  // Confirm whether focus is moved to button1
  DALI_TEST_EQUALS(button1.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == button);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button1);
  focusChangedCallback.Reset();

  END_TEST;
}

int UtcDaliKeyboardFocusManagerClearFocus(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerClearFocus");

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Create the first actor and add it to the stage
  Actor first = Actor::New();
  first.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(first);

  // Create the second actor and add it to the stage
  Actor second = Actor::New();
  second.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(second);

  // Check that the focus is set on the first actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);

  // Check that the focus is set on the second actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(second) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);

  // Clear the focus
  manager.ClearFocus();

  // Check that no actor is being focused now.
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());
  END_TEST;
}

int UtcDaliKeyboardFocusManagerSetAndGetFocusGroupLoop(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerSetAndGetFocusGroupLoop");

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Check that the focus movement is not looped within the same focus group by default
  DALI_TEST_CHECK(manager.GetFocusGroupLoop() == false);

  // Enable the loop
  manager.SetFocusGroupLoop(true);
  DALI_TEST_CHECK(manager.GetFocusGroupLoop() == true);
  END_TEST;
}

int UtcDaliKeyboardFocusManagerSetAsFocusGroup(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerSetAsFocusGroup");

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Create an actor and check that it is not a focus group by default
  Actor actor = Actor::New();
  DALI_TEST_CHECK(manager.IsFocusGroup(actor) == false);

  // Set the actor as focus group
  manager.SetAsFocusGroup(actor, true);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(manager.IsFocusGroup(actor) == true);

  // Set the actor not as focus group
  manager.SetAsFocusGroup(actor, false);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(manager.IsFocusGroup(actor) == false);
  END_TEST;
}

int UtcDaliKeyboardFocusManagerGetFocusGroup(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerGetFocusGroup");

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Create an actor with two child actors and add it to the stage
  Actor parent = Actor::New();
  Actor child  = Actor::New();
  parent.Add(child);
  application.GetScene().Add(parent);

  // Create three actors and add them as the children of the first child actor
  Actor grandChild = Actor::New();
  child.Add(grandChild);

  // Set the parent and the first child actor as focus groups
  manager.SetAsFocusGroup(parent, true);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(manager.IsFocusGroup(parent) == true);

  // The current focus group should be the parent, As it is the immediate parent which is also a focus group.
  DALI_TEST_CHECK(manager.GetFocusGroup(grandChild) == parent);

  manager.SetAsFocusGroup(child, true);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(manager.IsFocusGroup(child) == true);

  // The focus group should be the child, As it is the immediate parent which is also a focus group.
  DALI_TEST_CHECK(manager.GetFocusGroup(grandChild) == child);

  manager.SetAsFocusGroup(grandChild, true);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(manager.IsFocusGroup(grandChild) == true);

  // The current focus group should be itself, As it is also a focus group.
  DALI_TEST_CHECK(manager.GetFocusGroup(grandChild) == grandChild);
  END_TEST;
}

int UtcDaliKeyboardFocusManagerSetAndGetFocusIndicator(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerSetAndGetFocusIndicator");

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  Actor defaultFocusIndicatorActor = manager.GetFocusIndicatorActor();
  DALI_TEST_CHECK(defaultFocusIndicatorActor);

  Actor newFocusIndicatorActor = Actor::New();
  manager.SetFocusIndicatorActor(newFocusIndicatorActor);
  DALI_TEST_CHECK(manager.GetFocusIndicatorActor() == newFocusIndicatorActor);
  END_TEST;
}

int UtcDaliKeyboardFocusManagerSignalFocusedActorActivated(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerSignalFocusedActorActivated");

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  bool                          focusedActorActivatedSignalVerified = false;
  FocusedActorActivatedCallback focusedActorActivatedCallback(focusedActorActivatedSignalVerified);
  manager.FocusedActorEnterKeySignal().Connect(&focusedActorActivatedCallback, &FocusedActorActivatedCallback::Callback);

  Integration::KeyEvent returnEvent("Return", "", "", 0, 0, 0, Integration::KeyEvent::UP, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);

  // Press Any key to notice physical keyboard event is comming to KeyboardFocusManager
  // It makes mIsFocusIndicatorEnabled true
  application.ProcessEvent(returnEvent);

  // Create the first button and add it to the stage
  PushButton firstPushButton = PushButton::New();
  firstPushButton.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(firstPushButton);

  // Create the second button and add it to the stage
  PushButton secondPushButton = PushButton::New();
  secondPushButton.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(secondPushButton);

  // Check that the focus is set on the first button
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(firstPushButton) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == firstPushButton);

  // Send the return event to activate the first button
  application.ProcessEvent(returnEvent);
  DALI_TEST_CHECK(focusedActorActivatedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusedActorActivatedCallback.mActivatedActor == firstPushButton);
  focusedActorActivatedCallback.Reset();

  // Check that the focus is set on the second button
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(secondPushButton) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == secondPushButton);

  // Send the return event again to activate the second button
  application.ProcessEvent(returnEvent);
  DALI_TEST_CHECK(focusedActorActivatedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusedActorActivatedCallback.mActivatedActor == secondPushButton);
  focusedActorActivatedCallback.Reset();
  END_TEST;
}

int UtcDaliKeyboardFocusManagerSignalFocusGroupChanged(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerSignalFocusGroupChanged");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo("KeyboardFocusManager");
  DALI_TEST_CHECK(type);
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK(handle);

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  bool                      focusGroupChangedSignalVerified = false;
  FocusGroupChangedCallback focusGroupChangedCallback(focusGroupChangedSignalVerified);
  manager.FocusGroupChangedSignal().Connect(&focusGroupChangedCallback, &FocusGroupChangedCallback::Callback);

  Integration::KeyEvent tabEvent("Tab", "", "", 0, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  Integration::KeyEvent shiftTabEvent("Tab", "", "", 0, 1, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);

  // Press Any key to notice physical keyboard event is comming to KeyboardFocusManager
  // It makes mIsFocusIndicatorEnabled true
  application.ProcessEvent(tabEvent);

  // Send the tab event to change focus group in the forward direction
  application.ProcessEvent(tabEvent);
  DALI_TEST_CHECK(focusGroupChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusGroupChangedCallback.mCurrentFocusedActor == Actor());
  DALI_TEST_CHECK(focusGroupChangedCallback.mForward == true);
  focusGroupChangedCallback.Reset();

  // Send the shift tab event to change focus group in the backward direction
  application.ProcessEvent(shiftTabEvent);
  DALI_TEST_CHECK(focusGroupChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusGroupChangedCallback.mCurrentFocusedActor == Actor());
  DALI_TEST_CHECK(focusGroupChangedCallback.mForward == false);
  focusGroupChangedCallback.Reset();
  END_TEST;
}

int UtcDaliKeyboardFocusManagerSignals(void)
{
  ToolkitTestApplication application;

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  ConnectionTracker* testTracker = new ConnectionTracker();
  DALI_TEST_EQUALS(true, manager.ConnectSignal(testTracker, "keyboardPreFocusChange", CallbackFunctor()), TEST_LOCATION);
  DALI_TEST_EQUALS(true, manager.ConnectSignal(testTracker, "keyboardFocusChanged", CallbackFunctor()), TEST_LOCATION);
  DALI_TEST_EQUALS(true, manager.ConnectSignal(testTracker, "keyboardFocusGroupChanged", CallbackFunctor()), TEST_LOCATION);
  DALI_TEST_EQUALS(true, manager.ConnectSignal(testTracker, "keyboardFocusedActorEnterKey", CallbackFunctor()), TEST_LOCATION);

  END_TEST;
}

int UtcDaliKeyboardFocusManagerMoveFocusBackward(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerMoveFocusBackward");

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Create the first actor and add it to the stage
  Actor first = Actor::New();
  first.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(first);

  // Create the second actor and add it to the stage
  Actor second = Actor::New();
  second.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(second);

  // Create the third actor and add it to the stage
  Actor third = Actor::New();
  third.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(third);

  // Create the fourth actor and add it to the stage
  Actor fourth = Actor::New();
  fourth.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(fourth);

  // Check that the focus is set on the second actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);

  // Check that the focus is set on the second actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(second) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);

  // Check that the focus is set on the third  actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(third) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);

  // Check that the focus is set on the third  actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(fourth) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == fourth);

  // Move the focus backward
  manager.MoveFocusBackward();

  // Check that it current focused actor is third actor
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);

  // Remove the second actor on stage
  second.Unparent();

  // Reset the first actor
  first.Unparent();
  first.Reset();

  // Move the focus backward
  manager.MoveFocusBackward();

  // Check that it current focused actor is third actor
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);

  // Make history stack full
  for(int i = 0; i < 31; i++)
  {
    Actor actor = Actor::New();
    actor.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
    application.GetScene().Add(actor);
    manager.SetCurrentFocusActor(actor);
  }

  for(int i = 0; i < 31; i++)
  {
    manager.MoveFocusBackward();
  }

  // Check that it current focused actor is not second actor
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() != second);

  END_TEST;
}

int UtcDaliKeyboardFocusManagerChangeFocusDirectionByKeyEvents(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerChangeFocusDirectionByKeyEvents");

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  bool                   preFocusChangeSignalVerified = false;
  PreFocusChangeCallback preFocusChangeCallback(preFocusChangeSignalVerified);
  manager.PreFocusChangeSignal().Connect(&preFocusChangeCallback, &PreFocusChangeCallback::Callback);

  bool                 focusChangedSignalVerified = false;
  FocusChangedCallback focusChangedCallback(focusChangedSignalVerified);
  manager.FocusChangedSignal().Connect(&focusChangedCallback, &FocusChangedCallback::Callback);

  Integration::KeyEvent leftEvent("Left", "", "", 0, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  Integration::KeyEvent rightEvent("Right", "", "", 0, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  Integration::KeyEvent upEvent("Up", "", "", 0, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  Integration::KeyEvent downEvent("Down", "", "", 0, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  Integration::KeyEvent pageUpEvent("Prior", "", "", 0, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  Integration::KeyEvent pageDownEvent("Next", "", "", 0, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);

  // Press Any key to notice physical keyboard event is comming to KeyboardFocusManager
  // It makes mIsFocusIndicatorEnabled true
  application.ProcessEvent(leftEvent);

  // Create a 2x2 table view and try to move focus inside it
  TableView tableView = TableView::New(2, 2);
  application.GetScene().Add(tableView);

  // Create the first actor
  Actor first = Actor::New();
  first.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  // Create the second actor
  Actor second = Actor::New();
  second.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  // Create the third actor
  Actor third = Actor::New();
  third.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  // Create the fourth actor
  Actor fourth = Actor::New();
  fourth.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  // Add the four children to table view
  tableView.AddChild(first, TableView::CellPosition(0, 0));
  tableView.AddChild(second, TableView::CellPosition(0, 1));
  tableView.AddChild(third, TableView::CellPosition(1, 0));
  tableView.AddChild(fourth, TableView::CellPosition(1, 1));

  // Set the focus to the first actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == Actor());
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == first);
  focusChangedCallback.Reset();

  // Send the right key event to move the focus towards right
  application.ProcessEvent(rightEvent);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == first);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == second);
  focusChangedCallback.Reset();

  // Send the down key event to move the focus towards down
  application.ProcessEvent(downEvent);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == fourth);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == second);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == fourth);
  focusChangedCallback.Reset();

  // Send the down event to move the focus towards left
  application.ProcessEvent(leftEvent);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == fourth);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == third);
  focusChangedCallback.Reset();

  // Send the up event to move the focus towards up
  application.ProcessEvent(upEvent);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == third);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == first);
  focusChangedCallback.Reset();

  // Send the pape up event, but focus should not be moved because page up is not supported by table view
  application.ProcessEvent(pageUpEvent);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == first);
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == first);
  DALI_TEST_CHECK(preFocusChangeCallback.mDirection == Control::KeyboardFocus::PAGE_UP);
  preFocusChangeCallback.Reset();

  // Send the pape down event, but focus should not be moved because page down is not supported by table view
  application.ProcessEvent(pageDownEvent);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == first);
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == first);
  DALI_TEST_CHECK(preFocusChangeCallback.mDirection == Control::KeyboardFocus::PAGE_DOWN);
  preFocusChangeCallback.Reset();

  // Clear the focus
  manager.ClearFocus();

  // Send the pape up event, but nothing was focued so focus manager will try the initial focus
  preFocusChangeCallback.Reset();
  application.ProcessEvent(pageUpEvent);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mDirection == Control::KeyboardFocus::RIGHT);

  // Clear the focus again
  manager.ClearFocus();

  // Send the pape down event, but nothing was focued so focus manager will try the initial focus
  preFocusChangeCallback.Reset();
  application.ProcessEvent(pageDownEvent);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mDirection == Control::KeyboardFocus::RIGHT);

  // Clear the focus again
  manager.ClearFocus();

  // Send the up event for line coverage, but nothing was focued so focus manager will try the initial focus
  preFocusChangeCallback.Reset();
  application.ProcessEvent(upEvent);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());

  // Clear the focus again
  manager.ClearFocus();

  // Send the down event for line coverage, but nothing was focued so focus manager will try the initial focus
  preFocusChangeCallback.Reset();
  application.ProcessEvent(downEvent);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());

  END_TEST;
}

int UtcDaliKeyboardFocusManagerSignalChangedBySpaceKeyEvent(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerSignalChangedBySpaceKeyEvent");

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  bool                   preFocusChangeSignalVerified = false;
  PreFocusChangeCallback preFocusChangeCallback(preFocusChangeSignalVerified);
  manager.PreFocusChangeSignal().Connect(&preFocusChangeCallback, &PreFocusChangeCallback::Callback);

  Integration::KeyEvent spaceEvent("space", "", "", 0, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);

  // Press Any key to notice physical keyboard event is comming to KeyboardFocusManager
  // It makes mIsFocusIndicatorEnabled true
  application.ProcessEvent(spaceEvent);

  // Send the space event
  application.ProcessEvent(spaceEvent);
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == Actor());

  // Clear the focus again
  manager.ClearFocus();

  // Send the space event again for line coverage
  preFocusChangeCallback.Reset();
  application.ProcessEvent(spaceEvent);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());

  END_TEST;
}

int UtcDaliKeyboardFocusManagerMoveFocusTestStateChange(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerMoveFocusTestStateChange");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo("KeyboardFocusManager");
  DALI_TEST_CHECK(type);
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK(handle);

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  bool                   preFocusChangeSignalVerified = false;
  PreFocusChangeCallback preFocusChangeCallback(preFocusChangeSignalVerified);
  manager.PreFocusChangeSignal().Connect(&preFocusChangeCallback, &PreFocusChangeCallback::Callback);

  bool                 focusChangedSignalVerified = false;
  FocusChangedCallback focusChangedCallback(focusChangedSignalVerified);
  manager.FocusChangedSignal().Connect(&focusChangedCallback, &FocusChangedCallback::Callback);

  // Create the first actor and add it to the stage
  Control first = Control::New();
  first.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(first);

  // Create the second actor and add it to the stage
  Control second = Control::New();
  second.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(second);

  // Move the focus to the right
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == false);

  // Because no layout control in the stage and no actor is focused, it should emit the PreFocusChange signal
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mDirection == Control::KeyboardFocus::RIGHT);
  preFocusChangeCallback.Reset();

  // Check that the focus is set on the first actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == Actor());
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == first);
  DALI_TEST_EQUALS(first.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  focusChangedCallback.Reset();

  // Move the focus towards right
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == false);

  // Because no layout control in the stage and the first actor is focused, it should emit the PreFocusChange signal
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == first);
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mDirection == Control::KeyboardFocus::RIGHT);
  preFocusChangeCallback.Reset();

  // Check that the focus is set on the second actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(second) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == first);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == second);
  DALI_TEST_EQUALS(first.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  focusChangedCallback.Reset();

  // Move the focus towards up
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::UP) == false);

  // Because no layout control in the stage and no actor is focused, it should emit the PreFocusChange signal
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == second);
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mDirection == Control::KeyboardFocus::UP);
  preFocusChangeCallback.Reset();
  DALI_TEST_CHECK(!focusChangedCallback.mSignalVerified);

  // Create a 2x2 table view and try to move focus inside it
  TableView tableView = TableView::New(2, 2);
  application.GetScene().Add(tableView);

  // Create the third actor
  Control third = Control::New();
  third.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  // Create the fourth actor
  Control fourth = Control::New();
  fourth.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  // Add the four children to table view
  tableView.AddChild(first, TableView::CellPosition(0, 0));
  tableView.AddChild(second, TableView::CellPosition(0, 1));
  tableView.AddChild(third, TableView::CellPosition(1, 0));
  tableView.AddChild(fourth, TableView::CellPosition(1, 1));

  // Set the focus to the first actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == second);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == first);

  DALI_TEST_EQUALS(first.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);

  focusChangedCallback.Reset();

  // Move the focus towards right
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == first);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == second);
  DALI_TEST_EQUALS(first.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);

  focusChangedCallback.Reset();

  // Move the focus towards down
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::DOWN) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == fourth);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == second);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == fourth);

  DALI_TEST_EQUALS(first.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(third.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(fourth.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);

  focusChangedCallback.Reset();

  // Move the focus towards left
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::LEFT) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == fourth);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == third);

  DALI_TEST_EQUALS(first.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(third.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_EQUALS(fourth.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);

  focusChangedCallback.Reset();

  // Move the focus towards up
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::UP) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == third);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == first);
  DALI_TEST_EQUALS(first.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(third.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(fourth.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  focusChangedCallback.Reset();

  // Move the focus towards left. The focus move will fail as no way to move it upwards
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::LEFT) == false);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == first);
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mDirection == Control::KeyboardFocus::LEFT);
  DALI_TEST_EQUALS(first.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(third.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(fourth.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);

  preFocusChangeCallback.Reset();
  DALI_TEST_CHECK(!focusChangedCallback.mSignalVerified);

  // Enable the loop
  manager.SetFocusGroupLoop(true);
  DALI_TEST_CHECK(manager.GetFocusGroupLoop() == true);

  // Move the focus towards left again. The focus should move to the fourth actor.
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::LEFT) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == fourth);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == first);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == fourth);

  DALI_TEST_EQUALS(first.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(third.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(fourth.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);

  focusChangedCallback.Reset();

  // Clear the focus
  manager.ClearFocus();
  DALI_TEST_EQUALS(first.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(third.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(fourth.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::NORMAL, TEST_LOCATION);

  END_TEST;
}

int UtcDaliKeyboardFocusManagerFocusedActorUnstaged(void)
{
  ToolkitTestApplication application;

  tet_infoline("Ensure we cannot set an actor to be focused if it is not staged and that we do not retrieve an actor if it has been unstaged");

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(!manager.GetCurrentFocusActor());

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  tet_infoline("Attempt to set unstaged actor, no actor should be returned from KeyboardFocusManager");
  manager.SetCurrentFocusActor(actor);
  DALI_TEST_CHECK(!manager.GetCurrentFocusActor());

  tet_infoline("Add actor to stage and attempt to set, our actor should be returned from KeyboardFocusManager");
  application.GetScene().Add(actor);
  manager.SetCurrentFocusActor(actor);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == actor);

  tet_infoline("Remove actor from stage and attempt to retrieve, no actor should be returned from KeyboardFocusManager");
  actor.Unparent();
  DALI_TEST_CHECK(!manager.GetCurrentFocusActor());

  END_TEST;
}

int UtcDaliKeyboardFocusManagerEnableFocusIndicator(void)
{
  ToolkitTestApplication application;

  tet_infoline("Ensure we cannot set an actor to be focused if it is not staged and that we do not retrieve an actor if it has been unstaged");

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(!manager.GetCurrentFocusActor());

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(actor);
  manager.SetCurrentFocusActor(actor);

  // Press Any key to notice physical keyboard event is comming to KeyboardFocusManager
  // It makes mIsFocusIndicatorEnabled true and add focus indicator to focused actor.
  Integration::KeyEvent rightEvent("Right", "", "", 0, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(rightEvent);

  Actor indicatorActor = manager.GetFocusIndicatorActor();

  tet_infoline("Indicator is added to focused actor");
  DALI_TEST_CHECK(actor == indicatorActor.GetParent());

  Dali::Toolkit::DevelKeyboardFocusManager::EnableFocusIndicator(manager, false);
  DALI_TEST_CHECK(!Dali::Toolkit::DevelKeyboardFocusManager::IsFocusIndicatorEnabled(manager));

  tet_infoline("Indicator is removed from focused actor because mUseFocusIndicator is false");
  DALI_TEST_CHECK(!indicatorActor.GetParent());

  END_TEST;
}

int UtcDaliKeyboardFocusManagerCheckConsumedKeyEvent(void)
{
  ToolkitTestApplication application;

  tet_infoline("Ensure Window can't receive KeyEvent when Control already consumed it");
  Dali::Integration::Scene scene = application.GetScene();

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(!manager.GetCurrentFocusActor());

  // Create the first actor and add it to the stage
  Control control = Control::New();
  control.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  scene.Add(control);

  KeyEventCallback controlCallback(true);
  control.KeyEventSignal().Connect(&controlCallback, &KeyEventCallback::Callback);

  KeyEventCallback sceneCallback(false);
  scene.KeyEventSignal().Connect(&sceneCallback, &KeyEventCallback::Callback);

  manager.SetCurrentFocusActor(control);

  // Press Any key to notice physical keyboard event is comming to KeyboardFocusManager
  // It makes mIsFocusIndicatorEnabled true and add focus indicator to focused actor.
  Integration::KeyEvent event1("Right", "", "", 0, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(event1);

  DALI_TEST_CHECK(controlCallback.mIsCalled);
  DALI_TEST_CHECK(!sceneCallback.mIsCalled);

  END_TEST;
}

int UtcDaliKeyboardFocusManagerFocusPerWindow(void)
{
  ToolkitTestApplication application;

  tet_infoline("Ensure Memory focus actors for each window ");
  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(!manager.GetCurrentFocusActor());

  Window firstWindow = Window::New(PositionSize(0, 0, 300, 500), "", false);
  DALI_TEST_CHECK(firstWindow);
  Control first = Control::New();
  first.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  firstWindow.Add(first);

  Window secondWindow = Window::New(PositionSize(0, 0, 400, 600), "", false);
  DALI_TEST_CHECK(secondWindow);
  Control second = Control::New();
  second.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  secondWindow.Add(second);

  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);

  DALI_TEST_CHECK(manager.SetCurrentFocusActor(second) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);
  firstWindow.Raise();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);

  secondWindow.Remove(second);
  secondWindow.Raise();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() != second);

  secondWindow.Reset();
  END_TEST;
}

int UtcDaliKeyboardFocusManagerWithoutFocusablePropertiesMoveFocus(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerWithoutFocusablePropertiesMoveFocus");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo("KeyboardFocusManager");
  DALI_TEST_CHECK(type);
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK(handle);

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  bool                 focusChangedSignalVerified = false;
  FocusChangedCallback focusChangedCallback(focusChangedSignalVerified);
  manager.FocusChangedSignal().Connect(&focusChangedCallback, &FocusChangedCallback::Callback);

  PushButton button1 = PushButton::New();
  PushButton button2 = PushButton::New();
  PushButton button3 = PushButton::New();
  PushButton button4 = PushButton::New();
  PushButton button5 = PushButton::New();

  button1.SetProperty(Actor::Property::SIZE, Vector2(50, 50));
  button2.SetProperty(Actor::Property::SIZE, Vector2(50, 50));
  button3.SetProperty(Actor::Property::SIZE, Vector2(50, 50));
  button4.SetProperty(Actor::Property::SIZE, Vector2(50, 50));
  button5.SetProperty(Actor::Property::SIZE, Vector2(50, 50));

  button1.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  button2.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  button3.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  button4.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  button5.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  application.GetScene().Add(button1);
  application.GetScene().Add(button2);
  application.GetScene().Add(button3);
  button5.Add(button4);
  application.GetScene().Add(button5);

  // set position
  // button1 -- button2
  //   |           |
  //   |    button5|
  // button3 -- button4
  button1.SetProperty(Actor::Property::POSITION, Vector2(0.0f, 0.0f));
  button2.SetProperty(Actor::Property::POSITION, Vector2(100.0f, 0.0f));
  button3.SetProperty(Actor::Property::POSITION, Vector2(0.0f, 100.0f));
  button4.SetProperty(Actor::Property::POSITION, Vector2(40.0f, 40.0f));
  button5.SetProperty(Actor::Property::POSITION, Vector2(60.0f, 60.0f));

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Set the focus to the button1
  // [button1] -- button2
  //   |           |
  //   |    button5|
  // button3 -- button4
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(button1) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == button1);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == Actor());
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button1);
  focusChangedCallback.Reset();

  // without set the navigation properties, but we can focus move
  // enable the default algorithm
  Dali::Toolkit::DevelKeyboardFocusManager::EnableDefaultAlgorithm(manager, true);
  DALI_TEST_CHECK(Dali::Toolkit::DevelKeyboardFocusManager::IsDefaultAlgorithmEnabled(manager));

  // Move the focus towards right
  // button1 -- [button2]
  //   |           |
  //   |    button5|
  // button3 -- button4
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == true);

  // Confirm whether focus is moved to button2
  DALI_TEST_EQUALS(button2.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == button1);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button2);
  focusChangedCallback.Reset();

  // Move the focus towards down
  // button1 -- button2
  //   |           |
  //   |  [button5]|
  // button3 -- button4
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::DOWN) == true);

  // Confirm whether focus is moved to button5
  DALI_TEST_EQUALS(button5.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == button2);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button5);
  focusChangedCallback.Reset();

  // Move the focus towards right
  // button1 -- button2
  //   |           |
  //   |    button5|
  // button3 -- [button4]
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == true);

  // Confirm whether focus is moved to button4
  DALI_TEST_EQUALS(button4.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == button5);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button4);
  focusChangedCallback.Reset();

  // Move the focus towards left
  // button1 -- button2
  //   |           |
  //   |  [button5]|
  // button3 -- button4
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::LEFT) == true);

  // Confirm whether focus is moved to button5
  DALI_TEST_EQUALS(button5.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == button4);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button5);
  focusChangedCallback.Reset();

  // Move the focus towards left
  // button1 -- button2
  //   |           |
  //   |    button5|
  //[button3] -- button4
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::LEFT) == true);

  // Confirm whether focus is moved to button3
  DALI_TEST_EQUALS(button3.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == button5);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button3);
  focusChangedCallback.Reset();

  // Move the focus towards right
  // button1 -- button2
  //   |           |
  //   |  [button5]|
  // button3 -- button4
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == true);

  // Confirm whether focus is moved to button5
  DALI_TEST_EQUALS(button5.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == button3);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button5);
  focusChangedCallback.Reset();

  // Move the focus towards left
  // button1 -- button2
  //   |           |
  //   |    button5|
  //[button3] -- button4
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::LEFT) == true);

  // Confirm whether focus is moved to button3
  DALI_TEST_EQUALS(button3.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == button5);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button3);
  focusChangedCallback.Reset();

  // Move the focus towards up
  //[button1]-- button2
  //   |           |
  //   |    button5|
  // button3 -- button4
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::UP) == true);

  // Confirm whether focus is moved to button1
  DALI_TEST_EQUALS(button1.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == button3);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button1);
  focusChangedCallback.Reset();

  // Move the focus towards left. The focus move will fail as no way to move it upwards
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::LEFT) == false);

  // Move the focus toward page up/down. The focus move will fail as invalid direction.
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::PAGE_UP) == false);
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::PAGE_DOWN) == false);
  focusChangedCallback.Reset();

  END_TEST;
}

int UtcDaliKeyboardFocusManagerSetAndGetCurrentFocusActorInTouchMode(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerSetAndGetCurrentFocusActorInTouchMode");

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Create the first actor and add it to the stage
  Actor first = Actor::New();
  first.SetProperty(Actor::Property::SIZE, Vector2(50, 50));
  first.SetProperty(Actor::Property::POSITION, Vector2(0.0f, 0.0f));
  first.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  first.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  first.SetProperty(DevelActor::Property::TOUCH_FOCUSABLE, true);
  application.GetScene().Add(first);

  // Create the second actor and add it to the stage
  Actor second = Actor::New();
  second.SetProperty(Actor::Property::SIZE, Vector2(50, 50));
  second.SetProperty(Actor::Property::POSITION, Vector2(100.0f, 0.0f));
  second.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  second.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(second);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that no actor is being focused yet.
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());

  // Check that it will fail to set focus on an invalid actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(Actor()) == false);

  Dali::Integration::TouchEvent event1 = Dali::Integration::TouchEvent();
  Dali::Integration::Point      pointDown1;
  pointDown1.SetState(PointState::DOWN);
  pointDown1.SetDeviceId(1);
  // touch first actor
  pointDown1.SetScreenPosition(Vector2(10.0f, 10.0f));
  event1.AddPoint(pointDown1);
  application.ProcessEvent(event1);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the focus is successfully to the first actor
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);

  Dali::Integration::TouchEvent event2 = Dali::Integration::TouchEvent();
  Dali::Integration::Point      pointDown2;
  pointDown2.SetState(PointState::DOWN);
  pointDown2.SetDeviceId(1);
  // touch second actor
  pointDown2.SetScreenPosition(Vector2(110.0f, 10.0f));
  event2.AddPoint(pointDown2);
  application.ProcessEvent(event2);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the focus is successfully to clear
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());

  // Make the second actor focusableInTouchMode
  second.SetProperty(DevelActor::Property::TOUCH_FOCUSABLE, true);

  // touch second actor
  application.ProcessEvent(event2);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the focus is successfully to the second actor
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);

  END_TEST;
}

int UtcDaliKeyboardFocusManagerEnableDefaultAlgorithm(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerEnableDefaultAlgorithm");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo("KeyboardFocusManager");
  DALI_TEST_CHECK(type);
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK(handle);

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  bool                 focusChangedSignalVerified = false;
  FocusChangedCallback focusChangedCallback(focusChangedSignalVerified);
  manager.FocusChangedSignal().Connect(&focusChangedCallback, &FocusChangedCallback::Callback);

  PushButton button1 = PushButton::New();
  PushButton button2 = PushButton::New();

  button1.SetProperty(Actor::Property::SIZE, Vector2(50, 50));
  button2.SetProperty(Actor::Property::SIZE, Vector2(50, 50));

  button1.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  button2.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  application.GetScene().Add(button1);
  application.GetScene().Add(button2);

  // set position
  // button1 -- button2
  button1.SetProperty(Actor::Property::POSITION, Vector2(0.0f, 0.0f));
  button2.SetProperty(Actor::Property::POSITION, Vector2(100.0f, 0.0f));
  button1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  button2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Set the focus to the button1
  // [button1] -- button2
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(button1) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == button1);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == Actor());
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button1);
  focusChangedCallback.Reset();

  // without set the navigation properties, but we can focus move
  // enable the default algorithm
  Dali::Toolkit::DevelKeyboardFocusManager::EnableDefaultAlgorithm(manager, true);
  DALI_TEST_CHECK(Dali::Toolkit::DevelKeyboardFocusManager::IsDefaultAlgorithmEnabled(manager));

  // Move the focus towards right
  // button1 -- [button2]
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == true);

  // Confirm whether focus is moved to button2
  DALI_TEST_EQUALS(button2.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == button1);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button2);
  focusChangedCallback.Reset();

  // disable the default algorithm
  Dali::Toolkit::DevelKeyboardFocusManager::EnableDefaultAlgorithm(manager, false);
  DALI_TEST_CHECK(!Dali::Toolkit::DevelKeyboardFocusManager::IsDefaultAlgorithmEnabled(manager));

  // Move the focus towards left, The focus move will fail because the default algorithm is disabled.
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::LEFT) == false);

  // enable the default algorithm
  Dali::Toolkit::DevelKeyboardFocusManager::EnableDefaultAlgorithm(manager, true);
  DALI_TEST_CHECK(Dali::Toolkit::DevelKeyboardFocusManager::IsDefaultAlgorithmEnabled(manager));

  // Move the focus towards left, The focus move will success because the default algorithm is enabled.
  // [button1] -- button2
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::LEFT) == true);
  // Confirm whether focus is moved to button1
  DALI_TEST_EQUALS(button1.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == button2);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button1);
  focusChangedCallback.Reset();

  // Clears focus.
  manager.ClearFocus();
  // There is no actor focused.
  // button1 -- button2
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());

  // Move the focus towards right, The focus is on the actor closest to the top left of the window.
  // [button1] -- button2
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == true);

  // Confirm whether focus is moved to button1
  DALI_TEST_EQUALS(button1.GetProperty<int>(DevelControl::Property::STATE), (int)DevelControl::FOCUSED, TEST_LOCATION);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == Actor());
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == button1);
  focusChangedCallback.Reset();


  END_TEST;
}

int UtcDaliKeyboardFocusManagerWithKeyboardFocusableChildren(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerWithKeyboardFocusableChildren");

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Create the first actor and add it to the stage
  Actor first = Actor::New();
  first.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(first);

  // Create the second actor and add it to the first actor.
  Actor second = Actor::New();
  second.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  first.Add(second);

  // Check that no actor is being focused yet.
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());

  // Check that the focus is set on the first actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);

  // Set KeyboardFocusableChildren false.
  first.SetProperty(DevelActor::Property::KEYBOARD_FOCUSABLE_CHILDREN, false);

  // Check that it will fail to set focus on the second actor as it's not focusable
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(second) == false);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);

  // Set KeyboardFocusableChildren true.
  first.SetProperty(DevelActor::Property::KEYBOARD_FOCUSABLE_CHILDREN, true);

  // Check that the focus is set on the second actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(second) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);

  END_TEST;
}

int UtcDaliKeyboardFocusManagerCheckWheelEvent(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliKeyboardFocusManagerCheckWheelEvent");
  Dali::Integration::Scene scene = application.GetScene();

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(!manager.GetCurrentFocusActor());

  // Create the first actor and add it to the stage
  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  Actor child = Actor::New();
  child.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  parent.Add(child);
  scene.Add(parent);

  WheelEventCallback childCallback(false);
  child.WheelEventSignal().Connect(&childCallback, &WheelEventCallback::Callback);

  WheelEventCallback parentCallback(true);
  parent.WheelEventSignal().Connect(&parentCallback, &WheelEventCallback::Callback);

  WheelEventCallback sceneCallback(false);
  scene.WheelEventSignal().Connect(&sceneCallback, &WheelEventCallback::Callback);

  manager.SetCurrentFocusActor(child);

  // Emit custom wheel event is comming to KeyboardFocusManager
  Integration::WheelEvent event(Integration::WheelEvent::CUSTOM_WHEEL, 0, 0u, Vector2(0.0f, 0.0f), 1, 1000u);
  application.ProcessEvent(event);

  DALI_TEST_CHECK(childCallback.mIsCalled);
  DALI_TEST_CHECK(parentCallback.mIsCalled);
  DALI_TEST_CHECK(!sceneCallback.mIsCalled);

  END_TEST;
}
