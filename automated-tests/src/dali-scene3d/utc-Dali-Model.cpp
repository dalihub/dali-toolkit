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
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <stdlib.h>
#include <iostream>

#include <dali-toolkit/devel-api/focus-manager/keyboard-focus-manager-devel.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <toolkit-environment-variable.h>
#include <toolkit-event-thread-callback.h>

#include <dali-scene3d/public-api/controls/model/model.h>
#include <dali-scene3d/public-api/model-components/model-node.h>

#include <dali-scene3d/public-api/model-motion/motion-data.h>
#include <dali-scene3d/public-api/model-motion/motion-index/blend-shape-index.h>
#include <dali-scene3d/public-api/model-motion/motion-index/motion-transform-index.h>

#include <dali-scene3d/public-api/loader/node-definition.h>

#include <dali/devel-api/actors/camera-actor-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;

void model_startup(void)
{
  test_return_value = TET_UNDEF;
}

void model_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const bool DEFAULT_MODEL_CHILDREN_SENSITIVE = false;
const bool DEFAULT_MODEL_CHILDREN_FOCUSABLE = false;
/**
 * For the AnimatedCube.gltf and its Assets
 * Donated by Norbert Nopper for glTF testing.
 * Take from https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/AnimatedCube
 */
const char* TEST_GLTF_FILE_NAME                    = TEST_RESOURCE_DIR "/AnimatedCube.gltf";
const char* TEST_GLTF_FILE_NAME_SAME_FILE          = TEST_RESOURCE_DIR "/AnimatedCube2.gltf";
const char* TEST_GLTF_FILE_NAME_DIFF_META_FILE     = TEST_RESOURCE_DIR "/AnimatedCube3.gltf";
const char* TEST_GLTF_ANIMATION_TEST_FILE_NAME     = TEST_RESOURCE_DIR "/animationTest.gltf";
const char* TEST_GLTF_EXTRAS_FILE_NAME             = TEST_RESOURCE_DIR "/AnimatedMorphCubeAnimateNonZeroFrame.gltf";
const char* TEST_GLTF_MULTIPLE_PRIMITIVE_FILE_NAME = TEST_RESOURCE_DIR "/simpleMultiplePrimitiveTest.gltf";
const char* TEST_GLTF_MORPH_FILE_NAME              = TEST_RESOURCE_DIR "/AnimatedMorphCube.gltf";
const char* TEST_DLI_FILE_NAME                     = TEST_RESOURCE_DIR "/arc.dli";
const char* TEST_DLI_EXERCISE_FILE_NAME            = TEST_RESOURCE_DIR "/exercise.dli";

/**
 * For the diffuse and specular cube map texture.
 * These textures are based off version of Wave engine sample
 * Take from https://github.com/WaveEngine/Samples
 *
 * Copyright (c) 2024 Wave Coorporation
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

bool gTouchCallBackCalled = false;
bool TestTouchCallback(Actor, const TouchEvent&)
{
  gTouchCallBackCalled = true;
  return true;
}

bool gFocusChangedCallBackCalled = false;
void TestFocusChangedCallback(Actor, Actor)
{
  gFocusChangedCallBackCalled = true;
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

void ApplyAllMaterialPropertyRecursively(Scene3D::ModelNode modelNode, const std::vector<KeyValuePair>& materialPropertyValues)
{
  if(!modelNode)
  {
    return;
  }

  for(uint32_t primitiveIndex = 0u; primitiveIndex < modelNode.GetModelPrimitiveCount(); ++primitiveIndex)
  {
    Scene3D::ModelPrimitive primitive = modelNode.GetModelPrimitive(primitiveIndex);
    if(primitive)
    {
      Scene3D::Material material = primitive.GetMaterial();
      if(material)
      {
        for(const auto& keyValuePair : materialPropertyValues)
        {
          if(keyValuePair.first.type == Property::Key::Type::INDEX)
          {
            material.SetProperty(keyValuePair.first.indexKey, keyValuePair.second);
          }
        }
      }
    }
  }

  for(uint32_t childIndex = 0u; childIndex < modelNode.GetChildCount(); ++childIndex)
  {
    Scene3D::ModelNode childNode = Scene3D::ModelNode::DownCast(modelNode.GetChildAt(childIndex));
    ApplyAllMaterialPropertyRecursively(childNode, materialPropertyValues);
  }
}

} // namespace

// Negative test case for a method
int UtcDaliModelUninitialized(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelUninitialized");

  Scene3D::Model model;

  try
  {
    // New() must be called to create a Model or it wont be valid.
    Actor a = Actor::New();
    model.Add(a);
    DALI_TEST_CHECK(false);
  }
  catch(Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_CHECK(!model);
  }
  END_TEST;
}

// Positive test case for a method
int UtcDaliModelNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelNew");

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  DALI_TEST_CHECK(model);
  END_TEST;
}

// Positive test case for a method
int UtcDaliModelNewP2(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelNew without url");

  Scene3D::Model model = Scene3D::Model::New();
  DALI_TEST_CHECK(model);

  application.GetScene().Add(model);

  DALI_TEST_CHECK(model.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  application.GetScene().Remove(model);

  DALI_TEST_CHECK(!model.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  END_TEST;
}

int UtcDaliModelNewSameModelUrlCached(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelNew with same model");

  // Set up trace debug
  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  DALI_TEST_CHECK(model);
  Scene3D::Model model2 = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  DALI_TEST_CHECK(model2);

  application.GetScene().Add(model);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  tet_printf("Test if there is at least 1 texture.\n");
  int expectTextureCount = textureTrace.CountMethod("GenTextures");
  DALI_TEST_GREATER(expectTextureCount, 0, TEST_LOCATION);

  application.GetScene().Add(model2);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  tet_printf("Test if we reuse cached texture or not.\n");
  int currentTextureCount = textureTrace.CountMethod("GenTextures");
  DALI_TEST_EQUALS(currentTextureCount, expectTextureCount, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliModelNewSameResourceUrlCached01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelNew with difference url but same model");

  // Set up trace debug
  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  textureTrace.EnableLogging(true);

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  DALI_TEST_CHECK(model);
  Scene3D::Model model2 = Scene3D::Model::New(TEST_GLTF_FILE_NAME_SAME_FILE); // Difference model that use same Images.
  DALI_TEST_CHECK(model2);
  Scene3D::Model model3 = Scene3D::Model::New(TEST_GLTF_FILE_NAME_DIFF_META_FILE); // Difference model that use same Images, but difference metadata.
  DALI_TEST_CHECK(model3);

  application.GetScene().Add(model);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  tet_printf("Test if there is at least 1 texture.\n");
  int expectTextureCount = textureTrace.CountMethod("GenTextures");
  DALI_TEST_GREATER(expectTextureCount, 0, TEST_LOCATION);

  application.GetScene().Add(model2);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  tet_printf("Test if we reuse cached texture or not.\n");
  int currentTextureCount = textureTrace.CountMethod("GenTextures");
  DALI_TEST_EQUALS(currentTextureCount, expectTextureCount, TEST_LOCATION);

  application.GetScene().Add(model3);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  tet_printf("Test if we don't reuse cached texture, due to the metadata difference.\n");
  currentTextureCount = textureTrace.CountMethod("GenTextures");
  DALI_TEST_GREATER(currentTextureCount, expectTextureCount, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliModelNewSameResourceUrlCached02(void)
{
  /// Make we don't use mutiple thread loading for this UTC.
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_ASYNC_MANAGER_THREAD_POOL_SIZE", "1");
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_ASYNC_MANAGER_LOW_PRIORITY_THREAD_POOL_SIZE", "1");

  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelNew with difference url but same model");

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  DALI_TEST_CHECK(model);
  Scene3D::Model model2 = Scene3D::Model::New(TEST_GLTF_FILE_NAME_SAME_FILE);
  DALI_TEST_CHECK(model2);

  application.GetScene().Add(model);
  application.GetScene().Add(model2);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  END_TEST;
}

// Positive test case for a method
int UtcDaliModelDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelDownCast");

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  BaseHandle     handle(model);

  Scene3D::Model model2 = Scene3D::Model::DownCast(handle);
  DALI_TEST_CHECK(model);
  DALI_TEST_CHECK(model2);
  DALI_TEST_CHECK(model2 == model);
  END_TEST;
}

int UtcDaliModelTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK(typeRegistry);

  TypeInfo typeInfo = typeRegistry.GetTypeInfo("Model");
  DALI_TEST_CHECK(typeInfo);

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK(handle);

  Scene3D::Model model = Scene3D::Model::DownCast(handle);
  DALI_TEST_CHECK(model);

  END_TEST;
}

// Positive test case for a method
int UtcDaliModelAddRemove(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelAddRemove");

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  DALI_TEST_CHECK(model);

  Actor actor = Actor::New();
  DALI_TEST_CHECK(!actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  model.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  model.SetProperty(Actor::Property::SIZE, application.GetScene().GetSize());
  model.Add(actor);
  application.GetScene().Add(model);

  DALI_TEST_CHECK(actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  model.Remove(actor);

  DALI_TEST_CHECK(!actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));
  END_TEST;
}

int UtcDaliModelCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  DALI_TEST_CHECK(model);

  Scene3D::Model copy(model);
  DALI_TEST_CHECK(model == copy);

  Scene3D::Model assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == model);

  END_TEST;
}

int UtcDaliModelMoveConstructor(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  DALI_TEST_EQUALS(1, model.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  model.SetProperty(Actor::Property::SENSITIVE, false);
  DALI_TEST_CHECK(false == model.GetProperty<bool>(Actor::Property::SENSITIVE));

  Scene3D::Model moved = std::move(model);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(false == moved.GetProperty<bool>(Actor::Property::SENSITIVE));
  DALI_TEST_CHECK(!model);

  END_TEST;
}

int UtcDaliModelMoveAssignment(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  DALI_TEST_EQUALS(1, model.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  model.SetProperty(Actor::Property::SENSITIVE, false);
  DALI_TEST_CHECK(false == model.GetProperty<bool>(Actor::Property::SENSITIVE));

  Scene3D::Model moved;
  moved = std::move(model);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(false == moved.GetProperty<bool>(Actor::Property::SENSITIVE));
  DALI_TEST_CHECK(!model);

  END_TEST;
}

int UtcDaliModelOnScene01(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  application.GetScene().Add(model);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  uint32_t modelCount = model.GetModelRoot().GetChildCount();
  DALI_TEST_EQUALS(1, modelCount, TEST_LOCATION);
  END_TEST;
}

int UtcDaliModelOnScene02(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_DLI_FILE_NAME);
  application.GetScene().Add(model);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  uint32_t modelCount = model.GetModelRoot().GetChildCount();
  DALI_TEST_EQUALS(1, modelCount, TEST_LOCATION);

  Scene3D::ModelNode rootNode = model.GetModelRoot();
  Vector3            rootSize = rootNode.GetProperty<Vector3>(Dali::Actor::Property::SIZE);
  DALI_TEST_EQUALS(Vector3(2, 2, 1), rootSize, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelOnSizeSet(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);

  application.GetScene().Add(model);

  application.SendNotification();
  application.Render();

  Vector2 size(200.0f, 300.0f);
  model.SetProperty(Actor::Property::SIZE, size);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(model.GetCurrentProperty<Vector2>(Actor::Property::SIZE), size, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelGetNaturalSize(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);

  Vector3 naturalSize = model.GetNaturalSize();
  DALI_TEST_EQUALS(Vector3::ZERO, naturalSize, TEST_LOCATION);

  application.GetScene().Add(model);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  naturalSize = model.GetNaturalSize();
  DALI_TEST_EQUALS(Vector3(2, 2, 2), naturalSize, TEST_LOCATION);

  Scene3D::ModelNode rootNode = model.GetModelRoot();
  DALI_TEST_CHECK(rootNode);

  END_TEST;
}

int UtcDaliModelSetImageBasedLightSource01(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  application.GetScene().Add(model);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  Actor meshActor = model.FindChildByName("AnimatedCube");
  DALI_TEST_CHECK(meshActor);

  Renderer renderer = meshActor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  TextureSet textureSet = renderer.GetTextures();
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 10u, TEST_LOCATION);

  Texture diffuseTexture  = textureSet.GetTexture(8u);
  Texture specularTexture = textureSet.GetTexture(9u);

  gResourceReadyCalled = false;
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);
  model.SetImageBasedLightSource(TEST_DIFFUSE_TEXTURE, TEST_SPECULAR_TEXTURE);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  TextureSet newTextureSet      = renderer.GetTextures();
  Texture    newDiffuseTexture  = newTextureSet.GetTexture(8u);
  Texture    newSpecularTexture = newTextureSet.GetTexture(9u);

  DALI_TEST_NOT_EQUALS(diffuseTexture, newDiffuseTexture, 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(specularTexture, newSpecularTexture, 0.0f, TEST_LOCATION);

  model.Unparent();
  model.Reset();
  END_TEST;
}

int UtcDaliModelSetImageBasedLightSource02(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  application.GetScene().Add(model);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  Actor meshActor = model.FindChildByName("AnimatedCube");
  DALI_TEST_CHECK(meshActor);

  Renderer renderer = meshActor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  TextureSet textureSet = renderer.GetTextures();
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 10u, TEST_LOCATION);

  Texture diffuseTexture  = textureSet.GetTexture(8u);
  Texture specularTexture = textureSet.GetTexture(9u);

  // if url is empty, loading is not requested.
  model.SetImageBasedLightSource("", "");

  Texture newDiffuseTexture  = textureSet.GetTexture(8u);
  Texture newSpecularTexture = textureSet.GetTexture(9u);

  DALI_TEST_EQUALS(diffuseTexture, newDiffuseTexture, TEST_LOCATION);
  DALI_TEST_EQUALS(specularTexture, newSpecularTexture, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelSetImageBasedLightSource03(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  application.GetScene().Add(model);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  Actor meshActor = model.FindChildByName("AnimatedCube");
  DALI_TEST_CHECK(meshActor);

  Renderer renderer = meshActor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  TextureSet textureSet = renderer.GetTextures();
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 10u, TEST_LOCATION);

  Texture diffuseTexture  = textureSet.GetTexture(8u);
  Texture specularTexture = textureSet.GetTexture(9u);

  gResourceReadyCalled = false;
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);
  model.SetImageBasedLightSource("dummy.ktx", "dummy.ktx");

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  Texture newDiffuseTexture  = textureSet.GetTexture(8u);
  Texture newSpecularTexture = textureSet.GetTexture(9u);

  DALI_TEST_EQUALS(diffuseTexture, newDiffuseTexture, TEST_LOCATION);
  DALI_TEST_EQUALS(specularTexture, newSpecularTexture, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelSetImageBasedLightSource04(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  model.SetImageBasedLightSource(TEST_DIFFUSE_TEXTURE, TEST_SPECULAR_TEXTURE);
  application.GetScene().Add(model);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliModelImageBasedFactor(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);

  DALI_TEST_EQUALS(model.GetImageBasedLightScaleFactor(), 1.0f, TEST_LOCATION);

  model.SetImageBasedLightScaleFactor(0.5f);
  DALI_TEST_EQUALS(model.GetImageBasedLightScaleFactor(), 0.5f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliModelChildrenSensitive01(void)
{
  ToolkitTestApplication application;

  Scene3D::Model view = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  view.SetProperty(Dali::Actor::Property::SIZE, Vector3(100, 100, 100));
  view.SetProperty(Dali::Actor::Property::POSITION, Vector3(0, 0, 0));
  view.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  view.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  // Get default vaule.
  DALI_TEST_EQUALS(view.GetChildrenSensitive(), DEFAULT_MODEL_CHILDREN_SENSITIVE, TEST_LOCATION);

  // Allow children actor's event before on scene.
  view.SetChildrenSensitive(true);
  DALI_TEST_EQUALS(view.GetChildrenSensitive(), true, TEST_LOCATION);
  application.GetScene().Add(view);

  gResourceReadyCalled = false;
  view.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  Actor meshActor = view.FindChildByName("AnimatedCube");
  DALI_TEST_CHECK(meshActor);

  // connect hit-test signal
  gTouchCallBackCalled = false;
  meshActor.TouchedSignal().Connect(TestTouchCallback);

  Vector2 sceneSize = application.GetScene().GetSize();

  // Try to touch center of scene.
  Dali::Integration::Point point;
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(sceneSize * 0.5f);
  Dali::Integration::TouchEvent event;
  event.AddPoint(point);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Not touched yet.
  DALI_TEST_CHECK(!gTouchCallBackCalled);
  application.ProcessEvent(event);
  // Touched.
  DALI_TEST_CHECK(gTouchCallBackCalled);

  // Clear
  gTouchCallBackCalled = false;

  // Block children actor's event
  view.SetChildrenSensitive(false);
  DALI_TEST_EQUALS(view.GetChildrenSensitive(), false, TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Not touched yet.
  DALI_TEST_CHECK(!gTouchCallBackCalled);
  application.ProcessEvent(event);
  // Also not touched.
  DALI_TEST_CHECK(!gTouchCallBackCalled);

  // Clear
  gTouchCallBackCalled = false;

  // Allow again
  view.SetChildrenSensitive(true);
  DALI_TEST_EQUALS(view.GetChildrenSensitive(), true, TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Not touched yet.
  DALI_TEST_CHECK(!gTouchCallBackCalled);
  application.ProcessEvent(event);
  // Touched.
  DALI_TEST_CHECK(gTouchCallBackCalled);

  // Clear
  gTouchCallBackCalled = false;

  END_TEST;
}

int UtcDaliModelChildrenSensitive02(void)
{
  ToolkitTestApplication application;

  Scene3D::Model view = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  view.SetProperty(Dali::Actor::Property::SIZE, Vector3(100, 100, 100));
  view.SetProperty(Dali::Actor::Property::POSITION, Vector3(0, 0, 0));
  view.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  view.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  // Get vaule.
  DALI_TEST_EQUALS(view.GetChildrenSensitive(), DEFAULT_MODEL_CHILDREN_SENSITIVE, TEST_LOCATION);

  // Block children actor's event before on scene.
  view.SetChildrenSensitive(false);
  DALI_TEST_EQUALS(view.GetChildrenSensitive(), false, TEST_LOCATION);
  application.GetScene().Add(view);

  gResourceReadyCalled = false;
  view.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  Actor meshActor = view.FindChildByName("AnimatedCube");
  DALI_TEST_CHECK(meshActor);

  // connect hit-test signal
  gTouchCallBackCalled = false;
  meshActor.TouchedSignal().Connect(TestTouchCallback);

  Vector2 sceneSize = application.GetScene().GetSize();

  // Try to touch center of scene.
  Dali::Integration::Point point;
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(sceneSize * 0.5f);
  Dali::Integration::TouchEvent event;
  event.AddPoint(point);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Not touched yet.
  DALI_TEST_CHECK(!gTouchCallBackCalled);
  application.ProcessEvent(event);
  // Also not touched.
  DALI_TEST_CHECK(!gTouchCallBackCalled);

  // Clear
  gTouchCallBackCalled = false;

  // Allow again
  view.SetChildrenSensitive(true);
  DALI_TEST_EQUALS(view.GetChildrenSensitive(), true, TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Not touched yet.
  DALI_TEST_CHECK(!gTouchCallBackCalled);
  application.ProcessEvent(event);
  // Touched.
  DALI_TEST_CHECK(gTouchCallBackCalled);

  // Clear
  gTouchCallBackCalled = false;

  END_TEST;
}

int UtcDaliModelChildrenFocusable01(void)
{
  ToolkitTestApplication application;

  Scene3D::Model view = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  view.SetProperty(Dali::Actor::Property::SIZE, Vector3(100, 100, 100));
  view.SetProperty(Dali::Actor::Property::POSITION, Vector3(0, 0, 0));
  view.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  view.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  // Get vaule. Default is false.
  DALI_TEST_EQUALS(view.GetChildrenFocusable(), DEFAULT_MODEL_CHILDREN_FOCUSABLE, TEST_LOCATION);

  // Allow children actor's focus before on scene.
  view.SetChildrenFocusable(true);
  DALI_TEST_EQUALS(view.GetChildrenFocusable(), true, TEST_LOCATION);
  application.GetScene().Add(view);

  gResourceReadyCalled = false;
  view.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  Actor meshActor = view.FindChildByName("AnimatedCube");
  DALI_TEST_CHECK(meshActor);

  // Enable the default algorithm
  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);
  Dali::Toolkit::DevelKeyboardFocusManager::EnableDefaultAlgorithm(manager, true);

  // connect focusable signal
  gFocusChangedCallBackCalled = false;
  meshActor.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  manager.FocusChangedSignal().Connect(TestFocusChangedCallback);

  // Initialize with some left-positioned actor
  Control focusStartActor = Control::New();
  focusStartActor.SetProperty(Dali::Actor::Property::SIZE, Vector3(100, 100, 100));
  focusStartActor.SetProperty(Dali::Actor::Property::POSITION, Vector3(-200, 0, 0));
  focusStartActor.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  focusStartActor.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  focusStartActor.SetProperty(Dali::Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(focusStartActor);

  // Clear
  manager.ClearFocus();
  manager.SetCurrentFocusActor(focusStartActor);
  gFocusChangedCallBackCalled = false;

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Focusable view find success
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == true);
  DALI_TEST_CHECK(gFocusChangedCallBackCalled);

  // Clear
  manager.ClearFocus();
  manager.SetCurrentFocusActor(focusStartActor);
  gFocusChangedCallBackCalled = false;

  // Block children actor's focus
  view.SetChildrenFocusable(false);
  DALI_TEST_EQUALS(view.GetChildrenFocusable(), false, TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Focusable view find failed
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == false);
  DALI_TEST_CHECK(!gFocusChangedCallBackCalled);

  // Clear
  manager.ClearFocus();
  manager.SetCurrentFocusActor(focusStartActor);
  gFocusChangedCallBackCalled = false;

  // Allow again
  view.SetChildrenFocusable(true);
  DALI_TEST_EQUALS(view.GetChildrenFocusable(), true, TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Focusable view find success
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == true);
  DALI_TEST_CHECK(gFocusChangedCallBackCalled);

  // Clear
  manager.ClearFocus();
  manager.SetCurrentFocusActor(focusStartActor);
  gFocusChangedCallBackCalled = false;

  END_TEST;
}

int UtcDaliModelModelChildrenFocusable02(void)
{
  ToolkitTestApplication application;

  Scene3D::Model view = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  view.SetProperty(Dali::Actor::Property::SIZE, Vector3(100, 100, 100));
  view.SetProperty(Dali::Actor::Property::POSITION, Vector3(0, 0, 0));
  view.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  view.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  // Get vaule. Default is true.
  DALI_TEST_EQUALS(view.GetChildrenFocusable(), DEFAULT_MODEL_CHILDREN_FOCUSABLE, TEST_LOCATION);

  // Block children actor's focus before on scene.
  view.SetChildrenFocusable(false);
  DALI_TEST_EQUALS(view.GetChildrenFocusable(), false, TEST_LOCATION);
  application.GetScene().Add(view);

  gResourceReadyCalled = false;
  view.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  Actor meshActor = view.FindChildByName("AnimatedCube");
  DALI_TEST_CHECK(meshActor);

  // Enable the default algorithm
  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  DALI_TEST_CHECK(manager);
  Dali::Toolkit::DevelKeyboardFocusManager::EnableDefaultAlgorithm(manager, true);

  // connect focusable signal
  gFocusChangedCallBackCalled = false;
  meshActor.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  manager.FocusChangedSignal().Connect(TestFocusChangedCallback);

  // Initialize with some left-positioned actor
  Control focusStartActor = Control::New();
  focusStartActor.SetProperty(Dali::Actor::Property::SIZE, Vector3(100, 100, 100));
  focusStartActor.SetProperty(Dali::Actor::Property::POSITION, Vector3(-200, 0, 0));
  focusStartActor.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  focusStartActor.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  focusStartActor.SetProperty(Dali::Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(focusStartActor);

  // Clear
  manager.ClearFocus();
  manager.SetCurrentFocusActor(focusStartActor);
  gFocusChangedCallBackCalled = false;

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Focusable view find failed
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == false);
  DALI_TEST_CHECK(!gFocusChangedCallBackCalled);

  // Clear
  manager.ClearFocus();
  manager.SetCurrentFocusActor(focusStartActor);
  gFocusChangedCallBackCalled = false;

  // Allow again
  view.SetChildrenFocusable(true);
  DALI_TEST_EQUALS(view.GetChildrenFocusable(), true, TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Focusable view find success
  DALI_TEST_CHECK(manager.MoveFocus(Control::KeyboardFocus::RIGHT) == true);
  DALI_TEST_CHECK(gFocusChangedCallBackCalled);

  // Clear
  manager.ClearFocus();
  manager.SetCurrentFocusActor(focusStartActor);
  gFocusChangedCallBackCalled = false;

  END_TEST;
}

int UtcDaliModelAnimation01(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  model.SetProperty(Dali::Actor::Property::SIZE, Vector2(50, 50));
  application.GetScene().Add(model);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  uint32_t animationCount = model.GetAnimationCount();
  DALI_TEST_EQUALS(1, animationCount, TEST_LOCATION);

  Animation animationByIndex = model.GetAnimation(0u);
  DALI_TEST_CHECK(animationByIndex);

  Animation animationByName = model.GetAnimation("animation_AnimatedCube");
  DALI_TEST_CHECK(animationByName);
  DALI_TEST_EQUALS(animationByIndex, animationByName, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelAnimation02(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_ANIMATION_TEST_FILE_NAME);
  model.SetProperty(Dali::Actor::Property::SIZE, Vector2(50, 50));
  application.GetScene().Add(model);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  uint32_t animationCount = model.GetAnimationCount();
  DALI_TEST_EQUALS(9, animationCount, TEST_LOCATION);

  Animation animation1 = model.GetAnimation("Step Scale");
  DALI_TEST_CHECK(animation1);
  DALI_TEST_EQUALS(1.66667f, animation1.GetDuration(), 0.001f, TEST_LOCATION);

  Animation animation2 = model.GetAnimation("CubicSpline Scale");
  DALI_TEST_CHECK(animation2);
  DALI_TEST_EQUALS(1.66667f, animation2.GetDuration(), 0.001f, TEST_LOCATION);

  DALI_TEST_NOT_EQUALS(animation1, animation2, 0.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelAnimation03(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_DLI_EXERCISE_FILE_NAME);
  model.SetProperty(Dali::Actor::Property::SIZE, Vector2(50, 50));
  application.GetScene().Add(model);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  uint32_t animationCount = model.GetAnimationCount();
  DALI_TEST_EQUALS(18, animationCount, TEST_LOCATION);

  Animation animationByIndex = model.GetAnimation(0u);
  DALI_TEST_CHECK(animationByIndex);

  Animation animationByName = model.GetAnimation("idleClip");
  DALI_TEST_CHECK(animationByName);
  DALI_TEST_EQUALS(animationByIndex, animationByName, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelCameraGenerate01(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_DLI_EXERCISE_FILE_NAME);
  model.SetProperty(Dali::Actor::Property::SIZE, Vector2(50, 50));
  application.GetScene().Add(model);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  uint32_t cameraCount = model.GetCameraCount();
  DALI_TEST_EQUALS(1, cameraCount, TEST_LOCATION);

  CameraActor generatedCamera = model.GenerateCamera(0u);
  DALI_TEST_CHECK(generatedCamera);

  generatedCamera = model.GenerateCamera(1u); // Fail to generate camera
  DALI_TEST_CHECK(!generatedCamera);

  END_TEST;
}

int UtcDaliModelCameraGenerate02(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  model.SetProperty(Dali::Actor::Property::SIZE, Vector2(50, 50));
  application.GetScene().Add(model);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  uint32_t cameraCount = model.GetCameraCount();
  DALI_TEST_EQUALS(6, cameraCount, TEST_LOCATION);

  CameraActor generatedCamera0 = model.GenerateCamera(0u);
  DALI_TEST_CHECK(generatedCamera0);
  CameraActor generatedCamera1 = model.GenerateCamera(1u);
  DALI_TEST_CHECK(generatedCamera1);
  CameraActor generatedCamera2 = model.GenerateCamera(2u);
  DALI_TEST_CHECK(generatedCamera2);
  CameraActor generatedCamera3 = model.GenerateCamera(3u); // Infinity far camera
  DALI_TEST_CHECK(generatedCamera3);
  CameraActor generatedCamera4 = model.GenerateCamera(4u); // Broken camera 1
  DALI_TEST_CHECK(!generatedCamera4);
  CameraActor generatedCamera5 = model.GenerateCamera(5u); // Broken camera 2
  DALI_TEST_CHECK(!generatedCamera5);
  CameraActor generatedCamera6 = model.GenerateCamera(6u); // Out of bound
  DALI_TEST_CHECK(!generatedCamera6);

  CameraActor appliedCamera;
  DALI_TEST_EQUALS(model.ApplyCamera(0u, appliedCamera), false, TEST_LOCATION); // Cannot apply into empty camera.

  auto CompareCameraProperties = [](CameraActor lhs, CameraActor rhs, const char* location) {
    DALI_TEST_EQUALS(lhs.GetProperty<int>(Dali::CameraActor::Property::PROJECTION_MODE), rhs.GetProperty<int>(Dali::CameraActor::Property::PROJECTION_MODE), TEST_LOCATION);
    DALI_TEST_EQUALS(lhs.GetProperty<float>(Dali::CameraActor::Property::NEAR_PLANE_DISTANCE), rhs.GetProperty<float>(Dali::CameraActor::Property::NEAR_PLANE_DISTANCE), TEST_LOCATION);

    if(lhs.GetProperty<int>(Dali::CameraActor::Property::PROJECTION_MODE) == static_cast<int>(Dali::Camera::ProjectionMode::PERSPECTIVE_PROJECTION))
    {
      DALI_TEST_EQUALS(lhs.GetProperty<float>(Dali::CameraActor::Property::FIELD_OF_VIEW), rhs.GetProperty<float>(Dali::CameraActor::Property::FIELD_OF_VIEW), TEST_LOCATION);
      // TODO : Open this test when infinity far projection implement.
      // DALI_TEST_EQUALS(lhs.GetProperty<float>(Dali::CameraActor::Property::FAR_PLANE_DISTANCE), rhs.GetProperty<float>(Dali::CameraActor::Property::FAR_PLANE_DISTANCE), TEST_LOCATION);
    }
    else
    {
      DALI_TEST_EQUALS(lhs.GetProperty<float>(Dali::DevelCameraActor::Property::ORTHOGRAPHIC_SIZE), rhs.GetProperty<float>(Dali::DevelCameraActor::Property::ORTHOGRAPHIC_SIZE), TEST_LOCATION);
      DALI_TEST_EQUALS(lhs.GetProperty<float>(Dali::CameraActor::Property::FAR_PLANE_DISTANCE), rhs.GetProperty<float>(Dali::CameraActor::Property::FAR_PLANE_DISTANCE), TEST_LOCATION);
    }
  };

  appliedCamera = CameraActor::New();
  DALI_TEST_EQUALS(model.ApplyCamera(0u, appliedCamera), true, TEST_LOCATION);
  CompareCameraProperties(generatedCamera0, appliedCamera, TEST_LOCATION);
  DALI_TEST_EQUALS(model.ApplyCamera(1u, appliedCamera), true, TEST_LOCATION);
  CompareCameraProperties(generatedCamera1, appliedCamera, TEST_LOCATION);
  DALI_TEST_EQUALS(model.ApplyCamera(2u, appliedCamera), true, TEST_LOCATION);
  CompareCameraProperties(generatedCamera2, appliedCamera, TEST_LOCATION);
  DALI_TEST_EQUALS(model.ApplyCamera(3u, appliedCamera), true, TEST_LOCATION);
  CompareCameraProperties(generatedCamera3, appliedCamera, TEST_LOCATION);
  DALI_TEST_EQUALS(model.ApplyCamera(4u, appliedCamera), false, TEST_LOCATION); // Broken camera 1
  DALI_TEST_EQUALS(model.ApplyCamera(5u, appliedCamera), false, TEST_LOCATION); // Broken camera 2
  DALI_TEST_EQUALS(model.ApplyCamera(6u, appliedCamera), false, TEST_LOCATION); // Cannot apply over the index.

  END_TEST;
}

int UtcDaliModelMultiplePrimitives(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_MULTIPLE_PRIMITIVE_FILE_NAME);
  model.SetProperty(Dali::Actor::Property::SIZE, Vector2(50, 50));
  application.GetScene().Add(model);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  Actor actor = model.FindChildByName("rootNode");

  DALI_TEST_EQUALS(0, actor.GetChildCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(2, actor.GetRendererCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelColorMode(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  model.SetProperty(Dali::Actor::Property::SIZE, Vector2(50, 50));
  model.SetProperty(Dali::Actor::Property::COLOR, Color::RED);
  application.GetScene().Add(model);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  Actor   actor           = model.FindChildByName("AnimatedCube");
  Vector4 childColor      = actor[Dali::Actor::Property::COLOR];
  Vector4 childWorldColor = actor[Dali::Actor::Property::WORLD_COLOR];

  DALI_TEST_EQUALS(childColor, Color::WHITE, TEST_LOCATION);
  DALI_TEST_EQUALS(childWorldColor, Color::RED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelResourceReady(void)
{
  ToolkitTestApplication application;

  gOnRelayoutCallBackCalled = false;
  gResourceReadyCalled      = false;
  Scene3D::Model model      = Scene3D::Model::New(TEST_GLTF_ANIMATION_TEST_FILE_NAME);
  model.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  model.OnRelayoutSignal().Connect(OnRelayoutCallback);
  model.ResourceReadySignal().Connect(OnResourceReady);
  DALI_TEST_EQUALS(model.IsResourceReady(), false, TEST_LOCATION);

  // Sanity check
  DALI_TEST_CHECK(!gOnRelayoutCallBackCalled);
  DALI_TEST_CHECK(!gResourceReadyCalled);

  application.GetScene().Add(model);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gOnRelayoutCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(model.IsResourceReady(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelResourceReady02(void)
{
  tet_infoline("Test model load successfully even if shader language version is low\n");
  ToolkitTestApplication application;

  auto originalShaderVersion = application.GetGlAbstraction().GetShaderLanguageVersion();

  // Change the shader language version forcely!
  application.GetGlAbstraction().mShaderLanguageVersion = 200;

  try
  {
    gOnRelayoutCallBackCalled = false;
    gResourceReadyCalled      = false;
    Scene3D::Model model      = Scene3D::Model::New(TEST_GLTF_MORPH_FILE_NAME);
    model.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
    model.OnRelayoutSignal().Connect(OnRelayoutCallback);
    model.ResourceReadySignal().Connect(OnResourceReady);
    DALI_TEST_EQUALS(model.IsResourceReady(), false, TEST_LOCATION);

    // Sanity check
    DALI_TEST_CHECK(!gOnRelayoutCallBackCalled);
    DALI_TEST_CHECK(!gResourceReadyCalled);

    application.GetScene().Add(model);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(gOnRelayoutCallBackCalled, false, TEST_LOCATION);
    DALI_TEST_EQUALS(model.IsResourceReady(), true, TEST_LOCATION);
    DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

    // Change material information, for line coverage.
    auto modelNode = model.FindChildModelNodeByName("AnimatedMorphCube");
    DALI_TEST_CHECK(modelNode);
    DALI_TEST_GREATER(modelNode.GetModelPrimitiveCount(), 0u, TEST_LOCATION);
    auto modelPrimitive = modelNode.GetModelPrimitive(0u);
    DALI_TEST_CHECK(modelPrimitive);
    auto material = modelPrimitive.GetMaterial();
    DALI_TEST_CHECK(material);

    auto originBaseColorFactor = material.GetProperty<Dali::Vector4>(Dali::Scene3D::Material::Property::BASE_COLOR_FACTOR);
    auto expectBaseColorFactor = Vector4(originBaseColorFactor.r + 0.05f, originBaseColorFactor.g - 0.05f, originBaseColorFactor.b, originBaseColorFactor.a);
    material.SetProperty(Dali::Scene3D::Material::Property::BASE_COLOR_FACTOR, expectBaseColorFactor);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(material.GetProperty<Dali::Vector4>(Dali::Scene3D::Material::Property::BASE_COLOR_FACTOR), expectBaseColorFactor, TEST_LOCATION);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false);
  }

  // Revert shader version. We should revert it even if UTC failed.
  application.GetGlAbstraction().mShaderLanguageVersion = originalShaderVersion;

  END_TEST;
}

int UtcDaliModelResourceCacheCheck(void)
{
  ToolkitTestApplication application;

  // Load three instances of the same model and add them to the scene
  Scene3D::Model model1 = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  Scene3D::Model model2 = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  Scene3D::Model model3 = Scene3D::Model::New(TEST_GLTF_FILE_NAME);

  application.GetScene().Add(model1);
  application.GetScene().Add(model2);
  application.GetScene().Add(model3);

  gResourceReadyCalled = false;
  model1.ResourceReadySignal().Connect(&OnResourceReady);
  model2.ResourceReadySignal().Connect(&OnResourceReady);
  model3.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  // Check that the loading has finished for all the three instances
  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  Actor meshActor1 = model1.FindChildByName("AnimatedCube");
  Actor meshActor2 = model2.FindChildByName("AnimatedCube");
  Actor meshActor3 = model3.FindChildByName("AnimatedCube");
  DALI_TEST_CHECK(meshActor1);
  DALI_TEST_CHECK(meshActor2);
  DALI_TEST_CHECK(meshActor3);

  Renderer renderer1 = meshActor1.GetRendererAt(0u);
  Renderer renderer2 = meshActor2.GetRendererAt(0u);
  Renderer renderer3 = meshActor3.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer1);
  DALI_TEST_CHECK(renderer2);
  DALI_TEST_CHECK(renderer3);

  // Check that all the three instances use the shared textures and geometries from the cache
  // but have their own shader objects
  DALI_TEST_EQUALS(renderer1.GetTextures(), renderer2.GetTextures(), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer1.GetTextures(), renderer3.GetTextures(), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer1.GetGeometry(), renderer2.GetGeometry(), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer1.GetGeometry(), renderer3.GetGeometry(), TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(renderer1.GetShader(), renderer2.GetShader(), 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(renderer1.GetShader(), renderer3.GetShader(), 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(renderer2.GetShader(), renderer3.GetShader(), 0.0f, TEST_LOCATION);

  // Destroy model1
  model1.Unparent();
  model1.Reset();

  // Check that all the other two instances still use the shared textures and geometries from the cache
  // but have their own shader objects
  DALI_TEST_EQUALS(renderer2.GetTextures(), renderer3.GetTextures(), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer2.GetGeometry(), renderer3.GetGeometry(), TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(renderer2.GetShader(), renderer3.GetShader(), 0.0f, TEST_LOCATION);

  // Set new IBL textures for model2, and this should apply to model2 instance only
  gResourceReadyCalled = false;
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);
  model2.SetImageBasedLightSource(TEST_DIFFUSE_TEXTURE, TEST_SPECULAR_TEXTURE);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  // Check that the new IBL textures are loaded for model2
  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  // Check that the two instances still use the shared geometries from the cache
  // but now have their own shader objects and different texture set
  DALI_TEST_NOT_EQUALS(renderer2.GetTextures(), renderer3.GetTextures(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer2.GetGeometry(), renderer3.GetGeometry(), TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(renderer2.GetShader(), renderer3.GetShader(), 0.0f, TEST_LOCATION);

  // Check that the two instances now have their own diffuse texture and specular texture,
  // but all the other textures are still the same
  TextureSet textureSet2 = renderer2.GetTextures();
  TextureSet textureSet3 = renderer3.GetTextures();
  DALI_TEST_EQUALS(textureSet2.GetTextureCount(), 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet3.GetTextureCount(), 10u, TEST_LOCATION);

  for(uint32_t i = 0; i < 7u; i++)
  {
    DALI_TEST_EQUALS(textureSet2.GetTexture(i), textureSet3.GetTexture(i), TEST_LOCATION);
  }

  DALI_TEST_NOT_EQUALS(textureSet2.GetTexture(8u), textureSet3.GetTexture(7u), 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(textureSet2.GetTexture(9u), textureSet3.GetTexture(8u), 0.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelAddRemoveModelNode(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New();
  model.SetProperty(Dali::Actor::Property::SIZE, Vector2(50, 50));

  Scene3D::ModelNode node1 = Scene3D::ModelNode::New();
  Scene3D::ModelNode node2 = Scene3D::ModelNode::New();
  Scene3D::ModelNode node3 = Scene3D::ModelNode::New();
  Scene3D::ModelNode node4 = Scene3D::ModelNode::New();

  model.AddModelNode(node1);
  model.AddModelNode(node2);
  model.AddModelNode(node3);
  model.RemoveModelNode(node1); // Remove node before scene on

  application.GetScene().Add(model);

  Dali::Scene3D::ModelNode root = model.GetModelRoot();
  DALI_TEST_CHECK(root);
  DALI_TEST_EQUALS(2, root.GetChildCount(), TEST_LOCATION);

  model.RemoveModelNode(node2); // Remove node after scene on

  DALI_TEST_EQUALS(1, root.GetChildCount(), TEST_LOCATION);

  model.AddModelNode(node4); // Add during scene on

  DALI_TEST_EQUALS(2, root.GetChildCount(), TEST_LOCATION);

  application.GetScene().Remove(model);

  model.RemoveModelNode(node3); // Remove node after scene off

  END_TEST;
}

int UtcDaliModelFindChildModelNodeByName(void)
{
  tet_infoline(" UtcDaliModelNodeFindChildModelNodeByName.");

  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New();
  application.GetScene().Add(model);

  Scene3D::ModelNode modelNode1 = Scene3D::ModelNode::New();
  Scene3D::ModelNode modelNode2 = Scene3D::ModelNode::New();

  modelNode1.SetProperty(Dali::Actor::Property::NAME, "modelNode1");
  modelNode2.SetProperty(Dali::Actor::Property::NAME, "modelNode2");
  model.AddModelNode(modelNode1);
  model.AddModelNode(modelNode2);

  Scene3D::ModelNode child1 = model.FindChildModelNodeByName("modelNode1");
  DALI_TEST_CHECK(child1);
  DALI_TEST_EQUALS(child1, modelNode1, TEST_LOCATION);

  Scene3D::ModelNode child2 = model.FindChildModelNodeByName("modelNode2");
  DALI_TEST_CHECK(child2);
  DALI_TEST_EQUALS(child2, modelNode2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelSizeChange(void)
{
  tet_infoline(" UtcDaliModelSizeChange.");

  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  model.SetProperty(Dali::Actor::Property::SIZE, Vector3(300, 300, 300));
  application.GetScene().Add(model);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(model.GetChildCount(), 1u, TEST_LOCATION);
  Vector3 scale = model.GetChildAt(0u).GetProperty<Vector3>(Dali::Actor::Property::SCALE);

  model.SetProperty(Dali::Actor::Property::SIZE, Vector3(600, 600, 600));
  Vector3 scale2 = model.GetChildAt(0u).GetProperty<Vector3>(Dali::Actor::Property::SCALE);

  DALI_TEST_NOT_EQUALS(scale, scale2, 0.1f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelSizeChange2(void)
{
  tet_infoline(" UtcDaliModelSizeChange2.");

  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  model.SetProperty(Dali::Actor::Property::SIZE, Vector3(300, 300, 300));
  application.GetScene().Add(model);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(model.GetChildCount(), 1u, TEST_LOCATION);
  Vector3 scale = model.GetChildAt(0u).GetProperty<Vector3>(Dali::Actor::Property::SCALE);

  Animation animation = Animation::New(0.5f);
  animation.AnimateTo(Dali::Property(model, Dali::Actor::Property::SIZE), Vector3(600, 600, 600));
  animation.Play();

  application.SendNotification();
  application.Render(250);

  application.SendNotification();

  Vector3 scale2 = model.GetChildAt(0u).GetProperty<Vector3>(Dali::Actor::Property::SCALE);
  DALI_TEST_NOT_EQUALS(scale, scale2, 0.1f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelRetrieveBlendShapeNames(void)
{
  tet_infoline(" UtcDaliModelRetrieveBlendShapeByName.");

  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_EXTRAS_FILE_NAME);
  model.SetProperty(Dali::Actor::Property::SIZE, Vector3(300, 300, 300));
  application.GetScene().Add(model);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(model.GetChildCount(), 1u, TEST_LOCATION);

  // Get target ModelNode that has extras
  Scene3D::ModelNode expectNode = model.FindChildModelNodeByName("AnimatedMorphCube");

  // Pair of expected blend shape index from expectNode.
  std::map<std::string, Scene3D::Loader::BlendShapes::Index> expectBlendShapeNames = {
    {"Target_0", 0u},
    {"Target_1", 1u},
  };

  std::vector<std::string> blendShapeNameList;
  model.RetrieveBlendShapeNames(blendShapeNameList);

  DALI_TEST_EQUALS(blendShapeNameList.size(), expectBlendShapeNames.size(), TEST_LOCATION);
  for(auto i = 0u; i < blendShapeNameList.size(); ++i)
  {
    const auto& name = blendShapeNameList[i];
    tet_printf("Check retrieved blendshape name : %s\n", name.c_str());

    const auto& iter = expectBlendShapeNames.find(name);
    DALI_TEST_CHECK(iter != expectBlendShapeNames.end());

    std::vector<Scene3D::ModelNode> nodeList;
    model.RetrieveModelNodesByBlendShapeName(name, nodeList);
    DALI_TEST_EQUALS(nodeList.size(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(nodeList[0], expectNode, TEST_LOCATION);
    DALI_TEST_EQUALS(nodeList[0].GetBlendShapeIndexByName(name), iter->second, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliModelGenerateMotionDataAnimation01(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_MORPH_FILE_NAME);
  model.SetProperty(Dali::Actor::Property::SIZE, Vector2(50, 50));
  application.GetScene().Add(model);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  KeyFrames floatKeyFrames = KeyFrames::New();
  floatKeyFrames.Add(0.0f, 1.0f);
  floatKeyFrames.Add(1.0f, 0.5f);

  float               duration   = 3.0f;
  Scene3D::MotionData motionData = Scene3D::MotionData::New(duration);
  motionData.Add(Scene3D::MotionTransformIndex::New("AnimatedMorphCube", Scene3D::MotionTransformIndex::TransformType::SCALE_Y), Scene3D::MotionValue::New(2.0f));
  motionData.Add(Scene3D::MotionTransformIndex::New("AnimatedMorphCube", Scene3D::MotionTransformIndex::TransformType::SCALE_Z), Scene3D::MotionValue::New(floatKeyFrames));
  motionData.Add(Scene3D::BlendShapeIndex::New("AnimatedMorphCube", 0), Scene3D::MotionValue::New(0.5f));
  motionData.Add(Scene3D::BlendShapeIndex::New("AnimatedMorphCube", 1), Scene3D::MotionValue::New(floatKeyFrames));

  Animation generatedAnimation = model.GenerateMotionDataAnimation(motionData);
  DALI_TEST_CHECK(generatedAnimation);
  DALI_TEST_EQUALS(generatedAnimation.GetDuration(), duration, TEST_LOCATION);

  Scene3D::MotionData invalidMotionData = Scene3D::MotionData::New(duration);
  invalidMotionData.Add(Scene3D::MotionTransformIndex::New("NotAnimatedMorphCube", Scene3D::MotionTransformIndex::TransformType::SCALE_Y), Scene3D::MotionValue::New(2.0f));
  invalidMotionData.Add(Scene3D::MotionTransformIndex::New("NotAnimatedMorphCube", Scene3D::MotionTransformIndex::TransformType::SCALE_Z), Scene3D::MotionValue::New(floatKeyFrames));
  invalidMotionData.Add(Scene3D::BlendShapeIndex::New("NotAnimatedMorphCube", 0), Scene3D::MotionValue::New(0.5f));
  invalidMotionData.Add(Scene3D::BlendShapeIndex::New("NotAnimatedMorphCube", 1), Scene3D::MotionValue::New(floatKeyFrames));

  generatedAnimation = model.GenerateMotionDataAnimation(invalidMotionData);
  DALI_TEST_CHECK(!generatedAnimation); // Animation should be empty if motion data have invalid index.

  END_TEST;
}

int UtcDaliModelSetMotionData(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_MORPH_FILE_NAME);
  model.SetProperty(Dali::Actor::Property::SIZE, Vector2(50, 50));
  application.GetScene().Add(model);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  KeyFrames floatKeyFrames = KeyFrames::New();
  floatKeyFrames.Add(0.0f, 1.0f);
  floatKeyFrames.Add(1.0f, 0.5f);

  float               duration   = 3.0f;
  Scene3D::MotionData motionData = Scene3D::MotionData::New(duration);
  motionData.Add(Scene3D::MotionTransformIndex::New("AnimatedMorphCube", Scene3D::MotionTransformIndex::TransformType::SCALE_Y), Scene3D::MotionValue::New(2.0f));
  motionData.Add(Scene3D::MotionTransformIndex::New("AnimatedMorphCube", Scene3D::MotionTransformIndex::TransformType::SCALE_Z), Scene3D::MotionValue::New(floatKeyFrames));
  motionData.Add(Scene3D::BlendShapeIndex::New("AnimatedMorphCube", 0), Scene3D::MotionValue::New(0.5f));
  motionData.Add(Scene3D::BlendShapeIndex::New("AnimatedMorphCube", 1), Scene3D::MotionValue::New(floatKeyFrames));

  auto cubeModelNode = model.FindChildModelNodeByName("AnimatedMorphCube");

  float expectScaleX = cubeModelNode.GetProperty<float>(Actor::Property::SCALE_X);

  model.SetMotionData(motionData);

  DALI_TEST_EQUALS(cubeModelNode.GetProperty<float>(Actor::Property::SCALE_X), expectScaleX, TEST_LOCATION);
  DALI_TEST_EQUALS(cubeModelNode.GetProperty<float>(Actor::Property::SCALE_Y), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(cubeModelNode.GetProperty<float>(Actor::Property::SCALE_Z), 0.5f, TEST_LOCATION); ///< Last value of keyframes

  END_TEST;
}

int UtcDaliModelBlendShapeMotionDataByName(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_EXTRAS_FILE_NAME);
  model.SetProperty(Dali::Actor::Property::SIZE, Vector2(50, 50));
  application.GetScene().Add(model);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  KeyFrames floatKeyFrames = KeyFrames::New();
  floatKeyFrames.Add(0.0f, 0.5f);
  floatKeyFrames.Add(1.0f, 1.0f);

  float               duration   = 3.0f;
  Scene3D::MotionData motionData = Scene3D::MotionData::New(duration);
  motionData.Add(Scene3D::BlendShapeIndex::New("Target_0"), Scene3D::MotionValue::New(0.5f));
  motionData.Add(Scene3D::BlendShapeIndex::New("Target_1"), Scene3D::MotionValue::New(floatKeyFrames));

  Animation generatedAnimation = model.GenerateMotionDataAnimation(motionData);
  DALI_TEST_CHECK(generatedAnimation);
  DALI_TEST_EQUALS(generatedAnimation.GetDuration(), duration, TEST_LOCATION);

  model.SetMotionData(motionData);

  // Get target ModelNode that has extras
  Scene3D::ModelNode expectNode    = model.FindChildModelNodeByName("AnimatedMorphCube");
  auto               propertyIndex = expectNode.GetPropertyIndex(motionData.GetIndex(0u).GetPropertyName(expectNode));

  DALI_TEST_CHECK(propertyIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(expectNode.GetProperty<float>(propertyIndex), 0.5f, TEST_LOCATION);

  propertyIndex = expectNode.GetPropertyIndex(motionData.GetIndex(1u).GetPropertyName(expectNode));
  DALI_TEST_CHECK(propertyIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(expectNode.GetProperty<float>(propertyIndex), 1.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelMaterialUniformChange(void)
{
  ToolkitTestApplication application;

  static std::vector<UniformData> customUniforms =
    {
      UniformData("uColorFactor", Property::Type::VECTOR4),
      UniformData("uBaseColorTextureTransformAvailable", Property::Type::FLOAT),
      UniformData(Scene3D::Loader::NodeDefinition::GetIblMaxLodUniformName().data(), Property::Type::FLOAT),
      UniformData(Scene3D::Loader::NodeDefinition::GetIblScaleFactorUniformName().data(), Property::Type::FLOAT),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  auto& gl = application.GetGlAbstraction();

  Scene3D::Model model = Scene3D::Model::New(TEST_GLTF_FILE_NAME);

  gResourceReadyCalled = false;
  model.ResourceReadySignal().Connect(&OnResourceReady);

  float expectIblFactor = 0.5f;
  model.SetImageBasedLightSource(TEST_DIFFUSE_TEXTURE, TEST_SPECULAR_TEXTURE, expectIblFactor);
  DALI_TEST_EQUALS(model.GetImageBasedLightScaleFactor(), expectIblFactor, TEST_LOCATION);

  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);
  application.GetScene().Add(model);

  application.SendNotification();
  application.Render();

  // Wait 3 task. (Load 1 model + Load 2 IBL)
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(model.GetImageBasedLightScaleFactor(), expectIblFactor, TEST_LOCATION);

  // Check uniform values before change material value
  Vector4 expectBaseColorFactor = Vector4(1.000f, 0.766f, 0.336f, 1.0f); // Defined at AnimatedCube.gltf
  float   expectTransformValid  = 0.0f;                                  ///< Note : This value will be true when gltf have BaseColorTexture, and use KHR_texture_transform extension.
  float   expectMaxLOD          = 5.0f;                                  ///< Note : The number of LOD what TEST_SPECULAR_TEXTURE file has is 5.

  tet_printf("Check uniform value result\n");
  DALI_TEST_EQUALS(gl.CheckUniformValue<Vector4>("uColorFactor", expectBaseColorFactor), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gl.CheckUniformValue<float>("uBaseColorTextureTransformAvailable", expectTransformValid), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gl.CheckUniformValue<float>(Scene3D::Loader::NodeDefinition::GetIblMaxLodUniformName().data(), expectMaxLOD), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gl.CheckUniformValue<float>(Scene3D::Loader::NodeDefinition::GetIblScaleFactorUniformName().data(), expectIblFactor), true, TEST_LOCATION);

  // Change all materials in Model.
  expectBaseColorFactor = Color::BLUE;

  Scene3D::ModelNode rootModelNode = model.GetModelRoot();
  DALI_TEST_CHECK(rootModelNode);
  ApplyAllMaterialPropertyRecursively(rootModelNode, {{Dali::Scene3D::Material::Property::BASE_COLOR_FACTOR, expectBaseColorFactor}});

  application.SendNotification();
  application.Render();

  tet_printf("Check whether uniform values are not changed instead what we change now\n");
  DALI_TEST_EQUALS(gl.CheckUniformValue<Vector4>("uColorFactor", expectBaseColorFactor), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gl.CheckUniformValue<float>("uBaseColorTextureTransformAvailable", expectTransformValid), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gl.CheckUniformValue<float>(Scene3D::Loader::NodeDefinition::GetIblMaxLodUniformName().data(), expectMaxLOD), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gl.CheckUniformValue<float>(Scene3D::Loader::NodeDefinition::GetIblScaleFactorUniformName().data(), expectIblFactor), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliModelCastShadow(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New();
  application.GetScene().Add(model);

  Scene3D::ModelNode modelNode = Scene3D::ModelNode::New();
  model.AddModelNode(modelNode);

  DALI_TEST_EQUALS(model.IsShadowCasting(), true, TEST_LOCATION);

  DALI_TEST_EQUALS(modelNode.IsShadowCasting(), true, TEST_LOCATION);

  auto shadowCastingIndex = modelNode.GetPropertyIndex("uIsShadowCasting");
  DALI_TEST_EQUALS(modelNode.GetProperty<int>(shadowCastingIndex), 1, TEST_LOCATION);

  model.CastShadow(false);

  DALI_TEST_EQUALS(model.IsShadowCasting(), false, TEST_LOCATION);

  DALI_TEST_EQUALS(modelNode.IsShadowCasting(), false, TEST_LOCATION);

  DALI_TEST_EQUALS(modelNode.GetProperty<int>(shadowCastingIndex), 0, TEST_LOCATION);

  Scene3D::ModelNode modelNode2 = Scene3D::ModelNode::New();
  model.AddModelNode(modelNode2);

  DALI_TEST_EQUALS(modelNode2.IsShadowCasting(), true, TEST_LOCATION);

  auto shadowCastingIndex2 = modelNode2.GetPropertyIndex("uIsShadowCasting");
  DALI_TEST_EQUALS(modelNode2.GetProperty<int>(shadowCastingIndex2), 1, TEST_LOCATION);

  modelNode.CastShadow(true);

  DALI_TEST_EQUALS(modelNode.IsShadowCasting(), true, TEST_LOCATION);

  DALI_TEST_EQUALS(model.IsShadowCasting(), false, TEST_LOCATION);

  model.CastShadow(false);

  DALI_TEST_EQUALS(model.IsShadowCasting(), false, TEST_LOCATION);

  DALI_TEST_EQUALS(modelNode.IsShadowCasting(), false, TEST_LOCATION);

  DALI_TEST_EQUALS(modelNode.GetProperty<int>(shadowCastingIndex), 0, TEST_LOCATION);

  DALI_TEST_EQUALS(modelNode2.GetProperty<int>(shadowCastingIndex2), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliModelReceiveShadow(void)
{
  ToolkitTestApplication application;

  Scene3D::Model model = Scene3D::Model::New();
  application.GetScene().Add(model);

  Scene3D::ModelNode modelNode = Scene3D::ModelNode::New();
  model.AddModelNode(modelNode);

  DALI_TEST_EQUALS(model.IsShadowReceiving(), true, TEST_LOCATION);

  DALI_TEST_EQUALS(modelNode.IsShadowReceiving(), true, TEST_LOCATION);

  auto shadowReceivingIndex = modelNode.GetPropertyIndex("uIsShadowReceiving");
  DALI_TEST_EQUALS(modelNode.GetProperty<int>(shadowReceivingIndex), 1, TEST_LOCATION);

  model.ReceiveShadow(false);

  DALI_TEST_EQUALS(model.IsShadowReceiving(), false, TEST_LOCATION);

  DALI_TEST_EQUALS(modelNode.IsShadowReceiving(), false, TEST_LOCATION);

  DALI_TEST_EQUALS(modelNode.GetProperty<int>(shadowReceivingIndex), 0, TEST_LOCATION);

  Scene3D::ModelNode modelNode2 = Scene3D::ModelNode::New();
  model.AddModelNode(modelNode2);

  DALI_TEST_EQUALS(modelNode2.IsShadowReceiving(), true, TEST_LOCATION);

  auto shadowReceivingIndex2 = modelNode2.GetPropertyIndex("uIsShadowReceiving");
  DALI_TEST_EQUALS(modelNode2.GetProperty<int>(shadowReceivingIndex2), 1, TEST_LOCATION);

  modelNode.ReceiveShadow(true);

  DALI_TEST_EQUALS(modelNode.IsShadowReceiving(), true, TEST_LOCATION);

  DALI_TEST_EQUALS(model.IsShadowReceiving(), false, TEST_LOCATION);

  model.ReceiveShadow(false);

  DALI_TEST_EQUALS(model.IsShadowReceiving(), false, TEST_LOCATION);

  DALI_TEST_EQUALS(modelNode.IsShadowReceiving(), false, TEST_LOCATION);

  DALI_TEST_EQUALS(modelNode.GetProperty<int>(shadowReceivingIndex), 0, TEST_LOCATION);

  DALI_TEST_EQUALS(modelNode2.GetProperty<int>(shadowReceivingIndex2), 0, TEST_LOCATION);
  END_TEST;
}