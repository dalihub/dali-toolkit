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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/public-api/controls/render-effects/render-effect.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>

using namespace Dali;
using namespace Dali::Toolkit;

int UtcDaliRenderEffectNewP(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectNewP");

  RenderEffect blurEffect = RenderEffect::CreateBackgroundBlurEffect();
  DALI_TEST_CHECK(blurEffect);

  RenderEffect blurEffect2 = RenderEffect::CreateBackgroundBlurEffect(0.5f, 10);
  DALI_TEST_CHECK(blurEffect2);

  END_TEST;
}

int UtcDaliRenderEffectNewN(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectNewN");

  tet_printf("Check some invalid parameters clamp internally\n");

  RenderEffect blurEffect  = RenderEffect::CreateBackgroundBlurEffect(-0.5f, 10);
  RenderEffect blurEffect2 = RenderEffect::CreateBackgroundBlurEffect(10.0f, 10);
  RenderEffect blurEffect3 = RenderEffect::CreateBackgroundBlurEffect(0.5f, 0);
  RenderEffect blurEffect4 = RenderEffect::CreateBackgroundBlurEffect(0.5f, 2147483647);

  DALI_TEST_CHECK(blurEffect);
  DALI_TEST_CHECK(blurEffect2);
  DALI_TEST_CHECK(blurEffect3);
  DALI_TEST_CHECK(blurEffect4);

  END_TEST;
}

int UtcDaliRenderEffectActivateP01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectActivateP01");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  Control childControl = Control::New();
  childControl.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  scene.Add(control);
  control.Add(childControl);

  RenderTaskList taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);

  childControl.SetRenderEffect(RenderEffect::CreateBackgroundBlurEffect());

  taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectActivateP02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectActivateP02");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  scene.Add(control);

  RenderEffect blurEffect = RenderEffect::CreateBackgroundBlurEffect();
  control.SetRenderEffect(blurEffect);

  RenderTaskList taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);

  Control control2 = Control::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control2.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  scene.Add(control2);

  control2.SetRenderEffect(blurEffect); // Clone effect
  taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(7u, taskList.GetTaskCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectDeactivateP(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectDeactivateP");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  scene.Add(control);

  uint32_t count = control.GetRendererCount();
  control.SetRenderEffect(RenderEffect::CreateBackgroundBlurEffect());

  RenderTaskList taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, control.GetRendererCount(), TEST_LOCATION);

  control.ClearRenderEffect();
  taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectDeactivateN(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectDeactivateN");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  scene.Add(control);

  RenderTaskList taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  control.ClearRenderEffect(); // Nothing happens
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectActivateDeactivateInplace(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectActivateDeactivateInplace");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  scene.Add(control);

  RenderEffect blurEffect = RenderEffect::CreateBackgroundBlurEffect();
  control.SetRenderEffect(blurEffect);

  RenderTaskList taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);

  control.ClearRenderEffect();
  control.SetRenderEffect(blurEffect);
  control.ClearRenderEffect();
  control.SetRenderEffect(blurEffect);
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectReassign(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectReassign");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  scene.Add(control);

  RenderEffect blurEffect = RenderEffect::CreateBackgroundBlurEffect();
  control.SetRenderEffect(blurEffect);
  control.SetRenderEffect(blurEffect); // Duplicate actions will be ignored
  control.SetRenderEffect(blurEffect); // Duplicate actions will be ignored
  control.SetRenderEffect(blurEffect); // Duplicate actions will be ignored
  RenderTaskList taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectResize(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectResize");

  Integration::Scene scene   = application.GetScene();
  Control            control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  scene.Add(control);
  control.SetRenderEffect(RenderEffect::CreateBackgroundBlurEffect());

  application.SendNotification();
  application.Render();

  control.SetProperty(Actor::Property::SIZE, Vector2(30.0f, 30.0f));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(control.GetProperty<float>(Actor::Property::SIZE_WIDTH), 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 30.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectSynchronizeControlCornerRadius(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectSynchronizeBackgroundCornerRadius");

  Integration::Scene scene = application.GetScene();

  Property::Map blackDimmerMap;
  blackDimmerMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  blackDimmerMap.Insert(Toolkit::Visual::Property::MIX_COLOR, Color::BLACK);
  blackDimmerMap.Insert(Toolkit::Visual::Property::OPACITY, 0.2f);
  blackDimmerMap.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, 0.20f);
  blackDimmerMap.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY, Visual::Transform::Policy::Type::RELATIVE);
  blackDimmerMap.Insert(Toolkit::DevelVisual::Property::CORNER_SQUARENESS, 0.3f);
  RenderEffect effect = RenderEffect::CreateBackgroundBlurEffect();

  Control control = Control::New();
  DALI_TEST_CHECK(control.GetRendererCount() == 0u);
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  control.SetProperty(Toolkit::DevelControl::Property::CORNER_RADIUS, Vector4(30.0f, 30.0f, 30.0f, 30.0f));
  control.SetProperty(Toolkit::DevelControl::Property::CORNER_RADIUS_POLICY, Visual::Transform::Policy::Type::ABSOLUTE);
  // TODO set squarness
  scene.Add(control);

  control.SetProperty(Toolkit::Control::Property::BACKGROUND, blackDimmerMap);
  DALI_TEST_CHECK(control.GetRendererCount() == 1u);
  control.SetRenderEffect(effect);
  DALI_TEST_CHECK(control.GetRendererCount() == 2u);

  Renderer renderer = control.GetRendererAt(1u);
  Vector4  radius   = Vector4::ZERO;
  renderer.GetProperty(renderer.GetPropertyIndex(std::string("uCornerRadius"))).Get(radius);

  //Vector4 squreness = Vector4::ZERO;
  //renderer.GetProperty(renderer.GetPropertyIndex(std::string("uCornerSquareness"))).Get(squreness);

  Toolkit::Visual::Transform::Policy::Type policy;
  renderer.GetProperty(renderer.GetPropertyIndex(std::string("uCornerRadiusPolicy"))).Get(policy);
  DALI_TEST_CHECK(policy == Visual::Transform::Policy::Type::ABSOLUTE);

  DALI_TEST_CHECK(radius.x == 30.0f);
  DALI_TEST_CHECK(radius.y == 30.0f);
  DALI_TEST_CHECK(radius.z == 30.0f);
  DALI_TEST_CHECK(radius.w == 30.0f);

  //DALI_TEST_CHECK(squreness.x == 0.3f);
  //DALI_TEST_CHECK(squreness.y == 0.3f);
  //DALI_TEST_CHECK(squreness.z == 0.3f);
  //DALI_TEST_CHECK(squreness.w == 0.3f);

  END_TEST;
}

int UtcDaliRenderEffectInvalidTargetSize(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectInvalidTargetSize");

  Integration::Scene scene          = application.GetScene();
  const uint32_t     maxTextureSize = Dali::GetMaxTextureSize();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE_WIDTH, maxTextureSize + 1000.0f);
  control.SetProperty(Actor::Property::SIZE_HEIGHT, maxTextureSize + 1000.0f);
  scene.Add(control);
  control.SetRenderEffect(RenderEffect::CreateBackgroundBlurEffect(0.4f, 40));

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(true); // no error

  control.SetProperty(Actor::Property::SIZE_WIDTH, -10.0f);
  control.SetProperty(Actor::Property::SIZE_HEIGHT, -10.0f);

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(true); // no error

  END_TEST;
}

int UtcDaliRenderEffectControlSceneOnAndSceneOff01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectControlSceneOnAndSceneOff01");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  uint32_t count = control.GetRendererCount();

  // Add render effect during scene off.
  control.SetRenderEffect(RenderEffect::CreateBackgroundBlurEffect());

  RenderTaskList taskList = scene.GetRenderTaskList();

  // Still render effect is not activated.
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  scene.Add(control);
  // Render effect activated.
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, control.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  control.Unparent();
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  scene.Add(control);
  // Render effect activated.
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, control.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  control.Unparent();
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  control.ClearRenderEffect();
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  scene.Add(control);
  // Render effect not activated.
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectControlSceneOnAndSceneOff02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectControlSceneOnAndSceneOff02");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  uint32_t count = control.GetRendererCount();
  scene.Add(control);

  // Add render effect during scene on.
  control.SetRenderEffect(RenderEffect::CreateBackgroundBlurEffect());

  RenderTaskList taskList = scene.GetRenderTaskList();

  // Render effect activated.
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, control.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  control.Unparent();
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  scene.Add(control);
  // Render effect activated.
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, control.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  control.Unparent();
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  control.ClearRenderEffect();
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  scene.Add(control);
  // Render effect not activated.
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectControlVisiblityChanged01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectControlVisiblityChanged01");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  uint32_t count = control.GetRendererCount();
  scene.Add(control);

  // Add render effect during invisible.
  control.SetProperty(Actor::Property::VISIBLE, false);
  control.SetRenderEffect(RenderEffect::CreateBackgroundBlurEffect());

  RenderTaskList taskList = scene.GetRenderTaskList();

  // Still render effect is not activated.
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  // Render effect activated.
  control.SetProperty(Actor::Property::VISIBLE, true);
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, control.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  control.SetProperty(Actor::Property::VISIBLE, false);
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  control.Unparent();
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  // Render effect still deactivated.
  control.SetProperty(Actor::Property::VISIBLE, true);
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  // Render effect activated.
  scene.Add(control);
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, control.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  control.SetProperty(Actor::Property::VISIBLE, false);
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  control.ClearRenderEffect();
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  control.SetProperty(Actor::Property::VISIBLE, true);
  // Render effect not activated.
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectControlVisiblityChanged02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectControlVisiblityChanged02");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  uint32_t count = control.GetRendererCount();
  scene.Add(control);

  // Add render effect during scene on.
  control.SetRenderEffect(RenderEffect::CreateBackgroundBlurEffect());

  RenderTaskList taskList = scene.GetRenderTaskList();

  // Render effect activated.
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, control.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  control.SetProperty(Actor::Property::VISIBLE, false);
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  control.SetProperty(Actor::Property::VISIBLE, true);
  // Render effect activated.
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, control.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  control.SetProperty(Actor::Property::VISIBLE, false);
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  control.ClearRenderEffect();
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  control.SetProperty(Actor::Property::VISIBLE, true);
  // Render effect not activated.
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectRenderTaskOrdering(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectRenderTaskOrdering");

  Integration::Scene scene    = application.GetScene();
  RenderTaskList     taskList = scene.GetRenderTaskList();

  Control control1 = Control::New();
  control1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control1.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  tet_printf("render task cnt : %d\n", taskList.GetTaskCount());

  // Add render effect during scene on.
  control1.SetRenderEffect(RenderEffect::CreateBackgroundBlurEffect());

  tet_printf("render task cnt after set : %d\n", taskList.GetTaskCount());
  DALI_TEST_EQUALS(1, taskList.GetTaskCount(), TEST_LOCATION);

  scene.Add(control1);

  tet_printf("render task cnt after add : %d\n", taskList.GetTaskCount());
  DALI_TEST_EQUALS(4, taskList.GetTaskCount(), TEST_LOCATION);

  Dali::RenderTask sourceTaskControl1         = taskList.GetTask(taskList.GetTaskCount() - 3);
  Dali::RenderTask horizontalBlurTaskControl1 = taskList.GetTask(taskList.GetTaskCount() - 2);
  Dali::RenderTask verticalBlurTaskControl1   = taskList.GetTask(taskList.GetTaskCount() - 1);

  tet_printf("order : %d\n", sourceTaskControl1.GetOrderIndex());
  tet_printf("order : %d\n", horizontalBlurTaskControl1.GetOrderIndex());
  tet_printf("order : %d\n", verticalBlurTaskControl1.GetOrderIndex());

  DALI_TEST_EQUALS(0, sourceTaskControl1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, horizontalBlurTaskControl1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, verticalBlurTaskControl1.GetOrderIndex(), TEST_LOCATION);

  application.SendNotification();

  tet_printf("order af : %d\n", sourceTaskControl1.GetOrderIndex());
  tet_printf("order af : %d\n", horizontalBlurTaskControl1.GetOrderIndex());
  tet_printf("order af : %d\n", verticalBlurTaskControl1.GetOrderIndex());

  DALI_TEST_EQUALS(INT32_MIN, sourceTaskControl1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 1, horizontalBlurTaskControl1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 2, verticalBlurTaskControl1.GetOrderIndex(), TEST_LOCATION);

  Control control2 = Control::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control2.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  tet_printf("render task cnt : %d\n", taskList.GetTaskCount());

  // Add render effect during scene on.
  control2.SetRenderEffect(RenderEffect::CreateBackgroundBlurEffect());

  tet_printf("render task cnt after set : %d\n", taskList.GetTaskCount());

  scene.Add(control2);

  DALI_TEST_EQUALS(7, taskList.GetTaskCount(), TEST_LOCATION);

  tet_printf("render task cnt after add : %d\n", taskList.GetTaskCount());

  Dali::RenderTask sourceTaskControl2         = taskList.GetTask(taskList.GetTaskCount() - 3);
  Dali::RenderTask horizontalBlurTaskControl2 = taskList.GetTask(taskList.GetTaskCount() - 2);
  Dali::RenderTask verticalBlurTaskControl2   = taskList.GetTask(taskList.GetTaskCount() - 1);

  tet_printf("order after1 : %d\n", sourceTaskControl1.GetOrderIndex());
  tet_printf("order after1 : %d\n", horizontalBlurTaskControl1.GetOrderIndex());
  tet_printf("order after1 : %d\n", verticalBlurTaskControl1.GetOrderIndex());

  tet_printf("order after1 : %d\n", sourceTaskControl2.GetOrderIndex());
  tet_printf("order after1 : %d\n", horizontalBlurTaskControl2.GetOrderIndex());
  tet_printf("order after1 : %d\n", verticalBlurTaskControl2.GetOrderIndex());

  DALI_TEST_EQUALS(0, sourceTaskControl2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, horizontalBlurTaskControl2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, verticalBlurTaskControl2.GetOrderIndex(), TEST_LOCATION);

  application.SendNotification();

  tet_printf("order after2 : %d\n", sourceTaskControl1.GetOrderIndex());
  tet_printf("order after2 : %d\n", horizontalBlurTaskControl1.GetOrderIndex());
  tet_printf("order after2 : %d\n", verticalBlurTaskControl1.GetOrderIndex());

  tet_printf("order after2 : %d\n", sourceTaskControl2.GetOrderIndex());
  tet_printf("order after2 : %d\n", horizontalBlurTaskControl2.GetOrderIndex());
  tet_printf("order after2 : %d\n", verticalBlurTaskControl2.GetOrderIndex());

  DALI_TEST_EQUALS(INT32_MIN, sourceTaskControl1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 1, horizontalBlurTaskControl1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 2, verticalBlurTaskControl1.GetOrderIndex(), TEST_LOCATION);

  DALI_TEST_EQUALS(INT32_MIN + 3, sourceTaskControl2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 4, horizontalBlurTaskControl2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 5, verticalBlurTaskControl2.GetOrderIndex(), TEST_LOCATION);

  control2.Add(control1);

  sourceTaskControl1         = taskList.GetTask(taskList.GetTaskCount() - 3);
  horizontalBlurTaskControl1 = taskList.GetTask(taskList.GetTaskCount() - 2);
  verticalBlurTaskControl1   = taskList.GetTask(taskList.GetTaskCount() - 1);

  tet_printf("order after3 : %d\n", sourceTaskControl1.GetOrderIndex());
  tet_printf("order after3 : %d\n", horizontalBlurTaskControl1.GetOrderIndex());
  tet_printf("order after3 : %d\n", verticalBlurTaskControl1.GetOrderIndex());

  tet_printf("order after3 : %d\n", sourceTaskControl2.GetOrderIndex());
  tet_printf("order after3 : %d\n", horizontalBlurTaskControl2.GetOrderIndex());
  tet_printf("order after3 : %d\n", verticalBlurTaskControl2.GetOrderIndex());

  DALI_TEST_EQUALS(0, sourceTaskControl1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, horizontalBlurTaskControl1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, verticalBlurTaskControl1.GetOrderIndex(), TEST_LOCATION);

  DALI_TEST_EQUALS(INT32_MIN + 3, sourceTaskControl2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 4, horizontalBlurTaskControl2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 5, verticalBlurTaskControl2.GetOrderIndex(), TEST_LOCATION);

  application.SendNotification();

  tet_printf("order after4 : %d\n", sourceTaskControl1.GetOrderIndex());
  tet_printf("order after4 : %d\n", horizontalBlurTaskControl1.GetOrderIndex());
  tet_printf("order after4 : %d\n", verticalBlurTaskControl1.GetOrderIndex());

  tet_printf("order after4 : %d\n", sourceTaskControl2.GetOrderIndex());
  tet_printf("order after4 : %d\n", horizontalBlurTaskControl2.GetOrderIndex());
  tet_printf("order after4 : %d\n", verticalBlurTaskControl2.GetOrderIndex());

  DALI_TEST_EQUALS(INT32_MIN + 3, sourceTaskControl1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 4, horizontalBlurTaskControl1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 5, verticalBlurTaskControl1.GetOrderIndex(), TEST_LOCATION);

  DALI_TEST_EQUALS(INT32_MIN, sourceTaskControl2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 1, horizontalBlurTaskControl2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 2, verticalBlurTaskControl2.GetOrderIndex(), TEST_LOCATION);

  control1.SetProperty(Actor::Property::VISIBLE, false);
  control2.Unparent();

  tet_printf("render task cnt after unparent : %d\n", taskList.GetTaskCount());

  // The order index is not defined now.
  tet_printf("order after5 : %d\n", sourceTaskControl1.GetOrderIndex());
  tet_printf("order after5 : %d\n", horizontalBlurTaskControl1.GetOrderIndex());
  tet_printf("order after5 : %d\n", verticalBlurTaskControl1.GetOrderIndex());

  tet_printf("order after5 : %d\n", sourceTaskControl2.GetOrderIndex());
  tet_printf("order after5 : %d\n", horizontalBlurTaskControl2.GetOrderIndex());
  tet_printf("order after5 : %d\n", verticalBlurTaskControl2.GetOrderIndex());

  application.SendNotification();

  tet_printf("order after6 : %d\n", sourceTaskControl1.GetOrderIndex());
  tet_printf("order after6 : %d\n", horizontalBlurTaskControl1.GetOrderIndex());
  tet_printf("order after6 : %d\n", verticalBlurTaskControl1.GetOrderIndex());

  tet_printf("order after6 : %d\n", sourceTaskControl2.GetOrderIndex());
  tet_printf("order after6 : %d\n", horizontalBlurTaskControl2.GetOrderIndex());
  tet_printf("order after6 : %d\n", verticalBlurTaskControl2.GetOrderIndex());

  scene.Add(control2);

  tet_printf("render task cnt after re-add : %d\n", taskList.GetTaskCount());

  sourceTaskControl2         = taskList.GetTask(taskList.GetTaskCount() - 3);
  horizontalBlurTaskControl2 = taskList.GetTask(taskList.GetTaskCount() - 2);
  verticalBlurTaskControl2   = taskList.GetTask(taskList.GetTaskCount() - 1);

  tet_printf("order after7 : %d\n", sourceTaskControl1.GetOrderIndex());
  tet_printf("order after7 : %d\n", horizontalBlurTaskControl1.GetOrderIndex());
  tet_printf("order after7 : %d\n", verticalBlurTaskControl1.GetOrderIndex());

  tet_printf("order after7 : %d\n", sourceTaskControl2.GetOrderIndex());
  tet_printf("order after7 : %d\n", horizontalBlurTaskControl2.GetOrderIndex());
  tet_printf("order after7 : %d\n", verticalBlurTaskControl2.GetOrderIndex());

  DALI_TEST_EQUALS(0, sourceTaskControl2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, horizontalBlurTaskControl2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, verticalBlurTaskControl2.GetOrderIndex(), TEST_LOCATION);

  application.SendNotification();

  tet_printf("order after8 : %d\n", sourceTaskControl1.GetOrderIndex());
  tet_printf("order after8 : %d\n", horizontalBlurTaskControl1.GetOrderIndex());
  tet_printf("order after8 : %d\n", verticalBlurTaskControl1.GetOrderIndex());

  tet_printf("order after8 : %d\n", sourceTaskControl2.GetOrderIndex());
  tet_printf("order after8 : %d\n", horizontalBlurTaskControl2.GetOrderIndex());
  tet_printf("order after8 : %d\n", verticalBlurTaskControl2.GetOrderIndex());

  DALI_TEST_EQUALS(INT32_MIN, sourceTaskControl2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 1, horizontalBlurTaskControl2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 2, verticalBlurTaskControl2.GetOrderIndex(), TEST_LOCATION);

  control1.SetProperty(Actor::Property::VISIBLE, true);

  tet_printf("render task cnt after change visible : %d\n", taskList.GetTaskCount());

  sourceTaskControl1         = taskList.GetTask(taskList.GetTaskCount() - 3);
  horizontalBlurTaskControl1 = taskList.GetTask(taskList.GetTaskCount() - 2);
  verticalBlurTaskControl1   = taskList.GetTask(taskList.GetTaskCount() - 1);

  tet_printf("order after9 : %d\n", sourceTaskControl1.GetOrderIndex());
  tet_printf("order after9 : %d\n", horizontalBlurTaskControl1.GetOrderIndex());
  tet_printf("order after9 : %d\n", verticalBlurTaskControl1.GetOrderIndex());

  tet_printf("order after9 : %d\n", sourceTaskControl2.GetOrderIndex());
  tet_printf("order after9 : %d\n", horizontalBlurTaskControl2.GetOrderIndex());
  tet_printf("order after9 : %d\n", verticalBlurTaskControl2.GetOrderIndex());

  DALI_TEST_EQUALS(0, sourceTaskControl1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, horizontalBlurTaskControl1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, verticalBlurTaskControl1.GetOrderIndex(), TEST_LOCATION);

  DALI_TEST_EQUALS(INT32_MIN, sourceTaskControl2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 1, horizontalBlurTaskControl2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 2, verticalBlurTaskControl2.GetOrderIndex(), TEST_LOCATION);

  application.SendNotification();

  tet_printf("order after10 : %d\n", sourceTaskControl1.GetOrderIndex());
  tet_printf("order after10 : %d\n", horizontalBlurTaskControl1.GetOrderIndex());
  tet_printf("order after10 : %d\n", verticalBlurTaskControl1.GetOrderIndex());

  tet_printf("order after10 : %d\n", sourceTaskControl2.GetOrderIndex());
  tet_printf("order after10 : %d\n", horizontalBlurTaskControl2.GetOrderIndex());
  tet_printf("order after10 : %d\n", verticalBlurTaskControl2.GetOrderIndex());

  DALI_TEST_EQUALS(INT32_MIN + 3, sourceTaskControl1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 4, horizontalBlurTaskControl1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 5, verticalBlurTaskControl1.GetOrderIndex(), TEST_LOCATION);

  DALI_TEST_EQUALS(INT32_MIN, sourceTaskControl2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 1, horizontalBlurTaskControl2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 2, verticalBlurTaskControl2.GetOrderIndex(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectReInitialize(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectReInitialize");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  scene.Add(control);

  // Add render effect during scene on.
  control.SetRenderEffect(RenderEffect::CreateBackgroundBlurEffect());

  application.SendNotification();

  RenderTaskList taskList = scene.GetRenderTaskList();

  // Render effect activated.
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  tet_printf("order : %d\n", taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex());
  DALI_TEST_EQUALS(INT32_MIN + 2, taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex(), TEST_LOCATION);

  control.SetRenderEffect(RenderEffect::CreateBackgroundBlurEffect());

  application.SendNotification();

  // Render effect activated.
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  tet_printf("order : %d\n", taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex());
  DALI_TEST_EQUALS(INT32_MIN + 2, taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex(), TEST_LOCATION);

  END_TEST;
}
