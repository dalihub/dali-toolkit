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

#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>

using namespace Dali;
using namespace Dali::Toolkit;

int UtcDaliRenderEffectNewP(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectNewP");

  RenderEffect blurEffect = BackgroundBlurEffect::New();
  DALI_TEST_CHECK(blurEffect);

  RenderEffect blurEffect2 = BackgroundBlurEffect::New(10);
  DALI_TEST_CHECK(blurEffect2);

  Control control = Control::New();

  RenderEffect maskEffect1 = MaskEffect::New(control);
  DALI_TEST_CHECK(maskEffect1);

  RenderEffect maskEffect2 = MaskEffect::New(control, MaskEffect::MaskMode::LUMINANCE, Vector2(0.f, 0.f), Vector2(1.f, 1.f));
  DALI_TEST_CHECK(maskEffect2);

  MaskEffect maskEffect3 = MaskEffect::New(control);
  DALI_TEST_CHECK(maskEffect3);

  MaskEffect maskEffect4 = maskEffect3;
  DALI_TEST_CHECK(maskEffect4);

  END_TEST;
}

int UtcDaliRenderEffectNewN(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectNewN");

  tet_printf("Check some invalid parameters clamp internally\n");

  RenderEffect blurEffect1 = BackgroundBlurEffect::New(10);
  RenderEffect blurEffect2 = BackgroundBlurEffect::New(0);
  RenderEffect blurEffect3 = BackgroundBlurEffect::New(2147483647);

  DALI_TEST_CHECK(blurEffect1);
  DALI_TEST_CHECK(blurEffect2);
  DALI_TEST_CHECK(blurEffect3);

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

  Control newControl = Control::New();
  childControl.SetRenderEffect(MaskEffect::New(newControl));

  taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(3u, taskList.GetTaskCount(), TEST_LOCATION);

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

  RenderEffect blurEffect = BackgroundBlurEffect::New();
  control.SetRenderEffect(blurEffect);

  RenderTaskList taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, blurEffect.IsActivated(), TEST_LOCATION);

  Control control2 = Control::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control2.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  scene.Add(control2);

  control2.SetRenderEffect(blurEffect);
  taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectActivateP03(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectActivateP03");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  scene.Add(control);

  Control      newControl = Control::New();
  RenderEffect maskEffect = MaskEffect::New(newControl);
  control.SetRenderEffect(maskEffect);

  RenderTaskList taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(3u, taskList.GetTaskCount(), TEST_LOCATION);

  Control control2 = Control::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control2.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  scene.Add(control2);

  control2.SetRenderEffect(maskEffect);
  taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(3u, taskList.GetTaskCount(), TEST_LOCATION);

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

  uint32_t             count      = control.GetRendererCount();
  BackgroundBlurEffect blurEffect = BackgroundBlurEffect::New();
  control.SetRenderEffect(blurEffect);

  RenderTaskList taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, control.GetRendererCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, blurEffect.IsActivated(), TEST_LOCATION);

  control.ClearRenderEffect();
  taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, blurEffect.IsActivated(), TEST_LOCATION);

  Control newControl = Control::New();
  count              = control.GetRendererCount();
  control.SetRenderEffect(MaskEffect::New(newControl));

  taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(3u, taskList.GetTaskCount(), TEST_LOCATION);

  control.ClearRenderEffect();
  taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);

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

  RenderEffect blurEffect = BackgroundBlurEffect::New();
  control.SetRenderEffect(blurEffect);

  RenderTaskList taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);

  blurEffect.Deactivate();
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);

  blurEffect.Activate();
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

  RenderEffect blurEffect = BackgroundBlurEffect::New();
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

  RenderTaskList taskList = scene.GetRenderTaskList();
  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  uint32_t count = control.GetRendererCount();
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  ////////////////////////////////////////////
  tet_infoline("resize test on BackgroundBlurEffect");
  control.SetRenderEffect(BackgroundBlurEffect::New());

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);
  tet_infoline("size zero owner control's effect is not activated.");

  control.SetProperty(Actor::Property::SIZE, Vector2(30.0f, 30.0f));

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, control.GetRendererCount(), TEST_LOCATION);

  DALI_TEST_EQUALS(control.GetProperty<float>(Actor::Property::SIZE_WIDTH), 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 30.0f, TEST_LOCATION);
  tet_infoline("Background blur effect activated.\n");

  control.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(control.GetProperty<float>(Actor::Property::SIZE_WIDTH), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 10.0f, TEST_LOCATION);
  tet_infoline("Background blur effect refreshed.\n");

  control.SetProperty(Actor::Property::SIZE, Vector2(0.0f, 0.0f));

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);

  DALI_TEST_EQUALS(control.GetProperty<float>(Actor::Property::SIZE_WIDTH), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 0.0f, TEST_LOCATION);
  tet_infoline("Background blur effect deactivated.\n");
  /////////////////////////////////////////////
  tet_infoline("resize test on GaussianBlurEffect");
  GaussianBlurEffect effect = GaussianBlurEffect::New(20u);
  control.SetRenderEffect(effect);

  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION);
  tet_infoline("size zero owner control's effect is not activated.");

  control.SetProperty(Actor::Property::SIZE, Vector2(30.0f, 30.0f));

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(count, control.GetRendererCount(), TEST_LOCATION); // Uses cache renderer

  DALI_TEST_EQUALS(control.GetProperty<float>(Actor::Property::SIZE_WIDTH), 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 30.0f, TEST_LOCATION);
  tet_infoline("Blur effect activated.\n");
  DALI_TEST_EQUALS(effect.GetBlurRadius(), 20u, TEST_LOCATION);

  control.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(control.GetProperty<float>(Actor::Property::SIZE_WIDTH), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 10.0f, TEST_LOCATION);
  tet_infoline("Blur effect refreshed.\n");

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
  RenderEffect effect = BackgroundBlurEffect::New(40);

  Control control = Control::New();
  DALI_TEST_CHECK(control.GetRendererCount() == 0u);
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  control.SetProperty(Toolkit::DevelControl::Property::CORNER_RADIUS, Vector4(30.0f, 30.0f, 30.0f, 30.0f));
  control.SetProperty(Toolkit::DevelControl::Property::CORNER_RADIUS_POLICY, Visual::Transform::Policy::Type::ABSOLUTE);
  control.SetProperty(Toolkit::DevelControl::Property::CORNER_SQUARENESS, Vector4(0.7f, 0.7f, 0.7f, 0.7f));
  scene.Add(control);

  control.SetProperty(Toolkit::Control::Property::BACKGROUND, blackDimmerMap);
  DALI_TEST_CHECK(control.GetRendererCount() == 1u);
  control.SetRenderEffect(effect);
  DALI_TEST_CHECK(control.GetRendererCount() == 2u);

  Renderer renderer = control.GetRendererAt(1u);
  Vector4  radius   = Vector4::ZERO;
  renderer.GetProperty(renderer.GetPropertyIndex(std::string("uCornerRadius"))).Get(radius);

  Vector4 squreness = Vector4::ZERO;
  renderer.GetProperty(renderer.GetPropertyIndex(std::string("uCornerSquareness"))).Get(squreness);

  Toolkit::Visual::Transform::Policy::Type policy;
  renderer.GetProperty(renderer.GetPropertyIndex(std::string("uCornerRadiusPolicy"))).Get(policy);
  DALI_TEST_CHECK(policy == Visual::Transform::Policy::Type::ABSOLUTE);

  DALI_TEST_CHECK(radius.x == 30.0f);
  DALI_TEST_CHECK(radius.y == 30.0f);
  DALI_TEST_CHECK(radius.z == 30.0f);
  DALI_TEST_CHECK(radius.w == 30.0f);

  DALI_TEST_CHECK(squreness.x == 0.7f);
  DALI_TEST_CHECK(squreness.y == 0.7f);
  DALI_TEST_CHECK(squreness.z == 0.7f);
  DALI_TEST_CHECK(squreness.w == 0.7f);

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
  control.SetRenderEffect(BackgroundBlurEffect::New(40));

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
  control1.SetRenderEffect(BackgroundBlurEffect::New());

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
  control2.SetRenderEffect(BackgroundBlurEffect::New());

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
  control.SetRenderEffect(BackgroundBlurEffect::New());

  application.SendNotification();

  RenderTaskList taskList = scene.GetRenderTaskList();

  // Render effect activated.
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  tet_printf("order : %d\n", taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex());
  DALI_TEST_EQUALS(INT32_MIN + 2, taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex(), TEST_LOCATION);

  BackgroundBlurEffect effect = BackgroundBlurEffect::New(40u);
  control.SetRenderEffect(effect);
  DALI_TEST_EQUALS(effect.GetBlurRadius(), 40u, TEST_LOCATION);

  application.SendNotification();

  // Render effect activated.
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  tet_printf("order : %d\n", taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex());
  DALI_TEST_EQUALS(INT32_MIN + 2, taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex(), TEST_LOCATION);

  effect.SetBlurRadius(20u);
  application.SendNotification();

  // Render effect activated.
  DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  tet_printf("order : %d\n", taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex());
  DALI_TEST_EQUALS(INT32_MIN + 2, taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(effect.GetBlurRadius(), 20u, TEST_LOCATION);

  effect.SetBlurRadius(2u); // invalid blur radius value(too small)
  application.SendNotification();

  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(effect.GetBlurRadius(), 2u, TEST_LOCATION);

  control.SetRenderEffect(GaussianBlurEffect::New(2u)); // invalid blur radius value(too small)
  application.SendNotification();

  DALI_TEST_EQUALS(1u, taskList.GetTaskCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(effect.GetBlurRadius(), 2u, TEST_LOCATION);

  END_TEST;
}

namespace
{
void BlurRenderingFinishedCallback(void)
{
  tet_infoline("blur rendering finished signal emitted");
}
} // namespace

int UtcDaliBlurEffectBlurOnce(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliBlurEffectBlurOnce");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  scene.Add(control);

  {
    // Add render effect during scene on.
    BackgroundBlurEffect effect = BackgroundBlurEffect::New(20u);
    DALI_TEST_EQUALS(effect.GetBlurOnce(), false, TEST_LOCATION);

    effect.SetBlurOnce(true);
    effect.FinishedSignal().Connect(&application, &BlurRenderingFinishedCallback);
    control.SetRenderEffect(effect);
    DALI_TEST_EQUALS(effect.GetBlurOnce(), true, TEST_LOCATION);

    application.SendNotification();

    RenderTaskList taskList = scene.GetRenderTaskList();

    // Render effect activated.
    DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
    tet_printf("order : %d\n", taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex());
    DALI_TEST_EQUALS(INT32_MIN + 2, taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex(), TEST_LOCATION);

    effect.SetBlurOnce(false);
    DALI_TEST_EQUALS(effect.GetBlurOnce(), false, TEST_LOCATION);

    DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
    tet_printf("order : %d\n", taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex());
    DALI_TEST_EQUALS(INT32_MIN + 2, taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex(), TEST_LOCATION);
  }
  {
    // Add render effect during scene on.
    GaussianBlurEffect effect = GaussianBlurEffect::New(20u);
    DALI_TEST_EQUALS(effect.GetBlurOnce(), false, TEST_LOCATION);

    effect.SetBlurOnce(true);
    effect.FinishedSignal().Connect(&application, &BlurRenderingFinishedCallback);
    control.SetRenderEffect(effect);
    DALI_TEST_EQUALS(effect.GetBlurOnce(), true, TEST_LOCATION);

    application.SendNotification();

    RenderTaskList taskList = scene.GetRenderTaskList();

    // Render effect activated.
    DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);

    effect.SetBlurOnce(false);
    DALI_TEST_EQUALS(effect.GetBlurOnce(), false, TEST_LOCATION);

    DALI_TEST_EQUALS(4u, taskList.GetTaskCount(), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliBlurEffectDownscaleFactor(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliBlurEffectDownscaleFactor");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  scene.Add(control);

  {
    tet_printf("test BackgroundBlurEffect\n");
    BackgroundBlurEffect effect = BackgroundBlurEffect::New(200);
    control.SetRenderEffect(effect);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(effect.GetBlurDownscaleFactor(), 0.25f, TEST_LOCATION); // Default

    effect.Deactivate();
    effect.SetBlurDownscaleFactor(0.16f); // update while deactivated
    effect.Activate();
    DALI_TEST_EQUALS(effect.GetBlurDownscaleFactor(), 0.16f, TEST_LOCATION);
    DALI_TEST_EQUALS(effect.IsActivated(), true, TEST_LOCATION);

    effect.SetBlurDownscaleFactor(0.5f); // update while activated
    DALI_TEST_EQUALS(effect.GetBlurDownscaleFactor(), 0.5f, TEST_LOCATION);

    effect.Refresh();
    DALI_TEST_EQUALS(effect.IsActivated(), true, TEST_LOCATION);

    effect.Deactivate();
    effect.Refresh();
    DALI_TEST_EQUALS(effect.IsActivated(), true, TEST_LOCATION);
  }
  {
    tet_printf("test GaussianBlurEffect\n");
    GaussianBlurEffect effect = GaussianBlurEffect::New(200);
    control.SetRenderEffect(effect);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(effect.GetBlurDownscaleFactor(), 0.25f, TEST_LOCATION); // Default

    effect.Deactivate();
    effect.SetBlurDownscaleFactor(0.16f); // update while deactivated
    effect.Activate();
    DALI_TEST_EQUALS(effect.GetBlurDownscaleFactor(), 0.16f, TEST_LOCATION);

    effect.SetBlurDownscaleFactor(0.5f); // update while activated
    DALI_TEST_EQUALS(effect.GetBlurDownscaleFactor(), 0.5f, TEST_LOCATION);

    effect.SetBlurRadius(30);
  }

  END_TEST;
}

int UtcDaliMaskEffect(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliMaskEffect");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  scene.Add(control);

  Control maskControl = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  // Add render effect during scene on.
  control.SetRenderEffect(MaskEffect::New(maskControl));

  // send notification.
  application.SendNotification();
  application.Render();

  RenderTaskList taskList = scene.GetRenderTaskList();

  control.SetProperty(Actor::Property::SIZE, Vector2(3.0f, 3.0f));

  // send notification twice to refresh.
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // Render effect activated.
  DALI_TEST_EQUALS(3u, taskList.GetTaskCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectBlurStrengthAnimation(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectBlurStrengthAnimation");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  {
    BackgroundBlurEffect effect = BackgroundBlurEffect::New();
    control.SetRenderEffect(effect);
    scene.Add(control);

    float     durationSeconds = 0.05f;
    Animation animation       = Animation::New(durationSeconds);

    effect.AddBlurStrengthAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    animation.Play();
    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds * 1000.0f) + 1u /*just beyond the animation duration*/);
    animation.Clear();
    DALI_TEST_EQUALS(effect.GetBlurOnce(), false, TEST_LOCATION);

    effect.SetBlurOnce(true);
    effect.AddBlurStrengthAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    // animation will not be added but cannot check
    animation.Clear();
    DALI_TEST_EQUALS(effect.GetBlurOnce(), true, TEST_LOCATION);

    effect.SetBlurRadius(2u);
    effect.AddBlurStrengthAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    // animation will not be added but cannot check
    animation.Clear();
  }

  {
    GaussianBlurEffect effect = GaussianBlurEffect::New();
    control.SetRenderEffect(effect);
    scene.Add(control);

    float     durationSeconds = 0.05f;
    Animation animation       = Animation::New(durationSeconds);

    effect.AddBlurStrengthAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    animation.Play();
    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds * 1000.0f) + 1u /*just beyond the animation duration*/);
    animation.Clear();
    DALI_TEST_EQUALS(effect.GetBlurOnce(), false, TEST_LOCATION);

    effect.SetBlurOnce(true);
    effect.AddBlurStrengthAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    // animation will not be added but cannot check
    animation.Clear();
    DALI_TEST_EQUALS(effect.GetBlurOnce(), true, TEST_LOCATION);

    effect.SetBlurRadius(2u);
    effect.AddBlurStrengthAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    // animation will not be added but cannot check
    animation.Clear();
  }

  END_TEST;
}

int UtcDaliMaskEffectScaleN(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliMaskEffect");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  scene.Add(control);

  Control maskControl = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  // adjust to epsilon.
  control.SetRenderEffect(MaskEffect::New(maskControl, MaskEffect::MaskMode::ALPHA, Vector2(0.f, 0.f), Vector2(0.f, 0.f)));

  // send notification.
  application.SendNotification();
  application.Render();

  RenderTaskList taskList = scene.GetRenderTaskList();

  // Render effect activated.
  DALI_TEST_EQUALS(3u, taskList.GetTaskCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectBlurOpacityAnimation(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRenderEffectBlurOpacityAnimation");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  {
    BackgroundBlurEffect effect = BackgroundBlurEffect::New();
    control.SetRenderEffect(effect);
    scene.Add(control);

    float     durationSeconds = 0.05f;
    Animation animation       = Animation::New(durationSeconds);

    effect.AddBlurOpacityAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    animation.Play();
    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds * 1000.0f) + 1u /*just beyond the animation duration*/);
    DALI_TEST_EQUALS(effect.GetBlurOnce(), false, TEST_LOCATION);

    effect.SetBlurOnce(true);
    effect.AddBlurOpacityAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    // animation will not be added but cannot check
    animation.Clear();
    DALI_TEST_EQUALS(effect.GetBlurOnce(), true, TEST_LOCATION);

    effect.SetBlurRadius(2u);
    effect.AddBlurOpacityAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    // animation will not be added but cannot check
    animation.Clear();
  }
  {
    Toolkit::GaussianBlurEffect effect = Toolkit::GaussianBlurEffect::New();
    control.SetRenderEffect(effect);
    scene.Add(control);

    float     durationSeconds = 0.05f;
    Animation animation       = Animation::New(durationSeconds);

    effect.AddBlurOpacityAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    animation.Play();
    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds * 1000.0f) + 1u /*just beyond the animation duration*/);
    DALI_TEST_EQUALS(effect.GetBlurOnce(), false, TEST_LOCATION);

    effect.SetBlurOnce(true);
    effect.AddBlurOpacityAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    // animation will not be added but cannot check
    animation.Clear();
    DALI_TEST_EQUALS(effect.GetBlurOnce(), true, TEST_LOCATION);

    effect.SetBlurRadius(2u);
    effect.AddBlurOpacityAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    // animation will not be added but cannot check
    animation.Clear();
  }

  END_TEST;
}

int UtcDaliMaskEffectMaskOnce(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliMaskEffectMaskOnce");

  Integration::Scene scene = application.GetScene();

  Control control = Control::New();
  control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  scene.Add(control);

  {
    // Add mask effect before activated.
    Control maskControl = Control::New();
    maskControl.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    maskControl.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

    scene.Add(maskControl);

    MaskEffect maskEffect = MaskEffect::New(maskControl);
    maskEffect.SetTargetMaskOnce(true);
    maskEffect.SetSourceMaskOnce(true);

    control.SetRenderEffect(maskEffect);

    // send notification.
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(maskEffect.GetTargetMaskOnce(), true, TEST_LOCATION);
    DALI_TEST_EQUALS(maskEffect.GetSourceMaskOnce(), true, TEST_LOCATION);

    control.ClearRenderEffect();
    scene.Remove(maskControl);
  }
  {
    // Add mask effect during activate.
    Control maskControl = Control::New();
    maskControl.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    maskControl.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

    scene.Add(maskControl);

    MaskEffect maskEffect = MaskEffect::New(maskControl);
    maskEffect.SetTargetMaskOnce(true);
    maskEffect.SetSourceMaskOnce(true);

    control.SetRenderEffect(maskEffect);

    application.SendNotification();
    application.Render();

    maskEffect.SetTargetMaskOnce(false);
    maskEffect.SetSourceMaskOnce(false);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(maskEffect.GetTargetMaskOnce(), false, TEST_LOCATION);
    DALI_TEST_EQUALS(maskEffect.GetSourceMaskOnce(), false, TEST_LOCATION);

    maskEffect.SetTargetMaskOnce(true);
    maskEffect.SetSourceMaskOnce(true);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(maskEffect.GetTargetMaskOnce(), true, TEST_LOCATION);
    DALI_TEST_EQUALS(maskEffect.GetSourceMaskOnce(), true, TEST_LOCATION);

    control.ClearRenderEffect();
    scene.Remove(maskControl);
  }

  END_TEST;
}
