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

GaussianBlurView GaussianBlurView::New( const unsigned int numSamples, const float blurBellCurveWidth, const Pixel::Format renderTargetPixelFormat,
                                        const float downsampleWidthScale, const float downsampleHeightScale,
                                        bool blurUserImage)
{
  return Internal::GaussianBlurView::New( numSamples, blurBellCurveWidth, renderTargetPixelFormat,
                                           downsampleWidthScale, downsampleHeightScale,
                                           blurUserImage);
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

void GaussianBlurView::Add(Actor child)
{
  GetImpl(*this).Add(child);
}

void GaussianBlurView::Remove(Actor child)
{
  GetImpl(*this).Remove(child);
}

void GaussianBlurView::Activate()
{
  GetImpl(*this).Activate();
}

void GaussianBlurView::ActivateOnce()
{
  GetImpl(*this).ActivateOnce();
}

void GaussianBlurView::Deactivate()
{
  GetImpl(*this).Deactivate();
}

void GaussianBlurView::SetUserImageAndOutputRenderTarget(Image inputImage, FrameBufferImage outputRenderTarget)
{
  GetImpl(*this).SetUserImageAndOutputRenderTarget(inputImage, outputRenderTarget);
}

Property::Index GaussianBlurView::GetBlurStrengthPropertyIndex() const
{
  return GetImpl(*this).GetBlurStrengthPropertyIndex();
}

FrameBufferImage GaussianBlurView::GetBlurredRenderTarget() const
{
  return GetImpl(*this).GetBlurredRenderTarget();
}

void GaussianBlurView::SetBackgroundColor( const Vector4& color )
{
  GetImpl(*this).SetBackgroundColor(color);
}

Vector4 GaussianBlurView::GetBackgroundColor() const
{
  return GetImpl(*this).GetBackgroundColor();
}

GaussianBlurView::GaussianBlurViewSignal& GaussianBlurView::FinishedSignal()
{
  return GetImpl(*this).FinishedSignal();
}

} // namespace Toolkit

} // namespace Dali
