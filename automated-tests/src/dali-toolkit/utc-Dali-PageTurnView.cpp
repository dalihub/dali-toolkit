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

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/integration-api/events/pan-gesture-event.h>
#include <dali-toolkit/devel-api/controls/page-turn-view/page-factory.h>
#include <dali-toolkit/devel-api/controls/page-turn-view/page-turn-landscape-view.h>
#include <dali-toolkit/devel-api/controls/page-turn-view/page-turn-portrait-view.h>
#include <dali-toolkit/devel-api/controls/page-turn-view/page-turn-view.h>



using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const int RENDER_FRAME_INTERVAL = 16;                           ///< Duration of each frame in ms. (at approx 60FPS)
const unsigned int TOTAL_PAGE_NUMBER = 20;
const Vector2 PAGE_SIZE( 300.f,400.f );
const unsigned int IMAGE_WIDTH = 30;
const unsigned int IMAGE_HEIGHT = 30;
const Vector2 IMAGE_SIZE( static_cast<float>( IMAGE_WIDTH ), static_cast<float>(IMAGE_HEIGHT) );
const Vector2 SPINE_SHADOW_PARAMETER( 60.0f, 30.0f );

static bool gObjectCreatedCallBackCalled;
static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

/*
 * Simulate time passed by.
 *
 * @note this will always process at least 1 frame (1/60 sec)
 *
 * @param application Test application instance
 * @param duration Time to pass in milliseconds.
 */
void Wait(ToolkitTestApplication& application, int duration = 0)
{
  for(int i = 0; i <= ( duration / RENDER_FRAME_INTERVAL); i++)
  {
    application.SendNotification();
    application.Render(RENDER_FRAME_INTERVAL);
  }
}

// Generate a PanGestureEvent to send to Core
Integration::PanGestureEvent GeneratePan(
    Gesture::State state,
    const Vector2& previousPosition,
    const Vector2& currentPosition,
    unsigned long timeDelta,
    unsigned int numberOfTouches = 1)
{
  Integration::PanGestureEvent pan(state);

  pan.previousPosition = previousPosition;
  pan.currentPosition = currentPosition;
  pan.timeDelta = timeDelta;
  pan.numberOfTouches = numberOfTouches;

  return pan;
}

/**
 * Helper to generate PanGestureEvent
 *
 * @param[in] application Application instance
 * @param[in] state The Gesture State
 * @param[in] pos The current position of touch.
 */
static void SendPan(ToolkitTestApplication& application, Gesture::State state, const Vector2& pos)
{
  static Vector2 last;

  if( (state == Gesture::Started) ||
      (state == Gesture::Possible) )
  {
    last.x = pos.x;
    last.y = pos.y;
  }

  application.ProcessEvent(GeneratePan(state, last, pos, RENDER_FRAME_INTERVAL));

  last.x = pos.x;
  last.y = pos.y;
}

static Vector2 PerformGestureDiagonalSwipe(ToolkitTestApplication& application, Vector2 start, Vector2 direction, int frames, bool toStart = true, bool toFinish = true)
{
  // Now do a pan starting from (start) and heading (direction)
  Vector2 pos(start);

  if( toStart )
  {
    SendPan(application, Gesture::Possible, pos);
    Wait(application);
    SendPan(application, Gesture::Started, pos);
    Wait(application);
  }

  for(int i = 0;i<frames;i++)
  {
    pos += direction; // Move in this direction
    SendPan(application, Gesture::Continuing, pos);
    Wait(application);
  }

  if(toFinish)
  {
    pos += direction; // Move in this direction
    SendPan(application, Gesture::Finished, pos);
    Wait(application);
  }

  return pos;
}


//Functor to test whether PageTurnSignal / PagePanSignal is emitted
class PageSignalCallback : public Dali::ConnectionTracker
{
public:

  PageSignalCallback( bool& signalReceived, PageTurnView& view, unsigned int& pageIndex, bool& isForwards )
  : mSignalVerified( signalReceived ),
    mView( view ),
    mPageIndex( pageIndex ),
    mIsTurningForwards( isForwards )
  {
  }

  // callback to be connected to PageTurnSignal
  void PageTurnSignalCallback( PageTurnView view, unsigned int pageIndex, bool isTurningForward )
  {
    tet_infoline( "Verifying PageTurnedSignal" );

    if( mView == view && mPageIndex == pageIndex && mIsTurningForwards == isTurningForward )
    {
      mSignalVerified = true;
    }
  }

  // callback to be connected to PagePanSignal
  void PagePanSignalCallback( PageTurnView view )
  {
    tet_infoline( "Verifying PagePannedSignal" );

    if( mView == view )
    {
      mSignalVerified = true;
    }
  }

  void Reset()
  {
    mSignalVerified = false;
  }

  bool&         mSignalVerified;
  PageTurnView& mView;
  unsigned int& mPageIndex;
  bool&         mIsTurningForwards;

};


//Implementation of PageFactory for providing page actors to PageTurnView
class TestPageFactory : public PageFactory
{
public:

  TestPageFactory(ToolkitTestApplication& application)
  : mApplication( application )
  {
    mSourceActors.resize(TOTAL_PAGE_NUMBER);
    mTotalPageNumber = TOTAL_PAGE_NUMBER;
  }

  /**
   * Query the number of pages available from the factory.
   * The maximum available page has an ID of GetNumberOfPages()-1.
   */
  virtual unsigned int GetNumberOfPages()
  {
    return mTotalPageNumber;
  }

  /**
   * Create an image actor to represent a page.
   * @param[in] pageId The ID of the page to create.
   * @return An image actor, or an uninitialized pointer if the ID is out of range.
   */
  virtual Actor NewPage( unsigned int pageId )
  {
    if(!mSourceActors[pageId])
    {
      Actor actor = CreateSolidColorImageActor(mApplication, Color::BLUE,IMAGE_WIDTH,IMAGE_HEIGHT);
      actor.SetName( static_cast<std::ostringstream*>( &(std::ostringstream() << pageId) )->str()  );

      actor.SetParentOrigin( ParentOrigin::CENTER );
      actor.SetAnchorPoint( AnchorPoint::CENTER );

      SetActorHittability( actor, true );

      mSourceActors[pageId] = actor;
    }

    return mSourceActors[pageId];
  }

  void DeletePage( unsigned int pageId )
  {
    mSourceActors.erase( mSourceActors.begin() + pageId );
    mTotalPageNumber--;
  }

private:
  ToolkitTestApplication& mApplication;
  std::vector<Actor>      mSourceActors;
  unsigned int            mTotalPageNumber;
};

}// namespace

void dali_page_turn_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_page_turn_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliPageTurnPortraitViewNew(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliPageTurnViewNew ");

  // Test default constructor
  PageTurnView portraitView;
  DALI_TEST_CHECK( !portraitView );

  // Test object creation
  TestPageFactory factory(application);
  portraitView = PageTurnPortraitView::New( factory, PAGE_SIZE );
  DALI_TEST_CHECK( portraitView );

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    TestPageFactory factory(application);
    PageTurnView PortraitView = PageTurnPortraitView::New( factory, PAGE_SIZE );
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );

  // Test copy constructor
  PageTurnView viewCopy( portraitView );
  DALI_TEST_CHECK( viewCopy );

  // Test down cast
  Handle handleView;
  handleView = portraitView;
  PageTurnView downCastView = PageTurnView::DownCast( handleView );
  DALI_TEST_CHECK( downCastView );
  END_TEST;
}

int UtcDaliPageTurnLandscapeViewNew(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliPageTurnViewNew ");

  //Test default constructor
  PageTurnView landscapeView;
  DALI_TEST_CHECK( !landscapeView );

  // Test object creation
  TestPageFactory factory(application);
  landscapeView = PageTurnLandscapeView::New( factory, PAGE_SIZE );
  DALI_TEST_CHECK( landscapeView );

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    TestPageFactory factory(application);
    PageTurnView landscapeView = PageTurnLandscapeView::New( factory, PAGE_SIZE );
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );

  // Test copy constructor
  PageTurnView viewCopy( landscapeView );
  DALI_TEST_CHECK( viewCopy );

  // Test down cast
  Handle handleView;
  handleView = landscapeView;
  PageTurnView downCastView = PageTurnView::DownCast( handleView );
  DALI_TEST_CHECK( downCastView );

  END_TEST;
}

int UtcDaliPageTurnViewSetAndGetSpineShadowParameter(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliPageTurnViewSetAndGetSpineShadowParameter ");

  TestPageFactory factory(application);
  PageTurnView landscapeView = PageTurnLandscapeView::New( factory, PAGE_SIZE );
  DALI_TEST_CHECK( landscapeView.GetSpineShadowParameter() !=  SPINE_SHADOW_PARAMETER);
  landscapeView.SetSpineShadowParameter(SPINE_SHADOW_PARAMETER);
  DALI_TEST_CHECK( landscapeView.GetSpineShadowParameter() ==  SPINE_SHADOW_PARAMETER);
  END_TEST;
}

int UtcDaliPageTurnViewGoToPageAndGetCurrentPage(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliPageTurnViewGoToPageAndGetCurrentPage ");

  TestPageFactory factory(application);
  PageTurnView portraitView = PageTurnPortraitView::New( factory, PAGE_SIZE );
  DALI_TEST_CHECK( portraitView.GetCurrentPage() == 0 );

  portraitView.GoToPage( 10 );
  DALI_TEST_CHECK( portraitView.GetCurrentPage() == 10 );

  portraitView.GoToPage( 5 );
  DALI_TEST_CHECK( portraitView.GetCurrentPage() == 5 );
  END_TEST;
}

int UtcDaliPageTurnViewEnterLeaveEditMode(void)
{
  ToolkitTestApplication application;

  tet_infoline( " UtcDaliPageTurnViewEnterLeaveEditMode " );

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );

  TestPageFactory factory(application);
  factory.EnableOffscreenRendering( );

  PageTurnView pageTurnView = PageTurnLandscapeView::New( factory, PAGE_SIZE );
  pageTurnView.SetPositionInheritanceMode( USE_PARENT_POSITION );
  Stage::GetCurrent().Add( pageTurnView );

  Vector2 size = Stage::GetCurrent().GetSize();

  pageTurnView.GoToPage(5);

  // Render and notify
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);

  Actor actor = pageTurnView.EnterEditMode();
  // Test that when entering edit mode, current page source actor is returned.
  unsigned int pageId;
  std::istringstream( actor.GetName() ) >> pageId;
  DALI_TEST_CHECK( pageId == 5 );

  bool signalVerified;
  PageTurnView currentView;
  unsigned int pageIndex;
  bool isTurningForwards;
  PageSignalCallback callbackPanStarted( signalVerified, currentView, pageIndex, isTurningForwards );
  pageTurnView.PagePanStartedSignal().Connect( &callbackPanStarted, &PageSignalCallback::PagePanSignalCallback );

  currentView = pageTurnView;
  pageIndex = 5;
  DALI_TEST_CHECK( !callbackPanStarted.mSignalVerified );

  // Test that the control does not receive pan gesture in edit-mode
  PerformGestureDiagonalSwipe( application, Vector2(size*0.75f), Vector2(size*0.01f), 10, true, true);
  DALI_TEST_CHECK( !callbackPanStarted.mSignalVerified );

  pageTurnView.LeaveEditMode();
  // Test that the control receives pan gesture after leaving edit-mode
  PerformGestureDiagonalSwipe( application, Vector2(size*0.75f), Vector2(size*0.01f), 10, true, true);
  DALI_TEST_CHECK( callbackPanStarted.mSignalVerified );
  END_TEST;
}

int UtcDaliPageTurnViewGetHitActor(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliPageTurnViewGetHitActor ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );

  TestPageFactory factory(application);
  factory.EnableOffscreenRendering( );

  PageTurnView pageTurnView = PageTurnPortraitView::New( factory, PAGE_SIZE );
  pageTurnView.SetParentOrigin( ParentOrigin::TOP_LEFT );
  pageTurnView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  Stage::GetCurrent().Add( pageTurnView );

  // Render and notify
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);

  pageTurnView.GoToPage(3);

  Vector2 localCoordinate = Vector2();
  Vector2 screenCoordinate = PAGE_SIZE*0.5f+Vector2(7.f,8.f);
  Actor hitActor = pageTurnView.GetHitActor( screenCoordinate, localCoordinate );
  DALI_TEST_CHECK( hitActor );
  unsigned int pageId;
  std::istringstream( hitActor.GetName() ) >> pageId;
  DALI_TEST_CHECK( pageId == 3 );
  DALI_TEST_EQUALS(  localCoordinate, IMAGE_SIZE*0.5f+Vector2(7.f,8.f), 0.1f, TEST_LOCATION  );

  screenCoordinate = PAGE_SIZE*0.5f+IMAGE_SIZE;
  hitActor = pageTurnView.GetHitActor( screenCoordinate, localCoordinate );
  DALI_TEST_CHECK( !hitActor );
  END_TEST;
}

int UtcDaliPageTurnViewRefresh(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliPageTurnViewRefresh ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );

  TestPageFactory factory(application);
  factory.EnableOffscreenRendering( );
  PageTurnView pageTurnView = PageTurnPortraitView::New( factory, PAGE_SIZE );
  pageTurnView.SetParentOrigin( ParentOrigin::TOP_LEFT );
  pageTurnView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  Stage::GetCurrent().Add( pageTurnView );

  // Render and notify
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();

  factory.DeletePage( 0 );

  pageTurnView.RefreshCurrentPage();
  // simply calls the certain off screen render task to refresh
  Vector2 localCoordinate = Vector2();
  Vector2 screenCoordinate = PAGE_SIZE*0.5f;
  Actor hitActor = pageTurnView.GetHitActor( screenCoordinate, localCoordinate );
  unsigned int pageId;
  std::istringstream( hitActor.GetName() ) >> pageId;
  DALI_TEST_CHECK( pageId == 0 );

  pageTurnView.RefreshAll();
  // re-parent all the source actors and refresh
  hitActor = pageTurnView.GetHitActor( screenCoordinate, localCoordinate );
  std::istringstream( hitActor.GetName() ) >> pageId;
  DALI_TEST_CHECK( pageId == 1 );
  END_TEST;
}

int UtcDaliPageTurnViewSignals(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliPageTurnViewSignals ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );

  TestPageFactory factory(application);
  Vector2 size = Stage::GetCurrent().GetSize();
  PageTurnView portraitView = PageTurnPortraitView::New( factory, size );
  portraitView.SetPositionInheritanceMode( USE_PARENT_POSITION );
  Stage::GetCurrent().Add( portraitView );

  // Render and notify
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);

  // [0]: testing PageTurnStartedSignal;  [1]: testing PageTurnFinishedSignal
  // [2]: testing PagePanStartedSignal;   [3]: testing PagePanFinishedSignal
  bool signalVerified[4] = {false, false, false, false};
  PageTurnView currentView;
  unsigned int pageIndex;
  bool isTurningForwards;

  PageSignalCallback callbackTurnStarted( signalVerified[0], currentView, pageIndex, isTurningForwards );
  portraitView.PageTurnStartedSignal().Connect( &callbackTurnStarted, &PageSignalCallback::PageTurnSignalCallback );

  PageSignalCallback callbackTurnFinished( signalVerified[1], currentView, pageIndex, isTurningForwards );
  portraitView.PageTurnFinishedSignal().Connect( &callbackTurnFinished, &PageSignalCallback::PageTurnSignalCallback );

  PageSignalCallback callbackPanStarted( signalVerified[2], currentView, pageIndex, isTurningForwards );
  portraitView.PagePanStartedSignal().Connect( &callbackPanStarted, &PageSignalCallback::PagePanSignalCallback );

  PageSignalCallback callbackPanFinished( signalVerified[3], currentView, pageIndex, isTurningForwards );
  portraitView.PagePanFinishedSignal().Connect( &callbackPanFinished, &PageSignalCallback::PagePanSignalCallback );

  DALI_TEST_CHECK( !callbackTurnStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( !callbackPanStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackPanFinished.mSignalVerified );

  currentView = portraitView;

  //-----Test 1: pan 10 frames from position(size * 0.75f) to position(size * 0.25f), page 0 will be turned forward----
  pageIndex = 0;
  isTurningForwards = true;
  // Do a pan moving up diagonally.
  Vector2 start = size * 0.75f;
  Vector2 direction = -size*0.05f; //-size*0.5f/10.f;

  DALI_TEST_CHECK( portraitView.GetCurrentPage() == 0);
  PerformGestureDiagonalSwipe( application, start, direction, 5, true, false);
  DALI_TEST_CHECK( callbackTurnStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( callbackPanStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackPanFinished.mSignalVerified );

  PerformGestureDiagonalSwipe( application, start+direction*5.f, direction, 5, false, true);
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( callbackPanFinished.mSignalVerified );

  Wait(application, 1000);
  DALI_TEST_CHECK( callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( portraitView.GetCurrentPage() == pageIndex+1); // the page is turn over

  //---Test 2: pan from position( size*0.5f ) to position( size.width, size.height*0.5f ) to position( size * 0.75f ), page 1 will bent then slid back---
  callbackTurnStarted.Reset();
  callbackTurnFinished.Reset();
  callbackPanStarted.Reset();
  callbackPanFinished.Reset();
  portraitView.GoToPage(5);
  pageIndex = 5;
  isTurningForwards = true;

  //pan 10 frames from position( size.width, size.height*0.5f ) to position( size * 0.75f )
  start = Vector2( size.width, size.height*0.5f );
  direction = Vector2(-size.width*0.025f, size.height*0.025f);
  PerformGestureDiagonalSwipe( application, start, direction, 5, true, false);
  DALI_TEST_CHECK( callbackPanStarted.mSignalVerified );
  DALI_TEST_CHECK( callbackTurnStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( !callbackPanFinished.mSignalVerified );

  signalVerified[0] = false;
  isTurningForwards = false;
  PerformGestureDiagonalSwipe( application, start + direction*2 , direction, 5, false, true);
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( callbackPanFinished.mSignalVerified );
  DALI_TEST_CHECK( callbackTurnStarted.mSignalVerified ); // start the sliding back

  Wait(application, 1000);
  DALI_TEST_CHECK( callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( portraitView.GetCurrentPage() == pageIndex); // the page is not turned over

  // ----Test 3: pan 10 frames from position( size*0.25f ) to position( size.width*0.75f, size.height*0.25f ), the previous page will be turned backwards---
  callbackTurnStarted.Reset();
  callbackTurnFinished.Reset();
  callbackPanStarted.Reset();
  callbackPanFinished.Reset();
  portraitView.GoToPage(10);
  pageIndex = 9; // will turn the previous page back
  isTurningForwards = false;
  start = size*0.25f;
  direction = Vector2(size.x*0.05f, 0.f);
  PerformGestureDiagonalSwipe( application, start, direction, 5, true, false);
  DALI_TEST_CHECK( callbackPanStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackTurnStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( !callbackPanFinished.mSignalVerified );

  PerformGestureDiagonalSwipe( application, start+direction*5.f, direction, 5, false, true);
  DALI_TEST_CHECK( callbackTurnStarted.mSignalVerified );
  DALI_TEST_CHECK( callbackPanFinished.mSignalVerified );
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );

  Wait( application, 1000 );

  DALI_TEST_CHECK( callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( portraitView.GetCurrentPage() == 9);
  END_TEST;
}
