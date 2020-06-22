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

// EXTERNAL INCLUDES
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

void SetLoadedNPatchData( NPatchLoader::Data* data, Devel::PixelBuffer& pixelBuffer )
{
  if( data->border == Rect< int >( 0, 0, 0, 0 ) )
  {
    NPatchUtility::ParseBorders( pixelBuffer, data->stretchPixelsX, data->stretchPixelsY );

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

            NPatchUtility::StretchRanges stretchRangesX;
            stretchRangesX.PushBack( Uint16Pair( border.left, ( (data->croppedWidth >= static_cast< unsigned int >( border.right )) ? data->croppedWidth - border.right : 0 ) ) );

            NPatchUtility::StretchRanges stretchRangesY;
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
