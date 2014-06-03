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

#include "view.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

#include <dali-toolkit/internal/controls/view/view-impl.h>

namespace Dali
{

namespace Toolkit
{

const char* const View::SIGNAL_ORIENTATION_ANIMATION_START = "orientation-animation-start";

View::View()
{
}

View::View( const View& handle )
: Control( handle )
{
}

View& View::operator=( const View& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

View::~View()
{
}

View View::New( bool fullscreen )
{
  return Internal::View::New( fullscreen );
}

View View::DownCast( BaseHandle handle )
{
  return Control::DownCast<View, Internal::View>(handle);
}

Layer View::GetContentLayer( unsigned int index ) const
{
  return GetImpl( *this ).GetContentLayer( index );
}

unsigned int View::AddContentLayer( Layer layer )
{
  return GetImpl( *this ).AddContentLayer( layer );
}

void View::RemoveContentLayer( Layer layer )
{
  GetImpl( *this ).RemoveContentLayer( layer );
}

Layer View::GetBackgroundLayer() const
{
  return GetImpl( *this ).GetBackgroundLayer();
}

void View::SetBackground( ImageActor image )
{
  GetImpl( *this ).SetBackground( image );
}

void View::SetOrientationFunction( Degree portrait, Degree landscale, Degree portraitInverse, Degree landscapeInverse )
{
  GetImpl( *this ).SetOrientationFunction( portrait, landscale, portraitInverse, landscapeInverse );
}

void View::OrientationChanged( Orientation orientation )
{
  GetImpl( *this ).OrientationChanged( orientation );
}

void View::SetAutoRotate( bool enabled )
{
  GetImpl( *this ).SetAutoRotate( enabled );
}

View::OrientationAnimationStartedSignalV2& View::OrientationAnimationStartedSignal()
{
  return GetImpl( *this ).OrientationAnimationStartedSignal();
}

View::View( Internal::View& implementation )
: Control( implementation )
{
}

View::View( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::View>(internal);
}

} // namespace Toolkit

} // namespace Dali
