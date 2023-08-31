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
#include <stdlib.h>
#include <iostream>

#include <dali-scene3d/public-api/controls/model/model.h>
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>
#include <toolkit-event-thread-callback.h>

using namespace Dali;
using namespace Dali::Toolkit;

void scene_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void scene_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
/**
 * For the AnimatedCube.gltf and its Assets
 * Donated by Norbert Nopper for glTF testing.
 * Take from https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/AnimatedCube
 */
const char* TEST_GLTF_FILE_NAME = TEST_RESOURCE_DIR "/AnimatedCube.gltf";

/**
 * For the diffuse and specular cube map texture.
 * These textures are based off version of Wave engine sample
 * Take from https://github.com/WaveEngine/Samples
 *
 * Copyright (c) 2023 Wave Coorporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
const char* TEST_EQUIRECTANGULAR_TEXTURE = TEST_RESOURCE_DIR "/application-icon-20.png";
const char* TEST_DIFFUSE_TEXTURE         = TEST_RESOURCE_DIR "/forest_irradiance.ktx";
const char* TEST_SPECULAR_TEXTURE        = TEST_RESOURCE_DIR "/forest_radiance.ktx";
const char* TEST_DIFFUSE_TEXTURE2        = TEST_RESOURCE_DIR "//forest_irradiance.ktx";
const char* TEST_SPECULAR_TEXTURE2       = TEST_RESOURCE_DIR "//forest_radiance.ktx";

Dali::Texture GetDiffuseTexture(Dali::Scene3D::Model model)
{
  Dali::Texture texture;

  Actor meshActor = model.FindChildByName("AnimatedCube");
  if(meshActor)
  {
    Renderer renderer = meshActor.GetRendererAt(0u);
    if(renderer)
    {
      TextureSet textureSet = renderer.GetTextures();
      if(textureSet.GetTextureCount() == 10u)
      {
        texture = textureSet.GetTexture(8u);
      }
    }
  }

  return texture;
}

Dali::Texture GetSpecularTexture(Dali::Scene3D::Model model)
{
  Dali::Texture texture;

  Actor meshActor = model.FindChildByName("AnimatedCube");
  if(meshActor)
  {
    Renderer renderer = meshActor.GetRendererAt(0u);
    if(renderer)
    {
      TextureSet textureSet = renderer.GetTextures();
      if(textureSet.GetTextureCount() == 10u)
      {
        texture = textureSet.GetTexture(9u);
      }
    }
  }

  return texture;
}

// For ResourceReady
static bool gOnRelayoutCallBackCalled = false;
void        OnRelayoutCallback(Actor actor)
{
  gOnRelayoutCallBackCalled = true;
}

static bool gResourceReadyCalled = false;
void        OnResourceReady(Control control)
{
  gResourceReadyCalled = true;
}
} // namespace

// Negative test case for a method
int UtcDaliSceneViewUninitialized(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliSceneViewUninitialized");

  Scene3D::SceneView view;

  try
  {
    // New() must be called to create a Model or it wont be valid.
    Actor a = Actor::New();
    view.Add(a);
    DALI_TEST_CHECK(false);
  }
  catch(Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_CHECK(!view);
  }
  END_TEST;
}

// Positive test case for a method
int UtcDaliSceneViewNew(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliSceneViewNew");

  Scene3D::SceneView view = Scene3D::SceneView::New();
  DALI_TEST_CHECK(view);
  END_TEST;
}

// Positive test case for a method
int UtcDaliSceneViewDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliSceneViewDownCast");

  Scene3D::SceneView view = Scene3D::SceneView::New();
  BaseHandle         handle(view);

  Scene3D::SceneView sceneView = Scene3D::SceneView::DownCast(handle);
  DALI_TEST_CHECK(view);
  DALI_TEST_CHECK(sceneView);
  DALI_TEST_CHECK(sceneView == view);
  END_TEST;
}

int UtcDaliSceneViewTypeRegistry(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliSceneViewTypeRegistry");

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK(typeRegistry);

  TypeInfo typeInfo = typeRegistry.GetTypeInfo("SceneView");
  DALI_TEST_CHECK(typeInfo);

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK(handle);

  Scene3D::SceneView model = Scene3D::SceneView::DownCast(handle);
  DALI_TEST_CHECK(model);

  END_TEST;
}

// Positive test case for a method
int UtcDaliSceneViewAddRemove(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliSceneViewAddRemove");

  Scene3D::SceneView view = Scene3D::SceneView::New();
  DALI_TEST_CHECK(view);
  DALI_TEST_EQUALS(1u, view.GetChildCount(), TEST_LOCATION);

  Actor actor = Actor::New();

  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  view.SetProperty(Actor::Property::SIZE, application.GetScene().GetSize());
  view.Add(actor);

  DALI_TEST_EQUALS(1u, view.GetChildCount(), TEST_LOCATION);
  Actor layer = view.GetChildAt(0u);

  DALI_TEST_EQUALS(2u, layer.GetChildCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(actor, layer.GetChildAt(1u), TEST_LOCATION); // index 0u is default camera

  view.Remove(actor);
  DALI_TEST_EQUALS(1u, layer.GetChildCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliSceneViewCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  DALI_TEST_CHECK(view);

  Scene3D::SceneView copy(view);
  DALI_TEST_CHECK(view == copy);

  Scene3D::SceneView assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == view);

  END_TEST;
}

int UtcDaliSceneViewMoveConstructor(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  DALI_TEST_EQUALS(1, view.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  view.SetProperty(Actor::Property::SENSITIVE, false);
  DALI_TEST_CHECK(false == view.GetProperty<bool>(Actor::Property::SENSITIVE));

  Scene3D::SceneView moved = std::move(view);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(false == moved.GetProperty<bool>(Actor::Property::SENSITIVE));
  DALI_TEST_CHECK(!view);

  END_TEST;
}

int UtcDaliSceneViewMoveAssignment(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  DALI_TEST_EQUALS(1, view.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  view.SetProperty(Actor::Property::SENSITIVE, false);
  DALI_TEST_CHECK(false == view.GetProperty<bool>(Actor::Property::SENSITIVE));

  Scene3D::SceneView moved;
  moved = std::move(view);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(false == moved.GetProperty<bool>(Actor::Property::SENSITIVE));
  DALI_TEST_CHECK(!view);

  END_TEST;
}

int UtcDaliSceneViewOnScene01(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();

  DALI_TEST_EQUALS(1u, view.GetChildCount(), TEST_LOCATION);
  Actor layer = view.GetChildAt(0u);

  DALI_TEST_EQUALS(1u, layer.GetChildCount(), TEST_LOCATION);

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  // CameraActor is added on layer when on scene
  DALI_TEST_EQUALS(1u, layer.GetChildCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewOnScene02(void)
{
  ToolkitTestApplication application;

  uint32_t renderTaskCount = application.GetScene().GetRenderTaskList().GetTaskCount();
  DALI_TEST_EQUALS(1u, renderTaskCount, TEST_LOCATION);

  Scene3D::SceneView view = Scene3D::SceneView::New();

  renderTaskCount = application.GetScene().GetRenderTaskList().GetTaskCount();
  DALI_TEST_EQUALS(1u, renderTaskCount, TEST_LOCATION);

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  renderTaskCount = application.GetScene().GetRenderTaskList().GetTaskCount();
  DALI_TEST_EQUALS(3u, renderTaskCount, TEST_LOCATION);

  RenderTask  renderTask = application.GetScene().GetRenderTaskList().GetTask(2u);
  CameraActor camera     = renderTask.GetCameraActor();

  CameraActor defaultCamera = renderTask.GetCameraActor();
  DALI_TEST_CHECK(defaultCamera);
  DALI_TEST_EQUALS(camera, defaultCamera, TEST_LOCATION);
  DALI_TEST_EQUALS(defaultCamera, view.GetSelectedCamera(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewUserCamera(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(100, 100));

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  CameraActor defaultCamera = view.GetSelectedCamera();
  CameraActor camera        = Dali::CameraActor::New();
  camera.SetProperty(Dali::Actor::Property::NAME, "camera");
  view.AddCamera(camera);
  view.SelectCamera("camera");

  DALI_TEST_NOT_EQUALS(defaultCamera, view.GetSelectedCamera(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(camera, view.GetSelectedCamera(), TEST_LOCATION);

  camera.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  camera.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  camera.SetFieldOfView(0.5f);
  camera.SetNearClippingPlane(1.0f);
  camera.SetFarClippingPlane(5000.0f);
  camera.SetProperty(Dali::Actor::Property::POSITION, Vector3(20, 30, 40));

  float   fov            = camera[Dali::CameraActor::Property::FIELD_OF_VIEW];
  float   nearPlain      = camera[Dali::CameraActor::Property::NEAR_PLANE_DISTANCE];
  float   farPlain       = camera[Dali::CameraActor::Property::FAR_PLANE_DISTANCE];
  Vector3 cameraPosition = camera[Dali::Actor::Property::POSITION];

  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(400, 300));

  DALI_TEST_EQUALS(fov, camera.GetProperty<float>(Dali::CameraActor::Property::FIELD_OF_VIEW), TEST_LOCATION);
  DALI_TEST_EQUALS(nearPlain, camera.GetProperty<float>(Dali::CameraActor::Property::NEAR_PLANE_DISTANCE), TEST_LOCATION);
  DALI_TEST_EQUALS(farPlain, camera.GetProperty<float>(Dali::CameraActor::Property::FAR_PLANE_DISTANCE), TEST_LOCATION);
  DALI_TEST_EQUALS(cameraPosition, camera.GetProperty<Vector3>(Dali::Actor::Property::POSITION), TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewAddRemoveCamera(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(100, 100));

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  CameraActor camera0 = view.GetSelectedCamera();
  camera0.SetProperty(Dali::Actor::Property::NAME, "camera0");
  CameraActor camera1 = Dali::CameraActor::New();
  camera1.SetProperty(Dali::Actor::Property::NAME, "camera1");

  DALI_TEST_EQUALS(1u, view.GetCameraCount(), TEST_LOCATION);
  view.AddCamera(camera1);
  DALI_TEST_EQUALS(2u, view.GetCameraCount(), TEST_LOCATION);

  DALI_TEST_EQUALS(camera0, view.GetCamera(0), TEST_LOCATION);
  DALI_TEST_EQUALS(camera0, view.GetCamera("camera0"), TEST_LOCATION);
  DALI_TEST_EQUALS(camera1, view.GetCamera(1), TEST_LOCATION);
  DALI_TEST_EQUALS(camera1, view.GetCamera("camera1"), TEST_LOCATION);

  DALI_TEST_EQUALS(camera0, view.GetSelectedCamera(), TEST_LOCATION);
  view.SelectCamera(1); // 0 -> 1
  DALI_TEST_EQUALS(camera1, view.GetSelectedCamera(), TEST_LOCATION);
  view.SelectCamera("camera0"); // 1 -> 0
  DALI_TEST_EQUALS(camera0, view.GetSelectedCamera(), TEST_LOCATION);
  view.SelectCamera("camera1"); // 0 -> 1
  DALI_TEST_EQUALS(camera1, view.GetSelectedCamera(), TEST_LOCATION);
  view.SelectCamera(0); // 1 -> 0
  DALI_TEST_EQUALS(camera0, view.GetSelectedCamera(), TEST_LOCATION);

  view.SelectCamera(1); // 0 -> 1
  DALI_TEST_EQUALS(camera1, view.GetSelectedCamera(), TEST_LOCATION);
  DALI_TEST_EQUALS(2u, view.GetCameraCount(), TEST_LOCATION);
  view.RemoveCamera(camera1); // 1 -> 0
  DALI_TEST_EQUALS(camera0, view.GetSelectedCamera(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, view.GetCameraCount(), TEST_LOCATION);

  CameraActor defaultCamera = view.GetSelectedCamera();
  DALI_TEST_CHECK(defaultCamera);
  DALI_TEST_EQUALS(camera0, defaultCamera, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(camera1, defaultCamera, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSceneViewImageBasedLight01(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.ResourceReadySignal().Connect(OnResourceReady);
  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(100, 100));

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  Scene3D::Model modelView1 = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  Scene3D::Model modelView2 = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  Scene3D::Model modelView3 = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  view.Add(modelView1);
  view.Add(modelView2);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(GetDiffuseTexture(modelView1), GetDiffuseTexture(modelView2), TEST_LOCATION);
  DALI_TEST_EQUALS(GetSpecularTexture(modelView1), GetSpecularTexture(modelView2), TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetDiffuseTexture(modelView1), GetDiffuseTexture(modelView3), 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetSpecularTexture(modelView1), GetSpecularTexture(modelView3), 0.0f, TEST_LOCATION);

  gResourceReadyCalled = false;
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);
  view.SetImageBasedLightSource(TEST_DIFFUSE_TEXTURE, TEST_SPECULAR_TEXTURE);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  DALI_TEST_EQUALS(GetDiffuseTexture(modelView1), GetDiffuseTexture(modelView2), TEST_LOCATION);
  DALI_TEST_EQUALS(GetSpecularTexture(modelView1), GetSpecularTexture(modelView2), TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetDiffuseTexture(modelView1), GetDiffuseTexture(modelView3), 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetSpecularTexture(modelView1), GetSpecularTexture(modelView3), 0.0f, TEST_LOCATION);

  view.Add(modelView3);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(GetDiffuseTexture(modelView1), GetDiffuseTexture(modelView3), TEST_LOCATION);
  DALI_TEST_EQUALS(GetSpecularTexture(modelView1), GetSpecularTexture(modelView3), TEST_LOCATION);

  view.Remove(modelView1);

  gResourceReadyCalled = false;
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);
  view.SetImageBasedLightSource(TEST_DIFFUSE_TEXTURE2, TEST_SPECULAR_TEXTURE2);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  DALI_TEST_NOT_EQUALS(GetDiffuseTexture(modelView1), GetDiffuseTexture(modelView2), 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetSpecularTexture(modelView1), GetSpecularTexture(modelView2), 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetDiffuseTexture(modelView1), GetDiffuseTexture(modelView3), 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetSpecularTexture(modelView1), GetSpecularTexture(modelView3), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetDiffuseTexture(modelView2), GetDiffuseTexture(modelView3), TEST_LOCATION);
  DALI_TEST_EQUALS(GetSpecularTexture(modelView2), GetSpecularTexture(modelView3), TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewImageBasedLight02(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.ResourceReadySignal().Connect(OnResourceReady);
  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(100, 100));

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  Scene3D::Model modelView1 = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  modelView1.ResourceReadySignal().Connect(OnResourceReady);
  view.Add(modelView1);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  gResourceReadyCalled = false;
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);
  view.SetImageBasedLightSource(TEST_DIFFUSE_TEXTURE, TEST_SPECULAR_TEXTURE);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  Dali::Texture diffuseTexture  = GetDiffuseTexture(modelView1);
  Dali::Texture specularTexture = GetSpecularTexture(modelView1);

  gResourceReadyCalled = false;
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);
  modelView1.SetImageBasedLightSource(TEST_DIFFUSE_TEXTURE2, TEST_SPECULAR_TEXTURE2);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  DALI_TEST_NOT_EQUALS(GetDiffuseTexture(modelView1), diffuseTexture, 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetSpecularTexture(modelView1), specularTexture, 0.0f, TEST_LOCATION);
  diffuseTexture  = GetDiffuseTexture(modelView1);
  specularTexture = GetSpecularTexture(modelView1);

  // reset SceneView IBL
  view.SetImageBasedLightSource("", "");
  DALI_TEST_EQUALS(GetDiffuseTexture(modelView1), diffuseTexture, TEST_LOCATION);
  DALI_TEST_EQUALS(GetSpecularTexture(modelView1), specularTexture, TEST_LOCATION);

  modelView1.SetImageBasedLightSource("", "");
  DALI_TEST_NOT_EQUALS(GetDiffuseTexture(modelView1), diffuseTexture, 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetSpecularTexture(modelView1), specularTexture, 0.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewImageBasedLight03(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(100, 100));
  view.ResourceReadySignal().Connect(OnResourceReady);
  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  Scene3D::Model modelView1 = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  view.Add(modelView1);
  modelView1.SetImageBasedLightSource(TEST_SPECULAR_TEXTURE, TEST_DIFFUSE_TEXTURE);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  Dali::Texture diffuseTexture  = GetDiffuseTexture(modelView1);
  Dali::Texture specularTexture = GetSpecularTexture(modelView1);

  gResourceReadyCalled = false;
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);
  view.SetImageBasedLightSource(TEST_DIFFUSE_TEXTURE2, TEST_SPECULAR_TEXTURE2);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  DALI_TEST_EQUALS(GetDiffuseTexture(modelView1), diffuseTexture, TEST_LOCATION);
  DALI_TEST_EQUALS(GetSpecularTexture(modelView1), specularTexture, TEST_LOCATION);

  modelView1.SetImageBasedLightSource("", "");
  DALI_TEST_NOT_EQUALS(GetDiffuseTexture(modelView1), diffuseTexture, 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetSpecularTexture(modelView1), specularTexture, 0.0f, TEST_LOCATION);
  diffuseTexture  = GetDiffuseTexture(modelView1);
  specularTexture = GetSpecularTexture(modelView1);

  // reset SceneView IBL
  view.SetImageBasedLightSource("", "");
  DALI_TEST_NOT_EQUALS(GetDiffuseTexture(modelView1), diffuseTexture, 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetSpecularTexture(modelView1), specularTexture, 0.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewImageBasedFactor(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(100, 100));

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  Scene3D::Model modelView1 = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  view.Add(modelView1);

  DALI_TEST_EQUALS(view.GetImageBasedLightScaleFactor(), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(modelView1.GetImageBasedLightScaleFactor(), 1.0f, TEST_LOCATION);

  view.SetImageBasedLightScaleFactor(0.5f);
  DALI_TEST_EQUALS(view.GetImageBasedLightScaleFactor(), 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(modelView1.GetImageBasedLightScaleFactor(), 1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSceneViewUseFramebuffer01(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(100, 100));

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  bool useFramebuffer = view.IsUsingFramebuffer();
  view.UseFramebuffer(true);
  DALI_TEST_NOT_EQUALS(useFramebuffer, view.IsUsingFramebuffer(), 0.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewUseFramebuffer02(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(100, 100));

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  RenderTask renderTask = application.GetScene().GetRenderTaskList().GetTask(2u);
  DALI_TEST_CHECK(!renderTask.GetFrameBuffer());

  view.UseFramebuffer(true);
  DALI_TEST_CHECK(renderTask.GetFrameBuffer());

  view.UseFramebuffer(false);
  DALI_TEST_CHECK(!renderTask.GetFrameBuffer());

  view.UseFramebuffer(true);
  DALI_TEST_CHECK(renderTask.GetFrameBuffer());

  END_TEST;
}

int UtcDaliSceneViewFramebufferMultiSamplingLevel(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(100, 100));

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  uint8_t expectValue        = 0u; // Default MultiSamplingLevel is 0.
  uint8_t multiSamplingLevel = view.GetFramebufferMultiSamplingLevel();
  DALI_TEST_EQUALS(multiSamplingLevel, expectValue, TEST_LOCATION);

  expectValue = 4u; // Change value.
  view.UseFramebuffer(true);
  view.SetFramebufferMultiSamplingLevel(expectValue);

  multiSamplingLevel = view.GetFramebufferMultiSamplingLevel();
  DALI_TEST_EQUALS(multiSamplingLevel, expectValue, TEST_LOCATION);

  // Note : we don't check multi sampling level is applied to framebuffer, or not.
  view.UseFramebuffer(false);
  expectValue = 2u; // Change value.
  view.SetFramebufferMultiSamplingLevel(expectValue);

  application.SendNotification();
  application.Render();

  multiSamplingLevel = view.GetFramebufferMultiSamplingLevel();
  DALI_TEST_EQUALS(multiSamplingLevel, expectValue, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewResourceReady(void)
{
  ToolkitTestApplication application;

  gOnRelayoutCallBackCalled = false;
  gResourceReadyCalled      = false;
  Scene3D::SceneView view   = Scene3D::SceneView::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  view.OnRelayoutSignal().Connect(OnRelayoutCallback);
  view.ResourceReadySignal().Connect(OnResourceReady);
  // SceneView::IsResourceReady() returns true by default.
  DALI_TEST_EQUALS(view.IsResourceReady(), true, TEST_LOCATION);

  // Sanity check
  DALI_TEST_CHECK(!gOnRelayoutCallBackCalled);
  DALI_TEST_CHECK(!gResourceReadyCalled);

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gOnRelayoutCallBackCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(view.IsResourceReady(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  gOnRelayoutCallBackCalled = false;
  gResourceReadyCalled      = false;

  view.SetImageBasedLightSource(TEST_DIFFUSE_TEXTURE, TEST_SPECULAR_TEXTURE);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gOnRelayoutCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewSetSkybox(void)
{
  ToolkitTestApplication application;

  gResourceReadyCalled    = false;
  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  view.ResourceReadySignal().Connect(OnResourceReady);
  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  uint32_t childCount = view.GetChildAt(0u).GetChildCount();
  view.SetSkybox(TEST_SPECULAR_TEXTURE);

  gResourceReadyCalled = false;
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  DALI_TEST_EQUALS(view.GetChildAt(0u).GetChildCount(), childCount + 1, TEST_LOCATION);

  view.Unparent();
  view.Reset();

  END_TEST;
}

int UtcDaliSceneViewSetSkyboxEquirectangular(void)
{
  ToolkitTestApplication application;

  gResourceReadyCalled    = false;
  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  view.ResourceReadySignal().Connect(OnResourceReady);
  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  uint32_t childCount = view.GetChildAt(0u).GetChildCount();
  view.SetSkyboxEnvironmentMapType(Scene3D::EnvironmentMapType::EQUIRECTANGULAR);
  view.SetSkybox(TEST_EQUIRECTANGULAR_TEXTURE);

  gResourceReadyCalled = false;
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  DALI_TEST_EQUALS(view.GetChildAt(0u).GetChildCount(), childCount + 1, TEST_LOCATION);

  view.Unparent();
  view.Reset();

  END_TEST;
}

int UtcDaliSceneViewSetSkyboxEmpty(void)
{
  ToolkitTestApplication application;

  gResourceReadyCalled    = false;
  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  view.ResourceReadySignal().Connect(OnResourceReady);
  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  uint32_t childCount = view.GetChildAt(0u).GetChildCount();
  view.SetSkybox("");
  DALI_TEST_EQUALS(view.GetChildAt(0u).GetChildCount(), childCount, TEST_LOCATION);

  view.Unparent();
  view.Reset();

  END_TEST;
}

int UtcDaliSceneViewSetSkyboxEmpty2(void)
{
  ToolkitTestApplication application;

  gResourceReadyCalled    = false;
  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  view.ResourceReadySignal().Connect(OnResourceReady);
  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  view.SetSkybox(TEST_EQUIRECTANGULAR_TEXTURE);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  uint32_t childCount = view.GetChildAt(0u).GetChildCount();

  view.SetSkybox("");
  DALI_TEST_EQUALS(view.GetChildAt(0u).GetChildCount(), childCount - 1, TEST_LOCATION);

  view.Unparent();
  view.Reset();

  END_TEST;
}

int UtcDaliSceneViewSetSkyboxEquirectangularEmpty(void)
{
  ToolkitTestApplication application;

  gResourceReadyCalled    = false;
  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  view.ResourceReadySignal().Connect(OnResourceReady);
  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  uint32_t childCount = view.GetChildAt(0u).GetChildCount();
  view.SetSkybox("");
  DALI_TEST_EQUALS(view.GetChildAt(0u).GetChildCount(), childCount, TEST_LOCATION);

  view.Unparent();
  view.Reset();

  END_TEST;
}

int UtcDaliSceneViewSetSkyboxIntensity(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

  float intensity = 0.5f;
  DALI_TEST_EQUALS(view.GetSkyboxIntensity(), 1.0f, TEST_LOCATION);

  view.SetSkyboxIntensity(intensity);
  DALI_TEST_EQUALS(view.GetSkyboxIntensity(), intensity, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewSetSkyboxOrientation(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

  Dali::Quaternion orientation = Dali::Quaternion(Radian(0.5f), Vector3::YAXIS);
  view.SetSkyboxOrientation(orientation);
  DALI_TEST_EQUALS(view.GetSkyboxOrientation(), orientation, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewSetImageBasedLightAndSkybox(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.ResourceReadySignal().Connect(OnResourceReady);
  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(100, 100));
  view.SetImageBasedLightSource(TEST_DIFFUSE_TEXTURE, TEST_SPECULAR_TEXTURE);
  view.SetSkybox(TEST_SPECULAR_TEXTURE);
  application.GetScene().Add(view);

  // Check SceneView needs 3 trigger to load both of image based light and skybox.
  gResourceReadyCalled = false;
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewCreateAndRemoveRenderTask(void)
{
  ToolkitTestApplication application;
  RenderTaskList         taskList = application.GetScene().GetRenderTaskList();

  uint32_t renderTaskCount = taskList.GetTaskCount();

  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

  DALI_TEST_EQUALS(renderTaskCount, application.GetScene().GetRenderTaskList().GetTaskCount(), TEST_LOCATION);

  application.GetScene().Add(view);

  DALI_TEST_EQUALS(renderTaskCount + 2, application.GetScene().GetRenderTaskList().GetTaskCount(), TEST_LOCATION);

  view.Unparent();

  DALI_TEST_EQUALS(renderTaskCount, application.GetScene().GetRenderTaskList().GetTaskCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewColorMode(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  application.GetScene().Add(view);

  DALI_TEST_EQUALS(view.GetChildAt(0u).GetProperty<int>(Dali::Actor::Property::COLOR_MODE), static_cast<int>(ColorMode::USE_OWN_MULTIPLY_PARENT_ALPHA), TEST_LOCATION);

  view.UseFramebuffer(true);

  DALI_TEST_EQUALS(view.GetChildAt(0u).GetProperty<int>(Dali::Actor::Property::COLOR_MODE), static_cast<int>(ColorMode::USE_OWN_COLOR), TEST_LOCATION);

  view.UseFramebuffer(false);

  DALI_TEST_EQUALS(view.GetChildAt(0u).GetProperty<int>(Dali::Actor::Property::COLOR_MODE), static_cast<int>(ColorMode::USE_OWN_MULTIPLY_PARENT_ALPHA), TEST_LOCATION);

  END_TEST;
}
