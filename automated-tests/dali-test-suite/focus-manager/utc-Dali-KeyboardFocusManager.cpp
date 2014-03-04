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
using namespace Dali::Toolkit;

namespace
{

// Functors to test whether PreFocusChange signal is emitted when the keyboard focus is about to change
class PreFocusChangeCallback : public Dali::ConnectionTracker
{
public:
  PreFocusChangeCallback(bool& signalReceived)
  : mSignalVerified(signalReceived),
    mCurrentFocusedActor(),
    mProposedActorToFocus(),
    mDirection(Control::Left)
  {
  }

  Actor Callback(Actor currentFocusedActor, Actor proposedActorToFocus, Control::KeyboardFocusNavigationDirection direction)
  {
    tet_infoline("Verifying PreFocusChangeCallback()");

    mSignalVerified = true;

    mCurrentFocusedActor = currentFocusedActor;
    mProposedActorToFocus = proposedActorToFocus;
    mDirection = direction;

    return mProposedActorToFocus;
  }

  void Reset()
  {
    mSignalVerified = false;
    mCurrentFocusedActor = Actor();
    mProposedActorToFocus = Actor();
    mDirection = Control::Left;
  }

  bool& mSignalVerified;
  Actor mCurrentFocusedActor;
  Actor mProposedActorToFocus;
  Control::KeyboardFocusNavigationDirection mDirection;
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
    mCurrentFocusedActor = currentFocusedActor;
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
    mForward = forward;
  }

  void Reset()
  {
    mSignalVerified = false;
  }

  bool& mSignalVerified;
  Actor mCurrentFocusedActor;
  bool mForward;
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
TEST_FUNCTION( UtcDaliKeyboardFocusManagerGet,                              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyboardFocusManagerSetAndGetCurrentFocusActor,       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyboardFocusManagerMoveFocus,                        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyboardFocusManagerClearFocus,                       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyboardFocusManagerSetAndGetFocusGroupLoop,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyboardFocusManagerSetAsFocusGroup,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyboardFocusManagerGetFocusGroup,                    POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyboardFocusManagerSetAndGetFocusIndicator,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyboardFocusManagerSignalFocusGroupChanged,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyboardFocusManagerSignalFocusedActorActivated,      POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliKeyboardFocusManagerGet()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardKeyboardFocusManagerGet");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "KeyboardFocusManager" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  KeyboardFocusManager manager;

  manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  KeyboardFocusManager newManager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(newManager);

  // Check that focus manager is a singleton
  DALI_TEST_CHECK(manager == newManager);
}

static void UtcDaliKeyboardFocusManagerSetAndGetCurrentFocusActor()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerSetAndGetCurrentFocusActor");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "KeyboardFocusManager" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Create the first actor and add it to the stage
  Actor first = Actor::New();
  first.SetKeyboardFocusable(true);
  Stage::GetCurrent().Add(first);

  // Create the second actor and add it to the stage
  Actor second = Actor::New();
  second.SetKeyboardFocusable(true);
  Stage::GetCurrent().Add(second);

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
  Stage::GetCurrent().Add(third);

  // Check that it will fail to set focus on the third actor as it's not focusable
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(third) == false);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);

  // Make the third actor focusable
  third.SetKeyboardFocusable(true);

  // Check that the focus is successfully moved to the third actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(third) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
}

static void UtcDaliKeyboardFocusManagerMoveFocus()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerMoveFocus");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "KeyboardFocusManager" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  bool preFocusChangeSignalVerified = false;
  PreFocusChangeCallback preFocusChangeCallback(preFocusChangeSignalVerified);
  manager.PreFocusChangeSignal().Connect( &preFocusChangeCallback, &PreFocusChangeCallback::Callback );

  bool focusChangedSignalVerified = false;
  FocusChangedCallback focusChangedCallback(focusChangedSignalVerified);
  manager.FocusChangedSignal().Connect( &focusChangedCallback, &FocusChangedCallback::Callback );

  // Create the first actor and add it to the stage
  Actor first = Actor::New();
  first.SetKeyboardFocusable(true);
  Stage::GetCurrent().Add(first);

  // Create the second actor and add it to the stage
  Actor second = Actor::New();
  second.SetKeyboardFocusable(true);
  Stage::GetCurrent().Add(second);

  // Move the focus to the right
  DALI_TEST_CHECK(manager.MoveFocus(Control::Right) == false);

  // Because no layout control in the stage and no actor is focused, it should emit the PreFocusChange signal
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mDirection == Control::Right);
  preFocusChangeCallback.Reset();

  // Check that the focus is set on the first actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == Actor());
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == first);
  focusChangedCallback.Reset();

  // Move the focus towards right
  DALI_TEST_CHECK(manager.MoveFocus(Control::Right) == false);

  // Because no layout control in the stage and the first actor is focused, it should emit the PreFocusChange signal
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == first);
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mDirection == Control::Right);
  preFocusChangeCallback.Reset();

  // Check that the focus is set on the second actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(second) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == first);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == second);
  focusChangedCallback.Reset();

  // Move the focus towards up
  DALI_TEST_CHECK(manager.MoveFocus(Control::Up) == false);

  // Because no layout control in the stage and no actor is focused, it should emit the PreFocusChange signal
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == second);
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mDirection == Control::Up);
  preFocusChangeCallback.Reset();
  DALI_TEST_CHECK(!focusChangedCallback.mSignalVerified);

  // Create a 2x2 table view and try to move focus inside it
  TableView tableView = TableView::New( 2, 2 );
  Stage::GetCurrent().Add(tableView);

  // Create the third actor
  Actor third = Actor::New();
  third.SetKeyboardFocusable(true);

  // Create the fourth actor
  Actor fourth = Actor::New();
  fourth.SetKeyboardFocusable(true);

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
  DALI_TEST_CHECK(manager.MoveFocus(Control::Right) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == first);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == second);
  focusChangedCallback.Reset();

  // Move the focus towards down
  DALI_TEST_CHECK(manager.MoveFocus(Control::Down) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == fourth);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == second);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == fourth);
  focusChangedCallback.Reset();

  // Move the focus towards left
  DALI_TEST_CHECK(manager.MoveFocus(Control::Left) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == fourth);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == third);
  focusChangedCallback.Reset();

  // Move the focus towards up
  DALI_TEST_CHECK(manager.MoveFocus(Control::Up) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == third);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == first);
  focusChangedCallback.Reset();

  // Move the focus towards left. The focus move will fail as no way to move it upwards
  DALI_TEST_CHECK(manager.MoveFocus(Control::Left) == false);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(preFocusChangeCallback.mSignalVerified);
  DALI_TEST_CHECK(preFocusChangeCallback.mCurrentFocusedActor == first);
  DALI_TEST_CHECK(preFocusChangeCallback.mProposedActorToFocus == Actor());
  DALI_TEST_CHECK(preFocusChangeCallback.mDirection == Control::Left);
  preFocusChangeCallback.Reset();
  DALI_TEST_CHECK(!focusChangedCallback.mSignalVerified);

  // Enable the loop
  manager.SetFocusGroupLoop(true);
  DALI_TEST_CHECK(manager.GetFocusGroupLoop() == true);

  // Move the focus towards left again. The focus should move to the fourth actor.
  DALI_TEST_CHECK(manager.MoveFocus(Control::Left) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == fourth);
  DALI_TEST_CHECK(focusChangedCallback.mSignalVerified);
  DALI_TEST_CHECK(focusChangedCallback.mOriginalFocusedActor == first);
  DALI_TEST_CHECK(focusChangedCallback.mCurrentFocusedActor == fourth);
  focusChangedCallback.Reset();
}

static void UtcDaliKeyboardFocusManagerClearFocus()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerClearFocus");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "KeyboardFocusManager" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Create the first actor and add it to the stage
  Actor first = Actor::New();
  first.SetKeyboardFocusable(true);
  Stage::GetCurrent().Add(first);

  // Create the second actor and add it to the stage
  Actor second = Actor::New();
  second.SetKeyboardFocusable(true);
  Stage::GetCurrent().Add(second);

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
}

static void UtcDaliKeyboardFocusManagerSetAndGetFocusGroupLoop()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerSetAndGetFocusGroupLoop");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "KeyboardFocusManager" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Check that the focus movement is not looped within the same focus group by default
  DALI_TEST_CHECK(manager.GetFocusGroupLoop() == false);

  // Enable the loop
  manager.SetFocusGroupLoop(true);
  DALI_TEST_CHECK(manager.GetFocusGroupLoop() == true);
}

static void UtcDaliKeyboardFocusManagerSetAsFocusGroup()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerSetAsFocusGroup");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "KeyboardFocusManager" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

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
}

static void UtcDaliKeyboardFocusManagerGetFocusGroup()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerGetFocusGroup");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "KeyboardFocusManager" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Create an actor with two child actors and add it to the stage
  Actor parent = Actor::New();
  Actor child = Actor::New();
  parent.Add(child);
  Stage::GetCurrent().Add(parent);

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
}

static void UtcDaliKeyboardFocusManagerSetAndGetFocusIndicator()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerSetAndGetFocusIndicator");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "KeyboardFocusManager" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  Actor defaultFocusIndicatorActor = manager.GetFocusIndicatorActor();
  DALI_TEST_CHECK(defaultFocusIndicatorActor);

  Actor newFocusIndicatorActor = Actor::New();
  manager.SetFocusIndicatorActor(newFocusIndicatorActor);
  DALI_TEST_CHECK(manager.GetFocusIndicatorActor() == newFocusIndicatorActor);
}

static void UtcDaliKeyboardFocusManagerSignalFocusGroupChanged()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerSignalFocusGroupChanged");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "KeyboardFocusManager" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  bool focusGroupChangedSignalVerified = false;
  FocusGroupChangedCallback focusGroupChangedCallback(focusGroupChangedSignalVerified);
  manager.FocusGroupChangedSignal().Connect( &focusGroupChangedCallback, &FocusGroupChangedCallback::Callback );

  Integration::KeyEvent tabEvent("Tab", "", 0, 0, 0, Integration::KeyEvent::Down);
  Integration::KeyEvent shiftTabEvent("Tab", "", 1, 0, 0, Integration::KeyEvent::Down);

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
}

static void UtcDaliKeyboardFocusManagerSignalFocusedActorActivated()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliKeyboardFocusManagerSignalFocusedActorActivated");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "KeyboardFocusManager" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);

  bool focusedActorActivatedSignalVerified = false;
  FocusedActorActivatedCallback focusedActorActivatedCallback(focusedActorActivatedSignalVerified);
  manager.FocusedActorActivatedSignal().Connect( &focusedActorActivatedCallback, &FocusedActorActivatedCallback::Callback );

  Integration::KeyEvent returnEvent("Return", "", 0, 0, 0, Integration::KeyEvent::Up);

  // Create the first button and add it to the stage
  PushButton firstPushButton = PushButton::New();
  firstPushButton.SetKeyboardFocusable(true);
  Stage::GetCurrent().Add(firstPushButton);

  // Create the second button and add it to the stage
  PushButton secondPushButton = PushButton::New();
  secondPushButton.SetKeyboardFocusable(true);
  Stage::GetCurrent().Add(secondPushButton);

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
}
