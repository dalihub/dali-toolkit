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
#include <dali-scene3d/public-api/loader/shader-definition-option.h>

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

ShaderDefinitionOption MakeOption(const MaterialDefinition& materialDef, const MeshDefinition& meshDef)
{
  ShaderDefinitionOption option;

  const bool hasTransparency = MaskMatch(materialDef.mFlags, MaterialDefinition::TRANSPARENCY);
  if(hasTransparency)
  {
    option.SetTransparency();
  }

  if(hasTransparency ||
     !materialDef.CheckTextures(MaterialDefinition::ALBEDO | MaterialDefinition::METALLIC) ||
     !materialDef.CheckTextures(MaterialDefinition::NORMAL | MaterialDefinition::ROUGHNESS))
  {
    option.AddOption(ShaderDefinitionOption::Type::THREE_TEXTURE);

    // For the glTF, each of basecolor, metallic_roughness, normal texture is not essential.
    if(MaskMatch(materialDef.mFlags, MaterialDefinition::ALBEDO))
    {
      option.AddOption(ShaderDefinitionOption::Type::BASE_COLOR_TEXTURE);
    }

    if(materialDef.CheckTextures(MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS))
    {
      option.AddOption(ShaderDefinitionOption::Type::METALLIC_ROUGHNESS_TEXTURE);
    }

    if(MaskMatch(materialDef.mFlags, MaterialDefinition::NORMAL))
    {
      option.AddOption(ShaderDefinitionOption::Type::NORMAL_TEXTURE);
    }
  }

  if(materialDef.GetAlphaCutoff() > 0.f)
  {
    option.AddOption(ShaderDefinitionOption::Type::ALPHA_TEST);
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::SUBSURFACE))
  {
    option.AddOption(ShaderDefinitionOption::Type::SUBSURFACE);
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::OCCLUSION))
  {
    option.AddOption(ShaderDefinitionOption::Type::OCCLUSION);
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::EMISSIVE))
  {
    option.AddOption(ShaderDefinitionOption::Type::EMISSIVE);
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::SPECULAR))
  {
    option.AddOption(ShaderDefinitionOption::Type::SPECULAR);
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::SPECULAR_COLOR))
  {
    option.AddOption(ShaderDefinitionOption::Type::SPECULAR_COLOR);
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::GLTF_CHANNELS))
  {
    option.AddOption(ShaderDefinitionOption::Type::GLTF_CHANNELS);
  }

  if(meshDef.IsSkinned())
  {
    option.AddOption(ShaderDefinitionOption::Type::SKINNING);
  }

  if(MaskMatch(meshDef.mFlags, MeshDefinition::FLIP_UVS_VERTICAL))
  {
    option.AddOption(ShaderDefinitionOption::Type::FLIP_UVS_VERTICAL);
  }

  if(meshDef.mColors.IsDefined())
  {
    option.AddOption(ShaderDefinitionOption::Type::COLOR_ATTRIBUTE);
  }

  if(meshDef.mTangentType == Property::VECTOR4)
  {
    option.AddOption(ShaderDefinitionOption::Type::VEC4_TANGENT);
  }

  if(meshDef.HasBlendShapes())
  {
    bool hasPositions = false;
    bool hasNormals   = false;
    bool hasTangents  = false;
    meshDef.RetrieveBlendShapeComponents(hasPositions, hasNormals, hasTangents);
    if(hasPositions)
    {
      option.AddOption(ShaderDefinitionOption::Type::MORPH_POSITION);
    }

    if(hasNormals)
    {
      option.AddOption(ShaderDefinitionOption::Type::MORPH_NORMAL);
    }

    if(hasTangents)
    {
      option.AddOption(ShaderDefinitionOption::Type::MORPH_TANGENT);
    }

    if(hasPositions || hasNormals || hasTangents)
    {
      if(BlendShapes::Version::VERSION_2_0 == meshDef.mBlendShapeVersion)
      {
        option.AddOption(ShaderDefinitionOption::Type::MORPH_VERSION_2_0);
      }
    }
  }

  return option;
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

  auto&                  shaderMap = mImpl->mShaderMap;
  ShaderDefinitionOption option    = MakeOption(*receiver.mMaterialDef, *receiver.mMeshDef);
  uint64_t               hash      = option.GetOptionHash();
  auto                   iFind     = shaderMap.find(hash);
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

    option.GetDefines(shaderDef.mDefines);
    shaderDef.mUniforms["uCubeMatrix"] = Matrix::IDENTITY;

    Index result    = resources.mShaders.size();
    shaderMap[hash] = result;

    resources.mShaders.emplace_back(std::move(shaderDef), Shader());

    renderable.mShaderIdx = result;
  }

  return renderable.mShaderIdx;
}

} // namespace Dali::Scene3D::Loader
