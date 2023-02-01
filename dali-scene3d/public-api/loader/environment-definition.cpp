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

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/environment-variable.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>

// INTERNAL INCLUDES
#include "dali-scene3d/public-api/loader/environment-map-loader.h"
#include "dali-scene3d/public-api/loader/environment-definition.h"
#include "dali-scene3d/public-api/loader/utils.h"

namespace
{
#define TOKEN_STRING(x) #x
std::string GetDaliImagePath()
{
  return (nullptr == DALI_IMAGE_DIR) ? Dali::EnvironmentVariable::GetEnvironmentVariable(TOKEN_STRING(DALI_IMAGE_DIR)) : DALI_IMAGE_DIR;
}

static constexpr float DEFAULT_INTENSITY = 1.0f;
} // unnamed namespace

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
namespace
{
const std::string PRE_COMPUTED_BRDF_TEXTURE_FILE_NAME = "brdfLUT.png";
}

EnvironmentDefinition::RawData
EnvironmentDefinition::LoadRaw(const std::string& environmentsPath) const
{
  RawData raw;
  auto    loadFn = [&environmentsPath](const std::string& path, EnvironmentMapData& environmentMapData) {
    if(path.empty())
    {
      environmentMapData.mPixelData.resize(6);
      for(auto& face : environmentMapData.mPixelData)
      {
        face.push_back(PixelData::New(new uint8_t[3]{0xff, 0xff, 0xff}, 3, 1, 1, Pixel::RGB888, PixelData::DELETE_ARRAY));
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
    Devel::PixelBuffer pixelBuffer = LoadImageFromFile(GetDaliImagePath() + PRE_COMPUTED_BRDF_TEXTURE_FILE_NAME);
    if(pixelBuffer)
    {
      raw.mBrdf = Devel::PixelBuffer::Convert(pixelBuffer);
    }
  }
  return raw;
}

EnvironmentDefinition::Textures EnvironmentDefinition::Load(RawData&& raw) const
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
    textures.mSpecular = raw.mSpecular.GetTexture();
  }

  if(raw.mBrdf)
  {
    textures.mBrdf = Texture::New(TextureType::TEXTURE_2D, raw.mBrdf.GetPixelFormat(), raw.mBrdf.GetWidth(), raw.mBrdf.GetHeight());
    textures.mBrdf.Upload(raw.mBrdf);
  }
  return textures;
}

float EnvironmentDefinition::GetDefaultIntensity()
{
  return DEFAULT_INTENSITY;
}

} // namespace Loader
} // namespace Scene3D
} // namespace Dali
