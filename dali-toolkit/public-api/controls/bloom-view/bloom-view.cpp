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
#include <dali-toolkit/public-api/controls/bloom-view/bloom-view.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/bloom-view/bloom-view-impl.h>

namespace Dali
{

namespace Toolkit
{

BloomView::BloomView()
{
}

BloomView::~BloomView()
{
}

BloomView::BloomView(const BloomView& handle)
  : Control( handle )
{
}

BloomView& BloomView::operator=(const BloomView& rhs)
{
  if( &rhs != this )
  {
    Control::operator=(rhs);
  }
  return *this;
}

BloomView BloomView::New()
{
  return Internal::BloomView::New();
}

BloomView BloomView::New( const unsigned int numSamples, const float blurBellCurveWidth, const Pixel::Format renderTargetPixelFormat,
                                        const float downsampleWidthScale, const float downsampleHeightScale)
{
  return Internal::BloomView::New( numSamples, blurBellCurveWidth, renderTargetPixelFormat,
                                           downsampleWidthScale, downsampleHeightScale);
}

BloomView::BloomView( Internal::BloomView& implementation )
: Control( implementation )
{
}

BloomView::BloomView( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::BloomView>(internal);
}

BloomView BloomView::DownCast( BaseHandle handle )
{
  return Control::DownCast<BloomView, Internal::BloomView>(handle);
}

void BloomView::Add(Actor child)
{
  GetImpl(*this).Add(child);
}

void BloomView::Remove(Actor child)
{
  GetImpl(*this).Remove(child);
}

void BloomView::Activate()
{
  GetImpl(*this).Activate();
}

void BloomView::Deactivate()
{
  GetImpl(*this).Deactivate();
}

Property::Index BloomView::GetBloomThresholdPropertyIndex() const
{
  return GetImpl(*this).GetBloomThresholdPropertyIndex();
}

Property::Index BloomView::GetBlurStrengthPropertyIndex() const
{
  return GetImpl(*this).GetBlurStrengthPropertyIndex();
}

Property::Index BloomView::GetBloomIntensityPropertyIndex() const
{
  return GetImpl(*this).GetBloomIntensityPropertyIndex();
}

Property::Index BloomView::GetBloomSaturationPropertyIndex() const
{
  return GetImpl(*this).GetBloomSaturationPropertyIndex();
}

Property::Index BloomView::GetImageIntensityPropertyIndex() const
{
  return GetImpl(*this).GetImageIntensityPropertyIndex();
}

Property::Index BloomView::GetImageSaturationPropertyIndex() const
{
  return GetImpl(*this).GetImageSaturationPropertyIndex();
}

} // namespace Toolkit

} // namespace Dali
