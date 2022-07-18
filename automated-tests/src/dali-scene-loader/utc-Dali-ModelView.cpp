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

#include <dali-scene-loader/public-api/controls/model-view/model-view.h>

using namespace Dali;
using namespace Dali::Toolkit;

void model_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void model_view_cleanup(void)
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
const char* TEST_GLTF_FILE_NAME                = TEST_RESOURCE_DIR "/AnimatedCube.gltf";
const char* TEST_GLTF_TRIANGLE_FILE_NAME       = TEST_RESOURCE_DIR "/AnimatedTriangle.gltf";
const char* TEST_GLTF_ANIMATION_TEST_FILE_NAME = TEST_RESOURCE_DIR "/animationTest.gltf";
const char* TEST_DLI_FILE_NAME                 = TEST_RESOURCE_DIR "/arc.dli";
const char* TEST_DLI_BEER_FILE_NAME            = TEST_RESOURCE_DIR "/beer_modelViewTest.dli";
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
} // namespace

// Negative test case for a method
int UtcDaliModelViewUninitialized(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelViewUninitialized");

  Scene3D::ModelView view;

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
int UtcDaliModelViewNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelViewNew");

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);
  DALI_TEST_CHECK(view);
  END_TEST;
}

// Positive test case for a method
int UtcDaliModelViewDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelViewDownCast");

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);
  BaseHandle         handle(view);

  Scene3D::ModelView modelView = Scene3D::ModelView::DownCast(handle);
  DALI_TEST_CHECK(view);
  DALI_TEST_CHECK(modelView);
  DALI_TEST_CHECK(modelView == view);
  END_TEST;
}

int UtcDaliModelViewTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK(typeRegistry);

  TypeInfo typeInfo = typeRegistry.GetTypeInfo("ModelView");
  DALI_TEST_CHECK(typeInfo);

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK(handle);

  Scene3D::ModelView modelView = Scene3D::ModelView::DownCast(handle);
  DALI_TEST_CHECK(modelView);

  END_TEST;
}

// Positive test case for a method
int UtcDaliModelViewAddRemove(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelViewAddRemove");

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);
  DALI_TEST_CHECK(view);

  Actor actor = Actor::New();
  DALI_TEST_CHECK(!actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  view.SetProperty(Actor::Property::SIZE, application.GetScene().GetSize());
  view.Add(actor);
  application.GetScene().Add(view);

  DALI_TEST_CHECK(actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  view.Remove(actor);

  DALI_TEST_CHECK(!actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));
  END_TEST;
}

int UtcDaliModelViewCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);
  DALI_TEST_CHECK(view);

  Scene3D::ModelView copy(view);
  DALI_TEST_CHECK(view == copy);

  Scene3D::ModelView assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == view);

  END_TEST;
}

int UtcDaliModelViewMoveConstructor(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);
  DALI_TEST_EQUALS(1, view.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  view.SetProperty(Actor::Property::SENSITIVE, false);
  DALI_TEST_CHECK(false == view.GetProperty<bool>(Actor::Property::SENSITIVE));

  Scene3D::ModelView moved = std::move(view);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(false == moved.GetProperty<bool>(Actor::Property::SENSITIVE));
  DALI_TEST_CHECK(!view);

  END_TEST;
}

int UtcDaliModelViewMoveAssignment(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);
  DALI_TEST_EQUALS(1, view.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  view.SetProperty(Actor::Property::SENSITIVE, false);
  DALI_TEST_CHECK(false == view.GetProperty<bool>(Actor::Property::SENSITIVE));

  Scene3D::ModelView moved;
  moved = std::move(view);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(false == moved.GetProperty<bool>(Actor::Property::SENSITIVE));
  DALI_TEST_CHECK(!view);

  END_TEST;
}

int UtcDaliModelViewOnScene01(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  uint32_t modelCount = view.GetModelRoot().GetChildCount();
  DALI_TEST_EQUALS(1, modelCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelViewOnScene02(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_DLI_FILE_NAME);

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  uint32_t modelCount = view.GetModelRoot().GetChildCount();
  DALI_TEST_EQUALS(1, modelCount, TEST_LOCATION);

  Actor   rootActor = view.GetModelRoot();
  Vector3 rootSize  = rootActor.GetProperty<Vector3>(Dali::Actor::Property::SIZE);
  DALI_TEST_EQUALS(Vector3(2, 2, 1), rootSize, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelViewOnSizeSet(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  Vector2 size(200.0f, 300.0f);
  view.SetProperty(Actor::Property::SIZE, size);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(view.GetCurrentProperty<Vector2>(Actor::Property::SIZE), size, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelViewGetNaturalSize(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);

  Vector3 naturalSize = view.GetNaturalSize();

  DALI_TEST_EQUALS(Vector3(2, 2, 2), naturalSize, TEST_LOCATION);

  Actor root = view.GetModelRoot();
  DALI_TEST_CHECK(root);

  END_TEST;
}

int UtcDaliModelViewSetImageBasedLightSource01(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  Actor meshActor = view.FindChildByName("AnimatedCube");
  DALI_TEST_CHECK(meshActor);

  Renderer renderer = meshActor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  TextureSet textureSet = renderer.GetTextures();
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 7u, TEST_LOCATION);

  Texture diffuseTexture  = textureSet.GetTexture(5u);
  Texture specularTexture = textureSet.GetTexture(6u);

  view.SetImageBasedLightSource(TEST_DIFFUSE_TEXTURE, TEST_SPECULAR_TEXTURE);

  Texture newDiffuseTexture  = textureSet.GetTexture(5u);
  Texture newSpecularTexture = textureSet.GetTexture(6u);

  DALI_TEST_NOT_EQUALS(diffuseTexture, newDiffuseTexture, 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(specularTexture, newSpecularTexture, 0.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelViewSetImageBasedLightSource02(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  Actor meshActor = view.FindChildByName("AnimatedCube");
  DALI_TEST_CHECK(meshActor);

  Renderer renderer = meshActor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  TextureSet textureSet = renderer.GetTextures();
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 7u, TEST_LOCATION);

  Texture diffuseTexture  = textureSet.GetTexture(5u);
  Texture specularTexture = textureSet.GetTexture(6u);

  view.SetImageBasedLightSource("", "");

  Texture newDiffuseTexture  = textureSet.GetTexture(5u);
  Texture newSpecularTexture = textureSet.GetTexture(6u);

  DALI_TEST_EQUALS(diffuseTexture, newDiffuseTexture, TEST_LOCATION);
  DALI_TEST_EQUALS(specularTexture, newSpecularTexture, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelViewSetImageBasedLightSource03(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  Actor meshActor = view.FindChildByName("AnimatedCube");
  DALI_TEST_CHECK(meshActor);

  Renderer renderer = meshActor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  TextureSet textureSet = renderer.GetTextures();
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 7u, TEST_LOCATION);

  Texture diffuseTexture  = textureSet.GetTexture(5u);
  Texture specularTexture = textureSet.GetTexture(6u);

  view.SetImageBasedLightSource("dummy.ktx", "dummy.ktx");

  Texture newDiffuseTexture  = textureSet.GetTexture(5u);
  Texture newSpecularTexture = textureSet.GetTexture(6u);

  DALI_TEST_EQUALS(diffuseTexture, newDiffuseTexture, TEST_LOCATION);
  DALI_TEST_EQUALS(specularTexture, newSpecularTexture, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelViewFitSize01(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);
  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(50, 50));

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  Actor   rootActor = view.GetModelRoot();
  Vector3 rootSize  = rootActor.GetProperty<Vector3>(Dali::Actor::Property::SIZE);
  DALI_TEST_EQUALS(Vector3(2, 2, 2), rootSize, TEST_LOCATION);

  Vector3 rootScale = rootActor.GetProperty<Vector3>(Dali::Actor::Property::SCALE);
  DALI_TEST_EQUALS(Vector3(1, 1, 1), rootScale, TEST_LOCATION);

  view.FitSize(true);
  rootSize = rootActor.GetProperty<Vector3>(Dali::Actor::Property::SIZE);
  DALI_TEST_EQUALS(Vector3(2, 2, 2), rootSize, TEST_LOCATION);

  rootScale = rootActor.GetProperty<Vector3>(Dali::Actor::Property::SCALE);
  DALI_TEST_EQUALS(Vector3(25, 25, 25), rootScale, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelViewFitSize02(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_DLI_BEER_FILE_NAME);
  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(50, 50));

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  Actor   rootActor = view.GetModelRoot();
  Vector3 rootSize  = rootActor.GetProperty<Vector3>(Dali::Actor::Property::SIZE);
  DALI_TEST_EQUALS(Vector3(5, 5, 5), rootSize, TEST_LOCATION);

  Vector3 rootScale = rootActor.GetProperty<Vector3>(Dali::Actor::Property::SCALE);
  DALI_TEST_EQUALS(Vector3(1, 1, 1), rootScale, TEST_LOCATION);

  view.FitSize(true);
  rootSize = rootActor.GetProperty<Vector3>(Dali::Actor::Property::SIZE);
  DALI_TEST_EQUALS(Vector3(5, 5, 5), rootSize, TEST_LOCATION);

  rootScale = rootActor.GetProperty<Vector3>(Dali::Actor::Property::SCALE);
  DALI_TEST_EQUALS(Vector3(10, 10, 10), rootScale, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelViewFitSize03(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);
  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(0, 0));

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  Actor   rootActor = view.GetModelRoot();
  Vector3 rootSize  = rootActor.GetProperty<Vector3>(Dali::Actor::Property::SIZE);
  DALI_TEST_EQUALS(Vector3(2, 2, 2), rootSize, TEST_LOCATION);

  Vector3 rootScale = rootActor.GetProperty<Vector3>(Dali::Actor::Property::SCALE);
  DALI_TEST_EQUALS(Vector3(1, 1, 1), rootScale, TEST_LOCATION);

  view.FitSize(true);
  rootSize = rootActor.GetProperty<Vector3>(Dali::Actor::Property::SIZE);
  DALI_TEST_EQUALS(Vector3(2, 2, 2), rootSize, TEST_LOCATION);

  rootScale = rootActor.GetProperty<Vector3>(Dali::Actor::Property::SCALE);
  DALI_TEST_EQUALS(Vector3(1, 1, 1), rootScale, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelViewFitCenter(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_GLTF_TRIANGLE_FILE_NAME);
  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(50, 50));

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  Vector3 naturalSize = view.GetNaturalSize();
  DALI_TEST_EQUALS(Vector3(1, 1, 0), naturalSize, TEST_LOCATION);

  Actor   rootActor   = view.GetModelRoot();
  Vector3 anchorPoint = rootActor.GetProperty<Vector3>(Dali::Actor::Property::ANCHOR_POINT);
  DALI_TEST_EQUALS(Vector3(0.5, 0.5, 0.5), anchorPoint, TEST_LOCATION);

  view.FitCenter(true);
  anchorPoint = rootActor.GetProperty<Vector3>(Dali::Actor::Property::ANCHOR_POINT);
  DALI_TEST_EQUALS(Vector3(1.0, 1.0, 0.5), anchorPoint, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelViewAnimation01(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_GLTF_FILE_NAME);
  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(50, 50));

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  uint32_t animationCount = view.GetAnimationCount();
  DALI_TEST_EQUALS(1, animationCount, TEST_LOCATION);

  Animation animationByIndex = view.GetAnimation(0u);
  DALI_TEST_CHECK(animationByIndex);

  Animation animationByName = view.GetAnimation("animation_AnimatedCube");
  DALI_TEST_CHECK(animationByName);
  DALI_TEST_EQUALS(animationByIndex, animationByName, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelViewAnimation02(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelView view = Scene3D::ModelView::New(TEST_GLTF_ANIMATION_TEST_FILE_NAME);
  view.SetProperty(Dali::Actor::Property::SIZE, Vector2(50, 50));

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  uint32_t animationCount = view.GetAnimationCount();
  DALI_TEST_EQUALS(9, animationCount, TEST_LOCATION);

  Animation animation1 = view.GetAnimation("Step Scale");
  DALI_TEST_CHECK(animation1);
  DALI_TEST_EQUALS(1.66667f, animation1.GetDuration(), 0.001f, TEST_LOCATION);

  Animation animation2 = view.GetAnimation("CubicSpline Scale");
  DALI_TEST_CHECK(animation2);
  DALI_TEST_EQUALS(1.66667f, animation2.GetDuration(), 0.001f, TEST_LOCATION);

  DALI_TEST_NOT_EQUALS(animation1, animation2, 0.0f, TEST_LOCATION);

  END_TEST;
}