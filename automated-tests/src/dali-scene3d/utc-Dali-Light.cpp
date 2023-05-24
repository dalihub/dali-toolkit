/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/actors/actor-devel.h>
#include <stdlib.h>
#include <iostream>

#include <dali-scene3d/public-api/controls/model/model.h>
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>
#include <dali-scene3d/public-api/light/light.h>
#include <toolkit-event-thread-callback.h>

using namespace Dali;
using namespace Dali::Toolkit;

void light_startup(void)
{
  test_return_value = TET_UNDEF;
}

void light_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* TEST_GLTF_FILE_NAME = TEST_RESOURCE_DIR "/BoxAnimated.gltf";
} // namespace

// Negative test case for a method
int UtcDaliLightUninitialized(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLightUninitialized");

  Scene3D::Light light;

  try
  {
    // New() must be called to create a Model or it wont be valid.
    Actor a = Actor::New();
    light.Add(a);
    DALI_TEST_CHECK(false);
  }
  catch(Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_CHECK(!light);
  }
  END_TEST;
}

// Positive test case for a method
int UtcDaliLightNew(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLightNew");

  Scene3D::Light light = Scene3D::Light::New();
  DALI_TEST_CHECK(light);
  END_TEST;
}

// Positive test case for a method
int UtcDaliLightDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLightDownCast");

  Scene3D::Light light = Scene3D::Light::New();
  BaseHandle     handle(light);

  Scene3D::Light light2 = Scene3D::Light::DownCast(handle);
  DALI_TEST_CHECK(light);
  DALI_TEST_CHECK(light2);
  DALI_TEST_CHECK(light == light2);
  END_TEST;
}

// Positive test case for a method
int UtcDaliLightCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  Scene3D::Light light = Scene3D::Light::New();
  DALI_TEST_CHECK(light);

  Scene3D::Light copy(light);
  DALI_TEST_CHECK(light == copy);

  Scene3D::Light assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == light);

  END_TEST;
}

int UtcDaliLightMoveConstructor(void)
{
  ToolkitTestApplication application;

  Scene3D::Light light = Scene3D::Light::New();
  DALI_TEST_EQUALS(1, light.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  light.SetProperty(Actor::Property::SENSITIVE, false);
  DALI_TEST_CHECK(false == light.GetProperty<bool>(Actor::Property::SENSITIVE));

  Scene3D::Light moved = std::move(light);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(false == moved.GetProperty<bool>(Actor::Property::SENSITIVE));
  DALI_TEST_CHECK(!light);

  END_TEST;
}

int UtcDaliLightMoveAssignment(void)
{
  ToolkitTestApplication application;

  Scene3D::Light light = Scene3D::Light::New();
  DALI_TEST_EQUALS(1, light.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  light.SetProperty(Actor::Property::SENSITIVE, false);
  DALI_TEST_CHECK(false == light.GetProperty<bool>(Actor::Property::SENSITIVE));

  Scene3D::Light moved;
  moved = std::move(light);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(false == moved.GetProperty<bool>(Actor::Property::SENSITIVE));
  DALI_TEST_CHECK(!light);

  END_TEST;
}

// For TC coverage
int UtcDaliLightSize(void)
{
  ToolkitTestApplication application;

  Scene3D::Light light = Scene3D::Light::New();
  application.GetScene().Add(light);

  application.SendNotification();
  application.Render();

  light.SetProperty(Dali::Actor::Property::SIZE, Vector3::ONE);
  DALI_TEST_EQUALS(Vector3::ONE, light.GetProperty<Vector3>(Dali::Actor::Property::SIZE), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector3::ZERO, light.GetNaturalSize(), 0.01f, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  light.SetProperty(Dali::Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::FIXED);
  DALI_TEST_EQUALS(ResizePolicy::FIXED, light.GetProperty<ResizePolicy::Type>(Dali::Actor::Property::WIDTH_RESIZE_POLICY), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  float widthForHeight = light.GetWidthForHeight(light.GetProperty<float>(Dali::Actor::Property::SIZE_HEIGHT));
  float heightForWidth = light.GetHeightForWidth(light.GetProperty<float>(Dali::Actor::Property::SIZE_WIDTH));
  DALI_TEST_EQUALS(0.0f, widthForHeight, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, heightForWidth, 0.01f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLightOnScene01(void)
{
  ToolkitTestApplication application;

  Scene3D::Light light = Scene3D::Light::New();
  application.GetScene().Add(light);

  application.SendNotification();
  application.Render();

  // Light is added on layer when on scene
  DALI_TEST_EQUALS(true, light.GetProperty<bool>(Dali::Actor::Property::CONNECTED_TO_SCENE), TEST_LOCATION);

  application.GetScene().Remove(light);

  application.SendNotification();
  application.Render();

  // Light is removed from layer when on scene
  DALI_TEST_EQUALS(false, light.GetProperty<bool>(Dali::Actor::Property::CONNECTED_TO_SCENE), TEST_LOCATION);

  END_TEST;
}

// Add a light on SceneView directly
int UtcDaliLightAdd01(void)
{
  ToolkitTestApplication application;
  Scene3D::SceneView sceneView = Scene3D::SceneView::New();
  sceneView.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  sceneView.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  sceneView.SetProperty(Dali::Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
  sceneView.SetProperty(Dali::Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
  application.GetScene().Add(sceneView);

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  sceneView.Add(model);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  Scene3D::Light light = Scene3D::Light::New();
  light.SetProperty(Dali::Actor::Property::COLOR, Color::BLUE);
  Dali::DevelActor::LookAt(light, Vector3(1.0f, 0.0f, 0.0f));
  sceneView.Add(light);

  application.SendNotification();
  application.Render();

  Renderer renderer = model.FindChildByName("node2").GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  DALI_TEST_EQUALS(1u, sceneView.GetActivatedLightCount(), TEST_LOCATION);
  auto countPropertyIndex = renderer.GetPropertyIndex("uLightCount");
  DALI_TEST_CHECK(countPropertyIndex != DALI_KEY_INVALID);
  DALI_TEST_EQUALS(1, renderer.GetProperty<int32_t>(countPropertyIndex), TEST_LOCATION);
  auto colorPropertyIndex = renderer.GetPropertyIndex("uLightColor[0]");
  DALI_TEST_EQUALS(Vector3(0.0f, 0.0f, 1.0f), renderer.GetCurrentProperty<Vector3>(colorPropertyIndex), 0.01f, TEST_LOCATION);
  auto directionPropertyIndex = renderer.GetPropertyIndex("uLightDirection[0]");
  DALI_TEST_EQUALS(Vector3(1.0f, 0.0f, 0.0f), renderer.GetCurrentProperty<Vector3>(directionPropertyIndex), 0.01f, TEST_LOCATION);

  light.Enable(false);

  DALI_TEST_EQUALS(0u, sceneView.GetActivatedLightCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, renderer.GetProperty<int32_t>(countPropertyIndex), TEST_LOCATION);

  END_TEST;
}

// Add a light on an Actor that is child of SceneView.
int UtcDaliLightAdd02(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView sceneView = Scene3D::SceneView::New();
  sceneView.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  sceneView.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  sceneView.SetProperty(Dali::Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
  sceneView.SetProperty(Dali::Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
  application.GetScene().Add(sceneView);

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  sceneView.Add(model);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  Actor actor = Actor::New();
  sceneView.Add(actor);

  Scene3D::Light light = Scene3D::Light::New();
  light.SetProperty(Dali::Actor::Property::COLOR, Color::BLUE);
  Dali::DevelActor::LookAt(light, Vector3(1.0f, 0.0f, 0.0f));
  actor.Add(light);

  application.SendNotification();
  application.Render();

  Renderer renderer = model.FindChildByName("node2").GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  DALI_TEST_EQUALS(1u, sceneView.GetActivatedLightCount(), TEST_LOCATION);
  auto countPropertyIndex = renderer.GetPropertyIndex("uLightCount");
  DALI_TEST_CHECK(countPropertyIndex != DALI_KEY_INVALID);
  DALI_TEST_EQUALS(1, renderer.GetProperty<int32_t>(countPropertyIndex), TEST_LOCATION);
  auto colorPropertyIndex = renderer.GetPropertyIndex("uLightColor[0]");
  DALI_TEST_EQUALS(Vector3(0.0f, 0.0f, 1.0f), renderer.GetCurrentProperty<Vector3>(colorPropertyIndex), 0.01f, TEST_LOCATION);
  auto directionPropertyIndex = renderer.GetPropertyIndex("uLightDirection[0]");
  DALI_TEST_EQUALS(Vector3(1.0f, 0.0f, 0.0f), renderer.GetCurrentProperty<Vector3>(directionPropertyIndex), 0.01f, TEST_LOCATION);

  light.Enable(false);

  DALI_TEST_EQUALS(0u, sceneView.GetActivatedLightCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, renderer.GetProperty<int32_t>(countPropertyIndex), TEST_LOCATION);

  END_TEST;
}

// Enable a light after it is added on SceneView.
int UtcDaliLightAdd03(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView sceneView = Scene3D::SceneView::New();
  sceneView.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  sceneView.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  sceneView.SetProperty(Dali::Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
  sceneView.SetProperty(Dali::Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
  application.GetScene().Add(sceneView);

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  sceneView.Add(model);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  Scene3D::Light light = Scene3D::Light::New();
  light.SetProperty(Dali::Actor::Property::COLOR, Color::BLUE);
  Dali::DevelActor::LookAt(light, Vector3(1.0f, 0.0f, 0.0f));
  light.Enable(false);
  sceneView.Add(light);

  application.SendNotification();
  application.Render();

  Renderer renderer = model.FindChildByName("node2").GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  DALI_TEST_EQUALS(0u, sceneView.GetActivatedLightCount(), TEST_LOCATION);
  auto countPropertyIndex = renderer.GetPropertyIndex("uLightCount");
  DALI_TEST_CHECK(countPropertyIndex != DALI_KEY_INVALID);
  DALI_TEST_EQUALS(0, renderer.GetProperty<int32_t>(countPropertyIndex), TEST_LOCATION);

  light.Enable(true);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(1u, sceneView.GetActivatedLightCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1, renderer.GetProperty<int32_t>(countPropertyIndex), TEST_LOCATION);
  auto colorPropertyIndex = renderer.GetPropertyIndex("uLightColor[0]");
  DALI_TEST_EQUALS(Vector3(0.0f, 0.0f, 1.0f), renderer.GetCurrentProperty<Vector3>(colorPropertyIndex), 0.01f, TEST_LOCATION);
  auto directionPropertyIndex = renderer.GetPropertyIndex("uLightDirection[0]");
  DALI_TEST_EQUALS(Vector3(1.0f, 0.0f, 0.0f), renderer.GetCurrentProperty<Vector3>(directionPropertyIndex), 0.01f, TEST_LOCATION);

  light.Enable(false);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(0u, sceneView.GetActivatedLightCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, renderer.GetProperty<int32_t>(countPropertyIndex), TEST_LOCATION);

  END_TEST;
}

int UtcDaliLightAdd04(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView sceneView = Scene3D::SceneView::New();
  sceneView.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  sceneView.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  sceneView.SetProperty(Dali::Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
  sceneView.SetProperty(Dali::Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
  application.GetScene().Add(sceneView);

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  sceneView.Add(model);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  Scene3D::Light light1 = Scene3D::Light::New();
  light1.SetProperty(Dali::Actor::Property::COLOR, Color::BLUE);
  Dali::DevelActor::LookAt(light1, Vector3(1.0f, 0.0f, 0.0f));
  sceneView.Add(light1);

  Scene3D::Light light2 = Scene3D::Light::New();
  light2.SetProperty(Dali::Actor::Property::COLOR, Color::RED);
  Dali::DevelActor::LookAt(light2, Vector3(0.0f, 0.0f, -1.0f));
  sceneView.Add(light2);

  application.SendNotification();
  application.Render();

  Renderer renderer = model.FindChildByName("node2").GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  DALI_TEST_EQUALS(2u, sceneView.GetActivatedLightCount(), TEST_LOCATION);
  auto countPropertyIndex = renderer.GetPropertyIndex("uLightCount");
  DALI_TEST_CHECK(countPropertyIndex != DALI_KEY_INVALID);
  DALI_TEST_EQUALS(2, renderer.GetProperty<int32_t>(countPropertyIndex), TEST_LOCATION);
  auto colorPropertyIndex1 = renderer.GetPropertyIndex("uLightColor[0]");
  DALI_TEST_EQUALS(Vector3(0.0f, 0.0f, 1.0f), renderer.GetCurrentProperty<Vector3>(colorPropertyIndex1), 0.01f, TEST_LOCATION);
  auto directionPropertyIndex1 = renderer.GetPropertyIndex("uLightDirection[0]");
  DALI_TEST_EQUALS(Vector3(1.0f, 0.0f, 0.0f), renderer.GetCurrentProperty<Vector3>(directionPropertyIndex1), 0.01f, TEST_LOCATION);
  auto colorPropertyIndex2 = renderer.GetPropertyIndex("uLightColor[1]");
  DALI_TEST_EQUALS(Vector3(1.0f, 0.0f, 0.0f), renderer.GetCurrentProperty<Vector3>(colorPropertyIndex2), 0.01f, TEST_LOCATION);
  auto directionPropertyIndex2 = renderer.GetPropertyIndex("uLightDirection[1]");
  DALI_TEST_EQUALS(Vector3(0.0f, 0.0f, -1.0f), renderer.GetCurrentProperty<Vector3>(directionPropertyIndex2), 0.01f, TEST_LOCATION);

  light1.Enable(false);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(1u, sceneView.GetActivatedLightCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1, renderer.GetProperty<int32_t>(countPropertyIndex), TEST_LOCATION);

  // After light1 is disable, shader uniforms of lights are reordered.
  DALI_TEST_EQUALS(Vector3(1.0f, 0.0f, 0.0f), renderer.GetCurrentProperty<Vector3>(colorPropertyIndex1), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector3(0.0f, 0.0f, -1.0f), renderer.GetCurrentProperty<Vector3>(directionPropertyIndex1), 0.01f, TEST_LOCATION);

  END_TEST;
}

// Check unactivated light in SceneView becomes activated when a light become disabled.
int UtcDaliLightAdd05(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView sceneView = Scene3D::SceneView::New();
  sceneView.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  sceneView.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  sceneView.SetProperty(Dali::Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
  sceneView.SetProperty(Dali::Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
  application.GetScene().Add(sceneView);

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  sceneView.Add(model);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  uint32_t                    maxLightCount = Scene3D::Light::GetMaximumEnabledLightCount();
  std::vector<Scene3D::Light> lightList;
  for(uint32_t i = 0; i < maxLightCount; ++i)
  {
    Scene3D::Light light = Scene3D::Light::New();
    light.SetProperty(Dali::Actor::Property::COLOR, Color::BLUE);
    Dali::DevelActor::LookAt(light, Vector3(1.0f, 0.0f, 0.0f));
    sceneView.Add(light);
    lightList.push_back(light);
  }

  Scene3D::Light light2 = Scene3D::Light::New();
  light2.SetProperty(Dali::Actor::Property::COLOR, Color::RED);
  Dali::DevelActor::LookAt(light2, Vector3(0.0f, 0.0f, -1.0f));
  sceneView.Add(light2);
  lightList.push_back(light2);

  application.SendNotification();
  application.Render();

  Renderer renderer = model.FindChildByName("node2").GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  std::vector<int32_t> enableAnswers;
  enableAnswers.assign(maxLightCount, 1);

  DALI_TEST_EQUALS(maxLightCount, sceneView.GetActivatedLightCount(), TEST_LOCATION);
  auto countPropertyIndex = renderer.GetPropertyIndex("uLightCount");
  DALI_TEST_CHECK(countPropertyIndex != DALI_KEY_INVALID);
  DALI_TEST_EQUALS(static_cast<int32_t>(maxLightCount), renderer.GetProperty<int32_t>(countPropertyIndex), TEST_LOCATION);
  for(uint32_t i = 0; i < maxLightCount; ++i)
  {
    std::string colorStringKey     = std::string("uLightColor[") + std::to_string(i) + "]";
    auto        colorPropertyIndex = renderer.GetPropertyIndex(colorStringKey);
    DALI_TEST_EQUALS(Vector3(0.0f, 0.0f, 1.0f), renderer.GetCurrentProperty<Vector3>(colorPropertyIndex), 0.01f, TEST_LOCATION);

    std::string directionStringKey     = std::string("uLightDirection[") + std::to_string(i) + "]";
    auto        directionPropertyIndex = renderer.GetPropertyIndex(directionStringKey);
    DALI_TEST_EQUALS(Vector3(1.0f, 0.0f, 0.0f), renderer.GetCurrentProperty<Vector3>(directionPropertyIndex), 0.01f, TEST_LOCATION);
  }

  lightList[2].Enable(false);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(maxLightCount, sceneView.GetActivatedLightCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int32_t>(maxLightCount), renderer.GetProperty<int32_t>(countPropertyIndex), TEST_LOCATION);
  for(uint32_t i = 0; i < maxLightCount; ++i)
  {
    Vector3 color, direction;
    if(i == 2)
    {
      color     = Vector3(1.0f, 0.0f, 0.0f);
      direction = Vector3(0.0f, 0.0f, -1.0f);
    }
    else
    {
      color     = Vector3(0.0f, 0.0f, 1.0f);
      direction = Vector3(1.0f, 0.0f, 0.0f);
    }
    std::string colorStringKey     = std::string("uLightColor[") + std::to_string(i) + "]";
    auto        colorPropertyIndex = renderer.GetPropertyIndex(colorStringKey);
    DALI_TEST_EQUALS(color, renderer.GetCurrentProperty<Vector3>(colorPropertyIndex), 0.01f, TEST_LOCATION);

    std::string directionStringKey     = std::string("uLightDirection[") + std::to_string(i) + "]";
    auto        directionPropertyIndex = renderer.GetPropertyIndex(directionStringKey);
    DALI_TEST_EQUALS(direction, renderer.GetCurrentProperty<Vector3>(directionPropertyIndex), 0.01f, TEST_LOCATION);
  }

  for(uint32_t i = 0; i < lightList.size(); ++i)
  {
    if(i == 2)
    {
      DALI_TEST_EQUALS(false, lightList[i].IsEnabled(), TEST_LOCATION);
    }
    else
    {
      DALI_TEST_EQUALS(true, lightList[i].IsEnabled(), TEST_LOCATION);
    }
  }

  END_TEST;
}

int UtcDaliLightModelAddAndRemove(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView sceneView = Scene3D::SceneView::New();
  sceneView.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  sceneView.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  sceneView.SetProperty(Dali::Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
  sceneView.SetProperty(Dali::Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
  application.GetScene().Add(sceneView);

  Scene3D::Light light = Scene3D::Light::New();
  light.SetProperty(Dali::Actor::Property::COLOR, Color::BLUE);
  Dali::DevelActor::LookAt(light, Vector3(1.0f, 0.0f, 0.0f));
  sceneView.Add(light);

  application.SendNotification();
  application.Render();

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  sceneView.Add(model);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  Renderer renderer = model.FindChildByName("node2").GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  DALI_TEST_EQUALS(1u, sceneView.GetActivatedLightCount(), TEST_LOCATION);
  auto countPropertyIndex = renderer.GetPropertyIndex("uLightCount");
  DALI_TEST_CHECK(countPropertyIndex != DALI_KEY_INVALID);
  DALI_TEST_EQUALS(1, renderer.GetProperty<int32_t>(countPropertyIndex), TEST_LOCATION);

  auto colorPropertyIndex = renderer.GetPropertyIndex("uLightColor[0]");
  DALI_TEST_EQUALS(Vector3(0.0f, 0.0f, 1.0f), renderer.GetCurrentProperty<Vector3>(colorPropertyIndex), 0.01f, TEST_LOCATION);
  auto directionPropertyIndex = renderer.GetPropertyIndex("uLightDirection[0]");
  DALI_TEST_EQUALS(Vector3(1.0f, 0.0f, 0.0f), renderer.GetCurrentProperty<Vector3>(directionPropertyIndex), 0.01f, TEST_LOCATION);

  DALI_TEST_EQUALS(1u, sceneView.GetActivatedLightCount(), TEST_LOCATION);

  model.Unparent();

  DALI_TEST_EQUALS(1u, sceneView.GetActivatedLightCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, renderer.GetProperty<int32_t>(countPropertyIndex), TEST_LOCATION);

  END_TEST;
}
