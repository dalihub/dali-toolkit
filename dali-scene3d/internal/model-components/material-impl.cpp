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
#include <dali-scene3d/internal/model-components/material-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/rendering/sampler.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-scene3d/internal/light/light-impl.h>
#include <dali-scene3d/internal/model-components/material-modify-observer.h>
#include <dali-scene3d/public-api/loader/node-definition.h>
#include <dali-scene3d/public-api/loader/renderer-state.h>
#include <dali-scene3d/public-api/loader/utils.h>
#include <dali-scene3d/public-api/loader/shader-definition-option.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
namespace
{
/**
 * Creates Material through type registry
 */
BaseHandle Create()
{
  return Scene3D::Material::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Scene3D::Material, Dali::BaseHandle, Create);
DALI_TYPE_REGISTRATION_END()

static constexpr uint32_t         OFFSET_FOR_DIFFUSE_CUBE_TEXTURE  = 2u;
static constexpr uint32_t         OFFSET_FOR_SPECULAR_CUBE_TEXTURE = 1u;
static constexpr uint32_t         INVALID_INDEX                    = 0u;
static constexpr uint32_t         ALPHA_CUTOFF_FLAG                = Scene3D::Loader::MaterialDefinition::Flags::SUBSURFACE << 1;
static constexpr std::string_view THREE_TEX_KEYWORD                = "THREE_TEX";
static constexpr std::string_view GLTF_CHANNELS_KEYWORD            = "GLTF_CHANNELS";

enum TextureIndex
{
  BASE_COLOR,
  METALLIC_ROUGHNESS,
  NORMAL,
  OCCLUSION,
  EMISSIVE,
  SPECULAR,
  SPECULAR_COLOR,
  TEXTURE_TYPE_NUMBER,
};

} // unnamed namespace

MaterialPtr Material::New()
{
  MaterialPtr material = new Material();

  material->Initialize();

  return material;
}

Material::Material()
: mName(),
  mModifyFlag(MaterialModifyObserver::ModifyFlag::NONE),
  mObserverNotifying(false)
{
  mAsyncImageLoader = Dali::Toolkit::AsyncImageLoader::New();
  mAsyncImageLoader.ImageLoadedSignal().Connect(this, &Material::TextureLoadComplete);
  mTextureInformations.assign(TEXTURE_TYPE_NUMBER, TextureInformation());
  mTextureInformations[BASE_COLOR].mSemantic = Scene3D::Loader::MaterialDefinition::ALBEDO;
  // TODO : How we support dli manner
  mTextureInformations[METALLIC_ROUGHNESS].mSemantic = Scene3D::Loader::MaterialDefinition::METALLIC | Scene3D::Loader::MaterialDefinition::ROUGHNESS |
                                                       Scene3D::Loader::MaterialDefinition::GLTF_CHANNELS;
  mTextureInformations[NORMAL].mSemantic         = Scene3D::Loader::MaterialDefinition::NORMAL;
  mTextureInformations[OCCLUSION].mSemantic      = Scene3D::Loader::MaterialDefinition::OCCLUSION;
  mTextureInformations[EMISSIVE].mSemantic       = Scene3D::Loader::MaterialDefinition::EMISSIVE;
  mTextureInformations[SPECULAR].mSemantic       = Scene3D::Loader::MaterialDefinition::SPECULAR;
  mTextureInformations[SPECULAR_COLOR].mSemantic = Scene3D::Loader::MaterialDefinition::SPECULAR_COLOR;

  mTextureInformations[BASE_COLOR].mDefineKeyword         = Scene3D::Loader::ShaderDefinitionOption::GetDefineKeyword(Loader::ShaderDefinitionOption::Type::BASE_COLOR_TEXTURE).data();
  mTextureInformations[METALLIC_ROUGHNESS].mDefineKeyword = Scene3D::Loader::ShaderDefinitionOption::GetDefineKeyword(Loader::ShaderDefinitionOption::Type::METALLIC_ROUGHNESS_TEXTURE).data();
  mTextureInformations[NORMAL].mDefineKeyword             = Scene3D::Loader::ShaderDefinitionOption::GetDefineKeyword(Loader::ShaderDefinitionOption::Type::NORMAL_TEXTURE).data();
  mTextureInformations[OCCLUSION].mDefineKeyword          = Scene3D::Loader::ShaderDefinitionOption::GetDefineKeyword(Loader::ShaderDefinitionOption::Type::OCCLUSION).data();
  mTextureInformations[EMISSIVE].mDefineKeyword           = Scene3D::Loader::ShaderDefinitionOption::GetDefineKeyword(Loader::ShaderDefinitionOption::Type::EMISSIVE).data();
  mTextureInformations[SPECULAR].mDefineKeyword           = Scene3D::Loader::ShaderDefinitionOption::GetDefineKeyword(Loader::ShaderDefinitionOption::Type::SPECULAR).data();
  mTextureInformations[SPECULAR_COLOR].mDefineKeyword     = Scene3D::Loader::ShaderDefinitionOption::GetDefineKeyword(Loader::ShaderDefinitionOption::Type::SPECULAR_COLOR).data();

  mTextureInformations[TextureIndex::EMISSIVE].mFactor = Vector4::ZERO;
}

Material::~Material() = default;

void Material::Initialize()
{
}

void Material::SetProperty(Dali::Property::Index index, Dali::Property::Value propertyValue)
{
  bool needToApply = true;
  switch(index)
  {
    case Dali::Scene3D::Material::Property::NAME:
    {
      std::string name;
      if(propertyValue.Get(name))
      {
        mName = name;
      }
      needToApply = false;
      break;
    }
    case Dali::Scene3D::Material::Property::BASE_COLOR_URL:
    {
      std::string baseColorUrl;
      if(propertyValue.Get(baseColorUrl))
      {
        RequestTextureLoad(mTextureInformations[TextureIndex::BASE_COLOR], baseColorUrl);
        needToApply = false;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::BASE_COLOR_FACTOR:
    {
      Vector4 baseColorFactor;
      if(propertyValue.Get(baseColorFactor))
      {
        mTextureInformations[TextureIndex::BASE_COLOR].mFactor = baseColorFactor;
        mModifyFlag |= MaterialModifyObserver::ModifyFlag::UNIFORM;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::METALLIC_ROUGHNESS_URL:
    {
      std::string metallicRoughnessUrl;
      if(propertyValue.Get(metallicRoughnessUrl))
      {
        RequestTextureLoad(mTextureInformations[TextureIndex::METALLIC_ROUGHNESS], metallicRoughnessUrl);
        needToApply = false;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::METALLIC_FACTOR:
    {
      float metallicFactor;
      if(propertyValue.Get(metallicFactor))
      {
        mTextureInformations[TextureIndex::METALLIC_ROUGHNESS].mFactor.x = metallicFactor;
        mModifyFlag |= MaterialModifyObserver::ModifyFlag::UNIFORM;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::ROUGHNESS_FACTOR:
    {
      float roughnessFactor;
      if(propertyValue.Get(roughnessFactor))
      {
        mTextureInformations[TextureIndex::METALLIC_ROUGHNESS].mFactor.y = roughnessFactor;
        mModifyFlag |= MaterialModifyObserver::ModifyFlag::UNIFORM;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::NORMAL_URL:
    {
      std::string normalUrl;
      if(propertyValue.Get(normalUrl))
      {
        RequestTextureLoad(mTextureInformations[TextureIndex::NORMAL], normalUrl);
        needToApply = false;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::NORMAL_SCALE:
    {
      float normalScale;
      if(propertyValue.Get(normalScale))
      {
        mTextureInformations[TextureIndex::NORMAL].mFactor.x = normalScale;
        mModifyFlag |= MaterialModifyObserver::ModifyFlag::UNIFORM;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::OCCLUSION_URL:
    {
      std::string occlusionUrl;
      if(propertyValue.Get(occlusionUrl))
      {
        RequestTextureLoad(mTextureInformations[TextureIndex::OCCLUSION], occlusionUrl);
        needToApply = false;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::OCCLUSION_STRENGTH:
    {
      float occlusionStrength;
      if(propertyValue.Get(occlusionStrength))
      {
        mTextureInformations[TextureIndex::OCCLUSION].mFactor.x = occlusionStrength;
        mModifyFlag |= MaterialModifyObserver::ModifyFlag::UNIFORM;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::EMISSIVE_URL:
    {
      std::string emissiveUrl;
      if(propertyValue.Get(emissiveUrl))
      {
        RequestTextureLoad(mTextureInformations[TextureIndex::EMISSIVE], emissiveUrl);
        needToApply = false;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::EMISSIVE_FACTOR:
    {
      Vector3 emissiveFactor;
      if(propertyValue.Get(emissiveFactor))
      {
        mTextureInformations[TextureIndex::EMISSIVE].mFactor = emissiveFactor;
        mModifyFlag |= MaterialModifyObserver::ModifyFlag::UNIFORM;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::ALPHA_MODE:
    {
      Dali::Scene3D::Material::AlphaModeType alphaMode;
      if(propertyValue.Get(alphaMode))
      {
        mAlphaMode = alphaMode;
        mModifyFlag |= MaterialModifyObserver::ModifyFlag::UNIFORM;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::ALPHA_CUTOFF:
    {
      float alphaCutoff;
      if(propertyValue.Get(alphaCutoff))
      {
        mAlphaCutoff = alphaCutoff;
        mModifyFlag |= MaterialModifyObserver::ModifyFlag::UNIFORM;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::DOUBLE_SIDED:
    {
      bool doubleSided;
      if(propertyValue.Get(doubleSided))
      {
        mDoubleSided = doubleSided;
        mModifyFlag |= MaterialModifyObserver::ModifyFlag::UNIFORM;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::IOR:
    {
      float ior;
      if(propertyValue.Get(ior))
      {
        mIor = ior;
        mModifyFlag |= MaterialModifyObserver::ModifyFlag::UNIFORM;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::SPECULAR_URL:
    {
      std::string specularUrl;
      if(propertyValue.Get(specularUrl))
      {
        RequestTextureLoad(mTextureInformations[TextureIndex::SPECULAR], specularUrl);
        needToApply = false;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::SPECULAR_FACTOR:
    {
      float specularFactor;
      if(propertyValue.Get(specularFactor))
      {
        mTextureInformations[TextureIndex::SPECULAR].mFactor.x = specularFactor;
        mModifyFlag |= MaterialModifyObserver::ModifyFlag::UNIFORM;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::SPECULAR_COLOR_URL:
    {
      std::string specularColorUrl;
      if(propertyValue.Get(specularColorUrl))
      {
        RequestTextureLoad(mTextureInformations[TextureIndex::SPECULAR_COLOR], specularColorUrl);
        needToApply = false;
      }
      break;
    }
    case Dali::Scene3D::Material::Property::SPECULAR_COLOR_FACTOR:
    {
      Vector3 specularColorFactor;
      if(propertyValue.Get(specularColorFactor))
      {
        mTextureInformations[TextureIndex::SPECULAR_COLOR].mFactor = specularColorFactor;
        mModifyFlag |= MaterialModifyObserver::ModifyFlag::UNIFORM;
      }
      break;
    }
  }

  if(needToApply)
  {
    Apply();
  }
}

Dali::Property::Value Material::GetProperty(Dali::Property::Index index) const
{
  Dali::Property::Value value;
  switch(index)
  {
    case Dali::Scene3D::Material::Property::NAME:
    {
      value = mName;
      break;
    }
    case Dali::Scene3D::Material::Property::BASE_COLOR_URL:
    {
      value = mTextureInformations[TextureIndex::BASE_COLOR].mUrl;
      break;
    }
    case Dali::Scene3D::Material::Property::BASE_COLOR_FACTOR:
    {
      value = mTextureInformations[TextureIndex::BASE_COLOR].mFactor;
      break;
    }
    case Dali::Scene3D::Material::Property::METALLIC_ROUGHNESS_URL:
    {
      value = mTextureInformations[TextureIndex::METALLIC_ROUGHNESS].mUrl;
      break;
    }
    case Dali::Scene3D::Material::Property::METALLIC_FACTOR:
    {
      value = mTextureInformations[TextureIndex::METALLIC_ROUGHNESS].mFactor.x;
      break;
    }
    case Dali::Scene3D::Material::Property::ROUGHNESS_FACTOR:
    {
      value = mTextureInformations[TextureIndex::METALLIC_ROUGHNESS].mFactor.y;
      break;
    }
    case Dali::Scene3D::Material::Property::NORMAL_URL:
    {
      value = mTextureInformations[TextureIndex::NORMAL].mUrl;
      break;
    }
    case Dali::Scene3D::Material::Property::NORMAL_SCALE:
    {
      value = mTextureInformations[TextureIndex::NORMAL].mFactor.x;
      break;
    }
    case Dali::Scene3D::Material::Property::OCCLUSION_URL:
    {
      value = mTextureInformations[TextureIndex::OCCLUSION].mUrl;
      break;
    }
    case Dali::Scene3D::Material::Property::OCCLUSION_STRENGTH:
    {
      value = mTextureInformations[TextureIndex::OCCLUSION].mFactor.x;
      break;
    }
    case Dali::Scene3D::Material::Property::EMISSIVE_URL:
    {
      value = mTextureInformations[TextureIndex::EMISSIVE].mUrl;
      break;
    }
    case Dali::Scene3D::Material::Property::EMISSIVE_FACTOR:
    {
      value = Vector3(mTextureInformations[TextureIndex::EMISSIVE].mFactor);
      break;
    }
    case Dali::Scene3D::Material::Property::ALPHA_MODE:
    {
      value = mAlphaMode;
      break;
    }
    case Dali::Scene3D::Material::Property::ALPHA_CUTOFF:
    {
      value = mAlphaCutoff;
      break;
    }
    case Dali::Scene3D::Material::Property::DOUBLE_SIDED:
    {
      value = mDoubleSided;
      break;
    }
    case Dali::Scene3D::Material::Property::IOR:
    {
      value = mIor;
      break;
    }
    case Dali::Scene3D::Material::Property::SPECULAR_URL:
    {
      value = mTextureInformations[TextureIndex::SPECULAR].mUrl;
      break;
    }
    case Dali::Scene3D::Material::Property::SPECULAR_FACTOR:
    {
      value = mTextureInformations[TextureIndex::SPECULAR].mFactor.x;
      break;
    }
    case Dali::Scene3D::Material::Property::SPECULAR_COLOR_URL:
    {
      value = mTextureInformations[TextureIndex::SPECULAR_COLOR].mUrl;
      break;
    }
    case Dali::Scene3D::Material::Property::SPECULAR_COLOR_FACTOR:
    {
      value = Vector3(mTextureInformations[TextureIndex::SPECULAR_COLOR].mFactor);
      break;
    }
  }
  return value;
}

void Material::SetTextureInformation(Scene3D::Material::TextureType index, TextureInformation&& textureInformation)
{
  mTextureInformations[index].mFactor  = textureInformation.mFactor;
  mTextureInformations[index].mSampler = textureInformation.mSampler;
  mTextureInformations[index].mTexture = textureInformation.mTexture;
  mTextureInformations[index].mUrl     = std::move(textureInformation.mUrl);
}

void Material::SetTexture(Scene3D::Material::TextureType index, Dali::Texture texture)
{
  if(static_cast<uint32_t>(index) < static_cast<uint32_t>(TextureIndex::TEXTURE_TYPE_NUMBER))
  {
    if(mTextureInformations[index].mTexture != texture)
    {
      if(mTextureInformations[index].mLoadingTaskId != INVALID_INDEX)
      {
        mAsyncImageLoader.Cancel(mTextureInformations[index].mLoadingTaskId);
        mTextureInformations[index].mLoadingTaskId = INVALID_INDEX;
      }
      mTextureInformations[index].mTexture = texture;
      if(IsResourceReady())
      {
        ResourcesLoadComplete();
      }
    }
  }
}

Dali::Texture Material::GetTexture(Scene3D::Material::TextureType index)
{
  if(static_cast<uint32_t>(index) < static_cast<uint32_t>(TextureIndex::TEXTURE_TYPE_NUMBER))
  {
    return mTextureInformations[index].mTexture;
  }
  return Dali::Texture();
}

TextureSet Material::GetTextureSet()
{
  TextureSet textures = TextureSet::New();
  for(uint32_t i = 0, count = 0; i < TEXTURE_TYPE_NUMBER; ++i)
  {
    if(!mTextureInformations[i].mTexture)
    {
      continue;
    }
    textures.SetTexture(count, mTextureInformations[i].mTexture);
    Sampler sampler = mTextureInformations[i].mSampler;
    if(!sampler)
    {
      auto samplerFlag = Scene3D::Loader::SamplerFlags::FILTER_LINEAR | (Scene3D::Loader::SamplerFlags::FILTER_LINEAR << Scene3D::Loader::SamplerFlags::FILTER_MAG_SHIFT) |
                         (Scene3D::Loader::SamplerFlags::WRAP_REPEAT << Scene3D::Loader::SamplerFlags::WRAP_S_SHIFT) | (Scene3D::Loader::SamplerFlags::WRAP_REPEAT << Scene3D::Loader::SamplerFlags::WRAP_T_SHIFT);
      sampler = Scene3D::Loader::SamplerFlags::MakeSampler(samplerFlag);
    }
    textures.SetSampler(count, sampler);
    count++;
  }
  return textures;
}

void Material::SetSampler(Scene3D::Material::TextureType index, Dali::Sampler sampler)
{
  if(static_cast<uint32_t>(index) < static_cast<uint32_t>(TextureIndex::TEXTURE_TYPE_NUMBER))
  {
    mTextureInformations[index].mSampler = sampler;
  }
}

Dali::Sampler Material::GetSampler(Scene3D::Material::TextureType index)
{
  if(static_cast<uint32_t>(index) < static_cast<uint32_t>(TextureIndex::TEXTURE_TYPE_NUMBER))
  {
    return mTextureInformations[index].mSampler;
  }
  return Dali::Sampler();
}

std::string Material::GetVertexShader()
{
  return mShaderData.mVertexShaderSource;
}

std::string Material::GetFragmentShader()
{
  return mShaderData.mFragmentShaderSource;
}

void Material::Apply()
{
  if(IsResourceReady())
  {
    UpdateMaterialData();
    NotifyObserver();
    return;
  }
  // The cases this material is applied to primitive are,
  // 1. material is added on Primitive.
  // When material is added on Primitive (1 case)
  //   1-1. when IsResourceReady() returns true,
  //     Primitive can takes information from Material
  //   1-2. if false.
  //     Material will noti to primitives when all resources are ready.
  // 2. Some properties are changed
  //   2-1. when IsResourceReady() returns true,
  //     Call NotifyObserver directly.
  //   2-2. if false.
  //     Material will noti to primitives when all resources are ready.
}

void Material::AddObserver(MaterialModifyObserver* observer)
{
  for(auto& observerEntity : mObservers)
  {
    if(observerEntity.first == observer)
    {
      observerEntity.second = true;
      return;
    }
  }
  mObservers.push_back({observer, true});
}

void Material::RemoveObserver(MaterialModifyObserver* observer)
{
  // Block during notifying to observer
  if(mObserverNotifying)
  {
    for(uint32_t i = 0; i < mObservers.size(); ++i)
    {
      if(mObservers[i].first == observer)
      {
        mObservers[i].second = false;
        return;
      }
    }
  }
  else
  {
    for(uint32_t i = 0; i < mObservers.size(); ++i)
    {
      if(mObservers[i].first == observer)
      {
        mObservers.erase(mObservers.begin() + i);
        return;
      }
    }
  }
}

void Material::UpdateMaterialData()
{
  uint32_t materialFlag = 0u;
  if(mAlphaMode == Dali::Scene3D::Material::AlphaModeType::BLEND)
  {
    mIsOpaque = false;
    mIsMask   = false;
    materialFlag |= Scene3D::Loader::MaterialDefinition::TRANSPARENCY;
  }
  else if(mAlphaMode == Dali::Scene3D::Material::AlphaModeType::MASK)
  {
    mIsOpaque = true;
    mIsMask   = true;
  }
  const bool hasTransparency = MaskMatch(materialFlag, Scene3D::Loader::MaterialDefinition::TRANSPARENCY);

  for(auto&& textureInformation : mTextureInformations)
  {
    if(!textureInformation.mTexture)
    {
      continue;
    }
    materialFlag |= textureInformation.mSemantic;
  }

  if(mMaterialFlag != materialFlag || mShaderData.mVertexShaderSource.empty() || mShaderData.mFragmentShaderSource.empty())
  {
    mModifyFlag |= MaterialModifyObserver::ModifyFlag::SHADER;

    mMaterialFlag                     = materialFlag;
    mShaderData.mVertexShaderSource   = SHADER_DEFAULT_PHYSICALLY_BASED_SHADER_VERT.data();
    mShaderData.mFragmentShaderSource = SHADER_DEFAULT_PHYSICALLY_BASED_SHADER_FRAG.data();

    std::vector<std::string> defines;
    defines.push_back(THREE_TEX_KEYWORD.data());
    for(auto&& textureInformation : mTextureInformations)
    {
      if(!textureInformation.mTexture)
      {
        continue;
      }
      defines.push_back(textureInformation.mDefineKeyword);
    }
    defines.push_back(GLTF_CHANNELS_KEYWORD.data());

    for(const auto& define : defines)
    {
      Scene3D::Loader::ShaderDefinition::ApplyDefine(mShaderData.mFragmentShaderSource, define);
    }
  }

  // Finish to make all the material flag according to the gltf2-util.
  // Then make defines as fallowing shader-definition-factory.

  // The renderer State below can be used in primitive to set renderer properties.

  // for renderer setting
  mRendererState = Scene3D::Loader::RendererState::DEPTH_TEST;
  if(!mDoubleSided)
  {
    mRendererState |= Scene3D::Loader::RendererState::CULL_BACK;
  }

  if(hasTransparency)
  {
    mRendererState = (mRendererState | Scene3D::Loader::RendererState::ALPHA_BLEND);
  }
}

bool Material::IsResourceReady()
{
  for(auto&& textureInformation : mTextureInformations)
  {
    if(!textureInformation.IsReady())
    {
      return false;
    }
  }
  return true;
}

void Material::SetRendererUniform(Dali::Renderer renderer)
{
  renderer.RegisterProperty("uColorFactor", mTextureInformations[TextureIndex::BASE_COLOR].mFactor);
  renderer.RegisterProperty("uMetallicFactor", mTextureInformations[TextureIndex::METALLIC_ROUGHNESS].mFactor.x);
  renderer.RegisterProperty("uRoughnessFactor", mTextureInformations[TextureIndex::METALLIC_ROUGHNESS].mFactor.y);
  renderer.RegisterProperty("uNormalScale", mTextureInformations[TextureIndex::NORMAL].mFactor.x);
  if(mTextureInformations[TextureIndex::OCCLUSION].mTexture)
  {
    renderer.RegisterProperty("uOcclusionStrength", mTextureInformations[TextureIndex::OCCLUSION].mFactor.x);
  }
  renderer.RegisterProperty("uEmissiveFactor", Vector3(mTextureInformations[TextureIndex::EMISSIVE].mFactor));
  float dielectricSpecular = (Dali::Equals(mIor, -1.0)) ? 0.04f : powf((mIor - 1.0f) / (mIor + 1.0f), 2.0f);
  renderer.RegisterProperty("uDielectricSpecular", dielectricSpecular);
  renderer.RegisterProperty("uSpecularFactor", mTextureInformations[TextureIndex::SPECULAR].mFactor.x);
  renderer.RegisterProperty("uSpecularColorFactor", Vector3(mTextureInformations[TextureIndex::SPECULAR_COLOR].mFactor));

  float opaque = mIsOpaque ? 1.0f : 0.0f;
  float mask   = mIsMask ? 1.0f : 0.0f;
  renderer.RegisterProperty("uOpaque", opaque);
  renderer.RegisterProperty("uMask", mask);
  renderer.RegisterProperty("uAlphaThreshold", mAlphaCutoff);

  renderer.RegisterProperty("uCubeMatrix", Matrix::IDENTITY);

  renderer.RegisterProperty(Scene3D::Loader::NodeDefinition::GetIblMaxLodUniformName().data(), 6.f);
  renderer.RegisterProperty(Scene3D::Loader::NodeDefinition::GetIblScaleFactorUniformName().data(), 1.0f);
  renderer.RegisterProperty(Scene3D::Loader::NodeDefinition::GetIblYDirectionUniformName().data(), Vector3(1.0f, -1.0, 1.0));

  std::string lightCountPropertyName(Scene3D::Internal::Light::GetLightCountUniformName());
  renderer.RegisterProperty(lightCountPropertyName, 0);

  uint32_t maxLightCount = Scene3D::Internal::Light::GetMaximumEnabledLightCount();
  for(uint32_t i = 0; i < maxLightCount; ++i)
  {
    std::string lightDirectionPropertyName(Scene3D::Internal::Light::GetLightDirectionUniformName());
    lightDirectionPropertyName += "[" + std::to_string(i) + "]";
    renderer.RegisterProperty(lightDirectionPropertyName, Vector3::ZAXIS);

    std::string lightColorPropertyName(Scene3D::Internal::Light::GetLightColorUniformName());
    lightColorPropertyName += "[" + std::to_string(i) + "]";
    renderer.RegisterProperty(lightColorPropertyName, Vector3(Color::WHITE));
  }

  Scene3D::Loader::RendererState::Apply(mRendererState, renderer);
}

uint32_t Material::GetSpecularImageBasedLightTextureOffset()
{
  return OFFSET_FOR_SPECULAR_CUBE_TEXTURE;
}

uint32_t Material::GetDiffuseImageBasedLightTextureOffset()
{
  return OFFSET_FOR_DIFFUSE_CUBE_TEXTURE;
}

std::string_view Material::GetImageBasedLightScaleFactorName()
{
  return Dali::Scene3D::Loader::NodeDefinition::GetIblScaleFactorUniformName();
}

std::string_view Material::GetImageBasedLightMaxLodUniformName()
{
  return Dali::Scene3D::Loader::NodeDefinition::GetIblMaxLodUniformName();
}

void Material::ResetFlag()
{
  mModifyFlag = MaterialModifyObserver::ModifyFlag::NONE;
}

void Material::NotifyObserver()
{
  if(mModifyFlag != MaterialModifyObserver::ModifyFlag::NONE && IsResourceReady())
  {
    if(mObserverNotifying)
    {
      DALI_LOG_ERROR("Notify during observing is not allowed.");
      return;
    }

    Dali::Scene3D::Material handle(this); // Keep itself's life during notify

    // Copy the flag due to the flag can be changed during observe.
    MaterialModifyObserver::ModifyFlag copiedFlag = mModifyFlag;
    mModifyFlag                                   = MaterialModifyObserver::ModifyFlag::NONE;

    // Need to block mObserver container change during observe
    mObserverNotifying = true;
    for(uint32_t i = 0; i < mObservers.size(); ++i)
    {
      if(mObservers[i].second)
      {
        mObservers[i].first->OnMaterialModified(handle, copiedFlag);
      }
    }
    mObserverNotifying = false;

    // Resolve observer queue during notify
    mObservers.erase(std::remove_if(mObservers.begin(), mObservers.end(), [](auto& e) { return !e.second; }), mObservers.end());
  }
}

void Material::RequestTextureLoad(TextureInformation& textureInformation, const std::string& url)
{
  if(textureInformation.mUrl == url)
  {
    return;
  }

  textureInformation.mUrl = url;
  if(textureInformation.mLoadingTaskId != INVALID_INDEX)
  {
    mAsyncImageLoader.Cancel(textureInformation.mLoadingTaskId);
    textureInformation.mLoadingTaskId = INVALID_INDEX;
  }

  if(url.empty())
  {
    textureInformation.mTexture.Reset();
    return;
  }
  textureInformation.mLoadingTaskId = mAsyncImageLoader.Load(url);
}

void Material::TextureLoadComplete(uint32_t loadedTaskId, PixelData pixelData)
{
  for(auto&& textureInformation : mTextureInformations)
  {
    if(textureInformation.mLoadingTaskId != loadedTaskId)
    {
      continue;
    }

    if(pixelData)
    {
      textureInformation.mTexture = Texture::New(Dali::TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight());
      textureInformation.mTexture.Upload(pixelData);
    }
    textureInformation.mLoadingTaskId = INVALID_INDEX;
    break;
  }

  if(IsResourceReady())
  {
    ResourcesLoadComplete();
  }
}

void Material::ResourcesLoadComplete()
{
  mModifyFlag |= MaterialModifyObserver::ModifyFlag::TEXTURE;
  Apply();
}

} // namespace Internal

} // namespace Scene3D

} // namespace Dali
