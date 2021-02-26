/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit-test-suite-utils.h>
#include "dali-toolkit-test-utils/toolkit-timer.h"

#include <dali.h>
#include <dali/devel-api/adaptor-framework/web-engine-request-interceptor.h>
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/wheel-event-integ.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>
#include <dali-toolkit/devel-api/controls/web-view/web-back-forward-list.h>
#include <dali-toolkit/devel-api/controls/web-view/web-back-forward-list-item.h>
#include <dali-toolkit/devel-api/controls/web-view/web-context.h>
#include <dali-toolkit/devel-api/controls/web-view/web-cookie-manager.h>
#include <dali-toolkit/devel-api/controls/web-view/web-form-repost-decision.h>
#include <dali-toolkit/devel-api/controls/web-view/web-settings.h>
#include <dali-toolkit/devel-api/controls/web-view/web-view.h>

using namespace Dali;
using namespace Toolkit;

namespace
{

const char* const TEST_URL1( "http://www.somewhere.valid1.com" );
const char* const TEST_URL2( "http://www.somewhere.valid2.com" );

static int gPageLoadStartedCallbackCalled = 0;
static int gPageLoadInProgressCallbackCalled = 0;
static int gPageLoadFinishedCallbackCalled = 0;
static int gScrollEdgeReachedCallbackCalled = 0;
static int gUrlChangedCallbackCalled = 0;
static int gEvaluateJavaScriptCallbackCalled = 0;
static int gJavaScriptAlertCallbackCalled = 0;
static int gJavaScriptConfirmCallbackCalled = 0;
static int gJavaScriptPromptCallbackCalled = 0;
static int gScreenshotCapturedCallbackCalled = 0;
static int gVideoPlayingCallbackCalled = 0;
static int gGeolocationPermissionCallbackCalled = 0;
static bool gTouched = false;
static bool gHovered = false;
static bool gWheelEventHandled = false;
static int gFormRepostDecisionCallbackCalled = 0;
static std::shared_ptr<Dali::Toolkit::WebFormRepostDecision> gFormRepostDecisionInstance;
static int gFrameRenderedCallbackCalled = 0;
static int gRequestInterceptorCallbackCalled = 0;
static std::shared_ptr<Dali::WebEngineRequestInterceptor> gRequestInterceptorInstance = nullptr;

struct CallbackFunctor
{
  CallbackFunctor(bool* callbackFlag)
  : mCallbackFlag( callbackFlag )
  {
  }

  void operator()()
  {
    *mCallbackFlag = true;
  }
  bool* mCallbackFlag;
};

static void OnPageLoadStarted( WebView view, const std::string& url )
{
  gPageLoadStartedCallbackCalled++;
}

static void OnPageLoadInProgress( WebView view, const std::string& url )
{
  gPageLoadInProgressCallbackCalled++;
}

static void OnPageLoadFinished( WebView view, const std::string& url )
{
  gPageLoadFinishedCallbackCalled++;
}

static void OnScrollEdgeReached( WebView view, Dali::WebEnginePlugin::ScrollEdge edge )
{
  gScrollEdgeReachedCallbackCalled++;
}

static void OnUrlChanged( WebView view, const std::string& url )
{
  gUrlChangedCallbackCalled++;
}

static void OnPageLoadError( WebView view, const std::string& url, WebView::LoadErrorCode errorCode )
{
}

static void OnEvaluateJavaScript( const std::string& result )
{
  gEvaluateJavaScriptCallbackCalled++;
}

static bool OnJavaScriptAlert( const std::string& result )
{
  gJavaScriptAlertCallbackCalled++;
  return true;
}

static bool OnJavaScriptConfirm( const std::string& result )
{
  gJavaScriptConfirmCallbackCalled++;
  return true;
}

static bool OnJavaScriptPrompt( const std::string& meesage1, const std::string& message2 )
{
  gJavaScriptPromptCallbackCalled++;
  return true;
}

static void OnScreenshotCaptured(Dali::Toolkit::ImageView)
{
  gScreenshotCapturedCallbackCalled++;
}

static void OnVideoPlaying(bool isPlaying)
{
  gVideoPlayingCallbackCalled++;
}

static bool OnGeolocationPermission(const std::string&, const std::string&)
{
  gGeolocationPermissionCallbackCalled++;
  return true;
}

static bool OnTouched( Actor actor, const Dali::TouchEvent& touch )
{
  gTouched = true;
  return true;
}

static bool OnHovered( Actor actor, const Dali::HoverEvent& hover )
{
  gHovered = true;
  return true;
}

static bool OnWheelEvent( Actor actor, const Dali::WheelEvent& wheel )
{
  gWheelEventHandled = true;
  return true;
}

static void OnFormRepostDecision(WebView, std::shared_ptr<Dali::Toolkit::WebFormRepostDecision> decision)
{
  gFormRepostDecisionCallbackCalled++;
  gFormRepostDecisionInstance = decision;
}

static void OnFrameRendered(WebView)
{
  gFrameRenderedCallbackCalled++;
}

static void OnRequestInterceptor(WebView view, std::shared_ptr<Dali::WebEngineRequestInterceptor> interceptor)
{
  gRequestInterceptorCallbackCalled++;
  gRequestInterceptorInstance = std::move(interceptor);
}

} // namespace

void web_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void web_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliWebViewBasics(void)
{
  ToolkitTestApplication application;

  // Copy and Assignment Test
  tet_infoline( "UtcDaliWebViewBasic Copy and Assignment Test" );
  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  WebView copy( view );
  DALI_TEST_CHECK( view == copy );

  WebView assign;
  DALI_TEST_CHECK( !assign );

  assign = copy;
  DALI_TEST_CHECK( assign == view );


  // DownCast Test
  tet_infoline( "UtcDaliWebViewBasic DownCast Test" );
  BaseHandle handle(view);

  WebView view2 = WebView::DownCast( handle );
  DALI_TEST_CHECK( view );
  DALI_TEST_CHECK( view2 );
  DALI_TEST_CHECK( view == view2 );


  // TypeRegistry Test
  tet_infoline( "UtcDaliWebViewBasic TypeRegistry Test" );
  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK( typeRegistry );

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "WebView" );
  DALI_TEST_CHECK( typeInfo );

  BaseHandle handle2 = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle2 );

  WebView view3 = WebView::DownCast( handle2 );
  DALI_TEST_CHECK( view3 );

  END_TEST;
}

int UtcDaliWebViewPageNavigation(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  view.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  view.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  view.SetProperty( Actor::Property::POSITION, Vector2( 0, 0 ));
  view.SetProperty( Actor::Property::SIZE, Vector2( 800, 600 ) );
  application.GetScene().Add( view );
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( view );

  ConnectionTracker* testTracker = new ConnectionTracker();
  view.PageLoadStartedSignal().Connect( &OnPageLoadStarted );
  view.PageLoadInProgressSignal().Connect( &OnPageLoadInProgress );
  view.PageLoadFinishedSignal().Connect( &OnPageLoadFinished );
  view.PageLoadErrorSignal().Connect( &OnPageLoadError );
  view.UrlChangedSignal().Connect( &OnUrlChanged );
  bool signal1 = false;
  bool signal2 = false;
  bool signal3 = false;
  bool signal4 = false;
  bool signal5 = false;
  view.ConnectSignal( testTracker, "pageLoadStarted", CallbackFunctor(&signal1) );
  view.ConnectSignal( testTracker, "pageLoadInProgress", CallbackFunctor(&signal2) );
  view.ConnectSignal( testTracker, "pageLoadFinished", CallbackFunctor(&signal3) );
  view.ConnectSignal( testTracker, "urlChanged", CallbackFunctor(&signal4) );
  view.ConnectSignal( testTracker, "invalidname", CallbackFunctor(&signal5) );
  DALI_TEST_EQUALS( gPageLoadStartedCallbackCalled, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( gPageLoadInProgressCallbackCalled, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( gPageLoadFinishedCallbackCalled, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( gUrlChangedCallbackCalled, 0, TEST_LOCATION );

  view.LoadUrl( TEST_URL1 );
  view.GetNaturalSize();
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS( view.CanGoBack(), false, TEST_LOCATION );
  DALI_TEST_EQUALS( gPageLoadStartedCallbackCalled, 1, TEST_LOCATION );
  DALI_TEST_EQUALS( gPageLoadInProgressCallbackCalled, 1, TEST_LOCATION );
  DALI_TEST_EQUALS( gPageLoadFinishedCallbackCalled, 1, TEST_LOCATION );
  DALI_TEST_EQUALS( gUrlChangedCallbackCalled, 1, TEST_LOCATION );
  DALI_TEST_CHECK( signal1 & signal2 & signal3 & signal4 );
  DALI_TEST_CHECK( !signal5 );

  view.LoadUrl( TEST_URL2 );
  view.Suspend();
  view.SetProperty( Actor::Property::SIZE, Vector2( 400, 300 ) );
  application.SendNotification();
  application.Render();
  Test::EmitGlobalTimerSignal();
  view.Resume();
  DALI_TEST_EQUALS( view.CanGoBack(), true, TEST_LOCATION );
  DALI_TEST_EQUALS( view.CanGoForward(), false, TEST_LOCATION );
  DALI_TEST_EQUALS( gPageLoadStartedCallbackCalled, 2, TEST_LOCATION );
  DALI_TEST_EQUALS( gPageLoadInProgressCallbackCalled, 2, TEST_LOCATION );
  DALI_TEST_EQUALS( gPageLoadFinishedCallbackCalled, 2, TEST_LOCATION );
  DALI_TEST_EQUALS( gUrlChangedCallbackCalled, 2, TEST_LOCATION );

  view.GoBack();
  Test::EmitGlobalTimerSignal();
  DALI_TEST_CHECK( !view.CanGoBack() );
  DALI_TEST_CHECK( view.CanGoForward() );

  view.GoForward();
  Test::EmitGlobalTimerSignal();
  DALI_TEST_CHECK( view.CanGoBack() );
  DALI_TEST_CHECK( !view.CanGoForward() );

  view.Reload();
  view.StopLoading();
  view.ClearHistory();
  Test::EmitGlobalTimerSignal();
  DALI_TEST_CHECK( !view.CanGoBack() );
  DALI_TEST_CHECK( !view.CanGoForward() );

  END_TEST;
}

int UtcDaliWebViewTouchAndKeys(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  view.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  view.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  view.SetProperty( Actor::Property::POSITION, Vector2( 0, 0 ));
  view.SetProperty( Actor::Property::SIZE, Vector2( 800, 600 ) );

  application.GetScene().Add( view );
  application.SendNotification();
  application.Render();

  view.GetNaturalSize();
  view.TouchedSignal().Connect( &OnTouched );

  // Touch event
  Dali::Integration::TouchEvent event;
  Dali::Integration::Point pointDown, pointUp;

  event = Dali::Integration::TouchEvent();
  pointDown.SetState( PointState::DOWN );
  pointDown.SetScreenPosition( Vector2( 10, 10 ) );
  event.AddPoint( pointDown );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  pointUp.SetState( PointState::UP );
  pointUp.SetScreenPosition( Vector2( 10, 10 ) );
  event.AddPoint( pointUp );
  application.ProcessEvent( event );

  // Key event
  Toolkit::KeyboardFocusManager::Get().SetCurrentFocusActor( view );
  application.ProcessEvent( Integration::KeyEvent( "", "", "", DALI_KEY_ESCAPE, 0, 0, Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE ) );
  application.SendNotification();

  DALI_TEST_CHECK( gTouched );
  DALI_TEST_CHECK( view );

  END_TEST;
}

int UtcDaliWebViewFocusGainedAndLost(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  view.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  view.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  view.SetProperty( Actor::Property::POSITION, Vector2( 0, 0 ));
  view.SetProperty( Actor::Property::SIZE, Vector2( 800, 600 ) );

  application.GetScene().Add( view );
  application.SendNotification();
  application.Render();

  view.SetKeyInputFocus();
  DALI_TEST_CHECK( view.HasKeyInputFocus() );

  // reset
  view.ClearKeyInputFocus();
  DALI_TEST_CHECK( !view.HasKeyInputFocus() );

  END_TEST;
}

int UtcDaliWebViewPropertyPageZoomFactor(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  view.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  view.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  view.SetProperty( Actor::Property::POSITION, Vector2( 0, 0 ));
  view.SetProperty( Actor::Property::SIZE, Vector2( 800, 600 ) );

  application.GetScene().Add( view );
  application.SendNotification();
  application.Render();

  view.SetProperty( WebView::Property::PAGE_ZOOM_FACTOR, 1.5f);
  float zoomFactor = view.GetProperty<float>( WebView::Property::PAGE_ZOOM_FACTOR );
  DALI_TEST_EQUALS( zoomFactor, 1.5f, TEST_LOCATION );

  view.SetProperty( WebView::Property::PAGE_ZOOM_FACTOR, 1.0f);
  zoomFactor = view.GetProperty<float>( WebView::Property::PAGE_ZOOM_FACTOR );
  DALI_TEST_EQUALS( zoomFactor, 1.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliWebViewPropertyTextZoomFactor(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  view.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  view.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  view.SetProperty( Actor::Property::POSITION, Vector2( 0, 0 ));
  view.SetProperty( Actor::Property::SIZE, Vector2( 800, 600 ) );

  application.GetScene().Add( view );
  application.SendNotification();
  application.Render();

  view.SetProperty( WebView::Property::TEXT_ZOOM_FACTOR, 1.5f);
  float zoomFactor = view.GetProperty<float>( WebView::Property::TEXT_ZOOM_FACTOR );
  DALI_TEST_EQUALS( zoomFactor, 1.5f, TEST_LOCATION );

  view.SetProperty( WebView::Property::TEXT_ZOOM_FACTOR, 1.0f);
  zoomFactor = view.GetProperty<float>( WebView::Property::TEXT_ZOOM_FACTOR );
  DALI_TEST_EQUALS( zoomFactor, 1.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliWebViewPropertyLoadProgressPercentage(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  view.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  view.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  view.SetProperty( Actor::Property::POSITION, Vector2( 0, 0 ));
  view.SetProperty( Actor::Property::SIZE, Vector2( 800, 600 ) );

  application.GetScene().Add( view );
  application.SendNotification();
  application.Render();

  float percentage = view.GetProperty<float>( WebView::Property::LOAD_PROGRESS_PERCENTAGE );
  DALI_TEST_EQUALS( percentage, 0.5f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliWebViewMove(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  view.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  view.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  view.SetProperty( Actor::Property::POSITION, Vector2( 0, 0 ));
  view.SetProperty( Actor::Property::SIZE, Vector2( 800, 600 ) );

  application.GetScene().Add( view );
  application.SendNotification();
  application.Render();

  view.SetProperty( Actor::Property::POSITION, Vector2( 100, 100 ));
  Vector3 viewPos = view.GetProperty<Vector3>( Actor::Property::POSITION );
  DALI_TEST_EQUALS( viewPos, Vector3( 100, 100, 0 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliWebViewPropertyVideoHoleEnabled(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  const bool kDefaultValue = true;
  const bool kTestValue = false;

  // Check default value
  bool output;
  Property::Value value = view.GetProperty( WebView::Property::VIDEO_HOLE_ENABLED );
  DALI_TEST_CHECK( value.Get( output ) );
  DALI_TEST_EQUALS( output, kDefaultValue, TEST_LOCATION );

  // Check Set/GetProperty
  view.SetProperty( WebView::Property::VIDEO_HOLE_ENABLED, kTestValue );
  value = view.GetProperty( WebView::Property::VIDEO_HOLE_ENABLED );
  DALI_TEST_CHECK( value.Get( output ) );
  DALI_TEST_EQUALS( output, kTestValue, TEST_LOCATION );

  END_TEST;
}

int UtcDaliWebViewPropertyMouseEventsEnabled(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  const bool kDefaultValue = true;
  const bool kTestValue = false;

  // Check default value
  bool output;
  Property::Value value = view.GetProperty( WebView::Property::MOUSE_EVENTS_ENABLED );
  DALI_TEST_CHECK( value.Get( output ) );
  DALI_TEST_EQUALS( output, kDefaultValue, TEST_LOCATION );

  // Check Set/GetProperty
  view.SetProperty( WebView::Property::MOUSE_EVENTS_ENABLED, kTestValue );
  value = view.GetProperty( WebView::Property::MOUSE_EVENTS_ENABLED );
  DALI_TEST_CHECK( value.Get( output ) );
  DALI_TEST_EQUALS( output, kTestValue, TEST_LOCATION );

  END_TEST;
}

int UtcDaliWebViewPropertyKeyEventsEnabled(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  const bool kDefaultValue = true;
  const bool kTestValue = false;

  // Check default value
  bool output;
  Property::Value value = view.GetProperty( WebView::Property::KEY_EVENTS_ENABLED );
  DALI_TEST_CHECK( value.Get( output ) );
  DALI_TEST_EQUALS( output, kDefaultValue, TEST_LOCATION );

  // Check Set/GetProperty
  view.SetProperty( WebView::Property::KEY_EVENTS_ENABLED, kTestValue );
  value = view.GetProperty( WebView::Property::KEY_EVENTS_ENABLED );
  DALI_TEST_CHECK( value.Get( output ) );
  DALI_TEST_EQUALS( output, kTestValue, TEST_LOCATION );

  END_TEST;
}

int UtcDaliWebViewHoverAndWheel(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );
  view.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  view.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  view.SetProperty( Actor::Property::POSITION, Vector2( 0, 0 ));
  view.SetProperty( Actor::Property::SIZE, Vector2( 800, 600 ) );

  application.GetScene().Add( view );
  application.SendNotification();
  application.Render();

  view.GetNaturalSize();
  view.HoveredSignal().Connect( &OnHovered );
  view.WheelEventSignal().Connect( &OnWheelEvent );

  // Hover event
  Dali::Integration::HoverEvent event = Dali::Integration::HoverEvent();
  Dali::Integration::Point pointDown;
  pointDown.SetState( PointState::DOWN );
  pointDown.SetScreenPosition( Vector2( 10, 10 ) );
  event.AddPoint( pointDown );
  application.ProcessEvent( event );

  event = Dali::Integration::HoverEvent();
  Dali::Integration::Point pointUp;
  pointUp.SetState( PointState::UP );
  pointUp.SetScreenPosition( Vector2( 10, 10 ) );
  event.AddPoint( pointUp );
  application.ProcessEvent( event );

  event = Dali::Integration::HoverEvent();
  Dali::Integration::Point pointMotion;
  pointUp.SetState( PointState::MOTION );
  pointUp.SetScreenPosition( Vector2( 10, 10 ) );
  event.AddPoint( pointMotion );
  application.ProcessEvent( event );

  // Wheel event
  Dali::Integration::WheelEvent wheelEvent;
  wheelEvent.type = Dali::Integration::WheelEvent::Type::MOUSE_WHEEL;
  wheelEvent.direction = 0;
  wheelEvent.point = Vector2( 20, 20 );
  wheelEvent.delta = 10;
  application.ProcessEvent( wheelEvent );
  application.SendNotification();

  DALI_TEST_CHECK( gHovered );
  DALI_TEST_CHECK( gWheelEventHandled );

  END_TEST;
}

int UtcDaliWebViewFormRepostDecisionFrameRendering(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  view.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  view.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  view.SetProperty( Actor::Property::POSITION, Vector2( 0, 0 ));
  view.SetProperty( Actor::Property::SIZE, Vector2( 800, 600 ) );
  application.GetScene().Add( view );
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( view );

  ConnectionTracker* testTracker = new ConnectionTracker();
  view.FormRepostDecisionSignal().Connect(&OnFormRepostDecision);
  view.FrameRenderedSignal().Connect(&OnFrameRendered);
  bool signal1 = false;
  bool signal2 = false;
  view.ConnectSignal( testTracker, "formRepostDecision", CallbackFunctor(&signal1) );
  view.ConnectSignal( testTracker, "frameRendered", CallbackFunctor(&signal2) );
  DALI_TEST_EQUALS( gFormRepostDecisionCallbackCalled, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( gFrameRenderedCallbackCalled, 0, TEST_LOCATION );

  view.LoadUrl( TEST_URL1 );
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS( gFormRepostDecisionCallbackCalled, 1, TEST_LOCATION );
  DALI_TEST_EQUALS( gFrameRenderedCallbackCalled, 1, TEST_LOCATION );
  DALI_TEST_CHECK( signal1 & signal2);

  // form repost decision.
  DALI_TEST_CHECK(gFormRepostDecisionInstance);
  gFormRepostDecisionInstance->Reply(true);

  // reset
  gFormRepostDecisionInstance = nullptr;

  END_TEST;
}

int UtcDaliWebViewGetWebBackForwardList(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebBackForwardList* bfList = view.GetBackForwardList();
  DALI_TEST_CHECK( bfList != 0 );

  END_TEST;
}

int UtcDaliWebViewGetWebContext(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebContext* context = view.GetContext();
  DALI_TEST_CHECK( context != 0 );

  END_TEST;
}

int UtcDaliWebViewGetWebCookieManager(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebCookieManager* cookieManager = view.GetCookieManager();
  DALI_TEST_CHECK( cookieManager != 0 );

  END_TEST;
}

int UtcDaliWebViewGetWebSettings(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK( settings != 0 );

  END_TEST;
}

int UtcDaliWebViewProperty1(void)
{
  // URL
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  std::string local;
  view.SetProperty( WebView::Property::URL, TEST_URL1 );
  Property::Value val = view.GetProperty( WebView::Property::URL );
  DALI_TEST_CHECK( val.Get( local ) );
  DALI_TEST_EQUALS( local, TEST_URL1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliWebViewProperty4(void)
{
  // USER_AGENT
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  const std::string kDefaultValue;
  const std::string kTestValue = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/71.0.3578.98 Safari/537.36";

  // Check default value
  std::string output;
  Property::Value value = view.GetProperty( WebView::Property::USER_AGENT );
  DALI_TEST_CHECK( value.Get( output ) );
  DALI_TEST_EQUALS( output, kDefaultValue, TEST_LOCATION );

  // Check Set/GetProperty
  view.SetProperty( WebView::Property::USER_AGENT, kTestValue );
  value = view.GetProperty( WebView::Property::USER_AGENT );
  DALI_TEST_CHECK( value.Get( output ) );
  DALI_TEST_EQUALS( output, kTestValue, TEST_LOCATION );

  END_TEST;
}

int UtcDaliWebViewProperty9(void)
{
  // SCROLL_POSITION
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  // Check default value
  Dali::Vector2 output = Dali::Vector2::ONE;
  view.GetProperty( WebView::Property::SCROLL_POSITION ).Get( output );
  DALI_TEST_CHECK( output.x == 0 && output.y == 0 );

  // Check Set/GetProperty
  Dali::Vector2 testValue = Dali::Vector2( 100, 100 );
  view.SetProperty( WebView::Property::SCROLL_POSITION, testValue );
  view.GetProperty( WebView::Property::SCROLL_POSITION ).Get( output );
  DALI_TEST_EQUALS( output, testValue, TEST_LOCATION );

  // Check default value of scroll size
  output = Dali::Vector2::ONE;
  view.GetProperty( WebView::Property::SCROLL_SIZE ).Get( output );
  DALI_TEST_CHECK( output.x == 500 && output.y == 500 );

  // Check default value of content size
  output = Dali::Vector2::ONE;
  view.GetProperty( WebView::Property::CONTENT_SIZE ).Get( output );
  DALI_TEST_CHECK( output.x == 500 && output.y == 500 );

  END_TEST;
}

int UtcDaliWebViewPropertyBackgroundColorSelectedTextEtc(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Vector4 testValue = Dali::Vector4(0.0f, 0.0f, 0.0f, 0.0f);
  view.SetProperty(WebView::Property::DOCUMENT_BACKGROUND_COLOR, testValue);
  view.SetProperty(WebView::Property::TILES_CLEARED_WHEN_HIDDEN, true);
  view.SetProperty(WebView::Property::TILE_COVER_AREA_MULTIPLIER, 1.0f);
  view.SetProperty(WebView::Property::CURSOR_ENABLED_BY_CLIENT, true);

  // Check default value
  std::string testText("test");
  std::string output;
  view.GetProperty(WebView::Property::SELECTED_TEXT).Get(output);
  DALI_TEST_EQUALS(output, testText, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewPropertyTitleFavicon(void)
{
  ToolkitTestApplication application;

  char argv[] = "--test";
  WebView view = WebView::New( 1, (char**)&argv );
  DALI_TEST_CHECK( view );

  // reset something
  view.ClearAllTilesResources();

  // Check default value of title
  std::string testValue("title");
  std::string output;
  view.GetProperty( WebView::Property::TITLE ).Get( output );
  DALI_TEST_EQUALS( output, testValue, TEST_LOCATION );

  // Check default value of favicon
  Dali::Toolkit::ImageView* favicon = &view.GetFavicon();
  DALI_TEST_CHECK( favicon );
  Dali::Vector3 iconsize = favicon->GetProperty< Vector3 >( Dali::Actor::Property::SIZE );
  DALI_TEST_CHECK( ( int )iconsize.width == 2 && ( int )iconsize.height == 2 );

  END_TEST;
}

int UtcDaliWebViewScrollBy(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  // load url.
  ConnectionTracker* testTracker = new ConnectionTracker();
  view.ScrollEdgeReachedSignal().Connect( &OnScrollEdgeReached );
  bool signal1 = false;
  view.ConnectSignal( testTracker, "scrollEdgeReached", CallbackFunctor(&signal1) );
  DALI_TEST_EQUALS( gScrollEdgeReachedCallbackCalled, 0, TEST_LOCATION );

  view.LoadUrl( TEST_URL1 );
  Test::EmitGlobalTimerSignal();

  // set scroll position.
  Dali::Vector2 output = Dali::Vector2::ONE;
  Dali::Vector2 testValue = Dali::Vector2( 100, 100 );
  view.SetProperty( WebView::Property::SCROLL_POSITION, testValue );
  view.GetProperty( WebView::Property::SCROLL_POSITION ).Get( output );
  DALI_TEST_EQUALS( output, testValue, TEST_LOCATION );

  // scroll by and trigger scrollEdgeReached event.
  view.ScrollBy( 50, 50 );
  Test::EmitGlobalTimerSignal();

  view.GetProperty( WebView::Property::SCROLL_POSITION ).Get( output );
  DALI_TEST_CHECK( output.x == 150 && output.y == 150 );
  DALI_TEST_EQUALS( gScrollEdgeReachedCallbackCalled, 1, TEST_LOCATION );
  DALI_TEST_CHECK( signal1 );

  // scroll by and trigger scrollEdgeReached event.
  bool result = view.ScrollEdgeBy( 50, 50 );
  DALI_TEST_CHECK( result );
  Test::EmitGlobalTimerSignal();

  view.GetProperty( WebView::Property::SCROLL_POSITION ).Get( output );
  DALI_TEST_CHECK( output.x == 200 && output.y == 200 );
  DALI_TEST_EQUALS( gScrollEdgeReachedCallbackCalled, 2, TEST_LOCATION );
  DALI_TEST_CHECK( signal1 );

  END_TEST;
}

int UtcDaliWebViewSetGetScaleFactorActivateAccessibility(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  view.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  view.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  view.SetProperty( Actor::Property::POSITION, Vector2( 0, 0 ));
  view.SetProperty( Actor::Property::SIZE, Vector2( 800, 600 ) );

  application.GetScene().Add( view );
  application.SendNotification();
  application.Render();

  view.ActivateAccessibility(true);
  view.AddDynamicCertificatePath("host", "test/to/path");
  bool found = view.HighlightText("test", Dali::WebEnginePlugin::FindOption::CASE_INSENSITIVE, 2);
  DALI_TEST_CHECK( found );

  view.SetScaleFactor(1.5f, Dali::Vector2(0.0f, 0.0f));
  float result = view.GetScaleFactor();
  DALI_TEST_EQUALS( result, 1.5f, TEST_LOCATION );

  view.SetScaleFactor(1.0f, Dali::Vector2(0.0f, 0.0f));
  result = view.GetScaleFactor();
  DALI_TEST_EQUALS( result, 1.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliWebViewGetScreenshotSyncAndAsync(void)
{
  // SCROLL_POSITION
  ToolkitTestApplication application;

  char argv[] = "--test";
  WebView view = WebView::New( 1, (char**)&argv );
  DALI_TEST_CHECK( view );

  // Check GetScreenshot
  Dali::Rect<int> viewArea;
  viewArea.x = 100;
  viewArea.y = 100;
  viewArea.width = 10;
  viewArea.height = 10;
  Dali::Toolkit::ImageView screenshot = view.GetScreenshot(viewArea, 1.0f);
  DALI_TEST_CHECK( screenshot );
  Dali::Vector3 shotsize = screenshot.GetProperty< Vector3 >( Dali::Actor::Property::SIZE );
  DALI_TEST_CHECK( ( int )shotsize.width == viewArea.width && ( int )shotsize.height == viewArea.height );

  // Check GetScreenshotAsynchronously
  viewArea.x = 100;
  viewArea.y = 100;
  viewArea.width = 100;
  viewArea.height = 100;
  bool result = view.GetScreenshotAsynchronously(viewArea, 1.0f, &OnScreenshotCaptured);
  DALI_TEST_CHECK( result );

  Test::EmitGlobalTimerSignal();

  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS( gScreenshotCapturedCallbackCalled, 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliWebViewVideoPlayingGeolocationPermission(void)
{
  // SCROLL_POSITION
  ToolkitTestApplication application;

  char argv[] = "--test";
  WebView view = WebView::New( 1, (char**)&argv );
  DALI_TEST_CHECK( view );

  // Check CheckVideoPlayingAsynchronously
  bool result = view.CheckVideoPlayingAsynchronously(&OnVideoPlaying);
  DALI_TEST_CHECK( result );
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS( gVideoPlayingCallbackCalled, 1, TEST_LOCATION );

  // Check RegisterGeolocationPermissionCallback
  view.RegisterGeolocationPermissionCallback(&OnGeolocationPermission);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS( gGeolocationPermissionCallbackCalled, 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliWebViewHttpRequestInterceptor(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  // load url.
  ConnectionTracker* testTracker = new ConnectionTracker();
  view.RequestInterceptorSignal().Connect( &OnRequestInterceptor );
  bool signal1 = false;
  view.ConnectSignal( testTracker, "requestInterceptor", CallbackFunctor(&signal1) );
  DALI_TEST_EQUALS( gRequestInterceptorCallbackCalled, 0, TEST_LOCATION );
  DALI_TEST_CHECK(gRequestInterceptorInstance == 0);

  view.LoadUrl( TEST_URL1 );
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS( gRequestInterceptorCallbackCalled, 1, TEST_LOCATION );
  DALI_TEST_CHECK( signal1 );

  // check request interceptor.
  DALI_TEST_CHECK(gRequestInterceptorInstance != 0);
  DALI_TEST_CHECK(gRequestInterceptorInstance->Ignore());
  DALI_TEST_CHECK(gRequestInterceptorInstance->SetResponseStatus(400, "error"));
  DALI_TEST_CHECK(gRequestInterceptorInstance->AddResponseHeader("key", "value"));
  DALI_TEST_CHECK(gRequestInterceptorInstance->AddResponseBody("test", 4));
  std::string testUrl("http://test.html");
  DALI_TEST_EQUALS(gRequestInterceptorInstance->GetUrl(), testUrl, TEST_LOCATION);

  gRequestInterceptorInstance = nullptr;

  END_TEST;
}

int UtcDaliWebViewEvaluteJavaScript(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New( "ko-KR", "Asia/Seoul" );

  view.LoadHtmlString( "<body>Hello World!</body>" );
  view.EvaluateJavaScript( "jsObject.postMessage('Hello')" );
  view.EvaluateJavaScript( "jsObject.postMessage('World')", OnEvaluateJavaScript );
  Test::EmitGlobalTimerSignal();

  DALI_TEST_EQUALS( gEvaluateJavaScriptCallbackCalled, 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliWebViewJavaScriptAlertConfirmPrompt(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New( "ko-KR", "Asia/Seoul" );

  view.RegisterJavaScriptAlertCallback( &OnJavaScriptAlert );
  view.LoadHtmlString( "<head><script type='text/javascript'>alert('this is an alert popup.');</script></head><body>Hello World!</body>" );
  view.JavaScriptAlertReply();
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS( gJavaScriptAlertCallbackCalled, 1, TEST_LOCATION );

  view.RegisterJavaScriptConfirmCallback( &OnJavaScriptConfirm );
  view.LoadHtmlString( "<head><script type='text/javascript'>confirm('this is a confirm popup.');</script></head><body>Hello World!</body>" );
  view.JavaScriptConfirmReply( true );
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS( gJavaScriptConfirmCallbackCalled, 1, TEST_LOCATION );

  view.RegisterJavaScriptPromptCallback( &OnJavaScriptPrompt );
  view.LoadHtmlString( "<head><script type='text/javascript'>prompt('this is a prompt popup.');</script></head><body>Hello World!</body>" );
  view.JavaScriptPromptReply( "it is a prompt." );
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS( gJavaScriptPromptCallbackCalled, 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliWebViewLoadHtmlStringOverrideCurrentEntryAndContents(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New( "ko-KR", "Asia/Seoul" );
  DALI_TEST_CHECK( view );

  std::string html("<body>Hello World!</body>");
  std::string basicUri("http://basicurl");
  std::string unreachableUrl("http://unreachableurl");
  bool result = view.LoadHtmlStringOverrideCurrentEntry( html, basicUri, unreachableUrl );
  DALI_TEST_CHECK( result );

  application.SendNotification();
  application.Render();
  Test::EmitGlobalTimerSignal();

  result = view.LoadContents( html, html.length(), "html/text", "utf-8", basicUri );
  DALI_TEST_CHECK( result );

  END_TEST;
}

int UtcDaliWebViewReloadSuspendResumeNetworkLoadingCustomHeader(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  view.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  view.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  view.SetProperty( Actor::Property::POSITION, Vector2( 0, 0 ));
  view.SetProperty( Actor::Property::SIZE, Vector2( 800, 600 ) );

  application.GetScene().Add( view );
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( view );

  view.LoadUrl( "http://test.html" );
  bool result = view.AddCustomHeader("key", "value");
  DALI_TEST_CHECK( result );

  result = view.ReloadWithoutCache();
  DALI_TEST_CHECK( result );

  uint32_t portNumber = view.StartInspectorServer(5000);
  DALI_TEST_EQUALS( portNumber, 5000, TEST_LOCATION );

  application.SendNotification();
  application.Render();
  Test::EmitGlobalTimerSignal();

  result = view.StopInspectorServer();
  DALI_TEST_CHECK( result );

  view.SuspendNetworkLoading();

  result = view.RemoveCustomHeader("key");
  DALI_TEST_CHECK( result );

  view.ResumeNetworkLoading();

  END_TEST;
}

int UtcDaliWebViewMethodsForCoverage(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New( "ko-KR", "Asia/Seoul" );

  view.LoadHtmlString( "<body>Hello World!</body>" );
  view.AddJavaScriptMessageHandler( "jsObject",
    []( const std::string& arg ) {
    }
  );

  DALI_TEST_CHECK( view );

  END_TEST;
}

// test cases for web backforward list.

int UtcDaliWebBackForwardListCheckItem(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebBackForwardList* bfList = view.GetBackForwardList();
  DALI_TEST_CHECK( bfList != 0 )

  unsigned int itemCount = bfList->GetItemCount();
  DALI_TEST_CHECK( itemCount == 1 )

  Dali::Toolkit::WebBackForwardListItem* citem = bfList->GetCurrentItem();
  DALI_TEST_CHECK( citem != 0 );

  const std::string kDefaultUrl( "http://url" );
  std::string testValue = citem->GetUrl();
  DALI_TEST_EQUALS( testValue, kDefaultUrl, TEST_LOCATION );

  const std::string kDefaultTitle( "title" );
  testValue = citem->GetTitle();
  DALI_TEST_EQUALS( testValue, kDefaultTitle, TEST_LOCATION );

  const std::string kDefaultOriginalUrl( "http://originalurl" );
  testValue = citem->GetOriginalUrl();
  DALI_TEST_EQUALS( testValue, kDefaultOriginalUrl, TEST_LOCATION );

  Dali::Toolkit::WebBackForwardListItem* item = bfList->GetItemAtIndex( 0 );
  DALI_TEST_CHECK( item != 0 );

  END_TEST;
}

// test cases for web context.

int UtcDaliWebContextGetSetCacheModel(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebContext* context = view.GetContext();
  DALI_TEST_CHECK( context != 0 )

  std::string kDefaultValue;

  // Reset something
  context->SetProxyUri( kDefaultValue );
  context->SetCertificateFilePath( kDefaultValue );
  context->DisableCache( false );
  context->SetDefaultProxyAuth( kDefaultValue, kDefaultValue );
  context->DeleteWebDatabase();
  context->DeleteWebStorage();
  context->DeleteLocalFileSystem();
  context->ClearCache();

  // Check default value
  Dali::WebEngineContext::CacheModel value = context->GetCacheModel();
  DALI_TEST_CHECK( value == Dali::WebEngineContext::CacheModel::DOCUMENT_VIEWER );

  // Check Set/GetProperty
  context->SetCacheModel( Dali::WebEngineContext::CacheModel::DOCUMENT_BROWSER );
  value = context->GetCacheModel();
  DALI_TEST_CHECK( value == Dali::WebEngineContext::CacheModel::DOCUMENT_BROWSER );

  END_TEST;
}

// test cases for web cookie manager.

int UtcDaliWebCookieManagerGetSetCookieAcceptPolicy(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebCookieManager* cookieManager = view.GetCookieManager();
  DALI_TEST_CHECK( cookieManager != 0 )

  const std::string kDefaultValue;

  // Reset something
  cookieManager->SetPersistentStorage( kDefaultValue, Dali::WebEngineCookieManager::CookiePersistentStorage::SQLITE );
  cookieManager->ClearCookies();

  // Check default value
  Dali::WebEngineCookieManager::CookieAcceptPolicy value = cookieManager->GetCookieAcceptPolicy();
  DALI_TEST_CHECK( value == Dali::WebEngineCookieManager::CookieAcceptPolicy::NO_THIRD_PARTY );

  // Check Set/GetProperty
  cookieManager->SetCookieAcceptPolicy( Dali::WebEngineCookieManager::CookieAcceptPolicy::ALWAYS );
  value = cookieManager->GetCookieAcceptPolicy();
  DALI_TEST_CHECK( value == Dali::WebEngineCookieManager::CookieAcceptPolicy::ALWAYS );

  END_TEST;
}

// test cases for web settings.

int UtcDaliWebSettingsGetSetDefaultFontSize(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK( settings != 0 )

  // Reset something
  settings->AllowMixedContents( false );
  settings->EnableSpatialNavigation( false );
  settings->EnableWebSecurity( false );
  settings->EnableCacheBuilder( false );
  settings->EnableDoNotTrack( false );
  settings->UseScrollbarThumbFocusNotifications( false );
  settings->AllowFileAccessFromExternalUrl( false );
  settings->AllowScriptsOpenWindows( false );

  // Check default value
  int value = settings->GetDefaultFontSize();
  DALI_TEST_CHECK( value == 16 );

  // Check Set/GetProperty
  settings->SetDefaultFontSize( 20 );
  value = settings->GetDefaultFontSize();
  DALI_TEST_CHECK( value == 20 );

  END_TEST;
}

int UtcDaliWebSettingsCheckEnableJavaScript(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK( settings != 0 )

  // Reset something
  settings->AllowMixedContents( false );
  settings->EnableSpatialNavigation( false );
  settings->EnableWebSecurity( false );
  settings->EnableCacheBuilder( false );
  settings->EnableDoNotTrack( false );
  settings->UseScrollbarThumbFocusNotifications( false );
  settings->AllowFileAccessFromExternalUrl( false );
  settings->AllowScriptsOpenWindows( false );

  // Check default value is true or not
  bool value = settings->IsJavaScriptEnabled();
  DALI_TEST_CHECK( value );

  // Check Set/GetProperty
  settings->EnableJavaScript( false );
  value = settings->IsJavaScriptEnabled();
  DALI_TEST_CHECK( !value );

  END_TEST;
}

int UtcDaliWebSettingsCheckEnableAutoFitting(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK( settings != 0 )

  // Reset something
  settings->AllowMixedContents( false );
  settings->EnableSpatialNavigation( false );
  settings->EnableWebSecurity( false );
  settings->EnableCacheBuilder( false );
  settings->EnableDoNotTrack( false );
  settings->UseScrollbarThumbFocusNotifications( false );
  settings->AllowFileAccessFromExternalUrl( false );
  settings->AllowScriptsOpenWindows( false );

  // Check default value is true or not
  bool value = settings->IsAutoFittingEnabled();
  DALI_TEST_CHECK( value );

  // Check Set/GetProperty
  settings->EnableAutoFitting( false );
  value = settings->IsAutoFittingEnabled();
  DALI_TEST_CHECK( !value );

  END_TEST;
}

int UtcDaliWebSettingsCheckEnablePlugins(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK( settings != 0 )

  // Reset something
  settings->AllowMixedContents( false );
  settings->EnableSpatialNavigation( false );
  settings->EnableWebSecurity( false );
  settings->EnableCacheBuilder( false );
  settings->EnableDoNotTrack( false );
  settings->UseScrollbarThumbFocusNotifications( false );
  settings->AllowFileAccessFromExternalUrl( false );
  settings->AllowScriptsOpenWindows( false );

  // Check default value is true or not
  bool value = settings->ArePluginsEnabled();
  DALI_TEST_CHECK( value );

  // Check Set/GetProperty
  settings->EnablePlugins( false );
  value = settings->ArePluginsEnabled();
  DALI_TEST_CHECK( !value );

  END_TEST;
}

int UtcDaliWebSettingsCheckEnablePrivateBrowsing(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK( settings != 0 )

  // Reset something
  settings->AllowMixedContents( false );
  settings->EnableSpatialNavigation( false );
  settings->EnableWebSecurity( false );
  settings->EnableCacheBuilder( false );
  settings->EnableDoNotTrack( false );
  settings->UseScrollbarThumbFocusNotifications( false );
  settings->AllowFileAccessFromExternalUrl( false );
  settings->AllowScriptsOpenWindows( false );

  // Check default value is true or not
  bool value = settings->IsPrivateBrowsingEnabled();
  DALI_TEST_CHECK( value );

  // Check Set/GetProperty
  settings->EnablePrivateBrowsing( false );
  value = settings->IsPrivateBrowsingEnabled();
  DALI_TEST_CHECK( !value );

  END_TEST;
}

int UtcDaliWebSettingsCheckEnableLinkMagnifier(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK( settings != 0 )

  // Reset something
  settings->AllowMixedContents( false );
  settings->EnableSpatialNavigation( false );
  settings->EnableWebSecurity( false );
  settings->EnableCacheBuilder( false );
  settings->EnableDoNotTrack( false );
  settings->UseScrollbarThumbFocusNotifications( false );
  settings->AllowFileAccessFromExternalUrl( false );
  settings->AllowScriptsOpenWindows( false );

  // Check default value is true or not
  bool value = settings->IsLinkMagnifierEnabled();
  DALI_TEST_CHECK( value );

  // Check Set/GetProperty
  settings->EnableLinkMagnifier( false );
  value = settings->IsLinkMagnifierEnabled();
  DALI_TEST_CHECK( !value );

  END_TEST;
}

int UtcDaliWebSettingsCheckUseKeypadWithoutUserAction(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK( settings != 0 )

  // Reset something
  settings->AllowMixedContents( false );
  settings->EnableSpatialNavigation( false );
  settings->EnableWebSecurity( false );
  settings->EnableCacheBuilder( false );
  settings->EnableDoNotTrack( false );
  settings->UseScrollbarThumbFocusNotifications( false );
  settings->AllowFileAccessFromExternalUrl( false );
  settings->AllowScriptsOpenWindows( false );

  // Check default value is true or not
  bool value = settings->IsKeypadWithoutUserActionUsed();
  DALI_TEST_CHECK( value );

  // Check Set/GetProperty
  settings->UseKeypadWithoutUserAction( false );
  value = settings->IsKeypadWithoutUserActionUsed();
  DALI_TEST_CHECK( !value );

  END_TEST;
}

int UtcDaliWebSettingsCheckEnableAutofillPasswordForm(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK( settings != 0 )

  // Reset something
  settings->AllowMixedContents( false );
  settings->EnableSpatialNavigation( false );
  settings->EnableWebSecurity( false );
  settings->EnableCacheBuilder( false );
  settings->EnableDoNotTrack( false );
  settings->UseScrollbarThumbFocusNotifications( false );
  settings->AllowFileAccessFromExternalUrl( false );
  settings->AllowScriptsOpenWindows( false );

  // Check default value is true or not
  bool value = settings->IsAutofillPasswordFormEnabled();
  DALI_TEST_CHECK( value );
  settings->EnableAutofillPasswordForm( false );
  value = settings->IsAutofillPasswordFormEnabled();
  DALI_TEST_CHECK( !value );
  END_TEST;
}

int UtcDaliWebSettingsCheckEnableFormCandidateData(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK( settings != 0 );

  // Reset something
  settings->AllowMixedContents( false );
  settings->EnableSpatialNavigation( false );
  settings->EnableWebSecurity( false );
  settings->EnableCacheBuilder( false );
  settings->EnableDoNotTrack( false );
  settings->UseScrollbarThumbFocusNotifications( false );
  settings->AllowFileAccessFromExternalUrl( false );
  settings->AllowScriptsOpenWindows( false );

  // Check default value is true or not
  bool value = settings->IsFormCandidateDataEnabled();
  DALI_TEST_CHECK( value );

  // Check Set/GetProperty
  settings->EnableFormCandidateData( false );
  value = settings->IsFormCandidateDataEnabled();
  DALI_TEST_CHECK( !value );

  END_TEST;
}

int UtcDaliWebSettingsCheckEnableTextSelection(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK( settings != 0 );

  // Reset something
  settings->AllowMixedContents( false );
  settings->EnableSpatialNavigation( false );
  settings->EnableWebSecurity( false );
  settings->EnableCacheBuilder( false );
  settings->EnableDoNotTrack( false );
  settings->UseScrollbarThumbFocusNotifications( false );
  settings->AllowFileAccessFromExternalUrl( false );
  settings->AllowScriptsOpenWindows( false );

  // Check default value is true or not
  bool value = settings->IsTextSelectionEnabled();
  DALI_TEST_CHECK( value );

  //Check Set/GetProperty
  settings->EnableTextSelection(false);
  value = settings->IsTextSelectionEnabled();
  DALI_TEST_CHECK( !value );

  END_TEST;
}

int UtcDaliWebSettingsCheckEnableTextAutosizing(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK( settings != 0 );

  // Reset something
  settings->AllowMixedContents( false );
  settings->EnableSpatialNavigation( false );
  settings->EnableWebSecurity( false );
  settings->EnableCacheBuilder( false );
  settings->EnableDoNotTrack( false );
  settings->UseScrollbarThumbFocusNotifications( false );
  settings->AllowFileAccessFromExternalUrl( false );
  settings->AllowScriptsOpenWindows( false );

  // Check default value is true or not
  bool value = settings->IsTextAutosizingEnabled();
  DALI_TEST_CHECK( value );

  // Check Set/GetProperty
  settings->EnableTextAutosizing(false);
  value = settings->IsTextAutosizingEnabled();
  DALI_TEST_CHECK( !value );

  END_TEST;
}

int UtcDaliWebSettingsCheckEnableArrowScroll(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK( settings != 0 );

  // Reset something
  settings->AllowMixedContents( false );
  settings->EnableSpatialNavigation( false );
  settings->EnableWebSecurity( false );
  settings->EnableCacheBuilder( false );
  settings->EnableDoNotTrack( false );
  settings->UseScrollbarThumbFocusNotifications( false );
  settings->AllowFileAccessFromExternalUrl( false );
  settings->AllowScriptsOpenWindows( false );

  // Check default value is true or not
  bool value = settings->IsArrowScrollEnabled();
  DALI_TEST_CHECK( value );

  // Check Set/GetProperty
  settings->EnableArrowScroll(false);
  value = settings->IsArrowScrollEnabled();
  DALI_TEST_CHECK( !value );

  END_TEST;
}

int UtcDaliWebSettingsCheckEnableClipboard(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK( settings != 0 );

  // Reset something
  settings->AllowMixedContents( false );
  settings->EnableSpatialNavigation( false );
  settings->EnableWebSecurity( false );
  settings->EnableCacheBuilder( false );
  settings->EnableDoNotTrack( false );
  settings->UseScrollbarThumbFocusNotifications( false );
  settings->AllowFileAccessFromExternalUrl( false );
  settings->AllowScriptsOpenWindows( false );

  // Check default value is true or not
  bool value = settings->IsClipboardEnabled();
  DALI_TEST_CHECK( value );
  settings->EnableClipboard(false);
  value = settings->IsClipboardEnabled();
  DALI_TEST_CHECK( !value );
  END_TEST;
}

int UtcDaliWebSettingsCheckEnableImePanel(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK( settings != 0 );

  // Reset something
  settings->AllowMixedContents( false );
  settings->EnableSpatialNavigation( false );
  settings->EnableWebSecurity( false );
  settings->EnableCacheBuilder( false );
  settings->EnableDoNotTrack( false );
  settings->UseScrollbarThumbFocusNotifications( false );
  settings->AllowFileAccessFromExternalUrl( false );
  settings->AllowScriptsOpenWindows( false );

  // Check default value is true or not
  bool value = settings->IsImePanelEnabled();
  DALI_TEST_CHECK( value );

  // Check Set/GetProperty
  settings->EnableImePanel(false);
  value = settings->IsImePanelEnabled();
  DALI_TEST_CHECK( !value );

  END_TEST;
}

int UtcDaliWebSettingsCheckAllowImagesLoadAutomatically(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK( settings != 0 )

  // Reset something
  settings->AllowMixedContents( false );
  settings->EnableSpatialNavigation( false );
  settings->EnableWebSecurity( false );
  settings->EnableCacheBuilder( false );
  settings->EnableDoNotTrack( false );
  settings->UseScrollbarThumbFocusNotifications( false );
  settings->AllowFileAccessFromExternalUrl( false );
  settings->AllowScriptsOpenWindows( false );

  // Check default value is true or not
  bool value = settings->AreImagesLoadedAutomatically();
  DALI_TEST_CHECK( value );

  // Check Set/GetProperty
  settings->AllowImagesLoadAutomatically( false );
  value = settings->AreImagesLoadedAutomatically();
  DALI_TEST_CHECK( !value );

  END_TEST;
}

int UtcDaliWebSettingsGetSetDefaultTextEncodingName(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK( view );

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK( settings != 0 )

  const std::string kDefaultValue;
  const std::string kTestValue = "UTF-8";

  // Reset something
  settings->AllowMixedContents( false );
  settings->EnableSpatialNavigation( false );
  settings->EnableWebSecurity( false );
  settings->EnableCacheBuilder( false );
  settings->EnableDoNotTrack( false );
  settings->UseScrollbarThumbFocusNotifications( false );
  settings->AllowFileAccessFromExternalUrl( false );
  settings->AllowScriptsOpenWindows( false );

  // Check default value
  std::string value = settings->GetDefaultTextEncodingName();
  DALI_TEST_EQUALS( value, kDefaultValue, TEST_LOCATION );

  // Check Set/GetProperty
  settings->SetDefaultTextEncodingName( kTestValue );
  value = settings->GetDefaultTextEncodingName();
  DALI_TEST_EQUALS( value, kTestValue, TEST_LOCATION );

  END_TEST;
}

