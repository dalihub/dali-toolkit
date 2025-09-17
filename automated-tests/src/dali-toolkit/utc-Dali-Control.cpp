/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/alignment/alignment.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/visuals/image-visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/public-api/align-enumerations.h>
#include <dali.h>

#include <toolkit-event-thread-callback.h>

#include "dummy-control.h"

using namespace Dali;
using namespace Dali::Toolkit;

void utc_dali_toolkit_control_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_control_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
bool gObjectCreatedCallBackCalled;

void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

void TestVoidCallback()
{
}

static bool gKeyInputFocusCallBackCalled;

static void TestKeyInputFocusCallback(Control control)
{
  tet_infoline(" TestKeyInputFocusCallback");

  gKeyInputFocusCallBackCalled = true;
}

const char* TEST_LARGE_IMAGE_FILE_NAME = TEST_RESOURCE_DIR "/tbcol.png";
const char* TEST_IMAGE_FILE_NAME       = TEST_RESOURCE_DIR "/gallery-small-1.jpg";
const char* TEST_SVG_FILE_NAME         = TEST_RESOURCE_DIR "/svg1.svg";

Vector4 GetControlBackgroundColor(Control& control)
{
  Property::Value propValue = control.GetProperty(Control::Property::BACKGROUND);
  Property::Map*  resultMap = propValue.GetMap();
  DALI_TEST_CHECK(resultMap->Find(ColorVisual::Property::MIX_COLOR));

  Vector4 color;
  resultMap->Find(ColorVisual::Property::MIX_COLOR)->Get(color);

  return color;
}

bool gResourceReadySignalFired = false;

void ResourceReadySignal(Control control)
{
  if(control.GetVisualResourceStatus(Control::Property::BACKGROUND) == Visual::ResourceStatus::FAILED)
  {
    Property::Map propertyMap;
    propertyMap.Insert(ImageVisual::Property::URL, TEST_SVG_FILE_NAME);
    control.SetProperty(Control::Property::BACKGROUND, propertyMap);
  }

  gResourceReadySignalFired = true;
}

} // namespace

///////////////////////////////////////////////////////////////////////////////////////////////////

int UtcDaliControlConstructor(void)
{
  ToolkitTestApplication application; // Exceptions require ToolkitTestApplication

  DummyControl dummy;

  DALI_TEST_CHECK(!Control::DownCast(dummy));

  dummy = DummyControl::New();

  DALI_TEST_CHECK(Control::DownCast(dummy));
  END_TEST;
}

int UtcDaliControlNew(void)
{
  ToolkitTestApplication application; // Exceptions require ToolkitTestApplication

  Control control;

  DALI_TEST_CHECK(!Control::DownCast(control));

  control = Control::New();

  DALI_TEST_CHECK(Control::DownCast(control));
  END_TEST;
}

int UtcDaliControlRegister(void)
{
  ToolkitTestApplication application;

  // Ensure the object is registered after creation
  ObjectRegistry registry = application.GetCore().GetObjectRegistry();
  DALI_TEST_CHECK(registry);

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect(&TestCallback);
  {
    Alignment alignment = Alignment::New();
  }
  DALI_TEST_CHECK(gObjectCreatedCallBackCalled);
  END_TEST;
}

int UtcDaliControlCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  DummyControl control = DummyControl::New();
  Control      emptyControl;

  Control controlCopy(control);
  DALI_TEST_CHECK(control == controlCopy);

  Control emptyControlCopy(emptyControl);
  DALI_TEST_CHECK(emptyControl == emptyControlCopy);

  Control controlEquals;
  controlEquals = control;
  DALI_TEST_CHECK(control == controlEquals);

  Control emptyControlEquals;
  emptyControlEquals = emptyControl;
  DALI_TEST_CHECK(emptyControl == emptyControlEquals);

  // Self assignment
  control = control;
  DALI_TEST_CHECK(control == controlCopy);
  END_TEST;
}

int UtcDaliControlMoveConstructor(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  DALI_TEST_EQUALS(1, control.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  control.SetProperty(Actor::Property::SENSITIVE, false);
  DALI_TEST_CHECK(false == control.GetProperty<bool>(Actor::Property::SENSITIVE));

  Control moved = std::move(control);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(false == moved.GetProperty<bool>(Actor::Property::SENSITIVE));
  DALI_TEST_CHECK(!control);

  END_TEST;
}

int UtcDaliControlMoveAssignment(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  DALI_TEST_EQUALS(1, control.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  control.SetProperty(Actor::Property::SENSITIVE, false);
  DALI_TEST_CHECK(false == control.GetProperty<bool>(Actor::Property::SENSITIVE));

  Control moved;
  moved = std::move(control);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(false == moved.GetProperty<bool>(Actor::Property::SENSITIVE));
  DALI_TEST_CHECK(!control);

  END_TEST;
}

int UtcDaliControlDownCast(void)
{
  ToolkitTestApplication application;

  DummyControl control;

  DALI_TEST_CHECK(!Control::DownCast(control));

  control = DummyControl::New();

  DALI_TEST_CHECK(Control::DownCast(control));

  Actor actor;

  DALI_TEST_CHECK(!Control::DownCast(actor));

  actor = Actor::New();

  DALI_TEST_CHECK(!Control::DownCast(actor));
  END_TEST;
}

int UtcDaliControlDownCastTemplate(void)
{
  ToolkitTestApplication application;

  DummyControl control;

  DALI_TEST_CHECK(!DummyControl::DownCast(control));

  control = DummyControl::New();

  DALI_TEST_CHECK(DummyControl::DownCast(control));

  Actor actor;

  DALI_TEST_CHECK(!DummyControl::DownCast(actor));

  actor = Actor::New();

  DALI_TEST_CHECK(!DummyControl::DownCast(actor));
  END_TEST;
}

int UtcDaliControlNavigationProperties(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  application.GetScene().Add(control);

  DALI_TEST_EQUALS(-1, control.GetProperty(DevelControl::Property::LEFT_FOCUSABLE_ACTOR_ID).Get<int>(), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, control.GetProperty(DevelControl::Property::RIGHT_FOCUSABLE_ACTOR_ID).Get<int>(), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, control.GetProperty(DevelControl::Property::UP_FOCUSABLE_ACTOR_ID).Get<int>(), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, control.GetProperty(DevelControl::Property::DOWN_FOCUSABLE_ACTOR_ID).Get<int>(), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, control.GetProperty(DevelControl::Property::CLOCKWISE_FOCUSABLE_ACTOR_ID).Get<int>(), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, control.GetProperty(DevelControl::Property::COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID).Get<int>(), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::LEFT_FOCUSABLE_ACTOR_ID, 1);
  DALI_TEST_EQUALS(1, control.GetProperty(DevelControl::Property::LEFT_FOCUSABLE_ACTOR_ID).Get<int>(), TEST_LOCATION);
  control.SetProperty(DevelControl::Property::RIGHT_FOCUSABLE_ACTOR_ID, 2);
  DALI_TEST_EQUALS(2, control.GetProperty(DevelControl::Property::RIGHT_FOCUSABLE_ACTOR_ID).Get<int>(), TEST_LOCATION);
  control.SetProperty(DevelControl::Property::UP_FOCUSABLE_ACTOR_ID, 3);
  DALI_TEST_EQUALS(3, control.GetProperty(DevelControl::Property::UP_FOCUSABLE_ACTOR_ID).Get<int>(), TEST_LOCATION);
  control.SetProperty(DevelControl::Property::DOWN_FOCUSABLE_ACTOR_ID, 4);
  DALI_TEST_EQUALS(4, control.GetProperty(DevelControl::Property::DOWN_FOCUSABLE_ACTOR_ID).Get<int>(), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::LEFT_FOCUSABLE_ACTOR_ID, 15);
  DALI_TEST_EQUALS(15, control.GetProperty(DevelControl::Property::LEFT_FOCUSABLE_ACTOR_ID).Get<int>(), TEST_LOCATION);
  control.SetProperty(DevelControl::Property::RIGHT_FOCUSABLE_ACTOR_ID, 16);
  DALI_TEST_EQUALS(16, control.GetProperty(DevelControl::Property::RIGHT_FOCUSABLE_ACTOR_ID).Get<int>(), TEST_LOCATION);
  control.SetProperty(DevelControl::Property::UP_FOCUSABLE_ACTOR_ID, 17);
  DALI_TEST_EQUALS(17, control.GetProperty(DevelControl::Property::UP_FOCUSABLE_ACTOR_ID).Get<int>(), TEST_LOCATION);
  control.SetProperty(DevelControl::Property::DOWN_FOCUSABLE_ACTOR_ID, 18);
  DALI_TEST_EQUALS(18, control.GetProperty(DevelControl::Property::DOWN_FOCUSABLE_ACTOR_ID).Get<int>(), TEST_LOCATION);
  control.SetProperty(DevelControl::Property::CLOCKWISE_FOCUSABLE_ACTOR_ID, 19);
  DALI_TEST_EQUALS(19, control.GetProperty(DevelControl::Property::CLOCKWISE_FOCUSABLE_ACTOR_ID).Get<int>(), TEST_LOCATION);
  control.SetProperty(DevelControl::Property::COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID, 20);
  DALI_TEST_EQUALS(20, control.GetProperty(DevelControl::Property::COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID).Get<int>(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlKeyInputFocus(void)
{
  ToolkitTestApplication application;
  Integration::Scene     stage = application.GetScene();

  DummyControl control;

  PushButton pushButton1 = PushButton::New();
  stage.Add(pushButton1);

  pushButton1.SetKeyInputFocus();
  DALI_TEST_CHECK(pushButton1.HasKeyInputFocus());

  pushButton1.ClearKeyInputFocus();
  DALI_TEST_CHECK(!pushButton1.HasKeyInputFocus());
  END_TEST;
}

int UtcDaliControlGetImplementationN(void)
{
  ToolkitTestApplication application;
  DummyControl           control;

  // Get Empty
  {
    try
    {
      Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(control);
      (void)controlImpl; // Avoid unused warning
      tet_result(TET_FAIL);
    }
    catch(DaliException& exception)
    {
      tet_result(TET_PASS);
    }
  }
  END_TEST;
}

int UtcDaliControlGetImplementationConstN(void)
{
  ToolkitTestApplication application;
  DummyControl           control;

  // Get Const Empty
  {
    try
    {
      const DummyControl                constControl(control);
      const Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(constControl);
      (void)controlImpl; // Avoid unused warning
      tet_result(TET_FAIL);
    }
    catch(DaliException& exception)
    {
      tet_result(TET_PASS);
    }
  }
  END_TEST;
}

int UtcDaliControlGetImplementationP(void)
{
  ToolkitTestApplication application;
  DummyControl           control = DummyControl::New();

  // Get
  {
    try
    {
      Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(control);
      (void)controlImpl; // Avoid unused warning
      tet_result(TET_PASS);
    }
    catch(DaliException& exception)
    {
      tet_result(TET_FAIL);
    }
  }
  END_TEST;
}

int UtcDaliControlGetImplementationConstP(void)
{
  ToolkitTestApplication application;
  DummyControl           control = DummyControl::New();
  // Get Const
  {
    try
    {
      const DummyControl                constControl(control);
      const Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(constControl);
      (void)controlImpl; // Avoid unused warning
      tet_result(TET_PASS);
    }
    catch(DaliException& exception)
    {
      tet_result(TET_FAIL);
    }
  }
  END_TEST;
}

int UtcDaliControlSignalConnectDisconnect(void)
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControlImpl::New();

    Actor actor = Actor::New();
    DALI_TEST_EQUALS(actor.OnSceneSignal().GetConnectionCount(), 0u, TEST_LOCATION);
    Toolkit::Internal::Control& control   = Toolkit::Internal::GetImplementation(dummy);
    DummyControlImpl*           dummyImpl = dynamic_cast<DummyControlImpl*>(&control);

    if(dummyImpl == NULL)
    {
      tet_result(TET_FAIL);
      END_TEST;
    }

    actor.OnSceneSignal().Connect(dummyImpl, &DummyControlImpl::CustomSlot1);
    DALI_TEST_EQUALS(actor.OnSceneSignal().GetConnectionCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(dummyImpl->mCustomSlot1Called, false, TEST_LOCATION);

    application.GetScene().Add(actor);
    DALI_TEST_EQUALS(dummyImpl->mCustomSlot1Called, true, TEST_LOCATION);

    dummyImpl->mCustomSlot1Called = false;
    actor.OnSceneSignal().Disconnect(dummyImpl, &DummyControlImpl::CustomSlot1);
    DALI_TEST_EQUALS(actor.OnSceneSignal().GetConnectionCount(), 0u, TEST_LOCATION);
    application.GetScene().Remove(actor);
    application.GetScene().Add(actor);
    DALI_TEST_EQUALS(dummyImpl->mCustomSlot1Called, false, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliControlSignalAutomaticDisconnect(void)
{
  ToolkitTestApplication application;

  Actor actor = Actor::New();

  {
    DummyControl                dummy     = DummyControlImpl::New();
    Toolkit::Internal::Control& control   = Toolkit::Internal::GetImplementation(dummy);
    DummyControlImpl*           dummyImpl = dynamic_cast<DummyControlImpl*>(&control);

    if(dummyImpl == NULL)
    {
      tet_result(TET_FAIL);
      END_TEST;
    }

    actor.OnSceneSignal().Connect(dummyImpl, &DummyControlImpl::CustomSlot1);
    DALI_TEST_EQUALS(actor.OnSceneSignal().GetConnectionCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(dummyImpl->mCustomSlot1Called, false, TEST_LOCATION);

    application.GetScene().Add(actor);
    DALI_TEST_EQUALS(dummyImpl->mCustomSlot1Called, true, TEST_LOCATION);
    application.GetScene().Remove(actor);
  }
  // dummyControl automatically disconnects

  DALI_TEST_EQUALS(actor.OnSceneSignal().GetConnectionCount(), 0u, TEST_LOCATION);

  const Vector3 ignoredSize(20, 20, 0);
  actor.SetProperty(Actor::Property::SIZE, ignoredSize);
  END_TEST;
}

int UtcDaliControlTestParameters(void)
{
  ToolkitTestApplication application;
  DummyControl           test = DummyControl::New();

  test.SetProperty(Actor::Property::SIZE, Vector3(0.7f, 0.7f, 0.7f));

  application.GetScene().Add(test);

  application.SendNotification();
  application.Render();

  float width  = 640.0f;
  float height = test.GetHeightForWidth(width);
  DALI_TEST_EQUALS(640.0f, height, TEST_LOCATION);
  DALI_TEST_EQUALS(640.0f, test.GetWidthForHeight(height), TEST_LOCATION);

  test.KeyEventSignal();

  // Provide coverage for pointer destructor
  Control* testControlPtr = new Control;
  DALI_TEST_CHECK(testControlPtr);
  delete testControlPtr;
  END_TEST;
}

int UtcDaliControlBackgroundColor(void)
{
  ToolkitTestApplication application;
  Control                control = Control::New();

  DALI_TEST_CHECK(control.GetProperty(Control::Property::BACKGROUND).Get<Property::Map>().Empty());

  control.SetProperty(Control::Property::BACKGROUND, Color::RED);

  Property::Value propValue = control.GetProperty(Control::Property::BACKGROUND);
  Property::Map*  resultMap = propValue.GetMap();
  DALI_TEST_CHECK(resultMap->Find(Toolkit::Visual::Property::TYPE));
  DALI_TEST_CHECK(resultMap->Find(Toolkit::Visual::Property::TYPE)->Get<int>() == Visual::COLOR);
  DALI_TEST_CHECK(resultMap->Find(ColorVisual::Property::MIX_COLOR));
  DALI_TEST_CHECK(resultMap->Find(ColorVisual::Property::MIX_COLOR)->Get<Vector4>() == Color::RED);

  control.SetProperty(Control::Property::BACKGROUND, Color::YELLOW);

  propValue = control.GetProperty(Control::Property::BACKGROUND);
  resultMap = propValue.GetMap();
  DALI_TEST_CHECK(resultMap->Find(ColorVisual::Property::MIX_COLOR));
  DALI_TEST_CHECK(resultMap->Find(ColorVisual::Property::MIX_COLOR)->Get<Vector4>() == Color::YELLOW);

  END_TEST;
}

int UtcDaliControlBackgroundColorRendererCount(void)
{
  tet_infoline("Test ensures we only create renderers when non-transparent color is requested or if we our clipping-mode is set to CLIP_CHILDREN");

  ToolkitTestApplication application;
  Control                control = Control::New();
  control[Actor::Property::SIZE] = Vector2(100.0f, 100.0f);
  application.GetScene().Add(control);

  tet_infoline("Set semi transparent alpha with positive RGB values, 1 renderer should be created, but returned color should reflect what we set");
  const Vector4 semiTransparent(1.0f, 0.75f, 0.5f, 0.5f);
  control.SetBackgroundColor(semiTransparent);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(control.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetControlBackgroundColor(control), semiTransparent, TEST_LOCATION);

  Renderer renderer = control.GetRendererAt(0);
  DALI_TEST_CHECK(renderer);

  tet_infoline("Set semi transparent alpha with positive RGB values, renderer should not be changed");
  const Vector4 newColor(1.0f, 1.0f, 0.5f, 0.5f);
  control.SetBackgroundColor(newColor);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(control.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetControlBackgroundColor(control), newColor, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer, control.GetRendererAt(0), TEST_LOCATION);

  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  tet_infoline("Set transparent, 1 renderer should be created, but ensure nothing is drawn");
  control.SetBackgroundColor(Color::TRANSPARENT);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(control.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetControlBackgroundColor(control), Color::TRANSPARENT, TEST_LOCATION);
  DALI_TEST_EQUALS(drawTrace.FindMethod("DrawArrays"), false, TEST_LOCATION);

  drawTrace.Reset();

  tet_infoline("Set control to clip its children, a renderer should be created which will be transparent");
  control.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(control.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetControlBackgroundColor(control), Color::TRANSPARENT, TEST_LOCATION);
  DALI_TEST_EQUALS(drawTrace.FindMethod("DrawArrays"), true, TEST_LOCATION);

  tet_infoline("Set a color, only 1 renderer should exist");
  control.SetBackgroundColor(Color::RED);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(control.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetControlBackgroundColor(control), Color::RED, TEST_LOCATION);

  tet_infoline("Clear the background, no renderers");
  control.ClearBackground();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(control.GetRendererCount(), 0u, TEST_LOCATION);

  tet_infoline("Set control to clip its children again, a renderer should be created which will be transparent");
  control.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(control.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetControlBackgroundColor(control), Color::TRANSPARENT, TEST_LOCATION);

  drawTrace.Reset();

  tet_infoline("Disable clipping, render nothing");
  control.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::DISABLED);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(GetControlBackgroundColor(control), Color::TRANSPARENT, TEST_LOCATION);
  DALI_TEST_EQUALS(drawTrace.FindMethod("DrawArrays"), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlBackgroundImage(void)
{
  ToolkitTestApplication application;
  Control                control = Control::New();

  tet_infoline("Set first background image");
  control.SetProperty(Control::Property::BACKGROUND, "TestImage");

  Property::Value propValue = control.GetProperty(Control::Property::BACKGROUND);
  Property::Map*  resultMap = propValue.GetMap();
  DALI_TEST_CHECK(resultMap->Find(Toolkit::Visual::Property::TYPE));
  DALI_TEST_CHECK(resultMap->Find(Toolkit::Visual::Property::TYPE)->Get<int>() == Visual::IMAGE);
  DALI_TEST_CHECK(resultMap->Find(ImageVisual::Property::URL));
  DALI_TEST_CHECK(resultMap->Find(ImageVisual::Property::URL)->Get<std::string>() == "TestImage");

  tet_infoline("Set replacement background image");
  control.SetProperty(Control::Property::BACKGROUND, "TestImage2");

  propValue = control.GetProperty(Control::Property::BACKGROUND);
  resultMap = propValue.GetMap();
  DALI_TEST_CHECK(resultMap->Find(ImageVisual::Property::URL));
  DALI_TEST_CHECK(resultMap->Find(ImageVisual::Property::URL)->Get<std::string>() == "TestImage2");

  END_TEST;
}

int UtcDaliControlBackgroundProperties(void)
{
  ToolkitTestApplication application;
  Control                control = Control::New();

  DALI_TEST_CHECK(control.GetProperty(Control::Property::BACKGROUND).Get<Property::Map>().Empty());

  Property::Map imageMap;
  imageMap[Toolkit::Visual::Property::TYPE] = Visual::IMAGE;
  imageMap[ImageVisual::Property::URL]      = "TestImage";
  control.SetProperty(Control::Property::BACKGROUND, imageMap);
  Property::Value propValue = control.GetProperty(Control::Property::BACKGROUND);
  Property::Map*  resultMap = propValue.GetMap();
  DALI_TEST_CHECK(resultMap->Find(Toolkit::Visual::Property::TYPE));
  DALI_TEST_EQUALS(resultMap->Find(Toolkit::Visual::Property::TYPE)->Get<int>(), (int)Visual::IMAGE, TEST_LOCATION);
  DALI_TEST_CHECK(resultMap->Find(ImageVisual::Property::URL));
  DALI_TEST_EQUALS(resultMap->Find(ImageVisual::Property::URL)->Get<std::string>(), "TestImage", TEST_LOCATION);

  Property::Map rendererMap;
  rendererMap[Visual::Property::TYPE]           = Visual::COLOR;
  rendererMap[ColorVisual::Property::MIX_COLOR] = Color::CYAN;
  control.SetProperty(Control::Property::BACKGROUND, rendererMap);
  propValue = control.GetProperty(Control::Property::BACKGROUND);
  resultMap = propValue.GetMap();
  DALI_TEST_CHECK(resultMap->Find(Toolkit::Visual::Property::TYPE));
  DALI_TEST_EQUALS(resultMap->Find(Toolkit::Visual::Property::TYPE)->Get<int>(), (int)Visual::COLOR, TEST_LOCATION);
  DALI_TEST_CHECK(resultMap->Find(ColorVisual::Property::MIX_COLOR));
  DALI_TEST_EQUALS(resultMap->Find(ColorVisual::Property::MIX_COLOR)->Get<Vector4>(), Color::CYAN, TEST_LOCATION);

  Property::Map emptyMap;
  control.SetProperty(Control::Property::BACKGROUND, emptyMap);
  DALI_TEST_CHECK(control.GetProperty(Control::Property::BACKGROUND).Get<Property::Map>().Empty());

  // set as URL
  control.SetProperty(Control::Property::BACKGROUND, "Foobar.png");
  propValue = control.GetProperty(Control::Property::BACKGROUND);
  resultMap = propValue.GetMap();
  DALI_TEST_EQUALS(resultMap->Find(Toolkit::Visual::Property::TYPE)->Get<int>(), (int)Visual::IMAGE, TEST_LOCATION);
  DALI_TEST_EQUALS(resultMap->Find(ImageVisual::Property::URL)->Get<std::string>(), "Foobar.png", TEST_LOCATION);

  // set as Color
  control.SetProperty(Control::Property::BACKGROUND, Color::RED);
  propValue = control.GetProperty(Control::Property::BACKGROUND);
  resultMap = propValue.GetMap();
  DALI_TEST_EQUALS(resultMap->Find(Toolkit::Visual::Property::TYPE)->Get<int>(), (int)Visual::COLOR, TEST_LOCATION);
  DALI_TEST_EQUALS(resultMap->Find(ColorVisual::Property::MIX_COLOR)->Get<Vector4>(), Color::RED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlShadowProperties(void)
{
  ToolkitTestApplication application;
  Control                control = Control::New();

  DALI_TEST_CHECK(control.GetProperty(DevelControl::Property::SHADOW).Get<Property::Map>().Empty());

  Property::Map imageMap;
  imageMap[Toolkit::Visual::Property::TYPE] = Visual::IMAGE;
  imageMap[ImageVisual::Property::URL]      = "TestImage";
  control.SetProperty(DevelControl::Property::SHADOW, imageMap);
  Property::Value propValue = control.GetProperty(DevelControl::Property::SHADOW);
  Property::Map*  resultMap = propValue.GetMap();
  DALI_TEST_CHECK(resultMap->Find(Toolkit::Visual::Property::TYPE));
  DALI_TEST_EQUALS(resultMap->Find(Toolkit::Visual::Property::TYPE)->Get<int>(), (int)Visual::IMAGE, TEST_LOCATION);
  DALI_TEST_CHECK(resultMap->Find(ImageVisual::Property::URL));
  DALI_TEST_EQUALS(resultMap->Find(ImageVisual::Property::URL)->Get<std::string>(), "TestImage", TEST_LOCATION);

  application.SendNotification();
  application.Render();

  Property::Map colorMap;
  colorMap[Visual::Property::TYPE]           = Visual::COLOR;
  colorMap[ColorVisual::Property::MIX_COLOR] = Color::CYAN;
  control.SetProperty(DevelControl::Property::SHADOW, colorMap);
  propValue = control.GetProperty(DevelControl::Property::SHADOW);
  resultMap = propValue.GetMap();
  DALI_TEST_CHECK(resultMap->Find(Toolkit::Visual::Property::TYPE));
  DALI_TEST_EQUALS(resultMap->Find(Toolkit::Visual::Property::TYPE)->Get<int>(), (int)Visual::COLOR, TEST_LOCATION);
  DALI_TEST_CHECK(resultMap->Find(ColorVisual::Property::MIX_COLOR));
  DALI_TEST_EQUALS(resultMap->Find(ColorVisual::Property::MIX_COLOR)->Get<Vector4>(), Color::CYAN, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  Property::Map emptyMap;
  control.SetProperty(DevelControl::Property::SHADOW, emptyMap);
  DALI_TEST_CHECK(control.GetProperty(DevelControl::Property::SHADOW).Get<Property::Map>().Empty());

  END_TEST;
}

int UtcDaliControlInnerShadowProperties(void)
{
  ToolkitTestApplication application;
  Control                control = Control::New();

  DALI_TEST_CHECK(control.GetProperty(DevelControl::Property::INNER_SHADOW).Get<Property::Map>().Empty());

  Property::Map imageMap;
  imageMap[Toolkit::Visual::Property::TYPE] = Visual::IMAGE;
  imageMap[ImageVisual::Property::URL]      = "TestImage";
  control.SetProperty(DevelControl::Property::INNER_SHADOW, imageMap);
  Property::Value propValue = control.GetProperty(DevelControl::Property::INNER_SHADOW);
  Property::Map*  resultMap = propValue.GetMap();
  DALI_TEST_CHECK(resultMap->Find(Toolkit::Visual::Property::TYPE));
  DALI_TEST_EQUALS(resultMap->Find(Toolkit::Visual::Property::TYPE)->Get<int>(), (int)Visual::IMAGE, TEST_LOCATION);
  DALI_TEST_CHECK(resultMap->Find(ImageVisual::Property::URL));
  DALI_TEST_EQUALS(resultMap->Find(ImageVisual::Property::URL)->Get<std::string>(), "TestImage", TEST_LOCATION);

  application.SendNotification();
  application.Render();

  Property::Map colorMap;
  colorMap[Visual::Property::TYPE]           = Visual::COLOR;
  colorMap[ColorVisual::Property::MIX_COLOR] = Color::CYAN;
  control.SetProperty(DevelControl::Property::INNER_SHADOW, colorMap);
  propValue = control.GetProperty(DevelControl::Property::INNER_SHADOW);
  resultMap = propValue.GetMap();
  DALI_TEST_CHECK(resultMap->Find(Toolkit::Visual::Property::TYPE));
  DALI_TEST_EQUALS(resultMap->Find(Toolkit::Visual::Property::TYPE)->Get<int>(), (int)Visual::COLOR, TEST_LOCATION);
  DALI_TEST_CHECK(resultMap->Find(ColorVisual::Property::MIX_COLOR));
  DALI_TEST_EQUALS(resultMap->Find(ColorVisual::Property::MIX_COLOR)->Get<Vector4>(), Color::CYAN, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  control.SetProperty(DevelControl::Property::CORNER_RADIUS_POLICY, (int)Toolkit::Visual::Transform::Policy::RELATIVE);

  application.SendNotification();
  application.Render();

  Property::Map emptyMap;
  control.SetProperty(DevelControl::Property::INNER_SHADOW, emptyMap);
  DALI_TEST_CHECK(control.GetProperty(DevelControl::Property::INNER_SHADOW).Get<Property::Map>().Empty());

  END_TEST;
}

int UtcDaliControlKeyProperties(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  application.GetScene().Add(control);

  DALI_TEST_EQUALS(control.HasKeyInputFocus(), control.GetProperty(Control::Property::KEY_INPUT_FOCUS).Get<bool>(), TEST_LOCATION);

  control.SetKeyInputFocus();
  DALI_TEST_EQUALS(true, control.GetProperty(Control::Property::KEY_INPUT_FOCUS).Get<bool>(), TEST_LOCATION);

  control.ClearKeyInputFocus();
  DALI_TEST_EQUALS(false, control.GetProperty(Control::Property::KEY_INPUT_FOCUS).Get<bool>(), TEST_LOCATION);

  control.SetProperty(Control::Property::KEY_INPUT_FOCUS, true);
  DALI_TEST_EQUALS(true, control.HasKeyInputFocus(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlGestureSignals(void)
{
  ToolkitTestApplication application;
  ConnectionTracker      connectionTracker;
  Control                control = Control::New();

  // Each gesture detector gets created when connecting to the gesture signals
  DALI_TEST_CHECK(!control.GetTapGestureDetector());
  control.ConnectSignal(&connectionTracker, "tapped", &TestVoidCallback);
  DALI_TEST_CHECK(control.GetTapGestureDetector());

  DALI_TEST_CHECK(!control.GetPanGestureDetector());
  control.ConnectSignal(&connectionTracker, "panned", &TestVoidCallback);
  DALI_TEST_CHECK(control.GetPanGestureDetector());

  DALI_TEST_CHECK(!control.GetPinchGestureDetector());
  control.ConnectSignal(&connectionTracker, "pinched", &TestVoidCallback);
  DALI_TEST_CHECK(control.GetPinchGestureDetector());

  DALI_TEST_CHECK(!control.GetLongPressGestureDetector());
  control.ConnectSignal(&connectionTracker, "longPressed", &TestVoidCallback);
  DALI_TEST_CHECK(control.GetLongPressGestureDetector());

  END_TEST;
}

int UtcDaliControlImplKeyInputFocusGainedSignal(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  application.GetScene().Add(control);

  gKeyInputFocusCallBackCalled = false;
  control.KeyInputFocusGainedSignal().Connect(&TestKeyInputFocusCallback);

  application.SendNotification();
  application.Render();

  control.SetKeyInputFocus();

  DALI_TEST_CHECK(control.HasKeyInputFocus());

  DALI_TEST_CHECK(gKeyInputFocusCallBackCalled);

  END_TEST;
}

int UtcDaliControlImplKeyInputFocusLostSignal(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  application.GetScene().Add(control);

  gKeyInputFocusCallBackCalled = false;
  control.KeyInputFocusLostSignal().Connect(&TestKeyInputFocusCallback);

  application.SendNotification();
  application.Render();

  control.SetKeyInputFocus();

  DALI_TEST_CHECK(control.HasKeyInputFocus());

  control.ClearKeyInputFocus();

  DALI_TEST_CHECK(gKeyInputFocusCallBackCalled);

  END_TEST;
}

int UtcDaliControlImplGetControlExtensionP(void)
{
  ToolkitTestApplication application;
  Control                control = Control::New();

  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(control);

  DALI_TEST_CHECK(NULL == controlImpl.GetControlExtension());

  END_TEST;
}

int UtcDaliControlAutoClipping(void)
{
  ToolkitTestApplication application;
  Control                control = Control::New();

  tet_infoline("Test to see if a renderer gets added when we are clipping children");

  DALI_TEST_EQUALS(0, control.GetRendererCount(), TEST_LOCATION);

  control.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(1, control.GetRendererCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlAutoClippingN(void)
{
  ToolkitTestApplication application;
  Control                control = Control::New();
  control.SetProperty(Control::Property::BACKGROUND, Property::Map().Add(Toolkit::Visual::Property::TYPE, Visual::COLOR).Add(ColorVisual::Property::MIX_COLOR, Color::RED));

  tet_infoline("Test to ensure that a renderer does NOT get added when we are clipping children and already have renderers/visuals");

  DALI_TEST_EQUALS(0, control.GetRendererCount(), TEST_LOCATION);

  control.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(1, control.GetRendererCount(), TEST_LOCATION); // Only 1, not 2

  // Ensure the background color is still RED rather than what's set by the automatic clipping
  Property::Value value = control.GetProperty(Control::Property::BACKGROUND);
  Property::Map*  map   = value.GetMap();
  DALI_TEST_CHECK(map);
  Property::Value* colorValue = map->Find(ColorVisual::Property::MIX_COLOR);
  DALI_TEST_CHECK(colorValue);
  DALI_TEST_EQUALS(colorValue->Get<Vector4>(), Color::RED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlAutoClippingWhenAlreadyOnStage(void)
{
  ToolkitTestApplication application;
  Control                control = Control::New();

  tet_infoline("Test to see if a renderer gets added when we are clipping children and when already on stage");

  DALI_TEST_EQUALS(0, control.GetRendererCount(), TEST_LOCATION);

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(0, control.GetRendererCount(), TEST_LOCATION);

  control.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(1, control.GetRendererCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlAutoClippingWhenAlreadyOnStageN(void)
{
  ToolkitTestApplication application;
  Control                control = Control::New();
  control.SetProperty(Control::Property::BACKGROUND, Property::Map().Add(Toolkit::Visual::Property::TYPE, Visual::COLOR).Add(ColorVisual::Property::MIX_COLOR, Color::RED));

  tet_infoline("Test to ensure that a renderer does NOT get added when we are clipping children and already have renderers/visuals and when already on stage");

  DALI_TEST_EQUALS(0, control.GetRendererCount(), TEST_LOCATION);

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(1, control.GetRendererCount(), TEST_LOCATION);

  control.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(1, control.GetRendererCount(), TEST_LOCATION); // Still should be 1

  // Ensure the background color is still RED rather than what's set by the automatic clipping
  Property::Value value = control.GetProperty(Control::Property::BACKGROUND);
  Property::Map*  map   = value.GetMap();
  DALI_TEST_CHECK(map);
  Property::Value* colorValue = map->Find(ColorVisual::Property::MIX_COLOR);
  DALI_TEST_CHECK(colorValue);
  DALI_TEST_EQUALS(colorValue->Get<Vector4>(), Color::RED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlSetTransformSize(void)
{
  ToolkitTestApplication application;
  Control                control = Control::New();

  Property::Map transformMap;
  transformMap.Add(Visual::Transform::Property::OFFSET, Vector2(10, 10))
    .Add(Visual::Transform::Property::ANCHOR_POINT, Align::BOTTOM_END)
    .Add(Visual::Transform::Property::ORIGIN, Align::BOTTOM_END)
    .Add(Visual::Transform::Property::SIZE, Vector2(10, 20));

  control.SetProperty(Control::Property::BACKGROUND, Property::Map().Add(Toolkit::Visual::Property::TYPE, Visual::COLOR).Add(Visual::Property::TRANSFORM, transformMap));

  tet_infoline("Test to ensure that the control background transform does not get overwritten when adding to the stage");

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render();

  // Ensure the transform property still matches what we set
  Property::Value value = control.GetProperty(Control::Property::BACKGROUND);
  Property::Map*  map   = value.GetMap();
  DALI_TEST_CHECK(map);
  Property::Value* transformValue = map->Find(Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(transformValue);

  Property::Map* retMap = transformValue->GetMap();
  DALI_TEST_CHECK(retMap);
  DALI_TEST_EQUALS(retMap->Find(Visual::Transform::Property::OFFSET)->Get<Vector2>(), Vector2(10, 10), TEST_LOCATION);
  DALI_TEST_EQUALS(retMap->Find(Visual::Transform::Property::ANCHOR_POINT)->Get<int>(), (int)Align::BOTTOM_END, TEST_LOCATION);
  DALI_TEST_EQUALS(retMap->Find(Visual::Transform::Property::ORIGIN)->Get<int>(), (int)Align::BOTTOM_END, TEST_LOCATION);
  DALI_TEST_EQUALS(retMap->Find(Visual::Transform::Property::SIZE)->Get<Vector2>(), Vector2(10, 20), TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlResourcesReady(void)
{
  ToolkitTestApplication application;
  tet_infoline("Register 2 visuals and check ResourceReady when a visual is disabled");

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK(factory);

  Property::Map propertyMapLarge;
  propertyMapLarge.Insert(Toolkit::Visual::Property::TYPE, Visual::IMAGE);
  propertyMapLarge.Insert(ImageVisual::Property::URL, TEST_LARGE_IMAGE_FILE_NAME);

  Property::Map propertyMapSmall;
  propertyMapSmall.Insert(Toolkit::Visual::Property::TYPE, Visual::IMAGE);
  propertyMapSmall.Insert(ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME);

  Visual::Base smallVisual = factory.CreateVisual(propertyMapSmall);
  smallVisual.SetName("smallVisual");
  DALI_TEST_CHECK(smallVisual);

  DummyControl      actor     = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());

  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, smallVisual);

  actor.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));

  Toolkit::Visual::ResourceStatus resourceStatus = actor.GetVisualResourceStatus(DummyControl::Property::TEST_VISUAL);
  DALI_TEST_EQUALS(actor.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.IsResourceReady(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(resourceStatus), static_cast<int>(Toolkit::Visual::ResourceStatus::PREPARING), TEST_LOCATION);

  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  resourceStatus = actor.GetVisualResourceStatus(DummyControl::Property::TEST_VISUAL);
  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.IsResourceReady(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(resourceStatus), static_cast<int>(Toolkit::Visual::ResourceStatus::READY), TEST_LOCATION);

  Visual::Base largeVisual = factory.CreateVisual(propertyMapLarge);
  largeVisual.SetName("largeVisual");
  DALI_TEST_CHECK(largeVisual);

  tet_infoline("Register Visual but set disabled, IsResourceReady should be true");

  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL2, largeVisual, false);

  resourceStatus = actor.GetVisualResourceStatus(DummyControl::Property::TEST_VISUAL2);
  DALI_TEST_EQUALS(static_cast<int>(resourceStatus), static_cast<int>(Toolkit::Visual::ResourceStatus::PREPARING), TEST_LOCATION);

  application.SendNotification();

  resourceStatus = actor.GetVisualResourceStatus(DummyControl::Property::TEST_VISUAL2);
  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.IsResourceReady(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(resourceStatus), static_cast<int>(Toolkit::Visual::ResourceStatus::PREPARING), TEST_LOCATION);

  dummyImpl.EnableVisual(DummyControl::Property::TEST_VISUAL2, true);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();

  resourceStatus = actor.GetVisualResourceStatus(DummyControl::Property::TEST_VISUAL2);
  DALI_TEST_EQUALS(static_cast<int>(resourceStatus), static_cast<int>(Toolkit::Visual::ResourceStatus::READY), TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlResourcesReady02(void)
{
  ToolkitTestApplication application;
  tet_infoline("Change a resource during ResourceReady callback");

  gResourceReadySignalFired = false;

  Control control = Control::New();
  control.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
  control.ResourceReadySignal().Connect(&ResourceReadySignal);

  Property::Map propertyMap;
  propertyMap.Insert(ImageVisual::Property::URL, "invalid.jpg");
  control.SetProperty(Control::Property::BACKGROUND, propertyMap);

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Wait for loading & rasterization
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  DALI_TEST_EQUALS(control.IsResourceReady(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);
  gResourceReadySignalFired = false;

  END_TEST;
}

int UtcDaliControlMarginProperty(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  control.SetBackgroundColor(Color::BLUE);

  control.SetProperty(Control::Property::MARGIN, Extents(20, 10, 0, 0));

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(control.GetProperty<Extents>(Control::Property::MARGIN), Extents(20, 10, 0, 0), TEST_LOCATION);

  // Parent control has one ImageView as a Child.
  ImageView image = ImageView::New();
  image.SetBackgroundColor(Color::RED);
  image.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  image.SetProperty(Control::Property::PADDING, Extents(10, 10, 10, 10));
  control.Add(image);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(image.GetProperty<Extents>(Control::Property::PADDING), Extents(10, 10, 10, 10), TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlPaddingProperty(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  control.SetBackgroundColor(Color::BLUE);

  control.SetProperty(Control::Property::PADDING, Extents(15, 10, 5, 10));

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(control.GetProperty<Extents>(Control::Property::PADDING), Extents(15, 10, 5, 10), TEST_LOCATION);

  Control child = Control::New();
  control.Add(child);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(child.GetProperty<Vector3>(Dali::Actor::Property::POSITION), Vector3(15, 5, 0), TEST_LOCATION);

  control.SetProperty(Dali::Actor::Property::LAYOUT_DIRECTION, Dali::LayoutDirection::RIGHT_TO_LEFT);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(child.GetProperty<Vector3>(Dali::Actor::Property::POSITION), Vector3(10, 5, 0), TEST_LOCATION);

  control.SetProperty(Dali::Actor::Property::LAYOUT_DIRECTION, Dali::LayoutDirection::LEFT_TO_RIGHT);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(child.GetProperty<Vector3>(Dali::Actor::Property::POSITION), Vector3(15, 5, 0), TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlDoAction(void)
{
  ToolkitTestApplication application;
  tet_infoline("DoAction on a visual registered with a control");

  // Set up trace debug
  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  // Created AnimatedImageVisual
  VisualFactory factory     = VisualFactory::Get();
  Visual::Base  imageVisual = factory.CreateVisual(TEST_IMAGE_FILE_NAME, ImageDimensions());

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());

  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, imageVisual);
  dummyControl.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
  application.GetScene().Add(dummyControl);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION);
  textureTrace.Reset();

  Property::Map attributes;
  DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelImageVisual::Action::RELOAD, attributes);

  tet_infoline("Perform RELOAD action. should reload Image and generate a texture");
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(textureTrace.CountMethod("DeleteTextures"), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliControlDoActionWhenNotStage(void)
{
  ToolkitTestApplication application;
  tet_infoline("DoAction on a visual registered with a control but not staged");

  // Set up trace debug
  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  // Created AnimatedImageVisual
  VisualFactory factory     = VisualFactory::Get();
  Visual::Base  imageVisual = factory.CreateVisual(TEST_IMAGE_FILE_NAME, ImageDimensions());

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());

  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, imageVisual);
  dummyControl.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION);
  textureTrace.Reset();

  Property::Map attributes;
  DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelImageVisual::Action::RELOAD, attributes);

  tet_infoline("Perform RELOAD action. should reload Image and generate a texture");
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION);
  textureTrace.Reset();

  tet_infoline("Adding control to stage will in turn add the visual to the stage");

  application.GetScene().Add(dummyControl);
  application.SendNotification();
  application.Render();
  tet_infoline("No change in textures could occurs as already loaded and cached texture will be used");

  DALI_TEST_EQUALS(textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION);
  textureTrace.Reset();

  END_TEST;
}

int UtcDaliControlDoActionMultipleWhenNotStage01(void)
{
  ToolkitTestApplication application;
  tet_infoline("DoAction on a visual registered with a control multiple times but not staged");

  // Set up trace debug
  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  // Created AnimatedImageVisual
  VisualFactory factory     = VisualFactory::Get();
  Visual::Base  imageVisual = factory.CreateVisual(TEST_IMAGE_FILE_NAME, ImageDimensions());

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());

  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, imageVisual);
  dummyControl.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION);
  textureTrace.Reset();

  Property::Map  attributes;
  const uint32_t repeatMax = 10u;
  for(uint32_t repeatCnt = 0u; repeatCnt < repeatMax; ++repeatCnt)
  {
    // DoAction multiple times.
    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelImageVisual::Action::RELOAD, attributes);
  }

  tet_infoline("Perform RELOAD action. should reload Image and generate a texture");
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION);
  textureTrace.Reset();

  tet_infoline("Do not load image on more time even we request reload multiple times.");
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 1), false, TEST_LOCATION);

  tet_infoline("Adding control to stage will in turn add the visual to the stage");

  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();
  tet_infoline("No change in textures could occurs as already loaded and cached texture will be used");

  DALI_TEST_EQUALS(textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION);
  textureTrace.Reset();

  dummyControl.Unparent();
  dummyControl.Reset();
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliControlDoActionMultipleWhenNotStage02(void)
{
  ToolkitTestApplication application;
  tet_infoline("DoAction on a visual registered with a control multiple times but not staged");

  // Set up trace debug
  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  // Created AnimatedImageVisual
  VisualFactory factory      = VisualFactory::Get();
  Visual::Base  imageVisual  = factory.CreateVisual(TEST_IMAGE_FILE_NAME, ImageDimensions());
  Visual::Base  imageVisual2 = factory.CreateVisual(TEST_IMAGE_FILE_NAME, ImageDimensions());

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());

  gResourceReadySignalFired = false;

  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, imageVisual);
  dummyControl.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
  dummyControl.ResourceReadySignal().Connect(&ResourceReadySignal);

  application.SendNotification();
  application.Render();

  // Dummy control to keep cache
  DummyControl        keepCacheControl = DummyControl::New(true);
  Impl::DummyControl& keepCacheImpl    = static_cast<Impl::DummyControl&>(keepCacheControl.GetImplementation());

  keepCacheImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, imageVisual2);
  keepCacheControl.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));

  // Load request for keep cache control.
  application.GetScene().Add(keepCacheControl);

  Property::Map  attributes;
  const uint32_t repeatMax = 10u;
  for(uint32_t repeatCnt = 0u; repeatCnt < repeatMax; ++repeatCnt)
  {
    // DoAction multiple times.
    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelImageVisual::Action::RELOAD, attributes);
  }

  application.SendNotification();
  application.Render();

  try
  {
    application.SendNotification();
    application.Render();

    tet_infoline("Async load completed. Sigabort should not be occured");
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    tet_infoline("ResourceReady signal must be fired!");
    DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);

    tet_infoline("Texture generation occured");
    DALI_TEST_EQUALS(textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION);
    DALI_TEST_EQUALS(textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION);
    textureTrace.Reset();

    tet_result(TET_PASS);
  }
  catch(...)
  {
    // Must not be throw exception.
    tet_infoline("Exception occured!");
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliControlDoActionMultipleWhenNotStage03(void)
{
  ToolkitTestApplication application;
  tet_infoline("DoAction on a visual registered with a control multiple times but not staged");

  // Set up trace debug
  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  // Created AnimatedImageVisual
  VisualFactory factory      = VisualFactory::Get();
  Visual::Base  imageVisual  = factory.CreateVisual(TEST_IMAGE_FILE_NAME, ImageDimensions());
  Visual::Base  imageVisual2 = factory.CreateVisual(TEST_IMAGE_FILE_NAME, ImageDimensions());

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());

  gResourceReadySignalFired = false;

  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, imageVisual);
  dummyControl.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
  dummyControl.ResourceReadySignal().Connect(&ResourceReadySignal);

  application.SendNotification();
  application.Render();

  // Dummy control to keep cache
  DummyControl        keepCacheControl = DummyControl::New(true);
  Impl::DummyControl& keepCacheImpl    = static_cast<Impl::DummyControl&>(keepCacheControl.GetImplementation());

  keepCacheImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, imageVisual2);
  keepCacheControl.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));

  // Load request for keep cache control.
  application.GetScene().Add(keepCacheControl);

  Property::Map  attributes;
  const uint32_t repeatMax = 10u;
  for(uint32_t repeatCnt = 0u; repeatCnt < repeatMax; ++repeatCnt)
  {
    // DoAction multiple times.
    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelImageVisual::Action::RELOAD, attributes);
  }

  application.SendNotification();
  application.Render();

  try
  {
    tet_infoline("Destroy control without stage on. And create new object that as same visual pointer as previous control");

    const auto*    imageVisualObjectPtr = imageVisual.GetObjectPtr();
    const uint32_t tryCountMax          = 100u;
    uint32_t       tryCount             = 0u;
    do
    {
      dummyControl.Reset();
      imageVisual.Reset();

      imageVisual  = factory.CreateVisual(TEST_IMAGE_FILE_NAME, ImageDimensions());
      dummyControl = DummyControl::New(true);

      Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());

      dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, imageVisual);
      dummyControl.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    } while(++tryCount < tryCountMax && imageVisualObjectPtr != imageVisual.GetObjectPtr());

    tet_printf("Luck-trial count : %u. Success? %d\n", tryCount, imageVisualObjectPtr == imageVisual.GetObjectPtr());

    // Connect signal
    dummyControl.ResourceReadySignal().Connect(&ResourceReadySignal);

    application.SendNotification();
    application.Render();

    tet_infoline("Async load completed after control destroyed. Sigabort should not be occured");
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    tet_infoline("ResourceReady signal must not be fired!");
    DALI_TEST_EQUALS(gResourceReadySignalFired, false, TEST_LOCATION);

    tet_infoline("Texture generation occured");
    DALI_TEST_EQUALS(textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION);
    DALI_TEST_EQUALS(textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION);
    textureTrace.Reset();

    tet_result(TET_PASS);
  }
  catch(...)
  {
    // Must not be throw exception.
    tet_infoline("Exception occured!");
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliControlOffScreenRendering(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  control.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  control.SetProperty(DevelControl::Property::BORDERLINE_WIDTH, 10.0f);
  control.SetProperty(DevelControl::Property::BORDERLINE_COLOR, Color::RED);
  control.SetProperty(DevelControl::Property::BORDERLINE_OFFSET, 10.0f);

  application.GetScene().Add(control);

  control.SetBackgroundColor(Color::RED);
  DALI_TEST_EQUALS(control.GetProperty(DevelControl::Property::OFFSCREEN_RENDERING).Get<int>(), (int)DevelControl::OffScreenRenderingType::NONE, TEST_LOCATION);
  tet_infoline("Control is ready");

  control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::REFRESH_ALWAYS);
  DALI_TEST_EQUALS(control.GetProperty(DevelControl::Property::OFFSCREEN_RENDERING).Get<int>(), (int)DevelControl::OffScreenRenderingType::REFRESH_ALWAYS, TEST_LOCATION);
  tet_infoline("Set offscreen rendering : refresh always");

  const Property::Value SHADOW{
    {Visual::Property::TYPE, Visual::COLOR},
    {Visual::Property::MIX_COLOR, Vector4(0.0f, 0.0f, 0.0f, 0.5f)},
    {Visual::Property::TRANSFORM,
     Property::Map{{Visual::Transform::Property::SIZE, Vector2(1.05f, 1.05f)},
                   {Visual::Transform::Property::ORIGIN, Align::CENTER},
                   {Visual::Transform::Property::ANCHOR_POINT, Align::CENTER}}}};
  control.SetProperty(DevelControl::Property::SHADOW, SHADOW);

  control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::REFRESH_ONCE);
  DALI_TEST_EQUALS(control.GetProperty(DevelControl::Property::OFFSCREEN_RENDERING).Get<int>(), (int)DevelControl::OffScreenRenderingType::REFRESH_ONCE, TEST_LOCATION);
  tet_infoline("Set offscreen rendering : refresh once");

  application.SendNotification();
  application.Render();

  control.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  tet_infoline("Size set");
  application.SendNotification();
  application.Render();

  control.Unparent();
  application.GetScene().Add(control);

  application.SendNotification();
  application.Render();

  control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::NONE);
  DALI_TEST_EQUALS(control.GetProperty(DevelControl::Property::OFFSCREEN_RENDERING).Get<int>(), (int)DevelControl::OffScreenRenderingType::NONE, TEST_LOCATION);
  tet_infoline("Turn off offscreen rendering");

  control.Unparent(); // Disconnect fron scene.
  application.GetScene().Add(control);

  control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::REFRESH_ONCE);
  DALI_TEST_EQUALS(control.GetProperty(DevelControl::Property::OFFSCREEN_RENDERING).Get<int>(), (int)DevelControl::OffScreenRenderingType::REFRESH_ONCE, TEST_LOCATION);
  tet_infoline("Set offscreen rendering : refresh once");

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliControlOffScreenRenderingSizeSet(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  control.SetProperty(Actor::Property::SIZE, Vector2::ZERO);
  application.GetScene().Add(control);

  control.SetBackgroundColor(Color::RED);
  control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::REFRESH_ALWAYS);
  DALI_TEST_EQUALS(control.GetProperty(Actor::Property::SIZE).Get<Vector2>(), Vector2::ZERO, TEST_LOCATION);

  control.SetProperty(Actor::Property::SIZE, Vector2::ZERO);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(control.GetProperty(Actor::Property::SIZE).Get<Vector2>(), Vector2::ZERO, TEST_LOCATION);
  tet_infoline("Size update: zero to zero");

  control.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(control.GetProperty(Actor::Property::SIZE).Get<Vector2>(), Vector2(100.0f, 100.0f), TEST_LOCATION);
  tet_infoline("Size update: zero to a valid size");

  control.SetProperty(Actor::Property::SIZE, Vector2(150.0f, 150.0f));
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(control.GetProperty(Actor::Property::SIZE).Get<Vector2>(), Vector2(150.0f, 150.0f), TEST_LOCATION);
  tet_infoline("Size update: size1 to size2");

  control.SetProperty(Actor::Property::SIZE, Vector2::ZERO);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(control.GetProperty(Actor::Property::SIZE).Get<Vector2>(), Vector2::ZERO, TEST_LOCATION);
  tet_infoline("Size update: a valid size to zero");

  control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::NONE);
  control.SetProperty(Actor::Property::SIZE, Vector2(50.0f, 50.0f));
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(control.GetProperty(Actor::Property::SIZE).Get<Vector2>(), Vector2(50.0f, 50.0f), TEST_LOCATION);
  tet_infoline("Size update when type NONE");

  END_TEST;
}

int UtcDaliControlOffScreenRenderingGetOutput(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  control.SetProperty(Actor::Property::SIZE, Vector2(50.0f, 50.0f));
  application.GetScene().Add(control);

  control.SetBackgroundColor(Color::RED);

  control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::REFRESH_ALWAYS);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!Toolkit::Internal::GetImplementation(control).GetOffScreenRenderingOutput()); //fails

  control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::REFRESH_ONCE);
  control.OffScreenRenderingFinishedSignal().Connect([]() -> void
  { tet_printf("Signal emitted\n"); });

  DALI_TEST_EQUALS(control.GetProperty(Actor::Property::SIZE).Get<Vector2>(), Vector2(50.0f, 50.0f), TEST_LOCATION);

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  Dali::Texture texture = Toolkit::Internal::GetImplementation(control).GetOffScreenRenderingOutput();
  DALI_TEST_EQUALS(texture.GetHeight(), 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetWidth(), 50.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlNewWithDisableVisuals(void)
{
  ToolkitTestApplication application;

  Control control = Control::New(Control::ControlBehaviour::DISABLE_VISUALS);
  control.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(0u, control.GetRendererCount(), TEST_LOCATION);

  // Check BackgroundVisual skipped
  control.SetBackgroundColor(Color::RED);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(0u, control.GetRendererCount(), TEST_LOCATION);

  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(Visual::Property::MIX_COLOR, Color::BLUE);

  // Check Background or Shadow with property map skipped
  control.SetProperty(Control::Property::BACKGROUND, propertyMap);
  control.SetProperty(DevelControl::Property::SHADOW, propertyMap);
  control.SetProperty(DevelControl::Property::INNER_SHADOW, propertyMap);
  control.SetProperty(DevelControl::Property::BORDERLINE, propertyMap);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(0u, control.GetRendererCount(), TEST_LOCATION);

  Toolkit::Visual::ResourceStatus resourceStatus = control.GetVisualResourceStatus(Control::Property::BACKGROUND);
  DALI_TEST_EQUALS(control.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(control.IsResourceReady(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(resourceStatus), static_cast<int>(Toolkit::Visual::ResourceStatus::READY), TEST_LOCATION);

  control.ClearBackground();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(0u, control.GetRendererCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlCornerRadius(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  control.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  control.SetBackgroundColor(Color::RED);
  tet_infoline("Set background visual");

  application.GetScene().Add(control);

  RenderEffect effect = BackgroundBlurEffect::New(50.0f);
  control.SetRenderEffect(effect);
  control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::REFRESH_ALWAYS);

  Vector4 radius    = Vector4(0.5f, 0.5f, 0.5f, 0.5f);
  Vector4 squreness = Vector4(0.3f, 0.3f, 0.3f, 0.3f);
  control.SetProperty(DevelControl::Property::CORNER_RADIUS, radius); // default: relative policy
  control.SetProperty(DevelControl::Property::CORNER_RADIUS_POLICY, Toolkit::Visual::Transform::Policy::Type::RELATIVE);
  control.SetProperty(DevelControl::Property::CORNER_SQUARENESS, squreness);
  tet_infoline("Sync with render effects.");

  application.SendNotification();
  application.Render();

  Vector4 retrievedVector;
  control.GetProperty(DevelControl::Property::CORNER_RADIUS).Get(retrievedVector);
  DALI_TEST_CHECK(retrievedVector == radius);
  control.GetProperty(DevelControl::Property::CORNER_SQUARENESS).Get(retrievedVector);
  DALI_TEST_CHECK(retrievedVector == squreness);

  control.ClearRenderEffect();
  control.ClearBackground();
  control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::NONE);

  control.SetBackgroundColor(Color::YELLOW);
  control.SetRenderEffect(effect);
  control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::REFRESH_ALWAYS);
  tet_infoline("Late sync with render effects.");

  END_TEST;
}

int UtcDaliControlBorderline(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  control.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  application.GetScene().Add(control);

  float   borderlineWidth  = 10.0f;
  Vector4 borderlineColor  = Color::BLUE;
  float   borderlineOffset = 0.2f;

  control.SetProperty(DevelControl::Property::BORDERLINE_WIDTH, borderlineWidth);
  control.SetProperty(DevelControl::Property::BORDERLINE_COLOR, borderlineColor);
  control.SetProperty(DevelControl::Property::BORDERLINE_OFFSET, borderlineOffset);

  application.SendNotification();
  application.Render();

  float   retrievedFloat;
  Vector4 retrievedVector;
  DALI_TEST_CHECK(control.GetProperty(DevelControl::Property::BORDERLINE_WIDTH).Get(retrievedFloat));
  DALI_TEST_EQUALS(retrievedFloat, borderlineWidth, TEST_LOCATION);
  DALI_TEST_CHECK(control.GetProperty(DevelControl::Property::BORDERLINE_COLOR).Get(retrievedVector));
  DALI_TEST_EQUALS(retrievedVector, borderlineColor, TEST_LOCATION);
  DALI_TEST_CHECK(control.GetProperty(DevelControl::Property::BORDERLINE_OFFSET).Get(retrievedFloat));
  DALI_TEST_EQUALS(retrievedFloat, borderlineOffset, TEST_LOCATION);

  Property::Map retrievedMap;

  tet_printf("Check BORDERLINE visual has not corner radius value yet.\n");

  Property::Value value = control.GetProperty(DevelControl::Property::BORDERLINE);
  DALI_TEST_CHECK(value.GetMap());
  retrievedMap = *(value.GetMap());

  Property::Value* valuePtr = retrievedMap.Find(DevelVisual::Property::CORNER_RADIUS);
  if(valuePtr)
  {
    DALI_TEST_CHECK(valuePtr->Get(retrievedVector));
    DALI_TEST_EQUALS(retrievedVector, Vector4::ZERO, TEST_LOCATION);
  }

  valuePtr = retrievedMap.Find(DevelVisual::Property::BORDERLINE_WIDTH);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedFloat));
  DALI_TEST_EQUALS(retrievedFloat, borderlineWidth, TEST_LOCATION);
  valuePtr = retrievedMap.Find(DevelVisual::Property::BORDERLINE_COLOR);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedVector));
  DALI_TEST_EQUALS(retrievedVector, borderlineColor, TEST_LOCATION);
  valuePtr = retrievedMap.Find(DevelVisual::Property::BORDERLINE_OFFSET);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedFloat));
  DALI_TEST_EQUALS(retrievedFloat, borderlineOffset, TEST_LOCATION);

  tet_printf("Set corner radius. Check BORDERLINE visual has corner radius value now.\n");
  Vector4 radius    = Vector4(0.5f, 0.5f, 0.5f, 0.5f);
  Vector4 squreness = Vector4(0.3f, 0.3f, 0.3f, 0.3f);
  control.SetProperty(DevelControl::Property::CORNER_RADIUS, radius);
  control.SetProperty(DevelControl::Property::CORNER_RADIUS_POLICY, Toolkit::Visual::Transform::Policy::Type::RELATIVE);
  control.SetProperty(DevelControl::Property::CORNER_SQUARENESS, squreness);

  value = control.GetProperty(DevelControl::Property::BORDERLINE);
  DALI_TEST_CHECK(value.GetMap());
  retrievedMap = *(value.GetMap());

  int retrievedInteger;
  valuePtr = retrievedMap.Find(DevelVisual::Property::CORNER_RADIUS);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedVector));
  DALI_TEST_EQUALS(retrievedVector, radius, TEST_LOCATION);
  valuePtr = retrievedMap.Find(DevelVisual::Property::CORNER_RADIUS_POLICY);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedInteger));
  DALI_TEST_EQUALS(retrievedInteger, (int)Toolkit::Visual::Transform::Policy::Type::RELATIVE, TEST_LOCATION);
  valuePtr = retrievedMap.Find(DevelVisual::Property::CORNER_SQUARENESS);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedVector));
  DALI_TEST_EQUALS(retrievedVector, squreness, TEST_LOCATION);

  valuePtr = retrievedMap.Find(DevelVisual::Property::BORDERLINE_WIDTH);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedFloat));
  DALI_TEST_EQUALS(retrievedFloat, borderlineWidth, TEST_LOCATION);
  valuePtr = retrievedMap.Find(DevelVisual::Property::BORDERLINE_COLOR);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedVector));
  DALI_TEST_EQUALS(retrievedVector, borderlineColor, TEST_LOCATION);
  valuePtr = retrievedMap.Find(DevelVisual::Property::BORDERLINE_OFFSET);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedFloat));
  DALI_TEST_EQUALS(retrievedFloat, borderlineOffset, TEST_LOCATION);

  tet_printf("Change borderline value. Check BORDERLINE visual.\n");
  borderlineWidth  = 20.0f;
  borderlineColor  = Color::RED;
  borderlineOffset = -1.0f;

  control.SetProperty(DevelControl::Property::BORDERLINE_WIDTH, borderlineWidth);
  control.SetProperty(DevelControl::Property::BORDERLINE_COLOR, borderlineColor);
  control.SetProperty(DevelControl::Property::BORDERLINE_OFFSET, borderlineOffset);

  value = control.GetProperty(DevelControl::Property::BORDERLINE);
  DALI_TEST_CHECK(value.GetMap());
  retrievedMap = *(value.GetMap());

  valuePtr = retrievedMap.Find(DevelVisual::Property::CORNER_RADIUS);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedVector));
  DALI_TEST_EQUALS(retrievedVector, radius, TEST_LOCATION);
  valuePtr = retrievedMap.Find(DevelVisual::Property::CORNER_RADIUS_POLICY);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedInteger));
  DALI_TEST_EQUALS(retrievedInteger, (int)Toolkit::Visual::Transform::Policy::Type::RELATIVE, TEST_LOCATION);
  valuePtr = retrievedMap.Find(DevelVisual::Property::CORNER_SQUARENESS);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedVector));
  DALI_TEST_EQUALS(retrievedVector, squreness, TEST_LOCATION);

  valuePtr = retrievedMap.Find(DevelVisual::Property::BORDERLINE_WIDTH);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedFloat));
  DALI_TEST_EQUALS(retrievedFloat, borderlineWidth, TEST_LOCATION);
  valuePtr = retrievedMap.Find(DevelVisual::Property::BORDERLINE_COLOR);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedVector));
  DALI_TEST_EQUALS(retrievedVector, borderlineColor, TEST_LOCATION);
  valuePtr = retrievedMap.Find(DevelVisual::Property::BORDERLINE_OFFSET);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedFloat));
  DALI_TEST_EQUALS(retrievedFloat, borderlineOffset, TEST_LOCATION);

  tet_printf("Set borderline property forcibly!\n");
  borderlineWidth  = 30.0f;
  borderlineColor  = Color::GREEN;
  borderlineOffset = -0.3f;
  // Just re-use retrieved map, to reduce duplicated property settings.
  retrievedMap[DevelVisual::Property::CORNER_RADIUS]        = Vector4(10.f, 20.f, 30.f, 40.f);
  retrievedMap[DevelVisual::Property::CORNER_RADIUS_POLICY] = (int)Toolkit::Visual::Transform::Policy::Type::ABSOLUTE;
  retrievedMap[DevelVisual::Property::CORNER_SQUARENESS]    = Vector4(0.7f, 0.6f, 0.5f, 0.4f);
  retrievedMap[DevelVisual::Property::BORDERLINE_WIDTH]     = borderlineWidth;
  retrievedMap[DevelVisual::Property::BORDERLINE_COLOR]     = borderlineColor;
  retrievedMap[DevelVisual::Property::BORDERLINE_OFFSET]    = borderlineOffset;

  control.SetProperty(DevelControl::Property::BORDERLINE, retrievedMap);

  value = control.GetProperty(DevelControl::Property::BORDERLINE);
  DALI_TEST_CHECK(value.GetMap());
  retrievedMap = *(value.GetMap());

  // Check we are using Control's corner radius feature instead it's own.
  valuePtr = retrievedMap.Find(DevelVisual::Property::CORNER_RADIUS);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedVector));
  DALI_TEST_EQUALS(retrievedVector, radius, TEST_LOCATION);
  valuePtr = retrievedMap.Find(DevelVisual::Property::CORNER_RADIUS_POLICY);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedInteger));
  DALI_TEST_EQUALS(retrievedInteger, (int)Toolkit::Visual::Transform::Policy::Type::RELATIVE, TEST_LOCATION);
  valuePtr = retrievedMap.Find(DevelVisual::Property::CORNER_SQUARENESS);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedVector));
  DALI_TEST_EQUALS(retrievedVector, squreness, TEST_LOCATION);

  valuePtr = retrievedMap.Find(DevelVisual::Property::BORDERLINE_WIDTH);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedFloat));
  DALI_TEST_EQUALS(retrievedFloat, borderlineWidth, TEST_LOCATION);
  valuePtr = retrievedMap.Find(DevelVisual::Property::BORDERLINE_COLOR);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedVector));
  DALI_TEST_EQUALS(retrievedVector, borderlineColor, TEST_LOCATION);
  valuePtr = retrievedMap.Find(DevelVisual::Property::BORDERLINE_OFFSET);
  DALI_TEST_CHECK(valuePtr);
  DALI_TEST_CHECK(valuePtr->Get(retrievedFloat));
  DALI_TEST_EQUALS(retrievedFloat, borderlineOffset, TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlCornerRadiusAnimation1(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliControlCornerRadiusAnimation1: Bind/unbind every visuals\n");

  Control control = Control::New();
  control.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  tet_printf("Add first visual\n");
  Property::Map background;
  background.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  background.Insert(Toolkit::Visual::Property::MIX_COLOR, Color::RED);
  background.Insert(Toolkit::Visual::Property::OPACITY, 0.2f);
  control.SetProperty(Toolkit::Control::Property::BACKGROUND, background);

  tet_printf("Add second visual\n");
  Property::Value shadow{
    {Visual::Property::TYPE, Visual::COLOR},
    {Visual::Property::MIX_COLOR, Vector4(0.0f, 0.0f, 0.0f, 0.5f)}};
  control.SetProperty(DevelControl::Property::SHADOW, shadow);

  application.GetScene().Add(control);

  tet_printf("Play animation\n");
  Vector4 radius    = Vector4(0.5f, 0.5f, 0.5f, 0.5f);
  Vector4 squreness = Vector4(0.3f, 0.3f, 0.3f, 0.3f);

  Animation animation = Animation::New(0.5f);
  animation.AnimateTo(Property(control, DevelControl::Property::CORNER_RADIUS), radius);
  animation.AnimateTo(Property(control, DevelControl::Property::CORNER_SQUARENESS), squreness);
  animation.Play();

  application.SendNotification();
  application.Render(0);

  application.SendNotification();
  application.Render(250);

  animation.Pause();

  application.SendNotification();
  application.Render(250 + 1);

  DALI_TEST_EQUALS(control.GetCurrentProperty<Vector4>(DevelControl::Property::CORNER_RADIUS), radius * 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetCurrentProperty<Vector4>(DevelControl::Property::CORNER_SQUARENESS), squreness * 0.5f, TEST_LOCATION);

  animation.Stop();
  animation.Clear();

  application.Render();
  application.SendNotification();

  END_TEST;
}

int UtcDaliControlCornerRadiusAnimation2(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliControlCornerRadiusAnimation2: Add/Remove visual during Animation::Play()\n");

  Control control = Control::New();
  control.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  tet_printf("Add first visual\n");
  Property::Map background;
  background.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  background.Insert(Toolkit::Visual::Property::MIX_COLOR, Color::RED);
  background.Insert(Toolkit::Visual::Property::OPACITY, 0.2f);
  control.SetProperty(Toolkit::Control::Property::BACKGROUND, background);

  application.GetScene().Add(control);

  tet_printf("Play two corner animations\n");
  Vector4 radius    = Vector4(0.5f, 0.5f, 0.5f, 0.5f);
  Vector4 squreness = Vector4(0.3f, 0.3f, 0.3f, 0.3f);

  Animation animation = Animation::New(0.5f);
  animation.AnimateTo(Property(control, DevelControl::Property::CORNER_RADIUS), radius);
  animation.AnimateTo(Property(control, DevelControl::Property::CORNER_SQUARENESS), squreness);
  animation.Play();

  application.SendNotification();
  application.Render(0);

  application.SendNotification();
  application.Render(250);

  tet_printf("Add second visual\n");
  Property::Value shadow{
    {Visual::Property::TYPE, Visual::COLOR},
    {Visual::Property::MIX_COLOR, Vector4(0.0f, 0.0f, 0.0f, 0.5f)}};
  control.SetProperty(DevelControl::Property::SHADOW, shadow);

  application.SendNotification();
  application.Render(250 + 1);

  DALI_TEST_EQUALS(control.GetCurrentProperty<Vector4>(DevelControl::Property::CORNER_RADIUS), radius, TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetCurrentProperty<Vector4>(DevelControl::Property::CORNER_SQUARENESS), squreness, TEST_LOCATION);

  tet_printf("Remove first visual\n");
  control.ClearBackground();

  application.SendNotification();
  application.Render();

  animation.Stop();
  animation.Clear();

  tet_printf("All constraints are gone.\n");
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(control.GetCurrentProperty<Vector4>(DevelVisual::Property::CORNER_RADIUS), Vector4::ZERO, TEST_LOCATION);

  END_TEST;
}

// Positive test case for applying a custom corner property constraint via EnableCornerPropertiesOverridden.
// Tests that a custom constraint with APPLY_ONCE apply rate correctly synchronizes the visual's
// corner radius property with the control's corner radius property when the override is enabled.
int UtcDaliControlEnableCornerPropertiesOverriddenWithCustomConstraintP(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  control.SetProperty(Toolkit::DevelControl::Property::CORNER_RADIUS, Vector4::ZERO);
  application.GetScene().Add(control);
  application.SendNotification();
  application.Render();

  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(control);

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Property::Map          visualMap;
  visualMap[Visual::Property::TYPE]           = Visual::COLOR;
  visualMap[ColorVisual::Property::MIX_COLOR] = Color::BLUE;
  Toolkit::Visual::Base visual                = visualFactory.CreateVisual(visualMap);

  DALI_TEST_CHECK(visual);

  Property::Index visualIndex = 100; // Arbitrary index for the visual
  Toolkit::DevelControl::RegisterVisual(controlImpl, visualIndex, visual);
  DALI_TEST_CHECK(Toolkit::DevelControl::IsVisualEnabled(controlImpl, visualIndex));

  Actor dummyVisualActor = Actor::New();
  application.GetScene().Add(dummyVisualActor); // Add to scene so constraint can be applied

  auto       visualCornerRadiusProperty = visual.GetPropertyObject(Dali::Toolkit::DevelVisual::Property::CORNER_RADIUS);
  Constraint customConstraint           = Constraint::New<Vector4>(visualCornerRadiusProperty.object, visualCornerRadiusProperty.propertyIndex, EqualToConstraint());
  customConstraint.AddSource(Source(control, Toolkit::DevelControl::Property::CORNER_RADIUS));

  // Apply the custom constraint using EnableCornerPropertiesOverridden.
  Dali::Toolkit::DevelControl::EnableCornerPropertiesOverridden(controlImpl, visual, true, customConstraint);

  // Change control's corner radius
  Vector4 newCornerRadius(10.0f, 10.0f, 10.0f, 10.0f);
  control.SetProperty(Toolkit::DevelControl::Property::CORNER_RADIUS, newCornerRadius);
  application.SendNotification();
  application.Render(); // Allow APPLY_ONCE constraint to trigger

  // Verify the constraint effect by checking the visual's corner radius property.
  Vector4 visualCornerRadius = visualCornerRadiusProperty.object.GetCurrentProperty<Vector4>(visualCornerRadiusProperty.propertyIndex);
  DALI_TEST_EQUALS(visualCornerRadius, newCornerRadius, TEST_LOCATION);

  application.GetScene().Remove(control);
  application.GetScene().Remove(dummyVisualActor);
  END_TEST;
}

// Test case for disabling the corner property override and ensuring the constraint is removed.
// Tests that after enabling a custom constraint and then disabling the override,
// the visual's corner radius property is no longer synchronized with the control's property.
int UtcDaliControlEnableCornerPropertiesOverriddenDisableN(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  control.SetProperty(Toolkit::DevelControl::Property::CORNER_RADIUS, Vector4::ZERO);
  application.GetScene().Add(control);
  application.SendNotification();
  application.Render();

  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(control);

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Property::Map          visualMap;
  visualMap[Visual::Property::TYPE]           = Visual::COLOR;
  visualMap[ColorVisual::Property::MIX_COLOR] = Color::GREEN;
  Toolkit::Visual::Base visual                = visualFactory.CreateVisual(visualMap);

  Property::Index visualIndex = 101;
  Toolkit::DevelControl::RegisterVisual(controlImpl, visualIndex, visual);
  DALI_TEST_CHECK(Toolkit::DevelControl::IsVisualEnabled(controlImpl, visualIndex));

  auto       visualCornerRadiusProperty = visual.GetPropertyObject(Dali::Toolkit::DevelVisual::Property::CORNER_RADIUS);
  Constraint customConstraint           = Constraint::New<Vector4>(visualCornerRadiusProperty.object, visualCornerRadiusProperty.propertyIndex, EqualToConstraint());
  customConstraint.AddSource(Source(control, Toolkit::DevelControl::Property::CORNER_RADIUS));

  Dali::Toolkit::DevelControl::EnableCornerPropertiesOverridden(controlImpl, visual, true, customConstraint);

  // Verify initial application
  Vector4 initialCornerRadius(5.0f, 5.0f, 5.0f, 5.0f);
  control.SetProperty(Toolkit::DevelControl::Property::CORNER_RADIUS, initialCornerRadius);
  application.SendNotification();
  application.Render();
  Vector4 visualRadiusAfterApply = visualCornerRadiusProperty.object.GetCurrentProperty<Vector4>(visualCornerRadiusProperty.propertyIndex);
  DALI_TEST_EQUALS(visualRadiusAfterApply, initialCornerRadius, TEST_LOCATION);

  // Now, disable the corner property override
  Dali::Toolkit::DevelControl::EnableCornerPropertiesOverridden(controlImpl, visual, false, customConstraint);

  // Change control's corner radius again
  Vector4 newCornerRadiusAfterDisable(15.0f, 15.0f, 15.0f, 15.0f);
  control.SetProperty(Toolkit::DevelControl::Property::CORNER_RADIUS, newCornerRadiusAfterDisable);
  application.SendNotification();
  application.Render();

  // The visual's corner radius should NOT have changed because the constraint was removed
  Vector4 visualRadiusAfterDisable = visualCornerRadiusProperty.object.GetCurrentProperty<Vector4>(visualCornerRadiusProperty.propertyIndex);
  DALI_TEST_EQUALS(visualRadiusAfterDisable, initialCornerRadius, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(visualRadiusAfterDisable, newCornerRadiusAfterDisable, 0.01f, TEST_LOCATION);

  application.GetScene().Remove(control);
  END_TEST;
}

// Test case for interaction between custom corner constraints and animatable property binding.
// Tests the behavior of two visuals with different constraint apply rates (APPLY_ONCE vs APPLY_ALWAYS)
// when the control's corner radius property is animated. Both visuals should follow the animation
// as the default behavior for EnableCornerPropertiesOverridden is to apply continuously.
int UtcDaliControlCornerPropertiesWithAnimationBindingP(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  control.SetProperty(Toolkit::DevelControl::Property::CORNER_RADIUS, Vector4::ZERO);
  application.GetScene().Add(control);
  application.SendNotification();
  application.Render();

  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(control);

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Property::Map          visualMap1, visualMap2;
  visualMap1[Visual::Property::TYPE]           = Visual::COLOR;
  visualMap1[ColorVisual::Property::MIX_COLOR] = Color::CYAN;
  visualMap2[Visual::Property::TYPE]           = Visual::COLOR;
  visualMap2[ColorVisual::Property::MIX_COLOR] = Color::MAGENTA;

  Toolkit::Visual::Base visual1 = visualFactory.CreateVisual(visualMap1);
  Toolkit::Visual::Base visual2 = visualFactory.CreateVisual(visualMap2);

  Property::Index visualIndex1 = 200;
  Property::Index visualIndex2 = 201;
  Toolkit::DevelControl::RegisterVisual(controlImpl, visualIndex1, visual1);
  DALI_TEST_CHECK(Toolkit::DevelControl::IsVisualEnabled(controlImpl, visualIndex1));
  Toolkit::DevelControl::RegisterVisual(controlImpl, visualIndex2, visual2);
  DALI_TEST_CHECK(Toolkit::DevelControl::IsVisualEnabled(controlImpl, visualIndex2));

  auto visualCornerRadiusProperty1 = visual1.GetPropertyObject(Dali::Toolkit::DevelVisual::Property::CORNER_RADIUS);
  auto visualCornerRadiusProperty2 = visual2.GetPropertyObject(Dali::Toolkit::DevelVisual::Property::CORNER_RADIUS);

  // Apply a custom APPLY_ONCE constraint to visual1
  Constraint customConstraint1 = Constraint::New<Vector4>(visualCornerRadiusProperty1.object, visualCornerRadiusProperty1.propertyIndex, EqualToConstraint());
  customConstraint1.AddSource(Source(control, Toolkit::DevelControl::Property::CORNER_RADIUS));
  Dali::Toolkit::DevelControl::EnableCornerPropertiesOverridden(controlImpl, visual1, true, customConstraint1);

  // Apply default corner property override to visual2 (simulated with APPLY_ALWAYS for dummy actor)
  Constraint customConstraint2 = Constraint::New<Vector4>(visualCornerRadiusProperty2.object, visualCornerRadiusProperty2.propertyIndex, EqualToConstraint());
  customConstraint2.AddSource(Source(control, Toolkit::DevelControl::Property::CORNER_RADIUS));
  Dali::Toolkit::DevelControl::EnableCornerPropertiesOverridden(controlImpl, visual2, true, customConstraint2);

  // Set initial corner radius
  Vector4 initialRadius(8.0f, 8.0f, 8.0f, 8.0f);
  control.SetProperty(Toolkit::DevelControl::Property::CORNER_RADIUS, initialRadius);
  application.SendNotification();
  application.Render();

  // Both visuals should reflect the initial radius
  Vector4 visual1RadiusInitial = visualCornerRadiusProperty1.object.GetCurrentProperty<Vector4>(visualCornerRadiusProperty1.propertyIndex);
  Vector4 visual2RadiusInitial = visualCornerRadiusProperty2.object.GetCurrentProperty<Vector4>(visualCornerRadiusProperty2.propertyIndex);
  DALI_TEST_EQUALS(visual1RadiusInitial, initialRadius, TEST_LOCATION);
  DALI_TEST_EQUALS(visual2RadiusInitial, initialRadius, TEST_LOCATION);

  // Create an animation to change the control's corner radius
  Animation animation = Animation::New(1.0f);
  Vector4   targetRadius(20.0f, 20.0f, 20.0f, 20.0f);
  animation.AnimateTo(Property(control, Toolkit::DevelControl::Property::CORNER_RADIUS), targetRadius);
  animation.Play();

  application.SendNotification();
  application.Render(500); // Halfway

  Vector4 halfwayRadius = control.GetCurrentProperty<Vector4>(Toolkit::DevelControl::Property::CORNER_RADIUS);
  DALI_TEST_NOT_EQUALS(halfwayRadius, initialRadius, 0.01f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(halfwayRadius, targetRadius, 0.01f, TEST_LOCATION);

  Vector4 visual1RadiusHalfway = visualCornerRadiusProperty1.object.GetCurrentProperty<Vector4>(visualCornerRadiusProperty1.propertyIndex);
  Vector4 visual2RadiusHalfway = visualCornerRadiusProperty2.object.GetCurrentProperty<Vector4>(visualCornerRadiusProperty2.propertyIndex);
  DALI_TEST_EQUALS(visual1RadiusHalfway, halfwayRadius, TEST_LOCATION);
  DALI_TEST_EQUALS(visual2RadiusHalfway, halfwayRadius, TEST_LOCATION);

  application.SendNotification();
  application.Render(600); // Animation finishes

  DALI_TEST_EQUALS(control.GetCurrentProperty<Vector4>(Toolkit::DevelControl::Property::CORNER_RADIUS), targetRadius, TEST_LOCATION);

  // After animation, visual1 should still have initialRadius
  // visual2 should have targetRadius
  Vector4 visual1RadiusFinal = visualCornerRadiusProperty1.object.GetCurrentProperty<Vector4>(visualCornerRadiusProperty1.propertyIndex);
  Vector4 visual2RadiusFinal = visualCornerRadiusProperty2.object.GetCurrentProperty<Vector4>(visualCornerRadiusProperty2.propertyIndex);
  DALI_TEST_EQUALS(visual1RadiusFinal, targetRadius, TEST_LOCATION);
  DALI_TEST_EQUALS(visual2RadiusFinal, targetRadius, TEST_LOCATION);

  application.GetScene().Remove(control);
  END_TEST;
}

// Positive test case for cleanup of corner property override constraints on Control destruction.
// Ensures that when a Control with overridden corner properties and active animation
// constraints is destroyed, the destructor correctly removes these constraints without causing crashes or leaks.
int UtcDaliControlCornerPropertiesOverrideCleanupOnDestructionP(void)
{
  ToolkitTestApplication application;

  // This scope ensures that the control and its internal VisualData are destroyed
  // before the test function exits.
  {
    Control control = Control::New();
    control.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
    application.GetScene().Add(control);
    application.SendNotification();
    application.Render();

    Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(control);

    // Create a visual and register it
    Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
    Property::Map          visualMap;
    visualMap[Visual::Property::TYPE]           = Visual::COLOR;
    visualMap[ColorVisual::Property::MIX_COLOR] = Color::MAGENTA;
    Toolkit::Visual::Base visual                = visualFactory.CreateVisual(visualMap);

    Property::Index visualIndex = 300; // Arbitrary index
    Toolkit::DevelControl::RegisterVisual(controlImpl, visualIndex, visual);
    DALI_TEST_CHECK(Toolkit::DevelControl::IsVisualEnabled(controlImpl, visualIndex));

    // Create a dummy actor for the constraint target
    // This simulates the target of the constraint within the visual.
    // In a real scenario, this would be a property of the visual itself.
    // For testing the cleanup in VisualData, we just need the constraint to be created and stored.
    Actor dummyVisualActor = Actor::New();
    application.GetScene().Add(dummyVisualActor); // Add to stage so constraint can be applied

    auto visualCornerRadiusProperty = visual.GetPropertyObject(Dali::Toolkit::DevelVisual::Property::CORNER_RADIUS);

    // Apply a custom constraint with APPLY_ALWAYS (or continuous) to ensure it stays in animationConstraint map
    // Using EqualToConstraint which is continuous by default.
    Constraint customConstraint = Constraint::New<Vector4>(visualCornerRadiusProperty.object, visualCornerRadiusProperty.propertyIndex, EqualToConstraint());
    customConstraint.AddSource(Source(control, Toolkit::DevelControl::Property::CORNER_RADIUS));
    // Note: We don't call customConstraint.Apply() here. EnableCornerPropertiesOverridden will handle it.

    Dali::Toolkit::DevelControl::EnableCornerPropertiesOverridden(controlImpl, visual, true, customConstraint);

    // Set a corner radius to trigger the constraint
    control.SetProperty(Toolkit::DevelControl::Property::CORNER_RADIUS, Vector4(10.0f, 10.0f, 10.0f, 10.0f));
    application.SendNotification();
    application.Render();

    // At this point, the internal VisualData for the control should have:
    // - A RegisteredVisual with overrideCornerProperties = true
    // - At least one entry in its animationConstraint map

    tet_infoline("Control with overridden corner properties is about to go out of scope.");
  } // <- Control 'control' goes out of scope here. Its destructor (~Control) -> ~ControlImpl -> ~VisualData is called.
  // This is where the new cleanup logic in control-visual-data.cpp should be executed.

  tet_infoline("Control has been destroyed. If no crash occurred, the cleanup was successful.");

  // Render the application again to ensure no dangling references or issues persist.
  application.SendNotification();
  application.Render();

  // The test passes if it reaches this point without any crashes or sanitizer errors.
  // The testing framework (e.g., LeakSanitizer, AddressSanitizer if enabled) would catch
  // issues like memory leaks or invalid accesses during destruction.
  END_TEST;
}
