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

// CLASS HEADER
#include <dali-scene3d/public-api/loader/shader-definition-factory.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/map-wrapper.h>
#include <cstring>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/loader/hash.h>
#include <dali-scene3d/public-api/loader/blend-shape-details.h>
#include <dali-scene3d/public-api/loader/node-definition.h>

namespace Dali::Scene3D::Loader
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

void RetrieveBlendShapeComponents(const std::vector<MeshDefinition::BlendShape>& blendShapes, bool& hasPositions, bool& hasNormals, bool& hasTangents)
{
  for(const auto& blendShape : blendShapes)
  {
    hasPositions = hasPositions || blendShape.deltas.IsDefined();
    hasNormals   = hasNormals || blendShape.normals.IsDefined();
    hasTangents  = hasTangents || blendShape.tangents.IsDefined();
  }
}

uint64_t HashNode(const MaterialDefinition& materialDef, const MeshDefinition& meshDef)
{
  Hash hash;

  const bool hasTransparency = MaskMatch(materialDef.mFlags, MaterialDefinition::TRANSPARENCY);
  hash.Add(hasTransparency);

  if(hasTransparency ||
     materialDef.CheckTextures(MaterialDefinition::ALBEDO) ||
     materialDef.CheckTextures(MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS) ||
     materialDef.CheckTextures(MaterialDefinition::NORMAL))
  {
    hash.Add("3TEX");

    // For the glTF, each of basecolor, metallic_roughness, normal texture is not essential.
    if(materialDef.CheckTextures(MaterialDefinition::ALBEDO))
    {
      hash.Add("BCTEX");
    }

    if(materialDef.CheckTextures(MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS))
    {
      hash.Add("MRTEX");
    }

    if(materialDef.CheckTextures(MaterialDefinition::NORMAL))
    {
      hash.Add("NTEX");
    }
  }

  if(materialDef.GetAlphaCutoff() > 0.f)
  {
    hash.Add("ALPH" /*A_TEST*/);
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::SUBSURFACE))
  {
    hash.Add("SSS");
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::OCCLUSION))
  {
    hash.Add("OCCL" /*USION*/);
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::EMISSIVE))
  {
    hash.Add("EMIS" /*SIVE*/);
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::SPECULAR))
  {
    hash.Add("SPECTEX");
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::SPECULAR_COLOR))
  {
    hash.Add("SPECCOLTEX");
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

  if(meshDef.mColors.IsDefined())
  {
    hash.Add("COLATT");
  }

  if(meshDef.mTangentType == Property::VECTOR4)
  {
    hash.Add("V4TAN");
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

Index ShaderDefinitionFactory::ProduceShader(NodeDefinition::Renderable& renderable)
{
  auto& resources = *mImpl->mResources;

  ResourceReceiver receiver{resources};
  renderable.RegisterResources(receiver);

  if(!(receiver.mMeshDef && receiver.mMaterialDef))
  {
    renderable.mShaderIdx = INVALID_INDEX;
    return INVALID_INDEX;
  }

  auto&    shaderMap = mImpl->mShaderMap;
  uint64_t hash      = HashNode(*receiver.mMaterialDef, *receiver.mMeshDef);
  auto     iFind     = shaderMap.find(hash);
  if(iFind != shaderMap.end())
  {
    renderable.mShaderIdx = iFind->second;
  }
  else
  {
    ShaderDefinition shaderDef;
    shaderDef.mUseBuiltInShader = true;
    shaderDef.mRendererState    = RendererState::DEPTH_TEST;

    auto& materialDef = *receiver.mMaterialDef;
    if(!materialDef.mDoubleSided)
    {
      shaderDef.mRendererState |= RendererState::CULL_BACK;
    }

    const bool hasTransparency = MaskMatch(materialDef.mFlags, MaterialDefinition::TRANSPARENCY);
    if(hasTransparency)
    {
      // TODO: this requires more granularity
      shaderDef.mRendererState = (shaderDef.mRendererState | RendererState::ALPHA_BLEND);
    }

    if(hasTransparency ||
       !materialDef.CheckTextures(MaterialDefinition::ALBEDO | MaterialDefinition::METALLIC) ||
       !materialDef.CheckTextures(MaterialDefinition::NORMAL | MaterialDefinition::ROUGHNESS))

    {
      shaderDef.mDefines.push_back("THREE_TEX");

      // For the glTF, each of basecolor, metallic_roughness, normal texture is not essential.
      if(MaskMatch(materialDef.mFlags, MaterialDefinition::ALBEDO))
      {
        shaderDef.mDefines.push_back("BASECOLOR_TEX");
      }

      if(materialDef.CheckTextures(MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS))
      {
        shaderDef.mDefines.push_back("METALLIC_ROUGHNESS_TEX");
      }

      if(MaskMatch(materialDef.mFlags, MaterialDefinition::NORMAL))
      {
        shaderDef.mDefines.push_back("NORMAL_TEX");
      }
    }

    if(materialDef.GetAlphaCutoff() > 0.f)
    {
      shaderDef.mDefines.push_back("ALPHA_TEST");
    }

    if(MaskMatch(materialDef.mFlags, MaterialDefinition::SUBSURFACE))
    {
      shaderDef.mDefines.push_back("SSS");
    }

    if(MaskMatch(materialDef.mFlags, MaterialDefinition::OCCLUSION))
    {
      shaderDef.mDefines.push_back("OCCLUSION");
    }

    if(MaskMatch(materialDef.mFlags, MaterialDefinition::EMISSIVE))
    {
      shaderDef.mDefines.push_back("EMISSIVE");
    }

    if(MaskMatch(materialDef.mFlags, MaterialDefinition::SPECULAR))
    {
      shaderDef.mDefines.push_back("MATERIAL_SPECULAR_TEXTURE");
    }

    if(MaskMatch(materialDef.mFlags, MaterialDefinition::SPECULAR_COLOR))
    {
      shaderDef.mDefines.push_back("MATERIAL_SPECULAR_COLOR_TEXTURE");
    }

    if(MaskMatch(materialDef.mFlags, MaterialDefinition::GLTF_CHANNELS))
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

    if(meshDef.mColors.IsDefined())
    {
      shaderDef.mDefines.push_back("COLOR_ATTRIBUTE");
    }

    if(meshDef.mTangentType == Property::VECTOR4)
    {
      shaderDef.mDefines.push_back("VEC4_TANGENT");
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

    shaderDef.mUniforms["uCubeMatrix"] = Matrix::IDENTITY;

    Index result    = resources.mShaders.size();
    shaderMap[hash] = result;

    resources.mShaders.emplace_back(std::move(shaderDef), Shader());

    renderable.mShaderIdx = result;
  }

  return renderable.mShaderIdx;
}

} // namespace Dali::Scene3D::Loader
