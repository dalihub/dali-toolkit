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

// FILE HEADER
#include "dali-scene-loader/public-api/cube-data.h"

// EXTERNAL INCLUDES
#include "dali/public-api/rendering/texture.h"

namespace Dali
{
namespace SceneLoader
{

Texture CubeData::CreateTexture() const
{
  Texture texture = Texture::New(TextureType::TEXTURE_CUBE, data[0][0].GetPixelFormat(), data[0][0].GetWidth(), data[0][0].GetHeight());
  for(size_t iSide = 0u, iEndSize = data.size(); iSide < iEndSize; ++iSide)
  {
    auto& side = data[iSide];
    for(size_t iMipLevel = 0u, iEndMipLevel = data[0].size(); iMipLevel < iEndMipLevel; ++iMipLevel)
    {
      texture.Upload(side[iMipLevel], CubeMapLayer::POSITIVE_X + iSide, iMipLevel, 0u, 0u, side[iMipLevel].GetWidth(), side[iMipLevel].GetHeight());
    }
  }

  // If mipmap is not defined explicitly, use GenerateMipmaps.
  // TODO: Maybe we can use better way to know it already has mipmap or not.
  if(data.size() > 0u && data[0].size() == 1u)
  {
    texture.GenerateMipmaps();
  }

  return texture;
}

} // namespace SceneLoader
} // namespace Dali
