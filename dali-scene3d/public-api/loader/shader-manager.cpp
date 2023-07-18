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
#include <dali-scene3d/public-api/loader/shader-manager.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/public-api/animation/constraint.h>
#include <cstring>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/light/light-impl.h>
#include <dali-scene3d/internal/loader/hash.h>
#include <dali-scene3d/public-api/loader/blend-shape-details.h>
#include <dali-scene3d/public-api/loader/node-definition.h>

namespace Dali::Scene3D::Loader
{
namespace
{
static constexpr uint32_t INDEX_FOR_LIGHT_CONSTRAINT_TAG = 10;

ShaderOption MakeOption(const MaterialDefinition& materialDef, const MeshDefinition& meshDef)
{
  ShaderOption option;

  const bool hasTransparency = MaskMatch(materialDef.mFlags, MaterialDefinition::TRANSPARENCY);
  if(hasTransparency)
  {
    option.SetTransparency();
  }

  if(hasTransparency ||
     !materialDef.CheckTextures(MaterialDefinition::ALBEDO | MaterialDefinition::METALLIC) ||
     !materialDef.CheckTextures(MaterialDefinition::NORMAL | MaterialDefinition::ROUGHNESS))
  {
    option.AddOption(ShaderOption::Type::THREE_TEXTURE);

    // For the glTF, each of basecolor, metallic_roughness, normal texture is not essential.
    if(MaskMatch(materialDef.mFlags, MaterialDefinition::ALBEDO))
    {
      option.AddOption(ShaderOption::Type::BASE_COLOR_TEXTURE);
    }

    if(materialDef.CheckTextures(MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS))
    {
      option.AddOption(ShaderOption::Type::METALLIC_ROUGHNESS_TEXTURE);
    }

    if(MaskMatch(materialDef.mFlags, MaterialDefinition::NORMAL))
    {
      option.AddOption(ShaderOption::Type::NORMAL_TEXTURE);
    }
  }

  if(materialDef.GetAlphaCutoff() > 0.f)
  {
    option.AddOption(ShaderOption::Type::ALPHA_TEST);
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::SUBSURFACE))
  {
    option.AddOption(ShaderOption::Type::SUBSURFACE);
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::OCCLUSION))
  {
    option.AddOption(ShaderOption::Type::OCCLUSION);
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::EMISSIVE))
  {
    option.AddOption(ShaderOption::Type::EMISSIVE);
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::SPECULAR))
  {
    option.AddOption(ShaderOption::Type::SPECULAR);
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::SPECULAR_COLOR))
  {
    option.AddOption(ShaderOption::Type::SPECULAR_COLOR);
  }

  if(MaskMatch(materialDef.mFlags, MaterialDefinition::GLTF_CHANNELS))
  {
    option.AddOption(ShaderOption::Type::GLTF_CHANNELS);
  }

  if(meshDef.IsSkinned())
  {
    option.AddOption(ShaderOption::Type::SKINNING);
  }

  if(MaskMatch(meshDef.mFlags, MeshDefinition::FLIP_UVS_VERTICAL))
  {
    option.AddOption(ShaderOption::Type::FLIP_UVS_VERTICAL);
  }

  if(meshDef.mColors.IsDefined())
  {
    option.AddOption(ShaderOption::Type::COLOR_ATTRIBUTE);
  }

  if(meshDef.mTangentType == Property::VECTOR4)
  {
    option.AddOption(ShaderOption::Type::VEC4_TANGENT);
  }

  if(meshDef.HasBlendShapes())
  {
    bool hasPositions = false;
    bool hasNormals   = false;
    bool hasTangents  = false;
    meshDef.RetrieveBlendShapeComponents(hasPositions, hasNormals, hasTangents);
    if(hasPositions)
    {
      option.AddOption(ShaderOption::Type::MORPH_POSITION);
    }

    if(hasNormals)
    {
      option.AddOption(ShaderOption::Type::MORPH_NORMAL);
    }

    if(hasTangents)
    {
      option.AddOption(ShaderOption::Type::MORPH_TANGENT);
    }

    if(hasPositions || hasNormals || hasTangents)
    {
      if(BlendShapes::Version::VERSION_2_0 == meshDef.mBlendShapeVersion)
      {
        option.AddOption(ShaderOption::Type::MORPH_VERSION_2_0);
      }
    }
  }

  return option;
}
} // namespace

struct ShaderManager::Impl
{
  std::map<uint64_t, Index>   mShaderMap;
  std::vector<Dali::Shader>   mShaders;
  std::vector<Scene3D::Light> mLights;
};

ShaderManager::ShaderManager()
: mImpl{new Impl()}
{
}

ShaderManager::~ShaderManager() = default;

Dali::Shader ShaderManager::ProduceShader(const MaterialDefinition& materialDefinition, const MeshDefinition& meshDefinition)
{
  ShaderOption option = MakeOption(materialDefinition, meshDefinition);
  return ProduceShader(option);
}

Dali::Shader ShaderManager::ProduceShader(const ShaderOption& shaderOption)
{
  Dali::Shader result;

  auto&    shaderMap = mImpl->mShaderMap;
  uint64_t hash      = shaderOption.GetOptionHash();
  auto     iFind     = shaderMap.find(hash);
  if(iFind != shaderMap.end())
  {
    result = mImpl->mShaders[iFind->second];
  }
  else
  {
    ShaderDefinition shaderDef;
    shaderDef.mUseBuiltInShader = true;

    shaderOption.GetDefines(shaderDef.mDefines);
    shaderDef.mUniforms["uCubeMatrix"] = Matrix::IDENTITY;

    shaderMap[hash] = mImpl->mShaders.size();

    auto raw = shaderDef.LoadRaw("");
    mImpl->mShaders.emplace_back(shaderDef.Load(std::move(raw)));
    result = mImpl->mShaders.back();

    std::string lightCountPropertyName(Scene3D::Internal::Light::GetLightCountUniformName());
    result.RegisterProperty(lightCountPropertyName, static_cast<int32_t>(mImpl->mLights.size()));

    for(uint32_t index = 0; index < mImpl->mLights.size(); ++index)
    {
      SetLightConstraintToShader(index, result);
    }
  }

  return result;
}

RendererState::Type ShaderManager::GetRendererState(const MaterialDefinition& materialDefinition)
{
  RendererState::Type rendererState = RendererState::DEPTH_TEST;

  if(!materialDefinition.mDoubleSided)
  {
    rendererState |= RendererState::CULL_BACK;
  }

  const bool hasTransparency = MaskMatch(materialDefinition.mFlags, MaterialDefinition::TRANSPARENCY);
  if(hasTransparency)
  {
    // TODO: this requires more granularity
    rendererState = (rendererState | RendererState::ALPHA_BLEND);
  }
  return rendererState;
}

bool ShaderManager::AddLight(Scene3D::Light light)
{
  if(!light || mImpl->mLights.size() >= Scene3D::Internal::Light::GetMaximumEnabledLightCount())
  {
    return false;
  }

  uint32_t lightIndex = mImpl->mLights.size();
  mImpl->mLights.push_back(light);

  for(auto&& shader : mImpl->mShaders)
  {
    std::string lightCountPropertyName(Scene3D::Internal::Light::GetLightCountUniformName());
    shader.RegisterProperty(lightCountPropertyName, static_cast<int32_t>(mImpl->mLights.size()));
  }

  SetLightConstraint(lightIndex);

  return true;
}

void ShaderManager::RemoveLight(Scene3D::Light light)
{
  uint32_t lightCount = mImpl->mLights.size();
  for(uint32_t index = 0; index < lightCount; ++index)
  {
    if(mImpl->mLights[index] != light)
    {
      continue;
    }

    RemoveLightConstraint(index);

    if(!mImpl->mLights.empty() && light != mImpl->mLights.back())
    {
      RemoveLightConstraint(mImpl->mLights.size() - 1);
      mImpl->mLights[index] = mImpl->mLights.back();
      SetLightConstraint(index);
    }

    mImpl->mLights.pop_back();

    for(auto&& shader : mImpl->mShaders)
    {
      std::string lightCountPropertyName(Scene3D::Internal::Light::GetLightCountUniformName());
      shader.RegisterProperty(lightCountPropertyName, static_cast<int32_t>(mImpl->mLights.size()));
    }
    break;
  }
}

uint32_t ShaderManager::GetLightCount() const
{
  return mImpl->mLights.size();
}

void ShaderManager::SetLightConstraint(uint32_t lightIndex)
{
  for(auto&& shader : mImpl->mShaders)
  {
    SetLightConstraintToShader(lightIndex, shader);
  }
}

void ShaderManager::SetLightConstraintToShader(uint32_t lightIndex, Dali::Shader shader)
{
  std::string lightDirectionPropertyName(Scene3D::Internal::Light::GetLightDirectionUniformName());
  lightDirectionPropertyName += "[" + std::to_string(lightIndex) + "]";
  auto             lightDirectionPropertyIndex = shader.RegisterProperty(lightDirectionPropertyName, Vector3::ZAXIS);
  Dali::Constraint lightDirectionConstraint    = Dali::Constraint::New<Vector3>(shader, lightDirectionPropertyIndex, [](Vector3& output, const PropertyInputContainer& inputs) { output = inputs[0]->GetQuaternion().Rotate(Vector3::ZAXIS); });
  lightDirectionConstraint.AddSource(Source{mImpl->mLights[lightIndex], Dali::Actor::Property::WORLD_ORIENTATION});
  lightDirectionConstraint.ApplyPost();
  lightDirectionConstraint.SetTag(INDEX_FOR_LIGHT_CONSTRAINT_TAG + lightIndex);

  std::string lightColorPropertyName(Scene3D::Internal::Light::GetLightColorUniformName());
  lightColorPropertyName += "[" + std::to_string(lightIndex) + "]";
  auto             lightColorPropertyIndex = shader.RegisterProperty(lightColorPropertyName, Vector3(Color::WHITE));
  Dali::Constraint lightColorConstraint    = Dali::Constraint::New<Vector3>(shader, lightColorPropertyIndex, [](Vector3& output, const PropertyInputContainer& inputs) { output = Vector3(inputs[0]->GetVector4()); });
  lightColorConstraint.AddSource(Source{mImpl->mLights[lightIndex], Dali::Actor::Property::COLOR});
  lightColorConstraint.ApplyPost();
  lightColorConstraint.SetTag(INDEX_FOR_LIGHT_CONSTRAINT_TAG + lightIndex);
}

void ShaderManager::RemoveLightConstraint(uint32_t lightIndex)
{
  for(auto&& shader : mImpl->mShaders)
  {
    shader.RemoveConstraints(INDEX_FOR_LIGHT_CONSTRAINT_TAG + lightIndex);
  }
}

} // namespace Dali::Scene3D::Loader
