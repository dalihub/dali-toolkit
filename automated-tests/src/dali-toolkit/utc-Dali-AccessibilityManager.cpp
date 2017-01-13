/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit-test-suite-utils.h>
#include <toolkit-accessibility-adaptor.h>
#include <dummy-control.h>

using namespace Dali;
using namespace Toolkit;


void utc_dali_toolkit_accessibility_manager_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_accessibility_manager_cleanup(void)
{
  test_return_value = TET_PASS;
}


namespace
{

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
    mFocusOvershotDirection(Toolkit::AccessibilityManager::OVERSHOT_NEXT)
  {
  }

  void Callback(Actor currentFocusedActor, Toolkit::AccessibilityManager::FocusOvershotDirection direction)
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
  Toolkit::AccessibilityManager::FocusOvershotDirection mFocusOvershotDirection;
};

// Functor to test whether focused actor activated signal is emitted.
class FocusedActorActivatedCallback : public Dali::ConnectionTracker
{
public:
  FocusedActorActivatedCallback()
  {
  }

  void Callback(Actor activatedActor)
  {
    tet_infoline("Verifying FocusedActorActivatedCallback()");
  }
};

} // namespace


int UtcDaliAccessibilityManagerGet(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerGet");

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK(manager);

  AccessibilityManager newManager = AccessibilityManager::Get();
  DALI_TEST_CHECK(newManager);

  // Check that accessibility manager is a singleton
  DALI_TEST_CHECK(manager == newManager);
  END_TEST;
}

int UtcDaliAccessibilityManagerSetAndGetAccessibilityAttribute(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerSetAndGetAccessibilityAttribute");

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK(manager);

  Actor actor = Actor::New();
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(actor, AccessibilityManager::ACCESSIBILITY_LABEL) == "");

  manager.SetAccessibilityAttribute(actor, AccessibilityManager::ACCESSIBILITY_LABEL, "Description");
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(actor, AccessibilityManager::ACCESSIBILITY_LABEL) == "Description");

  manager.SetAccessibilityAttribute(actor, AccessibilityManager::ACCESSIBILITY_LABEL, "New description");
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(actor, AccessibilityManager::ACCESSIBILITY_LABEL) == "New description");
  END_TEST;
}

int UtcDaliAccessibilityManagerSetAndGetFocusOrder(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerSetAndGetFocusOrder");

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK(manager);

  Actor first = Actor::New();
  Actor second = Actor::New();
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 0);
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 0);

  // Set the focus order and description for the first actor
  manager.SetFocusOrder(first, 1);
  manager.SetAccessibilityAttribute(first, AccessibilityManager::ACCESSIBILITY_LABEL, "first");
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 1);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(first, AccessibilityManager::ACCESSIBILITY_LABEL) == "first");

  // Set the focus order and description for the second actor
  manager.SetFocusOrder(second, 2);
  manager.SetAccessibilityAttribute(second, AccessibilityManager::ACCESSIBILITY_LABEL, "second");
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 2);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(second, AccessibilityManager::ACCESSIBILITY_LABEL) == "second");

  // check that the focus order of the first actor is changed
  manager.SetFocusOrder(first, 2);
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 2);
  // make sure the change of focus order doesn't affect the actor's description
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(first, AccessibilityManager::ACCESSIBILITY_LABEL) == "first");

  // check that the focus order of the second actor is increased to 3
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 3);
  // make sure the change of focus order doesn't affect the actor's description
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(second, AccessibilityManager::ACCESSIBILITY_LABEL) == "second");

  // check that the focus order of the second actor is changed to 1
  manager.SetFocusOrder(second, 1);
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 1);
  // make sure the change of focus order doesn't affect the actor's description
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(second, AccessibilityManager::ACCESSIBILITY_LABEL) == "second");

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  // Set the focus order and description for the third actor
  Actor third = Actor::New();
  manager.SetFocusOrder(third, 1);
  manager.SetAccessibilityAttribute(third, AccessibilityManager::ACCESSIBILITY_LABEL, "third");
  DALI_TEST_CHECK(manager.GetFocusOrder(third) == 1);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(third, AccessibilityManager::ACCESSIBILITY_LABEL) == "third");

  // check that the focus order of the second actor is increased to 2.
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 2);
  // make sure the change of focus order doesn't affect the actor's description
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(second, AccessibilityManager::ACCESSIBILITY_LABEL) == "second");

  // check that the focus order of the first actor is increased to 3.
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 3);
  // make sure the change of focus order doesn't affect the actor's description
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(first, AccessibilityManager::ACCESSIBILITY_LABEL) == "first");
  END_TEST;
}

int UtcDaliAccessibilityManagerGenerateNewFocusOrder(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerGenerateNewFocusOrder");

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK(manager);

  DALI_TEST_CHECK(1 == manager.GenerateNewFocusOrder());
  DALI_TEST_CHECK(1 == manager.GenerateNewFocusOrder());

  Actor first = Actor::New();
  Actor second = Actor::New();

  // Set the focus order for the first actor
  manager.SetFocusOrder(first, 1);
  manager.SetAccessibilityAttribute(first, AccessibilityManager::ACCESSIBILITY_LABEL, "first");
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 1);

  //Test for new focus order
  DALI_TEST_CHECK(2 == manager.GenerateNewFocusOrder());

  // Set the focus order for the first actor
  manager.SetFocusOrder(second, 2);
  manager.SetAccessibilityAttribute(second, AccessibilityManager::ACCESSIBILITY_LABEL, "first");
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 2);
  END_TEST;
}

int UtcDaliAccessibilityManagerGetActorByFocusOrder(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerGetActorByFocusOrder");

  AccessibilityManager manager = AccessibilityManager::Get();
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

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  // Change the focus order of the third actor to 1
  manager.SetFocusOrder(third, 1);

  // Check that we still get correct actors after changing their focus orders
  DALI_TEST_CHECK(manager.GetActorByFocusOrder(1) == third);
  DALI_TEST_CHECK(manager.GetActorByFocusOrder(2) == first);
  DALI_TEST_CHECK(manager.GetActorByFocusOrder(3) == second);

  // Check that we get an empty handle because no actor has a focus order of 4
  DALI_TEST_CHECK(manager.GetActorByFocusOrder(4) == Actor());
  END_TEST;
}

int UtcDaliAccessibilityManagerSetAndGetCurrentFocusActor(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerSetAndGetCurrentFocusActor");

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK(manager);

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

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
  Property::Index propertyActorFocusable = third.GetPropertyIndex("focusable");
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
  END_TEST;
}

int UtcDaliAccessibilityManagerGetCurrentFocusGroup(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerGetCurrentFocusGroup");

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK(manager);

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

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

  END_TEST;
}

int UtcDaliAccessibilityManagerGetCurrentFocusOrder(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerGetCurrentFocusOrder");

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK(manager);

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  Actor first = Actor::New();
  Stage::GetCurrent().Add(first);

  Actor second = Actor::New();
  Stage::GetCurrent().Add(second);

  Actor third = Actor::New();
  Stage::GetCurrent().Add(third);

  // Set the focus order and description for the first actor
  manager.SetFocusOrder(first, 1);
  manager.SetAccessibilityAttribute(first, AccessibilityManager::ACCESSIBILITY_LABEL, "first");
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 1);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(first, AccessibilityManager::ACCESSIBILITY_LABEL) == "first");

  // Set the focus order and description for the second actor
  manager.SetFocusOrder(second, 2);
  manager.SetAccessibilityAttribute(second, AccessibilityManager::ACCESSIBILITY_LABEL, "second");
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 2);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(second, AccessibilityManager::ACCESSIBILITY_LABEL) == "second");

  // Set the focus order and description for the second actor
  manager.SetFocusOrder(third, 3);
  manager.SetAccessibilityAttribute(third, AccessibilityManager::ACCESSIBILITY_LABEL, "third");
  DALI_TEST_CHECK(manager.GetFocusOrder(third) == 3);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(third, AccessibilityManager::ACCESSIBILITY_LABEL) == "third");

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
  END_TEST;
}

int UtcDaliAccessibilityManagerMoveFocusForward(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerMoveFocusForward");

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK(manager);

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  Actor first = Actor::New();
  Stage::GetCurrent().Add(first);

  Actor second = Actor::New();
  Stage::GetCurrent().Add(second);

  Actor third = Actor::New();
  Stage::GetCurrent().Add(third);

  // Set the focus order and description for the first actor
  manager.SetFocusOrder(first, 1);
  manager.SetAccessibilityAttribute(first, AccessibilityManager::ACCESSIBILITY_LABEL, "first");
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 1);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(first, AccessibilityManager::ACCESSIBILITY_LABEL) == "first");

  // Set the focus order and description for the second actor
  manager.SetFocusOrder(second, 2);
  manager.SetAccessibilityAttribute(second, AccessibilityManager::ACCESSIBILITY_LABEL, "second");
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 2);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(second, AccessibilityManager::ACCESSIBILITY_LABEL) == "second");

  // Set the focus order and description for the second actor
  manager.SetFocusOrder(third, 3);
  manager.SetAccessibilityAttribute(third, AccessibilityManager::ACCESSIBILITY_LABEL, "third");
  DALI_TEST_CHECK(manager.GetFocusOrder(third) == 3);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(third, AccessibilityManager::ACCESSIBILITY_LABEL) == "third");

  // Check that no actor is being focused yet.
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());

  // Set the focus on the first actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(first) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), AccessibilityManager::ACCESSIBILITY_LABEL) == "first");

  // Test the non-wrapped move first
  manager.SetWrapMode(false);
  DALI_TEST_CHECK(manager.GetWrapMode() == false);

  // Move the focus forward to the second actor
  manager.MoveFocusForward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), AccessibilityManager::ACCESSIBILITY_LABEL) == "second");

  // Move the focus forward to the third actor
  manager.MoveFocusForward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), AccessibilityManager::ACCESSIBILITY_LABEL) == "third");

  // Check that it will fail to move the focus forward again as the third actor is the last
  // focusable actor in the focus chain
  manager.MoveFocusForward();
  // The focus should still be set on the third actor
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), AccessibilityManager::ACCESSIBILITY_LABEL) == "third");

  // Now test the wrapped move
  manager.SetWrapMode(true);
  DALI_TEST_CHECK(manager.GetWrapMode() == true);

  // Move the focus forward recursively and this time the first actor should be focused
  manager.MoveFocusForward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), AccessibilityManager::ACCESSIBILITY_LABEL) == "first");

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
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), AccessibilityManager::ACCESSIBILITY_LABEL) == "third");

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
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), AccessibilityManager::ACCESSIBILITY_LABEL) == "third");

  // Make the third actor invisible so that no actor can be focused.
  third.SetVisible(false);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the focus move is failed as all the three actors can not be focused
  manager.MoveFocusForward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), AccessibilityManager::ACCESSIBILITY_LABEL) == "third");
  END_TEST;
}

int UtcDaliAccessibilityManagerMoveFocusBackward(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerMoveFocusBackward");

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK(manager);

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  Actor first = Actor::New();
  Stage::GetCurrent().Add(first);

  Actor second = Actor::New();
  Stage::GetCurrent().Add(second);

  Actor third = Actor::New();
  Stage::GetCurrent().Add(third);

  // Set the focus order and description for the first actor
  manager.SetFocusOrder(first, 1);
  manager.SetAccessibilityAttribute(first, AccessibilityManager::ACCESSIBILITY_LABEL, "first");
  DALI_TEST_CHECK(manager.GetFocusOrder(first) == 1);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(first, AccessibilityManager::ACCESSIBILITY_LABEL) == "first");

  // Set the focus order and description for the second actor
  manager.SetFocusOrder(second, 2);
  manager.SetAccessibilityAttribute(second, AccessibilityManager::ACCESSIBILITY_LABEL, "second");
  DALI_TEST_CHECK(manager.GetFocusOrder(second) == 2);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(second, AccessibilityManager::ACCESSIBILITY_LABEL) == "second");

  // Set the focus order and description for the second actor
  manager.SetFocusOrder(third, 3);
  manager.SetAccessibilityAttribute(third, AccessibilityManager::ACCESSIBILITY_LABEL, "third");
  DALI_TEST_CHECK(manager.GetFocusOrder(third) == 3);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(third, AccessibilityManager::ACCESSIBILITY_LABEL) == "third");

  // Check that no actor is being focused yet.
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == Actor());

  // Set the focus on the third actor
  DALI_TEST_CHECK(manager.SetCurrentFocusActor(third) == true);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), AccessibilityManager::ACCESSIBILITY_LABEL) == "third");

  // Test the non-wrapped move first
  manager.SetWrapMode(false);
  DALI_TEST_CHECK(manager.GetWrapMode() == false);

  // Move the focus backward to the second actor
  manager.MoveFocusBackward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == second);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), AccessibilityManager::ACCESSIBILITY_LABEL) == "second");

  // Move the focus backward to the first actor
  manager.MoveFocusBackward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), AccessibilityManager::ACCESSIBILITY_LABEL) == "first");

  // Check that it will fail to move the focus backward again as the first actor is the first
  // focusable actor in the focus chain
  manager.MoveFocusBackward();
  // The focus should still be set on the first actor
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), AccessibilityManager::ACCESSIBILITY_LABEL) == "first");

  // Now test the wrapped move
  manager.SetWrapMode(true);
  DALI_TEST_CHECK(manager.GetWrapMode() == true);

  // Move the focus backward recursively and this time the third actor should be focused
  manager.MoveFocusBackward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == third);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), AccessibilityManager::ACCESSIBILITY_LABEL) == "third");

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
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), AccessibilityManager::ACCESSIBILITY_LABEL) == "first");

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
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), AccessibilityManager::ACCESSIBILITY_LABEL) == "first");

  // Make the first actor invisible so that no actor can be focused.
  first.SetVisible(false);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the focus move is failed as all the three actors can not be focused
  manager.MoveFocusBackward();
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(manager.GetAccessibilityAttribute(manager.GetCurrentFocusActor(), AccessibilityManager::ACCESSIBILITY_LABEL) == "first");
  END_TEST;
}

int UtcDaliAccessibilityManagerClearFocus(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerClearFocus");

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK(manager);

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

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
  END_TEST;
}

int UtcDaliAccessibilityManagerReset(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerReset");

  AccessibilityManager manager = AccessibilityManager::Get();
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
  END_TEST;
}

int UtcDaliAccessibilityManagerFocusGroup(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerFocusGroup");

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK(manager);

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

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
  END_TEST;
}

int UtcDaliAccessibilityManagerSetAndGetFocusIndicator(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerSetAndGetFocusIndicator");

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK(manager);

  Actor defaultFocusIndicatorActor = manager.GetFocusIndicatorActor();
  DALI_TEST_CHECK(defaultFocusIndicatorActor);

  Actor newFocusIndicatorActor = Actor::New();
  manager.SetFocusIndicatorActor(newFocusIndicatorActor);
  DALI_TEST_CHECK(manager.GetFocusIndicatorActor() == newFocusIndicatorActor);
  END_TEST;
}

int UtcDaliAccessibilityManagerSetAndGetFocusIndicatorWithFocusedActor(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerSetAndGetFocusIndicatorWithFocusedActor");

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK(manager);

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  Actor defaultFocusIndicatorActor = manager.GetFocusIndicatorActor();
  DALI_TEST_CHECK(defaultFocusIndicatorActor);

  Actor focusedActor = Actor::New();
  Stage::GetCurrent().Add( focusedActor );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( focusedActor.GetChildCount(), 0u, TEST_LOCATION );

  manager.SetFocusOrder( focusedActor, 1 );
  manager.SetCurrentFocusActor( focusedActor );

  DALI_TEST_EQUALS( focusedActor.GetChildCount(), 1u, TEST_LOCATION );
  DALI_TEST_CHECK( focusedActor.GetChildAt(0) == defaultFocusIndicatorActor );

  Actor newFocusIndicatorActor = Actor::New();
  manager.SetFocusIndicatorActor( newFocusIndicatorActor );
  DALI_TEST_CHECK(manager.GetFocusIndicatorActor() == newFocusIndicatorActor);
  DALI_TEST_EQUALS( focusedActor.GetChildCount(), 1u, TEST_LOCATION );
  DALI_TEST_CHECK( focusedActor.GetChildAt(0) == newFocusIndicatorActor );

  // Disable Accessibility
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, false );
  accAdaptor.HandleActionEnableEvent();

  DALI_TEST_EQUALS( focusedActor.GetChildCount(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerSignalFocusChanged(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerSignalFocusChanged");

  AccessibilityManager manager = AccessibilityManager::Get();
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
  END_TEST;
}

int UtcDaliAccessibilityManagerSignalFocusOvershot(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerSignalFocusOvershot");

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK(manager);

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

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
  callback.mFocusOvershotDirection = Toolkit::AccessibilityManager::OVERSHOT_NEXT;
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
  callback.mFocusOvershotDirection = Toolkit::AccessibilityManager::OVERSHOT_PREVIOUS;
  DALI_TEST_CHECK(manager.MoveFocusBackward() == false);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor() == first);
  DALI_TEST_CHECK(signalVerified);
  END_TEST;
}

int UtcDaliAccessibilityManagerSignalFocusedActorActivated(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliAccessibilityManagerSignalFocusedActorActivated");

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK(manager);

  FocusedActorActivatedCallback callback;
  manager.FocusedActorActivatedSignal().Connect(&callback, &FocusedActorActivatedCallback::Callback);
  DALI_TEST_CHECK(true);

  END_TEST;
}

// Note: No negative test for GetReadPosition as it will always return something.
int UtcDaliAccessibilityManagerGetReadPositionP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliAccessibilityManagerGetReadPositionP");

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  Vector2 readPosition( manager.GetReadPosition() );
  DALI_TEST_EQUALS( readPosition.x, 0.0f, Math::MACHINE_EPSILON_0, TEST_LOCATION );
  DALI_TEST_EQUALS( readPosition.y, 0.0f, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  END_TEST;
}

// Functor to test if an accessibility signal has been called.
class AccessibilityManagerSignalHandler : public Dali::ConnectionTracker
{
public:
  AccessibilityManagerSignalHandler() :
    mCalls( 0 )
  {
  }

  bool Callback( AccessibilityManager& accessibilityManager )
  {
    mCalls++;
    tet_infoline( "Signal called" );
    return true;
  }

  unsigned int GetCalls() const
  {
    return mCalls;
  }

private:
  unsigned int mCalls;  ///< Keeps track of how many times the signal has been called.
};

int UtcDaliAccessibilityManagerStatusChangedSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerStatusChangedSignalP" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.StatusChangedSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  // Cause a state change.
  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionEnableEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerStatusChangedSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerStatusChangedSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.StatusChangedSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionNextSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionNextSignalP" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionNextSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionNextEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionNextSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionNextSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionNextSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionPreviousSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionPreviousSignalP" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionPreviousSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionPreviousEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionPreviousSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionPreviousSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionPreviousSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionActivateSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionActivateSignalP" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  Dali::Toolkit::PushButton button = Dali::Toolkit::PushButton::New();
  button.SetSize(480, 800);
  Stage::GetCurrent().Add(button);
  manager.SetFocusOrder( button, 1 );
  manager.SetCurrentFocusActor( button );

  manager.ActionActivateSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionActivateEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionActivateSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionActivateSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionActivateSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionReadSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionReadSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionReadSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionReadEvent( 100.0f, 200.0f, true );

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionReadSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionReadSignalN" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionReadSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionOverSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionOverSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionOverSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  // Note that the ActionOverSignal is provoked by a read even when "allow read again" is set to false.
  accessibilityAdaptor.HandleActionReadEvent( 100.0f, 200.0f, false );

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionOverSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionOverSignalN" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionOverSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionReadNextSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionReadNextSignalP" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionReadNextSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionReadNextEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionReadNextSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionReadNextSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionReadNextSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionReadPreviousSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionReadPreviousSignalP" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionReadPreviousSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionReadPreviousEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionReadPreviousSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionReadPreviousSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionReadPreviousSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionUpSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionUpSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accessibilityAdaptor, true );
  accessibilityAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionUpSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  DummyControl dummyControl = DummyControl::New(true);
  dummyControl.SetSize(480, 800);
  manager.SetFocusOrder( dummyControl, 1 );
  Stage::GetCurrent().Add( dummyControl );
  manager.SetCurrentFocusActor( dummyControl );

  accessibilityAdaptor.HandleActionUpEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionUpSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionUpSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionUpSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionDownSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionDownSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accessibilityAdaptor, true );
  accessibilityAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionDownSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::Toolkit::PushButton button = Dali::Toolkit::PushButton::New();
  button.SetSize(480, 800);
  Stage::GetCurrent().Add(button);
  manager.SetFocusOrder( button, 1 );
  manager.SetCurrentFocusActor( button );

  accessibilityAdaptor.HandleActionDownEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionDownSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionDownSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionDownSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionClearFocusSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionClearFocusSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionClearFocusSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionClearFocusEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionClearFocusSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionClearFocusSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionClearFocusSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionBackSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionBackSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionBackSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionBackEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionBackSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionBackSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionBackSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionScrollUpSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionScrollUpSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionScrollUpSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionScrollUpEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionScrollUpSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionScrollUpSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionScrollUpSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionScrollDownSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionScrollDownSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionScrollDownSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionScrollDownEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionScrollDownSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionScrollDownSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionScrollDownSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionPageLeftSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionPageLeftSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionPageLeftSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionPageLeftEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionPageLeftSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionPageLeftSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionPageLeftSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionPageRightSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionPageRightSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionPageRightSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionPageRightEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionPageRightSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionPageRightSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionPageRightSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionPageUpSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionPageUpSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionPageUpSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionPageUpEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionPageUpSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionPageUpSignalN" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionPageUpSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionPageDownSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionPageDownSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionPageDownSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionPageDownEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionPageDownSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionPageDownSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionPageDownSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionMoveToFirstSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionMoveToFirstSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionMoveToFirstSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionMoveToFirstEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}


int UtcDaliAccessibilityManagerActionMoveToFirstSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionMoveToFirstSignalN" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionMoveToFirstSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionMoveToLastSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionMoveToLastSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionMoveToLastSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionMoveToLastEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionMoveToLastSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionMoveToLastSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionMoveToLastSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionReadFromTopSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionReadFromTopSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionReadFromTopSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionReadFromTopEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionReadFromTopSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionReadFromTopSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionReadFromTopSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionReadFromNextSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionReadFromNextSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionReadFromNextSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionReadFromNextEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionReadFromNextSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionReadFromNextSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionReadFromNextSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionZoomSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionZoomSignalP" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  Dali::Toolkit::PushButton button = Dali::Toolkit::PushButton::New();
  button.SetSize(480, 800);
  Stage::GetCurrent().Add(button);
  manager.SetFocusOrder( button, 1 );
  manager.SetCurrentFocusActor( button );

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  manager.ActionZoomSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionZoomEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionZoomSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionZoomSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionZoomSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionReadIndicatorInformationSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionReadIndicatorInformationSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionReadIndicatorInformationSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionReadIndicatorInformationEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionReadIndicatorInformationSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionReadIndicatorInformationSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionReadIndicatorInformationSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionReadPauseResumeSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionReadPauseResumeSignalP" );

  AccessibilityManagerSignalHandler callback;

  Dali::AccessibilityAdaptor accAdaptor = Dali::AccessibilityAdaptor::Get();
  Test::AccessibilityAdaptor::SetEnabled( accAdaptor, true );
  accAdaptor.HandleActionEnableEvent();

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionReadPauseResumeSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionReadPauseResumeEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionReadPauseResumeSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionReadPauseResumeSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionReadPauseResumeSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionStartStopSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionStartStopSignalP" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionStartStopSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  accessibilityAdaptor.HandleActionStartStopEvent();

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionStartStopSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionStartStopSignalN" );

  AccessibilityManagerSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionStartStopSignal().Connect( &callback, &AccessibilityManagerSignalHandler::Callback );
  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

// Functor to test if a accessibility scroll signal has been called.
class AccessibilityManagerScrollSignalHandler : public Dali::ConnectionTracker
{
public:
  AccessibilityManagerScrollSignalHandler() :
    mCalls( 0 )
  {
  }

  bool Callback( AccessibilityManager& accessibilityManager, const Dali::TouchEvent& touchEvent )
  {
    mCalls++;
    mTouchEvent = touchEvent;
    tet_infoline( "Signal called" );
    return true;
  }

  unsigned int GetCalls() const
  {
    return mCalls;
  }

  const Dali::TouchEvent& GetTouchEvent() const
  {
    return mTouchEvent;
  }

private:
  unsigned int mCalls;         ///< Keeps track of how many times the signal has been called.
  Dali::TouchEvent mTouchEvent; ///< Stores the last touch event received.
};

int UtcDaliAccessibilityManagerActionScrollSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionScrollSignalP" );

  AccessibilityManagerScrollSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionScrollSignal().Connect( &callback, &AccessibilityManagerScrollSignalHandler::Callback );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();

  TouchPoint point( 0, TouchPoint::Started, 100.0f, 200.0f );
  accessibilityAdaptor.HandleActionScrollEvent( point, 0u );

  DALI_TEST_EQUALS( callback.GetCalls(), 1u, TEST_LOCATION );

  const TouchEvent& signalTouchEvent = callback.GetTouchEvent();
  DALI_TEST_EQUALS( signalTouchEvent.GetPointCount(), 1u, TEST_LOCATION );

  const TouchPoint& signalTouchPoint = signalTouchEvent.GetPoint( 0u );

  DALI_TEST_EQUALS( signalTouchPoint.state, TouchPoint::Started, TEST_LOCATION );
  DALI_TEST_EQUALS( signalTouchPoint.screen.x, 100.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( signalTouchPoint.screen.y, 200.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionScrollSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionScrollSignalN" );

  AccessibilityManagerScrollSignalHandler callback;

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  manager.ActionScrollSignal().Connect( &callback, &AccessibilityManagerScrollSignalHandler::Callback );

  DALI_TEST_EQUALS( callback.GetCalls(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityManagerActionTouch(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerActionTouch" );

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();

  DummyControl dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyControl.SetSize(480, 800);
  manager.SetFocusOrder( dummyControl, 1 );
  Stage::GetCurrent().Add( dummyControl );
  manager.SetCurrentFocusActor( dummyControl );

  TouchPoint point( 0, TouchPoint::Started, 100.0f, 200.0f );
  accessibilityAdaptor.HandleActionTouchEvent( point, 0u );

  DALI_TEST_CHECK( dummyImpl.onAccTouchedCalled );

  END_TEST;
}


int UtcDaliAccessibilityManagerHandlePanGesture(void)
{
  // Pan gesture sent from adaptor to manager via AccessibilityGestureHandler
  // Adaptor.SetGestureHandler is called in Initialize (check it's the toolkit version)
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliAccessibilityManagerHandlePanGesture" );

  AccessibilityManager manager = AccessibilityManager::Get();
  DALI_TEST_CHECK( manager );

  Dali::AccessibilityAdaptor accessibilityAdaptor = Dali::AccessibilityAdaptor::Get();
  DummyControl dummyControl = DummyControl::New(true);
  dummyControl.SetSize(480, 800);
  Stage::GetCurrent().Add( dummyControl );

  Dali::Integration::PanGestureEvent panGestureEvent(Gesture::Started);
  panGestureEvent.previousPosition = Vector2(0.f, 0.f);
  panGestureEvent.currentPosition = Vector2(100.f, 0.f);
  panGestureEvent.timeDelta = 16;
  panGestureEvent.numberOfTouches = 1;

  Test::AccessibilityAdaptor::SendPanGesture( accessibilityAdaptor, panGestureEvent );

  panGestureEvent.state = Gesture::Continuing;
  panGestureEvent.previousPosition = Vector2(100.f, 0.f);
  panGestureEvent.currentPosition = Vector2(200.f, 0.f);
  Test::AccessibilityAdaptor::SendPanGesture( accessibilityAdaptor, panGestureEvent );

  panGestureEvent.state = Gesture::Finished;
  panGestureEvent.previousPosition = Vector2(200.f, 0.f);
  panGestureEvent.currentPosition = Vector2(300.f, 0.f);
  Test::AccessibilityAdaptor::SendPanGesture( accessibilityAdaptor, panGestureEvent );


  END_TEST;
}

// Methods missing coverage:
// IsActorFocusableFunction
// DoActivate
// SetFocusable
// TtsStateChanged
