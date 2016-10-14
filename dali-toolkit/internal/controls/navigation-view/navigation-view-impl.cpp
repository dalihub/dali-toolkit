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
#include "navigation-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry-helper.h>

// INTERNAL INCLUDES

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace // to register type
{

BaseHandle Create()
{
  return Toolkit::NavigationView::New();
}

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::NavigationView, Toolkit::Control, Create )
DALI_TYPE_REGISTRATION_END()

} // namespace

NavigationView::NavigationView()
: Control(ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) )
{
}

NavigationView::~NavigationView()
{
  // Clear all the items in the stack, forces their destruction before NavigationView is destroyed.
  mContentStack.clear();
}

Toolkit::NavigationView NavigationView::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< NavigationView > internalNavigationView = new NavigationView();

  // Pass ownership to CustomActor handle
  Toolkit::NavigationView navigationView( *internalNavigationView );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalNavigationView->Initialize();

  return navigationView;
}

void NavigationView::OnStageConnection( int depth )
{
  Self().SetSensitive(true);
}

void NavigationView::Push( Actor& actor )
{
  // check the uninitialized item
  // check the duplicated push for the top item
  if(!actor )
  {
    return;
  }

  if( mContentStack.size() > 0 )
  {
    Self().Remove( mContentStack.back()  );
  }

  //push the new item into the stack and show it
  mContentStack.push_back(actor);
  Self().Add(actor);
}

Actor NavigationView::Pop()
{
  // cannot pop out the bottom-most item
  Actor poppedItem;
  if( mContentStack.size() > 1 )
  {
    // pop out the top item of the stack and show the new item right under the old one.
    Self().Remove(mContentStack.back());
    poppedItem = mContentStack.back();
    mContentStack.pop_back();
    Self().Add(mContentStack.back());
  }

  return poppedItem;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
