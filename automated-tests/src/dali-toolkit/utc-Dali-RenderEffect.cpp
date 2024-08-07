/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/public-api/controls/render-effects/background-blur-effect.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>

using namespace Dali;
using namespace Dali::Toolkit;

int UtcDaliRenderEffectNewP(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectNewP");

  BackgroundBlurEffect blurEffect = BackgroundBlurEffect::New();
  DALI_TEST_CHECK(blurEffect);

  BackgroundBlurEffect blurEffect2 = BackgroundBlurEffect::New(0.5f, 10.0f);
  DALI_TEST_CHECK(blurEffect2);

  END_TEST;
}

int UtcDaliRenderEffectNewN(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectNewN");

  try
  {
    BackgroundBlurEffect blurEffect  = BackgroundBlurEffect::New(-0.5f, 10.0f);
    BackgroundBlurEffect blurEffect2 = BackgroundBlurEffect::New(10.0f, 10.0f);
    DALI_TEST_CHECK(!blurEffect && !blurEffect2);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_CHECK(true);
  }
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

  childControl.SetRenderEffect(BackgroundBlurEffect::New());

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

  BackgroundBlurEffect blurEffect = BackgroundBlurEffect::New();
  control.SetRenderEffect(blurEffect);

  RenderTaskList taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);

  Control control2 = Control::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control2.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  scene.Add(control2);

  control2.SetRenderEffect(blurEffect);
  taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);

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
  control.SetRenderEffect(BackgroundBlurEffect::New());

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

  BackgroundBlurEffect blurEffect = BackgroundBlurEffect::New();
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

  BackgroundBlurEffect blurEffect = BackgroundBlurEffect::New();
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
  control.SetRenderEffect(BackgroundBlurEffect::New());

  application.SendNotification();
  application.Render();

  control.SetProperty(Actor::Property::SIZE, Vector2(30.0f, 30.0f));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(control.GetProperty<float>(Actor::Property::SIZE_WIDTH), 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 30.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectSynchronizeBackgroundCornerRadius(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectSynchronizeBackgroundCornerRadius");

  Integration::Scene scene = application.GetScene();

  Property::Map blackDimmerMap;
  blackDimmerMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  blackDimmerMap.Insert(Toolkit::Visual::Property::MIX_COLOR, Color::BLACK);
  blackDimmerMap.Insert(Toolkit::Visual::Property::OPACITY, 0.2f);
  blackDimmerMap.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, 30.0f);

  RenderEffect effect = BackgroundBlurEffect::New(0.4f, 40);

  Control control = Control::New();
  DALI_TEST_CHECK(control.GetRendererCount() == 0u);
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  scene.Add(control);

  control.SetProperty(Toolkit::Control::Property::BACKGROUND, blackDimmerMap);
  DALI_TEST_CHECK(control.GetRendererCount() == 1u);
  control.SetRenderEffect(effect);
  DALI_TEST_CHECK(control.GetRendererCount() == 2u);

  Renderer renderer = control.GetRendererAt(1u);
  Vector4  radius   = Vector4::ZERO;
  renderer.GetProperty(renderer.GetPropertyIndex(std::string("uCornerRadius"))).Get(radius);

  Toolkit::Visual::Transform::Policy::Type policy;
  renderer.GetProperty(renderer.GetPropertyIndex(std::string("uCornerRadiusPolicy"))).Get(policy);
  DALI_TEST_CHECK(policy == 1);

  DALI_TEST_CHECK(radius.x == 30.0f);
  DALI_TEST_CHECK(radius.y == 30.0f);
  DALI_TEST_CHECK(radius.z == 30.0f);
  DALI_TEST_CHECK(radius.w == 30.0f);

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
  control.SetRenderEffect(BackgroundBlurEffect::New(0.4f, 40));

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
  control.SetRenderEffect(BackgroundBlurEffect::New());

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
  control.SetRenderEffect(BackgroundBlurEffect::New());

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
  control.SetRenderEffect(BackgroundBlurEffect::New());

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
  control.SetRenderEffect(BackgroundBlurEffect::New());

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
