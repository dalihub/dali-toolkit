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

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include <dali-scene3d/internal/common/image-resource-loader.h>
#include <dali-scene3d/internal/common/model-cache-manager.h>
#include <dali-scene3d/public-api/controls/model/model.h>
#include <dali-scene3d/public-api/loader/resource-bundle.h>
#include <dali-scene3d/public-api/loader/scene-definition.h>
#include <dali-toolkit-test-suite-utils.h>
#include <toolkit-event-thread-callback.h>
#include <toolkit-timer.h>
#include <string>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Scene3D::Internal;

namespace
{
/**
 * For the AnimatedCube.gltf and its Assets
 * Donated by Norbert Nopper for glTF testing.
 * Take from https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/AnimatedCube
 */
const char* TEST_GLTF_FILE_NAME = TEST_RESOURCE_DIR "/AnimatedCube.gltf";

static bool gResourceReadyCalled = false;

void OnResourceReady(Control control)
{
  gResourceReadyCalled = true;
}
} // namespace

int UtcDaliModelCacheManagerLoadModel(void)
{
  ToolkitTestApplication application;

  ModelCacheManager cacheManager = ModelCacheManager::Get();
  DALI_TEST_EQUALS(cacheManager.GetModelCacheRefCount(TEST_GLTF_FILE_NAME), 0u, TEST_LOCATION);

  // Load the first instance of the same model and add it to the scene
  Scene3D::Model model1 = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  application.GetScene().Add(model1);

  gResourceReadyCalled = false;
  model1.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  // Check that the loading has finished for mode1
  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  // Store the value of expect ref count with one model. Detail value could be changed with detail logic of cache.
  uint32_t refCountWithOneModel = cacheManager.GetModelCacheRefCount(TEST_GLTF_FILE_NAME);

  // Check whether model reference is greate or equal with 1.
  DALI_TEST_GREATER(refCountWithOneModel, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(cacheManager.IsSceneLoading(TEST_GLTF_FILE_NAME), false, TEST_LOCATION);
  DALI_TEST_EQUALS(cacheManager.IsSceneLoaded(TEST_GLTF_FILE_NAME), true, TEST_LOCATION);

  // Load the second instance of the same model and add it to the scene
  Scene3D::Model model2 = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  application.GetScene().Add(model2);

  gResourceReadyCalled = false;
  model2.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  // Check that the loading has finished for model2
  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  // Store the value of expect ref count with two models. Detail value could be changed with detail logic of cache.
  uint32_t refCountWithTwoModels = cacheManager.GetModelCacheRefCount(TEST_GLTF_FILE_NAME);

  // Check whether model reference is greate or equal with reference with one model.
  DALI_TEST_GREATER(refCountWithTwoModels, refCountWithOneModel, TEST_LOCATION);
  DALI_TEST_EQUALS(cacheManager.IsSceneLoading(TEST_GLTF_FILE_NAME), false, TEST_LOCATION);
  DALI_TEST_EQUALS(cacheManager.IsSceneLoaded(TEST_GLTF_FILE_NAME), true, TEST_LOCATION);

  Actor meshActor1 = model1.FindChildByName("AnimatedCube");
  Actor meshActor2 = model2.FindChildByName("AnimatedCube");
  DALI_TEST_CHECK(meshActor1);
  DALI_TEST_CHECK(meshActor2);

  Renderer renderer1 = meshActor1.GetRendererAt(0u);
  Renderer renderer2 = meshActor2.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer1);
  DALI_TEST_CHECK(renderer2);

  // Check that the two instances use the shared textures and geometries from the cache
  DALI_TEST_EQUALS(renderer1.GetTextures(), renderer2.GetTextures(), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer1.GetGeometry(), renderer2.GetGeometry(), TEST_LOCATION);

  // Destroy model1
  model1.Unparent();
  model1.Reset();

  application.SendNotification();
  application.Render();

  // Check that the reference count of the cmodel cache is decreased after model1 is destroyed
  DALI_TEST_EQUALS(cacheManager.GetModelCacheRefCount(TEST_GLTF_FILE_NAME), refCountWithOneModel, TEST_LOCATION);

  // Load another instance of the same model and add it to the scene
  Scene3D::Model model3 = Scene3D::Model::New(TEST_GLTF_FILE_NAME);
  application.GetScene().Add(model3);

  gResourceReadyCalled = false;
  model3.ResourceReadySignal().Connect(&OnResourceReady);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  // Check that the loading has finished for model3
  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  // Check the value of expect ref count with two models.
  DALI_TEST_EQUALS(cacheManager.GetModelCacheRefCount(TEST_GLTF_FILE_NAME), refCountWithTwoModels, TEST_LOCATION);
  DALI_TEST_EQUALS(cacheManager.IsSceneLoading(TEST_GLTF_FILE_NAME), false, TEST_LOCATION);
  DALI_TEST_EQUALS(cacheManager.IsSceneLoaded(TEST_GLTF_FILE_NAME), true, TEST_LOCATION);

  Actor meshActor3 = model3.FindChildByName("AnimatedCube");
  DALI_TEST_CHECK(meshActor3);

  Renderer renderer3 = meshActor3.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer3);

  // Check that model2 and model3 use the shared textures and geometries from the cache
  DALI_TEST_EQUALS(renderer2.GetTextures(), renderer3.GetTextures(), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer2.GetGeometry(), renderer3.GetGeometry(), TEST_LOCATION);

  // Destroy model2 and model3
  model2.Unparent();
  model2.Reset();

  model3.Unparent();
  model3.Reset();

  application.SendNotification();
  application.Render();

  // All reference count should be decreased.
  DALI_TEST_EQUALS(cacheManager.GetModelCacheRefCount(TEST_GLTF_FILE_NAME), 0u, TEST_LOCATION);

  // Collect garbages hardly.
  Dali::Scene3D::Internal::ImageResourceLoader::RequestGarbageCollect(true);
  Test::EmitGlobalTimerSignal();

  END_TEST;
}
