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

#include <dali/integration-api/debug.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_MODEL_SHADER_MANAGER");
#endif
} // namespace

namespace Dali::Scene3D::Loader
{
namespace
{
static constexpr uint32_t INDEX_FOR_LIGHT_CONSTRAINT_TAG  = 10;
static constexpr uint32_t INDEX_FOR_SHADOW_CONSTRAINT_TAG = 100;

static const char* ADD_EXTRA_SKINNING_ATTRIBUTES{"ADD_EXTRA_SKINNING_ATTRIBUTES"};
static const char* ADD_EXTRA_WEIGHTS{"ADD_EXTRA_WEIGHTS"};

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

    // Add options for ADD_EXTRA_SKINNING_ATTRIBUTES and ADD_EXTRA_WEIGHTS:
    size_t numberOfSets = meshDef.mJoints.size();
    if(numberOfSets > 1)
    {
      std::ostringstream attributes;
      std::ostringstream weights;
      for(size_t i = 1; i < numberOfSets; ++i)
      {
        attributes << "in vec4 aJoints" << i << ";\n";
        attributes << "in vec4 aWeights" << i << ";\n";

        weights << "bone +=\n"
                << "uBone[int(aJoints" << i << ".x)] * aWeights" << i << ".x +\n"
                << "uBone[int(aJoints" << i << ".y)] * aWeights" << i << ".y +\n"
                << "uBone[int(aJoints" << i << ".z)] * aWeights" << i << ".z +\n"
                << "uBone[int(aJoints" << i << ".w)] * aWeights" << i << ".w;\n";
      }
      option.AddMacroDefinition(ADD_EXTRA_SKINNING_ATTRIBUTES, attributes.str());
      option.AddMacroDefinition(ADD_EXTRA_WEIGHTS, weights.str());
    }
  }

  if(MaskMatch(meshDef.mFlags, MeshDefinition::FLIP_UVS_VERTICAL))
  {
    option.AddOption(ShaderOption::Type::FLIP_UVS_VERTICAL);
  }

  if(!meshDef.mColors.empty() && meshDef.mColors[0].IsDefined())
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

  Scene3D::Light mShadowLight;
};

ShaderManager::ShaderManager()
: mImpl{new Impl()}
{
}

ShaderManager::~ShaderManager() = default;

Dali::Shader ShaderManager::ProduceShader(const MaterialDefinition& materialDefinition, const MeshDefinition& meshDefinition)
{
  DALI_LOG_INFO(gLogFilter, Debug::Concise, "Defining shader from mat/mesh definitions\n");
  ShaderOption option = MakeOption(materialDefinition, meshDefinition);
  return ProduceShader(option);
}

Dali::Shader ShaderManager::ProduceShader(const ShaderOption& shaderOption)
{
  Dali::Shader result;

  auto&    shaderMap = mImpl->mShaderMap;
  uint64_t hash      = shaderOption.GetOptionHash();

#if defined(DEBUG_ENABLED)
  std::ostringstream oss;
  oss << "  ShaderOption defines:";
  std::vector<std::string> defines;
  shaderOption.GetDefines(defines);
  for(auto& def : defines)
  {
    oss << def << ", ";
  }
  oss << std::endl
      << "  ShaderOption macro definitions:" << std::endl;
  for(auto& macro : shaderOption.GetMacroDefinitions())
  {
    oss << macro.macro << " : " << macro.definition << std::endl;
  }
  DALI_LOG_INFO(gLogFilter, Debug::Concise, "ShaderOption:\n%s\n", oss.str().c_str());
#endif

  auto iFind = shaderMap.find(hash);
  if(iFind != shaderMap.end())
  {
    DALI_LOG_INFO(gLogFilter, Debug::Concise, "Defining Shader found: hash: %lx", hash);
    result = mImpl->mShaders[iFind->second];
  }
  else
  {
    DALI_LOG_INFO(gLogFilter, Debug::Concise, "Creating new shader: hash: %lx\n", hash);
    ShaderDefinition shaderDef;
    shaderDef.mUseBuiltInShader = true;

    shaderOption.GetDefines(shaderDef.mDefines);
    shaderDef.mMacros                  = shaderOption.GetMacroDefinitions();
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

    result.RegisterProperty("uIsShadowEnabled", static_cast<int32_t>(!!mImpl->mShadowLight));
    if(!!mImpl->mShadowLight)
    {
      SetShadowConstraintToShader(result);
      SetShadowUniformToShader(result);
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

void ShaderManager::SetShadow(Scene3D::Light light)
{
  mImpl->mShadowLight = light;
  for(auto&& shader : mImpl->mShaders)
  {
    std::string shadowEnabledPropertyName(Scene3D::Internal::Light::GetShadowEnabledUniformName());
    shader.RegisterProperty(shadowEnabledPropertyName, static_cast<int32_t>(true));
  }

  SetShadowProperty();
}

void ShaderManager::RemoveShadow()
{
  for(auto&& shader : mImpl->mShaders)
  {
    std::string shadowEnabledPropertyName(Scene3D::Internal::Light::GetShadowEnabledUniformName());
    shader.RegisterProperty(shadowEnabledPropertyName, static_cast<int32_t>(false));
    shader.RemoveConstraints(INDEX_FOR_SHADOW_CONSTRAINT_TAG);
  }
  mImpl->mShadowLight.Reset();
}

void ShaderManager::UpdateShadowUniform(Scene3D::Light light)
{
  if(light != mImpl->mShadowLight)
  {
    return;
  }

  for(auto&& shader : mImpl->mShaders)
  {
    SetShadowUniformToShader(shader);
  }
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

void ShaderManager::SetShadowUniformToShader(Dali::Shader shader)
{
  shader.RegisterProperty("uShadowIntensity", mImpl->mShadowLight.GetShadowIntensity());
  shader.RegisterProperty("uShadowBias", mImpl->mShadowLight.GetShadowBias());
  shader.RegisterProperty("uEnableShadowSoftFiltering", static_cast<int>(mImpl->mShadowLight.IsShadowSoftFilteringEnabled()));
}

void ShaderManager::SetShadowProperty()
{
  for(auto&& shader : mImpl->mShaders)
  {
    SetShadowUniformToShader(shader);
    SetShadowConstraintToShader(shader);
  }
}

void ShaderManager::SetShadowConstraintToShader(Dali::Shader shader)
{
  // Constraint is applied before View/Projection Matrix is computed in update thread.
  // So, it could show not plausible result if camera properties are changed discontinuesly.
  // If we want to make it be synchronized, View/Projection matrix are needed to be conputed in below constraint.

  std::string       shadowViewProjectionPropertyName(Scene3D::Internal::Light::GetShadowViewProjectionMatrixUniformName());
  auto              shadowViewProjectionPropertyIndex = shader.RegisterProperty(shadowViewProjectionPropertyName, Matrix::IDENTITY);
  Dali::CameraActor shadowLightCamera                 = Dali::Scene3D::Internal::GetImplementation(mImpl->mShadowLight).GetCamera();
  auto              tempViewProjectionMatrixIndex     = shadowLightCamera.GetPropertyIndex("tempViewProjectionMatrix");
  if(tempViewProjectionMatrixIndex != Dali::Property::INVALID_INDEX)
  {
    tempViewProjectionMatrixIndex = shadowLightCamera.RegisterProperty("tempViewProjectionMatrix", Matrix::IDENTITY);
  }
  Dali::Constraint shadowViewProjectionConstraint = Dali::Constraint::New<Matrix>(shader, shadowViewProjectionPropertyIndex, [](Matrix& output, const PropertyInputContainer& inputs) { output = inputs[0]->GetMatrix(); });
  shadowViewProjectionConstraint.AddSource(Source{shadowLightCamera, tempViewProjectionMatrixIndex});
  shadowViewProjectionConstraint.ApplyPost();
  shadowViewProjectionConstraint.SetTag(INDEX_FOR_SHADOW_CONSTRAINT_TAG);
}

} // namespace Dali::Scene3D::Loader
