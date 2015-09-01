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

#include <dali-toolkit/devel-api/controls/popup/confirmation-popup.h>

// INTERNAL INCLUDES

#include <dali-toolkit/internal/controls/popup/confirmation-popup-impl.h>
#include <dali/public-api/actors/image-actor.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{


ConfirmationPopup::ConfirmationPopup()
: Popup()
{
}

ConfirmationPopup::ConfirmationPopup( Internal::ConfirmationPopup& implementation )
: Popup( implementation )
{
}

ConfirmationPopup::ConfirmationPopup( const ConfirmationPopup& confirmationPopup )
: Popup( confirmationPopup )
{
}

ConfirmationPopup& ConfirmationPopup::operator=( const ConfirmationPopup& confirmationPopup )
{
  if( &confirmationPopup != this )
  {
    Popup::operator=( confirmationPopup );
  }
  return *this;
}

ConfirmationPopup::ConfirmationPopup( Dali::Internal::CustomActor* internal )
: Popup( internal )
{
  VerifyCustomActorPointer<Internal::ConfirmationPopup>( internal );
}

ConfirmationPopup::~ConfirmationPopup()
{
}

ConfirmationPopup ConfirmationPopup::New()
{
  return Internal::ConfirmationPopup::New();
}

ConfirmationPopup ConfirmationPopup::DownCast( BaseHandle handle )
{
  return Control::DownCast<ConfirmationPopup, Internal::ConfirmationPopup>( handle );
}


} // namespace Toolkit

} // namespace Dali
