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

// EXTERNAL INCLUDES

// CLASS HEADER
#include <dali-toolkit/public-api/controls/effects-view/effects-view.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/effects-view/effects-view-impl.h>

namespace Dali
{

namespace Toolkit
{

EffectsView EffectsView::New()
{
  return Internal::EffectsView::New();
}

EffectsView::EffectsView()
{
}

EffectsView::EffectsView( const EffectsView& handle )
: Control( handle )
{
}

EffectsView& EffectsView::operator=( const EffectsView& rhs )
{
  if( &rhs != this )
  {
    Control::operator=(rhs);
  }
  return *this;
}

EffectsView EffectsView::DownCast( BaseHandle handle )
{
  return Control::DownCast<EffectsView, Internal::EffectsView>(handle);
}

EffectsView::~EffectsView()
{
}

void EffectsView::SetType( EffectsView::EffectType type )
{
  GetImpl(*this).SetType( type );
}

EffectsView::EffectType EffectsView::GetType() const
{
  return GetImpl(*this).GetType();
}

void EffectsView::Enable()
{
  GetImpl(*this).Enable();
}

void EffectsView::Disable()
{
  GetImpl(*this).Disable();
}

void EffectsView::Refresh()
{
  GetImpl(*this).Refresh();
}

void EffectsView::SetRefreshOnDemand( bool onDemand )
{
  GetImpl(*this).SetRefreshOnDemand( onDemand );
}

void EffectsView::SetPixelFormat( Pixel::Format pixelFormat )
{
  GetImpl(*this).SetPixelFormat( pixelFormat );
}

void EffectsView::SetOutputImage( FrameBufferImage image )
{
  GetImpl(*this).SetOutputImage( image );
}

FrameBufferImage EffectsView::GetOutputImage()
{
  return GetImpl(*this).GetOutputImage();
}

Property::Index EffectsView::GetEffectSizePropertyIndex() const
{
  return GetImpl(*this).GetEffectSizePropertyIndex();
}

Property::Index EffectsView::GetEffectStrengthPropertyIndex() const
{
  return GetImpl(*this).GetEffectStrengthPropertyIndex();
}

Property::Index EffectsView::GetEffectOffsetPropertyIndex() const
{
  return GetImpl(*this).GetEffectOffsetPropertyIndex();
}

Property::Index EffectsView::GetEffectColorPropertyIndex() const
{
  return GetImpl(*this).GetEffectColorPropertyIndex();
}

void EffectsView::SetBackgroundColor( const Vector4& color )
{
  GetImpl(*this).SetBackgroundColor(color);
}

Vector4 EffectsView::GetBackgroundColor() const
{
  return GetImpl(*this).GetBackgroundColor();
}


EffectsView::EffectsView( Internal::EffectsView& implementation )
: Control( implementation )
{
}

EffectsView::EffectsView( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::EffectsView>(internal);
}


} //namespace Toolkit

} //namespace Dali
