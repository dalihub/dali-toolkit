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
#include <dali-scene3d/public-api/loader/environment-definition.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/environment-variable.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/threading/mutex.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/common/image-resource-loader.h>
#include <dali-scene3d/public-api/loader/environment-map-loader.h>
#include <dali-scene3d/public-api/loader/utils.h>

namespace
{
#define TOKEN_STRING(x) #x
std::string GetDaliImagePath()
{
  return (nullptr == DALI_IMAGE_DIR) ? Dali::EnvironmentVariable::GetEnvironmentVariable(TOKEN_STRING(DALI_IMAGE_DIR)) : DALI_IMAGE_DIR;
}

static constexpr float DEFAULT_INTENSITY = 1.0f;

} // unnamed namespace

namespace Dali::Scene3D::Loader
{
namespace
{
const char* PRE_COMPUTED_BRDF_TEXTURE_FILE_NAME = "brdfLUT.png";
}
PixelData EnvironmentDefinition::mBrdfPixelData;
Texture   EnvironmentDefinition::mBrdfTexture;
bool      EnvironmentDefinition::mIsBrdfLoaded = false;

Dali::Texture EnvironmentDefinition::GetBrdfTexture()
{
  if(!mBrdfTexture)
  {
    if(!mIsBrdfLoaded)
    {
      LoadBrdfTexture();
    }
    mBrdfTexture = Texture::New(TextureType::TEXTURE_2D, mBrdfPixelData.GetPixelFormat(), mBrdfPixelData.GetWidth(), mBrdfPixelData.GetHeight());
    mBrdfTexture.Upload(mBrdfPixelData);
  }
  return mBrdfTexture;
}

EnvironmentDefinition::RawData
EnvironmentDefinition::LoadRaw(const std::string& environmentsPath)
{
  RawData raw;
  auto    loadFn = [&environmentsPath](const std::string& path, EnvironmentMapData& environmentMapData) {
    if(path.empty())
    {
      environmentMapData.mPixelData.resize(6);
      for(auto& face : environmentMapData.mPixelData)
      {
        face.push_back(Dali::Scene3D::Internal::ImageResourceLoader::GetEmptyPixelDataWhiteRGB());
      }
      environmentMapData.SetEnvironmentMapType(Dali::Scene3D::EnvironmentMapType::CUBEMAP);
    }
    else if(!LoadEnvironmentMap(environmentsPath + path, environmentMapData))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Failed to load cubemap texture from '" << path << "'.";
    }
  };

  loadFn(mDiffuseMapPath, raw.mDiffuse);
  loadFn(mSpecularMapPath, raw.mSpecular);

  if(mUseBrdfTexture)
  {
    LoadBrdfTexture();
  }
  return raw;
}

EnvironmentDefinition::Textures EnvironmentDefinition::Load(RawData&& raw)
{
  Textures textures;

  // This texture should have 6 faces and only one mipmap
  if(!raw.mDiffuse.mPixelData.empty())
  {
    textures.mDiffuse = raw.mDiffuse.GetTexture();
  }

  // This texture should have 6 faces and 6 mipmaps
  if(!raw.mSpecular.mPixelData.empty())
  {
    textures.mSpecular             = raw.mSpecular.GetTexture();
    textures.mSpecularMipmapLevels = raw.mSpecular.GetMipmapLevels();
  }

  if(mUseBrdfTexture)
  {
    textures.mBrdf = GetBrdfTexture();
  }
  return textures;
}

float EnvironmentDefinition::GetDefaultIntensity()
{
  return DEFAULT_INTENSITY;
}

void EnvironmentDefinition::LoadBrdfTexture()
{
  static Dali::Mutex mutex;
  {
    Mutex::ScopedLock lock(mutex);
    if(!mIsBrdfLoaded)
    {
      Devel::PixelBuffer pixelBuffer = LoadImageFromFile(GetDaliImagePath() + PRE_COMPUTED_BRDF_TEXTURE_FILE_NAME);
      if(pixelBuffer)
      {
        mBrdfPixelData = Devel::PixelBuffer::Convert(pixelBuffer);
        mIsBrdfLoaded  = true;
      }
    }
  }
}

} // namespace Dali::Scene3D::Loader