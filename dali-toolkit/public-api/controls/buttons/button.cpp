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
#include <dali-toolkit/public-api/controls/buttons/button.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/images/resource-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/buttons/button-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>

namespace Dali
{

namespace Toolkit
{

Button::Button()
{}

Button::Button( const Button& button )
: Control( button )
{
}

Button& Button::operator=( const Button& button )
{
  if( &button != this )
  {
    Control::operator=( button );
  }
  return *this;
}

Button::~Button()
{
}

Button Button::DownCast( BaseHandle handle )
{
  return Control::DownCast<Button, Internal::Button>(handle);
}

void Button::SetDisabled( bool disabled )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetDisabled() is deprecated and will be removed from next release. Use SetProperty DISABLED or Styling file instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetDisabled( disabled );
}

bool Button::IsDisabled() const
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: IsDisabled() is deprecated and will be removed from next release. Use GetProperty DISABLED instead.\n" );

  return Dali::Toolkit::GetImplementation( *this ).IsDisabled();
}

void Button::SetAutoRepeating( bool autoRepeating )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetAutoRepeating() is deprecated and will be removed from next release. Use SetProperty AUTO_REPEATING or Styling file instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetAutoRepeating( autoRepeating );
}

bool Button::IsAutoRepeating() const
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: IsAutoRepeating() is deprecated and will be removed from next release. Use GetProperty AUTO_REPEATING instead.\n" );

  return Dali::Toolkit::GetImplementation( *this ).IsAutoRepeating();
}

void Button::SetInitialAutoRepeatingDelay( float initialAutoRepeatingDelay )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetInitialAutoRepeatingDelay() is deprecated and will be removed from next release. Use SetProperty INITIAL_AUTO_REPEATING_DELAY or Styling file instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetInitialAutoRepeatingDelay( initialAutoRepeatingDelay );
}

float Button::GetInitialAutoRepeatingDelay() const
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetPositionInheritanceMode() is deprecated and will be removed from next release. Use GetProperty INITIAL_AUTO_REPEATING_DELAY or Styling file instead.\n" );

  return Dali::Toolkit::GetImplementation( *this ).GetInitialAutoRepeatingDelay();
}

void Button::SetNextAutoRepeatingDelay( float nextAutoRepeatingDelay )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetNextAutoRepeatingDelay() is deprecated and will be removed from next release. Use SetProperty NEXT_AUTO_REPEATING_DELAY or Styling file instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetNextAutoRepeatingDelay( nextAutoRepeatingDelay );
}

float Button::GetNextAutoRepeatingDelay() const
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: GetNextAutoRepeatingDelay() is deprecated and will be removed from next release. Use GetProperty NEXT_AUTO_REPEATING_DELAY instead.\n" );

  return Dali::Toolkit::GetImplementation( *this ).GetNextAutoRepeatingDelay();
}

void Button::SetTogglableButton( bool togglable )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetTogglableButton() is deprecated and will be removed from next release. Use SetProperty TOGGLABLE or Styling file instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetTogglableButton( togglable );
}

bool Button::IsTogglableButton() const
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: IsTogglableButton() is deprecated and will be removed from next release. Use GetProperty TOGGLABLE instead.\n" );

  return Dali::Toolkit::GetImplementation( *this ).IsTogglableButton();
}

void Button::SetSelected( bool selected )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetSelected() is deprecated and will be removed from next release. Use SetProperty SELECTED instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetSelected( selected );
}

bool Button::IsSelected() const
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: IsSelected() is deprecated and will be removed from next release. Use GetProperty SELECTED instead.\n" );

  return Dali::Toolkit::GetImplementation( *this ).IsSelected();
}

void Button::SetAnimationTime( float animationTime )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetAnimationTime() is deprecated and will be removed from next release. Use Styling file to set animation instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetAnimationTime( animationTime );
}

float Button::GetAnimationTime() const
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: GetAnimationTime() is deprecated and will be removed from next release.\n" );

  return Dali::Toolkit::GetImplementation( *this ).GetAnimationTime();
}

void Button::SetLabelText( const std::string& label )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetLabelText() is deprecated and will be removed from next release. Use SetProperty LABEL or Styling file instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetLabelText( label );
}

std::string Button::GetLabelText() const
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: GetLabelText() is deprecated and will be removed from next release. Use GetProperty LABEL instead.\n" );

  return Dali::Toolkit::GetImplementation( *this ).GetLabelText();
}

void Button::SetUnselectedImage( const std::string& filename )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetUnselectedImage() is deprecated and will be removed from next release. Use SetProperty UNSELECTED_STATE_IMAGE or Styling file instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetUnselectedImage( filename );
}

void Button::SetBackgroundImage( const std::string& filename )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetBackgroundImage() is deprecated and will be removed from next release. Use Styling file instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetBackgroundImage( filename );
}

void Button::SetSelectedImage( const std::string& filename )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetSelectedImage() is deprecated and will be removed from next release. Use SetProperty SELECTED_STATE_IMAGE or Styling file instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetSelectedImage( filename );
}

void Button::SetSelectedBackgroundImage( const std::string& filename )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetSelectedBackgroundImage() is deprecated and will be removed from next release. Use Styling file instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetSelectedBackgroundImage( filename );
}

void Button::SetDisabledBackgroundImage( const std::string& filename )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetDisabledBackgroundImage() is deprecated and will be removed from next release. Use SetProperty DISABLED_STATE_IMAGE or Styling file instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetDisabledBackgroundImage( filename );
}

void Button::SetDisabledImage( const std::string& filename )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetDisabledImage() is deprecated and will be removed from next release. Use Styling file instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetDisabledImage( filename );
}

void Button::SetDisabledSelectedImage( const std::string& filename )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetDisabledSelectedImage() is deprecated and will be removed from next release. Use Styling file instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetDisabledSelectedImage( filename );
}

Button::ButtonSignalType& Button::PressedSignal()
{
  return Dali::Toolkit::GetImplementation( *this ).PressedSignal();
}

Button::ButtonSignalType& Button::ReleasedSignal()
{
  return Dali::Toolkit::GetImplementation( *this ).ReleasedSignal();
}

Button::ButtonSignalType& Button::ClickedSignal()
{
  return Dali::Toolkit::GetImplementation( *this ).ClickedSignal();
}

Button::ButtonSignalType& Button::StateChangedSignal()
{
  return Dali::Toolkit::GetImplementation( *this ).StateChangedSignal();
}

// Deprecated API

void Button::SetLabel( Actor label )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetLabel() is deprecated and will be removed from next release. Use SetProperty LABEL instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetLabel( label );
}

void Button::SetButtonImage( Image image )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetButtonImage() is deprecated and will be removed from next release. Use Styling file instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetButtonImage( image );
}

void Button::SetSelectedImage( Image image )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: SetSelectedImage() is deprecated and will be removed from next release. Use Styling file instead.\n" );

  Dali::Toolkit::GetImplementation( *this ).SetSelectedImage( image );
}

Actor Button::GetButtonImage() const
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: GetButtonImage() is deprecated and will be removed from next release.\n" );

  return Dali::Toolkit::GetImplementation( *this ).GetButtonImage();
}

Actor Button::GetSelectedImage() const
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: GetSelectedImage() is deprecated and will be removed from next release.\n" );

  return Dali::Toolkit::GetImplementation( *this ).GetSelectedImage();
}

Button::Button( Internal::Button& implementation )
: Control( implementation )
{
}

Button::Button( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::Button>(internal);
}

} // namespace Toolkit

} // namespace Dali
