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

#include <stdlib.h>

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>
#include "dali-toolkit-test-utils/toolkit-timer.h"

#include <dali.h>
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
const Dali::TouchPoint pointDownOutside( 0, TouchPoint::Down, 10.0f, 10.0f );
const Dali::TouchPoint pointUpOutside( 0, TouchPoint::Up, 10.0f, 10.0f );

/**
 * Counts how many descendants root Actor has, including
 * itself.
 *
 * @param[in] root The root actor to count from.
 * @return The number of descendants including root actor itself.
 */
int DescendentCount(const Actor& root)
{
  unsigned int numChildren = root.GetChildCount();

  int count = 1;

  for(unsigned int i=0; i<numChildren; ++i)
  {
    count += DescendentCount(root.GetChildAt(i));
  }

  return count;
}

bool HasAncestor(Actor child, Actor ancestor)
{
  while(child && child != ancestor)
  {
    child = child.GetParent();
  }

  return (child == ancestor);
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
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  application.SendNotification();
  application.Render();

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( gTouchedOutside );
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
  popup.SetProperty( Popup::Property::AUTO_HIDE_DELAY, 200 );

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

      popup.SetDisplayState( Popup::SHOWN );

      // Only wait for animation if it isn't instant.
      if( j == 0 )
      {
        DALI_TEST_EQUALS( gPopupState, Popup::SHOWING, TEST_LOCATION );
        WaitAnimation( application );
      }

      DALI_TEST_EQUALS( gPopupState, Popup::SHOWN, TEST_LOCATION );
      popup.SetDisplayState( Popup::HIDDEN );

      if( j == 0 )
      {
        DALI_TEST_EQUALS( gPopupState, Popup::HIDING, TEST_LOCATION );
        WaitAnimation( application );
      }

      DALI_TEST_EQUALS( gPopupState, Popup::HIDDEN, TEST_LOCATION );
    }
  }

  END_TEST;
}

