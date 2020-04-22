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

// CLASS HEADER
#include <dali-toolkit/internal/visuals/npatch-loader.h>

// EXTERNAL HEADER
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/common/hash.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace NPatchBuffer
{

void GetRedOffsetAndMask( Dali::Pixel::Format pixelFormat, int& byteOffset, int& bitMask )
{
  switch( pixelFormat )
  {
    case Dali::Pixel::A8:
    case Dali::Pixel::L8:
    case Dali::Pixel::LA88:
    {
      byteOffset = 0;
      bitMask = 0;
      break;
    }
    case Dali::Pixel::RGB888:
    case Dali::Pixel::RGB8888:
    case Dali::Pixel::RGBA8888:
    {
      byteOffset = 0;
      bitMask = 0xFF;
      break;
    }
    case Dali::Pixel::BGR8888:
    case Dali::Pixel::BGRA8888:
    {
      byteOffset = 2;
      bitMask = 0xff;
      break;
    }
    case Dali::Pixel::RGB565:
    {
      byteOffset = 0;
      bitMask = 0xf8;
      break;
    }
    case Dali::Pixel::BGR565:
    {
      byteOffset = 1;
      bitMask = 0x1f;
      break;
    }
    case Dali::Pixel::RGBA4444:
    {
      byteOffset = 0;
      bitMask = 0xf0;
      break;
    }
    case Dali::Pixel::BGRA4444:
    {
      byteOffset = 1;
      bitMask = 0xf0;
      break;
    }
    case Dali::Pixel::RGBA5551:
    {
      byteOffset = 0;
      bitMask = 0xf8;
      break;
    }
    case Dali::Pixel::BGRA5551:
    {
      byteOffset = 1;
      bitMask = 0x1e;
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
      byteOffset=0;
      bitMask=0;
      break;
    }
    case Dali::Pixel::RGB16F:
    case Dali::Pixel::RGB32F:
    case Dali::Pixel::DEPTH_UNSIGNED_INT:
    case Dali::Pixel::DEPTH_FLOAT:
    case Dali::Pixel::DEPTH_STENCIL:
    {
      DALI_LOG_ERROR("Pixel format not compatible.\n");
      byteOffset=0;
      bitMask=0;
      break;
    }
  }
}

Uint16Pair ParseRange( unsigned int& index, unsigned int width, unsigned char*& pixel, unsigned int pixelStride, int testByte, int testBits, int testValue )
{
  unsigned int start = 0xFFFF;
  for( ; index < width; ++index, pixel += pixelStride )
  {
    if( ( pixel[ testByte ] & testBits ) == testValue )
    {
        start = index;
        ++index;
        pixel += pixelStride;
        break;
    }
  }

  unsigned int end = width;
  for( ; index < width; ++index, pixel += pixelStride )
  {
    if( ( pixel[ testByte ] & testBits ) != testValue )
    {
        end = index;
        ++index;
        pixel += pixelStride;
        break;
    }
  }

  return Uint16Pair( start, end );
}

void ParseBorders( Devel::PixelBuffer& pixelBuffer, NPatchLoader::Data* data  )
{
  data->stretchPixelsX.Clear();
  data->stretchPixelsY.Clear();

  Pixel::Format pixelFormat = pixelBuffer.GetPixelFormat();

  int alphaByte = 0;
  int alphaBits = 0;
  Pixel::GetAlphaOffsetAndMask( pixelFormat, alphaByte, alphaBits );

  int testByte = alphaByte;
  int testBits = alphaBits;
  int testValue = alphaBits; // Opaque == stretch
  if( !alphaBits )
  {
    GetRedOffsetAndMask( pixelFormat, testByte, testBits );
    testValue = 0;           // Black == stretch
  }

  unsigned int bytesPerPixel = Pixel::GetBytesPerPixel( pixelFormat );
  unsigned int width = pixelBuffer.GetWidth();
  unsigned int height = pixelBuffer.GetHeight();
  unsigned char* srcPixels = pixelBuffer.GetBuffer();
  unsigned int srcStride = width * bytesPerPixel;

  // TOP
  unsigned char* top = srcPixels + bytesPerPixel;
  unsigned int index = 0;

  for( ; index < width - 2; )
  {
    Uint16Pair range = ParseRange( index, width - 2, top, bytesPerPixel, testByte, testBits, testValue );
    if( range.GetX() != 0xFFFF )
    {
      data->stretchPixelsX.PushBack( range );
    }
  }

  // LEFT
  unsigned char* left  = srcPixels + srcStride;
  index = 0;
  for( ; index < height - 2; )
  {
    Uint16Pair range = ParseRange( index, height - 2, left, srcStride, testByte, testBits, testValue );
    if( range.GetX() != 0xFFFF )
    {
      data->stretchPixelsY.PushBack( range );
    }
  }

  // If there are no stretch pixels then make the entire image stretchable
  if( data->stretchPixelsX.Size() == 0 )
  {
    data->stretchPixelsX.PushBack( Uint16Pair( 0, width - 2 ) );
  }
  if( data->stretchPixelsY.Size() == 0 )
  {
    data->stretchPixelsY.PushBack( Uint16Pair( 0, height - 2 ) );
  }
}

void SetLoadedNPatchData( NPatchLoader::Data* data, Devel::PixelBuffer& pixelBuffer )
{
  if( data->border == Rect< int >( 0, 0, 0, 0 ) )
  {
    NPatchBuffer::ParseBorders( pixelBuffer, data );

    // Crop the image
    pixelBuffer.Crop( 1, 1, pixelBuffer.GetWidth() - 2, pixelBuffer.GetHeight() - 2 );
  }
  else
  {
    data->stretchPixelsX.PushBack( Uint16Pair( data->border.left, ( (pixelBuffer.GetWidth() >= static_cast< unsigned int >( data->border.right )) ? pixelBuffer.GetWidth() - data->border.right : 0 ) ) );
    data->stretchPixelsY.PushBack( Uint16Pair( data->border.top, ( (pixelBuffer.GetHeight() >= static_cast< unsigned int >( data->border.bottom )) ? pixelBuffer.GetHeight() - data->border.bottom : 0 ) ) );
  }

  data->croppedWidth = pixelBuffer.GetWidth();
  data->croppedHeight = pixelBuffer.GetHeight();

  PixelData pixels = Devel::PixelBuffer::Convert( pixelBuffer ); // takes ownership of buffer

  Texture texture = Texture::New( TextureType::TEXTURE_2D, pixels.GetPixelFormat(), pixels.GetWidth(), pixels.GetHeight() );
  texture.Upload( pixels );

  data->textureSet = TextureSet::New();
  data->textureSet.SetTexture( 0u, texture );

  data->loadCompleted = true;
}

} // namespace NPatchBuffer

NPatchLoader::NPatchLoader()
{
}

NPatchLoader::~NPatchLoader()
{
}

std::size_t NPatchLoader::Load( TextureManager& textureManager, TextureUploadObserver* textureObserver, const std::string& url, const Rect< int >& border, bool& preMultiplyOnLoad, bool synchronousLoading )
{
  std::size_t hash = CalculateHash( url );
  OwnerContainer< Data* >::SizeType index = UNINITIALIZED_ID;
  const OwnerContainer< Data* >::SizeType count = mCache.Count();
  int cachedIndex = -1;
  Data* data;

  for( ; index < count; ++index )
  {
    if( mCache[ index ]->hash == hash )
    {
      // hash match, check url as well in case of hash collision
      if( mCache[ index ]->url == url )
      {
        // Use cached data
        if( mCache[ index ]->border == border )
        {
          if( mCache[ index ]->loadCompleted )
          {
            return index + 1u; // valid indices are from 1 onwards
          }
          data = mCache[ index ];
          cachedIndex = index + 1u; // valid indices are from 1 onwards
          break;
        }
        else
        {
          if( mCache[ index ]->loadCompleted )
          {
            // Same url but border is different - use the existing texture
            Data* data = new Data();
            data->hash = hash;
            data->url = url;
            data->croppedWidth = mCache[ index ]->croppedWidth;
            data->croppedHeight = mCache[ index ]->croppedHeight;

            data->textureSet = mCache[ index ]->textureSet;

            StretchRanges stretchRangesX;
            stretchRangesX.PushBack( Uint16Pair( border.left, ( (data->croppedWidth >= static_cast< unsigned int >( border.right )) ? data->croppedWidth - border.right : 0 ) ) );

            StretchRanges stretchRangesY;
            stretchRangesY.PushBack( Uint16Pair( border.top, ( (data->croppedHeight >= static_cast< unsigned int >( border.bottom )) ? data->croppedHeight - border.bottom : 0 ) ) );

            data->stretchPixelsX = stretchRangesX;
            data->stretchPixelsY = stretchRangesY;
            data->border = border;

            data->loadCompleted = mCache[ index ]->loadCompleted;

            mCache.PushBack( data );

            return mCache.Count(); // valid ids start from 1u
          }
        }
      }
    }
  }

  if( cachedIndex == -1 )
  {
    data = new Data();
    data->loadCompleted = false;
    data->hash = hash;
    data->url = url;
    data->border = border;

    mCache.PushBack( data );

    cachedIndex = mCache.Count();
  }

  auto preMultiplyOnLoading = preMultiplyOnLoad ? TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD
                                                : TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  Devel::PixelBuffer pixelBuffer = textureManager.LoadPixelBuffer( url, Dali::ImageDimensions(), FittingMode::DEFAULT,
                                                                   SamplingMode::BOX_THEN_LINEAR, synchronousLoading,
                                                                   textureObserver, true, preMultiplyOnLoading );

  if( pixelBuffer )
  {
    NPatchBuffer::SetLoadedNPatchData( data, pixelBuffer );
    preMultiplyOnLoad = ( preMultiplyOnLoading == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD ) ? true : false;
  }

  return cachedIndex;
}

void NPatchLoader::SetNPatchData( std::size_t id, Devel::PixelBuffer& pixelBuffer )
{
  Data* data;
  data = mCache[ id - 1u ];

  if( !data->loadCompleted )
  {
    NPatchBuffer::SetLoadedNPatchData( data, pixelBuffer );
  }
}

bool NPatchLoader::GetNPatchData( std::size_t id, const Data*& data )
{
  if( ( id > UNINITIALIZED_ID )&&( id <= mCache.Count() ) )
  {
    data = mCache[ id - 1u ]; // id's start from 1u
    return true;
  }
  data = NULL;
  return false;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
