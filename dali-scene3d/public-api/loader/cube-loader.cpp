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
#include <dali-scene3d/public-api/loader/cube-loader.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/integration-api/debug.h>
#include <string.h>

namespace Dali
{
namespace
{
/**
 * @brief cube map face index
 * Cube map layer order is as fallows:
 * POSITIVE_X, NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y, POSITIVE_Z, NEGATIVE_Z. @see CubeMapLayer
 * The indices are for 4 kind of environment cube map. Cross_horizontal, Array_horizontal, Cross_vertical, and Array_vertical.
 */
const uint32_t CUBEMAP_INDEX_X[4][6] = {{2, 0, 1, 1, 1, 3}, {0, 1, 2, 3, 4, 5}, {1, 1, 1, 1, 0, 2}, {0, 0, 0, 0, 0, 0}};
const uint32_t CUBEMAP_INDEX_Y[4][6] = {{1, 1, 0, 2, 1, 1}, {0, 0, 0, 0, 0, 0}, {1, 3, 0, 2, 1, 1}, {0, 1, 2, 3, 4, 5}};

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

PixelData GetCubeFace(Devel::PixelBuffer pixelBuffer, uint32_t faceIndex, CubeType cubeType, uint32_t faceSize)
{
  PixelData pixelData;
  if(cubeType != NONE)
  {
    uint8_t* imageBuffer   = pixelBuffer.GetBuffer();
    uint32_t bytesPerPixel = Pixel::GetBytesPerPixel(pixelBuffer.GetPixelFormat());
    uint32_t imageWidth    = pixelBuffer.GetWidth();
    uint32_t imageHeight   = pixelBuffer.GetHeight();

    uint32_t xOffset = CUBEMAP_INDEX_X[cubeType][faceIndex] * faceSize;
    uint32_t yOffset = CUBEMAP_INDEX_Y[cubeType][faceIndex] * faceSize;

    uint8_t* tempImageBuffer = GetCroppedBuffer(imageBuffer, bytesPerPixel, imageWidth, imageHeight, xOffset, yOffset, faceSize, faceSize);
    pixelData                = PixelData::New(tempImageBuffer, faceSize * faceSize * bytesPerPixel, faceSize, faceSize, pixelBuffer.GetPixelFormat(), PixelData::FREE);
  }
  return pixelData;
}
} // namespace

namespace Scene3D
{
namespace Loader
{
bool LoadCubeData(const std::string& path, CubeData& cubedata)
{
  // Diffuse Cube Map
  if(path.empty())
  {
    return false;
  }

  Devel::PixelBuffer pixelBuffer = LoadImageFromFile(path);
  if(pixelBuffer)
  {
    uint32_t imageWidth  = pixelBuffer.GetWidth();
    uint32_t imageHeight = pixelBuffer.GetHeight();

    uint32_t faceSize = 0;
    CubeType cubeType = NONE;
    if(imageWidth / 4 == imageHeight / 3)
    {
      cubeType = CROSS_HORIZONTAL;
      faceSize = imageWidth / 4;
    }
    else if(imageWidth / 6 == imageHeight)
    {
      cubeType = ARRAY_HORIZONTAL;
      faceSize = imageHeight;
    }
    else if(imageWidth / 3 == imageHeight / 4)
    {
      cubeType = CROSS_VERTICAL;
      faceSize = imageHeight / 4;
    }
    else if(imageWidth == imageHeight / 6)
    {
      cubeType = ARRAY_VERTICAL;
      faceSize = imageWidth;
    }
    else
    {
      DALI_LOG_ERROR("The image is not a cube map\n");
      return false;
    }

    cubedata.data.resize(6);
    for(uint32_t i = 0; i < 6; ++i)
    {
      cubedata.data[i].resize(1);
    }
    for(uint32_t i = 0; i < 6; ++i)
    {
      cubedata.data[i][0] = GetCubeFace(pixelBuffer, i, cubeType, faceSize);
    }

    return true;
  }
  return false;
}

} // namespace Loader
} // namespace Scene3D
} // namespace Dali
