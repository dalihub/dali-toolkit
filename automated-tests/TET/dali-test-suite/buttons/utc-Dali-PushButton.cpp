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
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-toolkit/public-api/controls/buttons/push-button.h>
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>
#include <dali-toolkit/public-api/controls/text-view/text-view.h>

#include <dali-toolkit-test-suite-utils.h>

#include <dali-toolkit/internal/controls/buttons/button-impl.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
Image CreateSolidColorImage( const Vector4& color, unsigned int width, unsigned int height )
{
  BitmapImage imageData = BitmapImage::New( width, height, Pixel::RGBA8888 );

  // Create the image
  PixelBuffer* pixbuf = imageData.GetBuffer();
  unsigned int size = width * height;

  for( size_t i = 0; i < size; i++ )
    {
      pixbuf[i*4+0] = 0xFF * color.r;
      pixbuf[i*4+1] = 0xFF * color.g;
      pixbuf[i*4+2] = 0xFF * color.b;
      pixbuf[i*4+3] = 0xFF * color.a;
    }

  imageData.Update();

  return imageData;
}

static bool gPushButtonToggleState = false;
bool PushButtonToggled( Button button, bool toggled )
{
  gPushButtonToggleState = toggled && ( toggled == static_cast<PushButton&>( button ).IsToggled() );
  return true;
}

static bool gPushButtonPressed = false;

static bool PushButtonPressed( Button button )
{
  gPushButtonPressed = true;
  return true;
}

static bool gPushButtonReleased = false;

static bool PushButtonReleased( Button button )
{
  gPushButtonReleased = true;
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

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

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

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliPushButtonSetGetAutoRepeating, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPushButtonSetGetToggleButton, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPushButtonSetGetAutoRepeatingAndToggleButton, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPushButtonSetGetToggled01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPushButtonSetGetToggled02, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPushButtonSetGetAutorepeatingDelayValues01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPushButtonSetGetAutorepeatingDelayValues02, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPushButtonSetImages, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPushButtonSetLabelText, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPushButtonPressed, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPushButtonReleased, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPushButtonToggled, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPushButtonInterruptEventWhenInsensitive, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPushButtonInterruptEventWhenNonVisible, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPushButtonProperties, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}


static void UtcDaliPushButtonSetGetAutoRepeating()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetAutoRepeating");

  PushButton pushButton = PushButton::New();

  pushButton.SetAutoRepeating( true );

  DALI_TEST_CHECK( pushButton.IsAutoRepeating() );

  pushButton.SetAutoRepeating( false );

  DALI_TEST_CHECK( !pushButton.IsAutoRepeating() );

  pushButton.SetAutoRepeating( true );

  DALI_TEST_CHECK( pushButton.IsAutoRepeating() );
}

static void UtcDaliPushButtonSetGetToggleButton()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetToggleButton");

  PushButton pushButton = PushButton::New();

  pushButton.SetToggleButton( true );

  DALI_TEST_CHECK( pushButton.IsToggleButton() );

  pushButton.SetToggleButton( false );

  DALI_TEST_CHECK( !pushButton.IsToggleButton() );

  pushButton.SetToggleButton( true );

  DALI_TEST_CHECK( pushButton.IsToggleButton() );
}

static void UtcDaliPushButtonSetGetAutoRepeatingAndToggleButton()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetAutoRepeatingAndToggleButton");

  PushButton pushButton = PushButton::New();

  pushButton.SetAutoRepeating( true );
  pushButton.SetToggleButton( true );

  DALI_TEST_CHECK( pushButton.IsToggleButton() );
  DALI_TEST_CHECK( !pushButton.IsAutoRepeating() );

  pushButton.SetToggleButton( true );
  pushButton.SetAutoRepeating( true );

  DALI_TEST_CHECK( pushButton.IsAutoRepeating() );
  DALI_TEST_CHECK( !pushButton.IsToggleButton() );
}

static void UtcDaliPushButtonSetGetToggled01()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetToggled01");

  PushButton pushButton = PushButton::New();

  pushButton.SetToggleButton( true );
  pushButton.ToggledSignal().Connect( &PushButtonToggled );

  gPushButtonToggleState = false;
  pushButton.SetToggled( true );

  DALI_TEST_CHECK( pushButton.IsToggled() );
  DALI_TEST_CHECK( gPushButtonToggleState );

  pushButton.SetToggled( false );

  DALI_TEST_CHECK( !pushButton.IsToggled() );
  DALI_TEST_CHECK( !gPushButtonToggleState );

  pushButton.SetToggled( true );

  DALI_TEST_CHECK( pushButton.IsToggled() );
  DALI_TEST_CHECK( gPushButtonToggleState );
}

static void UtcDaliPushButtonSetGetToggled02()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetToggled02");

  PushButton pushButton = PushButton::New();

  pushButton.SetToggleButton( false );
  pushButton.ToggledSignal().Connect( &PushButtonToggled );

  gPushButtonToggleState = false;
  pushButton.SetToggled( true );

  DALI_TEST_CHECK( !pushButton.IsToggled() );
  DALI_TEST_CHECK( !gPushButtonToggleState );

  pushButton.SetToggled( false );

  DALI_TEST_CHECK( !pushButton.IsToggled() );
  DALI_TEST_CHECK( !gPushButtonToggleState );

  pushButton.SetToggled( true );

  DALI_TEST_CHECK( !pushButton.IsToggled() );
  DALI_TEST_CHECK( !gPushButtonToggleState );
}

static void UtcDaliPushButtonSetGetAutorepeatingDelayValues01()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetAutorepeatingDelayValues01");

  PushButton pushButton = PushButton::New();

  pushButton.SetAutoRepeating( true );

  pushButton.SetInitialAutoRepeatingDelay( 1.f );
  DALI_TEST_EQUALS( pushButton.GetInitialAutoRepeatingDelay(), 1.f, TEST_LOCATION );

  pushButton.SetNextAutoRepeatingDelay( 1.f );
  DALI_TEST_EQUALS( pushButton.GetNextAutoRepeatingDelay(), 1.f, TEST_LOCATION );
}

static void UtcDaliPushButtonSetGetAutorepeatingDelayValues02()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetAutorepeatingDelayValues02");

  PushButton pushButton = PushButton::New();

  bool assert1( false );
  bool assert2( false );

  pushButton.SetAutoRepeating( true );

  try
  {
    pushButton.SetInitialAutoRepeatingDelay( -1.f );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_EQUALS(e.mCondition, "initialAutoRepeatingDelay > 0.f", TEST_LOCATION);
    assert1 = true;
  }

  try
  {
    pushButton.SetNextAutoRepeatingDelay( -1.f );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_EQUALS(e.mCondition, "nextAutoRepeatingDelay > 0.f", TEST_LOCATION);
    assert2 = true;
  }

  DALI_TEST_CHECK( assert1 && assert2 );
}

static void UtcDaliPushButtonSetImages()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetImages");

  Actor imageActor;

  Image image01 = CreateSolidColorImage( Color::RED, 10, 10 );
  ImageActor imageActor01 = CreateSolidColorActor( Color::RED );
  imageActor01.SetSize( 20.f, 20.f );

  Image image02 = CreateSolidColorImage( Color::RED, 30, 30 );
  ImageActor imageActor02 = CreateSolidColorActor( Color::RED );
  imageActor02.SetSize( 40.f, 40.f );

  Image image03 = CreateSolidColorImage( Color::RED, 50, 50 );
  ImageActor imageActor03 = CreateSolidColorActor( Color::RED );
  imageActor03.SetSize( 60.f, 60.f );

  Image image04 = CreateSolidColorImage( Color::RED, 70, 70 );
  ImageActor imageActor04 = CreateSolidColorActor( Color::RED );
  imageActor04.SetSize( 80.f, 80.f );

  Image image05 = CreateSolidColorImage( Color::RED, 90, 90 );
  ImageActor imageActor05 = CreateSolidColorActor( Color::RED );
  imageActor05.SetSize( 100.f, 100.f );

  Vector3 size;
  PushButton pushButton = PushButton::New();

  application.SendNotification();
  application.Render();

  // Just check if check box button size changes when a bigger image is set.

  pushButton.SetButtonImage( image01 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetButtonImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 10.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 10.f, TEST_LOCATION );

  pushButton.SetButtonImage( imageActor01 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetButtonImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 20.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 20.f, TEST_LOCATION );

  pushButton.SetBackgroundImage( image02 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetBackgroundImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 30.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 30.f, TEST_LOCATION );

  pushButton.SetBackgroundImage( imageActor02 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetBackgroundImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 40.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 40.f, TEST_LOCATION );

  pushButton.SetPressedImage( image03 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetPressedImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 50.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 50.f, TEST_LOCATION );

  pushButton.SetPressedImage( imageActor03 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetPressedImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 60.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 60.f, TEST_LOCATION );

  pushButton.SetDimmedBackgroundImage( image04 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetDimmedBackgroundImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 70.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 70.f, TEST_LOCATION );

  pushButton.SetDimmedBackgroundImage( imageActor04 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetDimmedBackgroundImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 80.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 80.f, TEST_LOCATION );

  pushButton.SetDimmedImage( image05 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetDimmedImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 90.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 90.f, TEST_LOCATION );

  pushButton.SetDimmedImage( imageActor05 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetDimmedImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 100.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 100.f, TEST_LOCATION );
}

static void UtcDaliPushButtonSetLabelText()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetLabelText");

  const std::string STR( "Hola!" );

  PushButton pushButton = PushButton::New();

  application.SendNotification();
  application.Render();

  TextView textView;

  pushButton.SetLabelText( STR );

  textView = TextView::DownCast( pushButton.GetLabelText() );
  DALI_TEST_CHECK( STR == textView.GetText() );

  TextView text = TextView::New( STR );
  pushButton.SetLabelText( text );

  textView = TextView::DownCast( pushButton.GetLabelText() );
  DALI_TEST_CHECK( STR == textView.GetText() );
}

static void UtcDaliPushButtonPressed()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonPressed");

  PushButton pushButton = PushButton::New();
  pushButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  pushButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  pushButton.SetPosition( 240, 400 );
  pushButton.SetSize( 100, 100 );

  Stage::GetCurrent().Add( pushButton );

  application.SendNotification();
  application.Render();

  gPushButtonPressed = false;

  // connect to its touch signal
  pushButton.PressedSignal().Connect( &PushButtonPressed );

  Dali::Integration::TouchEvent eventDown;
  eventDown.AddPoint( pointDownInside );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  application.ProcessEvent( eventDown );

  DALI_TEST_CHECK( gPushButtonPressed );
}

static void UtcDaliPushButtonReleased()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonReleased");

  PushButton pushButton = PushButton::New();
  pushButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  pushButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  pushButton.SetPosition( 240, 400 );
  pushButton.SetSize( 100, 100 );

  Stage::GetCurrent().Add( pushButton );

  application.SendNotification();
  application.Render();

  // connect to its touch signal
  pushButton.ReleasedSignal().Connect( &PushButtonReleased );

  Dali::Integration::TouchEvent event;

  // Test1. Touch point down and up inside the button.

  gPushButtonReleased = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gPushButtonReleased );

  // Test2. Touch point down and up outside the button.

  gPushButtonReleased = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonReleased );

  // Test3. Touch point down inside and up outside the button.

  gPushButtonReleased = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointLeave );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gPushButtonReleased );

  // Test4. Touch point down outside and up inside the button.

  gPushButtonReleased = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointEnter );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonReleased );
}

static void UtcDaliPushButtonToggled()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonToggled");

  PushButton pushButton = PushButton::New();
  pushButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  pushButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  pushButton.SetPosition( 240, 400 );
  pushButton.SetSize( 100, 100 );

  Stage::GetCurrent().Add( pushButton );

  application.SendNotification();
  application.Render();

  // connect to its touch signal
  pushButton.ToggledSignal().Connect( &PushButtonToggled );

  Dali::Integration::TouchEvent event;

  // Test1. No toggle button.

  gPushButtonToggleState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonToggleState );

  // Set toggle property.
  pushButton.SetToggleButton( true );

  // Test2. Touch point down and up inside the button twice.
  gPushButtonToggleState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gPushButtonToggleState );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonToggleState );

  // Test3. Touch point down and up outside the button.

  gPushButtonToggleState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonToggleState );

  // Test4. Touch point down inside and up outside the button.

  gPushButtonToggleState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointLeave );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonToggleState );

  // Test5. Touch point down outside and up inside the button.

  gPushButtonToggleState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointEnter );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonToggleState );
}

static void UtcDaliPushButtonInterruptEventWhenInsensitive()
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
}

static void UtcDaliPushButtonInterruptEventWhenNonVisible()
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
}

void UtcDaliPushButtonProperties()
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
}
