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

const Property::Index PushButton::PROPERTY_AUTO_REPEATING               = Internal::Button::BUTTON_PROPERTY_END_INDEX + 1;
const Property::Index PushButton::PROPERTY_INITIAL_AUTO_REPEATING_DELAY = Internal::Button::BUTTON_PROPERTY_END_INDEX + 2;
const Property::Index PushButton::PROPERTY_NEXT_AUTO_REPEATING_DELAY    = Internal::Button::BUTTON_PROPERTY_END_INDEX + 3;
const Property::Index PushButton::PROPERTY_TOGGLABLE                    = Internal::Button::BUTTON_PROPERTY_END_INDEX + 4;
const Property::Index PushButton::PROPERTY_TOGGLE                       = Internal::Button::BUTTON_PROPERTY_END_INDEX + 5;
const Property::Index PushButton::PROPERTY_NORMAL_STATE_ACTOR           = Internal::Button::BUTTON_PROPERTY_END_INDEX + 6;
const Property::Index PushButton::PROPERTY_PRESSED_STATE_ACTOR          = Internal::Button::BUTTON_PROPERTY_END_INDEX + 7;
const Property::Index PushButton::PROPERTY_DIMMED_STATE_ACTOR           = Internal::Button::BUTTON_PROPERTY_END_INDEX + 8;
const Property::Index PushButton::PROPERTY_LABEL_ACTOR                  = Internal::Button::BUTTON_PROPERTY_END_INDEX + 9;

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

PropertyRegistration property1( typeRegistration, "auto-repeating",               Toolkit::PushButton::PROPERTY_AUTO_REPEATING,               Property::BOOLEAN, &PushButton::SetProperty, &PushButton::GetProperty );
PropertyRegistration property2( typeRegistration, "initial-auto-repeating-delay", Toolkit::PushButton::PROPERTY_INITIAL_AUTO_REPEATING_DELAY, Property::FLOAT,   &PushButton::SetProperty, &PushButton::GetProperty );
PropertyRegistration property3( typeRegistration, "next-auto-repeating-delay",    Toolkit::PushButton::PROPERTY_NEXT_AUTO_REPEATING_DELAY,    Property::FLOAT,   &PushButton::SetProperty, &PushButton::GetProperty );
PropertyRegistration property4( typeRegistration, "togglable",                    Toolkit::PushButton::PROPERTY_TOGGLABLE,                    Property::BOOLEAN, &PushButton::SetProperty, &PushButton::GetProperty );
PropertyRegistration property5( typeRegistration, "toggle",                       Toolkit::PushButton::PROPERTY_TOGGLE,                       Property::BOOLEAN, &PushButton::SetProperty, &PushButton::GetProperty );
PropertyRegistration property6( typeRegistration, "normal-state-actor",           Toolkit::PushButton::PROPERTY_NORMAL_STATE_ACTOR,           Property::MAP,     &PushButton::SetProperty, &PushButton::GetProperty );
PropertyRegistration property7( typeRegistration, "pressed-state-actor",          Toolkit::PushButton::PROPERTY_PRESSED_STATE_ACTOR,          Property::MAP,     &PushButton::SetProperty, &PushButton::GetProperty );
PropertyRegistration property8( typeRegistration, "dimmed-state-actor",           Toolkit::PushButton::PROPERTY_DIMMED_STATE_ACTOR,           Property::MAP,     &PushButton::SetProperty, &PushButton::GetProperty );
PropertyRegistration property9( typeRegistration, "label-actor",                  Toolkit::PushButton::PROPERTY_LABEL_ACTOR,                  Property::MAP,     &PushButton::SetProperty, &PushButton::GetProperty );

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
  if( !mDimmed && mToggleButton && ( toggle != mToggled ) )
  {
    mToggled = toggle;

    Toolkit::PushButton handle( GetOwner() );

    // Notifies the painter the button has been toggled.
    GetPushButtonPainter( mPainter )->Toggled( handle );

    // Emit signal.
    mToggledSignalV2.Emit( handle, mToggled );
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

void PushButton::SetPressedImage( Image image )
{
  SetPressedImage( ImageActor::New( image ) );
}

void PushButton::SetPressedImage( Actor image )
{
  Toolkit::PushButton handle( GetOwner() );
  GetPushButtonPainter( mPainter )->SetPressedImage( handle, image );
}

Actor& PushButton::GetPressedImage()
{
  return mPressedImage;
}

Actor PushButton::GetPressedImage() const
{
  return mPressedImage;
}

void PushButton::SetDimmedBackgroundImage( Image image )
{
  SetDimmedBackgroundImage( ImageActor::New( image ) );
}

void PushButton::SetDimmedBackgroundImage( Actor image )
{
  Toolkit::PushButton handle( GetOwner() );
  GetPushButtonPainter( mPainter )->SetDimmedBackgroundImage( handle, image );
}

Actor& PushButton::GetDimmedBackgroundImage()
{
  return mDimmedBackgroundImage;
}

Actor PushButton::GetDimmedBackgroundImage() const
{
  return mDimmedBackgroundImage;
}

void PushButton::SetDimmedImage( Image image )
{
  SetDimmedImage( ImageActor::New( image ) );
}

void PushButton::SetDimmedImage( Actor image )
{
  Toolkit::PushButton handle( GetOwner() );
  GetPushButtonPainter( mPainter )->SetDimmedImage( handle, image );
}

Actor& PushButton::GetDimmedImage()
{
  return mDimmedImage;
}

Actor PushButton::GetDimmedImage() const
{
  return mDimmedImage;
}

void PushButton::SetLabelText( const std::string& text )
{
  // TODO
}

void PushButton::SetLabelText( Actor text )
{
  Toolkit::PushButton handle( GetOwner() );
  GetPushButtonPainter( mPainter )->SetLabelText( handle, text );
}

Actor& PushButton::GetLabel()
{
  return mLabel;
}

Actor PushButton::GetLabelText() const
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

Toolkit::PushButton::PressedSignalV2& PushButton::PressedSignal()
{
  return mPressedSignalV2;
}

Toolkit::PushButton::ReleasedSignalV2& PushButton::ReleasedSignal()
{
  return mReleasedSignalV2;
}

bool PushButton::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::PushButton button = Toolkit::PushButton::DownCast(handle);

  if( Toolkit::PushButton::SIGNAL_TOGGLED == signalName )
  {
    button.ToggledSignal().Connect( tracker, functor );
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

    switch ( propertyIndex )
    {
      case Toolkit::PushButton::PROPERTY_AUTO_REPEATING:
      {
        pushButtonImpl.SetAutoRepeating( value.Get< bool >() );
        break;
      }

      case Toolkit::PushButton::PROPERTY_INITIAL_AUTO_REPEATING_DELAY:
      {
        pushButtonImpl.SetInitialAutoRepeatingDelay( value.Get< float >() );
        break;
      }

      case Toolkit::PushButton::PROPERTY_NEXT_AUTO_REPEATING_DELAY:
      {
        pushButtonImpl.SetNextAutoRepeatingDelay( value.Get< float >() );
        break;
      }

      case Toolkit::PushButton::PROPERTY_TOGGLABLE:
      {
        pushButtonImpl.SetToggleButton( value.Get< bool >() );
        break;
      }

      case Toolkit::PushButton::PROPERTY_TOGGLE:
      {
        pushButtonImpl.SetToggled( value.Get< bool >() );
        break;
      }

      case Toolkit::PushButton::PROPERTY_NORMAL_STATE_ACTOR:
      {
        pushButtonImpl.SetButtonImage( Scripting::NewActor( value.Get< Property::Map >() ) );
        break;
      }

      case Toolkit::PushButton::PROPERTY_PRESSED_STATE_ACTOR:
      {
        pushButtonImpl.SetPressedImage( Scripting::NewActor( value.Get< Property::Map >() ) );
        break;
      }

      case Toolkit::PushButton::PROPERTY_DIMMED_STATE_ACTOR:
      {
        pushButtonImpl.SetDimmedImage( Scripting::NewActor( value.Get< Property::Map >() ) );
        break;
      }

      case Toolkit::PushButton::PROPERTY_LABEL_ACTOR:
      {
        pushButtonImpl.SetLabelText( Scripting::NewActor( value.Get< Property::Map >() ) );
        break;
      }
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

    switch ( propertyIndex )
    {
      case Toolkit::PushButton::PROPERTY_AUTO_REPEATING:
      {
        value = pushButtonImpl.mAutoRepeating;
        break;
      }

      case Toolkit::PushButton::PROPERTY_INITIAL_AUTO_REPEATING_DELAY:
      {
        value = pushButtonImpl.mInitialAutoRepeatingDelay;
        break;
      }

      case Toolkit::PushButton::PROPERTY_NEXT_AUTO_REPEATING_DELAY:
      {
        value = pushButtonImpl.mNextAutoRepeatingDelay;
        break;
      }

      case Toolkit::PushButton::PROPERTY_TOGGLABLE:
      {
        value = pushButtonImpl.mToggleButton;
        break;
      }

      case Toolkit::PushButton::PROPERTY_TOGGLE:
      {
        value = pushButtonImpl.mToggled;
        break;
      }

      case Toolkit::PushButton::PROPERTY_NORMAL_STATE_ACTOR:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( pushButtonImpl.mButtonImage, map );
        value = map;
        break;
      }

      case Toolkit::PushButton::PROPERTY_PRESSED_STATE_ACTOR:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( pushButtonImpl.mPressedImage, map );
        value = map;
        break;
      }

      case Toolkit::PushButton::PROPERTY_DIMMED_STATE_ACTOR:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( pushButtonImpl.mDimmedImage, map );
        value = map;
        break;
      }

      case Toolkit::PushButton::PROPERTY_LABEL_ACTOR:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( pushButtonImpl.mLabel, map );
        value = map;
        break;
      }
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
    mPressedSignalV2.Emit( handle );
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
      mToggledSignalV2.Emit( handle, mToggled );
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
      mReleasedSignalV2.Emit( handle );
      mClickedSignalV2.Emit( handle );
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
      mReleasedSignalV2.Emit( handle );
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
  if( !mDimmed )
  {
    // Restart the autorepeat timer.
    SetUpTimer( mNextAutoRepeatingDelay );

    Toolkit::PushButton handle( GetOwner() );

    // Notifies the painter the button has been pressed.
    GetPushButtonPainter( mPainter )->Pressed( handle );

    //Emit signal.
    consumed = mReleasedSignalV2.Emit( handle );
    consumed |= mClickedSignalV2.Emit( handle );
    consumed |= mPressedSignalV2.Emit( handle );
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
