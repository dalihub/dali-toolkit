/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include "dali-scene-loader/public-api/ktx-loader.h"

 // EXTERNAL INCLUDES
#include "dali/public-api/rendering/texture.h"
#include <fstream>
#include <memory>

namespace Dali
{

namespace
{

// http://github.khronos.org/KTX-Specification/
const uint8_t KTX_ID_HEAD[] = { 0xAB, 0x4B, 0x54, 0x58, 0x20 };
const uint8_t KTX_ID_TAIL[] = { 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };

const uint8_t KTX_VERSION_1_1[] = { 0x31, 0x31 };
const uint8_t KTX_VERSION_2_0[] = { 0x32, 0x30 };

static_assert(sizeof(KTX_ID_HEAD) + sizeof(KTX_ID_TAIL) == 10);
static_assert(sizeof(KTX_VERSION_1_1) == 2);
static_assert(sizeof(KTX_VERSION_2_0) == sizeof(KTX_VERSION_1_1));

void FreeBuffer(uint8_t* buffer)
{
  delete[] buffer;
}
} // namespace

namespace SceneLoader
{
struct KtxFileHeader
{
  uint8_t   identifier[12];
  uint32_t  endianness;
  uint32_t  glType;    //(UNSIGNED_BYTE, UNSIGNED_SHORT_5_6_5, etc.)
  uint32_t  glTypeSize;
  uint32_t  glFormat;  //(RGB, RGBA, BGRA, etc.)
  uint32_t  glInternalFormat; //For uncompressed textures, specifies the internalformat parameter passed to glTexStorage*D or glTexImage*D
  uint32_t  glBaseInternalFormat;
  uint32_t  pixelWidth;
  uint32_t  pixelHeight;
  uint32_t  pixelDepth;
  uint32_t  numberOfArrayElements;
  uint32_t  numberOfFaces; //Cube map faces are stored in the order: +X, -X, +Y, -Y, +Z, -Z.
  uint32_t  numberOfMipmapLevels;
  uint32_t  bytesOfKeyValueData;

  bool IsIdentifierValid() const
  {
    return std::equal(KTX_ID_HEAD, std::end(KTX_ID_HEAD), identifier) &&
      (std::equal(KTX_VERSION_1_1, std::end(KTX_VERSION_1_1), identifier + sizeof(KTX_ID_HEAD)) ||
        std::equal(KTX_VERSION_2_0, std::end(KTX_VERSION_2_0), identifier + sizeof(KTX_ID_HEAD))) &&
      std::equal(KTX_ID_TAIL, std::end(KTX_ID_TAIL), identifier + (sizeof(KTX_ID_HEAD) + sizeof(KTX_VERSION_1_1)));
  }
};

/**
 * Convert KTX format to Pixel::Format
 */
bool ConvertPixelFormat(const uint32_t ktxPixelFormat, Pixel::Format& format)
{
  switch (ktxPixelFormat)
  {
  case 0x93B0: // GL_COMPRESSED_RGBA_ASTC_4x4
  {
    format = Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR;
    break;
  }
  case 0x93B1: // GL_COMPRESSED_RGBA_ASTC_5x4
  {
    format = Pixel::COMPRESSED_RGBA_ASTC_5x4_KHR;
    break;
  }
  case 0x93B2: // GL_COMPRESSED_RGBA_ASTC_5x5
  {
    format = Pixel::COMPRESSED_RGBA_ASTC_5x5_KHR;
    break;
  }
  case 0x93B3: // GL_COMPRESSED_RGBA_ASTC_6x5
  {
    format = Pixel::COMPRESSED_RGBA_ASTC_6x5_KHR;
    break;
  }
  case 0x93B4: // GL_COMPRESSED_RGBA_ASTC_6x6
  {
    format = Pixel::COMPRESSED_RGBA_ASTC_6x6_KHR;
    break;
  }
  case 0x93B5: // GL_COMPRESSED_RGBA_ASTC_8x5
  {
    format = Pixel::COMPRESSED_RGBA_ASTC_8x5_KHR;
    break;
  }
  case 0x93B6: // GL_COMPRESSED_RGBA_ASTC_8x6
  {
    format = Pixel::COMPRESSED_RGBA_ASTC_8x6_KHR;
    break;
  }
  case 0x93B7: // GL_COMPRESSED_RGBA_ASTC_8x8
  {
    format = Pixel::COMPRESSED_RGBA_ASTC_8x8_KHR;
    break;
  }
  case 0x93B8: // GL_COMPRESSED_RGBA_ASTC_10x5
  {
    format = Pixel::COMPRESSED_RGBA_ASTC_10x5_KHR;
    break;
  }
  case 0x93B9: // GL_COMPRESSED_RGBA_ASTC_10x6
  {
    format = Pixel::COMPRESSED_RGBA_ASTC_10x6_KHR;
    break;
  }
  case 0x93BA: // GL_COMPRESSED_RGBA_ASTC_10x8
  {
    format = Pixel::COMPRESSED_RGBA_ASTC_10x8_KHR;
    break;
  }
  case 0x93BB: // GL_COMPRESSED_RGBA_ASTC_10x10
  {
    format = Pixel::COMPRESSED_RGBA_ASTC_10x10_KHR;
    break;
  }
  case 0x93BC: // GL_COMPRESSED_RGBA_ASTC_12x10
  {
    format = Pixel::COMPRESSED_RGBA_ASTC_12x10_KHR;
    break;
  }
  case 0x93BD: // GL_COMPRESSED_RGBA_ASTC_12x12
  {
    format = Pixel::COMPRESSED_RGBA_ASTC_12x12_KHR;
    break;
  }
  case 0x881B: // GL_RGB16F
  {
    format = Pixel::RGB16F;
    break;
  }
  case 0x8815: // GL_RGB32F
  {
    format = Pixel::RGB32F;
    break;
  }
  case 0x8C3A: // GL_R11F_G11F_B10F
  {
    format = Pixel::RGB32F;
    break;
  }
  case 0x8D7C: // GL_RGBA8UI
  {
    format = Pixel::RGBA8888;
    break;
  }
  case 0x8D7D: // GL_RGB8UI
  {
    format = Pixel::RGB888;
    break;
  }
  default:
  {
    return false;
  }
  }

  return true;
}

Texture CubeData::CreateTexture() const
{
  Texture texture = Texture::New(TextureType::TEXTURE_CUBE, data[0][0].GetPixelFormat(),
    data[0][0].GetWidth(), data[0][0].GetHeight());
  for (size_t iSide = 0u, iEndSize = data.size(); iSide < iEndSize; ++iSide)
  {
    auto& side = data[iSide];
    for (size_t iMipLevel = 0u, iEndMipLevel = data[0].size(); iMipLevel < iEndMipLevel; ++iMipLevel)
    {
      texture.Upload(side[iMipLevel], CubeMapLayer::POSITIVE_X + iSide, iMipLevel,
        0u, 0u, side[iMipLevel].GetWidth(), side[iMipLevel].GetHeight());
    }
  }

  return texture;
}

bool LoadCubeMapData(const std::string& path, CubeData& cubedata)
{
  std::fstream fp(path, std::ios::in | std::ios::binary);
  if (fp.is_open() == false)
  {
    return false;
  }

  KtxFileHeader header;
  if (fp.read(reinterpret_cast<char*>(&header), sizeof(KtxFileHeader)).good() == false)
  {
    return false;
  }

  if (!header.IsIdentifierValid())
  {
    return false;
  }

  // Skip the key-values:
  if (fp.seekg(header.bytesOfKeyValueData, fp.cur).good() == false)
  {
    return false;
  }

  header.numberOfMipmapLevels = std::max(header.numberOfMipmapLevels, 1u);
  header.numberOfArrayElements = std::max(header.numberOfArrayElements, 1u);
  header.pixelDepth = std::max(header.pixelDepth, 1u);
  header.pixelHeight = std::max(header.pixelHeight, 1u);

  cubedata.data.resize(header.numberOfFaces);
  for (uint32_t face = 0u; face < header.numberOfFaces; ++face)
  {
    cubedata.data[face].resize(header.numberOfMipmapLevels);
  }

  Pixel::Format daliformat = Pixel::RGB888;

  ConvertPixelFormat(header.glInternalFormat, daliformat);

  for (uint32_t mipmapLevel = 0u; mipmapLevel < header.numberOfMipmapLevels; ++mipmapLevel)
  {
    uint32_t byteSize = 0u;
    if (fp.read(reinterpret_cast<char*>(&byteSize), sizeof(byteSize)).good() == false)
    {
      return false;
    }

    if (0u != byteSize % 4u)
    {
      byteSize += 4u - byteSize % 4u;
    }

    for (uint32_t arrayElement = 0u; arrayElement < header.numberOfArrayElements; ++arrayElement) //arrayElement must be 0 or 1
    {
      for (uint32_t face = 0u; face < header.numberOfFaces; ++face)
      {
        std::unique_ptr<uint8_t, void(*)(uint8_t*)>img(new uint8_t[byteSize], FreeBuffer);
        if (fp.read(reinterpret_cast<char*>(img.get()), byteSize).good() == false)
        {
          return false;
        }
        cubedata.data[face][mipmapLevel] = PixelData::New(img.release(), byteSize, header.pixelWidth, header.pixelHeight, daliformat, PixelData::DELETE_ARRAY);
      }
    }

    header.pixelHeight /= 2u;
    header.pixelWidth /= 2u;
  }

  return true;
}

}
}
