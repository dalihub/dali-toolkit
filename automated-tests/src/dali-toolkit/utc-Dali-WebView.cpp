/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <iostream>

#include <dali-toolkit-test-suite-utils.h>
#include "dali-toolkit-test-utils/toolkit-timer.h"

#include <dali-toolkit/devel-api/controls/web-view/web-back-forward-list.h>
#include <dali-toolkit/devel-api/controls/web-view/web-settings.h>
#include <dali-toolkit/devel-api/controls/web-view/web-view.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>
#include <dali.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-certificate.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-console-message.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-context-menu-item.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-context-menu.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-context.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-cookie-manager.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-form-repost-decision.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-frame.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-hit-test.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-http-auth-handler.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-load-error.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-policy-decision.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-request-interceptor.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-security-origin.h>
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/wheel-event-integ.h>
#include <dali/public-api/images/pixel-data.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
const char* const TEST_URL1("http://www.somewhere.valid1.com");
const char* const TEST_URL2("http://www.somewhere.valid2.com");

static int                                                                gPageLoadStartedCallbackCalled         = 0;
static int                                                                gPageLoadInProgressCallbackCalled      = 0;
static int                                                                gPageLoadFinishedCallbackCalled        = 0;
static int                                                                gPageLoadErrorCallbackCalled           = 0;
static std::unique_ptr<Dali::WebEngineLoadError>                          gPageLoadErrorInstance                 = nullptr;
static int                                                                gScrollEdgeReachedCallbackCalled       = 0;
static int                                                                gUrlChangedCallbackCalled              = 0;
static int                                                                gEvaluateJavaScriptCallbackCalled      = 0;
static int                                                                gJavaScriptAlertCallbackCalled         = 0;
static int                                                                gJavaScriptConfirmCallbackCalled       = 0;
static int                                                                gJavaScriptPromptCallbackCalled        = 0;
static int                                                                gScreenshotCapturedCallbackCalled      = 0;
static int                                                                gVideoPlayingCallbackCalled            = 0;
static int                                                                gGeolocationPermissionCallbackCalled   = 0;
static bool                                                               gTouched                               = false;
static bool                                                               gHovered                               = false;
static bool                                                               gWheelEventHandled                     = false;
static int                                                                gFormRepostDecidedCallbackCalled       = 0;
static std::unique_ptr<Dali::WebEngineFormRepostDecision>                 gFormRepostDecidedInstance             = nullptr;
static int                                                                gFrameRenderedCallbackCalled           = 0;
static int                                                                gConsoleMessageCallbackCalled          = 0;
static std::unique_ptr<Dali::WebEngineConsoleMessage>                     gConsoleMessageInstance                = nullptr;
static int                                                                gResponsePolicyDecidedCallbackCalled   = 0;
static int                                                                gNavigationPolicyDecidedCallbackCalled = 0;
static int                                                                gNewWindowCreatedCallbackCalled        = 0;
static std::unique_ptr<Dali::WebEnginePolicyDecision>                     gResponsePolicyDecisionInstance        = nullptr;
static int                                                                gCertificateConfirmCallbackCalled      = 0;
static std::unique_ptr<Dali::WebEngineCertificate>                        gCertificateConfirmInstance            = nullptr;
static int                                                                gSslCertificateChangedCallbackCalled   = 0;
static std::unique_ptr<Dali::WebEngineCertificate>                        gSslCertificateInstance                = nullptr;
static int                                                                gHttpAuthHandlerCallbackCalled         = 0;
static std::unique_ptr<Dali::WebEngineHttpAuthHandler>                    gHttpAuthInstance                      = nullptr;
static int                                                                gSecurityOriginsAcquiredCallbackCalled = 0;
static int                                                                gStorageUsageAcquiredCallbackCalled    = 0;
static int                                                                gFormPasswordsAcquiredCallbackCalled   = 0;
static int                                                                gDownloadStartedCallbackCalled         = 0;
static int                                                                gMimeOverriddenCallbackCalled          = 0;
static int                                                                gRequestInterceptedCallbackCalled      = 0;
static Dali::WebEngineRequestInterceptorPtr                               gRequestInterceptorInstance            = nullptr;
static std::vector<std::unique_ptr<Dali::WebEngineSecurityOrigin>>        gSecurityOriginList;
static std::vector<std::unique_ptr<Dali::WebEngineContext::PasswordData>> gPasswordDataList;
static int                                                                gContextMenuShownCallbackCalled         = 0;
static std::unique_ptr<Dali::WebEngineContextMenu>                        gContextMenuShownInstance               = nullptr;
static int                                                                gContextMenuHiddenCallbackCalled        = 0;
static std::unique_ptr<Dali::WebEngineContextMenu>                        gContextMenuHiddenInstance              = nullptr;
static int                                                                gHitTestCreatedCallbackCalled           = 0;
static int                                                                gCookieManagerChangsWatchCallbackCalled = 0;
static int                                                                gPlainTextReceivedCallbackCalled        = 0;

struct CallbackFunctor
{
  CallbackFunctor(bool* callbackFlag)
  : mCallbackFlag(callbackFlag)
  {
  }

  void operator()()
  {
    *mCallbackFlag = true;
  }
  bool* mCallbackFlag;
};

static void OnPageLoadStarted(const std::string& url)
{
  gPageLoadStartedCallbackCalled++;
}

static void OnPageLoadInProgress(const std::string& url)
{
  gPageLoadInProgressCallbackCalled++;
}

static void OnPageLoadFinished(const std::string& url)
{
  gPageLoadFinishedCallbackCalled++;
}

static void OnScrollEdgeReached(Dali::WebEnginePlugin::ScrollEdge edge)
{
  gScrollEdgeReachedCallbackCalled++;
}

static void OnResponsePolicyDecided(std::unique_ptr<Dali::WebEnginePolicyDecision> decision)
{
  gResponsePolicyDecidedCallbackCalled++;
  gResponsePolicyDecisionInstance = std::move(decision);
}

static void OnNavigationPolicyDecided(std::unique_ptr<Dali::WebEnginePolicyDecision> decision)
{
  gNavigationPolicyDecidedCallbackCalled++;
}

static void OnNewWindowCreated(Dali::WebEnginePlugin*& outPlugin)
{
  gNewWindowCreatedCallbackCalled++;
  WebView newView = WebView::New();
  outPlugin       = newView.GetPlugin();
}

static void OnUrlChanged(const std::string& url)
{
  gUrlChangedCallbackCalled++;
}

static bool OnHitTestCreated(std::unique_ptr<Dali::WebEngineHitTest> test)
{
  gHitTestCreatedCallbackCalled++;
  return true;
}

static bool OnPlainTextReceived(const std::string& plainText)
{
  gPlainTextReceivedCallbackCalled++;
  return true;
}

static void OnPageLoadError(std::unique_ptr<Dali::WebEngineLoadError> error)
{
  gPageLoadErrorCallbackCalled++;
  gPageLoadErrorInstance = std::move(error);
}

static void OnEvaluateJavaScript(const std::string& result)
{
  gEvaluateJavaScriptCallbackCalled++;
}

static bool OnJavaScriptAlert(const std::string& result)
{
  gJavaScriptAlertCallbackCalled++;
  return true;
}

static bool OnJavaScriptConfirm(const std::string& result)
{
  gJavaScriptConfirmCallbackCalled++;
  return true;
}

static bool OnJavaScriptPrompt(const std::string& meesage1, const std::string& message2)
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

static bool OnTouched(Actor actor, const Dali::TouchEvent& touch)
{
  gTouched = true;
  return true;
}

static void OnChangesWatch()
{
  gCookieManagerChangsWatchCallbackCalled++;
}

static bool OnHovered(Actor actor, const Dali::HoverEvent& hover)
{
  gHovered = true;
  return true;
}

static bool OnWheelEvent(Actor actor, const Dali::WheelEvent& wheel)
{
  gWheelEventHandled = true;
  return true;
}

static void OnFormRepostDecided(std::unique_ptr<Dali::WebEngineFormRepostDecision> decision)
{
  gFormRepostDecidedCallbackCalled++;
  gFormRepostDecidedInstance = std::move(decision);
}

static void OnFrameRendered()
{
  gFrameRenderedCallbackCalled++;
}

static void OnConsoleMessage(std::unique_ptr<Dali::WebEngineConsoleMessage> message)
{
  gConsoleMessageCallbackCalled++;
  gConsoleMessageInstance = std::move(message);
}

static void OnCertificateConfirm(std::unique_ptr<Dali::WebEngineCertificate> certificate)
{
  gCertificateConfirmCallbackCalled++;
  gCertificateConfirmInstance = std::move(certificate);
}

static void OnSslCertificateChanged(std::unique_ptr<Dali::WebEngineCertificate> certificate)
{
  gSslCertificateChangedCallbackCalled++;
  gSslCertificateInstance = std::move(certificate);
}

static void OnHttpAuthHandler(std::unique_ptr<Dali::WebEngineHttpAuthHandler> hander)
{
  gHttpAuthHandlerCallbackCalled++;
  gHttpAuthInstance = std::move(hander);
}

static void OnSecurityOriginsAcquired(std::vector<std::unique_ptr<Dali::WebEngineSecurityOrigin>>& origins)
{
  gSecurityOriginsAcquiredCallbackCalled++;
  gSecurityOriginList.clear();
  gSecurityOriginList.swap(origins);
}

static void OnStorageUsageAcquired(uint64_t usage)
{
  gStorageUsageAcquiredCallbackCalled++;
}

static void OnFormPasswordsAcquired(std::vector<std::unique_ptr<Dali::WebEngineContext::PasswordData>>& passwords)
{
  gFormPasswordsAcquiredCallbackCalled++;
  gPasswordDataList.clear();
  gPasswordDataList.swap(passwords);
}

static void OnDownloadStarted(const std::string& url)
{
  gDownloadStartedCallbackCalled++;
}

static bool OnMimeOverridden(const std::string&, const std::string&, std::string&)
{
  gMimeOverriddenCallbackCalled++;
  return false;
}

static bool OnRequestIntercepted(Dali::WebEngineRequestInterceptorPtr interceptor)
{
  gRequestInterceptedCallbackCalled++;
  gRequestInterceptorInstance = interceptor;
  return false;
}

static void OnContextMenuShown(std::unique_ptr<Dali::WebEngineContextMenu> menu)
{
  gContextMenuShownCallbackCalled++;
  gContextMenuShownInstance = std::move(menu);
}

static void OnContextMenuHidden(std::unique_ptr<Dali::WebEngineContextMenu> menu)
{
  gContextMenuHiddenCallbackCalled++;
  gContextMenuHiddenInstance = std::move(menu);
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
  tet_infoline("UtcDaliWebViewBasic Copy and Assignment Test");
  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  WebView copy(view);
  DALI_TEST_CHECK(view == copy);

  WebView assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == view);

  // DownCast Test
  tet_infoline("UtcDaliWebViewBasic DownCast Test");
  BaseHandle handle(view);

  WebView view2 = WebView::DownCast(handle);
  DALI_TEST_CHECK(view);
  DALI_TEST_CHECK(view2);
  DALI_TEST_CHECK(view == view2);

  // TypeRegistry Test
  tet_infoline("UtcDaliWebViewBasic TypeRegistry Test");
  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK(typeRegistry);

  TypeInfo typeInfo = typeRegistry.GetTypeInfo("WebView");
  DALI_TEST_CHECK(typeInfo);

  BaseHandle handle2 = typeInfo.CreateInstance();
  DALI_TEST_CHECK(handle2);

  WebView view3 = WebView::DownCast(handle2);
  DALI_TEST_CHECK(view3);

  END_TEST;
}

int UtcDaliWebViewPageNavigation(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION, Vector2(0, 0));
  view.SetProperty(Actor::Property::SIZE, Vector2(800, 600));
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(view);

  view.RegisterPageLoadStartedCallback(&OnPageLoadStarted);
  view.RegisterPageLoadInProgressCallback(&OnPageLoadInProgress);
  view.RegisterPageLoadFinishedCallback(&OnPageLoadFinished);
  view.RegisterUrlChangedCallback(&OnUrlChanged);
  DALI_TEST_EQUALS(gPageLoadStartedCallbackCalled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gPageLoadInProgressCallbackCalled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gPageLoadFinishedCallbackCalled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gUrlChangedCallbackCalled, 0, TEST_LOCATION);

  view.LoadUrl(TEST_URL1);
  view.GetNaturalSize();
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(view.CanGoBack(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(gPageLoadStartedCallbackCalled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gPageLoadInProgressCallbackCalled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gPageLoadFinishedCallbackCalled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gUrlChangedCallbackCalled, 1, TEST_LOCATION);

  view.LoadUrl(TEST_URL2);
  view.Suspend();
  view.SetProperty(Actor::Property::SIZE, Vector2(400, 300));
  application.SendNotification();
  application.Render();
  Test::EmitGlobalTimerSignal();
  view.Resume();
  DALI_TEST_EQUALS(view.CanGoBack(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(view.CanGoForward(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(gPageLoadStartedCallbackCalled, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(gPageLoadInProgressCallbackCalled, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(gPageLoadFinishedCallbackCalled, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(gUrlChangedCallbackCalled, 2, TEST_LOCATION);

  view.GoBack();
  Test::EmitGlobalTimerSignal();
  DALI_TEST_CHECK(!view.CanGoBack());
  DALI_TEST_CHECK(view.CanGoForward());

  view.GoForward();
  Test::EmitGlobalTimerSignal();
  DALI_TEST_CHECK(view.CanGoBack());
  DALI_TEST_CHECK(!view.CanGoForward());

  view.Reload();
  view.StopLoading();
  view.ClearHistory();
  Test::EmitGlobalTimerSignal();
  DALI_TEST_CHECK(!view.CanGoBack());
  DALI_TEST_CHECK(!view.CanGoForward());

  END_TEST;
}

int UtcDaliWebViewPageLoadErrorConsoleMessage(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION, Vector2(0, 0));
  view.SetProperty(Actor::Property::SIZE, Vector2(800, 600));
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(view);

  view.RegisterPageLoadErrorCallback(&OnPageLoadError);
  view.RegisterConsoleMessageReceivedCallback(&OnConsoleMessage);
  DALI_TEST_EQUALS(gPageLoadErrorCallbackCalled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gConsoleMessageCallbackCalled, 0, TEST_LOCATION);

  view.LoadUrl(TEST_URL1);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gPageLoadErrorCallbackCalled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gConsoleMessageCallbackCalled, 1, TEST_LOCATION);

  // error code.
  DALI_TEST_CHECK(gPageLoadErrorInstance);
  DALI_TEST_EQUALS(gPageLoadErrorInstance->GetUrl(), TEST_URL1, TEST_LOCATION);
  DALI_TEST_EQUALS(gPageLoadErrorInstance->GetCode(), Dali::WebEngineLoadError::ErrorCode::UNKNOWN, TEST_LOCATION);
  std::string testErrorDescription("This is an error.");
  DALI_TEST_EQUALS(gPageLoadErrorInstance->GetDescription(), testErrorDescription, TEST_LOCATION);
  DALI_TEST_EQUALS(gPageLoadErrorInstance->GetType(), Dali::WebEngineLoadError::ErrorType::NONE, TEST_LOCATION);

  // console message.
  DALI_TEST_CHECK(gConsoleMessageInstance);
  std::string testConsoleSource("source");
  DALI_TEST_EQUALS(gConsoleMessageInstance->GetSource(), testConsoleSource, TEST_LOCATION);
  DALI_TEST_EQUALS(gConsoleMessageInstance->GetLine(), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(gConsoleMessageInstance->GetSeverityLevel(), Dali::WebEngineConsoleMessage::SeverityLevel::EMPTY, TEST_LOCATION);
  std::string testConsoleText("This is a text.");
  DALI_TEST_EQUALS(gConsoleMessageInstance->GetText(), testConsoleText, TEST_LOCATION);

  // reset
  gPageLoadErrorInstance  = nullptr;
  gConsoleMessageInstance = nullptr;

  END_TEST;
}

int UtcDaliWebViewTouchAndKeys(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION, Vector2(0, 0));
  view.SetProperty(Actor::Property::SIZE, Vector2(800, 600));

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  view.GetNaturalSize();
  view.TouchedSignal().Connect(&OnTouched);

  // Touch event
  Dali::Integration::TouchEvent event;
  Dali::Integration::Point      pointDown, pointUp;

  event = Dali::Integration::TouchEvent();
  pointDown.SetState(PointState::DOWN);
  pointDown.SetScreenPosition(Vector2(10, 10));
  event.AddPoint(pointDown);
  application.ProcessEvent(event);

  event = Dali::Integration::TouchEvent();
  pointUp.SetState(PointState::UP);
  pointUp.SetScreenPosition(Vector2(10, 10));
  event.AddPoint(pointUp);
  application.ProcessEvent(event);

  // Key event
  Toolkit::KeyboardFocusManager::Get().SetCurrentFocusActor(view);
  application.ProcessEvent(Integration::KeyEvent("", "", "", DALI_KEY_ESCAPE, 0, 0, Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();

  DALI_TEST_CHECK(gTouched);
  DALI_TEST_CHECK(view);

  END_TEST;
}

int UtcDaliWebViewFocusGainedAndLost(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION, Vector2(0, 0));
  view.SetProperty(Actor::Property::SIZE, Vector2(800, 600));

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  view.SetKeyInputFocus();
  DALI_TEST_CHECK(view.HasKeyInputFocus());

  // reset
  view.ClearKeyInputFocus();
  DALI_TEST_CHECK(!view.HasKeyInputFocus());

  END_TEST;
}

int UtcDaliWebViewPropertyPageZoomFactor(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION, Vector2(0, 0));
  view.SetProperty(Actor::Property::SIZE, Vector2(800, 600));

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  view.SetProperty(WebView::Property::PAGE_ZOOM_FACTOR, 1.5f);
  float zoomFactor = view.GetProperty<float>(WebView::Property::PAGE_ZOOM_FACTOR);
  DALI_TEST_EQUALS(zoomFactor, 1.5f, TEST_LOCATION);

  view.SetProperty(WebView::Property::PAGE_ZOOM_FACTOR, 1.0f);
  zoomFactor = view.GetProperty<float>(WebView::Property::PAGE_ZOOM_FACTOR);
  DALI_TEST_EQUALS(zoomFactor, 1.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewPropertyTextZoomFactor(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION, Vector2(0, 0));
  view.SetProperty(Actor::Property::SIZE, Vector2(800, 600));

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  view.SetProperty(WebView::Property::TEXT_ZOOM_FACTOR, 1.5f);
  float zoomFactor = view.GetProperty<float>(WebView::Property::TEXT_ZOOM_FACTOR);
  DALI_TEST_EQUALS(zoomFactor, 1.5f, TEST_LOCATION);

  view.SetProperty(WebView::Property::TEXT_ZOOM_FACTOR, 1.0f);
  zoomFactor = view.GetProperty<float>(WebView::Property::TEXT_ZOOM_FACTOR);
  DALI_TEST_EQUALS(zoomFactor, 1.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewPropertyLoadProgressPercentage(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION, Vector2(0, 0));
  view.SetProperty(Actor::Property::SIZE, Vector2(800, 600));

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  float percentage = view.GetProperty<float>(WebView::Property::LOAD_PROGRESS_PERCENTAGE);
  DALI_TEST_EQUALS(percentage, 0.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewMove(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION, Vector2(0, 0));
  view.SetProperty(Actor::Property::SIZE, Vector2(800, 600));

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  view.SetProperty(Actor::Property::POSITION, Vector2(100, 100));
  Vector3 viewPos = view.GetProperty<Vector3>(Actor::Property::POSITION);
  DALI_TEST_EQUALS(viewPos, Vector3(100, 100, 0), TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewPropertyVideoHoleEnabled(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  const bool kDefaultValue = false;
  const bool kTestValue    = true;

  // Check default value
  bool            output;
  Property::Value value = view.GetProperty(WebView::Property::VIDEO_HOLE_ENABLED);
  DALI_TEST_CHECK(value.Get(output));
  DALI_TEST_EQUALS(output, kDefaultValue, TEST_LOCATION);

  // Check Set/GetProperty
  view.SetProperty(WebView::Property::VIDEO_HOLE_ENABLED, kTestValue);
  value = view.GetProperty(WebView::Property::VIDEO_HOLE_ENABLED);
  DALI_TEST_CHECK(value.Get(output));
  DALI_TEST_EQUALS(output, kTestValue, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewPropertyMouseEventsEnabled(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  const bool kDefaultValue = true;
  const bool kTestValue    = false;

  // Check default value
  bool            output;
  Property::Value value = view.GetProperty(WebView::Property::MOUSE_EVENTS_ENABLED);
  DALI_TEST_CHECK(value.Get(output));
  DALI_TEST_EQUALS(output, kDefaultValue, TEST_LOCATION);

  // Check Set/GetProperty
  view.SetProperty(WebView::Property::MOUSE_EVENTS_ENABLED, kTestValue);
  value = view.GetProperty(WebView::Property::MOUSE_EVENTS_ENABLED);
  DALI_TEST_CHECK(value.Get(output));
  DALI_TEST_EQUALS(output, kTestValue, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewPropertyKeyEventsEnabled(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  const bool kDefaultValue = true;
  const bool kTestValue    = false;

  // Check default value
  bool            output;
  Property::Value value = view.GetProperty(WebView::Property::KEY_EVENTS_ENABLED);
  DALI_TEST_CHECK(value.Get(output));
  DALI_TEST_EQUALS(output, kDefaultValue, TEST_LOCATION);

  // Check Set/GetProperty
  view.SetProperty(WebView::Property::KEY_EVENTS_ENABLED, kTestValue);
  value = view.GetProperty(WebView::Property::KEY_EVENTS_ENABLED);
  DALI_TEST_CHECK(value.Get(output));
  DALI_TEST_EQUALS(output, kTestValue, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewHoverAndWheel(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION, Vector2(0, 0));
  view.SetProperty(Actor::Property::SIZE, Vector2(800, 600));

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  view.GetNaturalSize();
  view.HoveredSignal().Connect(&OnHovered);
  view.WheelEventSignal().Connect(&OnWheelEvent);

  // Hover event
  Dali::Integration::HoverEvent event = Dali::Integration::HoverEvent();
  Dali::Integration::Point      pointDown;
  pointDown.SetState(PointState::DOWN);
  pointDown.SetScreenPosition(Vector2(10, 10));
  event.AddPoint(pointDown);
  application.ProcessEvent(event);

  event = Dali::Integration::HoverEvent();
  Dali::Integration::Point pointUp;
  pointUp.SetState(PointState::UP);
  pointUp.SetScreenPosition(Vector2(10, 10));
  event.AddPoint(pointUp);
  application.ProcessEvent(event);

  event = Dali::Integration::HoverEvent();
  Dali::Integration::Point pointMotion;
  pointUp.SetState(PointState::MOTION);
  pointUp.SetScreenPosition(Vector2(10, 10));
  event.AddPoint(pointMotion);
  application.ProcessEvent(event);

  // Wheel event
  Dali::Integration::WheelEvent wheelEvent;
  wheelEvent.type      = Dali::Integration::WheelEvent::Type::MOUSE_WHEEL;
  wheelEvent.direction = 0;
  wheelEvent.point     = Vector2(20, 20);
  wheelEvent.delta     = 10;
  application.ProcessEvent(wheelEvent);
  application.SendNotification();

  DALI_TEST_CHECK(gHovered);
  DALI_TEST_CHECK(gWheelEventHandled);

  END_TEST;
}

int UtcDaliWebViewFormRepostDecidedFrameRendering(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION, Vector2(0, 0));
  view.SetProperty(Actor::Property::SIZE, Vector2(800, 600));
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(view);

  view.RegisterFormRepostDecidedCallback(&OnFormRepostDecided);
  view.RegisterFrameRenderedCallback(&OnFrameRendered);
  DALI_TEST_EQUALS(gFormRepostDecidedCallbackCalled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gFrameRenderedCallbackCalled, 0, TEST_LOCATION);

  view.LoadUrl(TEST_URL1);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gFormRepostDecidedCallbackCalled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gFrameRenderedCallbackCalled, 1, TEST_LOCATION);

  // form repost decision.
  DALI_TEST_CHECK(gFormRepostDecidedInstance);
  gFormRepostDecidedInstance->Reply(true);

  // reset
  gFormRepostDecidedInstance = nullptr;

  END_TEST;
}

int UtcDaliWebViewSslCertificateHttpAuthentication(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION, Vector2(0, 0));
  view.SetProperty(Actor::Property::SIZE, Vector2(800, 600));
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(view);

  view.RegisterCertificateConfirmedCallback(&OnCertificateConfirm);
  view.RegisterSslCertificateChangedCallback(&OnSslCertificateChanged);
  view.RegisterHttpAuthHandlerCallback(&OnHttpAuthHandler);
  DALI_TEST_EQUALS(gCertificateConfirmCallbackCalled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gSslCertificateChangedCallbackCalled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gHttpAuthHandlerCallbackCalled, 0, TEST_LOCATION);

  view.LoadUrl(TEST_URL1);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gCertificateConfirmCallbackCalled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gSslCertificateChangedCallbackCalled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gHttpAuthHandlerCallbackCalled, 1, TEST_LOCATION);

  // certificate.
  DALI_TEST_CHECK(gCertificateConfirmInstance);
  gCertificateConfirmInstance->Allow(true);
  DALI_TEST_CHECK(gCertificateConfirmInstance->IsFromMainFrame());

  DALI_TEST_CHECK(gSslCertificateInstance);
  DALI_TEST_EQUALS(gSslCertificateInstance->GetPem(), "abc", TEST_LOCATION);
  DALI_TEST_CHECK(gSslCertificateInstance->IsContextSecure());

  // http authentication.
  DALI_TEST_CHECK(gHttpAuthInstance);
  gHttpAuthInstance->Suspend();
  gHttpAuthInstance->UseCredential("", "");
  gHttpAuthInstance->CancelCredential();
  DALI_TEST_EQUALS(gHttpAuthInstance->GetRealm(), "test", TEST_LOCATION);

  // reset
  gCertificateConfirmInstance = nullptr;
  gSslCertificateInstance     = nullptr;
  gHttpAuthInstance           = nullptr;

  END_TEST;
}

int UtcDaliWebViewGetWebBackForwardList(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebBackForwardList* bfList = view.GetBackForwardList();
  DALI_TEST_CHECK(bfList != 0);

  END_TEST;
}

int UtcDaliWebViewGetWebContext(void)
{
  ToolkitTestApplication application;

  Dali::WebEngineContext* context = WebView::GetContext();
  DALI_TEST_CHECK(context != nullptr);

  END_TEST;
}

int UtcDaliWebViewGetWebCookieManager(void)
{
  ToolkitTestApplication application;

  Dali::WebEngineCookieManager* cookieManager = WebView::GetCookieManager();
  DALI_TEST_CHECK(cookieManager != 0);

  END_TEST;
}

int UtcDaliWebViewGetWebSettings(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0);

  END_TEST;
}

int UtcDaliWebViewProperty1(void)
{
  // URL
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  std::string local;
  view.SetProperty(WebView::Property::URL, TEST_URL1);
  Property::Value val = view.GetProperty(WebView::Property::URL);
  DALI_TEST_CHECK(val.Get(local));
  DALI_TEST_EQUALS(local, TEST_URL1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewProperty4(void)
{
  // USER_AGENT
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  const std::string kDefaultValue;
  const std::string kTestValue = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/71.0.3578.98 Safari/537.36";

  // Check default value
  std::string     output;
  Property::Value value = view.GetProperty(WebView::Property::USER_AGENT);
  DALI_TEST_CHECK(value.Get(output));
  DALI_TEST_EQUALS(output, kDefaultValue, TEST_LOCATION);

  // Check Set/GetProperty
  view.SetProperty(WebView::Property::USER_AGENT, kTestValue);
  value = view.GetProperty(WebView::Property::USER_AGENT);
  DALI_TEST_CHECK(value.Get(output));
  DALI_TEST_EQUALS(output, kTestValue, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewProperty9(void)
{
  // SCROLL_POSITION
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  // Check default value
  Dali::Vector2 output = Dali::Vector2::ONE;
  view.GetProperty(WebView::Property::SCROLL_POSITION).Get(output);
  DALI_TEST_CHECK(output.x == 0 && output.y == 0);

  // Check Set/GetProperty
  Dali::Vector2 testValue = Dali::Vector2(100, 100);
  view.SetProperty(WebView::Property::SCROLL_POSITION, testValue);
  view.GetProperty(WebView::Property::SCROLL_POSITION).Get(output);
  DALI_TEST_EQUALS(output, testValue, TEST_LOCATION);

  // Check default value of scroll size
  output = Dali::Vector2::ONE;
  view.GetProperty(WebView::Property::SCROLL_SIZE).Get(output);
  DALI_TEST_CHECK(output.x == 500 && output.y == 500);

  // Check default value of content size
  output = Dali::Vector2::ONE;
  view.GetProperty(WebView::Property::CONTENT_SIZE).Get(output);
  DALI_TEST_CHECK(output.x == 500 && output.y == 500);

  END_TEST;
}

int UtcDaliWebViewPropertyBackgroundColorSelectedTextEtc(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

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

  char    argv[] = "--test";
  WebView view   = WebView::New(1, (char**)&argv);
  DALI_TEST_CHECK(view);

  // reset something
  view.ClearAllTilesResources();

  // Check default value of title
  std::string testValue("title");
  std::string output;
  view.GetProperty(WebView::Property::TITLE).Get(output);
  DALI_TEST_EQUALS(output, testValue, TEST_LOCATION);

  // Check the case that favicon is not null.
  Dali::Toolkit::ImageView favicon = view.GetFavicon();
  DALI_TEST_CHECK(favicon);
  Dali::Vector3 iconsize = favicon.GetProperty<Vector3>(Dali::Actor::Property::SIZE);
  DALI_TEST_CHECK((int)iconsize.width == 2 && (int)iconsize.height == 2);

  // Check the case that favicon is null.
  favicon = view.GetFavicon();
  DALI_TEST_CHECK(!favicon);

  END_TEST;
}

int UtcDaliWebViewContextMenuShownAndHidden(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  // load url.
  view.RegisterContextMenuShownCallback(&OnContextMenuShown);
  view.RegisterContextMenuHiddenCallback(&OnContextMenuHidden);
  DALI_TEST_EQUALS(gContextMenuShownCallbackCalled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gContextMenuHiddenCallbackCalled, 0, TEST_LOCATION);
  DALI_TEST_CHECK(gContextMenuShownInstance == 0);
  DALI_TEST_CHECK(gContextMenuHiddenInstance == 0);

  view.LoadUrl(TEST_URL1);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gContextMenuShownCallbackCalled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gContextMenuHiddenCallbackCalled, 1, TEST_LOCATION);

  // check context meun & its items.
  DALI_TEST_CHECK(gContextMenuShownInstance != 0);
  std::unique_ptr<Dali::WebEngineContextMenuItem> item = gContextMenuShownInstance->GetItemAt(0);
  DALI_TEST_CHECK(item.get() != 0);
  std::vector<std::unique_ptr<Dali::WebEngineContextMenuItem>> itemList = gContextMenuShownInstance->GetItemList();
  DALI_TEST_CHECK(itemList.size() == 1);
  DALI_TEST_CHECK(gContextMenuShownInstance->RemoveItem(*(item.get())));
  DALI_TEST_CHECK(gContextMenuShownInstance->AppendItemAsAction(WebEngineContextMenuItem::ItemTag::NO_ACTION, "", false));
  DALI_TEST_CHECK(gContextMenuShownInstance->AppendItem(WebEngineContextMenuItem::ItemTag::NO_ACTION, "", "", false));
  DALI_TEST_CHECK(gContextMenuShownInstance->SelectItem(*(item.get())));
  DALI_TEST_CHECK(gContextMenuShownInstance->Hide());

  Dali::WebEngineContextMenuItem::ItemTag testItemTag = Dali::WebEngineContextMenuItem::ItemTag::NO_ACTION;
  DALI_TEST_EQUALS(item->GetTag(), testItemTag, TEST_LOCATION);
  Dali::WebEngineContextMenuItem::ItemType testItemType = Dali::WebEngineContextMenuItem::ItemType::ACTION;
  DALI_TEST_EQUALS(item->GetType(), testItemType, TEST_LOCATION);
  DALI_TEST_CHECK(item->IsEnabled());
  std::string testLinkUrl("http://test.html");
  DALI_TEST_EQUALS(item->GetLinkUrl(), testLinkUrl, TEST_LOCATION);
  std::string testImageUrl("http://test.jpg");
  DALI_TEST_EQUALS(item->GetImageUrl(), testImageUrl, TEST_LOCATION);
  std::string testTitle("title");
  DALI_TEST_EQUALS(item->GetTitle(), testTitle, TEST_LOCATION);
  DALI_TEST_CHECK(item->GetParentMenu().get() == 0);

  DALI_TEST_CHECK(gContextMenuHiddenInstance != 0);

  gContextMenuShownInstance  = nullptr;
  gContextMenuHiddenInstance = nullptr;

  END_TEST;
}

int UtcDaliWebViewScrollBy(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  // load url.
  view.RegisterScrollEdgeReachedCallback(&OnScrollEdgeReached);
  DALI_TEST_EQUALS(gScrollEdgeReachedCallbackCalled, 0, TEST_LOCATION);

  view.LoadUrl(TEST_URL1);
  Test::EmitGlobalTimerSignal();

  // set scroll position.
  Dali::Vector2 output    = Dali::Vector2::ONE;
  Dali::Vector2 testValue = Dali::Vector2(100, 100);
  view.SetProperty(WebView::Property::SCROLL_POSITION, testValue);
  view.GetProperty(WebView::Property::SCROLL_POSITION).Get(output);
  DALI_TEST_EQUALS(output, testValue, TEST_LOCATION);

  // scroll by and trigger scrollEdgeReached event.
  view.ScrollBy(50, 50);
  Test::EmitGlobalTimerSignal();

  view.GetProperty(WebView::Property::SCROLL_POSITION).Get(output);
  DALI_TEST_CHECK(output.x == 150 && output.y == 150);
  DALI_TEST_EQUALS(gScrollEdgeReachedCallbackCalled, 1, TEST_LOCATION);

  // scroll by and trigger scrollEdgeReached event.
  bool result = view.ScrollEdgeBy(50, 50);
  DALI_TEST_CHECK(result);
  Test::EmitGlobalTimerSignal();

  view.GetProperty(WebView::Property::SCROLL_POSITION).Get(output);
  DALI_TEST_CHECK(output.x == 200 && output.y == 200);
  DALI_TEST_EQUALS(gScrollEdgeReachedCallbackCalled, 2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewSetGetScaleFactorActivateAccessibility(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION, Vector2(0, 0));
  view.SetProperty(Actor::Property::SIZE, Vector2(800, 600));

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  view.ActivateAccessibility(true);
  view.AddDynamicCertificatePath("host", "test/to/path");
  bool found = view.HighlightText("test", Dali::WebEnginePlugin::FindOption::CASE_INSENSITIVE, 2);
  DALI_TEST_CHECK(found);

  view.SetScaleFactor(1.5f, Dali::Vector2(0.0f, 0.0f));
  float result = view.GetScaleFactor();
  DALI_TEST_EQUALS(result, 1.5f, TEST_LOCATION);

  view.SetScaleFactor(1.0f, Dali::Vector2(0.0f, 0.0f));
  result = view.GetScaleFactor();
  DALI_TEST_EQUALS(result, 1.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewGetScreenshotSyncAndAsync(void)
{
  // SCROLL_POSITION
  ToolkitTestApplication application;

  char    argv[] = "--test";
  WebView view   = WebView::New(1, (char**)&argv);
  DALI_TEST_CHECK(view);

  // Check GetScreenshot
  Dali::Rect<int> viewArea;
  viewArea.x                          = 100;
  viewArea.y                          = 100;
  viewArea.width                      = 10;
  viewArea.height                     = 10;
  Dali::Toolkit::ImageView screenshot = view.GetScreenshot(viewArea, 1.0f);
  DALI_TEST_CHECK(screenshot);
  Dali::Vector3 shotsize = screenshot.GetProperty<Vector3>(Dali::Actor::Property::SIZE);
  DALI_TEST_CHECK((int)shotsize.width == viewArea.width && (int)shotsize.height == viewArea.height);

  // Check GetScreenshotAsynchronously
  viewArea.x      = 100;
  viewArea.y      = 100;
  viewArea.width  = 100;
  viewArea.height = 100;
  bool result     = view.GetScreenshotAsynchronously(viewArea, 1.0f, &OnScreenshotCaptured);
  DALI_TEST_CHECK(result);

  Test::EmitGlobalTimerSignal();

  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gScreenshotCapturedCallbackCalled, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewVideoPlayingGeolocationPermission(void)
{
  // SCROLL_POSITION
  ToolkitTestApplication application;

  char    argv[] = "--test";
  WebView view   = WebView::New(1, (char**)&argv);
  DALI_TEST_CHECK(view);

  // Check CheckVideoPlayingAsynchronously
  bool result = view.CheckVideoPlayingAsynchronously(&OnVideoPlaying);
  DALI_TEST_CHECK(result);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gVideoPlayingCallbackCalled, 1, TEST_LOCATION);

  // Check RegisterGeolocationPermissionCallback
  view.RegisterGeolocationPermissionCallback(&OnGeolocationPermission);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gGeolocationPermissionCallbackCalled, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewResponsePolicyDecisionRequest(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  // load url.
  view.RegisterResponsePolicyDecidedCallback(&OnResponsePolicyDecided);
  DALI_TEST_EQUALS(gResponsePolicyDecidedCallbackCalled, 0, TEST_LOCATION);
  DALI_TEST_CHECK(gResponsePolicyDecisionInstance == 0);

  view.LoadUrl(TEST_URL1);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gResponsePolicyDecidedCallbackCalled, 1, TEST_LOCATION);

  // check response policy decision & its frame.
  DALI_TEST_CHECK(gResponsePolicyDecisionInstance != 0);
  std::string testUrl("http://test.html");
  DALI_TEST_EQUALS(gResponsePolicyDecisionInstance->GetUrl(), testUrl, TEST_LOCATION);
  std::string testCookie("test:abc");
  DALI_TEST_EQUALS(gResponsePolicyDecisionInstance->GetCookie(), testCookie, TEST_LOCATION);
  Dali::WebEnginePolicyDecision::DecisionType testDecisionType = Dali::WebEnginePolicyDecision::DecisionType::USE;
  DALI_TEST_EQUALS(gResponsePolicyDecisionInstance->GetDecisionType(), testDecisionType, TEST_LOCATION);
  std::string testResponseMime("txt/xml");
  DALI_TEST_EQUALS(gResponsePolicyDecisionInstance->GetResponseMime(), testResponseMime, TEST_LOCATION);
  int32_t ResponseStatusCode = 500;
  DALI_TEST_EQUALS(gResponsePolicyDecisionInstance->GetResponseStatusCode(), ResponseStatusCode, TEST_LOCATION);
  Dali::WebEnginePolicyDecision::NavigationType testNavigationType = Dali::WebEnginePolicyDecision::NavigationType::LINK_CLICKED;
  DALI_TEST_EQUALS(gResponsePolicyDecisionInstance->GetNavigationType(), testNavigationType, TEST_LOCATION);
  std::string testScheme("test");
  DALI_TEST_EQUALS(gResponsePolicyDecisionInstance->GetScheme(), testScheme, TEST_LOCATION);
  DALI_TEST_CHECK(gResponsePolicyDecisionInstance->Use());
  DALI_TEST_CHECK(gResponsePolicyDecisionInstance->Ignore());
  DALI_TEST_CHECK(gResponsePolicyDecisionInstance->Suspend());

  Dali::WebEngineFrame* webFrame = &(gResponsePolicyDecisionInstance->GetFrame());
  DALI_TEST_CHECK(webFrame);
  DALI_TEST_CHECK(webFrame->IsMainFrame());

  gResponsePolicyDecisionInstance = nullptr;

  END_TEST;
}

int UtcDaliWebViewNavigationPolicyDecisionRequest(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  // load url.
  view.RegisterNavigationPolicyDecidedCallback(&OnNavigationPolicyDecided);
  DALI_TEST_EQUALS(gNavigationPolicyDecidedCallbackCalled, 0, TEST_LOCATION);

  view.LoadUrl(TEST_URL1);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gNavigationPolicyDecidedCallbackCalled, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewNewWindowCreated(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  // load url.
  view.RegisterNewWindowCreatedCallback(&OnNewWindowCreated);
  DALI_TEST_EQUALS(gNewWindowCreatedCallbackCalled, 0, TEST_LOCATION);

  view.LoadUrl(TEST_URL1);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gNewWindowCreatedCallbackCalled, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewHitTest(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  // load url.
  view.LoadUrl(TEST_URL1);

  // sync hit test.
  std::unique_ptr<Dali::WebEngineHitTest> hitTest = view.CreateHitTest(100, 100, Dali::WebEngineHitTest::HitTestMode::DEFAULT);
  DALI_TEST_CHECK(hitTest != 0);
  DALI_TEST_EQUALS(hitTest->GetResultContext(), Dali::WebEngineHitTest::ResultContext::DOCUMENT, TEST_LOCATION);
  std::string testLinkUri("http://test.html");
  DALI_TEST_EQUALS(hitTest->GetLinkUri(), testLinkUri, TEST_LOCATION);
  std::string testLinkTitle("test");
  DALI_TEST_EQUALS(hitTest->GetLinkTitle(), testLinkTitle, TEST_LOCATION);
  std::string testLinkLabel("label");
  DALI_TEST_EQUALS(hitTest->GetLinkLabel(), testLinkLabel, TEST_LOCATION);
  std::string testImageUri("http://test.jpg");
  DALI_TEST_EQUALS(hitTest->GetImageUri(), testImageUri, TEST_LOCATION);
  std::string testMediaUri("http://test.mp4");
  DALI_TEST_EQUALS(hitTest->GetMediaUri(), testMediaUri, TEST_LOCATION);
  std::string testTagName("img");
  DALI_TEST_EQUALS(hitTest->GetTagName(), testTagName, TEST_LOCATION);
  std::string testNodeValue("test");
  DALI_TEST_EQUALS(hitTest->GetNodeValue(), testNodeValue, TEST_LOCATION);
  Dali::Property::Map testMap = hitTest->GetAttributes();
  DALI_TEST_EQUALS(testMap.Count(), 0, TEST_LOCATION);
  std::string testImageFileNameExtension("jpg");
  DALI_TEST_EQUALS(hitTest->GetImageFileNameExtension(), testImageFileNameExtension, TEST_LOCATION);
  Dali::PixelData testImageBuffer = hitTest->GetImageBuffer();
  DALI_TEST_CHECK((int)testImageBuffer.GetWidth() == 2 && (int)testImageBuffer.GetHeight() == 2);

  // async...
  bool result = view.CreateHitTestAsynchronously(100, 100, Dali::WebEngineHitTest::HitTestMode::DEFAULT, &OnHitTestCreated);
  DALI_TEST_CHECK(result);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gHitTestCreatedCallbackCalled, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewEvaluteJavaScript(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New("ko-KR", "Asia/Seoul");

  view.LoadHtmlString("<body>Hello World!</body>");
  view.EvaluateJavaScript("jsObject.postMessage('Hello')");
  view.EvaluateJavaScript("jsObject.postMessage('World')", OnEvaluateJavaScript);
  Test::EmitGlobalTimerSignal();

  DALI_TEST_EQUALS(gEvaluateJavaScriptCallbackCalled, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewJavaScriptAlertConfirmPrompt(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New("ko-KR", "Asia/Seoul");

  view.RegisterJavaScriptAlertCallback(&OnJavaScriptAlert);
  view.LoadHtmlString("<head><script type='text/javascript'>alert('this is an alert popup.');</script></head><body>Hello World!</body>");
  view.JavaScriptAlertReply();
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gJavaScriptAlertCallbackCalled, 1, TEST_LOCATION);

  view.RegisterJavaScriptConfirmCallback(&OnJavaScriptConfirm);
  view.LoadHtmlString("<head><script type='text/javascript'>confirm('this is a confirm popup.');</script></head><body>Hello World!</body>");
  view.JavaScriptConfirmReply(true);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gJavaScriptConfirmCallbackCalled, 1, TEST_LOCATION);

  view.RegisterJavaScriptPromptCallback(&OnJavaScriptPrompt);
  view.LoadHtmlString("<head><script type='text/javascript'>prompt('this is a prompt popup.');</script></head><body>Hello World!</body>");
  view.JavaScriptPromptReply("it is a prompt.");
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gJavaScriptPromptCallbackCalled, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewLoadHtmlStringOverrideCurrentEntryAndContents(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New("ko-KR", "Asia/Seoul");
  DALI_TEST_CHECK(view);

  std::string html("<body>Hello World!</body>");
  std::string basicUri("http://basicurl");
  std::string unreachableUrl("http://unreachableurl");
  bool        result = view.LoadHtmlStringOverrideCurrentEntry(html, basicUri, unreachableUrl);
  DALI_TEST_CHECK(result);

  application.SendNotification();
  application.Render();
  Test::EmitGlobalTimerSignal();

  result = view.LoadContents((const int8_t*)html.c_str(), html.length(), "html/text", "utf-8", basicUri);
  DALI_TEST_CHECK(result);

  END_TEST;
}

int UtcDaliWebViewReloadSuspendResumeNetworkLoadingCustomHeader(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION, Vector2(0, 0));
  view.SetProperty(Actor::Property::SIZE, Vector2(800, 600));

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(view);

  view.LoadUrl("http://test.html");
  bool result = view.AddCustomHeader("key", "value");
  DALI_TEST_CHECK(result);

  result = view.ReloadWithoutCache();
  DALI_TEST_CHECK(result);

  uint32_t portNumber = view.StartInspectorServer(5000);
  DALI_TEST_EQUALS(portNumber, 5000, TEST_LOCATION);

  application.SendNotification();
  application.Render();
  Test::EmitGlobalTimerSignal();

  result = view.StopInspectorServer();
  DALI_TEST_CHECK(result);

  view.SuspendNetworkLoading();

  result = view.RemoveCustomHeader("key");
  DALI_TEST_CHECK(result);

  view.ResumeNetworkLoading();

  END_TEST;
}

int UtcDaliWebViewMethodsForCoverage(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New("ko-KR", "Asia/Seoul");

  view.LoadHtmlString("<body>Hello World!</body>");
  view.AddJavaScriptMessageHandler("jsObject",
                                   [](const std::string& arg) {
                                   });
  view.SetTtsFocus(true);

  DALI_TEST_CHECK(view);

  END_TEST;
}

// test cases for web backforward list.

int UtcDaliWebBackForwardListCheckItem(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebBackForwardList* bfList = view.GetBackForwardList();
  DALI_TEST_CHECK(bfList != 0)

  unsigned int itemCount = bfList->GetItemCount();
  DALI_TEST_CHECK(itemCount == 1)

  std::unique_ptr<Dali::WebEngineBackForwardListItem> citem = bfList->GetCurrentItem();
  DALI_TEST_CHECK(citem != 0);

  std::unique_ptr<Dali::WebEngineBackForwardListItem> citemP = bfList->GetPreviousItem();
  DALI_TEST_CHECK(citemP != 0);

  std::unique_ptr<Dali::WebEngineBackForwardListItem> citemN = bfList->GetNextItem();
  DALI_TEST_CHECK(citemN != 0);

  const std::string kDefaultUrl("http://url");
  std::string       testValue = citem->GetUrl();
  DALI_TEST_EQUALS(testValue, kDefaultUrl, TEST_LOCATION);

  const std::string kDefaultTitle("title");
  testValue = citem->GetTitle();
  DALI_TEST_EQUALS(testValue, kDefaultTitle, TEST_LOCATION);

  const std::string kDefaultOriginalUrl("http://originalurl");
  testValue = citem->GetOriginalUrl();
  DALI_TEST_EQUALS(testValue, kDefaultOriginalUrl, TEST_LOCATION);

  std::unique_ptr<Dali::WebEngineBackForwardListItem> item = bfList->GetItemAtIndex(0);
  DALI_TEST_CHECK(item != 0);

  std::vector<std::unique_ptr<Dali::WebEngineBackForwardListItem>> vecBack = bfList->GetBackwardItems(-1);
  DALI_TEST_CHECK(vecBack.size() == 1);

  std::vector<std::unique_ptr<Dali::WebEngineBackForwardListItem>> vecForward = bfList->GetForwardItems(-1);
  DALI_TEST_CHECK(vecForward.size() == 1);

  END_TEST;
}

// test cases for web context.

int UtcDaliWebContextGetSetCacheModelEtc(void)
{
  ToolkitTestApplication application;

  Dali::WebEngineContext* context = WebView::GetContext();
  DALI_TEST_CHECK(context != 0)

  std::string kDefaultValue;

  // Reset something
  context->SetAppId("id");
  context->SetApplicationType(Dali::WebEngineContext::ApplicationType::OTHER);
  context->SetTimeOffset(0);
  context->SetTimeZoneOffset(0, 0);
  context->SetDefaultProxyAuth(kDefaultValue, kDefaultValue);
  context->DeleteAllWebDatabase();
  context->DeleteAllWebStorage();
  context->DeleteLocalFileSystem();
  context->ClearCache();
  context->DeleteAllFormPasswordData();
  context->DeleteAllFormCandidateData();

  // Check default value
  Dali::WebEngineContext::CacheModel value = context->GetCacheModel();
  DALI_TEST_CHECK(value == Dali::WebEngineContext::CacheModel::DOCUMENT_VIEWER);

  // Check Set/GetProperty
  context->SetCacheModel(Dali::WebEngineContext::CacheModel::DOCUMENT_BROWSER);
  value = context->GetCacheModel();
  DALI_TEST_CHECK(value == Dali::WebEngineContext::CacheModel::DOCUMENT_BROWSER);

  // Get cache enabled
  context->EnableCache(true);
  DALI_TEST_CHECK(context->IsCacheEnabled());

  // Get certificate
  context->SetCertificateFilePath("test");
  std::string str = context->GetCertificateFilePath();
  DALI_TEST_EQUALS(str, "test", TEST_LOCATION);

  // Set version
  DALI_TEST_CHECK(context->SetAppVersion("test"));

  // Register
  std::vector<std::string> temp;
  context->RegisterUrlSchemesAsCorsEnabled(temp);
  context->RegisterJsPluginMimeTypes(temp);
  context->DeleteFormPasswordDataList(temp);

  // Get zoom factor
  context->SetDefaultZoomFactor(1.0f);
  DALI_TEST_EQUALS(context->GetDefaultZoomFactor(), float(1.0f), TEST_LOCATION);

  // Delete cache and database
  DALI_TEST_CHECK(context->DeleteAllApplicationCache());
  DALI_TEST_CHECK(context->DeleteAllWebIndexedDatabase());

  // Get contextProxy
  context->SetProxyUri("test");
  DALI_TEST_EQUALS(context->GetProxyUri(), "test", TEST_LOCATION);
  context->SetProxyBypassRule("", "test");
  DALI_TEST_EQUALS(context->GetProxyBypassRule(), "test", TEST_LOCATION);

  //Notify low memory
  DALI_TEST_CHECK(context->FreeUnusedMemory());

  END_TEST;
}

int UtcDaliWebContextGetWebDatabaseStorageOrigins(void)
{
  ToolkitTestApplication application;

  Dali::WebEngineContext* context = WebView::GetContext();
  DALI_TEST_CHECK(context != 0)

  std::string kDefaultValue;

  // get origins of web database
  bool result = context->GetWebDatabaseOrigins(&OnSecurityOriginsAcquired);
  DALI_TEST_CHECK(result);

  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gSecurityOriginsAcquiredCallbackCalled, 1, TEST_LOCATION);
  DALI_TEST_CHECK(gSecurityOriginList.size() == 1);

  Dali::WebEngineSecurityOrigin* origin = gSecurityOriginList[0].get();
  DALI_TEST_CHECK(origin);

  result = context->DeleteWebDatabase(*origin);
  DALI_TEST_CHECK(result);

  // get origins of web storage
  result = context->GetWebStorageOrigins(&OnSecurityOriginsAcquired);
  DALI_TEST_CHECK(result);

  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gSecurityOriginsAcquiredCallbackCalled, 2, TEST_LOCATION);
  DALI_TEST_CHECK(gSecurityOriginList.size() == 1);

  origin = gSecurityOriginList[0].get();
  DALI_TEST_CHECK(origin);

  result = context->GetWebStorageUsageForOrigin(*origin, &OnStorageUsageAcquired);
  DALI_TEST_CHECK(result);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gStorageUsageAcquiredCallbackCalled, 1, TEST_LOCATION);

  result = context->DeleteWebStorage(*origin);
  DALI_TEST_CHECK(result);

  result = context->DeleteApplicationCache(*origin);
  DALI_TEST_CHECK(result);

  // form passwords, download state, mime type.
  context->GetFormPasswordList(&OnFormPasswordsAcquired);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gFormPasswordsAcquiredCallbackCalled, 1, TEST_LOCATION);
  DALI_TEST_CHECK(gPasswordDataList.size() == 1);
  DALI_TEST_EQUALS(gPasswordDataList[0]->url, "http://test.html", TEST_LOCATION);
  DALI_TEST_CHECK(gPasswordDataList[0]->useFingerprint == false);

  context->RegisterDownloadStartedCallback(&OnDownloadStarted);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gDownloadStartedCallbackCalled, 1, TEST_LOCATION);

  context->RegisterMimeOverriddenCallback(&OnMimeOverridden);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gMimeOverriddenCallbackCalled, 1, TEST_LOCATION);

  gSecurityOriginList.clear();
  gPasswordDataList.clear();

  END_TEST;
}

int UtcDaliWebContextHttpRequestInterceptor(void)
{
  ToolkitTestApplication application;

  Dali::WebEngineContext* context = WebView::GetContext();
  DALI_TEST_CHECK(context != 0)

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  // Check if web view is found or not when plugin is null.
  DALI_TEST_CHECK(!WebView::FindWebView(nullptr));

  // load url.
  context->RegisterRequestInterceptedCallback(&OnRequestIntercepted);
  DALI_TEST_EQUALS(gRequestInterceptedCallbackCalled, 0, TEST_LOCATION);
  DALI_TEST_CHECK(!gRequestInterceptorInstance);

  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gRequestInterceptedCallbackCalled, 1, TEST_LOCATION);

  // check request interceptor.
  DALI_TEST_CHECK(gRequestInterceptorInstance);
  DALI_TEST_CHECK(gRequestInterceptorInstance->Ignore());
  DALI_TEST_CHECK(gRequestInterceptorInstance->SetResponseStatus(400, "error"));
  DALI_TEST_CHECK(gRequestInterceptorInstance->AddResponseHeader("key1", "value1"));
  Dali::Property::Map testHeaders;
  testHeaders.Insert("key2", "value2");
  DALI_TEST_CHECK(gRequestInterceptorInstance->AddResponseHeaders(testHeaders));
  DALI_TEST_CHECK(gRequestInterceptorInstance->AddResponseBody((const int8_t*)"test", 4));
  DALI_TEST_CHECK(gRequestInterceptorInstance->AddResponse("key:value", (const int8_t*)"test", 4));
  DALI_TEST_CHECK(gRequestInterceptorInstance->WriteResponseChunk((const int8_t*)"test", 4));

  DALI_TEST_CHECK(gRequestInterceptorInstance->GetWebEngine());
  // Check if web view is found or not when plugin is not null.
  DALI_TEST_CHECK(WebView::FindWebView(gRequestInterceptorInstance->GetWebEngine()));

  std::string testUrl("http://test.html");
  DALI_TEST_EQUALS(gRequestInterceptorInstance->GetUrl(), testUrl, TEST_LOCATION);
  std::string testMethod("GET");
  DALI_TEST_EQUALS(gRequestInterceptorInstance->GetMethod(), testMethod, TEST_LOCATION);
  Dali::Property::Map resultHeaders = gRequestInterceptorInstance->GetHeaders();
  DALI_TEST_EQUALS(resultHeaders.Count(), 2, TEST_LOCATION);

  // Destroy web view.
  view.Reset();
  gRequestInterceptorInstance = nullptr;

  END_TEST;
}

// test cases for web cookie manager.

int UtcDaliWebCookieManagerGetSetCookieAcceptPolicy(void)
{
  ToolkitTestApplication application;

  Dali::WebEngineCookieManager* cookieManager = WebView::GetCookieManager();
  DALI_TEST_CHECK(cookieManager != 0)

  const std::string kDefaultValue;

  // Reset something
  cookieManager->SetPersistentStorage(kDefaultValue, Dali::WebEngineCookieManager::CookiePersistentStorage::SQLITE);
  cookieManager->ClearCookies();

  // Check default value
  Dali::WebEngineCookieManager::CookieAcceptPolicy value = cookieManager->GetCookieAcceptPolicy();
  DALI_TEST_CHECK(value == Dali::WebEngineCookieManager::CookieAcceptPolicy::NO_THIRD_PARTY);

  // Check Set/GetProperty
  cookieManager->SetCookieAcceptPolicy(Dali::WebEngineCookieManager::CookieAcceptPolicy::ALWAYS);
  value = cookieManager->GetCookieAcceptPolicy();
  DALI_TEST_CHECK(value == Dali::WebEngineCookieManager::CookieAcceptPolicy::ALWAYS);

  END_TEST;
}

int UtcDaliWebCookieManagerChangesWatch(void)
{
  ToolkitTestApplication application;

  Dali::WebEngineCookieManager* cookieManager = WebView::GetCookieManager();
  DALI_TEST_CHECK(cookieManager != 0)

  cookieManager->ChangesWatch(&OnChangesWatch);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gCookieManagerChangsWatchCallbackCalled, 1, TEST_LOCATION);

  END_TEST;
}

// test cases for web settings.

int UtcDaliWebSettingsGetSetDefaultFontSize(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0)

  // Reset something
  settings->AllowMixedContents(false);
  settings->EnableSpatialNavigation(false);
  settings->EnableWebSecurity(false);
  settings->EnableCacheBuilder(false);
  settings->EnableDoNotTrack(false);
  settings->UseScrollbarThumbFocusNotifications(false);
  settings->AllowFileAccessFromExternalUrl(false);
  settings->AllowScriptsOpenWindows(false);

  // Check default value
  int value = settings->GetDefaultFontSize();
  DALI_TEST_CHECK(value == 16);

  // Check Set/GetProperty
  settings->SetDefaultFontSize(20);
  value = settings->GetDefaultFontSize();
  DALI_TEST_CHECK(value == 20);

  END_TEST;
}

int UtcDaliWebSettingsCheckEnableJavaScript(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0)

  // Reset something
  settings->AllowMixedContents(false);
  settings->EnableSpatialNavigation(false);
  settings->EnableWebSecurity(false);
  settings->EnableCacheBuilder(false);
  settings->EnableDoNotTrack(false);
  settings->UseScrollbarThumbFocusNotifications(false);
  settings->AllowFileAccessFromExternalUrl(false);
  settings->AllowScriptsOpenWindows(false);

  // Check default value is true or not
  bool value = settings->IsJavaScriptEnabled();
  DALI_TEST_CHECK(value);

  // Check Set/GetProperty
  settings->EnableJavaScript(false);
  value = settings->IsJavaScriptEnabled();
  DALI_TEST_CHECK(!value);

  END_TEST;
}

int UtcDaliWebSettingsCheckEnableAutoFitting(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0)

  // Reset something
  settings->AllowMixedContents(false);
  settings->EnableSpatialNavigation(false);
  settings->EnableWebSecurity(false);
  settings->EnableCacheBuilder(false);
  settings->EnableDoNotTrack(false);
  settings->UseScrollbarThumbFocusNotifications(false);
  settings->AllowFileAccessFromExternalUrl(false);
  settings->AllowScriptsOpenWindows(false);

  // Check default value is true or not
  bool value = settings->IsAutoFittingEnabled();
  DALI_TEST_CHECK(value);

  // Check Set/GetProperty
  settings->EnableAutoFitting(false);
  value = settings->IsAutoFittingEnabled();
  DALI_TEST_CHECK(!value);

  END_TEST;
}

int UtcDaliWebSettingsCheckEnablePlugins(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0)

  // Reset something
  settings->AllowMixedContents(false);
  settings->EnableSpatialNavigation(false);
  settings->EnableWebSecurity(false);
  settings->EnableCacheBuilder(false);
  settings->EnableDoNotTrack(false);
  settings->UseScrollbarThumbFocusNotifications(false);
  settings->AllowFileAccessFromExternalUrl(false);
  settings->AllowScriptsOpenWindows(false);

  // Check default value is true or not
  bool value = settings->ArePluginsEnabled();
  DALI_TEST_CHECK(value);

  // Check Set/GetProperty
  settings->EnablePlugins(false);
  value = settings->ArePluginsEnabled();
  DALI_TEST_CHECK(!value);

  END_TEST;
}

int UtcDaliWebSettingsCheckEnablePrivateBrowsing(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0)

  // Reset something
  settings->AllowMixedContents(false);
  settings->EnableSpatialNavigation(false);
  settings->EnableWebSecurity(false);
  settings->EnableCacheBuilder(false);
  settings->EnableDoNotTrack(false);
  settings->UseScrollbarThumbFocusNotifications(false);
  settings->AllowFileAccessFromExternalUrl(false);
  settings->AllowScriptsOpenWindows(false);

  // Check default value is true or not
  bool value = settings->IsPrivateBrowsingEnabled();
  DALI_TEST_CHECK(value);

  // Check Set/GetProperty
  settings->EnablePrivateBrowsing(false);
  value = settings->IsPrivateBrowsingEnabled();
  DALI_TEST_CHECK(!value);

  END_TEST;
}

int UtcDaliWebSettingsCheckEnableLinkMagnifier(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0)

  // Reset something
  settings->AllowMixedContents(false);
  settings->EnableSpatialNavigation(false);
  settings->EnableWebSecurity(false);
  settings->EnableCacheBuilder(false);
  settings->EnableDoNotTrack(false);
  settings->UseScrollbarThumbFocusNotifications(false);
  settings->AllowFileAccessFromExternalUrl(false);
  settings->AllowScriptsOpenWindows(false);

  // Check default value is true or not
  bool value = settings->IsLinkMagnifierEnabled();
  DALI_TEST_CHECK(value);

  // Check Set/GetProperty
  settings->EnableLinkMagnifier(false);
  value = settings->IsLinkMagnifierEnabled();
  DALI_TEST_CHECK(!value);

  END_TEST;
}

int UtcDaliWebSettingsCheckUseKeypadWithoutUserAction(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0)

  // Reset something
  settings->AllowMixedContents(false);
  settings->EnableSpatialNavigation(false);
  settings->EnableWebSecurity(false);
  settings->EnableCacheBuilder(false);
  settings->EnableDoNotTrack(false);
  settings->UseScrollbarThumbFocusNotifications(false);
  settings->AllowFileAccessFromExternalUrl(false);
  settings->AllowScriptsOpenWindows(false);

  // Check default value is true or not
  bool value = settings->IsKeypadWithoutUserActionUsed();
  DALI_TEST_CHECK(value);

  // Check Set/GetProperty
  settings->UseKeypadWithoutUserAction(false);
  value = settings->IsKeypadWithoutUserActionUsed();
  DALI_TEST_CHECK(!value);

  END_TEST;
}

int UtcDaliWebSettingsCheckEnableAutofillPasswordForm(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0)

  // Reset something
  settings->AllowMixedContents(false);
  settings->EnableSpatialNavigation(false);
  settings->EnableWebSecurity(false);
  settings->EnableCacheBuilder(false);
  settings->EnableDoNotTrack(false);
  settings->UseScrollbarThumbFocusNotifications(false);
  settings->AllowFileAccessFromExternalUrl(false);
  settings->AllowScriptsOpenWindows(false);

  // Check default value is true or not
  bool value = settings->IsAutofillPasswordFormEnabled();
  DALI_TEST_CHECK(value);
  settings->EnableAutofillPasswordForm(false);
  value = settings->IsAutofillPasswordFormEnabled();
  DALI_TEST_CHECK(!value);
  END_TEST;
}

int UtcDaliWebSettingsCheckEnableFormCandidateData(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0);

  // Reset something
  settings->AllowMixedContents(false);
  settings->EnableSpatialNavigation(false);
  settings->EnableWebSecurity(false);
  settings->EnableCacheBuilder(false);
  settings->EnableDoNotTrack(false);
  settings->UseScrollbarThumbFocusNotifications(false);
  settings->AllowFileAccessFromExternalUrl(false);
  settings->AllowScriptsOpenWindows(false);

  // Check default value is true or not
  bool value = settings->IsFormCandidateDataEnabled();
  DALI_TEST_CHECK(value);

  // Check Set/GetProperty
  settings->EnableFormCandidateData(false);
  value = settings->IsFormCandidateDataEnabled();
  DALI_TEST_CHECK(!value);

  END_TEST;
}

int UtcDaliWebSettingsCheckEnableTextSelection(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0);

  // Reset something
  settings->AllowMixedContents(false);
  settings->EnableSpatialNavigation(false);
  settings->EnableWebSecurity(false);
  settings->EnableCacheBuilder(false);
  settings->EnableDoNotTrack(false);
  settings->UseScrollbarThumbFocusNotifications(false);
  settings->AllowFileAccessFromExternalUrl(false);
  settings->AllowScriptsOpenWindows(false);

  // Check default value is true or not
  bool value = settings->IsTextSelectionEnabled();
  DALI_TEST_CHECK(value);

  //Check Set/GetProperty
  settings->EnableTextSelection(false);
  value = settings->IsTextSelectionEnabled();
  DALI_TEST_CHECK(!value);

  END_TEST;
}

int UtcDaliWebSettingsCheckEnableTextAutosizing(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0);

  // Reset something
  settings->AllowMixedContents(false);
  settings->EnableSpatialNavigation(false);
  settings->EnableWebSecurity(false);
  settings->EnableCacheBuilder(false);
  settings->EnableDoNotTrack(false);
  settings->UseScrollbarThumbFocusNotifications(false);
  settings->AllowFileAccessFromExternalUrl(false);
  settings->AllowScriptsOpenWindows(false);

  // Check default value is true or not
  bool value = settings->IsTextAutosizingEnabled();
  DALI_TEST_CHECK(value);

  // Check Set/GetProperty
  settings->EnableTextAutosizing(false);
  value = settings->IsTextAutosizingEnabled();
  DALI_TEST_CHECK(!value);

  END_TEST;
}

int UtcDaliWebSettingsCheckEnableArrowScroll(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0);

  // Reset something
  settings->AllowMixedContents(false);
  settings->EnableSpatialNavigation(false);
  settings->EnableWebSecurity(false);
  settings->EnableCacheBuilder(false);
  settings->EnableDoNotTrack(false);
  settings->UseScrollbarThumbFocusNotifications(false);
  settings->AllowFileAccessFromExternalUrl(false);
  settings->AllowScriptsOpenWindows(false);

  // Check default value is true or not
  bool value = settings->IsArrowScrollEnabled();
  DALI_TEST_CHECK(value);

  // Check Set/GetProperty
  settings->EnableArrowScroll(false);
  value = settings->IsArrowScrollEnabled();
  DALI_TEST_CHECK(!value);

  END_TEST;
}

int UtcDaliWebSettingsCheckEnableClipboard(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0);

  // Reset something
  settings->AllowMixedContents(false);
  settings->EnableSpatialNavigation(false);
  settings->EnableWebSecurity(false);
  settings->EnableCacheBuilder(false);
  settings->EnableDoNotTrack(false);
  settings->UseScrollbarThumbFocusNotifications(false);
  settings->AllowFileAccessFromExternalUrl(false);
  settings->AllowScriptsOpenWindows(false);

  // Check default value is true or not
  bool value = settings->IsClipboardEnabled();
  DALI_TEST_CHECK(value);
  settings->EnableClipboard(false);
  value = settings->IsClipboardEnabled();
  DALI_TEST_CHECK(!value);
  END_TEST;
}

int UtcDaliWebSettingsCheckEnableImePanel(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0);

  // Reset something
  settings->AllowMixedContents(false);
  settings->EnableSpatialNavigation(false);
  settings->EnableWebSecurity(false);
  settings->EnableCacheBuilder(false);
  settings->EnableDoNotTrack(false);
  settings->UseScrollbarThumbFocusNotifications(false);
  settings->AllowFileAccessFromExternalUrl(false);
  settings->AllowScriptsOpenWindows(false);

  // Check default value is true or not
  bool value = settings->IsImePanelEnabled();
  DALI_TEST_CHECK(value);

  // Check Set/GetProperty
  settings->EnableImePanel(false);
  value = settings->IsImePanelEnabled();
  DALI_TEST_CHECK(!value);

  END_TEST;
}

int UtcDaliWebSettingsCheckAllowImagesLoadAutomatically(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0)

  // Reset something
  settings->AllowMixedContents(false);
  settings->EnableSpatialNavigation(false);
  settings->EnableWebSecurity(false);
  settings->EnableCacheBuilder(false);
  settings->EnableDoNotTrack(false);
  settings->UseScrollbarThumbFocusNotifications(false);
  settings->AllowFileAccessFromExternalUrl(false);
  settings->AllowScriptsOpenWindows(false);

  // Check default value is true or not
  bool value = settings->AreImagesLoadedAutomatically();
  DALI_TEST_CHECK(value);

  // Check Set/GetProperty
  settings->AllowImagesLoadAutomatically(false);
  value = settings->AreImagesLoadedAutomatically();
  DALI_TEST_CHECK(!value);

  END_TEST;
}

int UtcDaliWebSettingsGetSetDefaultTextEncodingName(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0)

  const std::string kDefaultValue;
  const std::string kTestValue = "UTF-8";

  // Reset something
  settings->AllowMixedContents(false);
  settings->EnableSpatialNavigation(false);
  settings->EnableWebSecurity(false);
  settings->EnableCacheBuilder(false);
  settings->EnableDoNotTrack(false);
  settings->UseScrollbarThumbFocusNotifications(false);
  settings->AllowFileAccessFromExternalUrl(false);
  settings->AllowScriptsOpenWindows(false);

  // Check default value
  std::string value = settings->GetDefaultTextEncodingName();
  DALI_TEST_EQUALS(value, kDefaultValue, TEST_LOCATION);

  // Check Set/GetProperty
  settings->SetDefaultTextEncodingName(kTestValue);
  value = settings->GetDefaultTextEncodingName();
  DALI_TEST_EQUALS(value, kTestValue, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebSettingsSetViewportMetaTag(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0)

  // Check the value is true or not
  bool value = settings->SetViewportMetaTag(true);
  DALI_TEST_CHECK(value);

  END_TEST;
}

int UtcDaliWebSettingsSetForceZoom(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0)

  // Check the value is true or not
  bool value = settings->SetForceZoom(true);
  DALI_TEST_CHECK(value);

  value = settings->IsZoomForced();
  DALI_TEST_CHECK(value);

  END_TEST;
}

int UtcDaliWebSettingsSetTextZoomEnabled(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0)

  // Check the value is true or not
  bool value = settings->SetTextZoomEnabled(true);
  DALI_TEST_CHECK(value);

  value = settings->IsTextZoomEnabled();
  DALI_TEST_CHECK(value);

  END_TEST;
}

int UtcDaliWebSettingsSetExtraFeature(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  Dali::Toolkit::WebSettings* settings = view.GetSettings();
  DALI_TEST_CHECK(settings != 0)

  // Check the value is true or not
  settings->SetExtraFeature("test", true);
  bool value = settings->IsExtraFeatureEnabled("test");
  DALI_TEST_CHECK(value);

  END_TEST;
}

int UtcDaliWebViewGetPlainText(void)
{
  ToolkitTestApplication application;

  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  view.LoadUrl(TEST_URL1);

  view.GetPlainTextAsynchronously(&OnPlainTextReceived);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(gPlainTextReceivedCallbackCalled, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewVisibilityChange(void)
{
  ToolkitTestApplication application;

  WebView view  = WebView::New();
  Control dummy = Control::New();
  DALI_TEST_CHECK(view);
  DALI_TEST_CHECK(dummy);

  view.LoadUrl(TEST_URL1);

  dummy.Add(view);
  auto window = application.GetWindow();
  window.Add(dummy);

  application.SendNotification();
  application.Render();

  try
  {
    // TODO : There is no way to check visiblity setting result from web engine.
    // Just call API and exception check.
    view.SetProperty(Actor::Property::VISIBLE, false);
    view.SetProperty(Actor::Property::VISIBLE, true);
    dummy.SetProperty(Actor::Property::VISIBLE, false);
    dummy.SetProperty(Actor::Property::VISIBLE, true);
    dummy.Unparent();
    window.Hide();
    window.Add(dummy);
    window.Show();
    window.Hide();
    window.Show();
    dummy.SetProperty(Actor::Property::VISIBLE, false);
    view.SetProperty(Actor::Property::VISIBLE, false);
    dummy.SetProperty(Actor::Property::VISIBLE, true);
    view.SetProperty(Actor::Property::VISIBLE, true);
    tet_result(TET_PASS);
  }
  catch(...)
  {
    // Should not throw exception
    tet_result(TET_FAIL);
  }

  END_TEST;
}