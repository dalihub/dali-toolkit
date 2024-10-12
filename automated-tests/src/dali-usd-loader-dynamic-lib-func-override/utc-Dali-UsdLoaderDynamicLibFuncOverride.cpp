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
  ResourceBundle::PathProvider pathProvider = [](ResourceType::Value type) {
    return TEST_RESOURCE_DIR "/";
  };

  ResourceBundle  resources;
  SceneDefinition scene;
  SceneMetadata   metaData;

  std::vector<AnimationDefinition>      animations;
  std::vector<AnimationGroupDefinition> animationGroups;
  std::vector<CameraParameters>         cameras;
  std::vector<LightParameters>          lights;

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

  Context ctx;

  ctx.loader = new Dali::Scene3D::Loader::ModelLoader(TEST_RESOURCE_DIR "/usd/CesiumMan.usdz", ctx.pathProvider(ResourceType::Mesh) + "/", ctx.loadResult);
  DALI_TEST_EQUAL(ctx.loader->LoadModel(ctx.pathProvider, true), false);

  DALI_TEST_EQUAL(0, ctx.scene.GetRoots().size());
  DALI_TEST_EQUAL(0, ctx.scene.GetNodeCount());

  DALI_TEST_EQUAL(0, ctx.resources.mEnvironmentMaps.size());
  DALI_TEST_EQUAL(0, ctx.resources.mMaterials.size());
  DALI_TEST_EQUAL(0, ctx.resources.mMeshes.size());
  DALI_TEST_EQUAL(0, ctx.resources.mShaders.size());
  DALI_TEST_EQUAL(0, ctx.resources.mSkeletons.size());

  DALI_TEST_EQUAL(0, ctx.cameras.size());
  DALI_TEST_EQUAL(0, ctx.lights.size());
  DALI_TEST_EQUAL(0, ctx.animations.size());
  DALI_TEST_EQUAL(0, ctx.animationGroups.size());

  delete ctx.loader;

  END_TEST;
}

int UtcDaliUsdLoaderDlsymFail(void)
{
  // Only make dlsym fail
  gDlopenOverrideEnabled = false;
  gDlsymOverrideEnabled  = true;

  Context ctx;

  ctx.loader = new Dali::Scene3D::Loader::ModelLoader(TEST_RESOURCE_DIR "/usd/CesiumMan.usdz", ctx.pathProvider(ResourceType::Mesh) + "/", ctx.loadResult);
  DALI_TEST_EQUAL(ctx.loader->LoadModel(ctx.pathProvider, true), false);

  DALI_TEST_EQUAL(0, ctx.scene.GetRoots().size());
  DALI_TEST_EQUAL(0, ctx.scene.GetNodeCount());

  DALI_TEST_EQUAL(0, ctx.resources.mEnvironmentMaps.size());
  DALI_TEST_EQUAL(0, ctx.resources.mMaterials.size());
  DALI_TEST_EQUAL(0, ctx.resources.mMeshes.size());
  DALI_TEST_EQUAL(0, ctx.resources.mShaders.size());
  DALI_TEST_EQUAL(0, ctx.resources.mSkeletons.size());

  DALI_TEST_EQUAL(0, ctx.cameras.size());
  DALI_TEST_EQUAL(0, ctx.lights.size());
  DALI_TEST_EQUAL(0, ctx.animations.size());
  DALI_TEST_EQUAL(0, ctx.animationGroups.size());

  delete ctx.loader;

  END_TEST;
}
