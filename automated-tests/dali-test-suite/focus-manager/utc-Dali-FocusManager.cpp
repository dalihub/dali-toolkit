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

#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Toolkit;

namespace
{

static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

// Functors to test whether focus changed signal is emitted when the focus is changed
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

// Functors to test whether focus overshot signal is emitted when there is no way to move focus further.
class FocusOvershotCallback : public Dali::ConnectionTracker
{
public:
  FocusOvershotCallback(bool& signalReceived)
  : mSignalVerified(signalReceived),
    mCurrentFocusedActor(),
    mFocusOvershotDirection(Toolkit::FocusManager::OVERSHOT_NEXT)
  {
  }

  void Callback(Actor currentFocusedActor, Toolkit::FocusManager::FocusOvershotDirection direction)
  {
    tet_infoline("Verifying FocusOvershotCallback()");

    if(currentFocusedActor == mCurrentFocusedActor && direction == mFocusOvershotDirection)
    {
      mSignalVerified = true;
    }
  }

  void Reset()
  {
    mSignalVerified = false;
  }

  bool& mSignalVerified;
  Actor mCurrentFocusedActor;
  Toolkit::FocusManager::FocusOvershotDirection mFocusOvershotDirection;
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
TEST_FUNCTION( UtcDaliFocusManagerGet,                              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFocusManagerSetAndGetAccessibilityAttribute,  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFocusManagerSetAndGetFocusOrder,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFocusManagerGenerateNewFocusOrder,            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFocusManagerGetActorByFocusOrder,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFocusManagerSetAndGetCurrentFocusActor,       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFocusManagerGetCurrentFocusGroup,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFocusManagerGetCurrentFocusOrder,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFocusManagerMoveFocusForward,                 POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFocusManagerMoveFocusBackward,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFocusManagerClearFocus,                       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFocusManagerReset,                            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFocusManagerFocusGroup,                       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFocusManagerSetAndGetFocusIndicator,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFocusManagerSignalFocusChanged,               POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFocusManagerSignalFocusOvershot,              POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliFocusManagerGet()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliFocusManagerGet");

  FocusManager manager;

  //Ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK(registry);

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    manager = FocusManager::Get();
    DALI_TEST_CHECK(manager);
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );

  FocusManager newManager = FocusManager::Get();
  DALI_TEST_CHECK(newManager);

  // Check that focus manager is a singleton
  DALI_TEST_CHECK(manager == newManager);
}

static void UtcDaliFocusManagerSetAndGetAccessibilityAttribute()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliFocusManagerSetAndGetAccessibilityAttribute");

  FocusManager manager = FocusManager::Get();
  DALI_TEST_CHECK(manager);

  Actor actor = Actor::New();
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(actor, FocusManager::ACCESSIBILITY_LABEL) == "");

  manager.SetAccessibilityAttribute(actor, FocusManager::ACCESSIBILITY_LABEL, "Description");
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(actor, FocusManager::ACCESSIBILITY_LABEL) == "Description");

  manager.SetAccessibilityAttribute(actor, FocusManager::ACCESSIBILITY_LABEL, "New description");
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(actor, FocusManager::ACCESSIBILITY_LABEL) == "New description");
}

static void UtcDaliFocusManagerSetAndGetFocusOrder()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliFocusManagerSetAndGetFocusOrder");

  FocusManager manager = FocusManager::Get();
  DALI_TEST_CHECK(manager);

  Actor first = Actor::New();
  Actor second = Actor::New();
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 0);
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 0);

  // Set the focus order and description for the first actor
  manager.SetFocusOrder(first, 1);
  manager.SetAccessibilityAttribute(first, FocusManager::ACCESSIBILITY_LABEL, "first");
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 1);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(first, FocusManager::ACCESSIBILITY_LABEL) == "first");

  // Set the focus order and description for the second actor
  manager.SetFocusOrder(second, 2);
  manager.SetAccessibilityAttribute(second, FocusManager::ACCESSIBILITY_LABEL, "second");
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 2);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(second, FocusManager::ACCESSIBILITY_LABEL) == "second");

  // check that the focus order of the first actor is changed
  manager.SetFocusOrder(first, 2);
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 2);
  // make sure the change of focus order doesn't affect the actor's description
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(first, FocusManager::ACCESSIBILITY_LABEL) == "first");

  // check that the focus order of the second actor is increased to 3
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 3);
  // make sure the change of focus order doesn't affect the actor's description
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(second, FocusManager::ACCESSIBILITY_LABEL) == "second");

  // check that the focus order of the second actor is changed to 1
  manager.SetFocusOrder(second, 1);
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 1);
  // make sure the change of focus order doesn't affect the actor's description
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(second, FocusManager::ACCESSIBILITY_LABEL) == "second");

  // Set the focus order and description for the third actor
  Actor third = Actor::New();
  manager.SetFocusOrder(third, 1);
  manager.SetAccessibilityAttribute(third, FocusManager::ACCESSIBILITY_LABEL, "third");
  DALI_TEST_CHECK(manager.GetFocusOrder(third) == 1);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(third, FocusManager::ACCESSIBILITY_LABEL) == "third");

  // check that the focus order of the second actor is increased to 2.
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 2);
  // make sure the change of focus order doesn't affect the actor's description
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(second, FocusManager::ACCESSIBILITY_LABEL) == "second");

  // check that the focus order of the first actor is increased to 3.
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 3);
  // make sure the change of focus order doesn't affect the actor's description
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(first, FocusManager::ACCESSIBILITY_LABEL) == "first");
}

static void UtcDaliFocusManagerGenerateNewFocusOrder()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliFocusManagerGenerateNewFocusOrder");

  FocusManager manager = FocusManager::Get();
  DALI_TEST_CHECK(manager);

  DALI_TEST_CHECK(1 == manager.GenerateNewFocusOrder());
  DALI_TEST_CHECK(1 == manager.GenerateNewFocusOrder());

  Actor first = Actor::New();
  Actor second = Actor::New();

  // Set the focus order for the first actor
  manager.SetFocusOrder(first, 1);
  manager.SetAccessibilityAttribute(first, FocusManager::ACCESSIBILITY_LABEL, "first");
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 1);

  //Test for new focus order
  DALI_TEST_CHECK(2 == manager.GenerateNewFocusOrder());

  // Set the focus order for the first actor
  manager.SetFocusOrder(second, 2);
  manager.SetAccessibilityAttribute(second, FocusManager::ACCESSIBILITY_LABEL, "first");
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 2);
}

static void UtcDaliFocusManagerGetActorByFocusOrder()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliFocusManagerGetActorByFocusOrder");

  FocusManager manager = FocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Create the actors and set their focus orders
  Actor first = Actor::New();
  manager.SetFocusOrder(first, 1);

  Actor second = Actor::New();
  manager.SetFocusOrder(second, 2);

  Actor third = Actor::New();
  manager.SetFocusOrder(third, 3);

  // Check that we get an empty handle as no actor is added to the stage yet.
  DALI_TEST_CHECK(manager.GetActorByFocusOrder(1) == Actor());
  DALI_TEST_CHECK(manager.GetActorByFocusOrder(2) == Actor());
  DALI_TEST_CHECK(manager.GetActorByFocusOrder(3) == Actor());

  // Add the actors to the stage
  Stage::GetCurrent().Add(first);
  Stage::GetCurrent().Add(second);
  Stage::GetCurrent().Add(third);

  // Check that we get an empty handle because focus order 0 means undefined.
  DALI_TEST_CHECK(manager.GetActorByFocusOrder(0) == Actor());

  // Check that we get correct actors for the specified focus orders
  DALI_TEST_CHECK(manager.GetActorByFocusOrder(1) == first);
  DALI_TEST_CHECK(manager.GetActorByFocusOrder(2) == second);
  DALI_TEST_CHECK(manager.GetActorByFocusOrder(3) == third);

  // Change the focus order of the third actor to 1
  manager.SetFocusOrder(third, 1);

  // Check that we still get correct actors after changing their focus orders
  DALI_TEST_CHECK(manager.GetActorByFocusOrder(1) == third);
  DALI_TEST_CHECK(manager.GetActorByFocusOrder(2) == first);
  DALI_TEST_CHECK(manager.GetActorByFocusOrder(3) == second);

  // Check that we get an empty handle because no actor has a focus order of 4
  DALI_TEST_CHECK(manager.GetActorByFocusOrder(4) == Actor());
}

static void UtcDaliFocusManagerSetAndGetCurrentFocusActor()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliFocusManagerSetAndGetCurrentFocusActor");

  FocusManager manager = FocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Create the first actor and add it to the stage
  Actor first = Actor::New();
  manager.SetFocusOrder(first, 1);
  Stage::GetCurrent().Add(first);

  // Create the second actor and add it to the stage
  Actor second = Actor::New();
  manager.SetFocusOrder(second, 2);
  Stage::GetCurrent().Add(second);

  // Create the third actor but don't add it to the stage
  Actor third = Actor::New();
  manager.SetFocusOrder(third, 3);

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

  // Add the third actor to the stage
  Stage::GetCurrent().Add(third);

  // make the third actor invisible
  third.SetVisible(false);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that it will fail to set focus on the third actor as it's invisible
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(third) == false);

  // Make the third actor visible
  third.SetVisible(true);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Make the third actor not focusable
  Property::Index propertyActorFocusable = third.GetPropertyIndex("is-focus-group");
  third.SetProperty(propertyActorFocusable, false);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that it will fail to set focus on the third actor as it's not focusable
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(third) == false);

  // Make the third actor focusable
  third.SetProperty(propertyActorFocusable, true);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the focus is successfully moved to the third actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(third) == true);

  // Make the current focused actor to be not focusable by setting its focus order to be 0
  manager.SetFocusOrder(third, 0);

  // Check that the focus is automatically cleared
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());

  // Set the focus order of the third actor again
  manager.SetFocusOrder(third, 3);

  // Check that the third actor can be focused successfully now
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(third) == true);
}

static void UtcDaliFocusManagerGetCurrentFocusGroup()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliFocusManagerGetCurrentFocusGroup");

  FocusManager manager = FocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Create an actor with two child actors and add it to the stage
  Actor parent = Actor::New();
  Actor firstChild = Actor::New();
  Actor secondChild = Actor::New();
  parent.Add(firstChild);
  parent.Add(secondChild);
  Stage::GetCurrent().Add(parent);

  // Create three actors and add them as the children of the first child actor
  Actor firstGrandChild = Actor::New();
  Actor secondGrandChild = Actor::New();
  Actor thirdGrandChild = Actor::New();
  firstChild.Add(firstGrandChild);
  firstChild.Add(secondGrandChild);
  firstChild.Add(thirdGrandChild);

  // Set focus order to the actors
  manager.SetFocusOrder(parent, 1);
  manager.SetFocusOrder(firstChild, 2);
  manager.SetFocusOrder(firstGrandChild, 3);
  manager.SetFocusOrder(secondGrandChild, 4);
  manager.SetFocusOrder(thirdGrandChild, 5);
  manager.SetFocusOrder(secondChild, 6);

  // Set the parent and the first child actor as focus groups
  manager.SetFocusGroup(parent, true);
  DALI_TEST_CHECK(manager.IsFocusGroup(parent) == true);

  // Set focus to the first grand child actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(firstGrandChild) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == firstGrandChild);

  // The current focus group should be the parent, As it is the immediate parent which is also a focus group.
  DALI_TEST_CHECK(manager.GetCurrentFocusGroup() == parent);

  manager.SetFocusGroup(firstChild, true);
  DALI_TEST_CHECK(manager.IsFocusGroup(firstChild) == true);

  // The current focus group should be the firstChild, As it is the immediate parent which is also a focus group.
  DALI_TEST_CHECK(manager.GetCurrentFocusGroup() == firstChild);

  manager.SetFocusGroup(firstGrandChild, true);
  DALI_TEST_CHECK(manager.IsFocusGroup(firstGrandChild) == true);

  // The current focus group should be itself, As it is also a focus group.
  DALI_TEST_CHECK(manager.GetCurrentFocusGroup() == firstGrandChild);

  // Set focus to the second grand child actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(secondGrandChild) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == secondGrandChild);

  // The current focus group should be the firstChild, As it is the immediate parent which is also a
  // focus group for the current focus actor.
  DALI_TEST_CHECK(manager.GetCurrentFocusGroup() == firstChild);

}

static void UtcDaliFocusManagerGetCurrentFocusOrder()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliFocusManagerGetCurrentFocusOrder");

  FocusManager manager = FocusManager::Get();
  DALI_TEST_CHECK(manager);

  Actor first = Actor::New();
  Stage::GetCurrent().Add(first);

  Actor second = Actor::New();
  Stage::GetCurrent().Add(second);

  Actor third = Actor::New();
  Stage::GetCurrent().Add(third);

  // Set the focus order and description for the first actor
  manager.SetFocusOrder(first, 1);
  manager.SetAccessibilityAttribute(first, FocusManager::ACCESSIBILITY_LABEL, "first");
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 1);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(first, FocusManager::ACCESSIBILITY_LABEL) == "first");

  // Set the focus order and description for the second actor
  manager.SetFocusOrder(second, 2);
  manager.SetAccessibilityAttribute(second, FocusManager::ACCESSIBILITY_LABEL, "second");
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 2);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(second, FocusManager::ACCESSIBILITY_LABEL) == "second");

  // Set the focus order and description for the second actor
  manager.SetFocusOrder(third, 3);
  manager.SetAccessibilityAttribute(third, FocusManager::ACCESSIBILITY_LABEL, "third");
  DALI_TEST_CHECK(manager.GetFocusOrder(third) == 3);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(third, FocusManager::ACCESSIBILITY_LABEL) == "third");

  // Check that no actor is being focused yet.
  DALI_TEST_CHECK(manager.GetCurrentFocusOrder() == 0);

  // Set the focus on the first actor and test
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusOrder() == 1);

  // Move the focus forward to the second actor and test
  manager.MoveFocusForward();
  DALI_TEST_CHECK(manager.GetCurrentFocusOrder() == 2);

  // Move the focus forward to the third actor and test
  manager.MoveFocusForward();
  DALI_TEST_CHECK(manager.GetCurrentFocusOrder() == 3);

  // Clear focus and test
  manager.ClearFocus();
  DALI_TEST_CHECK(manager.GetCurrentFocusOrder() == 0);
}

static void UtcDaliFocusManagerMoveFocusForward()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliFocusManagerMoveFocusForward");

  FocusManager manager = FocusManager::Get();
  DALI_TEST_CHECK(manager);

  Actor first = Actor::New();
  Stage::GetCurrent().Add(first);

  Actor second = Actor::New();
  Stage::GetCurrent().Add(second);

  Actor third = Actor::New();
  Stage::GetCurrent().Add(third);

  // Set the focus order and description for the first actor
  manager.SetFocusOrder(first, 1);
  manager.SetAccessibilityAttribute(first, FocusManager::ACCESSIBILITY_LABEL, "first");
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 1);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(first, FocusManager::ACCESSIBILITY_LABEL) == "first");

  // Set the focus order and description for the second actor
  manager.SetFocusOrder(second, 2);
  manager.SetAccessibilityAttribute(second, FocusManager::ACCESSIBILITY_LABEL, "second");
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 2);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(second, FocusManager::ACCESSIBILITY_LABEL) == "second");

  // Set the focus order and description for the second actor
  manager.SetFocusOrder(third, 3);
  manager.SetAccessibilityAttribute(third, FocusManager::ACCESSIBILITY_LABEL, "third");
  DALI_TEST_CHECK(manager.GetFocusOrder(third) == 3);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(third, FocusManager::ACCESSIBILITY_LABEL) == "third");

  // Check that no actor is being focused yet.
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());

  // Set the focus on the first actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), FocusManager::ACCESSIBILITY_LABEL) == "first");

  // Test the non-wrapped move first
  manager.SetWrapMode(false);
  DALI_TEST_CHECK(manager.GetWrapMode() == false);

  // Move the focus forward to the second actor
  manager.MoveFocusForward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), FocusManager::ACCESSIBILITY_LABEL) == "second");

  // Move the focus forward to the third actor
  manager.MoveFocusForward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), FocusManager::ACCESSIBILITY_LABEL) == "third");

  // Check that it will fail to move the focus forward again as the third actor is the last
  // focusable actor in the focus chain
  manager.MoveFocusForward();
  // The focus should still be set on the third actor
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), FocusManager::ACCESSIBILITY_LABEL) == "third");

  // Now test the wrapped move
  manager.SetWrapMode(true);
  DALI_TEST_CHECK(manager.GetWrapMode() == true);

  // Move the focus forward recursively and this time the first actor should be focused
  manager.MoveFocusForward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), FocusManager::ACCESSIBILITY_LABEL) == "first");

  // Make the second actor not focusable
  Property::Index propertyActorFocusable = second.GetPropertyIndex("focusable");
  second.SetProperty(propertyActorFocusable, false);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Move the focus forward and check that the second actor should be skipped and
  // the third actor should be focused now.
  manager.MoveFocusForward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), FocusManager::ACCESSIBILITY_LABEL) == "third");

  // Make the first actor invisible
  first.SetVisible(false);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Move the focus forward and check that the first actor should be skipped as it's
  // invisible and the second actor should also be skipped as it's not focusable,
  // so the focus will still be on the third actor
  manager.MoveFocusForward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), FocusManager::ACCESSIBILITY_LABEL) == "third");

  // Make the third actor invisible so that no actor can be focused.
  third.SetVisible(false);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the focus move is failed as all the three actors can not be focused
  manager.MoveFocusForward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), FocusManager::ACCESSIBILITY_LABEL) == "third");
}

static void UtcDaliFocusManagerMoveFocusBackward()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliFocusManagerMoveFocusBackward");

  FocusManager manager = FocusManager::Get();
  DALI_TEST_CHECK(manager);

  Actor first = Actor::New();
  Stage::GetCurrent().Add(first);

  Actor second = Actor::New();
  Stage::GetCurrent().Add(second);

  Actor third = Actor::New();
  Stage::GetCurrent().Add(third);

  // Set the focus order and description for the first actor
  manager.SetFocusOrder(first, 1);
  manager.SetAccessibilityAttribute(first, FocusManager::ACCESSIBILITY_LABEL, "first");
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 1);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(first, FocusManager::ACCESSIBILITY_LABEL) == "first");

  // Set the focus order and description for the second actor
  manager.SetFocusOrder(second, 2);
  manager.SetAccessibilityAttribute(second, FocusManager::ACCESSIBILITY_LABEL, "second");
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 2);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(second, FocusManager::ACCESSIBILITY_LABEL) == "second");

  // Set the focus order and description for the second actor
  manager.SetFocusOrder(third, 3);
  manager.SetAccessibilityAttribute(third, FocusManager::ACCESSIBILITY_LABEL, "third");
  DALI_TEST_CHECK(manager.GetFocusOrder(third) == 3);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(third, FocusManager::ACCESSIBILITY_LABEL) == "third");

  // Check that no actor is being focused yet.
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());

  // Set the focus on the third actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(third) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), FocusManager::ACCESSIBILITY_LABEL) == "third");

  // Test the non-wrapped move first
  manager.SetWrapMode(false);
  DALI_TEST_CHECK(manager.GetWrapMode() == false);

  // Move the focus backward to the second actor
  manager.MoveFocusBackward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), FocusManager::ACCESSIBILITY_LABEL) == "second");

  // Move the focus backward to the first actor
  manager.MoveFocusBackward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), FocusManager::ACCESSIBILITY_LABEL) == "first");

  // Check that it will fail to move the focus backward again as the first actor is the first
  // focusable actor in the focus chain
  manager.MoveFocusBackward();
  // The focus should still be set on the first actor
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), FocusManager::ACCESSIBILITY_LABEL) == "first");

  // Now test the wrapped move
  manager.SetWrapMode(true);
  DALI_TEST_CHECK(manager.GetWrapMode() == true);

  // Move the focus backward recursively and this time the third actor should be focused
  manager.MoveFocusBackward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), FocusManager::ACCESSIBILITY_LABEL) == "third");

  // Make the second actor not focusable
  Property::Index propertyActorFocusable = second.GetPropertyIndex("focusable");
  second.SetProperty(propertyActorFocusable, false);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Move the focus backward and check that the second actor should be skipped and
  // the first actor should be focused now.
  manager.MoveFocusBackward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), FocusManager::ACCESSIBILITY_LABEL) == "first");

  // Make the third actor invisible
  third.SetVisible(false);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Move the focus backward and check that the third actor should be skipped as it's
  // invisible and the second actor should also be skipped as it's not focusable,
  // so the focus will still be on the first actor
  manager.MoveFocusBackward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), FocusManager::ACCESSIBILITY_LABEL) == "first");

  // Make the first actor invisible so that no actor can be focused.
  first.SetVisible(false);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the focus move is failed as all the three actors can not be focused
  manager.MoveFocusBackward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), FocusManager::ACCESSIBILITY_LABEL) == "first");
}

static void UtcDaliFocusManagerClearFocus()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliFocusManagerClearFocus");

  FocusManager manager = FocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Create the first actor and add it to the stage
  Actor first = Actor::New();
  manager.SetFocusOrder(first, 1);
  Stage::GetCurrent().Add(first);

  // Create the second actor and add it to the stage
  Actor second = Actor::New();
  manager.SetFocusOrder(second, 2);
  Stage::GetCurrent().Add(second);

  // Check that no actor is being focused yet.
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());

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

static void UtcDaliFocusManagerReset()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliFocusManagerReset");

  FocusManager manager = FocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Create the first actor and add it to the stage
  Actor first = Actor::New();
  manager.SetFocusOrder(first, 1);
  Stage::GetCurrent().Add(first);

  // Create the second actor and add it to the stage
  Actor second = Actor::New();
  manager.SetFocusOrder(second, 2);
  Stage::GetCurrent().Add(second);

  // Check that no actor is being focused yet.
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());

  // Check that the focus is set on the first actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);

  // Check that the focus is set on the second actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(second) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);

  // Clear the focus
  manager.Reset();

  // Check that no actor is being focused now and the focus order of actors have been cleared
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 0);
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 0);
}

static void UtcDaliFocusManagerFocusGroup()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliFocusManagerFocusGroup");

  FocusManager manager = FocusManager::Get();
  DALI_TEST_CHECK(manager);

  // Create an actor with two child actors and add it to the stage
  Actor parent = Actor::New();
  Actor firstChild = Actor::New();
  Actor secondChild = Actor::New();
  parent.Add(firstChild);
  parent.Add(secondChild);
  Stage::GetCurrent().Add(parent);

  // Create three actors and add them as the children of the first child actor
  Actor firstGrandChild = Actor::New();
  Actor secondGrandChild = Actor::New();
  Actor thirdGrandChild = Actor::New();
  firstChild.Add(firstGrandChild);
  firstChild.Add(secondGrandChild);
  firstChild.Add(thirdGrandChild);

  // Set focus order to the actors
  manager.SetFocusOrder(parent, 1);
  manager.SetFocusOrder(firstChild, 2);
  manager.SetFocusOrder(firstGrandChild, 3);
  manager.SetFocusOrder(secondGrandChild, 4);
  manager.SetFocusOrder(thirdGrandChild, 5);
  manager.SetFocusOrder(secondChild, 6);

  // Set the parent and the first child actor as focus groups
  manager.SetFocusGroup(parent, true);
  DALI_TEST_CHECK(manager.IsFocusGroup(parent) == true);

  // The focus group of the parent should be itself, as it is set to be a focus group.
  DALI_TEST_CHECK(manager.GetFocusGroup(parent) == parent);

  // The focus group of the firstChild should be its parent, as it is the immediate parent which is also a group.
  DALI_TEST_CHECK(manager.GetFocusGroup(firstChild) == parent);

  manager.SetFocusGroup(firstChild, true);
  DALI_TEST_CHECK(manager.IsFocusGroup(firstChild) == true);

  // The focus group of the firstChild should be itself, as it is set to be a focus group now.
  DALI_TEST_CHECK(manager.GetFocusGroup(firstChild) == firstChild);

  // Enable wrap mode for focus movement.
  manager.SetWrapMode(true);
  DALI_TEST_CHECK(manager.GetWrapMode() == true);

  // Check that no actor is being focused yet.
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());

  // Check that the focus is set on the parent actor.
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(parent) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == parent);

  // Check that group mode is disabled.
  DALI_TEST_CHECK(manager.GetGroupMode() == false);

  // Check that the focus movement is wrapped as normal.
  DALI_TEST_CHECK(manager.MoveFocusForward() == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == firstChild);
  DALI_TEST_CHECK(manager.MoveFocusForward() == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == firstGrandChild);
  DALI_TEST_CHECK(manager.MoveFocusForward() == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == secondGrandChild);
  DALI_TEST_CHECK(manager.MoveFocusForward() == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == thirdGrandChild);
  DALI_TEST_CHECK(manager.MoveFocusForward() == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == secondChild);
  DALI_TEST_CHECK(manager.MoveFocusForward() == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == parent);
  DALI_TEST_CHECK(manager.MoveFocusForward() == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == firstChild);
  DALI_TEST_CHECK(manager.MoveFocusForward() == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == firstGrandChild);

  // Enable the group mode.
  manager.SetGroupMode(true);
  DALI_TEST_CHECK(manager.GetGroupMode() == true);

  // Check that the focus movement is now limited to the current focus group.
  DALI_TEST_CHECK(manager.MoveFocusForward() == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == secondGrandChild);
  DALI_TEST_CHECK(manager.MoveFocusForward() == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == thirdGrandChild);
  DALI_TEST_CHECK(manager.MoveFocusForward() == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == firstChild);
  DALI_TEST_CHECK(manager.MoveFocusForward() == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == firstGrandChild);
}

static void UtcDaliFocusManagerSetAndGetFocusIndicator()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliFocusManagerSetAndGetFocusIndicator");

  FocusManager manager = FocusManager::Get();
  DALI_TEST_CHECK(manager);

  Actor defaultFocusIndicatorActor = manager.GetFocusIndicatorActor();
  DALI_TEST_CHECK(defaultFocusIndicatorActor);

  Actor newFocusIndicatorActor = Actor::New();
  manager.SetFocusIndicatorActor(newFocusIndicatorActor);
  DALI_TEST_CHECK(manager.GetFocusIndicatorActor() == newFocusIndicatorActor);
}

static void UtcDaliFocusManagerSignalFocusChanged()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliFocusManagerSignalFocusChanged");

  FocusManager manager = FocusManager::Get();
  DALI_TEST_CHECK(manager);

  bool signalVerified = false;
  FocusChangedCallback callback(signalVerified);
  manager.FocusChangedSignal().Connect( &callback, &FocusChangedCallback::Callback );

  // Create the first actor and add it to the stage
  Actor first = Actor::New();
  manager.SetFocusOrder(first, 1);
  Stage::GetCurrent().Add(first);

  // Create the second actor and add it to the stage
  Actor second = Actor::New();
  manager.SetFocusOrder(second, 2);
  Stage::GetCurrent().Add(second);

  // Check that no actor is being focused yet.
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());

  // Check that the focus is set on the first actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(callback.mSignalVerified);
  callback.Reset();

  // Check that the focus is set on the second actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(second) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);
  DALI_TEST_CHECK(callback.mSignalVerified);
  callback.Reset();

  // Clear the focus
  manager.ClearFocus();

  // Check that no actor is being focused now.
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());
  DALI_TEST_CHECK(callback.mSignalVerified);
}

static void UtcDaliFocusManagerSignalFocusOvershot()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliFocusManagerSignalFocusOvershot");

  FocusManager manager = FocusManager::Get();
  DALI_TEST_CHECK(manager);

  bool signalVerified = false;
  FocusOvershotCallback callback(signalVerified);
  manager.FocusOvershotSignal().Connect(&callback, &FocusOvershotCallback::Callback);

  // Create the first actor and add it to the stage
  Actor first = Actor::New();
  manager.SetFocusOrder(first, 1);
  Stage::GetCurrent().Add(first);

  // Create the second actor and add it to the stage
  Actor second = Actor::New();
  manager.SetFocusOrder(second, 2);
  Stage::GetCurrent().Add(second);

  // Check that the wrap mode is disabled
  DALI_TEST_CHECK(manager.GetWrapMode() == false);

  // Check that the focus is set on the first actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);

  // Check that the focus is moved to the second actor successfully.
  DALI_TEST_CHECK(manager.MoveFocusForward() == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);

  // Check that the forward focus movement is overshot.
  callback.mCurrentFocusedActor = second;
  callback.mFocusOvershotDirection = Toolkit::FocusManager::OVERSHOT_NEXT;
  DALI_TEST_CHECK(manager.MoveFocusForward() == false);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);
  DALI_TEST_CHECK(signalVerified);
  callback.Reset();

  // Enable the wrap mode
  manager.SetWrapMode(true);
  DALI_TEST_CHECK(manager.GetWrapMode() == true);

  // Check that the forward focus movement is wrapped and no overshot happens.
  DALI_TEST_CHECK(manager.MoveFocusForward() == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(signalVerified == false);

  // Disable the wrap mode
  manager.SetWrapMode(false);
  DALI_TEST_CHECK(manager.GetWrapMode() == false);

  // Check that the backward focus movement is overshot.
  callback.mCurrentFocusedActor = first;
  callback.mFocusOvershotDirection = Toolkit::FocusManager::OVERSHOT_PREVIOUS;
  DALI_TEST_CHECK(manager.MoveFocusBackward() == false);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(signalVerified);
}
