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

#include <dali-scene3d/internal/loader/glb-loader-impl.h>
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/resource-bundle.h>
#include <dali-scene3d/public-api/loader/scene-definition.h>
#include <dali-scene3d/public-api/loader/shader-manager.h>
#include <dali-test-suite-utils.h>
#include <string_view>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

#define DALI_TEST_THROW(expression, exception, predicate) \
  {                                                       \
    bool daliTestThrowSuccess__ = false;                  \
    try                                                   \
    {                                                     \
      do                                                  \
      {                                                   \
        expression;                                       \
      } while(0);                                         \
      printf("No exception was thrown.\n");               \
    }                                                     \
    catch(std::decay<exception>::type & ex)               \
    {                                                     \
      daliTestThrowSuccess__ = predicate(ex);             \
    }                                                     \
    catch(...)                                            \
    {                                                     \
      printf("Wrong type of exception thrown.\n");        \
    }                                                     \
    DALI_TEST_CHECK(daliTestThrowSuccess__);              \
  }

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

  Dali::Scene3D::Loader::Internal::GlbLoaderImpl loader;
};

struct ExceptionMessageStartsWith
{
  const std::string_view expected;

  bool operator()(const std::runtime_error& e)
  {
    const bool success = (0 == strncmp(e.what(), expected.data(), expected.size()));
    if(!success)
    {
      printf("Expected: %s, got: %s.\n", expected.data(), e.what());
    }
    return success;
  }
};

} // namespace

int UtcDaliGlbLoaderFailedToLoad(void)
{
  Context ctx;

  DALI_TEST_EQUAL(ctx.loader.LoadModel("non-existent.glb", ctx.loadResult), false);

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

  END_TEST;
}

int UtcDaliGlbLoaderFailedToParse(void)
{
  Context ctx;
  DALI_TEST_EQUAL(ctx.loader.LoadModel(TEST_RESOURCE_DIR "/invalid.glb", ctx.loadResult), false);

  DALI_TEST_EQUAL(0, ctx.scene.GetRoots().size());
  DALI_TEST_EQUAL(0, ctx.scene.GetNodeCount());

  DALI_TEST_EQUAL(0, ctx.resources.mEnvironmentMaps.size());
  DALI_TEST_EQUAL(0, ctx.resources.mMaterials.size());
  DALI_TEST_EQUAL(0, ctx.resources.mMeshes.size());
  DALI_TEST_EQUAL(0, ctx.resources.mSkeletons.size());

  DALI_TEST_EQUAL(0, ctx.cameras.size());
  DALI_TEST_EQUAL(0, ctx.lights.size());
  DALI_TEST_EQUAL(0, ctx.animations.size());
  DALI_TEST_EQUAL(0, ctx.animationGroups.size());

  END_TEST;
}

int UtcDaliGlbLoaderSuccess1(void)
{
  Context                 ctx;
  ctx.loader.LoadModel(TEST_RESOURCE_DIR "/BoxAnimated.glb", ctx.loadResult);

  DALI_TEST_EQUAL(1u, ctx.scene.GetRoots().size());
  DALI_TEST_EQUAL(5u, ctx.scene.GetNodeCount());

  TestApplication app;

  Customization::Choices choices;
  for(auto iRoot : ctx.scene.GetRoots())
  {
    auto resourceRefs = ctx.resources.CreateRefCounter();
    ctx.scene.CountResourceRefs(iRoot, choices, resourceRefs);
    ctx.resources.mReferenceCounts = std::move(resourceRefs);
    ctx.resources.LoadResources(ctx.pathProvider);
  }

  DALI_TEST_EQUAL(true, ctx.resources.mMeshes[0u].first.mPositions.IsDefined());
  DALI_TEST_EQUAL(1152, ctx.resources.mMeshes[0u].first.mPositions.mBlob.mLength);

  END_TEST;
}
