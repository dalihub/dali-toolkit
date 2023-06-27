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
#include <dali-scene3d/internal/model-components/model-primitive-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/light/light-impl.h>
#include <dali-scene3d/internal/model-components/material-impl.h>
#include <dali-scene3d/public-api/loader/environment-definition.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
namespace
{
/**
 * Creates control through type registry
 */
BaseHandle Create()
{
  return Scene3D::ModelPrimitive::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Scene3D::ModelPrimitive, Dali::BaseHandle, Create);
DALI_TYPE_REGISTRATION_END()

constexpr std::string_view MORPH_POSITION_KEYWORD    = "MORPH_POSITION";
constexpr std::string_view MORPH_NORMAL_KEYWORD      = "MORPH_NORMAL";
constexpr std::string_view MORPH_TANGENT_KEYWORD     = "MORPH_TANGENT";
constexpr std::string_view MORPH_VERSION_2_0_KEYWORD = "MORPH_VERSION_2_0";

static constexpr uint32_t INDEX_FOR_LIGHT_CONSTRAINT_TAG = 10;
} // unnamed namespace

ModelPrimitivePtr ModelPrimitive::New()
{
  ModelPrimitivePtr primitive = new ModelPrimitive();

  primitive->Initialize();

  return primitive;
}

ModelPrimitive::ModelPrimitive()
{
}

ModelPrimitive::~ModelPrimitive()
{
  if(mMaterial)
  {
    GetImplementation(mMaterial).RemoveObserver(this);
  }
  mMaterial.Reset();
}

void ModelPrimitive::Initialize()
{
  mLights.resize(Scene3D::Internal::Light::GetMaximumEnabledLightCount());
}

void ModelPrimitive::SetRenderer(Dali::Renderer renderer)
{
  mRenderer   = renderer;
  mGeometry   = renderer.GetGeometry();
  mTextureSet = renderer.GetTextures();
  mShader     = renderer.GetShader();
}

Dali::Renderer ModelPrimitive::GetRenderer() const
{
  return mRenderer;
}

void ModelPrimitive::SetGeometry(Dali::Geometry geometry)
{
  mGeometry = geometry;
  CreateRenderer();
}

Dali::Geometry ModelPrimitive::GetGeometry() const
{
  return mGeometry;
}

void ModelPrimitive::SetMaterial(Dali::Scene3D::Material material, bool updateRenderer)
{
  if(!material)
  {
    return;
  }

  if(mMaterial != material)
  {
    // Stop observe from previous material.
    if(mMaterial)
    {
      GetImplementation(mMaterial).RemoveObserver(this);
    }

    mMaterial = material;

    // Start observe from new material.

    if(mMaterial)
    {
      GetImplementation(mMaterial).AddObserver(this);
    }

    if(updateRenderer)
    {
      mIsMaterialChanged = true;
      if(GetImplementation(mMaterial).IsResourceReady())
      {
        GetImplementation(mMaterial).UpdateMaterialData();
        ApplyMaterialToRenderer();
      }
    }
    UpdateImageBasedLightTexture();
  }
}

Dali::Scene3D::Material ModelPrimitive::GetMaterial() const
{
  return mMaterial;
}

void ModelPrimitive::AddPrimitiveObserver(ModelPrimitiveModifyObserver* observer)
{
  mObservers.insert(observer);
}

void ModelPrimitive::RemovePrimitiveObserver(ModelPrimitiveModifyObserver* observer)
{
  mObservers.erase(observer);
}

void ModelPrimitive::SetImageBasedLightTexture(Dali::Texture diffuseTexture, Dali::Texture specularTexture, float iblScaleFactor, uint32_t specularMipmapLevels)
{
  mDiffuseTexture       = diffuseTexture;
  mSpecularTexture      = specularTexture;
  mIblScaleFactor       = iblScaleFactor;
  mSpecularMipmapLevels = specularMipmapLevels;

  UpdateImageBasedLightTexture();
}

void ModelPrimitive::SetImageBasedLightScaleFactor(float iblScaleFactor)
{
  mIblScaleFactor = iblScaleFactor;
  if(mRenderer && mMaterial)
  {
    mRenderer.RegisterProperty(GetImplementation(mMaterial).GetImageBasedLightScaleFactorName().data(), iblScaleFactor);
  }
}

void ModelPrimitive::AddLight(Scene3D::Light light, uint32_t lightIndex)
{
  if(mLights[lightIndex])
  {
    RemoveLight(lightIndex);
  }

  mLights[lightIndex] = light;
  // TODO  Remove light at lightIndex if it is already set.
  if(mRenderer && mMaterial)
  {
    mLightCount++;
    std::string lightCountPropertyName(Scene3D::Internal::Light::GetLightCountUniformName());
    mRenderer.RegisterProperty(lightCountPropertyName, mLightCount);

    std::string lightDirectionPropertyName(Scene3D::Internal::Light::GetLightDirectionUniformName());
    lightDirectionPropertyName += "[" + std::to_string(lightIndex) + "]";
    auto             lightDirectionPropertyIndex = mRenderer.RegisterProperty(lightDirectionPropertyName, Vector3::ZAXIS);
    Dali::Constraint lightDirectionConstraint    = Dali::Constraint::New<Vector3>(mRenderer, lightDirectionPropertyIndex, [](Vector3& output, const PropertyInputContainer& inputs)
                                                                               { output = inputs[0]->GetQuaternion().Rotate(Vector3::ZAXIS); });
    lightDirectionConstraint.AddSource(Source{light, Dali::Actor::Property::WORLD_ORIENTATION});
    lightDirectionConstraint.ApplyPost();
    lightDirectionConstraint.SetTag(INDEX_FOR_LIGHT_CONSTRAINT_TAG + lightIndex);

    std::string lightColorPropertyName(Scene3D::Internal::Light::GetLightColorUniformName());
    lightColorPropertyName += "[" + std::to_string(lightIndex) + "]";
    auto             lightColorPropertyIndex = mRenderer.RegisterProperty(lightColorPropertyName, Vector3(Color::WHITE));
    Dali::Constraint lightColorConstraint    = Dali::Constraint::New<Vector3>(mRenderer, lightColorPropertyIndex, [](Vector3& output, const PropertyInputContainer& inputs)
                                                                           { output = Vector3(inputs[0]->GetVector4()); });
    lightColorConstraint.AddSource(Source{light, Dali::Actor::Property::COLOR});
    lightColorConstraint.ApplyPost();
    lightColorConstraint.SetTag(INDEX_FOR_LIGHT_CONSTRAINT_TAG + lightIndex);
  }
}

void ModelPrimitive::RemoveLight(uint32_t lightIndex)
{
  mLightCount--;
  std::string lightCountPropertyName(Scene3D::Internal::Light::GetLightCountUniformName());
  mRenderer.RegisterProperty(lightCountPropertyName, mLightCount);

  mRenderer.RemoveConstraints(INDEX_FOR_LIGHT_CONSTRAINT_TAG + lightIndex);

  mLights[lightIndex].Reset();
}

void ModelPrimitive::SetBlendShapeData(Scene3D::Loader::BlendShapes::BlendShapeData& data)
{
  mBlendShapeData = std::move(data);
  Scene3D::Loader::BlendShapes::ConfigureProperties(mBlendShapeData, mRenderer);
}

void ModelPrimitive::SetBlendShapeGeometry(Dali::Texture blendShapeGeometry)
{
  mBlendShapeGeometry = blendShapeGeometry;
}

void ModelPrimitive::SetBlendShapeOptions(bool hasPositions, bool hasNormals, bool hasTangents)
{
  mHasPositions = hasPositions;
  mHasNormals   = hasNormals;
  mHasTangents  = hasTangents;
}

void ModelPrimitive::SetSkinned(bool isSkinned)
{
  mHasSkinning = isSkinned;
}

// From MaterialModifyObserver

void ModelPrimitive::OnMaterialModified(Dali::Scene3D::Material material, MaterialModifyObserver::ModifyFlag flag)
{
  ApplyMaterialToRenderer(flag);
}

void ModelPrimitive::ApplyMaterialToRenderer(MaterialModifyObserver::ModifyFlag flag)
{
  uint32_t shaderFlag = (flag & static_cast<uint32_t>(MaterialModifyObserver::ModifyFlag::SHADER));
  if(mIsMaterialChanged || shaderFlag == static_cast<uint32_t>(MaterialModifyObserver::ModifyFlag::SHADER))
  {
    std::string vertexShader   = GetImplementation(mMaterial).GetVertexShader();
    std::string fragmentShader = GetImplementation(mMaterial).GetFragmentShader();

    std::vector<std::string> defines;
    defines.push_back("VEC4_TANGENT");
    if(mHasSkinning)
    {
      defines.push_back("SKINNING");
    }
    if(mHasPositions || mHasNormals || mHasTangents)
    {
      if(mHasPositions)
      {
        defines.push_back(MORPH_POSITION_KEYWORD.data());
      }
      if(mHasNormals)
      {
        defines.push_back(MORPH_NORMAL_KEYWORD.data());
      }
      if(mHasTangents)
      {
        defines.push_back(MORPH_TANGENT_KEYWORD.data());
      }
      if(mBlendShapeData.version == Scene3D::Loader::BlendShapes::Version::VERSION_2_0)
      {
        defines.push_back(MORPH_VERSION_2_0_KEYWORD.data());
      }
    }
    for(const auto& define : defines)
    {
      Scene3D::Loader::ShaderDefinition::ApplyDefine(vertexShader, define);
    }

    mShader.Reset();
    mShader = Shader::New(vertexShader, fragmentShader);

    if(!mRenderer)
    {
      CreateRenderer();
    }
    else
    {
      mRenderer.SetShader(mShader);
    }
  }

  uint32_t textureFlag = (flag & static_cast<uint32_t>(MaterialModifyObserver::ModifyFlag::TEXTURE));
  if(mIsMaterialChanged || textureFlag == static_cast<uint32_t>(MaterialModifyObserver::ModifyFlag::TEXTURE))
  {
    mTextureSet = GetImplementation(mMaterial).GetTextureSet();

    if(mBlendShapeGeometry)
    {
      TextureSet newTextureSet = TextureSet::New();
      newTextureSet.SetTexture(0u, mBlendShapeGeometry);

      const unsigned int numberOfTextures = mTextureSet.GetTextureCount();
      for(unsigned int index = 0u; index < numberOfTextures; ++index)
      {
        const unsigned int newIndex = index + 1u;
        newTextureSet.SetTexture(newIndex, mTextureSet.GetTexture(index));
        newTextureSet.SetSampler(newIndex, mTextureSet.GetSampler(index));
      }

      mTextureSet = newTextureSet;
    }

    uint32_t textureCount = mTextureSet.GetTextureCount();
    Texture  brdfTexture  = Scene3D::Loader::EnvironmentDefinition::GetBrdfTexture();
    if(!mSpecularTexture || !mDiffuseTexture)
    {
      Scene3D::Loader::EnvironmentMapData environmentMapData;
      environmentMapData.mPixelData.resize(6);
      for(auto& face : environmentMapData.mPixelData)
      {
        face.push_back(PixelData::New(new uint8_t[3]{0xff, 0xff, 0xff}, 3, 1, 1, Pixel::RGB888, PixelData::DELETE_ARRAY));
      }
      environmentMapData.SetEnvironmentMapType(Dali::Scene3D::EnvironmentMapType::CUBEMAP);
      Texture iblTexture = environmentMapData.GetTexture();
      mDiffuseTexture    = iblTexture;
      mSpecularTexture   = iblTexture;
    }

    mTextureSet.SetTexture(textureCount++, brdfTexture);
    mTextureSet.SetTexture(textureCount++, mDiffuseTexture);
    mTextureSet.SetTexture(textureCount, mSpecularTexture);

    auto specularSampler = Sampler::New();
    specularSampler.SetWrapMode(WrapMode::CLAMP_TO_EDGE, WrapMode::CLAMP_TO_EDGE, WrapMode::CLAMP_TO_EDGE);
    specularSampler.SetFilterMode(FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR);
    mTextureSet.SetSampler(textureCount, specularSampler);

    if(!mRenderer)
    {
      CreateRenderer();
    }
    else
    {
      mRenderer.SetTextures(mTextureSet);
    }
  }

  uint32_t uniformFlag = (flag & static_cast<uint32_t>(MaterialModifyObserver::ModifyFlag::UNIFORM));
  if(mIsMaterialChanged || uniformFlag == static_cast<uint32_t>(MaterialModifyObserver::ModifyFlag::UNIFORM))
  {
    if(!mRenderer)
    {
      mNeedToSetRendererUniform = true;
    }
    else
    {
      UpdateRendererUniform();
    }
  }
  mIsMaterialChanged = false;
}

void ModelPrimitive::CreateRenderer()
{
  if(!mShader || !mGeometry || !mTextureSet || mRenderer)
  {
    return;
  }

  mRenderer = Renderer::New(mGeometry, mShader);
  mRenderer.SetTextures(mTextureSet);
  UpdateRendererUniform();

  uint32_t maxLightCount = Scene3D::Internal::Light::GetMaximumEnabledLightCount();
  for(uint32_t i = 0; i < maxLightCount; ++i)
  {
    if(mLights[i])
    {
      AddLight(mLights[i], i);
    }
  }

  for(auto* observer : mObservers)
  {
    observer->OnRendererCreated(mRenderer);
  }
}

void ModelPrimitive::UpdateImageBasedLightTexture()
{
  if(mRenderer && mMaterial)
  {
    Dali::TextureSet textures = mRenderer.GetTextures();
    if(!textures)
    {
      return;
    }

    uint32_t textureCount = textures.GetTextureCount();
    if(textureCount > 2u &&
       (textures.GetTexture(textureCount - GetImplementation(mMaterial).GetDiffuseImageBasedLightTextureOffset()) != mDiffuseTexture ||
        textures.GetTexture(textureCount - GetImplementation(mMaterial).GetSpecularImageBasedLightTextureOffset()) != mSpecularTexture))
    {
      Dali::TextureSet newTextures = Dali::TextureSet::New();

      for(uint32_t index = 0u; index < textureCount; ++index)
      {
        Dali::Texture texture = textures.GetTexture(index);
        if(index == textureCount - GetImplementation(mMaterial).GetDiffuseImageBasedLightTextureOffset())
        {
          texture = mDiffuseTexture;
        }
        else if(index == textureCount - GetImplementation(mMaterial).GetSpecularImageBasedLightTextureOffset())
        {
          texture = mSpecularTexture;
        }

        newTextures.SetTexture(index, texture);
        newTextures.SetSampler(index, textures.GetSampler(index));
      }

      mRenderer.SetTextures(newTextures);
    }
    mRenderer.RegisterProperty(GetImplementation(mMaterial).GetImageBasedLightScaleFactorName().data(), mIblScaleFactor);
    mRenderer.RegisterProperty(GetImplementation(mMaterial).GetImageBasedLightMaxLodUniformName().data(), static_cast<float>(mSpecularMipmapLevels));
  }
}

void ModelPrimitive::UpdateRendererUniform()
{
  mRenderer.RegisterProperty(GetImplementation(mMaterial).GetImageBasedLightScaleFactorName().data(), mIblScaleFactor);
  mRenderer.RegisterProperty(GetImplementation(mMaterial).GetImageBasedLightMaxLodUniformName().data(), static_cast<float>(mSpecularMipmapLevels));
  GetImplementation(mMaterial).SetRendererUniform(mRenderer);
}

} // namespace Internal

} // namespace Scene3D

} // namespace Dali
