/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// FILE HEADER
#include <dali-scene3d/public-api/loader/environment-map-data.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/string-utils.h>
#include <dali/integration-api/texture-integ.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/common/image-resource-loader.h>
#include <dali-scene3d/internal/graphics/builtin-shader-extern-gen.h>

namespace Dali::Scene3D::Loader
{
Texture EnvironmentMapData::GetTexture()
{
  if(mEnvironmentMapTexture)
  {
    return mEnvironmentMapTexture;
  }

  if(!mPixelData.empty())
  {
    if(mEnvironmentMapType == Scene3D::EnvironmentMapType::CUBEMAP)
    {
      // Check for the default case, that we might share the graphic resources
      if(mPixelData[0].size() > 0u)
      {
        if(mPixelData[0][0] == Dali::Scene3D::Internal::ImageResourceLoader::GetEmptyPixelDataWhiteRGB())
        {
          mEnvironmentMapTexture = Dali::Scene3D::Internal::ImageResourceLoader::GetEmptyCubeTextureWhiteRGB();
        }
        else
        {
          mEnvironmentMapTexture = Texture::New(TextureType::TEXTURE_CUBE, mPixelData[0][0].GetPixelFormat(), mPixelData[0][0].GetWidth(), mPixelData[0][0].GetHeight());
          for(size_t iSide = 0u, iEndSize = mPixelData.size(); iSide < iEndSize; ++iSide)
          {
            auto& side = mPixelData[iSide];
            for(size_t iMipLevel = 0u, iEndMipLevel = mPixelData[0].size(); iMipLevel < iEndMipLevel; ++iMipLevel)
            {
#if defined(GPU_MEMORY_PROFILE_ENABLED)
              if(iSide == 0u && iMipLevel == 0u)
              {
                // Their is no good way to get url here. Just notify it is environment image.
                Dali::Integration::TextureUploadWithContent(mEnvironmentMapTexture,
                                                            side[iMipLevel],
                                                            "Scene3D::EnvironmentMapData(CUBEMAP)",
                                                            static_cast<Dali::Integration::TextureContextTypeHint::Type>(Dali::Integration::TextureContextTypeHint::SCENE3D_ENVIRONMENT_CUBEMAP));
              }
              else
              {
                mEnvironmentMapTexture.Upload(side[iMipLevel], CubeMapLayer::POSITIVE_X + iSide, iMipLevel, 0u, 0u, side[iMipLevel].GetWidth(), side[iMipLevel].GetHeight());
              }
#else
              mEnvironmentMapTexture.Upload(side[iMipLevel], CubeMapLayer::POSITIVE_X + iSide, iMipLevel, 0u, 0u, side[iMipLevel].GetWidth(), side[iMipLevel].GetHeight());
#endif
            }
          }
        }
      }
    }
    else
    {
      mEnvironmentMapTexture = Texture::New(TextureType::TEXTURE_2D, mPixelData[0][0].GetPixelFormat(), mPixelData[0][0].GetWidth(), mPixelData[0][0].GetHeight());
#if defined(GPU_MEMORY_PROFILE_ENABLED)
      // Their is no good way to get url here. Just notify it is environment image.
      Dali::Integration::TextureUploadWithContent(mEnvironmentMapTexture,
                                                  mPixelData[0][0],
                                                  "Scene3D::EnvironmentMapData(EQUIRECTANGULAR)",
                                                  static_cast<Dali::Integration::TextureContextTypeHint::Type>(Dali::Integration::TextureContextTypeHint::SCENE3D_ENVIRONMENT_EQUIRECTANGULAR));
#else
      mEnvironmentMapTexture.Upload(mPixelData[0][0], 0, 0, 0, 0, mPixelData[0][0].GetWidth(), mPixelData[0][0].GetHeight());
#endif
    }

    // If mipmap is not defined explicitly, use GenerateMipmaps.
    // TODO: Maybe we can use better way to know it already has mipmap or not.
    if(mPixelData.size() > 0u && mPixelData[0].size() == 1u)
    {
      mEnvironmentMapTexture.GenerateMipmaps();
    }
  }
  return mEnvironmentMapTexture;
}

} // namespace Dali::Scene3D::Loader
