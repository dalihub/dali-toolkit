/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include "dali-scene-loader/public-api/dli-loader.h"
#include "dali-scene-loader/public-api/resource-bundle.h"
#include "dali-scene-loader/public-api/scene-definition.h"
#include "dali-scene-loader/public-api/load-result.h"
#include "dali-scene-loader/internal/json-util.h"
#include <dali-test-suite-utils.h>
#include <string_view>

using namespace Dali;
using namespace Dali::SceneLoader;

namespace
{

void ConfigureBlendShapeShaders(ResourceBundle& resources, const SceneDefinition& scene, Actor root,
  std::vector<BlendshapeShaderConfigurationRequest>&& requests)
{
  std::vector<std::string> errors;
  auto onError = [&errors](const std::string& msg) {
    errors.push_back(msg);
  };

  if (!scene.ConfigureBlendshapeShaders(resources, root, std::move(requests), onError))
  {
    ExceptionFlinger flinger(ASSERT_LOCATION);
    for (auto& msg : errors)
    {
      flinger << msg << '\n';
    }
  }
}

struct Context
{
  ResourceBundle::PathProvider pathProvider = [](ResourceType::Value type) {
    return TEST_RESOURCE_DIR "/";
  };

  ResourceBundle resources;
  SceneDefinition scene;
  std::vector<CameraParameters> cameraParameters;
  std::vector<LightParameters> lights;
  std::vector<AnimationDefinition> animations;
  std::vector<AnimationGroupDefinition> animGroups;

  LoadResult output {
    resources,
    scene,
    animations,
    animGroups,
    cameraParameters,
    lights
  };

  DliLoader::InputParams input {
    pathProvider(ResourceType::Mesh),
    nullptr,
    {},
    {},
    nullptr,
  };
  DliLoader::LoadParams loadParams{ input, output };

  std::vector<std::string> errors;
  DliLoader loader;

  StringCallback onError = [this](const std::string& error) {
    errors.push_back(error);
    printf("%s\n", error.c_str());
  };

  Context()
  {
    loader.SetErrorCallback(onError);
  }
};

bool StringHasTokens(const char* string, const std::vector<const char*>& tokens)
{
  for (auto& token: tokens)
  {
    auto result = strstr(string, token);
    if(nullptr == result)
    {
      return false;
    }
    string = result + strlen(token);
  }
  return true;
}

}

int UtcDaliDliLoaderLoadSceneNotFound(void)
{
  Context ctx;

  DALI_TEST_EQUAL(ctx.loader.LoadScene("does_not_exist.dli", ctx.loadParams), false);

  auto error = ctx.loader.GetParseError();
  DALI_TEST_CHECK(StringHasTokens(error.c_str(), { "Empty source buffer to parse." }));

  END_TEST;
}

int UtcDaliDliLoaderLoadSceneFailParse(void)
{
  Context ctx;

  auto path = ctx.pathProvider(ResourceType::Mesh) + "invalid.gltf";
  DALI_TEST_EQUAL(ctx.loader.LoadScene(path, ctx.loadParams), false);

  auto error = ctx.loader.GetParseError();
  DALI_TEST_CHECK(StringHasTokens(error.c_str(), { "Unexpected character." }));

  END_TEST;
}

int UtcDaliDliLoaderLoadSceneAssertions(void)
{
  const std::pair<std::string, std::string> pathExceptionPairs[] {
     // from RequireChild()
    { "scenes-nodes-missing", "Failed to find child node" },
    { "scenes-missing", "Failed to find child node" },
    { "nodes-missing", "Failed to find child node" },
    // from ParseSceneInternal()
    { "scene-out-of-bounds", "out of bounds" },
    { "nodes-invalid-type", "invalid type; array required" },
    { "nodes-array-empty", "must define a node id" },
    { "root-id-invalid", "invalid value for root node index" },
    { "root-id-out-of-bounds", "out of bounds" },
    { "root-node-invalid-type", "invalid JSON type; object required" },
    // from ParseSkeletons()
    { "skeleton-node-missing", "Missing required attribute" },
    { "skeleton-root-not-found", "not defined" },
    // from ParseShaders()
    { "shader-vertex-missing", "Missing vertex / fragment shader" },
    { "shader-fragment-missing", "Missing vertex / fragment shader" },
    // from ParseMeshes()
    { "mesh-uri-missing", "Missing required attribute" },
    { "mesh-indices-read-fail", "Failed to read indices" },
    { "mesh-positions-read-fail", "Failed to read positions" },
    // from ParseMaterials()
    { "material-environment-out-of-bounds", "out of bounds" },
    // from ParseNodes()
    { "node-model-mesh-missing", "Missing mesh" },
    { "node-arc-mesh-missing", "Missing mesh" },
    { "node-animated-image-mesh-missing", "Missing mesh" },
    { "node-renderable-mesh-invalid-type", "Invalid Mesh index type" },
    { "node-renderable-mesh-out-of-bounds", "out of bounds" },
    { "node-child-invalid-type", "invalid index type" },
    { "node-name-already-used", "name already used" },
    // from ParseAnimations()
    { "animation-failed-to-open", "Failed to open animation data" }
  };
  for (auto& i: pathExceptionPairs)
  {
    Context ctx;

    auto path = ctx.pathProvider(ResourceType::Mesh) + "dli/" + i.first + ".dli";
    printf("\n\n%s: %s\n", path.c_str(), i.second.c_str());
    DALI_TEST_ASSERTION(ctx.loader.LoadScene(path, ctx.loadParams), i.second.c_str());
  }

  END_TEST;
}

int UtcDaliDliLoaderLoadSceneExercise(void)
{
  Context ctx;

  auto path = ctx.pathProvider(ResourceType::Mesh) + "exercise.dli";
  DALI_TEST_CHECK(ctx.loader.LoadScene(path, ctx.loadParams));
  DALI_TEST_CHECK(ctx.errors.empty());

  auto& scene = ctx.scene;
  auto& roots = scene.GetRoots();
  DALI_TEST_EQUAL(roots.size(), 2u);
  DALI_TEST_EQUAL(scene.GetNode(roots[0])->mName, "Backdrop"); // default scene is scene 1 - this one.
  DALI_TEST_EQUAL(scene.GetNode(roots[1])->mName, "ExerciseDemo");

  DALI_TEST_EQUAL(scene.GetNodeCount(), 96u);

  auto& resources = ctx.resources;
  DALI_TEST_EQUAL(resources.mMeshes.size(), 11u);
  DALI_TEST_EQUAL(resources.mMaterials.size(), 13u);
  DALI_TEST_EQUAL(resources.mShaders.size(), 5u);
  DALI_TEST_EQUAL(resources.mEnvironmentMaps.size(), 2u);
  DALI_TEST_EQUAL(resources.mSkeletons.size(), 1u);

  DALI_TEST_EQUAL(ctx.cameraParameters.size(), 1u);
  DALI_TEST_EQUAL(ctx.lights.size(), 1u);
  DALI_TEST_EQUAL(ctx.animations.size(), 18u);
  DALI_TEST_EQUAL(ctx.animGroups.size(), 16u);

  ViewProjection viewProjection;
  Transforms xforms {
    MatrixStack{},
    viewProjection
  };
  NodeDefinition::CreateParams nodeParams{
    resources,
    xforms,
  };

  Customization::Choices choices;

  TestApplication app;

  Actor root = Actor::New();
  SetActorCentered(root);
  for (auto iRoot : scene.GetRoots())
  {
    auto resourceRefs = resources.CreateRefCounter();
    scene.CountResourceRefs(iRoot, choices, resourceRefs);
    resources.CountEnvironmentReferences(resourceRefs);
    resources.LoadResources(resourceRefs, ctx.pathProvider);
    if (auto actor = scene.CreateNodes(iRoot, choices, nodeParams))
    {
      scene.ConfigureSkeletonJoints(iRoot, resources.mSkeletons, actor);
      scene.ConfigureSkinningShaders(resources, actor, std::move(nodeParams.mSkinnables));
      ConfigureBlendShapeShaders(resources, scene, actor, std::move(nodeParams.mBlendshapeRequests));
      scene.ApplyConstraints(actor, std::move(nodeParams.mConstrainables));
      root.Add(actor);
    }
  }

  DALI_TEST_EQUAL(root.GetChildCount(), 2u);
  DALI_TEST_EQUAL(root.GetChildAt(0).GetProperty(Actor::Property::NAME).Get<std::string>(), "Backdrop");
  DALI_TEST_EQUAL(root.GetChildAt(1).GetProperty(Actor::Property::NAME).Get<std::string>(), "ExerciseDemo");

  END_TEST;
}

int UtcDaliDliLoaderLoadSceneMorph(void)
{
  Context ctx;

  std::vector<std::string> metadata;
  uint32_t metadataCount = 0;
  ctx.input.mPreNodeCategoryProcessors.push_back({ "metadata",
    [&](const Property::Array& array, StringCallback) {
      std::string key, value;
      for (uint32_t i0 = 0, i1 = array.Count(); i0 < i1; ++i0)
      {
        auto& data = array.GetElementAt(i0);
        DALI_TEST_EQUAL(data.GetType(), Property::MAP);

        auto map = data.GetMap();
        auto key = map->Find("key");
        auto value = map->Find("value");
        DALI_TEST_EQUAL(key->GetType(), Property::STRING);
        DALI_TEST_EQUAL(value->GetType(), Property::STRING);
        metadata.push_back(key->Get<std::string>() + ":" + value->Get<std::string>());

        ++metadataCount;
      }
    }
  });

  std::vector<std::string> behaviors;
  uint32_t behaviorCount = 0;
  ctx.input.mPostNodeCategoryProcessors.push_back({ "behaviors",
    [&](const Property::Array& array, StringCallback) {
      for (uint32_t i0 = 0, i1 = array.Count(); i0 < i1; ++i0)
      {
        auto& data = array.GetElementAt(i0);
        DALI_TEST_EQUAL(data.GetType(), Property::MAP);

        auto map = data.GetMap();
        auto event = map->Find("event");
        auto url = map->Find("url");
        DALI_TEST_EQUAL(event->GetType(), Property::STRING);
        DALI_TEST_EQUAL(url->GetType(), Property::STRING);
        behaviors.push_back(event->Get<std::string>() + ":" + url->Get<std::string>());

        ++behaviorCount;
      }
    }
  });

  size_t numNodes = 0;
  ctx.input.mNodePropertyProcessor = [&](const NodeDefinition&, const Property::Map&, StringCallback) {
    ++numNodes;
  };

  auto path = ctx.pathProvider(ResourceType::Mesh) + "morph.dli";
  DALI_TEST_CHECK(ctx.loader.LoadScene(path, ctx.loadParams));
  DALI_TEST_CHECK(ctx.errors.empty());

  auto& scene = ctx.scene;
  auto& roots = scene.GetRoots();
  DALI_TEST_EQUAL(roots.size(), 1u);
  DALI_TEST_EQUAL(scene.GetNode(roots[0])->mName, "HeadTest_002");

  DALI_TEST_EQUAL(numNodes, 3u);
  DALI_TEST_EQUAL(scene.GetNodeCount(), numNodes);

  auto& resources = ctx.resources;
  DALI_TEST_EQUAL(resources.mMeshes.size(), 2u);
  DALI_TEST_EQUAL(resources.mMaterials.size(), 1u);
  DALI_TEST_EQUAL(resources.mShaders.size(), 5u);
  DALI_TEST_EQUAL(resources.mEnvironmentMaps.size(), 2u);
  DALI_TEST_EQUAL(resources.mSkeletons.size(), 0u);

  DALI_TEST_EQUAL(ctx.cameraParameters.size(), 1u);
  DALI_TEST_EQUAL(ctx.lights.size(), 1u);
  DALI_TEST_EQUAL(ctx.animations.size(), 1u);
  DALI_TEST_EQUAL(ctx.animGroups.size(), 0u);

  DALI_TEST_EQUAL(metadata.size(), 4u);
  DALI_TEST_EQUAL(behaviors.size(), 1u);

  ViewProjection viewProjection;
  Transforms xforms {
    MatrixStack{},
    viewProjection
  };
  NodeDefinition::CreateParams nodeParams{
    resources,
    xforms,
  };

  Customization::Choices choices;

  TestApplication app;

  Actor root = Actor::New();
  SetActorCentered(root);
  for (auto iRoot : scene.GetRoots())
  {
    auto resourceRefs = resources.CreateRefCounter();
    scene.CountResourceRefs(iRoot, choices, resourceRefs);
    resources.CountEnvironmentReferences(resourceRefs);
    resources.LoadResources(resourceRefs, ctx.pathProvider);
    if (auto actor = scene.CreateNodes(iRoot, choices, nodeParams))
    {
      scene.ConfigureSkeletonJoints(iRoot, resources.mSkeletons, actor);
      scene.ConfigureSkinningShaders(resources, actor, std::move(nodeParams.mSkinnables));
      ConfigureBlendShapeShaders(resources, scene, actor, std::move(nodeParams.mBlendshapeRequests));
      scene.ApplyConstraints(actor, std::move(nodeParams.mConstrainables));
      root.Add(actor);
    }
  }

  DALI_TEST_EQUAL(root.GetChildCount(), 1u);
  DALI_TEST_EQUAL(root.GetChildAt(0).GetProperty(Actor::Property::NAME).Get<std::string>(), "HeadTest_002");

  END_TEST;
}

int UtcDaliDliLoaderLoadSceneArc(void)
{
  Context ctx;

  auto path = ctx.pathProvider(ResourceType::Mesh) + "arc.dli";
  DALI_TEST_CHECK(ctx.loader.LoadScene(path, ctx.loadParams));
  DALI_TEST_CHECK(ctx.errors.empty());

  auto& scene = ctx.scene;
  auto& roots = scene.GetRoots();
  DALI_TEST_EQUAL(roots.size(), 1u);
  DALI_TEST_EQUAL(scene.GetNode(roots[0])->mName, "root");

  DALI_TEST_EQUAL(scene.GetNodeCount(), 2u);

  auto& resources = ctx.resources;
  DALI_TEST_EQUAL(resources.mMeshes.size(), 1u);
  DALI_TEST_EQUAL(resources.mMaterials.size(), 1u);
  DALI_TEST_EQUAL(resources.mShaders.size(), 1u);
  DALI_TEST_EQUAL(resources.mEnvironmentMaps.size(), 1u);
  DALI_TEST_EQUAL(resources.mSkeletons.size(), 0u);

  DALI_TEST_EQUAL(ctx.cameraParameters.size(), 0u);
  DALI_TEST_EQUAL(ctx.lights.size(), 0u);
  DALI_TEST_EQUAL(ctx.animations.size(), 0u);
  DALI_TEST_EQUAL(ctx.animGroups.size(), 0u);

  ViewProjection viewProjection;
  Transforms xforms {
    MatrixStack{},
    viewProjection
  };
  NodeDefinition::CreateParams nodeParams{
    resources,
    xforms,
  };

  Customization::Choices choices;

  TestApplication app;

  Actor root = Actor::New();
  SetActorCentered(root);
  for (auto iRoot : scene.GetRoots())
  {
    auto resourceRefs = resources.CreateRefCounter();
    scene.CountResourceRefs(iRoot, choices, resourceRefs);
    resources.CountEnvironmentReferences(resourceRefs);
    resources.LoadResources(resourceRefs, ctx.pathProvider);
    if (auto actor = scene.CreateNodes(iRoot, choices, nodeParams))
    {
      scene.ConfigureSkeletonJoints(iRoot, resources.mSkeletons, actor);
      scene.ConfigureSkinningShaders(resources, actor, std::move(nodeParams.mSkinnables));
      ConfigureBlendShapeShaders(resources, scene, actor, std::move(nodeParams.mBlendshapeRequests));
      scene.ApplyConstraints(actor, std::move(nodeParams.mConstrainables));
      root.Add(actor);
    }
  }

  DALI_TEST_EQUAL(root.GetChildCount(), 1u);
  DALI_TEST_EQUAL(root.GetChildAt(0).GetProperty(Actor::Property::NAME).Get<std::string>(), "root");

  END_TEST;
}

int UtcDaliDliLoaderLoadSceneShaderUniforms(void)
{
  Context ctx;

  auto path = ctx.pathProvider(ResourceType::Mesh) + "dli/shader-uniforms.dli";
  DALI_TEST_CHECK(ctx.loader.LoadScene(path, ctx.loadParams));
  DALI_TEST_EQUAL(ctx.errors.size(), 1u);
  DALI_TEST_CHECK(ctx.errors[0].find("failed to infer type") != std::string::npos);

  auto& scene = ctx.scene;
  auto& roots = scene.GetRoots();
  DALI_TEST_EQUAL(roots.size(), 1u);
  DALI_TEST_EQUAL(scene.GetNode(roots[0])->mName, "root");

  DALI_TEST_EQUAL(scene.GetNodeCount(), 1u);

  auto& resources = ctx.resources;
  DALI_TEST_EQUAL(resources.mMeshes.size(), 0u);
  DALI_TEST_EQUAL(resources.mMaterials.size(), 0u);
  DALI_TEST_EQUAL(resources.mShaders.size(), 1u);
  DALI_TEST_EQUAL(resources.mEnvironmentMaps.size(), 0u);
  DALI_TEST_EQUAL(resources.mSkeletons.size(), 0u);

  auto raw = resources.mShaders[0].first.LoadRaw(ctx.pathProvider(ResourceType::Shader));

  TestApplication app;

  auto shader = resources.mShaders[0].first.Load(std::move(raw));
  DALI_TEST_EQUAL(shader.GetProperty(shader.GetPropertyIndex("uBool")).Get<float>(), 1.0f);
  DALI_TEST_EQUAL(shader.GetProperty(shader.GetPropertyIndex("uInt")).Get<float>(), 255.0f);
  DALI_TEST_EQUAL(shader.GetProperty(shader.GetPropertyIndex("uFloat")).Get<float>(), -0.5f);
  DALI_TEST_EQUAL(shader.GetProperty(shader.GetPropertyIndex("uVec2")).Get<Vector2>(), Vector2(100.0f, -100.0f));
  DALI_TEST_EQUAL(shader.GetProperty(shader.GetPropertyIndex("uVec3")).Get<Vector3>(), Vector3(50.0f, 0.f, -200.0f));
  DALI_TEST_EQUAL(shader.GetProperty(shader.GetPropertyIndex("uVec4")).Get<Vector4>(), Vector4(0.1774f, 1.0f, 0.5333f, 0.7997f));
  DALI_TEST_EQUAL(shader.GetProperty(shader.GetPropertyIndex("uMat3")).Get<Matrix3>(), Matrix3(9.0f, 8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f));

  Matrix expectedMatrix;
  expectedMatrix.SetTransformComponents(Vector3::ONE * 8.0, Quaternion::IDENTITY, Vector3::ZERO);
  DALI_TEST_EQUAL(shader.GetProperty(shader.GetPropertyIndex("uMat4")).Get<Matrix>(), expectedMatrix);

  END_TEST;
}

int UtcDaliDliLoaderLoadSceneExtras(void)
{
  Context ctx;

  auto path = ctx.pathProvider(ResourceType::Mesh) + "dli/extras.dli";
  DALI_TEST_CHECK(ctx.loader.LoadScene(path, ctx.loadParams));
  DALI_TEST_EQUAL(ctx.errors.size(), 3u);
  DALI_TEST_CHECK(ctx.errors[0].find("already defined; overriding") != std::string::npos);
  DALI_TEST_CHECK(ctx.errors[1].find("empty string is invalid for name") != std::string::npos);
  DALI_TEST_CHECK(ctx.errors[2].find("failed to interpret value") != std::string::npos);

  auto& scene = ctx.scene;
  auto& roots = scene.GetRoots();
  DALI_TEST_EQUAL(roots.size(), 1u);
  DALI_TEST_EQUAL(scene.GetNode(roots[0])->mName, "root");

  DALI_TEST_EQUAL(scene.GetNodeCount(), 1u);

  ViewProjection viewProjection;
  Transforms xforms {
    MatrixStack{},
    viewProjection
  };
  auto& resources = ctx.resources;
  NodeDefinition::CreateParams nodeParams{
    resources,
    xforms,
  };

  Customization::Choices choices;

  TestApplication app;
  Actor actor = scene.CreateNodes(0, choices, nodeParams);

  DALI_TEST_EQUAL(actor.GetProperty(actor.GetPropertyIndex("fudgeFactor")).Get<float>(), 9000.1f);
  DALI_TEST_EQUAL(actor.GetProperty(actor.GetPropertyIndex("fudgeVector")).Get<Vector2>(), Vector2(-.25f, 17.f));
  DALI_TEST_EQUAL(actor.GetProperty(actor.GetPropertyIndex("isThisTheRealLife")).Get<bool>(), true);
  DALI_TEST_EQUAL(actor.GetProperty(actor.GetPropertyIndex("isThisJustFantasy")).Get<bool>(), false);
  DALI_TEST_EQUAL(actor.GetProperty(actor.GetPropertyIndex("velocity")).Get<Vector3>(), Vector3(.1f, 58.f, -.2f));
  DALI_TEST_EQUAL(actor.GetProperty(actor.GetPropertyIndex("frameOfReference")).Get<Matrix>(), Matrix::IDENTITY);

  END_TEST;
}

int UtcDaliDliLoaderLoadSceneConstraints(void)
{
  Context ctx;

  auto path = ctx.pathProvider(ResourceType::Mesh) + "dli/constraints.dli";
  DALI_TEST_CHECK(ctx.loader.LoadScene(path, ctx.loadParams));
  DALI_TEST_EQUAL(ctx.errors.size(), 1u);
  DALI_TEST_CHECK(ctx.errors[0].find("invalid", ctx.errors[0].find("node ID")) != std::string::npos);

  auto& scene = ctx.scene;
  auto& roots = scene.GetRoots();
  DALI_TEST_EQUAL(roots.size(), 1u);
  DALI_TEST_EQUAL(scene.GetNode(0)->mName, "root");
  DALI_TEST_EQUAL(scene.GetNode(1)->mName, "Alice");
  DALI_TEST_EQUAL(scene.GetNode(2)->mName, "Bob");
  DALI_TEST_EQUAL(scene.GetNode(3)->mName, "Charlie");

  DALI_TEST_EQUAL(scene.GetNodeCount(), 4u);

  ViewProjection viewProjection;
  Transforms xforms {
    MatrixStack{},
    viewProjection
  };
  auto& resources = ctx.resources;
  NodeDefinition::CreateParams nodeParams{
    resources,
    xforms,
  };

  Customization::Choices choices;

  TestApplication app;

  Actor root = scene.CreateNodes(0, choices, nodeParams);
  Actor alice = root.FindChildByName("Alice");
  Actor bob = root.FindChildByName("Bob");
  Actor charlie = root.FindChildByName("Charlie");

  DALI_TEST_EQUAL(nodeParams.mConstrainables.size(), 3u);
  DALI_TEST_EQUAL(bob.GetProperty(bob.GetPropertyIndex("angularVelocity")).Get<Vector2>(), Vector2(-0.5, 0.0004));

  ctx.errors.clear();
  scene.ApplyConstraints(root, std::move(nodeParams.mConstrainables), ctx.onError);
  DALI_TEST_CHECK(ctx.errors.empty());

  app.GetScene().Add(root);
  app.SendNotification();
  app.Render();
  app.SendNotification();
  app.Render();

  DALI_TEST_EQUAL(charlie.GetCurrentProperty(Actor::Property::ORIENTATION), alice.GetProperty(Actor::Property::ORIENTATION));
  DALI_TEST_EQUAL(charlie.GetCurrentProperty(Actor::Property::POSITION), bob.GetProperty(Actor::Property::POSITION));
  DALI_TEST_EQUAL(charlie.GetCurrentProperty(charlie.GetPropertyIndex("angularVelocity")), bob.GetProperty(bob.GetPropertyIndex("angularVelocity")));

  END_TEST;
}

int UtcDaliDliLoaderNodeProcessor(void)
{
  Context ctx;

  std::vector<Property::Map> nodeMaps;
  ctx.input.mNodePropertyProcessor = [&](const NodeDefinition&, Property::Map&& map, StringCallback) {
    nodeMaps.push_back(map);
  };

  auto path = ctx.pathProvider(ResourceType::Mesh) + "dli/node-processor.dli";
  DALI_TEST_CHECK(ctx.loader.LoadScene(path, ctx.loadParams));

  DALI_TEST_EQUAL(nodeMaps.size(), 2u);
  DALI_TEST_EQUAL(nodeMaps[0].Count(), 5u);
  DALI_TEST_EQUAL(nodeMaps[0].Find("name")->Get<std::string>(), "rootA");
  DALI_TEST_EQUAL(nodeMaps[0].Find("nickname")->Get<std::string>(), "same as name");
  DALI_TEST_EQUAL(nodeMaps[0].Find("favourite number")->Get<int32_t>(), 63478);

  auto propArray = nodeMaps[0].Find("array");
  DALI_TEST_EQUAL(propArray->GetType(), Property::ARRAY);

  auto array = propArray->GetArray();
  DALI_TEST_EQUAL(array->Count(), 5);
  DALI_TEST_EQUAL(array->GetElementAt(0).Get<int32_t>(), 1);
  DALI_TEST_EQUAL(array->GetElementAt(1).Get<int32_t>(), 2);
  DALI_TEST_EQUAL(array->GetElementAt(2).Get<int32_t>(), 4);
  DALI_TEST_EQUAL(array->GetElementAt(3).Get<int32_t>(), 8);
  DALI_TEST_EQUAL(array->GetElementAt(4).Get<int32_t>(), -500);

  auto propObject = nodeMaps[0].Find("object");
  DALI_TEST_EQUAL(propObject->GetType(), Property::MAP);

  auto object = propObject->GetMap();
  DALI_TEST_EQUAL(object->Count(), 5);
  DALI_TEST_EQUAL(object->Find("physics")->Get<bool>(), true);
  DALI_TEST_EQUAL(object->Find("elasticity")->Get<float>(), .27f);
  DALI_TEST_EQUAL(object->Find("drag")->Get<float>(), .91f);

  auto propInnerArray = object->Find("inner array");
  DALI_TEST_EQUAL(propInnerArray->GetType(), Property::ARRAY);

  auto innerArray = propInnerArray->GetArray();
  DALI_TEST_EQUAL(innerArray->Count(), 3);
  DALI_TEST_EQUAL(innerArray->GetElementAt(0).Get<std::string>(), "why");
  DALI_TEST_EQUAL(innerArray->GetElementAt(1).Get<std::string>(), "not");
  DALI_TEST_EQUAL(innerArray->GetElementAt(2).Get<bool>(), false);

  auto propInnerObject = object->Find("inner object");
  DALI_TEST_EQUAL(propInnerObject->GetType(), Property::MAP);

  auto innerObject = propInnerObject->GetMap();
  DALI_TEST_EQUAL(innerObject->Count(), 1);
  DALI_TEST_EQUAL(innerObject->Find("supported")->Get<bool>(), true);

  DALI_TEST_EQUAL(nodeMaps[1].Count(), 1u);
  DALI_TEST_EQUAL(nodeMaps[1].Find("name")->Get<std::string>(), "rootB");

  END_TEST;
}
