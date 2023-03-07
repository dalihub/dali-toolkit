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

// FILE HEADER
#include <dali-scene3d/public-api/loader/environment-map-loader.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <string.h>
#include <filesystem>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/ktx-loader.h>

namespace Dali
{
namespace
{
const std::string_view KTX_EXTENSION = ".ktx";

static constexpr uint32_t NUMBER_OF_ENVIRONMENT_MAP_TYPE = 5;
static constexpr uint32_t NUMBER_OF_CUBE_MAP_TYPE        = 4;

/**
 * @brief cube map face index
 * Cube map layer order is as fallows:
 * POSITIVE_X, NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y, POSITIVE_Z, NEGATIVE_Z. @see CubeMapLayer
 * The indices are for 4 kind of environment cube map. Cross_horizontal, Array_horizontal, Cross_vertical, and Array_vertical.
 */
static constexpr uint32_t CUBEMAP_INDEX_X[NUMBER_OF_CUBE_MAP_TYPE][6] = {{2, 0, 1, 1, 1, 3}, {0, 1, 2, 3, 4, 5}, {1, 1, 1, 1, 0, 2}, {0, 0, 0, 0, 0, 0}};
static constexpr uint32_t CUBEMAP_INDEX_Y[NUMBER_OF_CUBE_MAP_TYPE][6] = {{1, 1, 0, 2, 1, 1}, {0, 0, 0, 0, 0, 0}, {1, 3, 0, 2, 1, 1}, {0, 1, 2, 3, 4, 5}};

static constexpr Vector2 NUMBER_OF_CUBE_FACE[NUMBER_OF_CUBE_MAP_TYPE] =
  {
    Vector2(4, 3),
    Vector2(6, 1),
    Vector2(3, 4),
    Vector2(1, 6)};

static constexpr float expectedAspectRatios[NUMBER_OF_ENVIRONMENT_MAP_TYPE] =
  {
    4.0f / 3.0f,
    6.0f / 1.0f,
    3.0f / 4.0f,
    1.0f / 6.0f,
    2.0f / 1.0f};

enum CubeType
{
  CROSS_HORIZONTAL = 0, // Cross horizontal style cube map
  ARRAY_HORIZONTAL,     // array horizontal style cube map
  CROSS_VERTICAL,       // Cross vertical style cube map
  ARRAY_VERTICAL,       // array vertical style cube map
  NONE
};

uint8_t* GetCroppedBuffer(uint8_t* sourceBuffer, uint32_t bytesPerPixel, uint32_t width, uint32_t height, uint32_t xOffset, uint32_t yOffset, uint32_t xFaceSize, uint32_t yFaceSize)
{
  uint32_t byteSize   = bytesPerPixel * xFaceSize * yFaceSize;
  uint8_t* destBuffer = reinterpret_cast<uint8_t*>(malloc(byteSize + 4u));

  int32_t srcStride  = width * bytesPerPixel;
  int32_t destStride = xFaceSize * bytesPerPixel;
  int32_t srcOffset  = xOffset * bytesPerPixel + yOffset * srcStride;
  int32_t destOffset = 0;
  for(uint16_t row = yOffset; row < yOffset + yFaceSize; ++row)
  {
    memcpy(destBuffer + destOffset, sourceBuffer + srcOffset, destStride);
    srcOffset += srcStride;
    destOffset += destStride;
  }

  return destBuffer;
}

PixelData GetCubeFace(Devel::PixelBuffer pixelBuffer, uint32_t faceIndex, CubeType cubeType, float faceWidth, float faceHeight)
{
  PixelData pixelData;
  if(cubeType != NONE)
  {
    uint8_t* imageBuffer   = pixelBuffer.GetBuffer();
    uint32_t bytesPerPixel = Pixel::GetBytesPerPixel(pixelBuffer.GetPixelFormat());
    uint32_t imageWidth    = pixelBuffer.GetWidth();
    uint32_t imageHeight   = pixelBuffer.GetHeight();

    uint32_t xOffset = CUBEMAP_INDEX_X[cubeType][faceIndex] * static_cast<uint32_t>(faceWidth);
    uint32_t yOffset = CUBEMAP_INDEX_Y[cubeType][faceIndex] * static_cast<uint32_t>(faceHeight);

    uint32_t finalFaceWidth  = (xOffset + static_cast<uint32_t>(faceWidth) < imageWidth) ? static_cast<uint32_t>(faceWidth) : imageWidth - xOffset;
    uint32_t finalFaceHeight = (yOffset + static_cast<uint32_t>(faceHeight) < imageHeight) ? static_cast<uint32_t>(faceHeight) : imageHeight - yOffset;
    uint8_t* tempImageBuffer = GetCroppedBuffer(imageBuffer, bytesPerPixel, imageWidth, imageHeight, xOffset, yOffset, finalFaceWidth, finalFaceHeight);
    pixelData                = PixelData::New(tempImageBuffer, finalFaceWidth * finalFaceHeight * bytesPerPixel, finalFaceWidth, finalFaceHeight, pixelBuffer.GetPixelFormat(), PixelData::FREE);
  }
  return pixelData;
}

/**
 * @brief Loads environment map data texture from an image url.
 *
 * @param[in] environmentMapUrl The environment map file url.
 * @param[out] environmentMapData The data structure with all pixel data objects.
 * @return bool True if the loading is succeded.
 */
bool LoadEnvironmentMapData(const std::string& environmentMapUrl, Scene3D::Loader::EnvironmentMapData& environmentMapData)
{
  // Diffuse Environment Map
  if(environmentMapUrl.empty())
  {
    return false;
  }

  Devel::PixelBuffer pixelBuffer = LoadImageFromFile(environmentMapUrl);
  if(pixelBuffer)
  {
    CubeType cubeType    = NONE;
    uint32_t imageWidth  = pixelBuffer.GetWidth();
    uint32_t imageHeight = pixelBuffer.GetHeight();
    /**
     * If the environment map type is not EQUIRECTANGULAR,
     * The type should be defined internally.
     * DALi checkes aspect ratio of input image and find the closest type.
     * If the environment map type is CUBEMAP, DALi determines which of the following styles is closest:
     * cross horizontal, cross vertical, array horizontal, and array vertical.
     * When the environment map type is AUTO, it finds the closest type, including the Equirectangular type.
     */
    if(environmentMapData.GetEnvironmentMapType() != Scene3D::EnvironmentMapType::EQUIRECTANGULAR)
    {
      float aspectRatio = (float)imageWidth / (float)imageHeight;

      float    minDistance = FLT_MAX;
      uint32_t typeCount   = (environmentMapData.GetEnvironmentMapType() == Scene3D::EnvironmentMapType::CUBEMAP) ? NUMBER_OF_CUBE_MAP_TYPE : NUMBER_OF_ENVIRONMENT_MAP_TYPE;
      for(uint32_t i = 0; i < typeCount; ++i)
      {
        float distance = fabs(aspectRatio - expectedAspectRatios[i]);
        if(distance < minDistance)
        {
          minDistance = distance;
          cubeType    = static_cast<CubeType>(i);
        }
      }
    }

    if(cubeType != NONE)
    {
      float faceWidth = imageWidth, faceHeight = imageHeight;
      faceWidth /= NUMBER_OF_CUBE_FACE[static_cast<uint32_t>(cubeType)].x;
      faceHeight /= NUMBER_OF_CUBE_FACE[static_cast<uint32_t>(cubeType)].y;
      environmentMapData.mPixelData.resize(6);
      for(uint32_t i = 0; i < 6; ++i)
      {
        environmentMapData.mPixelData[i].resize(1);
      }
      for(uint32_t i = 0; i < 6; ++i)
      {
        environmentMapData.mPixelData[i][0] = GetCubeFace(pixelBuffer, i, cubeType, faceWidth, faceHeight);
      }
      environmentMapData.SetEnvironmentMapType(Scene3D::EnvironmentMapType::CUBEMAP);
    }
    else
    {
      environmentMapData.mPixelData.resize(1);
      environmentMapData.mPixelData[0].resize(1);
      environmentMapData.mPixelData[0][0] = Devel::PixelBuffer::Convert(pixelBuffer);
      environmentMapData.SetEnvironmentMapType(Scene3D::EnvironmentMapType::EQUIRECTANGULAR);
    }
    return true;
  }
  return false;
}
} // namespace

namespace Scene3D
{
namespace Loader
{
bool LoadEnvironmentMap(const std::string& environmentMapUrl, EnvironmentMapData& environmentMapData)
{
  std::filesystem::path modelPath(environmentMapUrl);
  std::string           extension = modelPath.extension();
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

  return (extension == KTX_EXTENSION) ? Dali::Scene3D::Loader::LoadKtxData(environmentMapUrl, environmentMapData) : LoadEnvironmentMapData(environmentMapUrl, environmentMapData);
}

} // namespace Loader
} // namespace Scene3D
} // namespace Dali
