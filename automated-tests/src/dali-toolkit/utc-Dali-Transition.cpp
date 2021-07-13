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
#include <dali-toolkit/public-api/transition/transition.h>

using namespace Dali;
using namespace Dali::Toolkit;
namespace
{

const char* TEST_IMAGE_FILE_NAME = TEST_RESOURCE_DIR "/gallery-small-1.jpg";

}

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

int UtcDaliTransitionSetGetProperty01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTransitionSetGetProperty01");

  Control control1 = Control::New();
  control1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  control1.SetProperty(Actor::Property::POSITION, Vector3(100, 200, 0));
  Property::Map controlProperty1;
  controlProperty1.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty1.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  controlProperty1.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, 50.f);
  controlProperty1.Insert(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, 50.f);
  controlProperty1.Insert(Toolkit::DevelVisual::Property::BORDERLINE_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  controlProperty1.Insert(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, 1.f);
  control1.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty1);

  Control control2 = Control::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control2.SetProperty(Actor::Property::ANCHOR_POINT, ParentOrigin::TOP_LEFT);
  control2.SetProperty(Actor::Property::POSITION, Vector3(50, 50, 0));
  Property::Map controlProperty2;
  controlProperty2.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty2.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 1.0f, 0.0f, 0.5f));
  controlProperty2.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, 30.f);
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, 30.f);
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_COLOR, Vector4(1.0f, 1.0f, 0.0f, 0.5f));
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, -1.f);
  control2.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty2);

  application.GetScene().Add(control1);
  application.GetScene().Add(control2);

  application.SendNotification();
  application.Render(20);

  Transition transition = Transition::New(control1, control2, TimePeriod(-0.1f, -0.1f));
  TimePeriod timePeriod = transition.GetTimePeriod();
  DALI_TEST_EQUALS(0.0f, timePeriod.durationSeconds, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, timePeriod.delaySeconds, TEST_LOCATION);

  transition.SetTimePeriod(TimePeriod(0.5f, 1.0f));
  timePeriod = transition.GetTimePeriod();
  DALI_TEST_EQUALS(1.0f, timePeriod.durationSeconds, TEST_LOCATION);
  DALI_TEST_EQUALS(0.5f, timePeriod.delaySeconds, TEST_LOCATION);

  DALI_TEST_EQUALS(Dali::AlphaFunction::DEFAULT, transition.GetAlphaFunction().GetBuiltinFunction(), TEST_LOCATION);
  transition.SetAlphaFunction(Dali::AlphaFunction::EASE_IN_OUT);
  DALI_TEST_EQUALS(Dali::AlphaFunction::EASE_IN_OUT, transition.GetAlphaFunction().GetBuiltinFunction(), TEST_LOCATION);

  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(transition);

  DALI_TEST_EQUALS(1, transitionSet.GetTransitionCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(transition, transitionSet.GetTransitionAt(0), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTransitionSetGetProperty02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTransitionSetGetProperty02");

  Control control1 = Control::New();
  control1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  control1.SetProperty(Actor::Property::POSITION, Vector3(100, 200, 0));
  Property::Map controlProperty1;
  controlProperty1.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty1.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  controlProperty1.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, Vector4(50.0f, 30.0f, 40.0f, 20.0f));
  controlProperty1.Insert(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, 50.f);
  controlProperty1.Insert(Toolkit::DevelVisual::Property::BORDERLINE_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  controlProperty1.Insert(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, -1.f);
  control1.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty1);

  Control control2 = Control::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control2.SetProperty(Actor::Property::ANCHOR_POINT, ParentOrigin::TOP_LEFT);
  control2.SetProperty(Actor::Property::POSITION, Vector3(50, 50, 0));
  Property::Map controlProperty2;
  controlProperty2.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty2.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 1.0f, 0.0f, 0.5f));
  controlProperty2.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, Vector4(32.f, 54.0f, 24.0f, 42.0f));
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, 30.f);
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_COLOR, Vector4(1.0f, 1.0f, 0.0f, 0.5f));
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, -1.f);
  control2.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty2);

  application.GetScene().Add(control1);
  application.GetScene().Add(control2);

  application.SendNotification();
  application.Render(20);

  Transition transition = Transition::New(control1, control2, TimePeriod(-0.1f));
  TimePeriod timePeriod = transition.GetTimePeriod();
  DALI_TEST_EQUALS(0.0f, timePeriod.durationSeconds, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, timePeriod.delaySeconds, TEST_LOCATION);

  transition.SetTimePeriod(TimePeriod(0.5f, 1.0f));
  timePeriod = transition.GetTimePeriod();
  DALI_TEST_EQUALS(1.0f, timePeriod.durationSeconds, TEST_LOCATION);
  DALI_TEST_EQUALS(0.5f, timePeriod.delaySeconds, TEST_LOCATION);

  DALI_TEST_EQUALS(Dali::AlphaFunction::DEFAULT, transition.GetAlphaFunction().GetBuiltinFunction(), TEST_LOCATION);
  transition.SetAlphaFunction(Dali::AlphaFunction::EASE_IN_OUT);
  DALI_TEST_EQUALS(Dali::AlphaFunction::EASE_IN_OUT, transition.GetAlphaFunction().GetBuiltinFunction(), TEST_LOCATION);

  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(transition);

  DALI_TEST_EQUALS(1, transitionSet.GetTransitionCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(transition, transitionSet.GetTransitionAt(0), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTransitionBetweenControlPair(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTransitionBetweenControlPair");

  Vector3 destinationPosition(50, 50, 0);
  Vector3 destinationSize(120, 120, 0);
  Vector3 destinationScale(2, 1, 0);
  Vector4 destinationColor(1.0f, 0.5f, 1.0f, 0.8f);
  float destinationOpacity(0.8f);
  float destinationRadius(50.f);
  float destinationBorderlineWidth(80.0f);
  Vector4 destinationBorderlineColor(0.5f, 1.0f, 0.5f, 0.3f);
  float destinationBorderlineOffset(-1.0f);
  Vector4 destinationRadiusV4 = Vector4(destinationRadius, destinationRadius, destinationRadius, destinationRadius);

  Control control1 = Control::New();
  control1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  control1.SetProperty(Actor::Property::POSITION, Vector3(100, 200, 0));
  control1.SetProperty(Actor::Property::SIZE, Vector3(150, 150, 0));
  control1.SetProperty(Actor::Property::SCALE, Vector3(1, 2, 0));
  control1.SetProperty(Actor::Property::COLOR, Vector4(1.0f, 1.0f, 1.0f, 0.5f));
  control1.SetProperty(Actor::Property::OPACITY, 0.5f);
  Property::Map controlProperty1;
  controlProperty1.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty1.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  controlProperty1.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, 30.f);
  controlProperty1.Insert(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, 60.f);
  controlProperty1.Insert(Toolkit::DevelVisual::Property::BORDERLINE_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  controlProperty1.Insert(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, 1.f);
  control1.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty1);

  Control control2 = Control::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control2.SetProperty(Actor::Property::ANCHOR_POINT, ParentOrigin::TOP_LEFT);
  control2.SetProperty(Actor::Property::POSITION, destinationPosition);
  control2.SetProperty(Actor::Property::SIZE, destinationSize);
  control2.SetProperty(Actor::Property::SCALE, destinationScale);
  control2.SetProperty(Actor::Property::COLOR, destinationColor);
  control2.SetProperty(Actor::Property::OPACITY, destinationOpacity);
  Property::Map controlProperty2;
  controlProperty2.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty2.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 1.0f, 0.0f, 0.5f));
  controlProperty2.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, destinationRadius);
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, destinationBorderlineWidth);
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_COLOR, destinationBorderlineColor);
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, destinationBorderlineOffset);
  control2.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty2);

  DALI_TEST_EQUALS(destinationPosition, control2.GetProperty<Vector3>(Actor::Property::POSITION), TEST_LOCATION);
  Property::Map backgroundMap = control2.GetProperty<Property::Map>(Toolkit::Control::Property::BACKGROUND);
  Vector4 cornerRadius = backgroundMap.Find(Toolkit::DevelVisual::Property::CORNER_RADIUS)->Get<Vector4>();
  DALI_TEST_EQUALS(destinationRadiusV4, cornerRadius, TEST_LOCATION);
  float borderlineWidth = backgroundMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH)->Get<float>();
  DALI_TEST_EQUALS(destinationBorderlineWidth, borderlineWidth, TEST_LOCATION);
  Vector4 borderlineColor = backgroundMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_COLOR)->Get<Vector4>();
  DALI_TEST_EQUALS(destinationBorderlineColor, borderlineColor, TEST_LOCATION);
  float borderlineOffset = backgroundMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET)->Get<float>();
  DALI_TEST_EQUALS(destinationBorderlineOffset, borderlineOffset, TEST_LOCATION);

  application.GetScene().Add(control1);
  application.GetScene().Add(control2);

  application.SendNotification();
  application.Render(20);

  Transition transition = Transition::New(control1, control2, TimePeriod(0.5f));
  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(transition);
  transitionSet.Play();

  bool signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(50);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  DALI_TEST_NOT_EQUALS(destinationPosition, control2.GetCurrentProperty<Vector3>(Actor::Property::POSITION), 0.00001f, TEST_LOCATION);
  DALI_TEST_EQUALS(1, control2.GetRendererCount(), TEST_LOCATION);
  Dali::Renderer renderer = control2.GetRendererAt(0);
  Property::Index index = renderer.GetPropertyIndex(DevelVisual::Property::CORNER_RADIUS);
  cornerRadius = renderer.GetCurrentProperty<Vector4>(index);
  DALI_TEST_NOT_EQUALS(destinationRadiusV4, cornerRadius, 0.00001f, TEST_LOCATION);
  index = renderer.GetPropertyIndex(DevelVisual::Property::BORDERLINE_WIDTH);
  borderlineWidth = renderer.GetCurrentProperty<float>(index);
  DALI_TEST_NOT_EQUALS(destinationBorderlineWidth, borderlineWidth, 0.00001f, TEST_LOCATION);
  index = renderer.GetPropertyIndex(DevelVisual::Property::BORDERLINE_COLOR);
  borderlineColor = renderer.GetCurrentProperty<Vector4>(index);
  DALI_TEST_NOT_EQUALS(destinationBorderlineColor, borderlineColor, 0.00001f, TEST_LOCATION);
  index = renderer.GetPropertyIndex(DevelVisual::Property::BORDERLINE_OFFSET);
  borderlineOffset = renderer.GetCurrentProperty<float>(index);
  DALI_TEST_NOT_EQUALS(destinationBorderlineOffset, borderlineOffset, 0.00001f, TEST_LOCATION);

  application.SendNotification();
  application.Render(700);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(destinationPosition, control2.GetCurrentProperty<Vector3>(Actor::Property::POSITION), TEST_LOCATION);
  DALI_TEST_EQUALS(destinationSize, control2.GetCurrentProperty<Vector3>(Actor::Property::SIZE), TEST_LOCATION);
  DALI_TEST_EQUALS(destinationScale, control2.GetCurrentProperty<Vector3>(Actor::Property::SCALE), TEST_LOCATION);
  DALI_TEST_EQUALS(destinationColor, control2.GetCurrentProperty<Vector4>(Actor::Property::COLOR), TEST_LOCATION);
  DALI_TEST_EQUALS(destinationOpacity, control2.GetCurrentProperty<float>(Actor::Property::OPACITY), TEST_LOCATION);
  DALI_TEST_EQUALS(1, control2.GetRendererCount(), TEST_LOCATION);
  renderer = control2.GetRendererAt(0);
  index = renderer.GetPropertyIndex(DevelVisual::Property::CORNER_RADIUS);
  cornerRadius = renderer.GetCurrentProperty<Vector4>(index);
  DALI_TEST_EQUALS(destinationRadiusV4, cornerRadius, TEST_LOCATION);
  index = renderer.GetPropertyIndex(DevelVisual::Property::BORDERLINE_WIDTH);
  borderlineWidth = renderer.GetCurrentProperty<float>(index);
  DALI_TEST_EQUALS(destinationBorderlineWidth, borderlineWidth, TEST_LOCATION);
  index = renderer.GetPropertyIndex(DevelVisual::Property::BORDERLINE_COLOR);
  borderlineColor = renderer.GetCurrentProperty<Vector4>(index);
  DALI_TEST_EQUALS(destinationBorderlineColor, borderlineColor, TEST_LOCATION);
  index = renderer.GetPropertyIndex(DevelVisual::Property::BORDERLINE_OFFSET);
  borderlineOffset = renderer.GetCurrentProperty<float>(index);
  DALI_TEST_EQUALS(destinationBorderlineOffset, borderlineOffset, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTransitionBetweenControlPairWithoutEmptySourceBackground(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTransitionBetweenControlPair");

  Vector4 destinationRadius(50.f, 30.f, 40.f, 0.f);
  float   destinationBorderlineWidth(40.f);
  Vector4 destinationBorderlineColor(1.0f, 0.5f, 0.2f, 0.8f);
  float   destinationBorderlineOffset(1.f);

  Control control1 = Control::New();
  control1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  control1.SetProperty(Actor::Property::POSITION, Vector3(100, 200, 0));

  Control control2 = Control::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control2.SetProperty(Actor::Property::ANCHOR_POINT, ParentOrigin::TOP_LEFT);
  control2.SetProperty(Actor::Property::POSITION, Vector3(50, 50, 0));
  Property::Map controlProperty2;
  controlProperty2.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty2.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 1.0f, 0.0f, 0.5f));
  controlProperty2.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, destinationRadius);
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, destinationBorderlineWidth);
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_COLOR, destinationBorderlineColor);
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, destinationBorderlineOffset);
  control2.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty2);

  Property::Map backgroundMap = control2.GetProperty<Property::Map>(Toolkit::Control::Property::BACKGROUND);
  Vector4 cornerRadius = backgroundMap.Find(Toolkit::DevelVisual::Property::CORNER_RADIUS)->Get<Vector4>();
  DALI_TEST_EQUALS(destinationRadius, cornerRadius, TEST_LOCATION);
  float borderlineWidth = backgroundMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH)->Get<float>();
  DALI_TEST_EQUALS(destinationBorderlineWidth, borderlineWidth, TEST_LOCATION);
  Vector4 borderlineColor = backgroundMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_COLOR)->Get<Vector4>();
  DALI_TEST_EQUALS(destinationBorderlineColor, borderlineColor, TEST_LOCATION);
  float borderlineOffset = backgroundMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET)->Get<float>();
  DALI_TEST_EQUALS(destinationBorderlineOffset, borderlineOffset, TEST_LOCATION);

  application.GetScene().Add(control1);
  application.GetScene().Add(control2);

  application.SendNotification();
  application.Render(20);

  Transition transition = Transition::New(control1, control2, TimePeriod(0.5f));
  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(transition);
  transitionSet.Play();

  bool signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(50);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  backgroundMap = control2.GetProperty<Property::Map>(Toolkit::Control::Property::BACKGROUND);
  cornerRadius = backgroundMap.Find(Toolkit::DevelVisual::Property::CORNER_RADIUS)->Get<Vector4>();
  DALI_TEST_EQUALS(destinationRadius, cornerRadius, TEST_LOCATION);
  borderlineWidth = backgroundMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH)->Get<float>();
  DALI_TEST_EQUALS(destinationBorderlineWidth, borderlineWidth, TEST_LOCATION);
  borderlineColor = backgroundMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_COLOR)->Get<Vector4>();
  DALI_TEST_EQUALS(destinationBorderlineColor, borderlineColor, TEST_LOCATION);
  borderlineOffset = backgroundMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET)->Get<float>();
  DALI_TEST_EQUALS(destinationBorderlineOffset, borderlineOffset, TEST_LOCATION);

  application.SendNotification();
  application.Render(700);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  application.SendNotification();
  application.Render(20);

  backgroundMap = control2.GetProperty<Property::Map>(Toolkit::Control::Property::BACKGROUND);
  cornerRadius = backgroundMap.Find(Toolkit::DevelVisual::Property::CORNER_RADIUS)->Get<Vector4>();
  DALI_TEST_EQUALS(destinationRadius, cornerRadius, TEST_LOCATION);
  borderlineWidth = backgroundMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH)->Get<float>();
  DALI_TEST_EQUALS(destinationBorderlineWidth, borderlineWidth, TEST_LOCATION);
  borderlineColor = backgroundMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_COLOR)->Get<Vector4>();
  DALI_TEST_EQUALS(destinationBorderlineColor, borderlineColor, TEST_LOCATION);
  borderlineOffset = backgroundMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET)->Get<float>();
  DALI_TEST_EQUALS(destinationBorderlineOffset, borderlineOffset, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTransitionBetweenImageViewPair(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTransitionBetweenControlPair");

  Vector3 destinationPosition(50, 50, 0);

  ImageView control1 = ImageView::New();
  control1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  control1.SetProperty(Actor::Property::POSITION, Vector3(100, 200, 0));
  control1.SetProperty(Actor::Property::SIZE, Vector3(150, 150, 0));
  Property::Map controlProperty1;
  controlProperty1.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  controlProperty1.Insert(Toolkit::ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME);
  controlProperty1.Insert(Toolkit::Visual::Property::MIX_COLOR, Vector4(1.0f, 1.0f, 0.5f, 0.5f));
  controlProperty1.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, 50.f);
  controlProperty1.Insert(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, 50.f);
  controlProperty1.Insert(Toolkit::DevelVisual::Property::BORDERLINE_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  controlProperty1.Insert(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, 1.f);
  control1.SetProperty(Toolkit::ImageView::Property::IMAGE, controlProperty1);

  ImageView control2 = ImageView::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control2.SetProperty(Actor::Property::ANCHOR_POINT, ParentOrigin::TOP_LEFT);
  control2.SetProperty(Actor::Property::POSITION, destinationPosition);
  control2.SetProperty(Actor::Property::SIZE, Vector3(120, 120, 0));
  Property::Map controlProperty2;
  controlProperty2.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  controlProperty2.Insert(Toolkit::ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME);
  controlProperty2.Insert(Toolkit::Visual::Property::MIX_COLOR, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
  controlProperty2.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, 30.f);
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, 30.f);
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_COLOR, Vector4(0.0f, 1.0f, 1.0f, 0.5f));
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, -1.f);
  control2.SetProperty(Toolkit::ImageView::Property::IMAGE, controlProperty2);

  DALI_TEST_EQUALS(destinationPosition, control2.GetProperty<Vector3>(Actor::Property::POSITION), TEST_LOCATION);

  application.GetScene().Add(control1);
  application.GetScene().Add(control2);

  application.SendNotification();
  application.Render(20);

  Vector3 startWorldPosition = control1.GetProperty<Vector3>(Actor::Property::WORLD_POSITION);
  Vector3 finishWorldPosition = control2.GetProperty<Vector3>(Actor::Property::WORLD_POSITION);

  Transition transition = Transition::New(control1, control2, TimePeriod(0.5f));
  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(transition);
  transitionSet.Play();

  bool signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(400);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // control2 moved about 80%
  Vector3 currentPosition = control2.GetCurrentProperty<Vector3>(Actor::Property::POSITION);
  Vector3 expectedPosition_0_7 = startWorldPosition + (finishWorldPosition - startWorldPosition) * 0.7;
  Vector3 expectedPosition_0_9 = startWorldPosition + (finishWorldPosition - startWorldPosition) * 0.9;
  DALI_TEST_CHECK(currentPosition.x <= expectedPosition_0_7.x && currentPosition.x >= expectedPosition_0_9.x);
  DALI_TEST_CHECK(currentPosition.y <= expectedPosition_0_7.y && currentPosition.y >= expectedPosition_0_9.y);

  application.SendNotification();
  application.Render(200);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  DALI_TEST_NOT_EQUALS(destinationPosition, control2.GetCurrentProperty<Vector3>(Actor::Property::POSITION), 0.00001f, TEST_LOCATION);

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(destinationPosition, control2.GetCurrentProperty<Vector3>(Actor::Property::POSITION), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTransitionBetweenImageViewPairWithDelay(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTransitionBetweenControlPair");

  Vector3 destinationPosition(50, 50, 0);

  ImageView control1 = ImageView::New();
  control1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  control1.SetProperty(Actor::Property::POSITION, Vector3(100, 200, 0));
  control1.SetProperty(Actor::Property::SIZE, Vector3(150, 150, 0));
  Property::Map controlProperty1;
  controlProperty1.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  controlProperty1.Insert(Toolkit::ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME);
  controlProperty1.Insert(Toolkit::Visual::Property::MIX_COLOR, Vector4(1.0f, 1.0f, 0.5f, 0.5f));
  controlProperty1.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, 50.f);
  controlProperty1.Insert(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, 50.f);
  controlProperty1.Insert(Toolkit::DevelVisual::Property::BORDERLINE_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  controlProperty1.Insert(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, 1.f);
  control1.SetProperty(Toolkit::ImageView::Property::IMAGE, controlProperty1);

  ImageView control2 = ImageView::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control2.SetProperty(Actor::Property::ANCHOR_POINT, ParentOrigin::TOP_LEFT);
  control2.SetProperty(Actor::Property::POSITION, destinationPosition);
  control2.SetProperty(Actor::Property::SIZE, Vector3(120, 120, 0));
  Property::Map controlProperty2;
  controlProperty2.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  controlProperty2.Insert(Toolkit::ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME);
  controlProperty2.Insert(Toolkit::Visual::Property::MIX_COLOR, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
  controlProperty2.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, 30.f);
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, 30.f);
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_COLOR, Vector4(0.0f, 1.0f, 1.0f, 0.5f));
  controlProperty2.Insert(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, -1.f);
  control2.SetProperty(Toolkit::ImageView::Property::IMAGE, controlProperty2);

  DALI_TEST_EQUALS(destinationPosition, control2.GetProperty<Vector3>(Actor::Property::POSITION), TEST_LOCATION);

  application.GetScene().Add(control1);
  application.GetScene().Add(control2);

  application.SendNotification();
  application.Render(20);

  Vector3 startWorldPosition = control1.GetProperty<Vector3>(Actor::Property::WORLD_POSITION);
  Vector3 finishWorldPosition = control2.GetProperty<Vector3>(Actor::Property::WORLD_POSITION);

  Transition transition = Transition::New(control1, control2, TimePeriod(0.5f, 0.5f));
  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(transition);
  transitionSet.Play();

  bool signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(400);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  DALI_TEST_EQUALS(startWorldPosition, control2.GetCurrentProperty<Vector3>(Actor::Property::POSITION), TEST_LOCATION);


  application.SendNotification();
  application.Render(400);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // control2 moved about 60% (800ms)
  Vector3 currentPosition = control2.GetCurrentProperty<Vector3>(Actor::Property::POSITION);
  Vector3 expectedPosition_0_5 = startWorldPosition + (finishWorldPosition - startWorldPosition) * 0.5;
  Vector3 expectedPosition_0_7 = startWorldPosition + (finishWorldPosition - startWorldPosition) * 0.7;
  DALI_TEST_CHECK(currentPosition.x <= expectedPosition_0_5.x && currentPosition.x >= expectedPosition_0_7.x);
  DALI_TEST_CHECK(currentPosition.y <= expectedPosition_0_5.y && currentPosition.y >= expectedPosition_0_7.y);

  application.SendNotification();
  application.Render(400);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  DALI_TEST_NOT_EQUALS(destinationPosition, control2.GetCurrentProperty<Vector3>(Actor::Property::POSITION), 0.00001f, TEST_LOCATION);

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(destinationPosition, control2.GetCurrentProperty<Vector3>(Actor::Property::POSITION), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTransitionBetweenControlPairWithTree(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTransitionBetweenControlPairWithTree");

  Vector3 destinationPosition(50, 50, 0);
  Vector3 destinationWorldPosition(-130, -290, 0);

  Control control1 = Control::New();
  control1.SetProperty(Actor::Property::POSITION, Vector3(100, 200, 0));
  control1.SetProperty(Actor::Property::SIZE, Vector3(150, 150, 0));
  Property::Map controlProperty1;
  controlProperty1.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty1.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control1.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty1);

  Control control2 = Control::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  control2.SetProperty(Actor::Property::POSITION, destinationPosition);
  control2.SetProperty(Actor::Property::SIZE, Vector3(120, 120, 0));
  Property::Map controlProperty2;
  controlProperty2.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty2.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control2.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty2);

  Control control3 = Control::New();
  control3.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control3.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  control3.SetProperty(Actor::Property::POSITION, Vector3(50, 50, 0));
  control3.SetProperty(Actor::Property::SIZE, Vector3(120, 120, 0));
  Property::Map controlProperty3;
  controlProperty3.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty3.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control3.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty3);

  application.GetScene().Add(control1);
  application.GetScene().Add(control2);
  control2.Add(control3);

  application.SendNotification();
  application.Render(20);

  Transition transition = Transition::New(control1, control2, TimePeriod(0.5f));
  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(transition);
  transitionSet.Play();

  bool signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(600);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(destinationPosition, control2.GetCurrentProperty<Vector3>(Actor::Property::POSITION), TEST_LOCATION);
  DALI_TEST_EQUALS(destinationWorldPosition, control2.GetProperty<Vector3>(Actor::Property::WORLD_POSITION), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTransitionBetweenControlPairWithTreeWithChild(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTransitionBetweenControlPairWithTreeWithChild");

  Vector3 destinationWorldPosition(-80, -240, 0);

  Control control1 = Control::New();
  control1.SetProperty(Actor::Property::POSITION, Vector3(100, 200, 0));
  control1.SetProperty(Actor::Property::SIZE, Vector3(150, 150, 0));
  Property::Map controlProperty1;
  controlProperty1.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty1.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control1.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty1);

  Control control2 = Control::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  control2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  control2.SetProperty(Actor::Property::POSITION, Vector3(50, 50, 0));
  control2.SetProperty(Actor::Property::SIZE, Vector3(120, 120, 0));
  Property::Map controlProperty2;
  controlProperty2.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty2.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control2.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty2);

  Control control3 = Control::New();
  control3.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control3.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  control3.SetProperty(Actor::Property::POSITION, Vector3(50, 50, 0));
  control3.SetProperty(Actor::Property::SIZE, Vector3(120, 120, 0));
  Property::Map controlProperty3;
  controlProperty3.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty3.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control3.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty3);

  application.GetScene().Add(control1);
  application.GetScene().Add(control2);
  control2.Add(control3);

  application.SendNotification();
  application.Render(20);

  Transition transition = Transition::New(control1, control2, TimePeriod(0.5f));
  transition.TransitionWithChild(true);
  TransitionSet transitionSet = TransitionSet::New();
  transitionSet.AddTransition(transition);
  transitionSet.Play();

  bool signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(600);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(destinationWorldPosition, control3.GetProperty<Vector3>(Actor::Property::WORLD_POSITION), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTransitionBetweenControlPairWithTreeWithoutPositionInheritance(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTransitionBetweenControlPairWithTreeWithoutPositionInheritance");

  Vector3 sourcePosition(50, 50, 0);
  Vector3 destinationPosition(100, 100, 0);

  Control control1 = Control::New();
  control1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  control1.SetProperty(Actor::Property::POSITION, sourcePosition);
  control1.SetProperty(Actor::Property::SIZE, Vector3(150, 150, 0));
  Property::Map controlProperty1;
  controlProperty1.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty1.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control1.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty1);

  Control control2 = Control::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  control2.SetProperty(Actor::Property::POSITION, Vector3(150, 150, 0));
  control2.SetProperty(Actor::Property::SIZE, Vector3(120, 120, 0));
  Property::Map controlProperty2;
  controlProperty2.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty2.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control2.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty2);

  Control control3 = Control::New();
  control3.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control3.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  control3.SetProperty(Actor::Property::POSITION, destinationPosition);
  control3.SetProperty(Actor::Property::SIZE, Vector3(120, 120, 0));
  Property::Map controlProperty3;
  controlProperty3.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty3.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control3.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty3);

  application.GetScene().Add(control1);
  application.GetScene().Add(control2);
  control2.Add(control3);

  application.SendNotification();
  application.Render(20);

  Transition transition;
  TransitionSet transitionSet;
  bool signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);

  // not inherit Position.
  control3.SetProperty(Actor::Property::INHERIT_POSITION, false);
  control3.SetProperty(Actor::Property::INHERIT_ORIENTATION, true);
  control3.SetProperty(Actor::Property::INHERIT_SCALE, true);

  transition = Transition::New(control1, control3, TimePeriod(0.5f));
  transitionSet = TransitionSet::New();
  transitionSet.AddTransition(transition);
  transitionSet.Play();
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(300);

  Vector3 currentPosition = control3.GetProperty<Vector3>(Actor::Property::WORLD_POSITION);
  DALI_TEST_CHECK(currentPosition.x <= destinationPosition.x && currentPosition.x >= sourcePosition.x);
  DALI_TEST_CHECK(currentPosition.y <= destinationPosition.y && currentPosition.y >= sourcePosition.y);
  DALI_TEST_CHECK(currentPosition.z <= destinationPosition.z && currentPosition.z >= sourcePosition.z);

  application.SendNotification();
  application.Render(300);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(destinationPosition, control3.GetProperty<Vector3>(Actor::Property::WORLD_POSITION), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTransitionBetweenControlPairWithTreeWithoutOrientationInheritance(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTransitionBetweenControlPairWithTreeWithoutOrientationInheritance");

  Radian sourceAngle(1.0f);
  Radian destinationAngle(2.0f);
  Quaternion sourceOrientation(sourceAngle, Vector3::XAXIS);
  Quaternion destinationOrientation(destinationAngle, Vector3::XAXIS);

  Control control1 = Control::New();
  control1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  control1.SetProperty(Actor::Property::ORIENTATION, sourceOrientation);
  control1.SetProperty(Actor::Property::SIZE, Vector3(150, 150, 0));
  Property::Map controlProperty1;
  controlProperty1.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty1.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control1.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty1);

  Control control2 = Control::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  control2.SetProperty(Actor::Property::ORIENTATION, Quaternion(Radian(2.0f), Vector3::XAXIS));
  control2.SetProperty(Actor::Property::SIZE, Vector3(120, 120, 0));
  Property::Map controlProperty2;
  controlProperty2.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty2.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control2.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty2);

  Control control3 = Control::New();
  control3.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control3.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  control3.SetProperty(Actor::Property::ORIENTATION, destinationOrientation);
  control3.SetProperty(Actor::Property::SIZE, Vector3(120, 120, 0));
  Property::Map controlProperty3;
  controlProperty3.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty3.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control3.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty3);

  // not inherit Orientation.
  control3.SetProperty(Actor::Property::INHERIT_POSITION, true);
  control3.SetProperty(Actor::Property::INHERIT_ORIENTATION, false);
  control3.SetProperty(Actor::Property::INHERIT_SCALE, true);

  Vector3 currentAxis;
  Radian currentRadian;

  application.GetScene().Add(control1);
  application.GetScene().Add(control2);
  control2.Add(control3);

  application.SendNotification();
  application.Render(20);

  Quaternion currentOrientation = control3.GetProperty<Quaternion>(Actor::Property::WORLD_ORIENTATION);
  DALI_TEST_EQUALS(currentOrientation, destinationOrientation, 0.0001f, TEST_LOCATION);

  Transition transition;
  TransitionSet transitionSet;
  bool signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);

  transition = Transition::New(control1, control3, TimePeriod(0.5f));
  transitionSet = TransitionSet::New();
  transitionSet.AddTransition(transition);
  transitionSet.Play();
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(300);

  currentOrientation = control3.GetProperty<Quaternion>(Actor::Property::WORLD_ORIENTATION);
  DALI_TEST_NOT_EQUALS(currentOrientation, destinationOrientation, 0.0001f, TEST_LOCATION);

  application.SendNotification();
  application.Render(300);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  application.SendNotification();
  application.Render(20);

  currentOrientation = control3.GetProperty<Quaternion>(Actor::Property::WORLD_ORIENTATION);
  DALI_TEST_EQUALS(currentOrientation, destinationOrientation, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTransitionBetweenControlPairWithTreeWithoutScaleInheritance(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTransitionBetweenControlPairWithTreeWithoutOrientationInheritance");

  Vector3 sourceScale(1, 1, 1);
  Vector3 destinationScale(2, 2, 1);

  Control control1 = Control::New();
  control1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  control1.SetProperty(Actor::Property::SCALE, sourceScale);
  control1.SetProperty(Actor::Property::SIZE, Vector3(150, 150, 0));
  Property::Map controlProperty1;
  controlProperty1.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty1.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control1.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty1);

  Control control2 = Control::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  control2.SetProperty(Actor::Property::SCALE, Vector3(3, 3, 1));
  control2.SetProperty(Actor::Property::SIZE, Vector3(120, 120, 0));
  Property::Map controlProperty2;
  controlProperty2.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty2.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control2.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty2);

  Control control3 = Control::New();
  control3.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control3.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  control3.SetProperty(Actor::Property::SCALE, destinationScale);
  control3.SetProperty(Actor::Property::SIZE, Vector3(120, 120, 0));
  Property::Map controlProperty3;
  controlProperty3.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  controlProperty3.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  control3.SetProperty(Toolkit::Control::Property::BACKGROUND, controlProperty3);

  // not inherit Orientation.
  control3.SetProperty(Actor::Property::INHERIT_POSITION, true);
  control3.SetProperty(Actor::Property::INHERIT_ORIENTATION, true);
  control3.SetProperty(Actor::Property::INHERIT_SCALE, false);

  application.GetScene().Add(control1);
  application.GetScene().Add(control2);
  control2.Add(control3);

  application.SendNotification();
  application.Render(20);

  Vector3 currentScale = control3.GetProperty<Vector3>(Actor::Property::WORLD_SCALE);
  DALI_TEST_EQUALS(currentScale, destinationScale, 0.0001f, TEST_LOCATION);

  Transition transition;
  TransitionSet transitionSet;
  bool signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);

  transition = Transition::New(control1, control3, TimePeriod(0.5f));
  transitionSet = TransitionSet::New();
  transitionSet.AddTransition(transition);
  transitionSet.Play();
  transitionSet.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(300);

  currentScale = control3.GetProperty<Vector3>(Actor::Property::WORLD_SCALE);
  DALI_TEST_CHECK(currentScale.x <= destinationScale.x && currentScale.x >= sourceScale.x);
  DALI_TEST_CHECK(currentScale.y <= destinationScale.y && currentScale.y >= sourceScale.y);
  DALI_TEST_CHECK(currentScale.z <= destinationScale.z && currentScale.z >= sourceScale.z);

  application.SendNotification();
  application.Render(300);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  application.SendNotification();
  application.Render(20);

  currentScale = control3.GetProperty<Vector3>(Actor::Property::WORLD_SCALE);
  DALI_TEST_EQUALS(currentScale, destinationScale, 0.0001f, TEST_LOCATION);

  END_TEST;
}
