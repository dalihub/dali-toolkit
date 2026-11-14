/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <cstdlib>

// INTERNAL INCLUDES
#include <dali-scene3d/integration-api/loader/environment-map-loader.h>
#include <dali-scene3d/internal/common/image-resource-loader.h>
#include <dali-scene3d/public-api/controls/model/model.h>
#include <dali-scene3d/public-api/model-components/material.h>
#include <dali-toolkit-test-suite-utils.h>
#include <toolkit-event-thread-callback.h>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

namespace
{
void SetGpuMemoryProfileEnvironment(const char* value)
{
#if defined(_MSC_VER)
  _putenv_s("DALI_GPU_MEMORY_PROFILE_ENABLED", value ? value : "");
#else
  if(value)
  {
    setenv("DALI_GPU_MEMORY_PROFILE_ENABLED", value, 1);
  }
  else
  {
    unsetenv("DALI_GPU_MEMORY_PROFILE_ENABLED");
  }
#endif
}
} // namespace

void scene3d_gpu_memory_profile_startup(void)
{
  test_return_value = TET_UNDEF;
  SetGpuMemoryProfileEnvironment("1");
}

void scene3d_gpu_memory_profile_cleanup(void)
{
  SetGpuMemoryProfileEnvironment(nullptr);
  test_return_value = TET_PASS;
}

int UtcDaliScene3DGpuMemoryProfileResources(void)
{
  ToolkitTestApplication application;

  EnvironmentMapData cubemap;
  DALI_TEST_CHECK(LoadEnvironmentMap(TEST_RESOURCE_DIR "/forest_radiance.ktx", cubemap));
  DALI_TEST_CHECK(cubemap.GetTexture());

  EnvironmentMapData equirectangular;
  DALI_TEST_CHECK(LoadEnvironmentMap(TEST_RESOURCE_DIR "/equirectangular.png", equirectangular));
  DALI_TEST_CHECK(equirectangular.GetTexture());

  Scene3D::Model model = Scene3D::Model::New(TEST_RESOURCE_DIR "/AnimatedCube.gltf");
  DALI_TEST_CHECK(model);
  application.GetScene().Add(model);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1));
  application.SendNotification();
  application.Render();

  Scene3D::Model morphModel = Scene3D::Model::New(TEST_RESOURCE_DIR "/AnimatedMorphCube.gltf");
  application.GetScene().Add(morphModel);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1));
  application.SendNotification();
  application.Render();

  Scene3D::Material material = Scene3D::Material::New();
  material.SetProperty(Scene3D::Material::Property::BASE_COLOR_URL, TEST_RESOURCE_DIR "/gallery-small-1.jpg");
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1));
  application.SendNotification();
  application.Render();

  application.GetScene().Remove(model);
  application.GetScene().Remove(morphModel);
  model.Reset();
  morphModel.Reset();
  application.GetPlatform().TriggerTimer();
  DALI_TEST_CHECK(true);
  END_TEST;
}
