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

#include <dali/integration-api/debug.h>

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

static constexpr uint32_t INDEX_FOR_LIGHT_CONSTRAINT_TAG = 10;

Texture MakeEmptyTexture()
{
  PixelData pixelData = PixelData::New(new uint8_t[3]{0xff, 0xff, 0xff}, 3, 1, 1, Pixel::RGB888, PixelData::DELETE_ARRAY);
  Texture texture            = Texture::New(TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight());
  texture.Upload(pixelData, 0, 0, 0, 0, pixelData.GetWidth(), pixelData.GetHeight());

  return texture;
}
} // unnamed namespace

ModelPrimitivePtr ModelPrimitive::New()
{
  ModelPrimitivePtr primitive = new ModelPrimitive();

  primitive->Initialize();

  return primitive;
}

ModelPrimitive::ModelPrimitive()
: mShaderManager(new Scene3D::Loader::ShaderManager())
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
    UpdateShadowMapTexture();
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

void ModelPrimitive::SetShadowMapTexture(Dali::Texture shadowMapTexture)
{
  mShadowMapTexture = shadowMapTexture;
  UpdateShadowMapTexture();
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

void ModelPrimitive::UpdateShader(Scene3D::Loader::ShaderManagerPtr shaderManager)
{
  if(mShaderManager != shaderManager)
  {
    mShaderManager = (shaderManager) ? shaderManager : new Scene3D::Loader::ShaderManager();
    if(mMaterial && GetImplementation(mMaterial).IsResourceReady())
    {
      ApplyMaterialToRenderer(MaterialModifyObserver::ModifyFlag::SHADER);
    }
  }
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

void ModelPrimitive::SetBlendShapeOptions(bool hasPositions, bool hasNormals, bool hasTangents, Scene3D::Loader::BlendShapes::Version version)
{
  mHasPositions      = hasPositions;
  mHasNormals        = hasNormals;
  mHasTangents       = hasTangents;
  mBlendShapeVersion = version;
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
  if(!mMaterial)
  {
    return;
  }

  uint32_t shaderFlag = (flag & static_cast<uint32_t>(MaterialModifyObserver::ModifyFlag::SHADER));
  if(mIsMaterialChanged || shaderFlag == static_cast<uint32_t>(MaterialModifyObserver::ModifyFlag::SHADER))
  {
    Scene3D::Loader::ShaderOption shaderOption = GetImplementation(mMaterial).GetShaderOption();

    shaderOption.AddOption(Scene3D::Loader::ShaderOption::Type::VEC4_TANGENT);
    if(mHasSkinning)
    {
      shaderOption.AddOption(Scene3D::Loader::ShaderOption::Type::SKINNING);
    }
    if(mHasPositions || mHasNormals || mHasTangents)
    {
      if(mHasPositions)
      {
        shaderOption.AddOption(Scene3D::Loader::ShaderOption::Type::MORPH_POSITION);
      }
      if(mHasNormals)
      {
        shaderOption.AddOption(Scene3D::Loader::ShaderOption::Type::MORPH_NORMAL);
      }
      if(mHasTangents)
      {
        shaderOption.AddOption(Scene3D::Loader::ShaderOption::Type::MORPH_TANGENT);
      }
      if(mBlendShapeVersion == Scene3D::Loader::BlendShapes::Version::VERSION_2_0)
      {
        shaderOption.AddOption(Scene3D::Loader::ShaderOption::Type::MORPH_VERSION_2_0);
      }
    }

    mShader.Reset();
    mShader = mShaderManager->ProduceShader(shaderOption);

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

    if(!mShadowMapTexture)
    {
      mShadowMapTexture = MakeEmptyTexture();
    }
    mTextureSet.SetTexture(textureCount++, mShadowMapTexture);

    Texture brdfTexture = Scene3D::Loader::EnvironmentDefinition::GetBrdfTexture();
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

  for(auto* observer : mObservers)
  {
    observer->OnRendererCreated(mRenderer);
  }
}

void ModelPrimitive::UpdateShadowMapTexture()
{
  if(mRenderer && mMaterial)
  {
    Dali::TextureSet textures = mRenderer.GetTextures();
    if(!textures)
    {
      return;
    }

    uint32_t textureCount = textures.GetTextureCount();
    if(mShadowMapTexture &&
       textureCount >= GetImplementation(mMaterial).GetShadowMapTextureOffset() &&
       textures.GetTexture(textureCount - GetImplementation(mMaterial).GetShadowMapTextureOffset()) != mShadowMapTexture)
    {
      Dali::TextureSet newTextures = Dali::TextureSet::New();

      for(uint32_t index = 0u; index < textureCount; ++index)
      {
        Dali::Texture texture = textures.GetTexture(index);
        if(index == textureCount - GetImplementation(mMaterial).GetShadowMapTextureOffset())
        {
          texture = (!!mShadowMapTexture) ? mShadowMapTexture : MakeEmptyTexture();
        }

        newTextures.SetTexture(index, texture);
        newTextures.SetSampler(index, textures.GetSampler(index));
      }

      mRenderer.SetTextures(newTextures);
    }
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
  if(mMaterial)
  {
    mRenderer.RegisterProperty(GetImplementation(mMaterial).GetImageBasedLightScaleFactorName().data(), mIblScaleFactor);
    mRenderer.RegisterProperty(GetImplementation(mMaterial).GetImageBasedLightMaxLodUniformName().data(), static_cast<float>(mSpecularMipmapLevels));
    GetImplementation(mMaterial).SetRendererUniform(mRenderer);
  }
}

} // namespace Internal

} // namespace Scene3D

} // namespace Dali
