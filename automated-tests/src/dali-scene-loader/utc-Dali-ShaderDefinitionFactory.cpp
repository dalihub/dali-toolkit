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

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include <dali-test-suite-utils.h>
#include <set>
#include <string_view>
#include "dali-scene-loader/public-api/gltf2-loader.h"
#include "dali-scene-loader/public-api/node-definition.h"
#include "dali-scene-loader/public-api/resource-bundle.h"
#include "dali-scene-loader/public-api/shader-definition-factory.h"

using namespace Dali;
using namespace Dali::SceneLoader;

namespace
{
MaterialDefinition& NewMaterialDefinition(ResourceBundle& resources)
{
  resources.mMaterials.push_back({});
  return resources.mMaterials.back().first;
}

MeshDefinition& NewMeshDefinition(ResourceBundle& resources)
{
  resources.mMeshes.push_back({});
  return resources.mMeshes.back().first;
}

void ClearMeshesAndMaterials(ResourceBundle& resources)
{
  resources.mMaterials.clear();
  resources.mMeshes.clear();
}

struct Context
{
  ResourceBundle          resources;
  ShaderDefinitionFactory factory;

  Context()
  {
    factory.SetResources(resources);
  }
};

struct ShaderParameters
{
  MeshDefinition&     meshDef;
  MaterialDefinition& materialDef;
  NodeDefinition&     nodeDef;
};

struct Permutation
{
  using ConfigureFn = void (*)(ShaderParameters&);

  ConfigureFn configureFn;

  std::set<std::string> defines;
  RendererState::Type   rendererStateSet   = 0;
  RendererState::Type   rendererStateClear = 0;
};

struct PermutationSet
{
  std::vector<const Permutation*> permutations;
  Index                           shaderIdx;
};

} // namespace

int UtcDaliShaderDefinitionFactoryProduceShaderInvalid(void)
{
  Context ctx;

  NodeDefinition nodeDef;
  nodeDef.mRenderable.reset(new NodeDefinition::Renderable());

  DALI_TEST_EQUAL(INVALID_INDEX, ctx.factory.ProduceShader(nodeDef));
  DALI_TEST_CHECK(ctx.resources.mShaders.empty());

  END_TEST;
}

int UtcDaliShaderDefinitionFactoryProduceShader(void)
{
  Context ctx;
  ctx.resources.mMaterials.push_back({});
  ctx.resources.mMeshes.push_back({});

  Permutation permutations[]{
    {
      [](ShaderParameters& p) {},
      {"THREE_TEX"},
      RendererState::DEPTH_TEST | RendererState::DEPTH_WRITE | RendererState::CULL_BACK,
    },
    {
      [](ShaderParameters& p) {
        p.materialDef.mFlags |= MaterialDefinition::TRANSPARENCY;
      },
      {"THREE_TEX"},
      RendererState::ALPHA_BLEND,
      RendererState::DEPTH_WRITE,
    },
    {[](ShaderParameters& p) {
       p.materialDef.mTextureStages.push_back({MaterialDefinition::ALBEDO, {}});
     },
     {"THREE_TEX"}},
    {[](ShaderParameters& p) {
       p.materialDef.mTextureStages.push_back({MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS, {}});
     },
     {"THREE_TEX"}},
    {[](ShaderParameters& p) {
       p.materialDef.mTextureStages.push_back({MaterialDefinition::NORMAL, {}});
     },
     {"THREE_TEX"}},
    {[](ShaderParameters& p) {
       p.materialDef.mFlags |= MaterialDefinition::SUBSURFACE;
     },
     {"SSS"}},
    {[](ShaderParameters& p) {
       p.materialDef.SetAlphaCutoff(.5f);
     },
     {"ALPHA_TEST"}},
    {[](ShaderParameters& p) {
       p.materialDef.SetAlphaCutoff(1.f);
     },
     {"ALPHA_TEST"}},
    {[](ShaderParameters& p) {
       p.materialDef.mFlags |= MaterialDefinition::GLTF_CHANNELS;
     },
     {"GLTF_CHANNELS"}},
    {[](ShaderParameters& p) {
       p.meshDef.mJoints0.mBlob.mOffset  = 0;
       p.meshDef.mWeights0.mBlob.mOffset = 0;
     },
     {"SKINNING"}},
    {[](ShaderParameters& p) {
       p.meshDef.mFlags |= MeshDefinition::FLIP_UVS_VERTICAL;
     },
     {"FLIP_V"}},
    {
      [](ShaderParameters& p) {
        p.meshDef.mBlendShapes.push_back({});
      },
    },
    {[](ShaderParameters& p) {
       p.meshDef.mBlendShapes.back().deltas.mBlob.mOffset = 0;
     },
     {"MORPH_POSITION", "MORPH"}},
    {[](ShaderParameters& p) {
       p.meshDef.mBlendShapes.back().normals.mBlob.mOffset = 0;
     },
     {"MORPH_NORMAL", "MORPH"}},
    {[](ShaderParameters& p) {
       p.meshDef.mBlendShapes.back().tangents.mBlob.mOffset = 0;
     },
     {"MORPH_TANGENT", "MORPH"}},
    {[](ShaderParameters& p) {
       auto& blendShapes = p.meshDef.mBlendShapes;
       DALI_ASSERT_ALWAYS(!blendShapes.empty() &&
                          (blendShapes.back().deltas.mBlob.mOffset != MeshDefinition::INVALID ||
                           blendShapes.back().normals.mBlob.mOffset != MeshDefinition::INVALID ||
                           blendShapes.back().tangents.mBlob.mOffset != MeshDefinition::INVALID));
       p.meshDef.mBlendShapeVersion = BlendShapes::Version::VERSION_2_0;
     },
     {"MORPH_VERSION_2_0"}},

    {[](ShaderParameters& p) {
       p.materialDef.mFlags |= MaterialDefinition::OCCLUSION;
     },

     {"OCCLUSION"}},
  };

  PermutationSet permSets[]{
    // default
    {{&permutations[0]}, 0},

    // alpha
    {{&permutations[0], &permutations[1]}, 1},

    // three-texture setups
    {{&permutations[0], &permutations[2]}, 2},
    {{&permutations[0], &permutations[3]}, 2},
    {{&permutations[0], &permutations[4]}, 2},
    {{&permutations[0], &permutations[2], &permutations[3]}, 2},
    {{&permutations[0], &permutations[3], &permutations[4]}, 2},
    {{&permutations[0], &permutations[4], &permutations[2]}, 2},
    {{&permutations[0], &permutations[2], &permutations[3], &permutations[4]}, 2},

    // subsurface scattering
    {{&permutations[0], &permutations[5]}, 3},

    // alpha test
    {{&permutations[0], &permutations[6]}, 4},
    {{&permutations[0], &permutations[7]}, 4},

    // glTF channels
    {{&permutations[0], &permutations[8]}, 5},

    // skinning
    {{&permutations[0], &permutations[9]}, 6},

    // flip uvs
    {{&permutations[0], &permutations[10]}, 7},

    // morphing
    {{&permutations[0], &permutations[11], &permutations[12]}, 8},
    {{&permutations[0], &permutations[11], &permutations[13]}, 9},
    {{&permutations[0], &permutations[11], &permutations[14]}, 10},
    {{&permutations[0], &permutations[11], &permutations[12], &permutations[13]}, 11},
    {{&permutations[0], &permutations[11], &permutations[13], &permutations[14]}, 12},
    {{&permutations[0], &permutations[11], &permutations[14], &permutations[12]}, 13},
    {{&permutations[0], &permutations[11], &permutations[12], &permutations[13], &permutations[14]}, 14},

    {{&permutations[0], &permutations[11], &permutations[12], &permutations[15]}, 15},
    {{&permutations[0], &permutations[11], &permutations[13], &permutations[15]}, 16},
    {{&permutations[0], &permutations[11], &permutations[14], &permutations[15]}, 17},
    {{&permutations[0], &permutations[11], &permutations[12], &permutations[13], &permutations[15]}, 18},
    {{&permutations[0], &permutations[11], &permutations[13], &permutations[14], &permutations[15]}, 19},
    {{&permutations[0], &permutations[11], &permutations[14], &permutations[12], &permutations[15]}, 20},
    {{&permutations[0], &permutations[11], &permutations[12], &permutations[13], &permutations[14], &permutations[15]}, 21},

    // etc.
    {{&permutations[0], &permutations[1], &permutations[2]}, 1},
    {{&permutations[0], &permutations[1], &permutations[3]}, 1},
    {{&permutations[0], &permutations[1], &permutations[2], &permutations[3]}, 1},

    // occlusion
    {{&permutations[0], &permutations[16]}, 22},
  };

  for(auto& ps : permSets)
  {
    auto modelNode          = new ModelNode();
    modelNode->mMeshIdx     = 0;
    modelNode->mMaterialIdx = 0;

    NodeDefinition nodeDef;
    nodeDef.mRenderable.reset(modelNode);

    auto&            meshDef     = NewMeshDefinition(ctx.resources);
    auto&            materialDef = NewMaterialDefinition(ctx.resources);
    ShaderParameters sp{meshDef, materialDef, nodeDef};

    std::set<std::string> defines;
    RendererState::Type   rendererState = 0;
    for(auto p : ps.permutations)
    {
      p->configureFn(sp);
      defines.insert(p->defines.begin(), p->defines.end());
      rendererState = (rendererState | p->rendererStateSet) & ~p->rendererStateClear;
    }

    auto shaderIdx = ctx.factory.ProduceShader(nodeDef);
    DALI_TEST_EQUAL(ps.shaderIdx, shaderIdx);

    auto& shaderDef = ctx.resources.mShaders[shaderIdx].first;
    DALI_TEST_EQUAL(shaderDef.mRendererState, rendererState);

    uint32_t definesUnmatched = shaderDef.mDefines.size();
    for(auto& define : shaderDef.mDefines)
    {
      auto iFind = defines.find(define);
      if(iFind != defines.end())
      {
        defines.erase(iFind);
        --definesUnmatched;
      }
      else
      {
        break;
      }
    }

    DALI_TEST_CHECK(defines.empty());
    DALI_TEST_EQUAL(0, definesUnmatched);

    auto uMaxLOD = shaderDef.mUniforms["uMaxLOD"];
    DALI_TEST_EQUAL(uMaxLOD.GetType(), Property::FLOAT);

    auto uCubeMatrix = shaderDef.mUniforms["uCubeMatrix"];
    DALI_TEST_EQUAL(uCubeMatrix.GetType(), Property::MATRIX);

    ClearMeshesAndMaterials(ctx.resources);
  }

  END_TEST;
}
