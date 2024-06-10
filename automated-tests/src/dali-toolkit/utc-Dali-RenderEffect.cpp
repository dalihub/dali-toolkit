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

using namespace Dali;
using namespace Dali::Toolkit;

int UtcDaliRenderEffectNewP(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectNewP");

  BackgroundBlurEffect blurEffect = BackgroundBlurEffect::New();
  DALI_TEST_CHECK(blurEffect);

  BackgroundBlurEffect blurEffect2 = BackgroundBlurEffect::New(0.5f, 10.0f, 10.0f);
  DALI_TEST_CHECK(blurEffect2);

  END_TEST;
}

int UtcDaliRenderEffectNewN(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectNewN");

  try
  {
    BackgroundBlurEffect blurEffect  = BackgroundBlurEffect::New(-0.5f, 10.0f, 10.0f);
    BackgroundBlurEffect blurEffect2 = BackgroundBlurEffect::New(10.0f, 10.0f, 10.0f);
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
  DALI_TEST_CHECK(1u == taskList.GetTaskCount());

  childControl.SetRenderEffect(BackgroundBlurEffect::New());

  taskList = scene.GetRenderTaskList();
  DALI_LOG_ERROR("%d\n", taskList.GetTaskCount());
  DALI_TEST_CHECK(4u == taskList.GetTaskCount());

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
  DALI_TEST_CHECK(4u == taskList.GetTaskCount());

  Control control2 = Control::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control2.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  scene.Add(control2);

  control2.SetRenderEffect(blurEffect); // Ownership changed.
  taskList = scene.GetRenderTaskList();
  DALI_TEST_CHECK(4u == taskList.GetTaskCount());

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
  DALI_TEST_CHECK(4u == taskList.GetTaskCount());
  DALI_TEST_CHECK(count + 1 == control.GetRendererCount());

  control.ClearRenderEffect();
  taskList = scene.GetRenderTaskList();
  DALI_TEST_CHECK(1u == taskList.GetTaskCount());
  DALI_TEST_CHECK(count == control.GetRendererCount());

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

  try
  {
    control.ClearRenderEffect();
    DALI_TEST_CHECK(false);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_CHECK(true);
  }

  END_TEST;
}

int UtcDaliRenderEffectRepeatActivateDeactivate(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectRepeatActivateDeactivate");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  scene.Add(control);

  BackgroundBlurEffect blurEffect = BackgroundBlurEffect::New();
  for(int i = 0; i < 3; i++)
  {
    control.SetRenderEffect(blurEffect); // Activate
    RenderTaskList taskList = scene.GetRenderTaskList();
    DALI_TEST_CHECK(4u == taskList.GetTaskCount());
    //control.ClearRenderEffect(); // Deactivate, Done automatically on duplicated jobs.
  }

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

  RenderEffect effect = BackgroundBlurEffect::New(0.4f, 40, 10.0f);

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
  renderer.GetProperty(renderer.GetPropertyIndex(std::string("uRadius"))).Get(radius);

  DALI_TEST_CHECK(radius.x == 30.0f);
  DALI_TEST_CHECK(radius.y == 30.0f);
  DALI_TEST_CHECK(radius.z == 30.0f);
  DALI_TEST_CHECK(radius.w == 30.0f);

  END_TEST;
}
