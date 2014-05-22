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
#include <dali-toolkit/public-api/controls/shadow-view/shadow-view.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/shadow-view/shadow-view-impl.h>

namespace
{

const float GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_WIDTH_SCALE = 1.0f;
const float GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_HEIGHT_SCALE = 1.0f;

} // namespace

namespace Dali
{

namespace Toolkit
{

ShadowView::ShadowView()
{
}

ShadowView::~ShadowView()
{
}

ShadowView::ShadowView(const ShadowView& handle)
  : Control( handle )
{
}

ShadowView& ShadowView::operator=(const ShadowView& rhs)
{
  if( &rhs != this )
  {
    Control::operator=(rhs);
  }
  return *this;
}

ShadowView ShadowView::New()
{
  return Internal::ShadowView::New(GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_WIDTH_SCALE,
                                   GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_HEIGHT_SCALE);
}

ShadowView ShadowView::New(float downsampleWidthScale, float downsampleHeightScale)
{
  return Internal::ShadowView::New(downsampleWidthScale, downsampleHeightScale);
}

ShadowView::ShadowView( Internal::ShadowView& implementation )
: Control( implementation )
{
}

ShadowView::ShadowView( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::ShadowView>(internal);
}

ShadowView ShadowView::DownCast( BaseHandle handle )
{
  return Control::DownCast<ShadowView, Internal::ShadowView>(handle);
}

void ShadowView::Add(Actor child)
{
  GetImpl(*this).Add(child);
}

void ShadowView::Remove(Actor child)
{
  GetImpl(*this).Remove(child);
}

void ShadowView::SetShadowPlane(ImageActor shadowPlane)
{
  GetImpl(*this).SetShadowPlane(shadowPlane);
}

void ShadowView::SetPointLight(Actor pointLight)
{
  GetImpl(*this).SetPointLight(pointLight);
}

void ShadowView::SetPointLightFieldOfView(float fieldOfView)
{
  GetImpl(*this).SetPointLightFieldOfView(fieldOfView);
}

void ShadowView::SetShadowColor(Vector4 color)
{
  GetImpl(*this).SetShadowColor(color);
}

void ShadowView::Activate()
{
  GetImpl(*this).Activate();
}

void ShadowView::Deactivate()
{
  GetImpl(*this).Deactivate();
}

Property::Index ShadowView::GetBlurStrengthPropertyIndex() const
{
  return GetImpl(*this).GetBlurStrengthPropertyIndex();
}

Property::Index ShadowView::GetShadowColorPropertyIndex() const
{
  return GetImpl(*this).GetShadowColorPropertyIndex();
}

} // namespace Toolkit

} // namespace Dali
