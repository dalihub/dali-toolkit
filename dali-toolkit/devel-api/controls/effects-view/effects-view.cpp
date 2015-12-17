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
#include <dali-toolkit/devel-api/controls/effects-view/effects-view.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/effects-view/effects-view-impl.h>

namespace Dali
{

namespace Toolkit
{

EffectsView EffectsView::New( EffectType type )
{
  EffectsView effectsView = Internal::EffectsView::New();
  GetImpl(effectsView).SetType( type );
  return effectsView;
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

EffectsView::EffectType EffectsView::GetType() const
{
  return GetImpl(*this).GetType();
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
