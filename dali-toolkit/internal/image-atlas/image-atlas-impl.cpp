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
#include "image-atlas-impl.h"

// EXTERNAL INCLUDES
#include <string.h>
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

ImageAtlas::ImageAtlas( SizeType width, SizeType height, Pixel::Format pixelFormat )
: mPacker( width, height ),
  mLoadQueue(),
  mCompleteQueue( new EventThreadCallback( MakeCallback( this, &ImageAtlas::UploadToAtlas ) ) ),
  mLoadingThread( mLoadQueue, mCompleteQueue ),
  mBrokenImageUrl(""),
  mBrokenImageSize(),
  mPixelFormat( pixelFormat ),
  mLoadingThreadStarted( false )
{
  mAtlas = Atlas::New( width, height, pixelFormat );
  mWidth = static_cast<float>(width);
  mHeight = static_cast<float>( height );
}

ImageAtlas::~ImageAtlas()
{
  if( mLoadingThreadStarted )
  {
    // add an empty task would stop the loading thread from contional wait.
    mLoadQueue.AddTask( NULL );
    // stop the loading thread
    mLoadingThread.Join();
    // The atlas can still be used as texture after ImageAtlas has been thrown away,
    // so make sure all the loaded bitmap been uploaded to atlas
    UploadToAtlas();
  }
}

IntrusivePtr<ImageAtlas> ImageAtlas::New( SizeType width, SizeType height, Pixel::Format pixelFormat )
{
  IntrusivePtr<ImageAtlas> internal = new ImageAtlas( width, height, pixelFormat );
  return internal;
}

Image ImageAtlas::GetAtlas()
{
  return mAtlas;
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
                         bool orientationCorrection )
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
        return Upload( textureRect, mBrokenImageUrl, mBrokenImageSize, FittingMode::DEFAULT, true );
      }
      else
      {
        textureRect = Vector4::ZERO;
        return true;
      }
    }
  }

  if( static_cast<unsigned int>(dimensions.GetWidth() * dimensions.GetHeight()) > mPacker.GetAvailableArea() )
  {
    return false;
  }

  unsigned int packPositionX = 0;
  unsigned int packPositionY = 0;
  if( mPacker.Pack( dimensions.GetWidth(), dimensions.GetHeight(), packPositionX, packPositionY ) )
  {
    if( !mLoadingThreadStarted )
    {
      mLoadingThread.Start();
      mLoadingThreadStarted = true;
    }

    LoadingTask* newTask = new LoadingTask(BitmapLoader::New(url, size, fittingMode, SamplingMode::BOX_THEN_LINEAR, orientationCorrection ),
                                           packPositionX, packPositionY, dimensions.GetWidth(), dimensions.GetHeight());
    mLoadQueue.AddTask( newTask );

    // apply the half pixel correction
    textureRect.x = ( static_cast<float>( packPositionX ) +0.5f ) / mWidth; // left
    textureRect.y = ( static_cast<float>( packPositionY ) +0.5f ) / mHeight; // right
    textureRect.z = ( static_cast<float>( packPositionX + dimensions.GetX() )-0.5f ) / mWidth; // right
    textureRect.w = ( static_cast<float>( packPositionY + dimensions.GetY() )-0.5f ) / mHeight;// bottom

    return true;
  }

  return false;
}

bool ImageAtlas::Upload( Vector4& textureRect, PixelDataPtr pixelData )
{
  unsigned int packPositionX = 0;
  unsigned int packPositionY = 0;
  if( mPacker.Pack( pixelData->GetWidth(), pixelData->GetHeight(), packPositionX, packPositionY ) )
  {
    mAtlas.Upload( pixelData, packPositionX, packPositionY );

    // apply the half pixel correction
    textureRect.x = ( static_cast<float>( packPositionX ) +0.5f ) / mWidth; // left
    textureRect.y = ( static_cast<float>( packPositionY ) +0.5f ) / mHeight; // right
    textureRect.z = ( static_cast<float>( packPositionX + pixelData->GetWidth() )-0.5f ) / mWidth; // right
    textureRect.w = ( static_cast<float>( packPositionY + pixelData->GetHeight() )-0.5f ) / mHeight;// bottom

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

void ImageAtlas::UploadToAtlas()
{
  while( LoadingTask* next = mCompleteQueue.NextTask() )
  {
    if( ! next->loader.IsLoaded() )
    {
      if(!mBrokenImageUrl.empty()) // replace with the broken image
      {
        UploadBrokenImage( next->packRect );
      }

      DALI_LOG_ERROR( "Failed to load the image: %s\n", (next->loader.GetUrl()).c_str());
    }
    else
    {
      if( next->loader.GetPixelData()->GetWidth() < next->packRect.width || next->loader.GetPixelData()->GetHeight() < next->packRect.height  )
      {
        DALI_LOG_ERROR( "Can not upscale the image from actual loaded size [ %d, %d ] to specified size [ %d, %d ]\n",
                        next->loader.GetPixelData()->GetWidth(),
                        next->loader.GetPixelData()->GetHeight(),
                        next->packRect.width,
                        next->packRect.height );
      }

      mAtlas.Upload( next->loader.GetPixelData(), next->packRect.x, next->packRect.y );
    }

    delete next;
  }
}

void ImageAtlas::UploadBrokenImage( const Rect<SizeType>& area )
{
  BitmapLoader loader = BitmapLoader::New(mBrokenImageUrl, ImageDimensions( area.width, area.height ) );
  loader.Load();
  SizeType loadedWidth = loader.GetPixelData()->GetWidth();
  SizeType loadedHeight = loader.GetPixelData()->GetHeight();

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
    PixelDataPtr background = PixelData::New( buffer, area.width, area.height, mPixelFormat, PixelData::DELETE_ARRAY );
    for( SizeType idx = 0; idx < size; idx++ )
    {
      buffer[idx] = 0x00;
    }
    mAtlas.Upload( background, area.x, area.y );
  }

  mAtlas.Upload( loader.GetPixelData(), packX, packY );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
