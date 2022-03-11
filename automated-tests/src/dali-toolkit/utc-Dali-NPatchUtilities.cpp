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
 */

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/utility/npatch-utilities.h>
#include <stdlib.h>
#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
void InitialiseRegionsToZeroAlpha(Dali::Devel::PixelBuffer pixelBuffer, uint32_t width, uint32_t height, Pixel::Format pixelFormat)
{
  unsigned char* buffer        = pixelBuffer.GetBuffer();
  uint32_t       bytesPerPixel = GetBytesPerPixel(pixelFormat);

  for(uint32_t row = 0; row < width; ++row)
  {
    uint32_t pixelOffset    = row * bytesPerPixel;
    buffer[pixelOffset + 3] = 0x00;
    pixelOffset += (height - 1) * width * bytesPerPixel;
    buffer[pixelOffset + 3] = 0x00;
  }

  for(unsigned int column = 0; column < height; ++column)
  {
    uint32_t pixelOffset    = column * width * bytesPerPixel;
    buffer[pixelOffset + 3] = 0x00;
    pixelOffset += (width - 1) * bytesPerPixel;
    buffer[pixelOffset + 3] = 0x00;
  }
}

void AddStretchRegionsToImage(Dali::Devel::PixelBuffer pixelBuffer, uint32_t width, uint32_t height, const Vector4& requiredStretchBorder, Pixel::Format pixelFormat)
{
  unsigned char* buffer        = pixelBuffer.GetBuffer();
  uint32_t       bytesPerPixel = GetBytesPerPixel(pixelFormat);

  for(uint32_t column = requiredStretchBorder.x; column < width - requiredStretchBorder.z; ++column)
  {
    uint32_t pixelOffset    = column * bytesPerPixel;
    buffer[pixelOffset]     = 0x00;
    buffer[pixelOffset + 1] = 0x00;
    buffer[pixelOffset + 2] = 0x00;
    buffer[pixelOffset + 3] = 0xFF;
  }

  for(uint32_t row = requiredStretchBorder.y; row < height - requiredStretchBorder.w; ++row)
  {
    unsigned int pixelOffset = row * width * bytesPerPixel;
    buffer[pixelOffset]      = 0x00;
    buffer[pixelOffset + 1]  = 0x00;
    buffer[pixelOffset + 2]  = 0x00;
    buffer[pixelOffset + 3]  = 0xFF;
  }
}

Dali::Devel::PixelBuffer CustomizeNPatch(uint32_t width, uint32_t height, const Vector4& requiredStretchBorder)
{
  Pixel::Format            pixelFormat = Pixel::RGBA8888;
  Dali::Devel::PixelBuffer pixelBuffer = Dali::Devel::PixelBuffer::New(width, height, pixelFormat);

  unsigned char* buffer = pixelBuffer.GetBuffer();
  memset(buffer, 0, width * height * Dali::Pixel::GetBytesPerPixel(pixelFormat));

  InitialiseRegionsToZeroAlpha(pixelBuffer, width, height, pixelFormat);

  AddStretchRegionsToImage(pixelBuffer, width, height, requiredStretchBorder, pixelFormat);

  return pixelBuffer;
}

} //namespace

void dali_npatch_utilities_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_npatch_utilities_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliNPatchUtilityGetRedOffsetAndMask(void)
{
  TestApplication application;

  tet_infoline("UtcDaliNPatchUtilityGetRedOffsetAndMask");

  int32_t byteOffset = 0;
  int32_t bitMask    = 0;

  NPatchUtility::GetRedOffsetAndMask(Pixel::A8, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::L8, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::LA88, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);

  NPatchUtility::GetRedOffsetAndMask(Pixel::RGB888, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0xff);
  NPatchUtility::GetRedOffsetAndMask(Pixel::RGB8888, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0xff);
  NPatchUtility::GetRedOffsetAndMask(Pixel::RGBA8888, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0xff);

  NPatchUtility::GetRedOffsetAndMask(Pixel::BGR8888, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 2 && bitMask == 0xff);
  NPatchUtility::GetRedOffsetAndMask(Pixel::BGRA8888, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 2 && bitMask == 0xff);

  NPatchUtility::GetRedOffsetAndMask(Pixel::RGB565, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0xf8);

  NPatchUtility::GetRedOffsetAndMask(Pixel::BGR565, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 1 && bitMask == 0x1f);

  NPatchUtility::GetRedOffsetAndMask(Pixel::RGBA4444, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0xf0);

  NPatchUtility::GetRedOffsetAndMask(Pixel::BGRA4444, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 1 && bitMask == 0xf0);

  NPatchUtility::GetRedOffsetAndMask(Pixel::RGBA5551, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0xf8);

  NPatchUtility::GetRedOffsetAndMask(Pixel::BGRA5551, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 1 && bitMask == 0x1e);

  // Compressed formats are not supported
  NPatchUtility::GetRedOffsetAndMask(Pixel::INVALID, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_R11_EAC, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SIGNED_R11_EAC, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RG11_EAC, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SIGNED_RG11_EAC, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGB8_ETC2, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SRGB8_ETC2, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGB8_ETC1, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGB_PVRTC_4BPPV1, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGBA8_ETC2_EAC, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGBA_ASTC_5x4_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGBA_ASTC_5x5_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGBA_ASTC_6x5_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGBA_ASTC_6x6_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGBA_ASTC_8x5_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGBA_ASTC_8x6_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGBA_ASTC_8x8_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGBA_ASTC_10x5_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGBA_ASTC_10x6_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGBA_ASTC_10x8_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGBA_ASTC_10x10_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGBA_ASTC_12x10_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_RGBA_ASTC_12x12_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);

  // Not supported
  NPatchUtility::GetRedOffsetAndMask(Pixel::RGB16F, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);
  NPatchUtility::GetRedOffsetAndMask(Pixel::RGB32F, byteOffset, bitMask);
  DALI_TEST_CHECK(byteOffset == 0 && bitMask == 0);

  END_TEST;
}

int UtcDaliNPatchUtilityParseBorders(void)
{
  TestApplication application;
  tet_infoline("UtcDaliNPatchUtilityParseBorders");

  /* Stretch region left(2) top(2) right (2) bottom (2)
    *    ss
    *  OOOOOO
    *  OOOOOOc
    * sOOooOOc
    * sOOooOOc
    *  OOOOOOc
    *  OOOOOO
    *   cccc
    */

  const unsigned int imageHeight = 18;
  const unsigned int imageWidth  = 28;
  const Vector4      requiredStretchBorder(3, 4, 5, 6);

  Dali::Devel::PixelBuffer pixelBuffer = CustomizeNPatch(imageHeight, imageWidth, requiredStretchBorder);
  DALI_TEST_CHECK(pixelBuffer);

  if(pixelBuffer)
  {
    NPatchUtility::StretchRanges stretchPixelsX;
    NPatchUtility::StretchRanges stretchPixelsY;

    NPatchUtility::ParseBorders(pixelBuffer, stretchPixelsX, stretchPixelsY);

    DALI_TEST_CHECK(stretchPixelsX.Size() == 1);
    DALI_TEST_CHECK(stretchPixelsY.Size() == 1);

    Vector4 stretchBorders;
    //The NPatch image stretch pixels are in the cropped image space, inset by 1 to get it to uncropped image space
    stretchBorders.x = stretchPixelsX[0].GetX() + 1;
    stretchBorders.y = stretchPixelsY[0].GetX() + 1;
    stretchBorders.z = imageHeight - stretchPixelsX[0].GetY() - 1;
    stretchBorders.w = imageWidth - stretchPixelsY[0].GetY() - 1;

    DALI_TEST_EQUALS(stretchBorders, requiredStretchBorder, 0.001, TEST_LOCATION);
  }
  else
  {
    test_return_value = TET_FAIL;
  }

  END_TEST;
}

int UtcDaliNPatchUtilityIsNinePatchUrl(void)
{
  tet_infoline("UtcDaliNPatchUtilityIsNinePatchUrl");

  DALI_TEST_CHECK(NPatchUtility::IsNinePatchUrl("test.9.jpg"));
  DALI_TEST_CHECK(NPatchUtility::IsNinePatchUrl("test.#.jpg"));
  DALI_TEST_CHECK(!NPatchUtility::IsNinePatchUrl("test.9"));
  DALI_TEST_CHECK(!NPatchUtility::IsNinePatchUrl("test.#"));
  DALI_TEST_CHECK(!NPatchUtility::IsNinePatchUrl("test"));

  END_TEST;
}
