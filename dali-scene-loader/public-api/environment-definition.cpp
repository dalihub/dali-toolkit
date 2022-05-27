/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include "dali-scene-loader/public-api/environment-definition.h"
#include "dali-scene-loader/public-api/utils.h"

namespace
{
#define TOKEN_STRING(x) #x
std::string GetDaliImagePath()
{
  return (nullptr == DALI_IMAGE_DIR) ? Dali::EnvironmentVariable::GetEnvironmentVariable(TOKEN_STRING(DALI_IMAGE_DIR)) : DALI_IMAGE_DIR;
}
} // unnamed namespace

namespace Dali
{
namespace SceneLoader
{
namespace
{
const std::string PRE_COMPUTED_BRDF_TEXTURE_FILE_NAME = "brdfLUT.png";
}

EnvironmentDefinition::RawData
EnvironmentDefinition::LoadRaw(const std::string& environmentsPath) const
{
  RawData raw;
  auto    loadFn = [&environmentsPath](const std::string& path, CubeData& cd) {
    if(path.empty())
    {
      cd.data.resize(6);
      for(auto& face : cd.data)
      {
        face.push_back(PixelData::New(new uint8_t[3]{0xff, 0xff, 0xff}, 3, 1, 1, Pixel::RGB888, PixelData::DELETE_ARRAY));
      }
    }
    else if(!LoadCubeMapData(environmentsPath + path, cd))
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
  if(!raw.mDiffuse.data.empty())
  {
    textures.mDiffuse = raw.mDiffuse.CreateTexture();
  }

  // This texture should have 6 faces and 6 mipmaps
  if(!raw.mSpecular.data.empty())
  {
    textures.mSpecular = raw.mSpecular.CreateTexture();
  }

  if(raw.mBrdf)
  {
    textures.mBrdf = Texture::New(TextureType::TEXTURE_2D, raw.mBrdf.GetPixelFormat(), raw.mBrdf.GetWidth(), raw.mBrdf.GetHeight());
    textures.mBrdf.Upload(raw.mBrdf);
  }
  return textures;
}

} // namespace SceneLoader
} // namespace Dali
