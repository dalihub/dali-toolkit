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

#include <stdlib.h>

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>
#include "dali-toolkit-test-utils/toolkit-timer.h"

#include <dali.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/popup/popup.h>

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_popup_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_popup_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

const int RENDER_FRAME_INTERVAL = 10;                          ///< Duration of each frame in ms.
const int RENDER_ANIMATION_TEST_DURATION_MS = 2000;            ///< 2000ms to test animation.
const int RENDER_ANIMATION_TEST_DURATION_FRAMES = RENDER_ANIMATION_TEST_DURATION_MS / RENDER_FRAME_INTERVAL; ///< equivalent frames.
const Vector3 DEFAULT_BUTTON_SIZE(100.0f, 50.0f, 0.0f);

Dali::Integration::Point GetPointDown()
{
  Dali::Integration::Point point;
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 10, 10 ) );
  return point;
}

Dali::Integration::Point GetPointUp()
{
  Dali::Integration::Point point;
  point.SetState( PointState::UP );
  point.SetScreenPosition( Vector2( 10, 10 ) );
  return point;
}

/**
 * Counts how many descendants root Actor has, including
 * itself.
 *
 * @param[in] root The root actor to count from.
 * @return The number of descendants including root actor itself.
 */
int DescendentCount( const Actor& root )
{
  unsigned int numChildren = root.GetChildCount();

  int count = 1;

  for( unsigned int i = 0; i < numChildren; ++i )
  {
    count += DescendentCount( root.GetChildAt( i ) );
  }

  return count;
}

bool HasAncestor( Actor child, Actor ancestor )
{
  while( child && child != ancestor )
  {
    child = child.GetParent();
  }

  return ( child == ancestor );
}

static Toolkit::Popup::DisplayState gPopupState = Toolkit::Popup::HIDDEN;
static bool gTouchedOutside;

// Signal callbacks

static void OnPopupTouchedOutside()
{
  gTouchedOutside = true;
}

static void OnPopupShowing()
{
  gPopupState = Toolkit::Popup::SHOWING;
}

static void OnPopupShown()
{
  gPopupState = Toolkit::Popup::SHOWN;
}

static void OnPopupHiding()
{
  gPopupState = Toolkit::Popup::HIDING;
}

static void OnPopupHidden()
{
  gPopupState = Toolkit::Popup::HIDDEN;
}

void ConnectStateSignals( Toolkit::Popup popup )
{
  popup.ShowingSignal().Connect( &OnPopupShowing );
  popup.ShownSignal().Connect( &OnPopupShown );
  popup.HidingSignal().Connect( &OnPopupHiding );
  popup.HiddenSignal().Connect( &OnPopupHidden );
}

void WaitAnimation( ToolkitTestApplication& application )
{
  // Wait for a while (allow animation to complete), and then check state.
  for( int i = 0; i < RENDER_ANIMATION_TEST_DURATION_FRAMES; i++ )
  {
    application.SendNotification();
    application.Render( RENDER_FRAME_INTERVAL );
  }
}

/**
 * A connection tracker is required when connecting to a signal with a functor.
 */
class TestConnectionTrackerObject : public ConnectionTracker
{
};

/**
 * This functor is used to test the popup's signal connection.
 */
struct PopupTestFunctor
{
  PopupTestFunctor()
  {
  }

  void operator()()
  {
  }
};

// Generate a KeyEvent to send to Core.
Integration::KeyEvent GenerateKey( const std::string& keyName,
                                   const std::string& keyString,
                                   int keyCode,
                                   int keyModifier,
                                   unsigned long timeStamp,
                                   const Integration::KeyEvent::State& keyState )
{
  return Integration::KeyEvent( keyName,
                                keyString,
                                keyCode,
                                keyModifier,
                                timeStamp,
                                keyState );
}

} // Anonymous namespace

/*
 * This test checks popup creation.
 */
int UtcDaliPopupNewP( void )
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupNewP" );

  // Create the Popup actor.
  Popup popup;

  DALI_TEST_CHECK( !popup );

  popup = Popup::New();

  DALI_TEST_CHECK( popup );

  Popup popup2( popup );

  DALI_TEST_CHECK( popup2 == popup );

  // Additional check to ensure object is created by checking if it's registered.
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    Popup popup = Popup::New();
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
  END_TEST;
}

/*
 * This test checks popup destruction.
 */
int UtcDaliPopupDestructorP( void )
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupDestructorP" );

  Popup* popup = new Popup();
  delete popup;

  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliPopupDownCastP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupDownCastP" );

  Handle handle = Popup::New();

  Popup popup = Popup::DownCast( handle );

  DALI_TEST_CHECK( popup == handle );
  END_TEST;
}

int UtcDaliPopupSetPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupSetProperty" );

  Popup popup = Popup::New();

  //Test properties
  std::string testString = "Hello World";

  TextLabel textActorIn = TextLabel::New( testString );
  Property::Map map;
  Scripting::CreatePropertyMap( textActorIn, map );
  popup.SetProperty( popup.GetPropertyIndex( "title" ), map );
  TextLabel textActorOut = TextLabel::DownCast( popup.GetTitle() );
  std::string resultText;
  DALI_TEST_CHECK( textActorOut.GetProperty( Toolkit::TextLabel::Property::TEXT ).Get( resultText ) );
  DALI_TEST_EQUALS( testString, resultText, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPopupSetTitleP(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline( " UtcDaliPopupSetTitleP" );

  // Create the Popup actor
  Popup popup = Popup::New();

  // Put in show state so it's layer is connected to popup (for ancestor check).
  popup.SetDisplayState( Popup::SHOWN );

  TextLabel titleActor = TextLabel::New();
  titleActor.SetProperty( Toolkit::TextLabel::Property::TEXT, "title" );

  DALI_TEST_CHECK( !popup.GetTitle() );
  popup.SetTitle( titleActor );
  TextLabel textActor = TextLabel::DownCast( popup.GetTitle() );
  DALI_TEST_CHECK( textActor == titleActor );

  std::string resultText;
  DALI_TEST_CHECK( textActor.GetProperty( Toolkit::TextLabel::Property::TEXT ).Get( resultText ) );

  DALI_TEST_CHECK( ( popup.GetTitle() ) && ( resultText == "title" ) );
  // verify titleActor is actually inside popup, and not elsewhere on stage, or off even.
  DALI_TEST_CHECK( HasAncestor( titleActor, popup ) );

  TextLabel titleActor2 = TextLabel::New();
  titleActor2.SetProperty( Toolkit::TextLabel::Property::TEXT, "anothertitle" );
  popup.SetTitle( titleActor2 );
  DALI_TEST_CHECK( popup.GetTitle() != titleActor );
  DALI_TEST_CHECK( popup.GetTitle() == titleActor2 );
  DALI_TEST_CHECK( TextLabel::DownCast( popup.GetTitle() ).GetProperty( Toolkit::TextLabel::Property::TEXT ).Get( resultText ) );

  DALI_TEST_CHECK( ( popup.GetTitle() ) && ( resultText == "anothertitle" ) );

  // verify titleActor is actually inside popup, and not elsewhere on stage, or off even.
  DALI_TEST_CHECK( HasAncestor( titleActor2, popup ) );
  END_TEST;
}

int UtcDaliPopupSetTitleN(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline( " UtcDaliPopupSetTitleN" );

  // Create the Popup actor
  Popup popup = Popup::New();

  TextLabel titleActor = TextLabel::New( "text" );
  popup.SetTitle( titleActor );

  DALI_TEST_CHECK( popup.GetTitle() );

  // Set a bad title value.
  // Confirm this has disabled the title.
  Actor badActor;
  popup.SetTitle( badActor );

  DALI_TEST_CHECK( !popup.GetTitle() );

  END_TEST;
}

int UtcDaliPopupSetContentP(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline( " UtcDaliPopupSetContentP" );

  // Create the Popup actor
  Popup popup = Popup::New();
  Stage::GetCurrent().Add( popup );
  popup.SetProperty( Toolkit::Popup::Property::ANIMATION_DURATION, 0.0f );

  // Put in show state so it's layer is connected to popup (for ancestor check).
  popup.SetDisplayState( Popup::SHOWN );

  PushButton button = PushButton::New();
  DALI_TEST_CHECK( !HasAncestor( button, popup ) );
  popup.SetFooter( button );
  // Hide and then re-show popup to cause button to be rearranged and added to popup.
  popup.SetDisplayState( Popup::HIDDEN );
  popup.SetDisplayState( Popup::SHOWN );
  DALI_TEST_CHECK( HasAncestor( button, popup ) );
  END_TEST;
}

int UtcDaliPopupSetContentN(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline( " UtcDaliPopupSetContentN" );

  // Create the Popup actor
  Popup popup = Popup::New();

  TextLabel content = TextLabel::New( "text" );
  popup.SetContent( content );

  DALI_TEST_CHECK( popup.GetContent() );

  // Set a bad title value.
  Actor badActor;
  popup.SetContent( badActor );

  DALI_TEST_CHECK( !popup.GetContent() );

  END_TEST;
}

int UtcDaliPopupSetFooterP(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliPopupSetFooterP");

  // Create the Popup actor
  Popup popup = Popup::New();
  Stage::GetCurrent().Add( popup );
  popup.SetProperty( Toolkit::Popup::Property::ANIMATION_DURATION, 0.0f );
  // Put in show state so it's layer is connected to popup (for ancestor check).
  popup.SetDisplayState( Popup::SHOWN );

  PushButton button = PushButton::New();
  DALI_TEST_CHECK( !HasAncestor(button, popup) );
  popup.SetFooter( button );
  // Hide and then re-show popup to cause button to be rearranged and added to popup.
  popup.SetDisplayState( Popup::HIDDEN );
  popup.SetDisplayState( Popup::SHOWN );
  DALI_TEST_CHECK( HasAncestor( button, popup ) );
  END_TEST;
}

int UtcDaliPopupSetFooterN(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliPopupSetFooterN");

  // Create the Popup actor
  Popup popup = Popup::New();

  PushButton button = PushButton::New();
  popup.SetFooter( button );

  DALI_TEST_CHECK( popup.GetFooter() );

  // Set a bad title value.
  Actor badActor;
  popup.SetFooter( badActor );

  DALI_TEST_CHECK( !popup.GetFooter() );

  END_TEST;
}

int UtcDaliPopupSetControlFooterMultiple(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliPopupSetControlFooterMultiple");

  // Create the Popup actor
  Popup popup = Popup::New();
  Stage::GetCurrent().Add( popup );
  popup.SetProperty( Toolkit::Popup::Property::ANIMATION_DURATION, 0.0f );
  // Put in show state so it's layer is connected to popup (for ancestor check).
  popup.SetDisplayState( Popup::SHOWN );

  Actor container = Actor::New();
  PushButton button1 = PushButton::New();
  PushButton button2 = PushButton::New();
  DALI_TEST_CHECK( !HasAncestor( button1, popup ) );
  DALI_TEST_CHECK( !HasAncestor( button2, popup ) );
  container.Add( button1 );
  container.Add( button2 );
  popup.SetFooter( container );

  // Hide and then re-show popup to cause button to be rearranged and added to popup.
  popup.SetDisplayState( Popup::HIDDEN );
  popup.SetDisplayState( Popup::SHOWN );
  DALI_TEST_CHECK( HasAncestor( button1, popup ) );
  DALI_TEST_CHECK( HasAncestor( button2, popup ) );
  END_TEST;
}

int UtcDaliPopupSetStateP(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliPopupSetStateP");

  // Create the Popup actor
  Popup popup = Popup::New();

  popup.SetProperty( Toolkit::Popup::Property::ANIMATION_DURATION, 0.0f );

  DALI_TEST_EQUALS( popup.GetDisplayState(), Popup::HIDDEN, TEST_LOCATION );

  popup.SetDisplayState( Popup::SHOWN );
  DALI_TEST_EQUALS( Popup::SHOWN, popup.GetDisplayState(), TEST_LOCATION );

  popup.SetDisplayState( Popup::HIDDEN );
  DALI_TEST_EQUALS( Popup::HIDDEN, popup.GetDisplayState(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPopupSetStateN(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliPopupSetStateN");

  // Create the Popup actor
  Popup popup = Popup::New();

  popup.SetProperty( Toolkit::Popup::Property::ANIMATION_DURATION, 1.0f );

  DALI_TEST_EQUALS( popup.GetDisplayState(), Popup::HIDDEN, TEST_LOCATION );

  popup.SetDisplayState( Popup::SHOWN );
  DALI_TEST_EQUALS( Popup::SHOWING, popup.GetDisplayState(), TEST_LOCATION );

  // Test cancelling a show before it has finished.
  popup.SetDisplayState( Popup::HIDDEN );
  DALI_TEST_EQUALS( Popup::HIDING, popup.GetDisplayState(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPopupDisplayStateSignal(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline( " UtcDaliPopupDisplayStateSignal" );

  // Create the Popup actor
  Popup popup = Popup::New();
  ConnectStateSignals( popup );

  popup.SetProperty( Toolkit::Popup::Property::ANIMATION_DURATION, 1.0f );
  popup.SetDisplayState( Popup::SHOWN );
  DALI_TEST_EQUALS( Popup::SHOWING, popup.GetDisplayState(), TEST_LOCATION );
  DALI_TEST_EQUALS( gPopupState, Popup::SHOWING, TEST_LOCATION );

  // Wait for a while (allow animation to complete), and then check state.
  for( int i = 0; i < RENDER_ANIMATION_TEST_DURATION_FRAMES; i++)
  {
    application.SendNotification();
    application.Render( RENDER_FRAME_INTERVAL );
  }

  DALI_TEST_EQUALS( Popup::SHOWN, popup.GetDisplayState(), TEST_LOCATION );
  DALI_TEST_EQUALS( gPopupState, Popup::SHOWN, TEST_LOCATION );

  // Hide slowly
  popup.SetDisplayState( Popup::HIDDEN );
  DALI_TEST_EQUALS( Popup::HIDING, popup.GetDisplayState(), TEST_LOCATION );
  DALI_TEST_EQUALS( gPopupState, Popup::HIDING, TEST_LOCATION );

  // Wait for a while (allow animation to complete), and then check state.
  for( int i = 0; i < RENDER_ANIMATION_TEST_DURATION_FRAMES; i++)
  {
    application.SendNotification();
    application.Render( RENDER_FRAME_INTERVAL );
  }

  DALI_TEST_EQUALS( Popup::HIDDEN, popup.GetDisplayState(), TEST_LOCATION );
  DALI_TEST_EQUALS( gPopupState, Popup::HIDDEN, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPopupShowHide(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliPopupShowHide");

  // Create the Popup actor
  Popup popup = Popup::New();
  ConnectStateSignals( popup );

  Actor container = Actor::New();
  PushButton button1 = PushButton::New();
  PushButton button2 = PushButton::New();
  button1.SetSize( DEFAULT_BUTTON_SIZE.GetVectorXY() );
  button2.SetSize( DEFAULT_BUTTON_SIZE.GetVectorXY() );
  container.Add( button1 );
  container.Add( button2 );
  popup.SetFooter( container );

  // Show
  // Note: in most popup animation implementations show would result in
  // popup being onstage immediately following Show(). However we can't
  // assume for all. e.g. If one creates a animation with a delay.
  popup.SetDisplayState( Popup::SHOWN );

  // Wait for a while (allow animation to complete), and then check state.
  for(int i = 0; i < RENDER_ANIMATION_TEST_DURATION_FRAMES; i++)
  {
    application.SendNotification();
    application.Render( RENDER_FRAME_INTERVAL );
  }

  // Hide
  popup.SetDisplayState( Popup::HIDDEN );

  // Wait for a while (allow animation to complete), and then check state.
  for(int i = 0; i < RENDER_ANIMATION_TEST_DURATION_FRAMES; i++)
  {
    application.SendNotification();
    application.Render(RENDER_FRAME_INTERVAL);
  }

  DALI_TEST_EQUALS( gPopupState, Popup::HIDDEN, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPopupPropertyTailVisibility(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliPopupShowHideTail");

  // Create the Popup actor
  Popup popup = Popup::New();
  Stage::GetCurrent().Add( popup );

  popup.SetProperty( Popup::Property::TAIL_VISIBILITY, false );
  popup.SetDisplayState( Popup::SHOWN );

  int withoutTailCount = DescendentCount( popup );

  popup.SetDisplayState( Popup::HIDDEN );

  popup.SetProperty( Popup::Property::TAIL_POSITION, "BOTTOM_CENTER" );
  popup.SetProperty( Popup::Property::TAIL_VISIBILITY, true );
  popup.SetDisplayState( Popup::SHOWN );

  int withTailCount = DescendentCount( popup );

  // There should be more actors if the Tail has been added.
  DALI_TEST_CHECK( withTailCount > withoutTailCount );

  // Hide again
  popup.SetDisplayState( Popup::HIDDEN );
  popup.SetProperty( Popup::Property::TAIL_VISIBILITY, false );
  popup.SetDisplayState( Popup::SHOWN );
  int withoutTailCount2 = DescendentCount(popup);

  DALI_TEST_CHECK( withTailCount > withoutTailCount2 );
  END_TEST;
}

int UtcDaliPopupOnTouchedOutsideSignal(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliPopupOnTouchedOutside");

  // Create the Popup actor
  Popup popup = Popup::New();
  popup.SetParentOrigin( ParentOrigin::CENTER );
  popup.SetAnchorPoint( ParentOrigin::CENTER );
  popup.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  popup.SetSize( 50.0f, 50.0f );
  popup.SetProperty( Popup::Property::ANIMATION_DURATION, 0.0f );
  Stage::GetCurrent().Add( popup );
  popup.OutsideTouchedSignal().Connect( &OnPopupTouchedOutside );
  popup.SetDisplayState( Popup::SHOWN );

  application.SendNotification();
  application.Render();

  gTouchedOutside = false;
  Dali::Integration::TouchEvent event;

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDown() );
  application.ProcessEvent( event );

  application.SendNotification();
  application.Render();

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUp() );
  application.ProcessEvent( event );

  application.SendNotification();
  application.Render();

  // Confirm the signal is ignored if touch_transparent.
  gTouchedOutside = false;
  popup.SetProperty( Popup::Property::TOUCH_TRANSPARENT, true );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDown() );
  application.ProcessEvent( event );

  application.SendNotification();
  application.Render();

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUp() );
  application.ProcessEvent( event );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( !gTouchedOutside );

  END_TEST;
}

int UtcDaliPopupPropertyAutoHide(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline( " UtcDaliPopupPropertyAutoHide" );

  // Create the Popup actor
  Popup popup = Popup::New();
  ConnectStateSignals( popup );

  Actor container = Actor::New();
  PushButton button1 = PushButton::New();
  button1.SetSize( DEFAULT_BUTTON_SIZE.GetVectorXY() );
  container.Add( button1 );
  popup.SetFooter( container );

  popup.SetProperty( Popup::Property::ANIMATION_DURATION, 0.0f );
  float getAnimationDuration = 0.0f;
  DALI_TEST_CHECK( popup.GetProperty( Popup::Property::ANIMATION_DURATION ).Get( getAnimationDuration ) );
  DALI_TEST_EQUALS( getAnimationDuration, 0.0f, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  popup.SetProperty( Popup::Property::AUTO_HIDE_DELAY, 200 );
  int getAutoHideDelay = 0;
  DALI_TEST_CHECK( popup.GetProperty( Popup::Property::AUTO_HIDE_DELAY ).Get( getAutoHideDelay ) );
  DALI_TEST_EQUALS( getAutoHideDelay, 200, TEST_LOCATION );

  Stage::GetCurrent().Add( popup );

  DALI_TEST_EQUALS( gPopupState, Popup::HIDDEN, TEST_LOCATION );

  // Show
  // Note: in most popup animation implementations show would result in
  // popup being onstage immediately following Show(). However we can't
  // assume for all. e.g. If one creates a animation with a delay.
  popup.SetDisplayState( Popup::SHOWN );

  DALI_TEST_EQUALS( gPopupState, Popup::SHOWN, TEST_LOCATION );

  for( int i = 0; i < RENDER_ANIMATION_TEST_DURATION_FRAMES; i++ )
  {
    application.SendNotification();
    application.Render( RENDER_FRAME_INTERVAL );
  }

  // Force the timer used by the popup to expire,
  // this will cause the popup to hide automatically.
  Dali::Timer timer = Timer::New( 0 );
  timer.MockEmitSignal();

  DALI_TEST_EQUALS( gPopupState, Popup::HIDDEN, TEST_LOCATION );

  END_TEST;
}

/*
 * This test checks all animation modes to confirm they all trigger all display states at the expected times.
 */
int UtcDaliPopupPropertyAnimationMode(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline( " UtcDaliPopupPropertyAnimationMode" );

  // Create the Popup actor
  Popup popup = Popup::New();
  ConnectStateSignals( popup );
  popup.SetTitle( TextLabel::New( "Title" ) );
  Stage::GetCurrent().Add( popup );

  std::string animationModes[] = { "NONE", "ZOOM", "FADE", "CUSTOM" };

  // Try both default and zero animation duration, as zero has a special case for some animation types.
  for( int j = 0; j <= 1; j++ )
  {
    // On the second loop, set duration to zero.
    if( j == 1 )
    {
      popup.SetProperty( Popup::Property::ANIMATION_DURATION, 0.0f );
    }

    // Loop through 4 animation modes.
    for( int i = 0; i < 4; i++ )
    {
      popup.SetProperty( Popup::Property::ANIMATION_MODE, animationModes[i] );

      std::string checkMode;
      DALI_TEST_CHECK( popup.GetProperty( Popup::Property::ANIMATION_MODE ).Get( checkMode ) )

      DALI_TEST_EQUALS( checkMode, animationModes[i], TEST_LOCATION );

      popup.SetProperty( Toolkit::Popup::Property::DISPLAY_STATE, "SHOWN" );
      std::string resultState;

      // Only wait for animation if it isn't instant.
      if( j == 0 )
      {
        DALI_TEST_EQUALS( gPopupState, Popup::SHOWING, TEST_LOCATION );
        DALI_TEST_CHECK( popup.GetProperty( Toolkit::Popup::Property::DISPLAY_STATE ).Get( resultState ) );
        DALI_TEST_EQUALS( resultState, "SHOWING", TEST_LOCATION );
        WaitAnimation( application );
      }

      DALI_TEST_EQUALS( gPopupState, Popup::SHOWN, TEST_LOCATION );
      DALI_TEST_CHECK( popup.GetProperty( Toolkit::Popup::Property::DISPLAY_STATE ).Get( resultState ) );
      DALI_TEST_EQUALS( resultState, "SHOWN", TEST_LOCATION );
      popup.SetDisplayState( Popup::HIDDEN );

      if( j == 0 )
      {
        DALI_TEST_EQUALS( gPopupState, Popup::HIDING, TEST_LOCATION );
        DALI_TEST_CHECK( popup.GetProperty( Toolkit::Popup::Property::DISPLAY_STATE ).Get( resultState ) );
        DALI_TEST_EQUALS( resultState, "HIDING", TEST_LOCATION );
        WaitAnimation( application );
      }

      DALI_TEST_EQUALS( gPopupState, Popup::HIDDEN, TEST_LOCATION );
      DALI_TEST_CHECK( popup.GetProperty( Toolkit::Popup::Property::DISPLAY_STATE ).Get( resultState ) );
      DALI_TEST_EQUALS( resultState, "HIDDEN", TEST_LOCATION );
    }
  }

  END_TEST;
}

int UtcDaliPopupPropertyTitle(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupPropertyTitle" );

  // Create the Popup actor
  Popup popup = Popup::New();

  std::string testLabelText = "TitleTest";
  TextLabel titleLabel = TextLabel::New();
  titleLabel.SetProperty( Toolkit::TextLabel::Property::TEXT, testLabelText );
  Actor title = titleLabel;
  Property::Map map;
  Scripting::CreatePropertyMap( title, map );
  popup.SetProperty( Toolkit::Popup::Property::TITLE, map );

  Property::Map resultMap;
  DALI_TEST_CHECK( popup.GetProperty( Toolkit::Popup::Property::TITLE ).Get( resultMap ) );

  Property::Value* resultProperty = resultMap.Find( "text" );
  DALI_TEST_CHECK( resultProperty );

  std::string resultText;
  DALI_TEST_CHECK( resultProperty->Get( resultText ) );

  DALI_TEST_EQUALS( resultText, testLabelText, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPopupPropertyContent(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupPropertyContent" );

  // Create the Popup actor
  Popup popup = Popup::New();

  std::string testLabelText = "ContentTest";
  TextLabel contentLabel = TextLabel::New();
  contentLabel.SetProperty( Toolkit::TextLabel::Property::TEXT, testLabelText );
  Actor content = contentLabel;
  Property::Map map;
  Scripting::CreatePropertyMap( content, map );
  popup.SetProperty( Toolkit::Popup::Property::CONTENT, map );

  Property::Map resultMap;
  DALI_TEST_CHECK( popup.GetProperty( Toolkit::Popup::Property::CONTENT ).Get( resultMap ) );

  Property::Value* resultProperty = resultMap.Find( "text" );
  DALI_TEST_CHECK( resultProperty );

  std::string resultText;
  DALI_TEST_CHECK( resultProperty->Get( resultText ) );

  DALI_TEST_EQUALS( resultText, testLabelText, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPopupPropertyFooter(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupPropertyFooter" );

  // Create the Popup actor
  Popup popup = Popup::New();

  std::string testLabelText = "FooterTest";
  TextLabel footerLabel = TextLabel::New();
  footerLabel.SetProperty( Toolkit::TextLabel::Property::TEXT, testLabelText );
  Actor footer = footerLabel;
  Property::Map map;
  Scripting::CreatePropertyMap( footer, map );
  popup.SetProperty( Toolkit::Popup::Property::FOOTER, map );

  Property::Map resultMap;
  DALI_TEST_CHECK( popup.GetProperty( Toolkit::Popup::Property::FOOTER ).Get( resultMap ) );

  Property::Value* resultProperty = resultMap.Find( "text" );
  DALI_TEST_CHECK( resultProperty );

  std::string resultText;
  DALI_TEST_CHECK( resultProperty->Get( resultText ) );

  DALI_TEST_EQUALS( resultText, testLabelText, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPopupPropertyContextualMode(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupPropertyContextualMode" );

  // Create the Popup actor
  Popup popup = Popup::New();
  popup.SetProperty( Popup::Property::ANIMATION_DURATION, 0.0f );
  std::string testLabelText = "ContentTest";

  TextLabel contentLabel = TextLabel::New();

  popup.SetContent( contentLabel );

  // Placement actor to parent the popup from so the popup's contextual position can be relative to it.
  Actor placement = Actor::New();
  placement.SetParentOrigin( ParentOrigin::CENTER );
  placement.SetAnchorPoint( AnchorPoint::CENTER );
  placement.SetSize( 1.0f, 1.0f );
  placement.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
  Stage::GetCurrent().Add( placement );

  placement.Add( popup );

  // Test all contextual modes.
  const char* mode[5] = { "NON_CONTEXTUAL", "ABOVE", "RIGHT", "BELOW", "LEFT" };
  Vector2 offsetValues[5];
  offsetValues[0] = Vector2( 0.375f, 0.0f );
  offsetValues[1] = Vector2( -0.125f, -10.5f );
  offsetValues[2] = Vector2( 10.875f, -0.5f );
  offsetValues[3] = Vector2( -0.125f, 10.5f );
  offsetValues[4] = Vector2( -10.875f, -0.5f );

  for( int i = 0; i < 5; ++i )
  {
    popup.SetProperty( Toolkit::Popup::Property::CONTEXTUAL_MODE, mode[i] );

    std::string propertyResult;
    DALI_TEST_CHECK( popup.GetProperty( Toolkit::Popup::Property::CONTEXTUAL_MODE ).Get( propertyResult ) );
    DALI_TEST_EQUALS( propertyResult, std::string( mode[i] ), TEST_LOCATION );

    popup.SetDisplayState( Popup::SHOWN );
    application.SendNotification();
    application.Render();

    // Check the position of the label within the popup.
    DALI_TEST_EQUALS( contentLabel.GetCurrentWorldPosition().GetVectorXY(), offsetValues[i], TEST_LOCATION );

    popup.SetDisplayState( Popup::HIDDEN );
    application.SendNotification();
    application.Render();
  }

  END_TEST;
}

int UtcDaliPopupPropertyBacking(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupPropertyBacking" );

  // Create the Popup actor
  Popup popup = Popup::New();
  popup.SetProperty( Popup::Property::ANIMATION_DURATION, 0.0f );
  Stage::GetCurrent().Add( popup );

  Actor backing = popup.FindChildByName( "popupBacking" );
  DALI_TEST_CHECK( backing );

  DALI_TEST_EQUALS( backing.GetCurrentOpacity(), 1.0f, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  // Check enabled property.
  popup.SetDisplayState( Popup::SHOWN );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( backing.GetCurrentOpacity(), 0.5f, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  popup.SetDisplayState( Popup::HIDDEN );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( backing.GetCurrentOpacity(), 0.0f, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  popup.SetProperty( Popup::Property::BACKING_ENABLED, false );
  bool propertyResult;
  DALI_TEST_CHECK( popup.GetProperty( Popup::Property::BACKING_ENABLED ).Get( propertyResult ) );
  DALI_TEST_EQUALS( propertyResult, false, TEST_LOCATION );

  popup.SetDisplayState( Popup::SHOWN );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( backing.GetCurrentOpacity(), 0.0f, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  popup.SetDisplayState( Popup::HIDDEN );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( backing.GetCurrentOpacity(), 0.0f, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  // Check color property.
  popup.SetProperty( Popup::Property::BACKING_ENABLED, true );
  popup.SetProperty( Popup::Property::BACKING_COLOR, Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );

  popup.SetDisplayState( Popup::SHOWN );
  application.SendNotification();
  application.Render();

  Vector4 resultColor;
  popup.GetProperty( Popup::Property::BACKING_COLOR ).Get( resultColor );
  DALI_TEST_EQUALS( resultColor, Vector4( 1.0f, 0.0f, 0.0f, 1.0f ), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPopupPropertyBackgroundImage(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupPropertyBackgroundImage" );

  // Create the Popup actor
  Popup popup = Popup::New();
  Stage::GetCurrent().Add( popup );

  // Check setting an image
  popup.SetProperty( Toolkit::Popup::Property::POPUP_BACKGROUND_IMAGE, "invalid-image.png" );
  std::string resultString;
  popup.GetProperty( Toolkit::Popup::Property::POPUP_BACKGROUND_IMAGE ).Get( resultString );
  DALI_TEST_EQUALS( resultString, "invalid-image.png", TEST_LOCATION );

  END_TEST;
}

int UtcDaliPopupPropertyCustomAnimation(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupPropertyCustomAnimation" );

  // Create the Popup actor
  Popup popup = Popup::New();
  TextLabel content = TextLabel::New( "text" );
  popup.SetContent( content );

  popup.SetProperty( Popup::Property::ANIMATION_DURATION, 1.0f );
  popup.SetProperty( Popup::Property::ANIMATION_MODE, "CUSTOM" );

  Actor popupContainer = popup.FindChildByName( "popupContainer" );
  DALI_TEST_CHECK( popupContainer );

  Vector3 entryAnimationDestination( 300.0f, 200.0f, 0.0f );
  Vector3 exitAnimationDestination( -300.0f, -200.0f, 0.0f );

  Property::Map animationMapEntry;
  animationMapEntry.Insert( "actor", "customAnimationPopup" );
  animationMapEntry.Insert( "property", "position" );
  animationMapEntry.Insert( "value", entryAnimationDestination );
  animationMapEntry.Insert( "alphaFunction",  "EASE_OUT" );

  Property::Array timePeriodMapEntry;
  timePeriodMapEntry.PushBack( 0.0f );
  timePeriodMapEntry.PushBack( 1.0f );

  animationMapEntry.Insert( "timePeriod",  timePeriodMapEntry );

  Property::Map animationMapExit;
  animationMapExit.Insert( "actor", "customAnimationPopup" );
  animationMapExit.Insert( "property", "position" );
  animationMapExit.Insert( "value", exitAnimationDestination );
  animationMapExit.Insert( "alphaFunction",  "EASE_IN" );

  Property::Array timePeriodMapExit;
  timePeriodMapExit.PushBack( 0.0f );
  timePeriodMapExit.PushBack( 1.0f );

  animationMapExit.Insert( "timePeriod",  timePeriodMapExit );

  popup.SetProperty( Toolkit::Popup::Property::ENTRY_ANIMATION, animationMapEntry );
  popup.SetProperty( Toolkit::Popup::Property::EXIT_ANIMATION, animationMapExit );

  Property::Map resultMap;
  DALI_TEST_CHECK( popup.GetProperty( Toolkit::Popup::Property::ENTRY_ANIMATION ).Get( resultMap ) );
  DALI_TEST_EQUALS( resultMap.Count(), 0u, TEST_LOCATION );
  DALI_TEST_CHECK( popup.GetProperty( Toolkit::Popup::Property::EXIT_ANIMATION ).Get( resultMap ) );
  DALI_TEST_EQUALS( resultMap.Count(), 0u, TEST_LOCATION );

  Stage::GetCurrent().Add( popup );
  popup.SetDisplayState( Popup::SHOWN );

  for( int i = 0; i < RENDER_ANIMATION_TEST_DURATION_FRAMES; i++ )
  {
    application.SendNotification();
    application.Render( RENDER_FRAME_INTERVAL );
  }

  // Test the popup has animated to it's entry-transition destination.
  DALI_TEST_EQUALS( popupContainer.GetCurrentWorldPosition(), entryAnimationDestination, 0.1f, TEST_LOCATION );

  popup.SetDisplayState( Popup::HIDDEN );

  for( int j = 0; j < RENDER_ANIMATION_TEST_DURATION_FRAMES; j++ )
  {
    application.SendNotification();
    application.Render( RENDER_FRAME_INTERVAL );
  }

  DALI_TEST_EQUALS( popupContainer.GetCurrentWorldPosition(), exitAnimationDestination, 0.1f, TEST_LOCATION );

  END_TEST;
}

static bool gPushButtonClicked;
static bool PushButtonClicked( Button button )
{
  gPushButtonClicked = true;
  return true;
}

int UtcDaliPopupPropertyTouchTransparent(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupPropertyTouchTransparent" );

  // Create the Popup actor
  Popup popup = Popup::New();
  TextLabel content = TextLabel::New( "text" );
  popup.SetContent( content );
  popup.SetProperty( Popup::Property::ANIMATION_DURATION, 0.0f );
  popup.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  popup.SetParentOrigin( ParentOrigin::TOP_LEFT );
  popup.SetSize( 100, 100 );
  popup.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

  // Create a button (to go underneath the popup).
  PushButton button = Toolkit::PushButton::New();
  button.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  button.SetParentOrigin( ParentOrigin::TOP_LEFT );
  button.SetSize( 100, 100 );
  button.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

  button.ClickedSignal().Connect( &PushButtonClicked );

  Stage::GetCurrent().Add( button );

  button.Add( popup );

  popup.SetDisplayState( Popup::SHOWN );
  application.SendNotification();
  application.Render();

  gPushButtonClicked = false;
  Dali::Integration::TouchEvent event;

  // Perform a click, the popup should block the click from hitting the button.
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDown() );
  application.ProcessEvent( event );
  application.SendNotification();
  application.Render();

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUp() );
  application.ProcessEvent( event );
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( !gPushButtonClicked );

  // Enable touch transparency.
  popup.SetProperty( Popup::Property::TOUCH_TRANSPARENT, true );
  bool propertyResult;
  DALI_TEST_CHECK( popup.GetProperty( Popup::Property::TOUCH_TRANSPARENT ).Get( propertyResult ) );
  DALI_TEST_EQUALS( propertyResult, true, TEST_LOCATION );

  // Perform a click, the popup should allow the click to travel through to the button.
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDown() );
  application.ProcessEvent( event );
  application.SendNotification();
  application.Render();

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUp() );
  application.ProcessEvent( event );
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( gPushButtonClicked );

  END_TEST;
}

int UtcDaliPopupPropertyTail(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupPropertyTail" );

  // Create the Popup actor
  Popup popup = Popup::New();
  popup.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  popup.SetParentOrigin( ParentOrigin::TOP_LEFT );
  popup.SetSize( 100, 100 );
  popup.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
  TextLabel content = TextLabel::New( "text" );
  popup.SetContent( content );

  std::string imageFilename = "invalid-image.jpg";
  popup.SetProperty( Popup::Property::TAIL_DOWN_IMAGE, imageFilename );
  popup.SetProperty( Popup::Property::TAIL_UP_IMAGE, imageFilename );
  popup.SetProperty( Popup::Property::TAIL_RIGHT_IMAGE, imageFilename );
  popup.SetProperty( Popup::Property::TAIL_LEFT_IMAGE, imageFilename );

  std::string resultString;
  popup.GetProperty( Toolkit::Popup::Property::TAIL_DOWN_IMAGE ).Get( resultString );
  DALI_TEST_EQUALS( resultString, imageFilename, TEST_LOCATION );
  popup.GetProperty( Toolkit::Popup::Property::TAIL_UP_IMAGE ).Get( resultString );
  DALI_TEST_EQUALS( resultString, imageFilename, TEST_LOCATION );
  popup.GetProperty( Toolkit::Popup::Property::TAIL_RIGHT_IMAGE ).Get( resultString );
  DALI_TEST_EQUALS( resultString, imageFilename, TEST_LOCATION );
  popup.GetProperty( Toolkit::Popup::Property::TAIL_LEFT_IMAGE ).Get( resultString );
  DALI_TEST_EQUALS( resultString, imageFilename, TEST_LOCATION );

  popup.SetProperty( Popup::Property::TAIL_VISIBILITY, true );
  bool boolResult;
  DALI_TEST_CHECK( popup.GetProperty( Popup::Property::TAIL_VISIBILITY ).Get( boolResult ) );
  DALI_TEST_EQUALS( boolResult, true, TEST_LOCATION );

  Actor tailActor;
  Vector3 tailPosition( ParentOrigin::TOP_CENTER );

  popup.SetProperty( Popup::Property::TAIL_POSITION, tailPosition );
  Vector3 vectorResult;
  DALI_TEST_CHECK( popup.GetProperty( Popup::Property::TAIL_POSITION ).Get( vectorResult ) );
  DALI_TEST_EQUALS( vectorResult, tailPosition, TEST_LOCATION );

  Stage::GetCurrent().Add( popup );

  popup.SetDisplayState( Popup::SHOWN );
  application.SendNotification();
  application.Render();
  tailActor = popup.FindChildByName( "tailImage" );
  DALI_TEST_CHECK( tailActor );

  float baseValX = tailActor.GetCurrentWorldPosition().x;

  DALI_TEST_GREATER( baseValX, tailActor.GetCurrentWorldPosition().y, TEST_LOCATION );

  popup.SetDisplayState( Popup::HIDDEN );
  application.SendNotification();
  application.Render();

  tailPosition = ParentOrigin::CENTER_LEFT;
  popup.SetProperty( Popup::Property::TAIL_POSITION, tailPosition );

  popup.SetDisplayState( Popup::SHOWN );
  application.SendNotification();
  application.Render();
  tailActor = popup.FindChildByName( "tailImage" );
  DALI_TEST_CHECK( tailActor );

  float baseValY = tailActor.GetCurrentWorldPosition().y;
  DALI_TEST_GREATER( baseValX, tailActor.GetCurrentWorldPosition().x, TEST_LOCATION );

  popup.SetDisplayState( Popup::HIDDEN );
  application.SendNotification();
  application.Render();

  tailPosition = ParentOrigin::BOTTOM_CENTER;
  popup.SetProperty( Popup::Property::TAIL_POSITION, tailPosition );

  popup.SetDisplayState( Popup::SHOWN );
  application.SendNotification();
  application.Render();
  tailActor = popup.FindChildByName( "tailImage" );
  DALI_TEST_CHECK( tailActor );
  DALI_TEST_EQUALS( tailActor.GetCurrentWorldPosition().x, baseValX, TEST_LOCATION );
  DALI_TEST_GREATER( tailActor.GetCurrentWorldPosition().y, baseValY, TEST_LOCATION );

  popup.SetDisplayState( Popup::HIDDEN );
  application.SendNotification();
  application.Render();

  tailPosition = ParentOrigin::CENTER_RIGHT;
  popup.SetProperty( Popup::Property::TAIL_POSITION, tailPosition );

  popup.SetDisplayState( Popup::SHOWN );
  application.SendNotification();
  application.Render();
  tailActor = popup.FindChildByName( "tailImage" );
  DALI_TEST_CHECK( tailActor );
  DALI_TEST_GREATER( tailActor.GetCurrentWorldPosition().x, baseValX, TEST_LOCATION );
  DALI_TEST_EQUALS( tailActor.GetCurrentWorldPosition().y, baseValY, TEST_LOCATION );

  popup.SetDisplayState( Popup::HIDDEN );
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliPopupTypeToast(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupTypeToast" );

  TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( "PopupToast" );
  DALI_TEST_CHECK( typeInfo )

  BaseHandle baseHandle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( baseHandle )

  Toolkit::Popup popup = Toolkit::Popup::DownCast( baseHandle );
  gPopupState = Toolkit::Popup::HIDDEN;
  ConnectStateSignals( popup );
  popup.SetProperty( Popup::Property::ANIMATION_DURATION, 1.0f );

  popup.SetTitle( Toolkit::TextLabel::New( "This is a Toast Popup.\nIt will auto-hide itself" ) );
  Stage::GetCurrent().Add( popup );
  popup.SetDisplayState( Toolkit::Popup::SHOWN );

  for( int i = 0; i < RENDER_ANIMATION_TEST_DURATION_FRAMES; i++ )
  {
    application.SendNotification();
    application.Render( RENDER_FRAME_INTERVAL );
  }

  // Check the toast popup is shown.
  DALI_TEST_EQUALS( gPopupState, Popup::SHOWN, TEST_LOCATION );

  for( int i = 0; i < RENDER_ANIMATION_TEST_DURATION_FRAMES; i++ )
  {
    application.SendNotification();
    application.Render( RENDER_FRAME_INTERVAL );
  }

  // Check the toast popup hides.
  Dali::Timer timer = Timer::New( 0 );
  timer.MockEmitSignal();

  for( int i = 0; i < RENDER_ANIMATION_TEST_DURATION_FRAMES; i++ )
  {
    application.SendNotification();
    application.Render( RENDER_FRAME_INTERVAL );
  }

  DALI_TEST_EQUALS( gPopupState, Popup::HIDDEN, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPopupTypeRegistryCreation(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupTypeRegistryCreation" );

  TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( "Popup" );
  DALI_TEST_CHECK( typeInfo )

  BaseHandle baseHandle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( baseHandle )

  Toolkit::Popup popup = Toolkit::Popup::DownCast( baseHandle );
  gPopupState = Toolkit::Popup::HIDDEN;
  ConnectStateSignals( popup );
  popup.SetProperty( Popup::Property::ANIMATION_DURATION, 0.0f );

  Stage::GetCurrent().Add( popup );
  popup.SetDisplayState( Toolkit::Popup::SHOWN );

  application.SendNotification();
  application.Render();

  // Check the popup is shown.
  DALI_TEST_EQUALS( gPopupState, Popup::SHOWN, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPopupPropertyTypeRegistryConnectSignal(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupPropertyTypeRegistryConnectSignal" );

  // Create the Popup actor
  Popup popup = Popup::New();

  TestConnectionTrackerObject* testTracker = new TestConnectionTrackerObject();
  // Note: The emmision of this signals has already been tested in other tests.
  DALI_TEST_CHECK( popup.ConnectSignal( testTracker, "touchedOutside",  PopupTestFunctor() ) );
  DALI_TEST_CHECK( popup.ConnectSignal( testTracker, "showing", PopupTestFunctor() ) );
  DALI_TEST_CHECK( popup.ConnectSignal( testTracker, "shown", PopupTestFunctor() ) );
  DALI_TEST_CHECK( popup.ConnectSignal( testTracker, "hiding", PopupTestFunctor() ) );
  DALI_TEST_CHECK( popup.ConnectSignal( testTracker, "hidden", PopupTestFunctor() ) );

  // Test connecting to an invalid signal does not work.
  DALI_TEST_CHECK( !popup.ConnectSignal( testTracker, "invalid", PopupTestFunctor() ) );

  END_TEST;
}

int UtcDaliPopupOnControlChildAdd(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupOnControlChildAdd" );

  // Create the Popup actor
  Popup popup = Popup::New();
  popup.SetProperty( Popup::Property::ANIMATION_DURATION, 0.0f );
  std::string testLabelText = "ContentTest";
  TextLabel contentLabel = TextLabel::New( testLabelText );

  popup.Add( contentLabel );

  DALI_TEST_CHECK( HasAncestor( contentLabel, popup ) );

  END_TEST;
}

int UtcDaliPopupOnKeyEvent(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliPopupOnKeyEvent" );

  // Create the Popup actor
  Popup popup = Popup::New();
  popup.SetProperty( Popup::Property::ANIMATION_DURATION, 0.0f );
  Stage::GetCurrent().Add( popup );

  popup.SetDisplayState( Popup::SHOWN );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( popup.GetDisplayState(), Popup::SHOWN, TEST_LOCATION );

  popup.SetKeyInputFocus();

  application.ProcessEvent( GenerateKey( "", "", DALI_KEY_ESCAPE, 0, 0, Integration::KeyEvent::Down ) );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( popup.GetDisplayState(), Popup::HIDDEN, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPopupSetPopupBackgroundBorderProperty(void)
{
  ToolkitTestApplication application;

  tet_infoline( "Set the background border property of a popup & retrieve it" );
  Popup popup = Popup::New();

  Rect< int > rect( 40, 30, 20, 10 );
  tet_infoline( "Ensure value we want to set is different from what is already set" );
  DALI_TEST_CHECK( rect != popup.GetProperty( Popup::Property::POPUP_BACKGROUND_BORDER ).Get< Rect< int > >() );

  tet_infoline( "Set the property and retrieve it to make sure it's the value we set" );
  popup.SetProperty( Popup::Property::POPUP_BACKGROUND_BORDER, rect );
  DALI_TEST_EQUALS( rect, popup.GetProperty( Popup::Property::POPUP_BACKGROUND_BORDER ).Get< Rect< int > >(), TEST_LOCATION );

  tet_infoline( "Set a vector4 as well which should also work" );
  Vector4 vectorValue( 10.0f, 20.0f, 30.0f, 40.0f );
  popup.SetProperty( Popup::Property::POPUP_BACKGROUND_BORDER, vectorValue );
  DALI_TEST_EQUALS( Rect< int >( 10, 20, 30, 40 ), popup.GetProperty( Popup::Property::POPUP_BACKGROUND_BORDER ).Get< Rect< int > >(), TEST_LOCATION );

  END_TEST;
}
