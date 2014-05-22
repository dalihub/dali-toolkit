/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include "image-filter.h"

// INTERNAL INCLUDES

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

} // namespace

ImageFilter::ImageFilter()
: mBackgroundColor( Vector4( 1.0f, 1.0f, 1.0f, 0.0f ) ),
  mTargetSize( Vector2::ZERO ),
  mPixelFormat( Pixel::RGBA8888 ),
  mRefreshOnDemand( false ),
  mDebugRender( false )
{
}

ImageFilter::~ImageFilter()
{
}

void ImageFilter::SetRefreshOnDemand( bool onDemand )
{
  mRefreshOnDemand = onDemand;
}

void ImageFilter::SetInputImage( Image image )
{
  mInputImage = image;
}

void ImageFilter::SetOutputImage( FrameBufferImage image )
{
  mOutputImage = image;
}

void ImageFilter::SetSize( const Vector2& size )
{
  mTargetSize = size;
}

void ImageFilter::SetPixelFormat( Pixel::Format pixelFormat )
{
  mPixelFormat = pixelFormat;
}

void ImageFilter::SetKernel( const FilterKernel& kernel )
{
  mKernel = kernel;
}

const ImageFilter::FilterKernel& ImageFilter::GetKernel() const
{
  return mKernel;
}

size_t ImageFilter::GetKernelSize() const
{
  return mKernel.size();
}

void ImageFilter::CreateKernel( const float* weights, size_t count )
{
  if( (mTargetSize.width * mTargetSize.height ) > 0.0f )
  {
    Vector2 pixelsToUV( 1.0f / mTargetSize.width, 1.0f / mTargetSize.height );

    mKernel.clear();

    mKernel.push_back( Vector3( 0.0f, 0.0f, weights[0] ) );
    for( size_t i = 0; i < count >> 1; ++i )
    {
      float offset = 1.5f + (i << 1);

      mKernel.push_back( Vector3( pixelsToUV.x * offset, pixelsToUV.y * offset, weights[(i << 1) + 1] ) );
      mKernel.push_back( Vector3( -pixelsToUV.x * offset, -pixelsToUV.y * offset, weights[(i << 1) + 2] ) );
    }
  }
}

void ImageFilter::SetRootActor( Actor rootActor )
{
  mRootActor = rootActor;
}

void ImageFilter::SetBackgroundColor( const Vector4& color )
{
  mBackgroundColor = color;
}

void ImageFilter::RenderDebug( bool flag )
{
  mDebugRender = flag;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
