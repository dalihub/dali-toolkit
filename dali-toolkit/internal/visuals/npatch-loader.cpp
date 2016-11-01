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
#include <dali-toolkit/internal/visuals/npatch-loader.h>

// EXTERNAL HEADER
#include <dali/devel-api/common/hash.h>
#include <dali/devel-api/images/texture-set-image.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

NPatchLoader::NPatchLoader()
{
}

NPatchLoader::~NPatchLoader()
{
}

std::size_t NPatchLoader::Load( const std::string& url )
{
  std::size_t hash = CalculateHash( url );
  OwnerContainer< Data* >::SizeType index = UNINITIALIZED_ID;
  const OwnerContainer< Data* >::SizeType count = mCache.Count();
  for( ; index < count; ++index )
  {
    if( mCache[ index ]->hash == hash )
    {
      // hash match, check url as well in case of hash collision
      if( mCache[ index ]->url == url )
      {
        return index+1u; // valid indices are from 1 onwards
      }
    }
  }
  // got to the end so no match, decode N patch and append new item to cache
  NinePatchImage ninePatch = NinePatchImage::New( url );
  if( ninePatch )
  {
    Data* data = new Data();
    data->hash = hash;
    data->url = url;
    BufferImage croppedImage = ninePatch.CreateCroppedBufferImage();
    if( croppedImage )
    {
      data->textureSet = TextureSet::New();
      TextureSetImage( data->textureSet, 0u, croppedImage );
      data->croppedWidth = croppedImage.GetWidth();
      data->croppedHeight = croppedImage.GetHeight();
      data->stretchPixelsX = ninePatch.GetStretchPixelsX();
      data->stretchPixelsY = ninePatch.GetStretchPixelsY();
      mCache.PushBack( data );

      return mCache.Count(); // valid ids start from 1u
    }
  }
  return 0u;
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
