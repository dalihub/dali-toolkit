 /*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include "image-atlas-manager.h"

// EXTERNAL HEADER
#include <dali/devel-api/images/texture-set-image.h>
#include <dali/public-api/images/resource-image.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
const uint32_t DEFAULT_ATLAS_SIZE( 1024u ); // this size can fit 8 by 8 images of average size 128*128
const uint32_t MAX_ITEM_SIZE( 512u  );
const uint32_t MAX_ITEM_AREA( MAX_ITEM_SIZE*MAX_ITEM_SIZE  );
}

ImageAtlasManager::ImageAtlasManager()
: mBrokenImageUrl( "" )
{
}

ImageAtlasManager::~ImageAtlasManager()
{
}

TextureSet ImageAtlasManager::Add( Vector4& textureRect,
                                 const std::string& url,
                                 ImageDimensions size,
                                 FittingMode::Type fittingMode,
                                 bool orientationCorrection )
{
  ImageDimensions dimensions = size;
  ImageDimensions zero;
  if( size == zero )
  {
    dimensions = ResourceImage::GetImageSize( url );
  }

  // big image, atlasing is not applied
  if( static_cast<uint32_t>(dimensions.GetWidth()) * static_cast<uint32_t>(dimensions.GetHeight()) > MAX_ITEM_AREA
      || dimensions.GetWidth()>DEFAULT_ATLAS_SIZE
      || dimensions.GetHeight()>DEFAULT_ATLAS_SIZE)
  {
    return TextureSet();
  }

  unsigned int i = 0;
  for( AtlasContainer::iterator iter = mAtlasList.begin(); iter != mAtlasList.end(); ++iter)
  {
    if( (*iter).Upload( textureRect, url, size, fittingMode, orientationCorrection ) )
    {
      return mTextureSetList[i];
    }
    i++;
  }

  CreateNewAtlas();
  mAtlasList.back().Upload( textureRect, url, size, fittingMode, orientationCorrection );
  return mTextureSetList.back();
}

TextureSet ImageAtlasManager::Add( Vector4& textureRect,
                                 PixelData pixelData )
{

  // big buffer, atlasing is not applied
  if( static_cast<uint32_t>(pixelData.GetWidth()) * static_cast<uint32_t>(pixelData.GetHeight()) > MAX_ITEM_AREA
      || pixelData.GetWidth()>DEFAULT_ATLAS_SIZE
      || pixelData.GetHeight()>DEFAULT_ATLAS_SIZE )
  {
    return TextureSet();
  }

  unsigned int i = 0;
  for( AtlasContainer::iterator iter = mAtlasList.begin(); iter != mAtlasList.end(); ++iter)
  {
    if( (*iter).Upload( textureRect, pixelData ) )
    {
      return mTextureSetList[i];
    }
    i++;
  }

  CreateNewAtlas();
  mAtlasList.back().Upload( textureRect, pixelData );
  return mTextureSetList.back();

}

void ImageAtlasManager::Remove( TextureSet textureSet, const Vector4& textureRect )
{
  unsigned int i = 0;
  for( TextureSetContainer::iterator iter = mTextureSetList.begin(); iter != mTextureSetList.end(); ++iter)
  {
    if( (*iter) == textureSet )
    {
      mAtlasList[i].Remove(textureRect);
      return;
    }
    i++;
  }
}

void ImageAtlasManager::SetBrokenImage( const std::string& brokenImageUrl )
{
  if( !brokenImageUrl.empty() )
  {
    mBrokenImageUrl = brokenImageUrl;
  }
}

void ImageAtlasManager::CreateNewAtlas()
{
  Toolkit::ImageAtlas newAtlas = Toolkit::ImageAtlas::New( DEFAULT_ATLAS_SIZE, DEFAULT_ATLAS_SIZE  );
  if( !mBrokenImageUrl.empty() )
  {
    newAtlas.SetBrokenImage( mBrokenImageUrl );
  }
  mAtlasList.push_back( newAtlas );
  TextureSet textureSet = TextureSet::New();
  TextureSetImage( textureSet, 0u, newAtlas.GetAtlas() );
  mTextureSetList.push_back( textureSet );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
