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
#include "image-atlas-impl.h"

// EXTERNAL INCLUDES
#include <string.h>
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/devel-api/adaptor-framework/bitmap-loader.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
typedef unsigned char PixelBuffer;

Texture ImageAtlas::PackToAtlas( const std::vector<PixelData>& pixelData, Dali::Vector<Vector4>& textureRects  )
{
  // Record each block size
  Dali::Vector<Uint16Pair> blockSizes;
  SizeType count = pixelData.size();
  for( SizeType index = 0; index < count; index++ )
  {
    blockSizes.PushBack( ImageDimensions( pixelData[index].GetWidth(), pixelData[index].GetHeight() ) );
  }

  // Ask atlasPacker for packing position of each block
  Dali::Vector<Uint16Pair> packPositions;
  ImageDimensions atlasSize = AtlasPacker::GroupPack( blockSizes, packPositions );

  // Prepare for outout texture rect array
  textureRects.Clear();
  textureRects.Resize( count );

  // create the texture for uploading the multiple pixel data
  Texture atlasTexture = Texture::New( Dali::TextureType::TEXTURE_2D, Pixel::RGBA8888, atlasSize.GetWidth(), atlasSize.GetHeight() );

  float atlasWidth = static_cast<float>( atlasTexture.GetWidth() );
  float atlasHeight = static_cast<float>( atlasTexture.GetHeight() );
  int packPositionX, packPositionY;
  // Upload the pixel data one by one to its packing position, and record the texture rects
  for( SizeType index = 0; index < count; index++ )
  {
    packPositionX = packPositions[index].GetX();
    packPositionY = packPositions[index].GetY();
    atlasTexture.Upload( pixelData[index], 0u, 0u,
                         packPositionX, packPositionY,
                         pixelData[index].GetWidth(), pixelData[index].GetHeight() );

    // Apply the half pixel correction to avoid the color bleeding between neighbour blocks
    textureRects[index].x = ( static_cast<float>( packPositionX ) +0.5f ) / atlasWidth; // left
    textureRects[index].y = ( static_cast<float>( packPositionY ) +0.5f ) / atlasHeight; // right
    textureRects[index].z = ( static_cast<float>( packPositionX + pixelData[index].GetWidth() )-0.5f ) / atlasWidth; // right
    textureRects[index].w = ( static_cast<float>( packPositionY + pixelData[index].GetHeight() )-0.5f ) / atlasHeight;// bottom
  }

  return atlasTexture;
}

ImageAtlas::ImageAtlas( SizeType width, SizeType height, Pixel::Format pixelFormat )
: mAtlas( Texture::New( Dali::TextureType::TEXTURE_2D, pixelFormat, width, height ) ),
  mPacker( width, height ),
  mAsyncLoader( Toolkit::AsyncImageLoader::New() ),
  mBrokenImageUrl(""),
  mBrokenImageSize(),
  mWidth( static_cast<float>(width) ),
  mHeight( static_cast<float>( height ) ),
  mPixelFormat( pixelFormat )
{
  mAsyncLoader.ImageLoadedSignal().Connect( this, &ImageAtlas::UploadToAtlas );
}

ImageAtlas::~ImageAtlas()
{
  const std::size_t count = mLoadingTaskInfoContainer.Count();
  for( std::size_t i=0; i < count; ++i )
  {
    // Call unregister to every observer in the list.
    // Note that, the Atlas can be registered to same observer multiple times, and the Unregister method only remove one item each time.
    // In this way, the atlas is actually detached from a observer either every upload call invoked by this observer is completed or atlas is destroyed.
    if( mLoadingTaskInfoContainer[i]->observer )
    {
      mLoadingTaskInfoContainer[i]->observer->Unregister( *this );
    }
  }

  mLoadingTaskInfoContainer.Clear();
}

IntrusivePtr<ImageAtlas> ImageAtlas::New( SizeType width, SizeType height, Pixel::Format pixelFormat )
{
  IntrusivePtr<ImageAtlas> internal = new ImageAtlas( width, height, pixelFormat );

  return internal;
}

Texture ImageAtlas::GetAtlas()
{
  return mAtlas;
}

float ImageAtlas::GetOccupancyRate() const
{
  return 1.f - static_cast<float>( mPacker.GetAvailableArea() ) / ( mWidth*mHeight );
}

void ImageAtlas::SetBrokenImage( const std::string& brokenImageUrl )
{
  mBrokenImageSize = ResourceImage::GetImageSize( brokenImageUrl );
  if(mBrokenImageSize.GetWidth() > 0 && mBrokenImageSize.GetHeight() > 0 ) // check the url is valid
  {
    mBrokenImageUrl = brokenImageUrl;
  }
}

bool ImageAtlas::Upload( Vector4& textureRect,
                         const std::string& url,
                         ImageDimensions size,
                         FittingMode::Type fittingMode,
                         bool orientationCorrection,
                         AtlasUploadObserver* atlasUploadObserver )
{
  ImageDimensions dimensions = size;
  ImageDimensions zero;
  if( size == zero ) // image size not provided
  {
    dimensions = ResourceImage::GetImageSize( url );
    if( dimensions == zero ) // Fail to read the image & broken image file exists
    {
      if( !mBrokenImageUrl.empty() )
      {
        return Upload( textureRect, mBrokenImageUrl, mBrokenImageSize, FittingMode::DEFAULT, true, atlasUploadObserver );
      }
      else
      {
        textureRect = Vector4::ZERO;
        return true;
      }
    }
  }

  unsigned int packPositionX = 0;
  unsigned int packPositionY = 0;
  if( mPacker.Pack( dimensions.GetWidth(), dimensions.GetHeight(), packPositionX, packPositionY ) )
  {
    unsigned short loadId = mAsyncLoader.Load( url, size, fittingMode, SamplingMode::BOX_THEN_LINEAR, orientationCorrection );
    mLoadingTaskInfoContainer.PushBack( new LoadingTaskInfo( loadId, packPositionX, packPositionY, dimensions.GetWidth(), dimensions.GetHeight(), atlasUploadObserver ) );
    // apply the half pixel correction
    textureRect.x = ( static_cast<float>( packPositionX ) +0.5f ) / mWidth; // left
    textureRect.y = ( static_cast<float>( packPositionY ) +0.5f ) / mHeight; // right
    textureRect.z = ( static_cast<float>( packPositionX + dimensions.GetX() )-0.5f ) / mWidth; // right
    textureRect.w = ( static_cast<float>( packPositionY + dimensions.GetY() )-0.5f ) / mHeight;// bottom

    if( atlasUploadObserver )
    {
      // register to the observer,
      // Not that a matching unregister call should be invoked in UploadToAtlas if the observer is still alive by then.
      atlasUploadObserver->Register( *this );
    }

    return true;
  }

  return false;
}

bool ImageAtlas::Upload( Vector4& textureRect, PixelData pixelData )
{
  unsigned int packPositionX = 0;
  unsigned int packPositionY = 0;
  if( mPacker.Pack( pixelData.GetWidth(), pixelData.GetHeight(), packPositionX, packPositionY ) )
  {
    mAtlas.Upload( pixelData, 0u, 0u, packPositionX, packPositionY, pixelData.GetWidth(), pixelData.GetHeight() );

    // apply the half pixel correction
    textureRect.x = ( static_cast<float>( packPositionX ) +0.5f ) / mWidth; // left
    textureRect.y = ( static_cast<float>( packPositionY ) +0.5f ) / mHeight; // right
    textureRect.z = ( static_cast<float>( packPositionX + pixelData.GetWidth() )-0.5f ) / mWidth; // right
    textureRect.w = ( static_cast<float>( packPositionY + pixelData.GetHeight() )-0.5f ) / mHeight;// bottom

    return true;
  }

  return false;
}

void ImageAtlas::Remove( const Vector4& textureRect )
{
  mPacker.DeleteBlock( static_cast<SizeType>(textureRect.x*mWidth),
                       static_cast<SizeType>(textureRect.y*mHeight),
                       static_cast<SizeType>((textureRect.z-textureRect.x)*mWidth+1.f),
                       static_cast<SizeType>((textureRect.w-textureRect.y)*mHeight+1.f) );
}

void ImageAtlas::ObserverDestroyed( AtlasUploadObserver* observer )
{
  const std::size_t count = mLoadingTaskInfoContainer.Count();
  for( std::size_t i=0; i < count; ++i )
  {
    if( mLoadingTaskInfoContainer[i]->observer == observer )
    {
      // the observer is destructing, so its member function should not be called anymore
      mLoadingTaskInfoContainer[i]->observer = NULL;
    }
  }
}

void ImageAtlas::UploadToAtlas( uint32_t id, PixelData pixelData )
{
  if(  mLoadingTaskInfoContainer[0]->loadTaskId == id)
  {
    Rect<unsigned int> packRect( mLoadingTaskInfoContainer[0]->packRect  );
    if( !pixelData || ( pixelData.GetWidth() ==0 && pixelData.GetHeight() == 0 ))
    {
      if(!mBrokenImageUrl.empty()) // replace with the broken image
      {
        UploadBrokenImage( packRect );
      }
    }
    else
    {
      if( pixelData.GetWidth() < packRect.width || pixelData.GetHeight() < packRect.height  )
      {
        DALI_LOG_ERROR( "Can not upscale the image from actual loaded size [ %d, %d ] to specified size [ %d, %d ]\n",
            pixelData.GetWidth(), pixelData.GetHeight(),
            packRect.width, packRect.height );
      }

      mAtlas.Upload( pixelData, 0u, 0u, packRect.x, packRect.y, packRect.width, packRect.height );
    }

    if( mLoadingTaskInfoContainer[0]->observer )
    {
      mLoadingTaskInfoContainer[0]->observer->UploadCompleted();
      mLoadingTaskInfoContainer[0]->observer->Unregister( *this );
    }

    mLoadingTaskInfoContainer.Erase( mLoadingTaskInfoContainer.Begin() );
  }
}

void ImageAtlas::UploadBrokenImage( const Rect<unsigned int>& area )
{
  BitmapLoader loader = BitmapLoader::New(mBrokenImageUrl, ImageDimensions( area.width, area.height ) );
  loader.Load();
  SizeType loadedWidth = loader.GetPixelData().GetWidth();
  SizeType loadedHeight = loader.GetPixelData().GetHeight();

  bool needBackgroundClear = false;
  SizeType packX = area.x;
  SizeType packY = area.y;
  // locate the broken image in the middle.
  if( area.width > loadedWidth)
  {
    packX += (area.width - loadedWidth)/2;
    needBackgroundClear = true;
  }
  if( area.height > loadedHeight)
  {
    packY += (area.height - loadedHeight)/2;
    needBackgroundClear = true;
  }

  if( needBackgroundClear )
  {
    SizeType size = area.width * area.height * Pixel::GetBytesPerPixel( mPixelFormat );
    PixelBuffer* buffer = new PixelBuffer [size];
    PixelData background = PixelData::New( buffer, size, area.width, area.height, mPixelFormat, PixelData::DELETE_ARRAY );
    for( SizeType idx = 0; idx < size; idx++ )
    {
      buffer[idx] = 0x00;
    }
    mAtlas.Upload( background, 0u, 0u, area.x, area.y, area.width, area.height );
  }

  mAtlas.Upload( loader.GetPixelData(), 0u, 0u, packX, packY, loadedWidth, loadedHeight );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
