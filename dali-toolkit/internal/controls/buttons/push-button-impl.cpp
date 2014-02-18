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

// CLASS HEADER

#include "push-button-impl.h"

// EXTERNAL INCLUDES

#include <algorithm>

// INTERNAL INCLUDES

#include "push-button-default-painter-impl.h"

#include <dali-toolkit/public-api/controls/text-view/text-view.h>

namespace Dali
{

namespace Toolkit
{

const Property::Index PushButton::PROPERTY_AUTO_REPEATING               = Internal::Button::BUTTON_PROPERTY_END_INDEX + 1;
const Property::Index PushButton::PROPERTY_INITIAL_AUTO_REPEATING_DELAY = Internal::Button::BUTTON_PROPERTY_END_INDEX + 2;
const Property::Index PushButton::PROPERTY_NEXT_AUTO_REPEATING_DELAY    = Internal::Button::BUTTON_PROPERTY_END_INDEX + 3;
const Property::Index PushButton::PROPERTY_TOGGLABLE                    = Internal::Button::BUTTON_PROPERTY_END_INDEX + 4;
const Property::Index PushButton::PROPERTY_TOGGLE                       = Internal::Button::BUTTON_PROPERTY_END_INDEX + 5;
const Property::Index PushButton::PROPERTY_BUTTON_IMAGE                 = Internal::Button::BUTTON_PROPERTY_END_INDEX + 6;
const Property::Index PushButton::PROPERTY_PRESSED_IMAGE                = Internal::Button::BUTTON_PROPERTY_END_INDEX + 8;
const Property::Index PushButton::PROPERTY_DIMMED_IMAGE                 = Internal::Button::BUTTON_PROPERTY_END_INDEX + 9;
const Property::Index PushButton::PROPERTY_LABEL_TEXT                   = Internal::Button::BUTTON_PROPERTY_END_INDEX + 11;

namespace Internal
{

namespace
{

BaseHandle Create()
{
  return Toolkit::PushButton::New();
}

TypeRegistration typeRegistration( typeid(Toolkit::PushButton), typeid(Toolkit::Button), Create );

SignalConnectorType signalConnector1( typeRegistration, Toolkit::PushButton::SIGNAL_TOGGLED , &PushButton::DoConnectSignal );
SignalConnectorType signalConnector2( typeRegistration, Toolkit::PushButton::SIGNAL_PRESSED , &PushButton::DoConnectSignal );
SignalConnectorType signalConnector3( typeRegistration, Toolkit::PushButton::SIGNAL_RELEASED, &PushButton::DoConnectSignal );

TypeAction action1( typeRegistration, Toolkit::PushButton::ACTION_PUSH_BUTTON_CLICK, &PushButton::DoAction );

PropertyRegistration property1( typeRegistration, "auto-repeating",               Toolkit::PushButton::PROPERTY_AUTO_REPEATING,               Property::BOOLEAN, &PushButton::SetProperty, &PushButton::GetProperty );
PropertyRegistration property2( typeRegistration, "initial-auto-repeating-delay", Toolkit::PushButton::PROPERTY_INITIAL_AUTO_REPEATING_DELAY, Property::FLOAT,   &PushButton::SetProperty, &PushButton::GetProperty );
PropertyRegistration property3( typeRegistration, "next-auto-repeating-delay",    Toolkit::PushButton::PROPERTY_NEXT_AUTO_REPEATING_DELAY,    Property::FLOAT,   &PushButton::SetProperty, &PushButton::GetProperty );
PropertyRegistration property4( typeRegistration, "togglable",                    Toolkit::PushButton::PROPERTY_TOGGLABLE,                    Property::BOOLEAN, &PushButton::SetProperty, &PushButton::GetProperty );
PropertyRegistration property5( typeRegistration, "toggle",                       Toolkit::PushButton::PROPERTY_TOGGLE,                       Property::BOOLEAN, &PushButton::SetProperty, &PushButton::GetProperty );
PropertyRegistration property6( typeRegistration, "button-image",                 Toolkit::PushButton::PROPERTY_BUTTON_IMAGE,                 Property::STRING,  &PushButton::SetProperty, &PushButton::GetProperty );
PropertyRegistration property7( typeRegistration, "pressed-image",                Toolkit::PushButton::PROPERTY_PRESSED_IMAGE,                Property::STRING,  &PushButton::SetProperty, &PushButton::GetProperty );
PropertyRegistration property8( typeRegistration, "dimmed-image",                 Toolkit::PushButton::PROPERTY_DIMMED_IMAGE,                 Property::STRING,  &PushButton::SetProperty, &PushButton::GetProperty );
PropertyRegistration property9( typeRegistration, "label-text",                   Toolkit::PushButton::PROPERTY_LABEL_TEXT,                   Property::STRING,  &PushButton::SetProperty, &PushButton::GetProperty );

} // unnamed namespace

namespace
{

const unsigned int INITIAL_AUTOREPEATING_DELAY( 0.15f );
const unsigned int NEXT_AUTOREPEATING_DELAY( 0.05f );

// Helper function used to cast a ButtonPainter to PushButtonDefaultPainter
PushButtonDefaultPainterPtr GetPushButtonPainter( Dali::Toolkit::Internal::ButtonPainterPtr painter )
{
  return static_cast<PushButtonDefaultPainter*>( painter.Get() );
}

/**
 * Helper function to checks if the specified actor is an ImageActor and if it has an Image with a path.
 *
 * @param[in]  actor  Actor handle to check.
 * @param[out] path   The image path will be applied to this parameter, if available.
 *                    If not available then this will be an empty string.
 */
void GetImageActorFilename( Actor& actor, std::string& path )
{
  path = ""; // Just return an empty string if not using ImageActor with an image

  if ( actor )
  {
    ImageActor imageActor = ImageActor::DownCast( actor );
    if ( imageActor )
    {
      Image image = imageActor.GetImage();
      if ( image )
      {
        path = image.GetFilename();
      }
    }
  }
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
  Toolkit::TextView textView ( Toolkit::TextView::New( text ) );
  textView.SetWidthExceedPolicy( Toolkit::TextView::ShrinkToFit ); // Make sure our text always fits inside the button
  SetLabelText( textView );
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

Toolkit::PushButton::ToggledSignalV2& PushButton::ToggledSignal()
{
  return mToggledSignalV2;
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

      case Toolkit::PushButton::PROPERTY_BUTTON_IMAGE:
      {
        Image image = Image::New( value.Get<std::string>() );
        pushButtonImpl.SetButtonImage( image );
        break;
      }

      case Toolkit::PushButton::PROPERTY_PRESSED_IMAGE:
      {
        Image image = Image::New( value.Get<std::string>() );
        pushButtonImpl.SetPressedImage( image );
        break;
      }

      case Toolkit::PushButton::PROPERTY_DIMMED_IMAGE:
      {
        Image image = Image::New( value.Get<std::string>() );
        pushButtonImpl.SetDimmedImage( image );
        break;
      }

      case Toolkit::PushButton::PROPERTY_LABEL_TEXT:
      {
        pushButtonImpl.SetLabelText( value.Get< std::string >() );
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

      case Toolkit::PushButton::PROPERTY_BUTTON_IMAGE:
      {
        std::string path;
        GetImageActorFilename( pushButtonImpl.mButtonImage, path );
        value = path;
        break;
      }

      case Toolkit::PushButton::PROPERTY_PRESSED_IMAGE:
      {
        std::string path;
        GetImageActorFilename( pushButtonImpl.mPressedImage, path );
        value = path;
        break;
      }

      case Toolkit::PushButton::PROPERTY_DIMMED_IMAGE:
      {
        std::string path;
        GetImageActorFilename( pushButtonImpl.mDimmedImage, path );
        value = path;
        break;
      }

      case Toolkit::PushButton::PROPERTY_LABEL_TEXT:
      {
        value = ""; // Just return an empty string if not using a TextView

        if ( pushButtonImpl.mLabel )
        {
          Toolkit::TextView textView = Toolkit::TextView::DownCast( pushButtonImpl.mLabel );
          if ( textView )
          {
            value = textView.GetText();
          }
        }
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

      //Emit signal.
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
  std::vector<Property::Value> attributes;
  DoClickAction(attributes);
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

bool PushButton::DoAction(BaseObject* object, const std::string& actionName, const std::vector<Property::Value>& attributes)
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
