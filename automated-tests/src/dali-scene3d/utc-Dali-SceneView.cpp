/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <dali-scene3d/public-api/controls/model-view/model-view.h>
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>

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
 * Copyright (c) 2022 Wave Coorporation
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
const char* TEST_DIFFUSE_TEXTURE  = TEST_RESOURCE_DIR "/forest_irradiance.ktx";
const char* TEST_SPECULAR_TEXTURE = TEST_RESOURCE_DIR "/forest_radiance.ktx";

Dali::Texture GetDiffuseTexture(Dali::Scene3D::ModelView modelView)
{
  Dali::Texture texture;

  Actor meshActor = modelView.FindChildByName("AnimatedCube");
  if(meshActor)
  {
    Renderer renderer = meshActor.GetRendererAt(0u);
    if(renderer)
    {
      TextureSet textureSet = renderer.GetTextures();
      if(textureSet.GetTextureCount() == 7u)
      {
        texture = textureSet.GetTexture(5u);
      }
    }
  }

  return texture;
}

Dali::Texture GetSpecularTexture(Dali::Scene3D::ModelView modelView)
{
  Dali::Texture texture;

  Actor meshActor = modelView.FindChildByName("AnimatedCube");
  if(meshActor)
  {
    Renderer renderer = meshActor.GetRendererAt(0u);
    if(renderer)
    {
      TextureSet textureSet = renderer.GetTextures();
      if(textureSet.GetTextureCount() == 7u)
      {
        texture = textureSet.GetTexture(6u);
      }
    }
  }

  return texture;
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
    // New() must be called to create a ModelView or it wont be valid.
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

  Scene3D::SceneView modelView = Scene3D::SceneView::DownCast(handle);
  DALI_TEST_CHECK(modelView);

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
  DALI_TEST_EQUALS(2u, renderTaskCount, TEST_LOCATION);

  RenderTask  renderTask = application.GetScene().GetRenderTaskList().GetTask(1u);
  CameraActor camera     = renderTask.GetCameraActor();

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

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

int UtcDaliSceneViewImageBasedLight(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(100, 100));

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  Scene3D::ModelView modelView1 = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);
  Scene3D::ModelView modelView2 = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);
  Scene3D::ModelView modelView3 = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);
  view.Add(modelView1);
  view.Add(modelView2);

  DALI_TEST_NOT_EQUALS(GetDiffuseTexture(modelView1), GetDiffuseTexture(modelView2), 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetSpecularTexture(modelView1), GetSpecularTexture(modelView2), 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetDiffuseTexture(modelView1), GetDiffuseTexture(modelView3), 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetSpecularTexture(modelView1), GetSpecularTexture(modelView3), 0.0f, TEST_LOCATION);

  view.SetImageBasedLightSource(TEST_DIFFUSE_TEXTURE, TEST_SPECULAR_TEXTURE);

  DALI_TEST_EQUALS(GetDiffuseTexture(modelView1), GetDiffuseTexture(modelView2), TEST_LOCATION);
  DALI_TEST_EQUALS(GetSpecularTexture(modelView1), GetSpecularTexture(modelView2), TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetDiffuseTexture(modelView1), GetDiffuseTexture(modelView3), 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetSpecularTexture(modelView1), GetSpecularTexture(modelView3), 0.0f, TEST_LOCATION);

  view.Add(modelView3);

  DALI_TEST_EQUALS(GetDiffuseTexture(modelView1), GetDiffuseTexture(modelView3), TEST_LOCATION);
  DALI_TEST_EQUALS(GetSpecularTexture(modelView1), GetSpecularTexture(modelView3), TEST_LOCATION);

  view.Remove(modelView1);
  view.SetImageBasedLightSource(TEST_DIFFUSE_TEXTURE, TEST_SPECULAR_TEXTURE);

  DALI_TEST_NOT_EQUALS(GetDiffuseTexture(modelView1), GetDiffuseTexture(modelView2), 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetSpecularTexture(modelView1), GetSpecularTexture(modelView2), 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetDiffuseTexture(modelView1), GetDiffuseTexture(modelView3), 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(GetSpecularTexture(modelView1), GetSpecularTexture(modelView3), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetDiffuseTexture(modelView2), GetDiffuseTexture(modelView3), TEST_LOCATION);
  DALI_TEST_EQUALS(GetSpecularTexture(modelView2), GetSpecularTexture(modelView3), TEST_LOCATION);

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

  RenderTask renderTask = application.GetScene().GetRenderTaskList().GetTask(1u);
  DALI_TEST_CHECK(!renderTask.GetFrameBuffer());

  view.UseFramebuffer(true);
  DALI_TEST_CHECK(renderTask.GetFrameBuffer());

  view.UseFramebuffer(false);
  DALI_TEST_CHECK(!renderTask.GetFrameBuffer());

  END_TEST;
}
