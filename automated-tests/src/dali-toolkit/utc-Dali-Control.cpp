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

  Property::Map emptyMap;
  control.SetProperty(DevelControl::Property::SHADOW, emptyMap);
  DALI_TEST_CHECK(control.GetProperty(DevelControl::Property::SHADOW).Get<Property::Map>().Empty());

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

  //Created AnimatedImageVisual
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

  //Created AnimatedImageVisual
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

  //Created AnimatedImageVisual
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

  //Created AnimatedImageVisual
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

  //Created AnimatedImageVisual
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

  application.GetScene().Add(control);

  control.SetBackgroundColor(Color::RED);
  DALI_TEST_EQUALS(control.GetProperty(DevelControl::Property::OFFSCREEN_RENDERING).Get<int>(), (int)DevelControl::OffScreenRenderingType::NONE, TEST_LOCATION);
  tet_infoline("Control is ready");

  control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::REFRESH_ALWAYS);
  DALI_TEST_EQUALS(control.GetProperty(DevelControl::Property::OFFSCREEN_RENDERING).Get<int>(), (int)DevelControl::OffScreenRenderingType::REFRESH_ALWAYS, TEST_LOCATION);
  tet_infoline("Set offscreen rendering : refresh always");

  control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::REFRESH_ONCE);
  DALI_TEST_EQUALS(control.GetProperty(DevelControl::Property::OFFSCREEN_RENDERING).Get<int>(), (int)DevelControl::OffScreenRenderingType::REFRESH_ONCE, TEST_LOCATION);
  tet_infoline("Set offscreen rendering : refresh once");

  application.SendNotification();
  application.Render();

  control.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  tet_infoline("Size set");
  application.SendNotification();
  application.Render();

  control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::NONE);
  DALI_TEST_EQUALS(control.GetProperty(DevelControl::Property::OFFSCREEN_RENDERING).Get<int>(), (int)DevelControl::OffScreenRenderingType::NONE, TEST_LOCATION);
  tet_infoline("Turn off offscreen rendering");

  control.Unparent(); // Disconnect fron scene.
  application.GetScene().Add(control);

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

  RenderEffect effect = RenderEffect::CreateBackgroundBlurEffect(0.25f, 50.0f);
  control.SetRenderEffect(effect);
  control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::REFRESH_ALWAYS);

  Vector4 vector = Vector4(0.5f, 0.5f, 0.5f, 0.5f);
  control.SetProperty(DevelControl::Property::CORNER_RADIUS, vector); // default: relative policy
  control.SetProperty(DevelControl::Property::CORNER_RADIUS_POLICY, Toolkit::Visual::Transform::Policy::Type::RELATIVE);
  tet_infoline("Sync with render effects.");

  application.SendNotification();
  application.Render();

  Vector4 retrievedVector;
  control.GetProperty(DevelControl::Property::CORNER_RADIUS).Get(retrievedVector);
  DALI_TEST_CHECK(retrievedVector == vector);

  control.ClearRenderEffect();
  control.ClearBackground();
  control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::NONE);

  control.SetBackgroundColor(Color::YELLOW);
  control.SetRenderEffect(effect);
  control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::REFRESH_ALWAYS);
  tet_infoline("Late sync with render effects.");

  END_TEST;
}
