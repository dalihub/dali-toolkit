/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali-toolkit/devel-api/drag-drop-detector/drag-and-drop-detector.h>
#include <dali/integration-api/events/touch-event-integ.h>

using namespace Dali;
using namespace Dali::Toolkit;

void utc_dali_toolkit_drag_drop_detector_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_drag_drop_detector_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
  const int RENDER_FRAME_INTERVAL = 16;
  struct SignalData
  {
    SignalData()
    :functorCalled(false),
    control(),
    detector()
    {
    }

    void Reset()
    {
      functorCalled = false;
      control.Reset();
    }

    bool functorCalled;
    Control control;
    Dali::Toolkit::DragAndDropDetector detector;
  };

  struct DragSignalFunctor
  {
    DragSignalFunctor(SignalData& data, bool returnValue = true)
    :signalData(data),
    returnValue(returnValue)
    {
    }

    bool operator()(Control control, Dali::Toolkit::DragAndDropDetector detector)
    {
      signalData.functorCalled = true;
      signalData.control = control;
      signalData.detector = detector;
      return returnValue;
    }

    SignalData& signalData;
    bool returnValue;
    };

  Integration::TouchEvent GenerateSingleTouch(TouchPoint::State state, const Vector2& screenPosition)
  {
    Integration::TouchEvent touchEvent;
    Integration::Point point;
    point.SetState(static_cast<PointState::Type>(state));
    point.SetScreenPosition(screenPosition);
    touchEvent.points.push_back(point);
    return touchEvent;
  }
}

int UtcDaliDragAndDropDetectorConstructorN(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector;
  DALI_TEST_CHECK(!detector);

  END_TEST;
}

int UtcDaliDragAndDropDetectorConstructorP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector = Dali::Toolkit::DragAndDropDetector::New();
  DALI_TEST_CHECK(detector);

  END_TEST;
}

int UtcDaliDragAndDropDetectorAttachN(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector = Dali::Toolkit::DragAndDropDetector::New();
  Control control;
  detector.Attach(control);

  DALI_TEST_EQUALS(0, detector.GetAttachedControlCount(), TEST_LOCATION);
  Control control1 = Control::New();
  detector.Attach(control1);
  DALI_TEST_EQUALS(1, detector.GetAttachedControlCount(), TEST_LOCATION);
  detector.Attach(control1);
  DALI_TEST_EQUALS(1, detector.GetAttachedControlCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliDragAndDropDetectorAttachP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector = Dali::Toolkit::DragAndDropDetector::New();
  Control control = Control::New();

  detector.Attach(control);
  DALI_TEST_EQUALS(1, detector.GetAttachedControlCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliDragAndDropDetectorDetachN(void)
  {
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector = Dali::Toolkit::DragAndDropDetector::New();
  Control control1;
  Control control2 = Control::New();

  detector.Attach(control1);
  detector.Attach(control2);
  DALI_TEST_EQUALS(1, detector.GetAttachedControlCount(), TEST_LOCATION);

  detector.Detach(control2);
  DALI_TEST_EQUALS(0, detector.GetAttachedControlCount(), TEST_LOCATION);

  detector.Detach(control1);
  DALI_TEST_EQUALS(0, detector.GetAttachedControlCount(), TEST_LOCATION);

  END_TEST;

}

int UtcDaliDragAndDropDetectorDetachP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector = Dali::Toolkit::DragAndDropDetector::New();
  Control control1 = Control::New();
  Control control2 = Control::New();
  Control control3;

  detector.Attach(control1);
  detector.Attach(control2);
  detector.Attach(control3);

  DALI_TEST_EQUALS(2, detector.GetAttachedControlCount(), TEST_LOCATION);

  detector.Detach(control3);
  DALI_TEST_EQUALS(2, detector.GetAttachedControlCount(), TEST_LOCATION);
  detector.Detach(control2);
  DALI_TEST_EQUALS(1, detector.GetAttachedControlCount(), TEST_LOCATION);

  detector.Detach(control1);
  DALI_TEST_EQUALS(0, detector.GetAttachedControlCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliDragAndDropDetectorDetachAllN(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector = Dali::Toolkit::DragAndDropDetector::New();
  detector.DetachAll();
  DALI_TEST_EQUALS(0, detector.GetAttachedControlCount(), TEST_LOCATION);

  END_TEST;
}
int UtcDaliDragAndDropDetectorDetachAllP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector = Dali::Toolkit::DragAndDropDetector::New();
  Control control1 = Control::New();
  Control control2 = Control::New();

  detector.Attach(control1);
  detector.Attach(control2);
  detector.DetachAll();
  DALI_TEST_EQUALS(0, detector.GetAttachedControlCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliDragAndDropDetectorGetAttachedControlCountP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector = Dali::Toolkit::DragAndDropDetector::New();
  Control control1 = Control::New();
  Control control2 = Control::New();

  detector.Attach(control1);
  detector.Attach(control2);
  DALI_TEST_EQUALS(2, detector.GetAttachedControlCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliDragAndDropDetectorGetAttachedControlN(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector = Dali::Toolkit::DragAndDropDetector::New();
  Control control1 = Control::New();
  Control control2 = Control::New();

  detector.Attach(control1);

  Control control = detector.GetAttachedControl(1);
  DALI_TEST_CHECK(!control);

  END_TEST;
}

int UtcDaliDragAndDropDetectorGetAttachedControlP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector = Dali::Toolkit::DragAndDropDetector::New();
  Control control1 = Control::New();
  Control control2 = Control::New();

  detector.Attach(control1);
  detector.Attach(control2);
  Control control = detector.GetAttachedControl(1);
  DALI_TEST_CHECK(control);
  DALI_TEST_EQUALS(control2, control, TEST_LOCATION);

  END_TEST;
}

int UtcDaliDragAndDropDetectorStartSignal(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector = Dali::Toolkit::DragAndDropDetector::New();
  Control control = Control::New();
  control.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  control.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(control);
  detector.Attach(control);

  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);

  DALI_TEST_CHECK(detector);
  DALI_TEST_CHECK(control);

  SignalData data;
  DragSignalFunctor functor(data);
  detector.StartedSignal().Connect(&application, functor);

  TestGenerateMiniPan(application);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(control, data.control, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 40.0f), data.detector.GetCurrentScreenPosition(), TEST_LOCATION);
  data.Reset();

  END_TEST;
}

int UtcDaliDragAndDropDetectorEnteredSignal(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector = Dali::Toolkit::DragAndDropDetector::New();
  Control control1 = Control::New();
  Control control2 = Control::New();
  control1.SetProperty( Actor::Property::SIZE, Vector2(100.0f,100.0f) );
  control2.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  control1.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_LEFT);
  control2.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_LEFT);
  control1.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
  control2.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
  control1.SetProperty( Actor::Property::POSITION, Vector2(0.0f, 0.0f));
  control2.SetProperty( Actor::Property::POSITION, Vector2(0.0f, 100.0f));

  Stage::GetCurrent().Add(control1);
  Stage::GetCurrent().Add(control2);

  detector.Attach(control1);
  detector.Attach(control2);

  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);

  SignalData data;
  DragSignalFunctor functor(data);
  detector.EnteredSignal().Connect(&application, functor);

  TestGenerateMiniPan(application);

  Vector2 screenCoordinates(10.0f, 110.0f);
  application.ProcessEvent(GenerateSingleTouch(TouchPoint::Motion, screenCoordinates));

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(control2, data.control, TEST_LOCATION);

  data.Reset();

  END_TEST;

}

int UtcDaliDragAndDropDetectorMovedSignal(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector = Dali::Toolkit::DragAndDropDetector::New();
  Control control1 = Control::New();
  Control control2 = Control::New();
  control1.SetProperty( Actor::Property::SIZE, Vector2(100.0f,100.0f) );
  control2.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  control1.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_LEFT);
  control2.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_LEFT);
  control1.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
  control2.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
  control1.SetProperty( Actor::Property::POSITION, Vector2(0.0f, 0.0f));
  control2.SetProperty( Actor::Property::POSITION, Vector2(0.0f, 100.0f));

  Stage::GetCurrent().Add(control1);
  Stage::GetCurrent().Add(control2);

  detector.Attach(control1);
  detector.Attach(control2);

  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);

  SignalData data;
  DragSignalFunctor functor(data);
  detector.MovedSignal().Connect(&application, functor);

  TestGenerateMiniPan(application);

  Vector2 screenCoordinates(10.0f, 110.0f);
  application.ProcessEvent(GenerateSingleTouch(TouchPoint::Motion, screenCoordinates));

  screenCoordinates.x = 10.0f;
  screenCoordinates.y = 120.0f;
  application.ProcessEvent(GenerateSingleTouch(TouchPoint::Motion, screenCoordinates));

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(10.0f, 120.0f), data.detector.GetCurrentScreenPosition(), TEST_LOCATION);
  DALI_TEST_EQUALS(control2, data.control, TEST_LOCATION);

  data.Reset();

  END_TEST;
}

int UtcDaliDragAndDropDetectorExitedSignal(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector = Dali::Toolkit::DragAndDropDetector::New();
  Control control1 = Control::New();
  Control control2 = Control::New();
  control1.SetProperty( Actor::Property::SIZE, Vector2(100.0f,100.0f) );
  control2.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  control1.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_LEFT);
  control2.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_LEFT);
  control1.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
  control2.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
  control1.SetProperty( Actor::Property::POSITION, Vector2(0.0f, 0.0f));
  control2.SetProperty( Actor::Property::POSITION, Vector2(0.0f, 100.0f));

  control1.SetProperty( Actor::Property::LEAVE_REQUIRED,true);
  control2.SetProperty( Actor::Property::LEAVE_REQUIRED,true);

  Stage::GetCurrent().Add(control1);
  Stage::GetCurrent().Add(control2);

  detector.Attach(control1);
  detector.Attach(control2);

  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);

  SignalData data;
  DragSignalFunctor functor(data);
  detector.ExitedSignal().Connect(&application, functor);

  TestGenerateMiniPan(application);

  Vector2 screenCoordinates(10.0f, 110.0f);
  application.ProcessEvent(GenerateSingleTouch(TouchPoint::Motion, screenCoordinates));

  screenCoordinates.x = 20.0f;
  screenCoordinates.y = 20.0f;
  application.ProcessEvent(GenerateSingleTouch(TouchPoint::Motion, screenCoordinates));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(control2, data.control, TEST_LOCATION);

  data.Reset();

  END_TEST;
}

int UtcDaliDragAndDropDetectorDroppedSignal(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector = Dali::Toolkit::DragAndDropDetector::New();
  Control control1 = Control::New();
  Control control2 = Control::New();
  control1.SetProperty( Actor::Property::SIZE, Vector2(100.0f,100.0f) );
  control2.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  control1.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_LEFT);
  control2.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_LEFT);
  control1.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
  control2.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
  control1.SetProperty( Actor::Property::POSITION, Vector2(0.0f, 0.0f));
  control2.SetProperty( Actor::Property::POSITION, Vector2(0.0f, 100.0f));

  Stage::GetCurrent().Add(control1);
  Stage::GetCurrent().Add(control2);

  detector.Attach(control1);
  detector.Attach(control2);

  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);

  SignalData data;
  DragSignalFunctor functor(data);
  detector.DroppedSignal().Connect(&application, functor);

  TestGenerateMiniPan(application);

  Vector2 screenCoordinates(10.0f, 110.0f);
  application.ProcessEvent(GenerateSingleTouch(TouchPoint::Motion, screenCoordinates));

  screenCoordinates.x = 10.0f;
  screenCoordinates.y = 112.0f;
  application.ProcessEvent(GenerateSingleTouch(TouchPoint::Up, screenCoordinates));

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(control2, data.control, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(10.0f, 112.0f), data.detector.GetCurrentScreenPosition(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, detector.GetContent().empty(), TEST_LOCATION);

  data.Reset();

  END_TEST;
}

int UtcDaliDragAndDropDetectorEndedSignal(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector = Dali::Toolkit::DragAndDropDetector::New();
  Control control1 = Control::New();
  Control control2 = Control::New();
  control1.SetProperty( Actor::Property::SIZE, Vector2(100.0f,100.0f) );
  control2.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  control1.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_LEFT);
  control2.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_LEFT);
  control1.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
  control2.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
  control1.SetProperty( Actor::Property::POSITION, Vector2(0.0f, 0.0f));
  control2.SetProperty( Actor::Property::POSITION, Vector2(0.0f, 100.0f));

  Stage::GetCurrent().Add(control1);
  Stage::GetCurrent().Add(control2);

  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);

  detector.Attach(control1);
  detector.Attach(control2);

  SignalData data;
  DragSignalFunctor functor(data);
  detector.EndedSignal().Connect(&application, functor);

  TestGenerateMiniPan(application);

  application.ProcessEvent(GenerateSingleTouch(TouchPoint::Down, Vector2(10.0f, 10.0f)));

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(control1, data.control, TEST_LOCATION);
  data.Reset();

  END_TEST;
}

int UtcDaliDragAndDropDetectorGetContent(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::DragAndDropDetector detector = Dali::Toolkit::DragAndDropDetector::New();
  Control control1 = Control::New();
  Control control2 = Control::New();
  control1.SetProperty( Dali::Actor::Property::NAME,"control1");
  control2.SetProperty( Dali::Actor::Property::NAME,"control2");
  control1.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  control2.SetProperty( Actor::Property::SIZE, Vector2(100.0f, 100.0f) );
  control1.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_LEFT);
  control2.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_LEFT);
  control1.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
  control2.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
  control1.SetProperty( Actor::Property::POSITION, Vector2(0.0f, 0.0f));
  control2.SetProperty( Actor::Property::POSITION, Vector2(0.0f, 100.0f));

  Stage::GetCurrent().Add(control1);
  Stage::GetCurrent().Add(control2);

  detector.Attach(control1);
  detector.Attach(control2);

  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);

  SignalData data;
  DragSignalFunctor functor(data);
  detector.DroppedSignal().Connect(&application, functor);

  TestGenerateMiniPan(application);

  Vector2 screenCoordinates(10.0f, 110.0f);
  application.ProcessEvent(GenerateSingleTouch(TouchPoint::Motion, screenCoordinates));

  screenCoordinates.x = 10.0f;
  screenCoordinates.y = 112.0f;
  application.ProcessEvent(GenerateSingleTouch(TouchPoint::Up, screenCoordinates));

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(control2, data.control, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(10.0f, 112.0f), data.detector.GetCurrentScreenPosition(), TEST_LOCATION);
  DALI_TEST_EQUALS("control1", detector.GetContent(), TEST_LOCATION);

  data.Reset();

  END_TEST;
}
