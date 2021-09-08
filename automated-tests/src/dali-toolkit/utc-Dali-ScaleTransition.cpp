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
#include <dali-toolkit/public-api/transition/scale-transition.h>

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

int UtcDaliScaleTransitionSetGetProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliScaleTransitionSetGetProperty");

  Control control = Control::New();

  ScaleTransition scale = ScaleTransition::New(control, 0.5f, TimePeriod(-0.5f, -0.5f));

  Vector2 scaleFactor = scale.GetScaleFactor();
  DALI_TEST_EQUALS(0.5f, scaleFactor.x, TEST_LOCATION);
  DALI_TEST_EQUALS(0.5f, scaleFactor.y, TEST_LOCATION);


  scale.SetScaleFactor(Vector2(1.5f, 1.2f));
  scaleFactor = scale.GetScaleFactor();
  DALI_TEST_EQUALS(1.5f, scaleFactor.x, TEST_LOCATION);
  DALI_TEST_EQUALS(1.2f, scaleFactor.y, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScaleTransitionWithOffScene(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliScaleTransitionWithOffScene");

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

  Vector3 currentScale = control.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  DALI_TEST_CHECK(currentScale.x == 1.0f);
  DALI_TEST_CHECK(currentScale.y == 1.0f);

  ScaleTransition scale = ScaleTransition::New(control, 0.5f, TimePeriod(0.5f));
  scale.SetAppearingTransition(false);
  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(scale);
  transitionSet.Play();

  bool signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(400);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  currentScale = control.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  DALI_TEST_CHECK(currentScale.x == 1.0f);
  DALI_TEST_CHECK(currentScale.y == 1.0f);

  application.SendNotification();
  application.Render(200);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  application.SendNotification();
  application.Render(20);

  currentScale = control.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  DALI_TEST_CHECK(currentScale.x == 1.0f);
  DALI_TEST_CHECK(currentScale.y == 1.0f);

  END_TEST;
}

int UtcDaliScaleTransitionDisappearing(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliScaleTransitionDisappearing");

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

  Vector3 currentScale = control.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  DALI_TEST_CHECK(currentScale.x == 1.0f);
  DALI_TEST_CHECK(currentScale.y == 1.0f);

  ScaleTransition scale = ScaleTransition::New(control, 0.5f, TimePeriod(0.5f));
  scale.SetAppearingTransition(false);

  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(scale);
  transitionSet.Play();

  bool signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(400);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  currentScale = control.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  DALI_TEST_CHECK(currentScale.x > 0.55f && currentScale.x < 0.65f);
  DALI_TEST_CHECK(currentScale.y > 0.55f && currentScale.y < 0.65f);

  application.SendNotification();
  application.Render(200);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  application.SendNotification();
  application.Render(20);

  // Property is reset after animation.
  currentScale = control.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  DALI_TEST_CHECK(currentScale.x == 1.0f);
  DALI_TEST_CHECK(currentScale.y == 1.0f);

  END_TEST;
}

int UtcDaliScaleTransitionAppearing(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliScaleTransitionAppearing");

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

  Vector3 currentScale = control.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  DALI_TEST_CHECK(currentScale.x == 1.0f);
  DALI_TEST_CHECK(currentScale.y == 1.0f);

  ScaleTransition scale = ScaleTransition::New(control, Vector2(2.0, 0.5), TimePeriod(0.5f));
  scale.SetAppearingTransition(true);
  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(scale);
  transitionSet.Play();

  bool signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(400);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  currentScale = control.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  DALI_TEST_CHECK(currentScale.x > 1.1f && currentScale.x < 1.3f);
  DALI_TEST_CHECK(currentScale.y > 0.85f && currentScale.y < 0.95f);

  application.SendNotification();
  application.Render(200);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  application.SendNotification();
  application.Render(20);

  currentScale = control.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  DALI_TEST_CHECK(currentScale.x == 1.0f);
  DALI_TEST_CHECK(currentScale.y == 1.0f);

  END_TEST;
}
