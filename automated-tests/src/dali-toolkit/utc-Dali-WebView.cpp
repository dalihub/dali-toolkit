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
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>
#include <dali-toolkit/devel-api/controls/web-view/web-back-forward-list.h>
#include <dali-toolkit/devel-api/controls/web-view/web-back-forward-list-item.h>
#include <dali-toolkit/devel-api/controls/web-view/web-context.h>
#include <dali-toolkit/devel-api/controls/web-view/web-cookie-manager.h>
#include <dali-toolkit/devel-api/controls/web-view/web-settings.h>
#include <dali-toolkit/devel-api/controls/web-view/web-view.h>

using namespace Dali;
using namespace Toolkit;

namespace
{

const char* const TEST_URL1( "http://www.somewhere.valid1.com" );
const char* const TEST_URL2( "http://www.somewhere.valid2.com" );

static int gPageLoadStartedCallbackCalled = 0;
static int gPageLoadFinishedCallbackCalled = 0;
static int gScrollEdgeReachedCallbackCalled = 0;
static int gEvaluateJavaScriptCallbackCalled = 0;
static bool gTouched = false;

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

static void OnPageLoadFinished( WebView view, const std::string& url )
{
  gPageLoadFinishedCallbackCalled++;
}

static void OnScrollEdgeReached( WebView view, Dali::WebEnginePlugin::ScrollEdge edge )
{
  gScrollEdgeReachedCallbackCalled++;
}

static void OnPageLoadError( WebView view, const std::string& url, WebView::LoadErrorCode errorCode )
{
}

static void OnEvaluateJavaScript( const std::string& result )
{
  gEvaluateJavaScriptCallbackCalled++;
}

static bool OnTouched( Actor actor, const Dali::TouchEvent& touch )
{
  gTouched = true;
  return true;
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
  view.PageLoadFinishedSignal().Connect( &OnPageLoadFinished );
  view.PageLoadErrorSignal().Connect( &OnPageLoadError );
  bool signal1 = false;
  bool signal2 = false;
  bool signal3 = false;
  view.ConnectSignal( testTracker, "pageLoadStarted", CallbackFunctor(&signal1) );
  view.ConnectSignal( testTracker, "pageLoadFinished", CallbackFunctor(&signal2) );
  view.ConnectSignal( testTracker, "invalidname", CallbackFunctor(&signal3) );
  DALI_TEST_EQUALS( gPageLoadStartedCallbackCalled, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( gPageLoadFinishedCallbackCalled, 0, TEST_LOCATION );


  view.LoadUrl( TEST_URL1 );
  view.GetNaturalSize();
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS( view.CanGoBack(), false, TEST_LOCATION );
  DALI_TEST_EQUALS( gPageLoadStartedCallbackCalled, 1, TEST_LOCATION );
  DALI_TEST_EQUALS( gPageLoadFinishedCallbackCalled, 1, TEST_LOCATION );
  DALI_TEST_CHECK( signal1 & signal2 );
  DALI_TEST_CHECK( !signal3 );

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
  DALI_TEST_EQUALS( gPageLoadFinishedCallbackCalled, 2, TEST_LOCATION );

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

int UtcDaliWebViewPropertyVideoHole(void)
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

int UtcDaliWebViewPropertyTitleFavicon(void)
{
  // SCROLL_POSITION
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
  Dali::Toolkit::ImageView favicon = view.GetFavicon();
  DALI_TEST_CHECK( favicon );
  Dali::Vector3 iconsize = favicon.GetProperty< Vector3 >( Dali::Actor::Property::SIZE );
  DALI_TEST_CHECK( ( int )iconsize.width == 2 && ( int )iconsize.height == 2 );

  END_TEST;
}

int UtcDaliWebViewMethodsForCoverage(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New( "ko-KR", "Asia/Seoul" );
  view.SetTtsFocus(true);

  DALI_TEST_CHECK( view );

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
  view.GetProperty( WebView::Property::SCROLL_POSITION ).Get( output );
  DALI_TEST_CHECK( output.x == 150 && output.y == 150 );
  DALI_TEST_EQUALS( gScrollEdgeReachedCallbackCalled, 1, TEST_LOCATION );
  DALI_TEST_CHECK( signal1 );

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

