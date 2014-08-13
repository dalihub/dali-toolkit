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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/image-view/masked-image-view.h>
#include <dali-toolkit/internal/controls/image-view/masked-image-view-impl.h>

namespace Dali
{

namespace Toolkit
{

const float MaskedImageView::DEFAULT_MAXIMUM_SOURCE_SCALE(3.0f);

MaskedImageView::MaskedImageView()
{
}

MaskedImageView::MaskedImageView( const MaskedImageView& handle )
: Control( handle )
{
}

MaskedImageView& MaskedImageView::operator=( const MaskedImageView& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

MaskedImageView::~MaskedImageView()
{
}

MaskedImageView MaskedImageView::New( unsigned int targetWidth,
                                      unsigned int targetHeight,
                                      Image sourceImage,
                                      Image maskImage )
{
  return Internal::MaskedImageView::New( targetWidth, targetHeight, sourceImage, maskImage );
}

MaskedImageView MaskedImageView::DownCast( BaseHandle handle )
{
  return Control::DownCast<MaskedImageView, Internal::MaskedImageView>( handle );
}

void MaskedImageView::SetSourceImage( Image sourceImage )
{
  GetImpl(*this).SetSourceImage( sourceImage );
}

Image MaskedImageView::GetSourceImage()
{
  return GetImpl(*this).GetSourceImage();
}

void MaskedImageView::SetMaskImage( Image sourceImage )
{
  GetImpl(*this).SetMaskImage( sourceImage );
}

Image MaskedImageView::GetMaskImage()
{
  return GetImpl(*this).GetMaskImage();
}

Property::Index MaskedImageView::GetPropertyIndex( MaskedImageView::CustomProperty customProperty ) const
{
  return GetImpl(*this).GetPropertyIndex( customProperty );
}

void MaskedImageView::Pause()
{
  GetImpl(*this).Pause();
}

void MaskedImageView::Resume()
{
  GetImpl(*this).Resume();
}

bool MaskedImageView::IsPaused() const
{
  return GetImpl(*this).IsPaused();
}

void MaskedImageView::SetEditMode( MaskedImageView::EditMode editMode )
{
  GetImpl(*this).SetEditMode( editMode );
}

MaskedImageView::EditMode MaskedImageView::GetEditMode() const
{
  return GetImpl(*this).GetEditMode();
}

void MaskedImageView::SetSourceAspectRatio( float widthOverHeight )
{
  GetImpl(*this).SetSourceAspectRatio( widthOverHeight );
}

float MaskedImageView::GetSourceAspectRatio() const
{
  return GetImpl(*this).GetSourceAspectRatio();
}

void MaskedImageView::SetMaximumSourceScale( float scale )
{
  GetImpl(*this).SetMaximumSourceScale( scale );
}

float MaskedImageView::GetMaximumSourceScale() const
{
  return GetImpl(*this).GetMaximumSourceScale();
}

void MaskedImageView::SetSourceRotation( MaskedImageView::ImageRotation rotation )
{
  GetImpl(*this).SetSourceRotation( rotation );
}

MaskedImageView::ImageRotation MaskedImageView::GetSourceRotation() const
{
  return GetImpl(*this).GetSourceRotation();
}

MaskedImageView::MaskedImageViewSignal& MaskedImageView::MaskFinishedSignal()
{
  return GetImpl(*this).MaskFinishedSignal();
}

MaskedImageView::MaskedImageView(Internal::MaskedImageView& implementation)
: Control(implementation)
{
}

MaskedImageView::MaskedImageView( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::MaskedImageView>(internal);
}

} // namespace Toolkit

} // namespace Dali
