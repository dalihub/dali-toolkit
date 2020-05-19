/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/events/touch-event-integ.h>
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
const Vector2 VIEW_PAGE_SIZE( 300.f,400.f );
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
int Wait(ToolkitTestApplication& application, int duration = 0)
{
  int time = 0;

  for(int i = 0; i <= ( duration / RENDER_FRAME_INTERVAL); i++)
  {
    application.SendNotification();
    application.Render(RENDER_FRAME_INTERVAL);
    time += RENDER_FRAME_INTERVAL;
  }

  return time;
}


static Vector2 PerformGestureDiagonalSwipe(ToolkitTestApplication& application, Vector2 start, Vector2 direction, int frames, uint32_t& time, bool toStart = true, bool toFinish = true)
{
  Vector2 pos( start );

  if( toStart )
  {
    // Now do a pan starting from (start + 20) and heading (direction)
    Vector2 pos_start_jump( start + Vector2(15.0f, 0.0f) );
    TestStartPan( application, start, pos_start_jump, time );
    pos += direction;
  }

  time += Wait(application);

  for(int i = 0;i<frames;i++)
  {
    pos += direction;
    TestMovePan(application, pos, time );
    time += Wait(application);
  }

  if(toFinish)
  {
    pos += direction;
    TestEndPan(application, pos, time );
    time += Wait(application);
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

  TestPageFactory( bool returnValidTexture = true )
  : mValidTexture( returnValidTexture )
  {
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
   * Create an texture to represent a page content.
   * @param[in] pageId The ID of the page to create.
   * @return An image, or an empty handle if the ID is out of range.
   */
  virtual Texture NewPage( unsigned int pageId )
  {
    if( mValidTexture )
    {
      return Texture::New( Dali::TextureType::TEXTURE_2D, Pixel::RGB888, 100, 100 );
    }
    return Texture(); // empty handle
  }

private:
  unsigned int            mTotalPageNumber;
  bool                    mValidTexture;
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
  TestPageFactory factory;
  portraitView = PageTurnPortraitView::New( factory, VIEW_PAGE_SIZE );
  DALI_TEST_CHECK( portraitView );

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    TestPageFactory factory;
    PageTurnView portraitView = PageTurnPortraitView::New( factory, VIEW_PAGE_SIZE );
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
  TestPageFactory factory;
  landscapeView = PageTurnLandscapeView::New( factory, VIEW_PAGE_SIZE );
  DALI_TEST_CHECK( landscapeView );

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    TestPageFactory factory;
    PageTurnView landscapeView = PageTurnLandscapeView::New( factory, VIEW_PAGE_SIZE );
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

int UtcDaliPageTurnPortraitViewCopyConstructorAndAssignment(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliPageTurnPortraitViewCopyConstructorAndAssignment ");

  // Test default constructor
  PageTurnPortraitView portraitView;
  DALI_TEST_CHECK( !portraitView );

  // Test object creation
  TestPageFactory factory;
  portraitView = PageTurnPortraitView::New( factory, VIEW_PAGE_SIZE );
  DALI_TEST_CHECK( portraitView );

  // Test copy constructor
  PageTurnPortraitView viewCopy( portraitView );
  DALI_TEST_CHECK( viewCopy );

  // Test assignment
  PageTurnPortraitView portraitView2;
  portraitView2 = portraitView;
  DALI_TEST_CHECK( portraitView2 );
  DALI_TEST_CHECK( portraitView == portraitView2);

  // Test down cast
  Handle handleView;
  handleView = portraitView;
  PageTurnPortraitView downCastView = PageTurnPortraitView::DownCast( handleView );
  DALI_TEST_CHECK( downCastView );

  END_TEST;
}

int UtcDaliPageTurnLandscapeViewCopyConstructorAndAssignment(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliPageTurnLandscapeViewCopyConstructorAndAssignment ");

  // Test default constructor
  PageTurnLandscapeView landscapeView;
  DALI_TEST_CHECK( !landscapeView );

  // Test object creation
  TestPageFactory factory;
  landscapeView = PageTurnLandscapeView::New( factory, VIEW_PAGE_SIZE );
  DALI_TEST_CHECK( landscapeView );

  // Test copy constructor
  PageTurnLandscapeView viewCopy( landscapeView );
  DALI_TEST_CHECK( viewCopy );

  // Test assignment
  PageTurnLandscapeView landscapeView2;
  landscapeView2 = landscapeView;
  DALI_TEST_CHECK( landscapeView2 );
  DALI_TEST_CHECK( landscapeView == landscapeView2);

  // Test down cast
  Handle handleView;
  handleView = landscapeView;
  PageTurnLandscapeView downCastView = PageTurnLandscapeView::DownCast( handleView );
  DALI_TEST_CHECK( downCastView );

  END_TEST;
}

int UtcDaliPageTurnViewSetGetProperty(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliPageTurnViewSetGetProperty ");

  TestPageFactory factory;
  PageTurnView landscapeView = PageTurnLandscapeView::New( factory, VIEW_PAGE_SIZE );
  DALI_TEST_CHECK( landscapeView );

  Stage::GetCurrent().Add( landscapeView );

  // Test "viewPageSize" property
  DALI_TEST_CHECK( landscapeView.GetPropertyIndex("viewPageSize") == PageTurnView::Property::VIEW_PAGE_SIZE  );
  DALI_TEST_EQUALS( landscapeView.GetProperty(PageTurnView::Property::VIEW_PAGE_SIZE).Get<Vector2>(), VIEW_PAGE_SIZE, TEST_LOCATION );

  Vector2 newSize( VIEW_PAGE_SIZE.x*0.75, VIEW_PAGE_SIZE.y*0.5f );
  landscapeView.SetProperty( PageTurnView::Property::VIEW_PAGE_SIZE, newSize );
  DALI_TEST_EQUALS( landscapeView.GetProperty(PageTurnView::Property::VIEW_PAGE_SIZE).Get<Vector2>(), newSize, TEST_LOCATION );
  Wait( application);
  DALI_TEST_EQUALS( Vector2(landscapeView.GetTargetSize()), Vector2(newSize.x*2.f, newSize.y), TEST_LOCATION);

  landscapeView.SetProperty( PageTurnView::Property::VIEW_PAGE_SIZE,newSize*1.5f);
  DALI_TEST_EQUALS( landscapeView.GetProperty(PageTurnView::Property::VIEW_PAGE_SIZE).Get<Vector2>(), newSize*1.5f, TEST_LOCATION );
  Wait( application);
  DALI_TEST_EQUALS( Vector2(landscapeView.GetTargetSize()), Vector2(newSize.x*3.f, newSize.y*1.5f), TEST_LOCATION);

  // Test "currentPageId" property
  DALI_TEST_CHECK( landscapeView.GetPropertyIndex("currentPageId") == PageTurnView::Property::CURRENT_PAGE_ID );
  DALI_TEST_EQUALS( landscapeView.GetProperty(PageTurnView::Property::CURRENT_PAGE_ID).Get<int>(), 0, TEST_LOCATION );

  int pageId = static_cast<int>(TOTAL_PAGE_NUMBER)/3;
  landscapeView.SetProperty( PageTurnView::Property::CURRENT_PAGE_ID, pageId );
  DALI_TEST_EQUALS( landscapeView.GetProperty(PageTurnView::Property::CURRENT_PAGE_ID).Get<int>(), pageId, TEST_LOCATION );

  landscapeView.SetProperty( PageTurnView::Property::CURRENT_PAGE_ID, pageId*2 );
  DALI_TEST_EQUALS( landscapeView.GetProperty(PageTurnView::Property::CURRENT_PAGE_ID).Get<int>(), pageId*2, TEST_LOCATION );

  // Test "spinShadow" property
  DALI_TEST_CHECK( landscapeView.GetPropertyIndex( "spineShadow" ) == PageTurnView::Property::SPINE_SHADOW );
  landscapeView.SetProperty( PageTurnView::Property::SPINE_SHADOW, SPINE_SHADOW_PARAMETER );
  DALI_TEST_EQUALS( landscapeView.GetProperty(PageTurnView::Property::SPINE_SHADOW).Get<Vector2>(), SPINE_SHADOW_PARAMETER, TEST_LOCATION );

  landscapeView.SetProperty( PageTurnView::Property::SPINE_SHADOW,  SPINE_SHADOW_PARAMETER*0.75f );
  DALI_TEST_EQUALS( landscapeView.GetProperty(PageTurnView::Property::SPINE_SHADOW).Get<Vector2>(), SPINE_SHADOW_PARAMETER*0.75f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPageTurnPortraitViewSignals(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliPageTurnPortraitViewSignals ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );

  TestPageFactory factory;
  Vector2 size = Stage::GetCurrent().GetSize();
  PageTurnView portraitView = PageTurnPortraitView::New( factory, size );
  portraitView.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
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
  // Do a pan moving up diagonally
  uint32_t time = 0;
  Vector2 start = size * 0.75f;
  Vector2 direction = -size*0.05f; //-size*0.5f/10.f;

  DALI_TEST_EQUALS( portraitView.GetProperty(PageTurnView::Property::CURRENT_PAGE_ID).Get<int>(), 0, TEST_LOCATION );
  PerformGestureDiagonalSwipe( application, start, direction, 5, time, true, false);
  DALI_TEST_CHECK( callbackTurnStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( callbackPanStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackPanFinished.mSignalVerified );

  PerformGestureDiagonalSwipe( application, start+direction*5.f, direction, 5, time, false, true);
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( callbackPanFinished.mSignalVerified );

  time += Wait(application, 1000);
  DALI_TEST_CHECK( callbackTurnFinished.mSignalVerified );
  // the page is turned over
  DALI_TEST_EQUALS( portraitView.GetProperty(PageTurnView::Property::CURRENT_PAGE_ID).Get<int>(), (int)(pageIndex+1), TEST_LOCATION );

  //---Test 2: pan from position( size*0.5f ) to position( size.width, size.height*0.5f ) to position( size * 0.75f ), page 1 will bent then slid back---
  callbackTurnStarted.Reset();
  callbackTurnFinished.Reset();
  callbackPanStarted.Reset();
  callbackPanFinished.Reset();
  portraitView.SetProperty( PageTurnView::Property::CURRENT_PAGE_ID, 5 );
  pageIndex = 5;
  isTurningForwards = true;

  //pan 10 frames from position( size.width, size.height*0.5f ) to position( size * 0.75f )
  start = Vector2( size.width, size.height*0.5f );
  direction = Vector2(-size.width*0.025f, size.height*0.025f);
  PerformGestureDiagonalSwipe( application, start, direction, 5, time, true, false);
  DALI_TEST_CHECK( callbackPanStarted.mSignalVerified );
  DALI_TEST_CHECK( callbackTurnStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( !callbackPanFinished.mSignalVerified );

  signalVerified[0] = false;
  isTurningForwards = false;
  PerformGestureDiagonalSwipe( application, start + direction*2 , direction, 5, time, false, true);
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( callbackPanFinished.mSignalVerified );
  DALI_TEST_CHECK( callbackTurnStarted.mSignalVerified ); // start the sliding back

  time += Wait(application, 1000);
  DALI_TEST_CHECK( callbackTurnFinished.mSignalVerified );
  DALI_TEST_EQUALS( portraitView.GetProperty(PageTurnView::Property::CURRENT_PAGE_ID).Get<int>(), (int)pageIndex, TEST_LOCATION ); // the page is not turned over

  // ----Test 3: pan 10 frames from position( size*0.25f ) to position( size.width*0.75f, size.height*0.25f ), the previous page will be turned backwards---
  callbackTurnStarted.Reset();
  callbackTurnFinished.Reset();
  callbackPanStarted.Reset();
  callbackPanFinished.Reset();
  portraitView.SetProperty( PageTurnView::Property::CURRENT_PAGE_ID, 10);
  pageIndex = 9; // will turn the previous page back
  isTurningForwards = false;
  start = size*0.25f;
  direction = Vector2(size.x*0.05f, 0.f);
  PerformGestureDiagonalSwipe( application, start, direction, 4, time, true, false);
  DALI_TEST_CHECK( callbackPanStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackTurnStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( !callbackPanFinished.mSignalVerified );

  PerformGestureDiagonalSwipe( application, start+direction*5.f, direction, 5, time, false, true);
  DALI_TEST_CHECK( callbackTurnStarted.mSignalVerified );
  DALI_TEST_CHECK( callbackPanFinished.mSignalVerified );
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );

  Wait( application, 1000 );

  DALI_TEST_CHECK( callbackTurnFinished.mSignalVerified );
  DALI_TEST_EQUALS( portraitView.GetProperty(PageTurnView::Property::CURRENT_PAGE_ID).Get<int>(), 9, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPageTurnLanscapeViewSignals(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliPageTurnLanscapeViewSignals ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );
  /***
   * -----------------
   * |               |
   * |---------------|
   * |bookboo|bookboo|
   * |kbookbo|kbookbo|
   * |okbookb|okbookb|
   * |ookbook|ookbook|
   * |bookboo|bookboo|
   * |---------------|
   * |               |
   * ----------------
   */

  TestPageFactory factory;
  Vector2 stageSize = Stage::GetCurrent().GetSize();
  PageTurnView landscapeView = PageTurnLandscapeView::New( factory, Vector2(stageSize.x*0.5f, stageSize.x*0.8f) );
  landscapeView.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
  Stage::GetCurrent().Add( landscapeView );

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
  landscapeView.PageTurnStartedSignal().Connect( &callbackTurnStarted, &PageSignalCallback::PageTurnSignalCallback );

  PageSignalCallback callbackTurnFinished( signalVerified[1], currentView, pageIndex, isTurningForwards );
  landscapeView.PageTurnFinishedSignal().Connect( &callbackTurnFinished, &PageSignalCallback::PageTurnSignalCallback );

  PageSignalCallback callbackPanStarted( signalVerified[2], currentView, pageIndex, isTurningForwards );
  landscapeView.PagePanStartedSignal().Connect( &callbackPanStarted, &PageSignalCallback::PagePanSignalCallback );

  PageSignalCallback callbackPanFinished( signalVerified[3], currentView, pageIndex, isTurningForwards );
  landscapeView.PagePanFinishedSignal().Connect( &callbackPanFinished, &PageSignalCallback::PagePanSignalCallback );

  DALI_TEST_CHECK( !callbackTurnStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( !callbackPanStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackPanFinished.mSignalVerified );

  currentView = landscapeView;

  //-----Test 1: pan 10 frames from position(stageSize.x * 0.85f, stageSize.y*0.5) to position(stageSize.x * 0.45f, stageSize.y*0.5-stageSize.x * 0.3f), page 0 will be turned forward----
  pageIndex = 0;
  isTurningForwards = true;
  // Do a pan to the left.
  uint32_t time = 0;
  Vector2 start = Vector2(stageSize.x * 0.85f, stageSize.y*0.5);
  Vector2 direction = Vector2(-stageSize.x*0.04f, -stageSize.x*0.03f);

  DALI_TEST_EQUALS( landscapeView.GetProperty(PageTurnView::Property::CURRENT_PAGE_ID).Get<int>(), 0, TEST_LOCATION );
  PerformGestureDiagonalSwipe( application, start, direction, 5, time, true, false);
  DALI_TEST_CHECK( callbackTurnStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( callbackPanStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackPanFinished.mSignalVerified );

  PerformGestureDiagonalSwipe( application, start+direction*5.f, direction, 5, time, false, true);
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( callbackPanFinished.mSignalVerified );

  time += Wait(application, 1000);
  DALI_TEST_CHECK( callbackTurnFinished.mSignalVerified );
  // the page is turned over
  DALI_TEST_EQUALS( landscapeView.GetProperty(PageTurnView::Property::CURRENT_PAGE_ID).Get<int>(), (int)(pageIndex+1), TEST_LOCATION );

  //---Test 2: pan from position(stageSize.x * 0.15f, stageSize.y*0.5) to position(stageSize.x * 0.45f, stageSize.y*0.5) page 4 will be turned back---
  callbackTurnStarted.Reset();
  callbackTurnFinished.Reset();
  callbackPanStarted.Reset();
  callbackPanFinished.Reset();
  isTurningForwards = false;

  //pan 10 frames from position( size.width, size.height*0.5f ) to position( size * 0.75f )
  start = Vector2( stageSize.x * 0.15f, stageSize.y*0.5f );
  direction = Vector2(stageSize.x * 0.03f, 0.f);
  PerformGestureDiagonalSwipe( application, start, direction, 5, time, true, false);
  DALI_TEST_CHECK( callbackPanStarted.mSignalVerified );
  DALI_TEST_CHECK( callbackTurnStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( !callbackPanFinished.mSignalVerified );

  PerformGestureDiagonalSwipe( application, start + direction*5.f , direction, 5, time, false, true);
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( callbackPanFinished.mSignalVerified );

  time += Wait(application, 1000);
  DALI_TEST_CHECK( callbackTurnFinished.mSignalVerified );
  DALI_TEST_EQUALS( landscapeView.GetProperty(PageTurnView::Property::CURRENT_PAGE_ID).Get<int>(), 0, TEST_LOCATION ); // the first page is turned back

  // ----Test 3: pan 10 frames from position( size*0.55f ) to position( size.width*0.8f, size.height*0.5f ), no page turn will be started---
  callbackTurnStarted.Reset();
  callbackTurnFinished.Reset();
  callbackPanStarted.Reset();
  callbackPanFinished.Reset();
  isTurningForwards = false;
  start = stageSize*0.55f;
  direction = Vector2(stageSize.x*0.025f, 0.f);
  PerformGestureDiagonalSwipe( application, start, direction, 5, time, true, false);
  DALI_TEST_CHECK( callbackPanStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackTurnStarted.mSignalVerified );
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_CHECK( !callbackPanFinished.mSignalVerified );

  PerformGestureDiagonalSwipe( application, start+direction*5.f, direction, 5, time, false, true);
  DALI_TEST_CHECK( !callbackTurnStarted.mSignalVerified );
  DALI_TEST_CHECK( callbackPanFinished.mSignalVerified );
  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );

  Wait( application, 1000 );

  DALI_TEST_CHECK( !callbackTurnFinished.mSignalVerified );
  DALI_TEST_EQUALS( landscapeView.GetProperty(PageTurnView::Property::CURRENT_PAGE_ID).Get<int>(), 0, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPageImageFactoryGetExtention(void)
{
  ToolkitTestApplication application;
  TestPageFactory factory;
  DALI_TEST_CHECK( factory.GetExtension() == NULL );
  END_TEST;
}

int UtcDaliPageTurnEmptyTextureHandle(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliPageTurnEmptyTextureHandle ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );

  TestPageFactory factory( false /* returns empty handles */ );
  Vector2 size = Stage::GetCurrent().GetSize();
  try
  {
    PageTurnView portraitView = PageTurnPortraitView::New( factory, size );
    portraitView.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    Stage::GetCurrent().Add( portraitView );

    tet_result(TET_FAIL);
  }
  catch (DaliException& e)
  {
    DALI_TEST_ASSERT(e, "must pass in valid texture", TEST_LOCATION );
  }
  catch (...)
  {
    tet_result(TET_FAIL);
  }

  END_TEST;
}

