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
#include "push-button-impl.h"

// EXTERNAL INCLUDES
#include <algorithm>
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include "push-button-default-painter-impl.h"

#include <dali-toolkit/internal/controls/relayout-helper.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

BaseHandle Create()
{
  return Toolkit::PushButton::New();
}

TypeRegistration typeRegistration( typeid(Toolkit::PushButton), typeid(Toolkit::Button), Create );

SignalConnectorType signalConnector1( typeRegistration, Toolkit::PushButton::SIGNAL_PRESSED , &PushButton::DoConnectSignal );
SignalConnectorType signalConnector2( typeRegistration, Toolkit::PushButton::SIGNAL_RELEASED, &PushButton::DoConnectSignal );

TypeAction action1( typeRegistration, Toolkit::PushButton::ACTION_PUSH_BUTTON_CLICK, &PushButton::DoAction );

} // unnamed namespace

namespace
{

const unsigned int INITIAL_AUTOREPEATING_DELAY( 0.15f );
const unsigned int NEXT_AUTOREPEATING_DELAY( 0.05f );

const float TEXT_PADDING = 12.0f;

// Helper function used to cast a ButtonPainter to PushButtonDefaultPainter
PushButtonDefaultPainterPtr GetPushButtonPainter( Dali::Toolkit::Internal::ButtonPainterPtr painter )
{
  return static_cast<PushButtonDefaultPainter*>( painter.Get() );
}

/**
 * Find the first image actor in the actor hierarchy
 */
ImageActor FindImageActor( Actor root )
{
  ImageActor imageActor = ImageActor::DownCast( root );
  if( !imageActor && root )
  {
    for( unsigned int i = 0, numChildren = root.GetChildCount(); i < numChildren; ++i )
    {
      ImageActor childImageActor = FindImageActor( root.GetChildAt( i ) );
      if( childImageActor )
      {
        return childImageActor;
      }
    }
  }

  return imageActor;
}


} // unnamed namespace

Dali::Toolkit::PushButton PushButton::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr< PushButton > internalPushButton = new PushButton();

  // Pass ownership to CustomActor
  Dali::Toolkit::PushButton pushButton( *internalPushButton );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalPushButton->Initialize();

  return pushButton;
}

void PushButton::SetAutoRepeating( bool autoRepeating )
{
  mAutoRepeating = autoRepeating;

  // An autorepeating button can't be a toggle button.
  if( autoRepeating )
  {
    mToggleButton = false;
    if( mToggled )
    {
      // Emit a signal is not wanted, only change the appearance.
      Toolkit::PushButton handle( GetOwner() );
      GetPushButtonPainter( mPainter )->Toggled( handle );
      mToggled = false;
    }
  }

  // Notifies the painter.
  GetPushButtonPainter( mPainter )->SetAutoRepeating( mAutoRepeating );
}

bool PushButton::IsAutoRepeating() const
{
  return mAutoRepeating;
}

void PushButton::SetInitialAutoRepeatingDelay( float initialAutoRepeatingDelay )
{
  DALI_ASSERT_ALWAYS( initialAutoRepeatingDelay > 0.f );
  mInitialAutoRepeatingDelay = initialAutoRepeatingDelay;
}

float PushButton::GetInitialAutoRepeatingDelay() const
{
  return mInitialAutoRepeatingDelay;
}

void PushButton::SetNextAutoRepeatingDelay( float nextAutoRepeatingDelay )
{
  DALI_ASSERT_ALWAYS( nextAutoRepeatingDelay > 0.f );
  mNextAutoRepeatingDelay = nextAutoRepeatingDelay;
}

float PushButton::GetNextAutoRepeatingDelay() const
{
  return mNextAutoRepeatingDelay;
}

void PushButton::SetToggleButton( bool toggle )
{
  mToggleButton = toggle;

  // A toggle button can't be an autorepeating button.
  if( toggle )
  {
    mAutoRepeating = false;

    // Notifies the painter.
    GetPushButtonPainter( mPainter )->SetAutoRepeating( mAutoRepeating );
  }
}

bool PushButton::IsToggleButton() const
{
  return mToggleButton;
}

void PushButton::SetToggled( bool toggle )
{
  if( !mDisabled && mToggleButton && ( toggle != mToggled ) )
  {
    mToggled = toggle;

    Toolkit::PushButton handle( GetOwner() );

    // Notifies the painter the button has been toggled.
    GetPushButtonPainter( mPainter )->Toggled( handle );

    // Emit signal.
    mStateChangedSignal.Emit( handle, mToggled );
  }
}

bool PushButton::IsToggled() const
{
  return mToggleButton && mToggled;
}

void PushButton::SetButtonImage( Image image )
{
  SetButtonImage( ImageActor::New( image ) );
}

void PushButton::SetButtonImage( Actor image )
{
  Toolkit::PushButton handle( GetOwner() );
  GetPushButtonPainter( mPainter )->SetButtonImage( handle, image );
}

Actor& PushButton::GetButtonImage()
{
  return mButtonImage;
}

Actor PushButton::GetButtonImage() const
{
  return mButtonImage;
}

void PushButton::SetBackgroundImage( Image image )
{
  SetBackgroundImage( ImageActor::New( image ) );
}

void PushButton::SetBackgroundImage( Actor image )
{
  Toolkit::PushButton handle( GetOwner() );
  GetPushButtonPainter( mPainter )->SetBackgroundImage( handle, image );
}

Actor& PushButton::GetBackgroundImage()
{
  return mBackgroundImage;
}

Actor PushButton::GetBackgroundImage() const
{
  return mBackgroundImage;
}

void PushButton::SetSelectedImage( Image image )
{
  SetSelectedImage( ImageActor::New( image ) );
}

void PushButton::SetSelectedImage( Actor image )
{
  Toolkit::PushButton handle( GetOwner() );
  GetPushButtonPainter( mPainter )->SetSelectedImage( handle, image );
}

Actor& PushButton::GetSelectedImage()
{
  return mSelectedImage;
}

Actor PushButton::GetSelectedImage() const
{
  return mSelectedImage;
}

void PushButton::SetDisabledBackgroundImage( Image image )
{
  SetDisabledBackgroundImage( ImageActor::New( image ) );
}

void PushButton::SetDisabledBackgroundImage( Actor image )
{
  Toolkit::PushButton handle( GetOwner() );
  GetPushButtonPainter( mPainter )->SetDisabledBackgroundImage( handle, image );
}

Actor& PushButton::GetDisabledBackgroundImage()
{
  return mDisabledBackgroundImage;
}

Actor PushButton::GetDisabledBackgroundImage() const
{
  return mDisabledBackgroundImage;
}

void PushButton::SetDisabledImage( Image image )
{
  SetDisabledImage( ImageActor::New( image ) );
}

void PushButton::SetDisabledImage( Actor image )
{
  Toolkit::PushButton handle( GetOwner() );
  GetPushButtonPainter( mPainter )->SetDisabledImage( handle, image );
}

Actor& PushButton::GetDisabledImage()
{
  return mDisabledImage;
}

Actor PushButton::GetDisabledImage() const
{
  return mDisabledImage;
}

void PushButton::SetLabel( const std::string& label )
{
  // TODO
}

void PushButton::SetLabel( Actor label )
{
  Toolkit::PushButton handle( GetOwner() );
  GetPushButtonPainter( mPainter )->SetLabel( handle, label );
}

Actor PushButton::GetLabel() const
{
  return mLabel;
}

Actor& PushButton::GetLabel()
{
  return mLabel;
}

Actor& PushButton::GetFadeOutBackgroundImage()
{
  return mFadeOutBackgroundImage;
}

Actor& PushButton::GetFadeOutButtonImage()
{
  return mFadeOutButtonImage;
}

Toolkit::PushButton::PressedSignalType& PushButton::PressedSignal()
{
  return mPressedSignal;
}

Toolkit::PushButton::ReleasedSignalType& PushButton::ReleasedSignal()
{
  return mReleasedSignal;
}

bool PushButton::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::PushButton button = Toolkit::PushButton::DownCast(handle);

  if( Toolkit::PushButton::SIGNAL_STATE_CHANGED == signalName )
  {
    button.StateChangedSignal().Connect( tracker, functor );
  }
  else if( Toolkit::PushButton::SIGNAL_PRESSED == signalName )
  {
    button.PressedSignal().Connect( tracker, functor );
  }
  else if( Toolkit::PushButton::SIGNAL_RELEASED == signalName )
  {
    button.ReleasedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void PushButton::SetProperty( BaseObject* object, Property::Index propertyIndex, const Property::Value& value )
{
  Toolkit::PushButton pushButton = Toolkit::PushButton::DownCast( Dali::BaseHandle( object ) );

  if ( pushButton )
  {
    PushButton& pushButtonImpl( GetImplementation( pushButton ) );

    if ( propertyIndex == Toolkit::Button::PROPERTY_AUTO_REPEATING )
    {
      pushButtonImpl.SetAutoRepeating( value.Get< bool >() );
    }
    else if ( propertyIndex == Toolkit::Button::PROPERTY_INITIAL_AUTO_REPEATING_DELAY )
    {
      pushButtonImpl.SetInitialAutoRepeatingDelay( value.Get< float >() );
    }
    else if ( propertyIndex == Toolkit::Button::PROPERTY_NEXT_AUTO_REPEATING_DELAY )
    {
      pushButtonImpl.SetNextAutoRepeatingDelay( value.Get< float >() );
    }
    else if ( propertyIndex == Toolkit::Button::PROPERTY_TOGGLABLE )
    {
      pushButtonImpl.SetToggleButton( value.Get< bool >() );
    }
    else if ( propertyIndex == Toolkit::Button::PROPERTY_TOGGLED )
    {
      pushButtonImpl.SetToggled( value.Get< bool >() );
    }
    else if ( propertyIndex == Toolkit::Button::PROPERTY_NORMAL_STATE_ACTOR )
    {
      pushButtonImpl.SetButtonImage( Scripting::NewActor( value.Get< Property::Map >() ) );
    }
    else if ( propertyIndex == Toolkit::Button::PROPERTY_SELECTED_STATE_ACTOR )
    {
      pushButtonImpl.SetSelectedImage( Scripting::NewActor( value.Get< Property::Map >() ) );
    }
    else if ( propertyIndex == Toolkit::Button::PROPERTY_DISABLED_STATE_ACTOR )
    {
      pushButtonImpl.SetDisabledImage( Scripting::NewActor( value.Get< Property::Map >() ) );
    }
    else if ( propertyIndex == Toolkit::Button::PROPERTY_LABEL_ACTOR )
    {
      pushButtonImpl.SetLabel( Scripting::NewActor( value.Get< Property::Map >() ) );
    }
  }
}

Property::Value PushButton::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Toolkit::PushButton pushButton = Toolkit::PushButton::DownCast( Dali::BaseHandle( object ) );

  if ( pushButton )
  {
    PushButton& pushButtonImpl( GetImplementation( pushButton ) );

    if ( propertyIndex == Toolkit::Button::PROPERTY_AUTO_REPEATING )
    {
      value = pushButtonImpl.mAutoRepeating;
    }
    else if ( propertyIndex == Toolkit::Button::PROPERTY_INITIAL_AUTO_REPEATING_DELAY )
    {
      value = pushButtonImpl.mInitialAutoRepeatingDelay;
    }
    else if ( propertyIndex == Toolkit::Button::PROPERTY_NEXT_AUTO_REPEATING_DELAY )
    {
      value = pushButtonImpl.mNextAutoRepeatingDelay;
    }
    else if ( propertyIndex == Toolkit::Button::PROPERTY_TOGGLABLE )
    {
      value = pushButtonImpl.mToggleButton;
    }
    else if ( propertyIndex == Toolkit::Button::PROPERTY_TOGGLED )
    {
      value = pushButtonImpl.mToggled;
    }
    else if ( propertyIndex == Toolkit::Button::PROPERTY_NORMAL_STATE_ACTOR )
    {
      Property::Map map;
      Scripting::CreatePropertyMap( pushButtonImpl.mButtonImage, map );
      value = map;
    }
    else if ( propertyIndex == Toolkit::Button::PROPERTY_SELECTED_STATE_ACTOR )
    {
      Property::Map map;
      Scripting::CreatePropertyMap( pushButtonImpl.mSelectedImage, map );
      value = map;
    }
    else if ( propertyIndex == Toolkit::Button::PROPERTY_DISABLED_STATE_ACTOR )
    {
      Property::Map map;
      Scripting::CreatePropertyMap( pushButtonImpl.mDisabledImage, map );
      value = map;
    }
    else if ( propertyIndex == Toolkit::Button::PROPERTY_LABEL_ACTOR )
    {
      Property::Map map;
      Scripting::CreatePropertyMap( pushButtonImpl.mLabel, map );
      value = map;
    }
  }

  return value;
}

void PushButton::OnButtonInitialize()
{
  // Push button requires the Leave event.
  Actor root = Self();
  root.SetLeaveRequired( true );
}

void PushButton::OnButtonDown()
{
  if( !mToggleButton )
  {
    Toolkit::PushButton handle( GetOwner() );

    // Notifies the painter the button has been pressed.
    GetPushButtonPainter( mPainter )->Pressed( handle );

    if( mAutoRepeating )
    {
      SetUpTimer( mInitialAutoRepeatingDelay );
    }

    //Emit signal.
    mPressedSignal.Emit( handle );
  }
}

void PushButton::OnButtonUp()
{
  if( ButtonDown == mState )
  {
    if( mToggleButton )
    {
      mToggled = !mToggled;

      Toolkit::PushButton handle( GetOwner() );

      // Notifies the painter the button has been toggled.
      GetPushButtonPainter( mPainter )->Toggled( handle );

      // Emit signal.
      mStateChangedSignal.Emit( handle, mToggled );
    }
    else
    {
      Toolkit::PushButton handle( GetOwner() );

      // Notifies the painter the button has been clicked.
      GetPushButtonPainter( mPainter )->Released( handle );
      GetPushButtonPainter( mPainter )->Clicked( handle );

      if( mAutoRepeating )
      {
        mAutoRepeatingTimer.Reset();
      }

      //Emit signal.
      mReleasedSignal.Emit( handle );
      mClickedSignal.Emit( handle );
    }
  }
}

void PushButton::OnTouchPointLeave()
{
  if( ButtonDown == mState )
  {
    if( !mToggleButton )
    {
      Toolkit::PushButton handle( GetOwner() );

      // Notifies the painter the button has been released.
      GetPushButtonPainter( mPainter )->Released( handle );

      if( mAutoRepeating )
      {
        mAutoRepeatingTimer.Reset();
      }

      //Emit signal.
      mReleasedSignal.Emit( handle );
    }
  }
}

void PushButton::OnTouchPointInterrupted()
{
  OnTouchPointLeave();
}

void PushButton::OnAnimationTimeSet( float animationTime )
{
  GetPushButtonPainter( mPainter )->SetAnimationTime( animationTime );
}

float PushButton::OnAnimationTimeRequested() const
{
  return GetPushButtonPainter( mPainter )->GetAnimationTime();
}

void PushButton::OnButtonStageDisconnection()
{
  if( ButtonDown == mState )
  {
    if( !mToggleButton )
    {
      Toolkit::PushButton handle( GetOwner() );

      // Notifies the painter the button has been released.
      GetPushButtonPainter( mPainter )->Released( handle );

      if( mAutoRepeating )
      {
        mAutoRepeatingTimer.Reset();
      }
    }
  }
}

PushButton::PushButton()
: Button(),
  mAutoRepeating( false ),
  mInitialAutoRepeatingDelay( INITIAL_AUTOREPEATING_DELAY ),
  mNextAutoRepeatingDelay( NEXT_AUTOREPEATING_DELAY ),
  mToggleButton( false ),
  mAutoRepeatingTimer(),
  mToggled( false ),
  mClickActionPerforming(false)
{
  // Creates specific painter.
  mPainter = PushButtonDefaultPainterPtr( new PushButtonDefaultPainter() );
}

PushButton::~PushButton()
{
  if( mAutoRepeatingTimer )
  {
    mAutoRepeatingTimer.Reset();
  }

  mPainter = NULL;
}

void PushButton::SetUpTimer( float delay )
{
  mAutoRepeatingTimer = Dali::Timer::New( static_cast<unsigned int>( 1000.f * delay ) );
  mAutoRepeatingTimer.TickSignal().Connect( this, &PushButton::AutoRepeatingSlot );
  mAutoRepeatingTimer.Start();
}

bool PushButton::AutoRepeatingSlot()
{
  bool consumed = false;
  if( !mDisabled )
  {
    // Restart the autorepeat timer.
    SetUpTimer( mNextAutoRepeatingDelay );

    Toolkit::PushButton handle( GetOwner() );

    // Notifies the painter the button has been pressed.
    GetPushButtonPainter( mPainter )->Pressed( handle );

    //Emit signal.
    consumed = mReleasedSignal.Emit( handle );
    consumed |= mClickedSignal.Emit( handle );
    consumed |= mPressedSignal.Emit( handle );
 }

  return consumed;
}

void PushButton::OnActivated()
{
  // When the button is activated, it performs the click action
  PropertyValueContainer attributes;
  DoClickAction(attributes);
}

Vector3 PushButton::GetNaturalSize()
{
  Vector3 size = Control::GetNaturalSize();

  const bool widthIsZero = EqualsZero( size.width );
  const bool heightIsZero = EqualsZero( size.height );

  if( widthIsZero || heightIsZero )
  {
    // If background and background not scale9 try get size from that
    ImageActor imageActor = FindImageActor( mButtonImage );
    if( imageActor && imageActor.GetStyle() != ImageActor::STYLE_NINE_PATCH )
    {
      Vector3 imageSize = RelayoutHelper::GetNaturalSize( imageActor );

      if( widthIsZero )
      {
        size.width = imageSize.width;
      }

      if( heightIsZero )
      {
        size.height = imageSize.height;
      }
    }

    ImageActor backgroundImageActor = FindImageActor( mBackgroundImage );
    if( backgroundImageActor && backgroundImageActor.GetStyle() != ImageActor::STYLE_NINE_PATCH )
    {
      Vector3 imageSize = RelayoutHelper::GetNaturalSize( backgroundImageActor );

      if( widthIsZero )
      {
        size.width = std::max( size.width, imageSize.width );
      }

      if( heightIsZero )
      {
        size.height = std::max( size.height, imageSize.height );
      }
    }
  }

  return size;
}

void PushButton::DoClickAction(const PropertyValueContainer& attributes)
{
  // Prevents the button signals from doing a recursive loop by sending an action
  // and re-emitting the signals.
  if(!mClickActionPerforming)
  {
    mClickActionPerforming = true;
    OnButtonDown();
    mState = ButtonDown;
    OnButtonUp();
    mClickActionPerforming = false;
  }
}

bool PushButton::DoAction(BaseObject* object, const std::string& actionName, const PropertyValueContainer& attributes)
{
  bool ret = false;

  Dali::BaseHandle handle(object);

  Toolkit::PushButton button = Toolkit::PushButton::DownCast(handle);

  DALI_ASSERT_ALWAYS(button);

  if(Toolkit::PushButton::ACTION_PUSH_BUTTON_CLICK == actionName)
  {
    GetImplementation(button).DoClickAction(attributes);
    ret = true;
  }

  return ret;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
