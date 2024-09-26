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

#include <dali-scene3d/public-api/loader/node-definition.h>
#include <dali-scene3d/public-api/loader/resource-bundle.h>
#include <dali-scene3d/public-api/loader/shader-manager.h>
#include <dali-test-suite-utils.h>
#include <set>
#include <string_view>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

namespace
{
struct Context
{
  ResourceBundle resources;
};

struct ShaderParameters
{
  MeshDefinition&     meshDefinition;
  MaterialDefinition& materialDefinition;
  NodeDefinition&     nodeDefinition;
};

struct Permutation
{
  using ConfigureFn = void (*)(ShaderParameters&);

  ConfigureFn configureFn;

  std::set<ShaderOption::Type> options;
  RendererState::Type          rendererStateSet   = 0;
  RendererState::Type          rendererStateClear = 0;
};

struct PermutationSet
{
  std::vector<const Permutation*> permutations;
};

} // namespace

int UtcDaliShaderManagerProduceShader(void)
{
  Context ctx;
  ctx.resources.mMaterials.push_back({});
  ctx.resources.mMeshes.push_back({});

  Permutation permutations[]{
    {
      //0
      [](ShaderParameters& p) {},
      {},
      RendererState::DEPTH_TEST | RendererState::CULL_BACK,
    },
    {
      //1
      [](ShaderParameters& p) {
        p.materialDefinition.mFlags |= MaterialDefinition::TRANSPARENCY;
      },
      {ShaderOption::Type::THREE_TEXTURE},
      RendererState::ALPHA_BLEND,
    },
    {//2
     [](ShaderParameters& p) {
       p.materialDefinition.mFlags |= MaterialDefinition::ALBEDO;
       p.materialDefinition.mTextureStages.push_back({MaterialDefinition::ALBEDO, {}});
     },
     {ShaderOption::Type::THREE_TEXTURE, ShaderOption::Type::BASE_COLOR_TEXTURE}},
    {//3
     [](ShaderParameters& p) {
       p.materialDefinition.mFlags |= MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS;
       p.materialDefinition.mTextureStages.push_back({MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS, {}});
     },
     {ShaderOption::Type::THREE_TEXTURE, ShaderOption::Type::METALLIC_ROUGHNESS_TEXTURE}},
    {//4
     [](ShaderParameters& p) {
       p.materialDefinition.mFlags |= MaterialDefinition::NORMAL;
       p.materialDefinition.mTextureStages.push_back({MaterialDefinition::NORMAL, {}});
     },
     {ShaderOption::Type::THREE_TEXTURE, ShaderOption::Type::NORMAL_TEXTURE}},
    {//5
     [](ShaderParameters& p) {
       p.materialDefinition.mFlags |= MaterialDefinition::SUBSURFACE;
     },
     {ShaderOption::Type::SUBSURFACE}},
    {//6
     [](ShaderParameters& p) {
       p.materialDefinition.SetAlphaCutoff(.5f);
     },
     {ShaderOption::Type::ALPHA_TEST}},
    {//7
     [](ShaderParameters& p) {
       p.materialDefinition.SetAlphaCutoff(1.f);
     },
     {ShaderOption::Type::ALPHA_TEST}},
    {//8
     [](ShaderParameters& p) {
       p.materialDefinition.mFlags |= MaterialDefinition::GLTF_CHANNELS;
     },
     {ShaderOption::Type::GLTF_CHANNELS}},
    {//9
     [](ShaderParameters& p) {
       p.meshDefinition.mJoints[0].mBlob.mOffset  = 0;
       p.meshDefinition.mWeights[0].mBlob.mOffset = 0;
     },
     {ShaderOption::Type::SKINNING}},
    {//10
     [](ShaderParameters& p) {
       p.meshDefinition.mFlags |= MeshDefinition::FLIP_UVS_VERTICAL;
     },
     {ShaderOption::Type::FLIP_UVS_VERTICAL}},
    {
      //11
      [](ShaderParameters& p) {
        p.meshDefinition.mBlendShapes.push_back({});
      },
    },
    {//12
     [](ShaderParameters& p) {
       p.meshDefinition.mBlendShapes.back().deltas.mBlob.mOffset = 0;
     },
     {ShaderOption::Type::MORPH_POSITION}},
    {//13
     [](ShaderParameters& p) {
       p.meshDefinition.mBlendShapes.back().normals.mBlob.mOffset = 0;
     },
     {ShaderOption::Type::MORPH_NORMAL}},
    {//14
     [](ShaderParameters& p) {
       p.meshDefinition.mBlendShapes.back().tangents.mBlob.mOffset = 0;
     },
     {ShaderOption::Type::MORPH_TANGENT}},
    {//15
     [](ShaderParameters& p) {
       auto& blendShapes = p.meshDefinition.mBlendShapes;
       DALI_ASSERT_ALWAYS(!blendShapes.empty() &&
                          (blendShapes.back().deltas.mBlob.mOffset != MeshDefinition::INVALID ||
                           blendShapes.back().normals.mBlob.mOffset != MeshDefinition::INVALID ||
                           blendShapes.back().tangents.mBlob.mOffset != MeshDefinition::INVALID));
       p.meshDefinition.mBlendShapeVersion = BlendShapes::Version::VERSION_2_0;
     },
     {ShaderOption::Type::MORPH_VERSION_2_0}},

    {//16
     [](ShaderParameters& p) {
       p.materialDefinition.mFlags |= MaterialDefinition::OCCLUSION;
     },
     {ShaderOption::Type::OCCLUSION}},

    {//17
     [](ShaderParameters& p) {
       p.meshDefinition.mColors[0].mBlob.mOffset = 0;
     },
     {ShaderOption::Type::COLOR_ATTRIBUTE}},
    {//18
     [](ShaderParameters& p) {
       p.meshDefinition.mTangentType = Property::VECTOR4;
     },
     {ShaderOption::Type::VEC4_TANGENT}},
  };

  PermutationSet permutationSets[]{
    // default
    {{&permutations[0]}},

    // alpha
    {{&permutations[0], &permutations[1]}},

    // three-texture setups
    {{&permutations[0], &permutations[2]}},
    {{&permutations[0], &permutations[3]}},
    {{&permutations[0], &permutations[4]}},
    {{&permutations[0], &permutations[2], &permutations[3]}},
    {{&permutations[0], &permutations[3], &permutations[4]}},
    {{&permutations[0], &permutations[4], &permutations[2]}},
    {{&permutations[0], &permutations[2], &permutations[3], &permutations[4]}},

    // subsurface scattering
    {{&permutations[0], &permutations[5]}},

    // alpha test
    {{&permutations[0], &permutations[6]}},
    {{&permutations[0], &permutations[7]}},

    // glTF channels
    {{&permutations[0], &permutations[8]}},

    // skinning
    {{&permutations[0], &permutations[9]}},

    // flip uvs
    {{&permutations[0], &permutations[10]}},

    // morphing
    {{&permutations[0], &permutations[11], &permutations[12]}},
    {{&permutations[0], &permutations[11], &permutations[13]}},
    {{&permutations[0], &permutations[11], &permutations[14]}},
    {{&permutations[0], &permutations[11], &permutations[12], &permutations[13]}},
    {{&permutations[0], &permutations[11], &permutations[13], &permutations[14]}},
    {{&permutations[0], &permutations[11], &permutations[14], &permutations[12]}},
    {{&permutations[0], &permutations[11], &permutations[12], &permutations[13], &permutations[14]}},

    {{&permutations[0], &permutations[11], &permutations[12], &permutations[15]}},
    {{&permutations[0], &permutations[11], &permutations[13], &permutations[15]}},
    {{&permutations[0], &permutations[11], &permutations[14], &permutations[15]}},
    {{&permutations[0], &permutations[11], &permutations[12], &permutations[13], &permutations[15]}},
    {{&permutations[0], &permutations[11], &permutations[13], &permutations[14], &permutations[15]}},
    {{&permutations[0], &permutations[11], &permutations[14], &permutations[12], &permutations[15]}},
    {{&permutations[0], &permutations[11], &permutations[12], &permutations[13], &permutations[14], &permutations[15]}},

    // etc.
    {{&permutations[0], &permutations[1], &permutations[2]}},
    {{&permutations[0], &permutations[1], &permutations[3]}},
    {{&permutations[0], &permutations[1], &permutations[2], &permutations[3]}},

    // occlusion
    {{&permutations[0], &permutations[16]}},
  };

  TestApplication app;
  ShaderManager   shaderManager;
  for(auto& permutationSet : permutationSets)
  {
    static int tc = 0;
    tet_printf("Test %d's tc\n", ++tc);
    auto modelRenderable          = new ModelRenderable();
    modelRenderable->mMeshIdx     = 0;
    modelRenderable->mMaterialIdx = 0;

    NodeDefinition                              nodeDefinition;
    std::unique_ptr<NodeDefinition::Renderable> renderable;
    renderable.reset(modelRenderable);
    nodeDefinition.mRenderables.push_back(std::move(renderable));

    MeshDefinition     meshDefinition;
    MaterialDefinition materialDefinition;
    ShaderParameters   shaderParameter{meshDefinition, materialDefinition, nodeDefinition};

    // Only define skinning accessors for skinning test...
    if(permutationSet.permutations.size() > 1)
    {
      auto& checkP = permutationSet.permutations[1];
      if(auto search = checkP->options.find(ShaderOption::Type::SKINNING);
         search != checkP->options.end())
      {
        meshDefinition.mJoints.push_back(MeshDefinition::Accessor{MeshDefinition::Blob{0, 0}, {}});
        meshDefinition.mWeights.push_back(MeshDefinition::Accessor{MeshDefinition::Blob{0, 0}, {}});
      }
    }
    std::set<std::string> defines;
    ShaderOption          option1;
    RendererState::Type   rendererState = 0;
    for(auto permutation : permutationSet.permutations)
    {
      permutation->configureFn(shaderParameter);
      if(materialDefinition.mFlags & MaterialDefinition::TRANSPARENCY)
      {
        option1.SetTransparency();
      }
      for(auto&& optionType : permutation->options)
      {
        option1.AddOption(optionType);
      }
      // No permutation uses extra joint weights.
      option1.AddJointMacros(0);
      rendererState = (rendererState | permutation->rendererStateSet) & ~permutation->rendererStateClear;
    }
    option1.AddOption(ShaderOption::Type::THREE_TEXTURE);

    ShaderOption option2 = shaderManager.ProduceShaderOption(materialDefinition, meshDefinition);

    Shader shaderFromOption1 = shaderManager.ProduceShader(option1);
    Shader shaderFromOption2 = shaderManager.ProduceShader(option2);
    DALI_TEST_EQUAL(option1.GetOptionHash(), option2.GetOptionHash());
    DALI_TEST_EQUAL(shaderFromOption1, shaderFromOption2);

    RendererState::Type rendererStateFromMaterialDefinition = shaderManager.GetRendererState(materialDefinition);
    DALI_TEST_EQUAL(rendererStateFromMaterialDefinition, rendererState);
  }

  END_TEST;
}

int UtcDaliShaderManagerAddAndRemoveLights(void)
{
  TestApplication app;
  ShaderManager   shaderManager;

  ShaderOption option1;
  option1.AddOption(ShaderOption::Type::THREE_TEXTURE);
  Dali::Shader shader1 = shaderManager.ProduceShader(option1);

  ShaderOption option2;
  option2.AddOption(ShaderOption::Type::BASE_COLOR_TEXTURE);
  Dali::Shader shader2 = shaderManager.ProduceShader(option2);

  DALI_TEST_NOT_EQUALS(shader1, shader2, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(shader1.GetProperty<int>(shader1.GetPropertyIndex("uLightCount")), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(shader2.GetProperty<int>(shader2.GetPropertyIndex("uLightCount")), 0, TEST_LOCATION);

  Scene3D::Light light = Scene3D::Light::New();
  shaderManager.AddLight(light);

  DALI_TEST_EQUALS(shader1.GetProperty<int>(shader1.GetPropertyIndex("uLightCount")), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(shader2.GetProperty<int>(shader2.GetPropertyIndex("uLightCount")), 1, TEST_LOCATION);

  ShaderOption option3;
  option3.AddOption(ShaderOption::Type::METALLIC_ROUGHNESS_TEXTURE);
  Dali::Shader shader3 = shaderManager.ProduceShader(option3);

  DALI_TEST_NOT_EQUALS(shader1, shader3, 0.1f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(shader2, shader3, 0.1f, TEST_LOCATION);

  DALI_TEST_EQUALS(shader3.GetProperty<int>(shader3.GetPropertyIndex("uLightCount")), 1, TEST_LOCATION);

  END_TEST;
}
