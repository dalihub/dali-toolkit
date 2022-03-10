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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/public-api/transition/slide-transition.h>
#include <dali-toolkit/public-api/transition/transition-base.h>
#include <dali-toolkit/public-api/transition/transition-set.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <stdlib.h>
#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;

// Functor to test whether a Finish signal is emitted
struct TransitionFinishCheck
{
  TransitionFinishCheck(bool& signalReceived)
  : mSignalReceived(signalReceived)
  {
  }

  void operator()(TransitionSet& transitionSet)
  {
    mSignalReceived = true;
  }

  void Reset()
  {
    mSignalReceived = false;
  }

  void CheckSignalReceived()
  {
    if(!mSignalReceived)
    {
      tet_printf("Expected Finish signal was not received\n");
      tet_result(TET_FAIL);
    }
    else
    {
      tet_result(TET_PASS);
    }
  }

  void CheckSignalNotReceived()
  {
    if(mSignalReceived)
    {
      tet_printf("Unexpected Finish signal was received\n");
      tet_result(TET_FAIL);
    }
    else
    {
      tet_result(TET_PASS);
    }
  }

  bool& mSignalReceived; // owned by individual tests
};

int UtcDaliSlideTransitionSetGetProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliSlideTransitionSetGetProperty");

  Control control = Control::New();

  SlideTransition slide = SlideTransition::New(control, Dali::Toolkit::SlideTransitionDirection::TOP, TimePeriod(-0.5f, -0.5f));

  TimePeriod timePeriod = slide.GetTimePeriod();
  DALI_TEST_EQUALS(0.0f, timePeriod.delaySeconds, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, timePeriod.durationSeconds, TEST_LOCATION);

  DALI_TEST_EQUALS(Dali::Toolkit::SlideTransitionDirection::TOP, slide.GetDirection(), TEST_LOCATION);

  slide.SetDirection(Dali::Toolkit::SlideTransitionDirection::BOTTOM);
  DALI_TEST_EQUALS(Dali::Toolkit::SlideTransitionDirection::BOTTOM, slide.GetDirection(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliSlideTransitionWithOffScene(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliSlideTransitionWithOffScene");

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  control.SetProperty(Actor::Property::SIZE, Vector3(150, 150, 0));
  Property::Map controlProperty;
  controlProperty.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty);

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(Vector2(0, 0), control.GetCurrentProperty<Vector2>(Actor::Property::POSITION), TEST_LOCATION);

  SlideTransition slide = SlideTransition::New(control, Dali::Toolkit::SlideTransitionDirection::BOTTOM, TimePeriod(0.5f));
  slide.SetAppearingTransition(false);
  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(slide);
  transitionSet.Play();

  bool                  signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(400);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  DALI_TEST_EQUALS(Vector2(0, 0), control.GetCurrentProperty<Vector2>(Actor::Property::POSITION), TEST_LOCATION);

  application.SendNotification();
  application.Render(200);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(Vector2(0, 0), control.GetCurrentProperty<Vector2>(Actor::Property::POSITION), TEST_LOCATION);

  END_TEST;
}

int UtcDaliSlideTransitionOut(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliSlideTransitionOut");

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  control.SetProperty(Actor::Property::SIZE, Vector3(100, 100, 0));
  control.SetProperty(Actor::Property::OPACITY, 1.0f);
  Property::Map controlProperty;
  controlProperty.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty);

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(Vector2(0, 0), control.GetCurrentProperty<Vector2>(Actor::Property::POSITION), TEST_LOCATION);

  Vector2         windowSize = application.GetScene().GetSize();
  SlideTransition slide      = SlideTransition::New(control, Dali::Toolkit::SlideTransitionDirection::BOTTOM, TimePeriod(0.5f));
  slide.SetAppearingTransition(false);

  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(slide);
  transitionSet.Play();

  bool                  signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(400);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  Vector2 currentPosition = control.GetCurrentProperty<Vector2>(Actor::Property::POSITION);
  currentPosition += (windowSize / 2.0f) - Vector2(50, 50);
  DALI_TEST_CHECK(currentPosition.y >= 0.7 * windowSize.y && currentPosition.y <= 0.9 * windowSize.y);

  application.SendNotification();
  application.Render(200);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  application.SendNotification();
  application.Render(20);

  // Property is reset after animation.
  DALI_TEST_EQUALS(Vector2(0, 0), control.GetCurrentProperty<Vector2>(Actor::Property::POSITION), TEST_LOCATION);

  END_TEST;
}

int UtcDaliSlideTransitionIn(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliSlideTransitionIn");

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  control.SetProperty(Actor::Property::SIZE, Vector3(100, 100, 0));
  control.SetProperty(Actor::Property::OPACITY, 1.0f);
  Property::Map controlProperty;
  controlProperty.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty);

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(Vector2(0, 0), control.GetCurrentProperty<Vector2>(Actor::Property::POSITION), TEST_LOCATION);

  Vector2 windowSize = application.GetScene().GetSize();

  SlideTransition slide = SlideTransition::New(control, Dali::Toolkit::SlideTransitionDirection::RIGHT, TimePeriod(0.5f));
  slide.SetAppearingTransition(true);
  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(slide);
  transitionSet.Play();

  bool                  signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(400);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  Vector2 currentPosition = control.GetCurrentProperty<Vector2>(Actor::Property::POSITION);
  currentPosition += (windowSize / 2.0f) - Vector2(50, 50);
  DALI_TEST_CHECK(currentPosition.x >= 0.1 * windowSize.x && currentPosition.x <= 0.3 * windowSize.x);

  application.SendNotification();
  application.Render(200);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(Vector2(0, 0), control.GetCurrentProperty<Vector2>(Actor::Property::POSITION), TEST_LOCATION);

  END_TEST;
}
