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

#include <dali-test-suite-utils.h>
#include <toolkit-test-application.h>
#include <string_view>
#include "dali-scene3d/public-api/loader/node-definition.h"
#include "dali-scene3d/public-api/loader/view-projection.h"

using namespace Dali;
using namespace Dali::Scene3D::Loader;

namespace
{

struct Context
{
  ResourceBundle resources;

  ViewProjection viewProjection;
  Transforms     transforms{MatrixStack{}, viewProjection};

  NodeDefinition::CreateParams createParams{
    resources,
    transforms};
};

} // namespace

int UtcDaliConstraintDefinitionsCompare(void)
{
  ConstraintDefinition cd1{"orientation", 0};
  ConstraintDefinition cd2{"position", 1};

  DALI_TEST_CHECK(cd1 < cd2);
  DALI_TEST_CHECK(!(cd2 < cd1));
  DALI_TEST_CHECK(!(cd1 < cd1));
  DALI_TEST_CHECK(!(cd2 < cd2));

  DALI_TEST_CHECK(cd1 == cd1);
  DALI_TEST_CHECK(cd2 == cd2);

  ConstraintDefinition cd3{"position", 0};
  ConstraintDefinition cd4{"scale", 1};
  ConstraintDefinition cd5{"position", 1};
  DALI_TEST_CHECK(cd2 != cd3);
  DALI_TEST_CHECK(cd2 != cd4);
  DALI_TEST_CHECK(cd2 == cd5);
  DALI_TEST_CHECK(cd5 == cd2);

  END_TEST;
}

int UtcDaliBlendshapeShaderConfigurationRequestsCompare(void)
{
  TestApplication                      app;
  BlendshapeShaderConfigurationRequest bsscr1{"", 0, Shader(nullptr)};

  BlendshapeShaderConfigurationRequest bsscr2{"", 0, Shader::New("void main(){ gl_Position = vec4(0.); }", "void main(){ gl_FragColor = vec4(1.); }")};

  DALI_TEST_CHECK(bsscr1 < bsscr2);
  DALI_TEST_CHECK(!(bsscr2 < bsscr1));
  DALI_TEST_CHECK(!(bsscr1 < bsscr1));
  DALI_TEST_CHECK(!(bsscr2 < bsscr2));

  END_TEST;
}

int UtcDaliNodeDefinitionExtrasCompare(void)
{
  NodeDefinition::Extra e1{"alpha", Vector3::XAXIS * 2.f};
  NodeDefinition::Extra e2{"beta", 8};

  DALI_TEST_CHECK(e1 < e2);
  DALI_TEST_CHECK(!(e1 < e1));
  DALI_TEST_CHECK(!(e2 < e1));
  DALI_TEST_CHECK(!(e2 < e2));

  END_TEST;
}

int UtcDaliNodeDefinitionProperties(void)
{
  TestApplication testApp;
  NodeDefinition  nodeDef{
    "testRootNode",
    INVALID_INDEX,
    Vector3{-100.f, 100.f, -500.f},
    Quaternion{Radian(Degree(45.f)), Vector3::ZAXIS},
    Vector3{2.f, 4.f, 8.f},
    Vector3{100.f, 50.f, 25.f},
    false,
  };

  Quaternion frobnicateFactor(0.f, 1.f, 2.f, 3.f);
  frobnicateFactor.Normalize(); // because it will be (by DALi) once it's set as a property.
  nodeDef.mExtras.push_back(NodeDefinition::Extra{"frobnicateFactor", frobnicateFactor});

  Context ctx;
  auto    actor = nodeDef.CreateModelNode(ctx.createParams);
  DALI_TEST_EQUAL(nodeDef.mName, actor.GetProperty(Actor::Property::NAME).Get<std::string>());
  DALI_TEST_EQUAL(nodeDef.mPosition, actor.GetProperty(Actor::Property::POSITION).Get<Vector3>());
  DALI_TEST_EQUAL(nodeDef.mOrientation, actor.GetProperty(Actor::Property::ORIENTATION).Get<Quaternion>());
  DALI_TEST_EQUAL(nodeDef.mScale, actor.GetProperty(Actor::Property::SCALE).Get<Vector3>());
  DALI_TEST_EQUAL(nodeDef.mSize, actor.GetProperty(Actor::Property::SIZE).Get<Vector3>());
  DALI_TEST_EQUAL(nodeDef.mIsVisible, actor.GetProperty(Actor::Property::VISIBLE).Get<bool>());

  Property::Index propFrobnicateFactor = actor.GetPropertyIndex("frobnicateFactor");
  DALI_TEST_CHECK(propFrobnicateFactor != Property::INVALID_INDEX);

  auto frobnicateFactorValue = actor.GetProperty(propFrobnicateFactor);
  DALI_TEST_EQUAL(Property::ROTATION, frobnicateFactorValue.GetType());
  DALI_TEST_EQUAL(frobnicateFactorValue.Get<Quaternion>(), frobnicateFactor);

  DALI_TEST_EQUAL(0, actor.GetChildCount());
  DALI_TEST_EQUAL(0, actor.GetRendererCount());

  END_TEST;
}

int UtcDaliNodeDefinitionRenderableRegisterResources(void)
{
  NodeDefinition nodeDef;

  std::unique_ptr<NodeDefinition::Renderable> renderable = std::unique_ptr<NodeDefinition::Renderable>(new NodeDefinition::Renderable());
  nodeDef.mRenderables.push_back(std::move(renderable));
  nodeDef.mRenderables[0]->mShaderIdx = 0;

  struct : IResourceReceiver
  {
    std::vector<Index> shaders;
    uint32_t           otherResources = 0;

    void Register(ResourceType::Value type, Index id) override
    {
      switch(type)
      {
        case ResourceType::Shader:
          shaders.push_back(id);
          break;

        default:
          ++otherResources;
      }
    }
  } resourceReceiver;

  nodeDef.mRenderables[0]->RegisterResources(resourceReceiver);
  DALI_TEST_EQUAL(1u, resourceReceiver.shaders.size());
  DALI_TEST_EQUAL(0, resourceReceiver.shaders[0]);
  DALI_TEST_EQUAL(0, resourceReceiver.otherResources);

  END_TEST;
}

int UtcDaliNodeDefinitionRenderableReflectResources(void)
{
  NodeDefinition nodeDef;

  std::unique_ptr<NodeDefinition::Renderable> renderable = std::unique_ptr<NodeDefinition::Renderable>(new NodeDefinition::Renderable());
  nodeDef.mRenderables.push_back(std::move(renderable));
  nodeDef.mRenderables[0]->mShaderIdx = 0;

  struct : IResourceReflector
  {
    std::vector<Index*> shaders;
    uint32_t            otherResources = 0;

    void Reflect(ResourceType::Value type, Index& id) override
    {
      switch(type)
      {
        case ResourceType::Shader:
          shaders.push_back(&id);
          break;

        default:
          ++otherResources;
      }
    }
  } resourceReflector;

  nodeDef.mRenderables[0]->ReflectResources(resourceReflector);
  DALI_TEST_EQUAL(1u, resourceReflector.shaders.size());
  DALI_TEST_EQUAL(&nodeDef.mRenderables[0]->mShaderIdx, resourceReflector.shaders[0]);
  DALI_TEST_EQUAL(0, resourceReflector.otherResources);

  END_TEST;
}

int UtcDaliNodeDefinitionRenderable(void)
{
  TestApplication testApp;
  NodeDefinition  nodeDef;

  std::unique_ptr<NodeDefinition::Renderable> renderable = std::unique_ptr<NodeDefinition::Renderable>(new NodeDefinition::Renderable());
  nodeDef.mRenderables.push_back(std::move(renderable));
  nodeDef.mRenderables[0]->mShaderIdx = 0;

  Context    ctx;
  const auto VSH    = "void main() { gl_Position = vec4(0.); }";
  const auto FSH    = "void main() { gl_FragColor = vec4(1.); }";
  auto       shader = Shader::New(VSH, FSH);
  ctx.resources.mShaders.push_back({ShaderDefinition{}, shader});

  auto actor = nodeDef.CreateModelNode(ctx.createParams);
  DALI_TEST_EQUAL(1, actor.GetRendererCount());

  auto renderer = actor.GetRendererAt(0);
  DALI_TEST_EQUAL(renderer.GetShader(), shader);

  END_TEST;
}
