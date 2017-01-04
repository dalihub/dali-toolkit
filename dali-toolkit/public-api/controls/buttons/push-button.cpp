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
#include <dali-toolkit/public-api/controls/buttons/push-button.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

#include <dali/public-api/images/resource-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/buttons/push-button-impl.h>

namespace Dali
{

namespace Toolkit
{

PushButton::PushButton()
: Button()
{
}

PushButton::PushButton( Internal::PushButton& implementation )
: Button( implementation )
{
}

PushButton::PushButton( const PushButton& pushButton )
: Button( pushButton )
{
}

PushButton& PushButton::operator=( const PushButton& pushButton )
{
  if( &pushButton != this )
  {
    Button::operator=( pushButton );
  }
  return *this;
}

PushButton::PushButton( Dali::Internal::CustomActor* internal )
: Button( internal )
{
  VerifyCustomActorPointer<Internal::PushButton>(internal);
}

PushButton::~PushButton()
{
}

PushButton PushButton::New()
{
  return Internal::PushButton::New();
}

PushButton PushButton::DownCast( BaseHandle handle )
{
  return Control::DownCast<PushButton, Internal::PushButton>(handle);
}

// Deprecated API

void PushButton::SetButtonImage( Actor image )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetButtonImage() is deprecated and will be removed from next release. Use Button.SetProperty UNSELECTED_STATE_IMAGE or Styling file instead.\n" );

  // Passing Image as Actor not supported
}

void PushButton::SetBackgroundImage( Actor image )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetBackgroundImage() is deprecated and will be removed from next release.\n" );

  // Passing Image as Actor not supported}
}

void PushButton::SetSelectedImage( Actor image )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetSelectedImage() is deprecated and will be removed from next release. Use Button.SetProperty SELECTED_STATE_IMAGE or Styling file instead.\n" );

  // Passing Image as Actor not supported}
}

void PushButton::SetSelectedBackgroundImage( Actor image )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetSelectedBackgroundImage() is deprecated and will be removed from next release.\n" );

  // Passing Image as Actor not supported}
}

void PushButton::SetDisabledBackgroundImage( Actor image )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetDisabledBackgroundImage() is deprecated and will be removed from next release.\n" );
  // Passing Image as Actor not supported}
}

void PushButton::SetDisabledImage( Actor image )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetDisabledImage() is deprecated and will be removed from next release. Use Button.SetProperty DISABLED_STATE_IMAGE or Styling file instead.\n" );
  // Passing Image as Actor not supported
}

void PushButton::SetDisabledSelectedImage( Actor image )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetDisabledSelectedImage() is deprecated and will be removed from next release.\n" );
  // Passing Image as Actor not supported
}

} // namespace Toolkit

} // namespace Dali
