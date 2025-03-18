/*
* Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/utility/npatch-utilities.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Toolkit
{
namespace NPatchUtility
{
namespace
{
Uint16Pair ParseRange(uint32_t& index, uint32_t width, uint8_t*& pixel, uint32_t pixelStride, int32_t testByte, int32_t testBits, int32_t testValue)
{
  unsigned int start = 0xFFFF;
  for(; index < width; ++index, pixel += pixelStride)
  {
    if((pixel[testByte] & testBits) == testValue)
    {
      start = index;
      ++index;
      pixel += pixelStride;
      break;
    }
  }

  unsigned int end = width;
  for(; index < width; ++index, pixel += pixelStride)
  {
    if((pixel[testByte] & testBits) != testValue)
    {
      end = index;
      ++index;
      pixel += pixelStride;
      break;
    }
  }

  return Uint16Pair(start, end);
}

} // unnamed namespace

void GetRedOffsetAndMask(Dali::Pixel::Format pixelFormat, int32_t& byteOffset, int32_t& bitMask)
{
  switch(pixelFormat)
  {
    case Dali::Pixel::A8:
    case Dali::Pixel::L8:
    case Dali::Pixel::LA88:
    {
      byteOffset = 0;
      bitMask    = 0;
      break;
    }
    case Dali::Pixel::RGB888:
    case Dali::Pixel::RGB8888:
    case Dali::Pixel::RGBA8888:
    {
      byteOffset = 0;
      bitMask    = 0xFF;
      break;
    }
    case Dali::Pixel::BGR8888:
    case Dali::Pixel::BGRA8888:
    {
      byteOffset = 2;
      bitMask    = 0xff;
      break;
    }
    case Dali::Pixel::RGB565:
    {
      byteOffset = 0;
      bitMask    = 0xf8;
      break;
    }
    case Dali::Pixel::BGR565:
    {
      byteOffset = 1;
      bitMask    = 0x1f;
      break;
    }
    case Dali::Pixel::RGBA4444:
    {
      byteOffset = 0;
      bitMask    = 0xf0;
      break;
    }
    case Dali::Pixel::BGRA4444:
    {
      byteOffset = 1;
      bitMask    = 0xf0;
      break;
    }
    case Dali::Pixel::RGBA5551:
    {
      byteOffset = 0;
      bitMask    = 0xf8;
      break;
    }
    case Dali::Pixel::BGRA5551:
    {
      byteOffset = 1;
      bitMask    = 0x1e;
      break;
    }
    case Dali::Pixel::INVALID:
    case Dali::Pixel::COMPRESSED_R11_EAC:
    case Dali::Pixel::COMPRESSED_SIGNED_R11_EAC:
    case Dali::Pixel::COMPRESSED_RG11_EAC:
    case Dali::Pixel::COMPRESSED_SIGNED_RG11_EAC:
    case Dali::Pixel::COMPRESSED_RGB8_ETC2:
    case Dali::Pixel::COMPRESSED_SRGB8_ETC2:
    case Dali::Pixel::COMPRESSED_RGB8_ETC1:
    case Dali::Pixel::COMPRESSED_RGB_PVRTC_4BPPV1:
    case Dali::Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case Dali::Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case Dali::Pixel::COMPRESSED_RGBA8_ETC2_EAC:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_5x4_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_5x5_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_6x5_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_6x6_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_8x5_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_8x6_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_8x8_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_10x5_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_10x6_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_10x8_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_10x10_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_12x10_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_12x12_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
    {
      DALI_LOG_ERROR("Pixel formats for compressed images are not compatible with simple masking-out of per-pixel alpha.\n");
      byteOffset = 0;
      bitMask    = 0;
      break;
    }
    case Dali::Pixel::RGB16F:
    case Dali::Pixel::RGB32F:
    case Dali::Pixel::DEPTH_UNSIGNED_INT:
    case Dali::Pixel::DEPTH_FLOAT:
    case Dali::Pixel::DEPTH_STENCIL:
    case Dali::Pixel::R11G11B10F:
    case Dali::Pixel::CHROMINANCE_U:
    case Dali::Pixel::CHROMINANCE_V:
    {
      DALI_LOG_ERROR("Pixel format not compatible.\n");
      byteOffset = 0;
      bitMask    = 0;
      break;
    }
  }
}

bool ParseBorders(Devel::PixelBuffer& pixelBuffer, StretchRanges& stretchPixelsX, StretchRanges& stretchPixelsY)
{
  stretchPixelsX.Clear();
  stretchPixelsY.Clear();

  Pixel::Format pixelFormat = pixelBuffer.GetPixelFormat();

  int32_t alphaByte = 0;
  int32_t alphaBits = 0;
  Pixel::GetAlphaOffsetAndMask(pixelFormat, alphaByte, alphaBits);

  int32_t testByte  = alphaByte;
  int32_t testBits  = alphaBits;
  int32_t testValue = alphaBits; // Opaque == stretch
  if(!alphaBits)
  {
    GetRedOffsetAndMask(pixelFormat, testByte, testBits);
    testValue = 0; // Black == stretch
  }

  uint32_t bytesPerPixel = Pixel::GetBytesPerPixel(pixelFormat);
  uint32_t width         = pixelBuffer.GetWidth();
  uint32_t height        = pixelBuffer.GetHeight();
  uint32_t srcStride     = pixelBuffer.GetStrideBytes() ? pixelBuffer.GetStrideBytes() : width * bytesPerPixel;
  uint8_t* srcPixels     = pixelBuffer.GetBuffer();

  if(width <= 2 || width >= 0xFFFF || height <= 2 || height >= 0xFFFF)
  {
    DALI_LOG_ERROR("PixelBuffer size not allowed! [%u x %u] border parsing failed\n", width, height);
    return false;
  }

  // TOP
  uint8_t* top   = srcPixels + bytesPerPixel;
  uint32_t index = 0;

  for(; index < width - 2;)
  {
    Uint16Pair range = ParseRange(index, width - 2, top, bytesPerPixel, testByte, testBits, testValue);
    if(range.GetX() != 0xFFFF)
    {
      stretchPixelsX.PushBack(range);
    }
  }

  // LEFT
  uint8_t* left = srcPixels + srcStride;
  index         = 0;
  for(; index < height - 2;)
  {
    Uint16Pair range = ParseRange(index, height - 2, left, srcStride, testByte, testBits, testValue);
    if(range.GetX() != 0xFFFF)
    {
      stretchPixelsY.PushBack(range);
    }
  }

  // If there are no stretch pixels then make the entire image stretchable
  if(stretchPixelsX.Size() == 0)
  {
    stretchPixelsX.PushBack(Uint16Pair(0, width - 2));
  }
  if(stretchPixelsY.Size() == 0)
  {
    stretchPixelsY.PushBack(Uint16Pair(0, height - 2));
  }

  return true;
}

bool IsNinePatchUrl(const std::string& url)
{
  bool match = false;

  std::string::const_reverse_iterator iter = url.rbegin();
  enum
  {
    SUFFIX,
    HASH,
    HASH_DOT,
    DONE
  } state = SUFFIX;
  while(iter < url.rend())
  {
    switch(state)
    {
      case SUFFIX:
      {
        if(*iter == '.')
        {
          state = HASH;
        }
        else if(!isalnum(*iter))
        {
          state = DONE;
        }
      }
      break;
      case HASH:
      {
        if(*iter == '#' || *iter == '9')
        {
          state = HASH_DOT;
        }
        else
        {
          state = DONE;
        }
      }
      break;
      case HASH_DOT:
      {
        if(*iter == '.')
        {
          match = true;
        }
        state = DONE; // Stop testing characters
      }
      break;
      case DONE:
      {
      }
      break;
    }

    // Satisfy prevent
    if(state == DONE)
    {
      break;
    }

    ++iter;
  }
  return match;
}

Dali::Uint16Pair GetValidStrechPointFromBorder(uint32_t maxRangeSize, uint32_t rangeFromZero, uint32_t rangeFromMax)
{
  maxRangeSize  = std::min(maxRangeSize, 0xFFFFu);
  rangeFromZero = std::min(rangeFromZero, 0xFFFFu);
  rangeFromMax  = std::min(rangeFromMax, 0xFFFFu);
  if(DALI_UNLIKELY(rangeFromZero + rangeFromMax > maxRangeSize))
  {
    // Keep ratio and make ensure that sume of value didn't overflow the max range.
    // Note that we can assume that rangeSum is bigger than zero!
    uint32_t rangeSum = rangeFromZero + rangeFromMax;

    rangeFromZero = (rangeFromZero * maxRangeSize) / rangeSum;
    rangeFromMax  = (rangeFromMax * maxRangeSize) / rangeSum;

    // Ensure to make rangeFromZero + rangeFromMax is equal to maxRangeSize.
    uint32_t remainedRange = maxRangeSize - (rangeFromZero + rangeFromMax);
    rangeFromZero += (remainedRange / 2);
    rangeFromMax += remainedRange - (remainedRange / 2);
  }

  DALI_ASSERT_DEBUG(rangeFromZero + rangeFromMax <= maxRangeSize && "Rearrange strech point failed!");

  return Uint16Pair(rangeFromZero, maxRangeSize - rangeFromMax);
}

} // namespace NPatchUtility

} // namespace Toolkit

} // namespace Dali
