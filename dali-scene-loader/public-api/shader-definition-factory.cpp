/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include "dali-scene-loader/public-api/shader-definition-factory.h"
#include <cstring>
#include "dali-scene-loader/internal/hash.h"
#include "dali-scene-loader/public-api/blend-shape-details.h"
#include "dali-scene-loader/public-api/node-definition.h"
#include "dali/devel-api/common/map-wrapper.h"

namespace Dali
{
namespace SceneLoader
{
namespace
{
struct ResourceReceiver : IResourceReceiver
{
  const ResourceBundle&     mResources;
  const MeshDefinition*     mMeshDef     = nullptr;
  const MaterialDefinition* mMaterialDef = nullptr;

  ResourceReceiver(const ResourceBundle& resources)
  : mResources(resources)
  {
  }

  void Register(ResourceType::Value type, Index id) override
  {
    switch(type)
    {
      case ResourceType::Mesh:
        mMeshDef = &mResources.mMeshes[id].first;
        break;

      case ResourceType::Material:
        mMaterialDef = &mResources.mMaterials[id].first;
        break;

      default:
        break;
    }
  }
};

const std::string PBR_SHADER_NAME = "dli_pbr";

void RetrieveBlendShapeComponents(const std::vector<MeshDefinition::BlendShape>& blendShapes, bool& hasPositions, bool& hasNormals, bool& hasTangents)
{
  for(const auto& blendShape : blendShapes)
  {
    hasPositions = hasPositions || blendShape.deltas.IsDefined();
    hasNormals   = hasNormals || blendShape.normals.IsDefined();
    hasTangents  = hasTangents || blendShape.tangents.IsDefined();
  }
}

uint64_t HashNode(const NodeDefinition& nodeDef, const MaterialDefinition& materialDef, const MeshDefinition& meshDef)
{
  Hash hash;

  // note: could be per vertex / fragment component - in WatchViewer, these have the same name.
  hash.Add(PBR_SHADER_NAME);

  const bool hasTransparency = MaskMatch(materialDef.mFlags, MaterialDefinition::TRANSPARENCY);
  hash.Add(hasTransparency);

  if(hasTransparency ||
     materialDef.CheckTextures(MaterialDefinition::ALBEDO) ||
     materialDef.CheckTextures(MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS) ||
     materialDef.CheckTextures(MaterialDefinition::NORMAL))
  {
    hash.Add("3TEX");
  }

  if(materialDef.GetAlphaCutoff() > 0.f)
  {
    hash.Add("ALPH" /*A_TEST*/);
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::SUBSURFACE))
  {
    hash.Add("SSS");
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::GLTF_CHANNELS))
  {
    hash.Add("GLTF" /*_CHANNELS*/);
  }

  if(meshDef.IsSkinned())
  {
    hash.Add("SKIN" /*NING*/);
  }

  if(MaskMatch(meshDef.mFlags, MeshDefinition::FLIP_UVS_VERTICAL))
  {
    hash.Add("FLIP" /*_V*/);
  }

  if(meshDef.HasBlendShapes())
  {
    bool hasPositions = false;
    bool hasNormals   = false;
    bool hasTangents  = false;
    RetrieveBlendShapeComponents(meshDef.mBlendShapes, hasPositions, hasNormals, hasTangents);
    if(hasPositions)
    {
      hash.Add("MORPHPOS");
    }

    if(hasNormals)
    {
      hash.Add("MORPHNOR");
    }

    if(hasTangents)
    {
      hash.Add("MORPHTAN");
    }

    if(hasPositions || hasNormals || hasTangents)
    {
      hash.Add("MORPH");

      if(BlendShapes::Version::VERSION_2_0 == meshDef.mBlendShapeVersion)
      {
        hash.Add("MORPHV2");
      }
    }
  }

  return hash;
}
} // namespace

struct ShaderDefinitionFactory::Impl
{
  ResourceBundle*           mResources; // no ownership
  std::map<uint64_t, Index> mShaderMap;
};

ShaderDefinitionFactory::ShaderDefinitionFactory()
: mImpl{new Impl()}
{
}

ShaderDefinitionFactory::~ShaderDefinitionFactory() = default;

void ShaderDefinitionFactory::SetResources(ResourceBundle& resources)
{
  mImpl->mResources = &resources;
  mImpl->mShaderMap.clear();
}

Index ShaderDefinitionFactory::ProduceShader(const NodeDefinition& nodeDef)
{
  DALI_ASSERT_DEBUG(nodeDef.mRenderable);

  auto&            resources = *mImpl->mResources;
  ResourceReceiver receiver{resources};
  nodeDef.mRenderable->RegisterResources(receiver);
  if(!(receiver.mMeshDef && receiver.mMaterialDef))
  {
    return INVALID_INDEX;
  }

  auto&    shaderMap = mImpl->mShaderMap;
  uint64_t hash      = HashNode(nodeDef, *receiver.mMaterialDef, *receiver.mMeshDef);
  auto     iFind     = shaderMap.find(hash);
  if(iFind != shaderMap.end())
  {
    return iFind->second;
  }

  ShaderDefinition shaderDef;
  shaderDef.mVertexShaderPath   = PBR_SHADER_NAME + ".vsh";
  shaderDef.mFragmentShaderPath = PBR_SHADER_NAME + ".fsh";
  shaderDef.mRendererState      = RendererState::DEPTH_TEST | RendererState::DEPTH_WRITE | RendererState::CULL_BACK;

  auto&      materialDef     = *receiver.mMaterialDef;
  const bool hasTransparency = MaskMatch(materialDef.mFlags, MaterialDefinition::TRANSPARENCY);
  if(hasTransparency)
  {
    // TODO: this requires more granularity
    shaderDef.mRendererState = (shaderDef.mRendererState | RendererState::ALPHA_BLEND) & ~RendererState::DEPTH_WRITE;
  }

  if(hasTransparency ||
     materialDef.CheckTextures(MaterialDefinition::ALBEDO) ||
     materialDef.CheckTextures(MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS) ||
     materialDef.CheckTextures(MaterialDefinition::NORMAL))
  {
    shaderDef.mDefines.push_back("THREE_TEX");
  }

  if(materialDef.GetAlphaCutoff() > 0.f)
  {
    shaderDef.mDefines.push_back("ALPHA_TEST");
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::SUBSURFACE))
  {
    shaderDef.mDefines.push_back("SSS");
  }

  if(MaskMatch(receiver.mMaterialDef->mFlags, MaterialDefinition::GLTF_CHANNELS))
  {
    shaderDef.mDefines.push_back("GLTF_CHANNELS");
  }

  const auto& meshDef = *receiver.mMeshDef;
  if(meshDef.IsSkinned())
  {
    shaderDef.mDefines.push_back("SKINNING");
  }

  if(MaskMatch(meshDef.mFlags, MeshDefinition::FLIP_UVS_VERTICAL))
  {
    shaderDef.mDefines.push_back("FLIP_V");
  }

  if(meshDef.HasBlendShapes())
  {
    bool hasPositions = false;
    bool hasNormals   = false;
    bool hasTangents  = false;
    RetrieveBlendShapeComponents(meshDef.mBlendShapes, hasPositions, hasNormals, hasTangents);

    if(hasPositions)
    {
      shaderDef.mDefines.push_back("MORPH_POSITION");
    }

    if(hasNormals)
    {
      shaderDef.mDefines.push_back("MORPH_NORMAL");
    }

    if(hasTangents)
    {
      shaderDef.mDefines.push_back("MORPH_TANGENT");
    }

    if(hasPositions || hasNormals || hasTangents)
    {
      shaderDef.mDefines.push_back("MORPH");

      if(BlendShapes::Version::VERSION_2_0 == meshDef.mBlendShapeVersion)
      {
        shaderDef.mDefines.push_back("MORPH_VERSION_2_0");
      }
    }
  }

  shaderDef.mUniforms["uMaxLOD"]     = 6.f;
  shaderDef.mUniforms["uCubeMatrix"] = Matrix::IDENTITY;

  Index result    = resources.mShaders.size();
  shaderMap[hash] = result;

  resources.mShaders.emplace_back(std::move(shaderDef), Shader());

  return result;
}

} // namespace SceneLoader
} // namespace Dali
