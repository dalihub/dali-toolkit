/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/public-api/transition/transition-set.h>
#include <dali-toolkit/public-api/transition/transition-base.h>
#include <dali-toolkit/public-api/transition/fade-transition.h>

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

int UtcDaliFadeTransitionSetGetProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliFadeTransitionSetGetProperty");

  Control control = Control::New();

  FadeTransition fade = FadeTransition::New(control, 0.5, TimePeriod(-0.5f, -0.5f));

  TimePeriod timePeriod = fade.GetTimePeriod();
  DALI_TEST_EQUALS(0.0f, timePeriod.delaySeconds, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, timePeriod.durationSeconds, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFadeTransitionWithOffScene(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliFadeTransitionWithOffScene");

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  control.SetProperty(Actor::Property::POSITION, Vector3(100, 200, 0));
  control.SetProperty(Actor::Property::SIZE, Vector3(150, 150, 0));
  control.SetProperty(Actor::Property::OPACITY, 1.0f);
  Property::Map controlProperty;
  controlProperty.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty);

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(1.0f, control.GetCurrentProperty<float>(Actor::Property::OPACITY), TEST_LOCATION);

  FadeTransition fade = FadeTransition::New(control, 0.5, TimePeriod(0.5f));
  fade.SetAppearingTransition(false); // set fade out
  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(fade);
  transitionSet.Play();

  bool signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(400);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  DALI_TEST_EQUALS(1.0f, control.GetCurrentProperty<float>(Actor::Property::OPACITY), TEST_LOCATION);
  
  application.SendNotification();
  application.Render(200);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(1.0f, control.GetCurrentProperty<float>(Actor::Property::OPACITY), TEST_LOCATION);

  END_TEST;
}

int UtcDaliFadeTransitionDisappearing(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliFadeTransitionOut");

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  control.SetProperty(Actor::Property::POSITION, Vector3(100, 200, 0));
  control.SetProperty(Actor::Property::SIZE, Vector3(150, 150, 0));
  control.SetProperty(Actor::Property::OPACITY, 1.0f);
  Property::Map controlProperty;
  controlProperty.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty);

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(1.0f, control.GetCurrentProperty<float>(Actor::Property::OPACITY), TEST_LOCATION);

  FadeTransition fade = FadeTransition::New(control, 0.5, TimePeriod(0.5f));
  fade.SetAppearingTransition(false); // set fade out
  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(fade);
  transitionSet.Play();

  bool signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(400);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  float currentOpacity = control.GetCurrentProperty<float>(Actor::Property::OPACITY);
  DALI_TEST_CHECK(currentOpacity <= 0.7 && currentOpacity >= 0.5);
  
  application.SendNotification();
  application.Render(200);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  application.SendNotification();
  application.Render(20);

  // Property is reset after animation.
  DALI_TEST_EQUALS(1.0f, control.GetCurrentProperty<float>(Actor::Property::OPACITY), TEST_LOCATION);

  END_TEST;
}

int UtcDaliFadeTransitionAppearing(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliFadeTransitionIn");

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  control.SetProperty(Actor::Property::POSITION, Vector3(100, 200, 0));
  control.SetProperty(Actor::Property::SIZE, Vector3(150, 150, 0));
  control.SetProperty(Actor::Property::OPACITY, 1.0f);
  Property::Map controlProperty;
  controlProperty.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty);

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(1.0f, control.GetCurrentProperty<float>(Actor::Property::OPACITY), TEST_LOCATION);

  FadeTransition fade = FadeTransition::New(control, 0.5, TimePeriod(0.5f));
  fade.SetAppearingTransition(true); // set fade in
  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(fade);
  transitionSet.Play();

  bool signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(400);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  float currentOpacity = control.GetCurrentProperty<float>(Actor::Property::OPACITY);
  DALI_TEST_CHECK(currentOpacity <= 1.0 && currentOpacity >= 0.8);
  
  application.SendNotification();
  application.Render(200);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(1.0f, control.GetCurrentProperty<float>(Actor::Property::OPACITY), TEST_LOCATION);

  END_TEST;
}


int UtcDaliFadeTransitionAppearingWithDelay(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliFadeTransitionIn");

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  control.SetProperty(Actor::Property::POSITION, Vector3(100, 200, 0));
  control.SetProperty(Actor::Property::SIZE, Vector3(150, 150, 0));
  control.SetProperty(Actor::Property::OPACITY, 1.0f);
  Property::Map controlProperty;
  controlProperty.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty);

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(1.0f, control.GetCurrentProperty<float>(Actor::Property::OPACITY), TEST_LOCATION);

  FadeTransition fade = FadeTransition::New(control, 0.5, TimePeriod(0.5f, 0.5f));
  fade.SetAppearingTransition(true); // set fade in
  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(fade);
  transitionSet.Play();

  bool signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(400);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  float currentOpacity = control.GetCurrentProperty<float>(Actor::Property::OPACITY);
  DALI_TEST_CHECK(currentOpacity <= 0.01f);

  application.SendNotification();
  application.Render(500);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  currentOpacity = control.GetCurrentProperty<float>(Actor::Property::OPACITY);
  DALI_TEST_CHECK(currentOpacity <= 1.0 && currentOpacity >= 0.8);

  application.SendNotification();
  application.Render(200);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(1.0f, control.GetCurrentProperty<float>(Actor::Property::OPACITY), TEST_LOCATION);

  END_TEST;
}
