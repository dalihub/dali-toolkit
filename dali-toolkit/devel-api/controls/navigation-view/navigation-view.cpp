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
#include <dali-toolkit/devel-api/controls/navigation-view/navigation-view.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/navigation-view/navigation-view-impl.h>

namespace Dali
{

namespace Toolkit
{

NavigationView::NavigationView()
{
}

NavigationView::NavigationView( const NavigationView& handle )
: Control(handle)
{
}

NavigationView& NavigationView::operator=( const NavigationView& handle)
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

NavigationView::~NavigationView()
{
}

NavigationView NavigationView::New()
{
  return Internal::NavigationView::New();
}

NavigationView NavigationView::DownCast( BaseHandle handle )
{
  return Control::DownCast<NavigationView, Internal::NavigationView>(handle);
}

NavigationView::NavigationView( Internal::NavigationView& implementation )
: Control( implementation )
{
}

NavigationView::NavigationView( Dali::Internal::CustomActor* internal )
: Control( internal)
{
  VerifyCustomActorPointer<Internal::NavigationView>(internal);
}


void NavigationView::Push( Actor actor )
{
  GetImpl( *this ).Push( actor );
}

Actor NavigationView::Pop()
{
  return GetImpl( *this ).Pop();
}

} // namespace Toolkit

} // namespace Dali
