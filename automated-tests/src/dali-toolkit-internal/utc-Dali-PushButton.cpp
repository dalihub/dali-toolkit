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

#include <iostream>

#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-toolkit/internal/controls/buttons/button-impl.h>


using namespace Dali;
using namespace Toolkit;

namespace
{

static bool gPushButtonToggleState = false;
bool PushButtonToggled( Button button, bool toggled )
{
  gPushButtonToggleState = toggled && ( toggled == static_cast<PushButton&>( button ).IsToggled() );
  return true;
}


const Dali::TouchPoint pointDownInside( 0, TouchPoint::Down, 240, 400 );
const Dali::TouchPoint pointUpInside( 0, TouchPoint::Up, 240, 400 );
const Dali::TouchPoint pointLeave( 0, TouchPoint::Leave, 240, 400 );
const Dali::TouchPoint pointEnter( 0, TouchPoint::Motion, 240, 400 );
const Dali::TouchPoint pointMotionOut( 0, TouchPoint::Motion, 10, 10 );
const Dali::TouchPoint pointDownOutside( 0, TouchPoint::Down, 10, 10 );
const Dali::TouchPoint pointUpOutside( 0, TouchPoint::Up, 10, 10 );
} // namespace


//////////////////////////////////////////////////////////

namespace
{
static bool gOnTouchPointInterrupted = false;
} //namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
class TETButton;
}

/**
 * Creates a Button to test if interrupt events are handled correctly.
 */
class TETButton : public Button
{
public:
  // PushButton Pressed
  typedef SignalV2< bool ( Button ) > PressedSignalV2;

  PressedSignalV2& PressedSignal();

  /**
   * Default constructor.
   */
  TETButton();

  /**
   * Copy constructor.
   */
  TETButton( const Button& button );

  /**
   * Assignment operator.
   */
  TETButton& operator=( const TETButton& button );

  /**
   * Creates and initializes a new button.
   */
  static TETButton New();

  /**
   * Down cast to TETButton.
   */
  static TETButton DownCast( BaseHandle handle );

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  TETButton( Internal::TETButton& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  TETButton( Dali::Internal::CustomActor* internal );
};

namespace Internal
{

/**
 * Internal implementation
 */
class TETButton : public Button
{
public:
  /**
   * Construct a new Button.
   */
  TETButton();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TETButton();

  /**
   * Creates an internal button.
   */
  static Toolkit::TETButton New();

  /**
   * @return the pressed signal.
   */
  Toolkit::TETButton::PressedSignalV2& PressedSignal();

  /**
   * Callback called when an interrupt events is received.
   */
  void OnTouchPointInterrupted();

  /**
   * Callback received when a down event is received.
   */
  void OnButtonDown();

  Toolkit::TETButton::PressedSignalV2 mPressedSignal;   ///< Signal emitted when the button is pressed.
};

} // namespace Internal

TETButton::TETButton()
{
}

TETButton::TETButton( const Button& button )
: Button( button )
{
}

TETButton& TETButton::operator=( const TETButton& button )
{
  if( &button != this )
  {
    Button::operator=( button );
  }
  return *this;
}

TETButton TETButton::New()
{
  return Internal::TETButton::New();
}

TETButton TETButton::DownCast( BaseHandle handle )
{
  return Control::DownCast<TETButton, Internal::TETButton>(handle);
}

TETButton::PressedSignalV2& TETButton::PressedSignal()
{
  TETButton button( *this );
  DALI_ASSERT_ALWAYS( button );

  Dali::RefObject& handle = button.GetImplementation();

  return static_cast<Toolkit::Internal::TETButton&>( handle ).PressedSignal();
}

TETButton::TETButton( Internal::TETButton& implementation )
: Button( implementation )
{}

TETButton::TETButton( Dali::Internal::CustomActor* internal )
: Button( internal )
{
  VerifyCustomActorPointer<Internal::TETButton>(internal);
}

namespace Internal
{

TETButton::TETButton()
: Button(),
  mPressedSignal()
{
}

TETButton::~TETButton()
{
}

Toolkit::TETButton TETButton::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr< TETButton > internalTETButton = new TETButton();

  // Pass ownership to CustomActor
  Dali::Toolkit::TETButton tetButton( *internalTETButton );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalTETButton->Initialize();

  return tetButton;
}

Toolkit::TETButton::PressedSignalV2& TETButton::PressedSignal()
{
  return mPressedSignal;
}

void TETButton::OnButtonDown()
{
  Toolkit::TETButton handle( GetOwner() );

  //Emit signal.
  mPressedSignal.Emit( handle );
}

void TETButton::OnTouchPointInterrupted()
{
  gOnTouchPointInterrupted = true;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

namespace
{

class TETButtonPressed : public Dali::ConnectionTracker
{
public:
  enum Test
  {
    SENSITIVENESS,
    VISIBILITY
  };

  TETButtonPressed( Actor actor, Test test )
  : mActor( actor ),
    mTest( test )
  {
  }

  bool Callback( Button button )
  {
    switch( mTest )
    {
      case SENSITIVENESS:
      {
        mActor.SetSensitive( false );
        break;
      }
      case VISIBILITY:
      {
        std::cout <<"VISIBILITY false" << std::endl;
        mActor.SetVisible( false );
        break;
      }
      default:
      {
        break;
      }
    }
    return true;
  }

  Actor mActor;
  Test mTest;
};

static bool TestCallback(Actor actor, const TouchEvent& event)
{
  return true;
}

} // namespace

//////////////////////////////////////////////////////////


int UtcDaliPushButtonInterruptEventWhenInsensitive(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonInterruptEventWhenInsensitive");

  // * Creates an actor which contains a button.
  // * The size of the actor is bigger than the button.
  // * The button's boundary is contained in the actor's one.
  Actor actor = Actor::New();
  TETButton tetButton= Toolkit::TETButton::New();

  actor.SetName( "Actor" );
  tetButton.SetName( "TETButton" );

  actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  actor.SetPosition( 0, 0 );
  actor.SetSize( 400, 800 );

  tetButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  tetButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  tetButton.SetPosition( 240, 400 );
  tetButton.SetSize( 100, 100 );

  actor.Add( tetButton );
  Stage::GetCurrent().Add( actor );

  // * Actor's touch event is connected to a callback function
  //   and this callback function consumes the event.
  actor.TouchedSignal().Connect( &TestCallback );

  // * Button's pressed signal is connected to a callback function
  //   which also consumes the event.
  // * Changes the sensitiveness of the button to false.
  TETButtonPressed tetButtonPressed( actor, TETButtonPressed::SENSITIVENESS );
  tetButton.PressedSignal().Connect( &tetButtonPressed, &TETButtonPressed::Callback );

  // Initializes TET state.
  gOnTouchPointInterrupted = false;
  tetButton.SetSensitive( true );

  Dali::Integration::TouchEvent event;

  // TET starts.

  // Test a down point inside the button which is also consumed by the actor, and an up point
  // consumed only by the actor.  gOnTouchPointInterrupted should be true (Button receives an
  // interrupt event.

  application.SendNotification();
  application.Render();

  // A down event is sent inside the button's boundary.

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  application.ProcessEvent( event );

  // An up event is sent outside the button's boundary but inside the actor's one.

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gOnTouchPointInterrupted );

  // Test a down point inside the button which is also consumed by the actor, and a motion point
  // consumed only by the actor.  gOnTouchPointInterrupted should be true (Button receives an
  // interrupt event.

  // Initializes TET state.
  gOnTouchPointInterrupted = false;
  actor.SetSensitive( true );
  tetButton.SetSensitive( true );

  application.SendNotification();
  application.Render();

  // A down event is sent inside the button's boundary.

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  application.ProcessEvent( event );

  // A motion event is sent outside the button's boundary but inside the actor's one.

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointMotionOut );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gOnTouchPointInterrupted );

  // Test a down point inside the button which is also consumed by the actor, and an up point
  // also inside the button and consumed by the actor.  gOnTouchPointInterrupted should be false.

  // Initializes TET state.
  gOnTouchPointInterrupted = false;
  actor.SetSensitive( true );
  tetButton.SetSensitive( true );

  // A down event is sent inside the button's boundary.

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  application.ProcessEvent( event );

  actor.SetSensitive( true );
  // An up event is sent inside the button's boundary.

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gOnTouchPointInterrupted );
  END_TEST;
}

int UtcDaliPushButtonInterruptEventWhenNonVisible(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonInterruptEventWhenNonVisible");

  // Does same test as above but changing the visibility instead the sensitiveness.

  // * Creates an actor which contains a button.
  // * The size of the actor is bigger than the button.
  // * The button's boundary is contained in the actor's one.
  Actor actor = Actor::New();
  TETButton tetButton = Toolkit::TETButton::New();

  actor.SetName( "Actor" );
  tetButton.SetName( "TETButton" );

  actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  actor.SetPosition( 0, 0 );
  actor.SetSize( 400, 800 );

  tetButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  tetButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  tetButton.SetPosition( 240, 400 );
  tetButton.SetSize( 100, 100 );

  actor.Add( tetButton );
  Stage::GetCurrent().Add( actor );

  // * Actor's touch event is connected to a callback function
  //   and this callback function consumes the event.
  actor.TouchedSignal().Connect( &TestCallback );

  // * Button's pressed signal is connected to a callback function
  //   which also consumes the event.
  // * Changes the visibility of the button to false.
  TETButtonPressed tetButtonPressed( tetButton, TETButtonPressed::VISIBILITY );
  tetButton.PressedSignal().Connect( &tetButtonPressed, &TETButtonPressed::Callback );

  // Initializes TET state.
  gOnTouchPointInterrupted = false;
  tetButton.SetVisible( true );

  Dali::Integration::TouchEvent event;

  // TET starts.

  // Test a down point inside the button which is also consumed by the actor, and an up point
  // consumed only by the actor.  gOnTouchPointInterrupted should be true (Button receives an
  // interrupt event.

  application.SendNotification();
  application.Render();

  // A down event is sent inside the button's boundary.

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  application.ProcessEvent( event );

  // More renders are needed in order to allow the node of the actor to become invisible.
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // An up event is sent outside the button's boundary but inside the actor's one.

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gOnTouchPointInterrupted );

  // Test a down point inside the button which is also consumed by the actor, and a motion point
  // consumed only by the actor.  gOnTouchPointInterrupted should be true (Button receives an
  // interrupt event.

  // Initializes TET state.
  gOnTouchPointInterrupted = false;
  tetButton.SetVisible( true );

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // A down event is sent inside the button's boundary.

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  application.ProcessEvent( event );

  // More renders are needed in order to allow the node of the actor to become invisible.
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // A motion event is sent outside the button's boundary but inside the actor's one.

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointMotionOut );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gOnTouchPointInterrupted );

  // Test a down point inside the button which is also consumed by the actor, and an up point
  // also inside the button and consumed by the actor.  gOnTouchPointInterrupted should be false.

  // Initializes TET state.
  gOnTouchPointInterrupted = false;
  tetButton.SetVisible( true );

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // A down event is sent inside the button's boundary.

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  application.ProcessEvent( event );

  tetButton.SetVisible( true );

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // An up event is sent inside the button's boundary.

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gOnTouchPointInterrupted );
  END_TEST;
}

int UtcDaliPushButtonProperties(void)
{
  ToolkitTestApplication application;

  PushButton button = PushButton::New();
  Stage::GetCurrent().Add( button );

  // PushButton::PROPERTY_AUTO_REPEATING
  button.SetAutoRepeating( false );
  DALI_TEST_CHECK( ! button.GetProperty< bool >( PushButton::PROPERTY_AUTO_REPEATING ) );
  button.SetProperty( PushButton::PROPERTY_AUTO_REPEATING, true );
  DALI_TEST_CHECK( button.IsAutoRepeating() ) ;
  DALI_TEST_CHECK( button.GetProperty< bool >( PushButton::PROPERTY_AUTO_REPEATING ) );

  // PushButton::PROPERTY_INITIAL_AUTO_REPEATING_DELAY
  button.SetInitialAutoRepeatingDelay( 10.0f );
  DALI_TEST_EQUALS( 10.0f, button.GetProperty< float >( PushButton::PROPERTY_INITIAL_AUTO_REPEATING_DELAY ), TEST_LOCATION );
  button.SetProperty( PushButton::PROPERTY_INITIAL_AUTO_REPEATING_DELAY, 25.0f );
  DALI_TEST_EQUALS( 25.0f, button.GetInitialAutoRepeatingDelay(), TEST_LOCATION );
  DALI_TEST_EQUALS( 25.0f, button.GetProperty< float >( PushButton::PROPERTY_INITIAL_AUTO_REPEATING_DELAY ), TEST_LOCATION );

  //  PushButton::PROPERTY_NEXT_AUTO_REPEATING_DELAY
  button.SetNextAutoRepeatingDelay( 3.0f );
  DALI_TEST_EQUALS( 3.0f, button.GetProperty< float >( PushButton::PROPERTY_NEXT_AUTO_REPEATING_DELAY ), TEST_LOCATION );
  button.SetProperty( PushButton::PROPERTY_NEXT_AUTO_REPEATING_DELAY, 4.0f );
  DALI_TEST_EQUALS( 4.0f, button.GetNextAutoRepeatingDelay(), TEST_LOCATION );
  DALI_TEST_EQUALS( 4.0f, button.GetProperty< float >( PushButton::PROPERTY_NEXT_AUTO_REPEATING_DELAY ), TEST_LOCATION );

  //  PushButton::PROPERTY_TOGGLABLE
  button.SetToggleButton( false );
  DALI_TEST_CHECK( ! button.GetProperty< bool >( PushButton::PROPERTY_TOGGLABLE ) );
  button.SetProperty( PushButton::PROPERTY_TOGGLABLE, true );
  DALI_TEST_CHECK( button.IsToggleButton() ) ;
  DALI_TEST_CHECK( button.GetProperty< bool >( PushButton::PROPERTY_TOGGLABLE ) );

  //  PushButton::PROPERTY_TOGGLE
  button.SetToggled( false );
  DALI_TEST_CHECK( ! button.GetProperty< bool >( PushButton::PROPERTY_TOGGLE ) );
  button.SetProperty( PushButton::PROPERTY_TOGGLE, true );
  DALI_TEST_CHECK( button.IsToggled() ) ;
  DALI_TEST_CHECK( button.GetProperty< bool >( PushButton::PROPERTY_TOGGLE ) );

  //  PushButton::PROPERTY_NORMAL_STATE_ACTOR
  {
    button.SetButtonImage( Image::New( "IMAGE_PATH_1") );
    DALI_TEST_EQUALS( "IMAGE_PATH_1", button.GetProperty( PushButton::PROPERTY_NORMAL_STATE_ACTOR ).GetValue( "image" ).GetValue( "filename" ).Get< std::string >(), TEST_LOCATION );

    Property::Map map;
    map.push_back( Property::StringValuePair( "type", "ImageActor" ) );

    button.SetProperty( PushButton::PROPERTY_NORMAL_STATE_ACTOR, map );
    DALI_TEST_EQUALS( "ImageActor", button.GetProperty( PushButton::PROPERTY_NORMAL_STATE_ACTOR ).GetValue( "type" ).Get< std::string >(), TEST_LOCATION );
  }

  //  PushButton::PROPERTY_PRESSED_IMAGE
  {
    button.SetPressedImage( Image::New( "IMAGE_PATH_2") );
    DALI_TEST_EQUALS( "IMAGE_PATH_2", button.GetProperty( PushButton::PROPERTY_PRESSED_STATE_ACTOR ).GetValue( "image" ).GetValue( "filename" ).Get< std::string >(), TEST_LOCATION );

    Property::Map map;
    map.push_back( Property::StringValuePair( "type", "TextActor" ) );

    button.SetProperty( PushButton::PROPERTY_PRESSED_STATE_ACTOR, map );
    DALI_TEST_EQUALS( "TextActor", button.GetProperty( PushButton::PROPERTY_PRESSED_STATE_ACTOR ).GetValue( "type" ).Get< std::string >(), TEST_LOCATION );
  }

  //  PushButton::PROPERTY_DIMMED_STATE_ACTOR
  {
    button.SetDimmedImage( Image::New( "IMAGE_PATH_3") );
    DALI_TEST_EQUALS( "IMAGE_PATH_3", button.GetProperty( PushButton::PROPERTY_DIMMED_STATE_ACTOR ).GetValue( "image" ).GetValue( "filename" ).Get< std::string >(), TEST_LOCATION );

    Property::Map map;
    map.push_back( Property::StringValuePair( "type", "Actor" ) );

    button.SetProperty( PushButton::PROPERTY_DIMMED_STATE_ACTOR, map );
    DALI_TEST_EQUALS( "Actor", button.GetProperty( PushButton::PROPERTY_DIMMED_STATE_ACTOR ).GetValue( "type" ).Get< std::string >(), TEST_LOCATION );
  }

  //  PushButton::PROPERTY_LABEL_ACTOR
  {
    button.SetLabelText( "LABEL_TEXT_CUSTOM" );
    DALI_TEST_EQUALS( "TextView", button.GetProperty( PushButton::PROPERTY_LABEL_ACTOR ).GetValue( "type" ).Get< std::string >(), TEST_LOCATION );

    Property::Map map;
    map.push_back( Property::StringValuePair( "type", "Actor" ) );

    button.SetProperty( PushButton::PROPERTY_LABEL_ACTOR, map );
    DALI_TEST_EQUALS( "Actor", button.GetProperty( PushButton::PROPERTY_LABEL_ACTOR ).GetValue( "type" ).Get< std::string >(), TEST_LOCATION );
  }
  END_TEST;
}
