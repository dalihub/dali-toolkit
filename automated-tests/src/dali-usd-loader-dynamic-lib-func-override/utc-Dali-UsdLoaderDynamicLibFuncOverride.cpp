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
 *
 */

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/model-loader.h>
#include <dali-scene3d/public-api/loader/resource-bundle.h>
#include <dali-scene3d/public-api/loader/scene-definition.h>
#include <dali-test-suite-utils.h>
#include <dlfcn.h>
#include <string_view>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

namespace
{
struct Context
{
  ResourceBundle::PathProvider pathProvider = [](ResourceType::Value type)
  {
    return TEST_RESOURCE_DIR "/";
  };

  ResourceBundle  resources;
  SceneDefinition scene;
  SceneMetadata   metaData;

  Dali::Vector<AnimationDefinition>      animations;
  Dali::Vector<AnimationGroupDefinition> animationGroups;
  Dali::Vector<CameraParameters>         cameras;
  Dali::Vector<LightParameters>          lights;

  LoadResult loadResult{
    resources,
    scene,
    metaData,
    animations,
    animationGroups,
    cameras,
    lights};

  Dali::Scene3D::Loader::ModelLoader* loader;
};

bool gDlopenOverrideEnabled = false;
bool gDlsymOverrideEnabled  = false;

extern "C" void* DlopenProxy(const char* filename, int flag)
{
  if(gDlopenOverrideEnabled)
  {
    return nullptr;
  }
  else
  {
    return dlopen("libdali2-scene3d.so", RTLD_LAZY);
  }
}

extern "C" void* DlsymProxy(void* handle, const char* symbol)
{
  if(gDlsymOverrideEnabled)
  {
    return nullptr;
  }
  else
  {
    return dlsym(handle, symbol);
  }
}

} // namespace

int UtcDaliUsdLoaderDlopenFail(void)
{
  // Only make dlopen fail
  gDlopenOverrideEnabled = true;
  gDlsymOverrideEnabled  = false;

  Context     ctx;
  std::string model(TEST_RESOURCE_DIR "/usd/CesiumMan.usdz");
  std::string resourceProvider(ctx.pathProvider(ResourceType::Mesh) + "/");
  ctx.loader = new Dali::Scene3D::Loader::ModelLoader(String(model.data()), String(resourceProvider.data()), ctx.loadResult);
  DALI_TEST_EQUAL(ctx.loader->LoadModel(ctx.pathProvider, true), false);

  DALI_TEST_EQUAL(0, ctx.scene.GetRoots().Size());
  DALI_TEST_EQUAL(0, ctx.scene.GetNodeCount());

  DALI_TEST_EQUAL(0, ctx.resources.mEnvironmentMaps.Size());
  DALI_TEST_EQUAL(0, ctx.resources.mMaterials.Size());
  DALI_TEST_EQUAL(0, ctx.resources.mMeshes.Size());
  DALI_TEST_EQUAL(0, ctx.resources.mShaders.Size());
  DALI_TEST_EQUAL(0, ctx.resources.mSkeletons.Size());

  DALI_TEST_EQUAL(0, ctx.cameras.Size());
  DALI_TEST_EQUAL(0, ctx.lights.Size());
  DALI_TEST_EQUAL(0, ctx.animations.Size());
  DALI_TEST_EQUAL(0, ctx.animationGroups.Size());

  delete ctx.loader;

  END_TEST;
}

int UtcDaliUsdLoaderDlsymFail(void)
{
  // Only make dlsym fail
  gDlopenOverrideEnabled = false;
  gDlsymOverrideEnabled  = true;

  Context ctx;

  std::string model(TEST_RESOURCE_DIR "/usd/CesiumMan.usdz");
  std::string resourceProvider(ctx.pathProvider(ResourceType::Mesh) + "/");
  ctx.loader = new Dali::Scene3D::Loader::ModelLoader(String(model.data()), String(resourceProvider.data()), ctx.loadResult);

  DALI_TEST_EQUAL(ctx.loader->LoadModel(ctx.pathProvider, true), false);

  DALI_TEST_EQUAL(0, ctx.scene.GetRoots().Size());
  DALI_TEST_EQUAL(0, ctx.scene.GetNodeCount());

  DALI_TEST_EQUAL(0, ctx.resources.mEnvironmentMaps.Size());
  DALI_TEST_EQUAL(0, ctx.resources.mMaterials.Size());
  DALI_TEST_EQUAL(0, ctx.resources.mMeshes.Size());
  DALI_TEST_EQUAL(0, ctx.resources.mShaders.Size());
  DALI_TEST_EQUAL(0, ctx.resources.mSkeletons.Size());

  DALI_TEST_EQUAL(0, ctx.cameras.Size());
  DALI_TEST_EQUAL(0, ctx.lights.Size());
  DALI_TEST_EQUAL(0, ctx.animations.Size());
  DALI_TEST_EQUAL(0, ctx.animationGroups.Size());

  delete ctx.loader;

  END_TEST;
}
