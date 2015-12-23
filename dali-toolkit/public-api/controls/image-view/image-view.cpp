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
#include <dali-toolkit/public-api/controls/image-view/image-view.h>

// EXTERNAL INCLUDES
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/image-view/image-view-impl.h>

namespace Dali
{

namespace Toolkit
{

ImageView::ImageView()
{
}

ImageView::ImageView( const ImageView& imageView )
: Control( imageView )
{
}

ImageView& ImageView::operator=( const ImageView& imageView )
{
  if( &imageView != this )
  {
    Control::operator=( imageView );
  }
  return *this;
}

ImageView::~ImageView()
{
}

ImageView ImageView::New()
{
  return Internal::ImageView::New();
}

ImageView ImageView::New( Image image )
{
  ImageView imageView = Internal::ImageView::New();
  imageView.SetImage( image );
  return imageView;
}

ImageView ImageView::New( const std::string& url )
{
  ImageView imageView = Internal::ImageView::New();
  imageView.SetImage( url, ImageDimensions() );
  return imageView;
}

ImageView ImageView::New( const std::string& url, ImageDimensions size )
{
  ImageView imageView = Internal::ImageView::New();
  imageView.SetImage( url, size );
  return imageView;
}

ImageView ImageView::DownCast( BaseHandle handle )
{
  return Control::DownCast<ImageView, Internal::ImageView>( handle );
}

void ImageView::SetImage( Image image )
{
  Dali::Toolkit::GetImpl( *this ).SetImage( image );
}

void ImageView::SetImage( const std::string& url )
{
  Dali::Toolkit::GetImpl( *this ).SetImage( url, ImageDimensions() );
}

void ImageView::SetImage( const std::string& url, ImageDimensions size )
{
  Dali::Toolkit::GetImpl( *this ).SetImage( url, size );
}

Image ImageView::GetImage() const
{
  return Image();
}

ImageView::ImageView( Internal::ImageView& implementation )
 : Control( implementation )
{
}

ImageView::ImageView( Dali::Internal::CustomActor* internal )
 : Control( internal )
{
  VerifyCustomActorPointer<Internal::ImageView>( internal );
}

} // namespace Toolkit

} // namespace Dali
