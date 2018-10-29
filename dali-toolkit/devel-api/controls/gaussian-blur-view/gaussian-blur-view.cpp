/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/gaussian-blur-view/gaussian-blur-view.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/gaussian-blur-view/gaussian-blur-view-impl.h>

namespace Dali
{

namespace Toolkit
{

GaussianBlurView::GaussianBlurView()
{
}

GaussianBlurView::~GaussianBlurView()
{
}

GaussianBlurView::GaussianBlurView(const GaussianBlurView& handle)
  : Control( handle )
{
}

GaussianBlurView& GaussianBlurView::operator=(const GaussianBlurView& rhs)
{
  if( &rhs != this )
  {
    Control::operator=(rhs);
  }
  return *this;
}

GaussianBlurView GaussianBlurView::New()
{
  return Internal::GaussianBlurView::New();
}

GaussianBlurView GaussianBlurView::New(
  const unsigned int numSamples, const float blurBellCurveWidth,
  const float downsampleWidthScale, const float downsampleHeightScale )
{
  return Internal::GaussianBlurView::New( numSamples, blurBellCurveWidth, downsampleWidthScale, downsampleHeightScale );
}

GaussianBlurView::GaussianBlurView( Internal::GaussianBlurView& implementation )
: Control( implementation )
{
}

GaussianBlurView::GaussianBlurView( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::GaussianBlurView>(internal);
}

GaussianBlurView GaussianBlurView::DownCast( BaseHandle handle )
{
  return Control::DownCast<GaussianBlurView, Internal::GaussianBlurView>(handle);
}

void GaussianBlurView::Activate()
{
  GetImpl(*this).Activate();
}

void GaussianBlurView::Deactivate()
{
  GetImpl(*this).Deactivate();
}

ImageView GaussianBlurView::GetBlurredImageView() const
{
  return GetImpl(*this).GetBlurredImageView();
}

void GaussianBlurView::SetBackgroundColor( const Vector4& color )
{
  GetImpl(*this).SetBackgroundColor(color);
}

Vector4 GaussianBlurView::GetBackgroundColor() const
{
  return GetImpl(*this).GetBackgroundColor();
}

void GaussianBlurView::SetBlurBellCurveWidth(float blurBellCurveWidth)
{
  GetImpl(*this).SetBlurBellCurveWidth( blurBellCurveWidth );
}

float GaussianBlurView::GetBlurBellCurveWidth()
{
  return GetImpl(*this).GetBlurBellCurveWidth();
}

} // namespace Toolkit

} // namespace Dali
