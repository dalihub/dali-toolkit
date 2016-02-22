 /*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

ImageAtlasManager::ImageAtlasManager( Shader shader, const std::string& textureUniformName )
: mShader( shader ),
  mTextureUniformName( textureUniformName ),
  mBrokenImageUrl( "" )
{
}

ImageAtlasManager::~ImageAtlasManager()
{
}

Material ImageAtlasManager::Add( Vector4& textureRect,
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
    return Material();
  }

  unsigned int i = 0;
  for( AtlasContainer::iterator iter = mAtlasList.begin(); iter != mAtlasList.end(); ++iter)
  {
    if( (*iter).Upload( textureRect, url, size, fittingMode, orientationCorrection ) )
    {
      return mMaterialList[i];
    }
    i++;
  }

  CreateNewAtlas();
  mAtlasList.back().Upload( textureRect, url, size, fittingMode, orientationCorrection );
  return mMaterialList.back();
}

Material ImageAtlasManager::Add( Vector4& textureRect,
                                 PixelDataPtr pixelData )
{

  // big buffer, atlasing is not applied
  if( static_cast<uint32_t>(pixelData->GetWidth()) * static_cast<uint32_t>(pixelData->GetHeight()) > MAX_ITEM_AREA
      || pixelData->GetWidth()>DEFAULT_ATLAS_SIZE
      || pixelData->GetHeight()>DEFAULT_ATLAS_SIZE )
  {
    return Material();
  }

  unsigned int i = 0;
  for( AtlasContainer::iterator iter = mAtlasList.begin(); iter != mAtlasList.end(); ++iter)
  {
    if( (*iter).Upload( textureRect, pixelData ) )
    {
      return mMaterialList[i];
    }
    i++;
  }

  CreateNewAtlas();
  mAtlasList.back().Upload( textureRect, pixelData );
  return mMaterialList.back();

}

void ImageAtlasManager::Remove( Material material, const Vector4& textureRect )
{
  unsigned int i = 0;
  for( MaterialContainer::iterator iter = mMaterialList.begin(); iter != mMaterialList.end(); ++iter)
  {
    if( (*iter) == material )
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
  Material newMaterial = Material::New( mShader );
  newMaterial.AddTexture( newAtlas.GetAtlas(), mTextureUniformName );
  mMaterialList.push_back( newMaterial );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
