//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/internal/controls/image-view/image-view-impl.h>
#include <dali-toolkit/public-api/controls/buttons/button.h>

using namespace Dali;

namespace
{
const float DEFAULT_MINIMUM_DETAIL = 0.125f;      ///< Default Minimum Detail level 12.5% of original size.
const float DEFAULT_MAXIMUM_DETAIL = 1.0f;        ///< Default Maximum Detail level 100% (original size)
const float CAMERA_100_PCT_DISTANCE(1695.0f);     ///< Based on Camera/Viewport/Projection settings at this distance object is 100% size.
} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// ImageView
///////////////////////////////////////////////////////////////////////////////////////////////////

const std::string ImageView::DETAIL_PROPERTY_NAME( "image-view-detail" );

ImageView::ImageView()
{
}

ImageView::ImageView( const ImageView& handle )
: Control( handle )
{
}

ImageView& ImageView::operator=( const ImageView& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

ImageView::ImageView(Internal::ImageView& implementation)
: Control(implementation)
{
}

ImageView::ImageView( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::ImageView>(internal);
}

ImageView ImageView::New()
{
  return Internal::ImageView::New();
}

ImageView ImageView::DownCast( BaseHandle handle )
{
  return Control::DownCast<ImageView, Internal::ImageView>( handle );
}

ImageView::~ImageView()
{
}

void ImageView::SetImage(const std::string& filename, ImageType type)
{
  GetImpl(*this).SetImage( filename, type, DEFAULT_MINIMUM_DETAIL, DEFAULT_MAXIMUM_DETAIL );
}

void ImageView::SetImage(const std::string& filename, ImageType type, float min, float max)
{
  GetImpl(*this).SetImage( filename, type, min, max );
}

void ImageView::SetImage(Image image)
{
  GetImpl(*this).SetImage( image );
}

void ImageView::SetCameraActor(CameraActor camera)
{
  // TODO: Default detail factor should be calculated based on
  // current Camera's field of view/viewport/projection.
  // Ideal place would be inside the constraint, and have the Camera
  // settings as properties.
  GetImpl(*this).SetCameraActor( camera, CAMERA_100_PCT_DISTANCE );
}

void ImageView::SetCameraActor(CameraActor camera, float detailFactor)
{
  GetImpl(*this).SetCameraActor( camera, detailFactor );
}

void ImageView::SetDetail(float detail)
{
  GetImpl(*this).SetDetail( detail );
}

} // namespace Toolkit

} // namespace Dali
