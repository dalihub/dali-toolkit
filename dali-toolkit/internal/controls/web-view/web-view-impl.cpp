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

// CLASS HEADER
#include "web-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-back-forward-list.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-certificate.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-console-message.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-context-menu-item.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-context-menu.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-context.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-cookie-manager.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-form-repost-decision.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-hit-test.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-http-auth-handler.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-load-error.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-policy-decision.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-settings.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/public-api/adaptor-framework/native-image-source.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <cstring>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/web-view/web-back-forward-list.h>
#include <dali-toolkit/devel-api/controls/web-view/web-settings.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/public-api/image-loader/image.h>
#include <dali-toolkit/public-api/image-loader/image-url.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali/integration-api/debug.h>

#include <functional>
#include <memory>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
BaseHandle Create()
{
  return Toolkit::WebView::New();
}

// clang-format off
DALI_TYPE_REGISTRATION_BEGIN(Toolkit::WebView, Toolkit::Control, Create)

DALI_PROPERTY_REGISTRATION(Toolkit, WebView, "url",                     STRING,  URL                       )
DALI_PROPERTY_REGISTRATION(Toolkit, WebView, "userAgent",               STRING,  USER_AGENT                )
DALI_PROPERTY_REGISTRATION(Toolkit, WebView, "scrollPosition",          VECTOR2, SCROLL_POSITION           )
DALI_PROPERTY_REGISTRATION(Toolkit, WebView, "scrollSize",              VECTOR2, SCROLL_SIZE               )
DALI_PROPERTY_REGISTRATION(Toolkit, WebView, "contentSize",             VECTOR2, CONTENT_SIZE              )
DALI_PROPERTY_REGISTRATION(Toolkit, WebView, "title",                   STRING,  TITLE                     )
DALI_PROPERTY_REGISTRATION(Toolkit, WebView, "videoHoleEnabled",        BOOLEAN, VIDEO_HOLE_ENABLED        )
DALI_PROPERTY_REGISTRATION(Toolkit, WebView, "mouseEventsEnabled",      BOOLEAN, MOUSE_EVENTS_ENABLED      )
DALI_PROPERTY_REGISTRATION(Toolkit, WebView, "keyEventsEnabled",        BOOLEAN, KEY_EVENTS_ENABLED        )
DALI_PROPERTY_REGISTRATION(Toolkit, WebView, "documentBackgroundColor", VECTOR4, DOCUMENT_BACKGROUND_COLOR )
DALI_PROPERTY_REGISTRATION(Toolkit, WebView, "tilesClearedWhenHidden",  BOOLEAN, TILES_CLEARED_WHEN_HIDDEN )
DALI_PROPERTY_REGISTRATION(Toolkit, WebView, "tileCoverAreaMultiplier", FLOAT,   TILE_COVER_AREA_MULTIPLIER)
DALI_PROPERTY_REGISTRATION(Toolkit, WebView, "cursorEnabledByClient",   BOOLEAN, CURSOR_ENABLED_BY_CLIENT  )
DALI_PROPERTY_REGISTRATION(Toolkit, WebView, "selectedText",            STRING,  SELECTED_TEXT             )
DALI_PROPERTY_REGISTRATION(Toolkit, WebView, "pageZoomFactor",          FLOAT,   PAGE_ZOOM_FACTOR          )
DALI_PROPERTY_REGISTRATION(Toolkit, WebView, "textZoomFactor",          FLOAT,   TEXT_ZOOM_FACTOR          )
DALI_PROPERTY_REGISTRATION(Toolkit, WebView, "loadProgressPercentage",  FLOAT,   LOAD_PROGRESS_PERCENTAGE  )

DALI_TYPE_REGISTRATION_END()
// clang-format on

} // namespace

std::unordered_map<Dali::WebEnginePlugin*, Dali::WeakHandle<Toolkit::WebView>> WebView::mPluginWebViewMap;

WebView::WebView(const std::string& locale, const std::string& timezoneId)
: Control(ControlBehaviour(ACTOR_BEHAVIOUR_DEFAULT | DISABLE_STYLE_CHANGE_SIGNALS)),
  mVisual(),
  mWebViewSize(Stage::GetCurrent().GetSize()),
  mWebEngine(),
  mWebViewArea(0, 0, mWebViewSize.width, mWebViewSize.height),
  mVideoHoleEnabled(false),
  mMouseEventsEnabled(true),
  mKeyEventsEnabled(true),
  mScreenshotCapturedCallback{nullptr},
  mFrameRenderedCallback{nullptr},
  mCornerRadius(Vector4::ZERO),
  mCornerRadiusPolicy(1.0f)
{
  mWebEngine = Dali::WebEngine::New();

  // WebEngine is empty when it is not properly initialized.
  if(mWebEngine)
  {
    mWebEngine.Create(mWebViewSize.width, mWebViewSize.height, locale, timezoneId);
  }
}

WebView::WebView(uint32_t argc, char** argv)
: Control(ControlBehaviour(ACTOR_BEHAVIOUR_DEFAULT | DISABLE_STYLE_CHANGE_SIGNALS)),
  mVisual(),
  mWebViewSize(Stage::GetCurrent().GetSize()),
  mWebEngine(),
  mWebViewArea(0, 0, mWebViewSize.width, mWebViewSize.height),
  mVideoHoleEnabled(false),
  mMouseEventsEnabled(true),
  mKeyEventsEnabled(true),
  mScreenshotCapturedCallback{nullptr},
  mFrameRenderedCallback{nullptr},
  mCornerRadius(Vector4::ZERO),
  mCornerRadiusPolicy(1.0f)
{
  mWebEngine = Dali::WebEngine::New();

  // WebEngine is empty when it is not properly initialized.
  if(mWebEngine)
  {
    mWebEngine.Create(mWebViewSize.width, mWebViewSize.height, argc, argv);
  }
}

WebView::WebView()
: WebView("", "")
{
}

WebView::~WebView()
{
  if(mWebEngine)
  {
    auto iter = mPluginWebViewMap.find(mWebEngine.GetPlugin());
    if (iter != mPluginWebViewMap.end())
    {
      mPluginWebViewMap.erase(iter);
    }
    mWebEngine.Destroy();
  }
}

Toolkit::WebView WebView::New()
{
  WebView*         impl   = new WebView();
  Toolkit::WebView handle = Toolkit::WebView(*impl);
  if (impl->GetPlugin())
  {
    mPluginWebViewMap[impl->GetPlugin()] = handle;
  }
  impl->Initialize();
  return handle;
}

Toolkit::WebView WebView::New(const std::string& locale, const std::string& timezoneId)
{
  WebView*         impl   = new WebView(locale, timezoneId);
  Toolkit::WebView handle = Toolkit::WebView(*impl);
  if (impl->GetPlugin())
  {
    mPluginWebViewMap[impl->GetPlugin()] = handle;
  }
  impl->Initialize();
  return handle;
}

Toolkit::WebView WebView::New(uint32_t argc, char** argv)
{
  WebView*         impl   = new WebView(argc, argv);
  Toolkit::WebView handle = Toolkit::WebView(*impl);
  if (impl->GetPlugin())
  {
    mPluginWebViewMap[impl->GetPlugin()] = handle;
  }
  impl->Initialize();
  return handle;
}

Toolkit::WebView WebView::FindWebView(Dali::WebEnginePlugin* plugin)
{
  auto iter = mPluginWebViewMap.find(plugin);
  if (iter != mPluginWebViewMap.end())
  {
    return iter->second.GetHandle();
  }
  return Toolkit::WebView();
}

Dali::WebEngineContext* WebView::GetContext()
{
  return Dali::WebEngine::GetContext();
}

Dali::WebEngineCookieManager* WebView::GetCookieManager()
{
  return Dali::WebEngine::GetCookieManager();
}

void WebView::OnInitialize()
{
  Actor self = Self();

  self.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  self.TouchedSignal().Connect(this, &WebView::OnTouchEvent);
  self.HoveredSignal().Connect(this, &WebView::OnHoverEvent);
  self.WheelEventSignal().Connect(this, &WebView::OnWheelEvent);
  Dali::DevelActor::VisibilityChangedSignal(self).Connect(this, &WebView::OnVisibilityChanged);

  mPositionUpdateNotification = self.AddPropertyNotification(Actor::Property::WORLD_POSITION, StepCondition(1.0f, 1.0f));
  mSizeUpdateNotification     = self.AddPropertyNotification(Actor::Property::SIZE, StepCondition(1.0f, 1.0f));
  mScaleUpdateNotification    = self.AddPropertyNotification(Actor::Property::WORLD_SCALE, StepCondition(0.1f, 1.0f));
  mPositionUpdateNotification.NotifySignal().Connect(this, &WebView::OnDisplayAreaUpdated);
  mSizeUpdateNotification.NotifySignal().Connect(this, &WebView::OnDisplayAreaUpdated);
  mScaleUpdateNotification.NotifySignal().Connect(this, &WebView::OnDisplayAreaUpdated);

  // Create WebVisual for WebView
  Property::Map propertyMap;
  propertyMap.Insert(Dali::Toolkit::Visual::Property::TYPE, Dali::Toolkit::Visual::COLOR);
  propertyMap.Insert(Dali::Toolkit::Visual::Property::MIX_COLOR, Color::TRANSPARENT);
  Toolkit::Visual::Base webVisual = Toolkit::VisualFactory::Get().CreateVisual(propertyMap);
  if(webVisual)
  {
    Dali::Toolkit::DevelControl::RegisterVisual(*this, Toolkit::WebView::Property::URL, webVisual);
  }
  else
  {
    DALI_LOG_ERROR("fail to create webVisual for CornerRadius");
    Dali::Toolkit::DevelControl::UnregisterVisual(*this, Toolkit::WebView::Property::URL);
  }

  if(mWebEngine)
  {
    mWebEngine.RegisterFrameRenderedCallback(std::bind(&WebView::OnFrameRendered, this));
    mWebSettings        = std::unique_ptr<Dali::Toolkit::WebSettings>(new WebSettings(mWebEngine.GetSettings()));
    mWebBackForwardList = std::unique_ptr<Dali::Toolkit::WebBackForwardList>(new WebBackForwardList(mWebEngine.GetBackForwardList()));
  }

  self.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::FILLER);
}

DevelControl::ControlAccessible* WebView::CreateAccessibleObject()
{
  return new WebViewAccessible(Self(), mWebEngine);
}

void WebView::ChangeOrientation(int orientation)
{
  if(mWebEngine)
  {
    mWebEngine.ChangeOrientation(orientation);
  }
}

Dali::Toolkit::WebSettings* WebView::GetSettings() const
{
  return mWebSettings.get();
}

Dali::Toolkit::WebBackForwardList* WebView::GetBackForwardList() const
{
  return mWebBackForwardList.get();
}

Dali::WebEnginePlugin* WebView::GetPlugin() const
{
  return mWebEngine ? mWebEngine.GetPlugin() : nullptr;
}

Dali::Toolkit::ImageView WebView::GetFavicon() const
{
  Dali::Toolkit::ImageView faviconView;
  if(mWebEngine)
  {
    Dali::PixelData pixelData = mWebEngine.GetFavicon();
    faviconView               = CreateImageView(pixelData);
  }
  return faviconView;
}

void WebView::LoadUrl(const std::string& url)
{
  if(mWebEngine)
  {
    mWebEngine.LoadUrl(url);
  }
}

void WebView::LoadHtmlString(const std::string& htmlString)
{
  if(mWebEngine)
  {
    mWebEngine.LoadHtmlString(htmlString);
  }
}

bool WebView::LoadHtmlStringOverrideCurrentEntry(const std::string& html, const std::string& basicUri, const std::string& unreachableUrl)
{
  if(!mWebEngine)
    return false;

  return mWebEngine.LoadHtmlStringOverrideCurrentEntry(html, basicUri, unreachableUrl);
}

bool WebView::LoadContents(const int8_t* contents, uint32_t contentSize, const std::string& mimeType, const std::string& encoding, const std::string& baseUri)
{
  if(!mWebEngine)
    return false;

  return mWebEngine.LoadContents(contents, contentSize, mimeType, encoding, baseUri);
}

void WebView::Reload()
{
  if(mWebEngine)
  {
    mWebEngine.Reload();
  }
}

bool WebView::ReloadWithoutCache()
{
  return mWebEngine ? mWebEngine.ReloadWithoutCache() : false;
}

void WebView::StopLoading()
{
  if(mWebEngine)
  {
    mWebEngine.StopLoading();
  }
}

void WebView::Suspend()
{
  if(mWebEngine)
  {
    mWebEngine.Suspend();
  }
}

void WebView::Resume()
{
  if(mWebEngine)
  {
    mWebEngine.Resume();
  }
}

void WebView::SuspendNetworkLoading()
{
  if(mWebEngine)
  {
    mWebEngine.SuspendNetworkLoading();
  }
}

void WebView::ResumeNetworkLoading()
{
  if(mWebEngine)
  {
    mWebEngine.ResumeNetworkLoading();
  }
}

bool WebView::AddCustomHeader(const std::string& name, const std::string& value)
{
  return mWebEngine ? mWebEngine.AddCustomHeader(name, value) : false;
}

bool WebView::RemoveCustomHeader(const std::string& name)
{
  return mWebEngine ? mWebEngine.RemoveCustomHeader(name) : false;
}

uint32_t WebView::StartInspectorServer(uint32_t port)
{
  return mWebEngine ? mWebEngine.StartInspectorServer(port) : false;
}

bool WebView::StopInspectorServer()
{
  return mWebEngine ? mWebEngine.StopInspectorServer() : false;
}

void WebView::ScrollBy(int32_t deltaX, int32_t deltaY)
{
  if(mWebEngine)
  {
    mWebEngine.ScrollBy(deltaX, deltaY);
  }
}

bool WebView::ScrollEdgeBy(int32_t deltaX, int32_t deltaY)
{
  return mWebEngine ? mWebEngine.ScrollEdgeBy(deltaX, deltaY) : false;
}

bool WebView::CanGoForward()
{
  return mWebEngine ? mWebEngine.CanGoForward() : false;
}

void WebView::GoForward()
{
  if(mWebEngine)
  {
    mWebEngine.GoForward();
  }
}

bool WebView::CanGoBack()
{
  return mWebEngine ? mWebEngine.CanGoBack() : false;
}

void WebView::GoBack()
{
  if(mWebEngine)
  {
    mWebEngine.GoBack();
  }
}

void WebView::EvaluateJavaScript(const std::string& script, std::function<void(const std::string&)> resultHandler)
{
  if(mWebEngine)
  {
    mWebEngine.EvaluateJavaScript(script, resultHandler);
  }
}

void WebView::AddJavaScriptMessageHandler(const std::string& exposedObjectName, std::function<void(const std::string&)> handler)
{
  if(mWebEngine)
  {
    mWebEngine.AddJavaScriptMessageHandler(exposedObjectName, handler);
  }
}

void WebView::RegisterJavaScriptAlertCallback(Dali::WebEnginePlugin::JavaScriptAlertCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterJavaScriptAlertCallback(callback);
  }
}

void WebView::JavaScriptAlertReply()
{
  if(mWebEngine)
  {
    mWebEngine.JavaScriptAlertReply();
  }
}

void WebView::RegisterJavaScriptConfirmCallback(Dali::WebEnginePlugin::JavaScriptConfirmCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterJavaScriptConfirmCallback(callback);
  }
}

void WebView::JavaScriptConfirmReply(bool confirmed)
{
  if(mWebEngine)
  {
    mWebEngine.JavaScriptConfirmReply(confirmed);
  }
}

void WebView::RegisterJavaScriptPromptCallback(Dali::WebEnginePlugin::JavaScriptPromptCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterJavaScriptPromptCallback(callback);
  }
}

void WebView::JavaScriptPromptReply(const std::string& result)
{
  if(mWebEngine)
  {
    mWebEngine.JavaScriptPromptReply(result);
  }
}

std::unique_ptr<Dali::WebEngineHitTest> WebView::CreateHitTest(int32_t x, int32_t y, Dali::WebEngineHitTest::HitTestMode mode)
{
  std::unique_ptr<Dali::WebEngineHitTest> webHitTest;
  if(!mWebEngine)
  {
    return webHitTest;
  }

  return mWebEngine.CreateHitTest(x, y, mode);
}

bool WebView::CreateHitTestAsynchronously(int32_t x, int32_t y, Dali::WebEngineHitTest::HitTestMode mode, Dali::WebEnginePlugin::WebEngineHitTestCreatedCallback callback)
{
  bool result = false;
  if(mWebEngine)
  {
    result = mWebEngine.CreateHitTestAsynchronously(x, y, mode, callback);
  }
  return result;
}

void WebView::ClearHistory()
{
  if(mWebEngine)
  {
    mWebEngine.ClearHistory();
  }
}

void WebView::ClearAllTilesResources()
{
  if(mWebEngine)
  {
    mWebEngine.ClearAllTilesResources();
  }
}

void WebView::SetScaleFactor(float scaleFactor, Dali::Vector2 point)
{
  if(mWebEngine)
  {
    mWebEngine.SetScaleFactor(scaleFactor, point);
  }
}

float WebView::GetScaleFactor() const
{
  return mWebEngine ? mWebEngine.GetScaleFactor() : 0.0f;
}

void WebView::ActivateAccessibility(bool activated)
{
  if(mWebEngine)
  {
    mWebEngine.ActivateAccessibility(activated);
  }
}

bool WebView::HighlightText(const std::string& text, Dali::WebEnginePlugin::FindOption options, uint32_t maxMatchCount)
{
  return mWebEngine ? mWebEngine.HighlightText(text, options, maxMatchCount) : false;
}

void WebView::AddDynamicCertificatePath(const std::string& host, const std::string& certPath)
{
  if(mWebEngine)
  {
    mWebEngine.AddDynamicCertificatePath(host, certPath);
  }
}

Dali::Toolkit::ImageView WebView::GetScreenshot(Dali::Rect<int32_t> viewArea, float scaleFactor)
{
  Dali::Toolkit::ImageView imageView;
  if(mWebEngine)
  {
    Dali::PixelData pixelData = mWebEngine.GetScreenshot(viewArea, scaleFactor);
    imageView                 = CreateImageView(pixelData);
  }
  return imageView;
}

bool WebView::GetScreenshotAsynchronously(Dali::Rect<int32_t> viewArea, float scaleFactor, Dali::Toolkit::WebView::WebViewScreenshotCapturedCallback callback)
{
  mScreenshotCapturedCallback = callback;
  return mWebEngine ? mWebEngine.GetScreenshotAsynchronously(viewArea, scaleFactor, std::bind(&WebView::OnScreenshotCaptured, this, std::placeholders::_1)) : false;
}

bool WebView::CheckVideoPlayingAsynchronously(Dali::WebEnginePlugin::VideoPlayingCallback callback)
{
  return mWebEngine ? mWebEngine.CheckVideoPlayingAsynchronously(callback) : false;
}

void WebView::ExitFullscreen()
{
  if(mWebEngine)
  {
    mWebEngine.ExitFullscreen();
  }
}

void WebView::RegisterGeolocationPermissionCallback(Dali::WebEnginePlugin::GeolocationPermissionCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterGeolocationPermissionCallback(callback);
  }
}

void WebView::SetTtsFocus(bool focused)
{
  if(mWebEngine && !HasKeyInputFocus())
  {
    mWebEngine.SetFocus(focused);
  }
}

void WebView::EnableVideoHole(bool enabled)
{
  mVideoHoleEnabled = enabled;

  EnableBlendMode(!mVideoHoleEnabled);

  if(mWebEngine)
  {
    mWebEngine.EnableVideoHole(mVideoHoleEnabled);
  }
}

void WebView::EnableBlendMode(bool blendEnabled)
{
  Actor self = Self();
  for(uint32_t i = 0; i < self.GetRendererCount(); i++)
  {
    Dali::Renderer render = self.GetRendererAt(i);
    render.SetProperty(Renderer::Property::BLEND_MODE, blendEnabled ? BlendMode::ON : BlendMode::OFF);
  }
}

Dali::Toolkit::ImageView WebView::CreateImageView(Dali::PixelData pixel) const
{
  if(!pixel)
  {
    return Dali::Toolkit::ImageView();
  }

  Dali::Toolkit::ImageUrl  url       = Dali::Toolkit::Image::GenerateUrl(pixel);
  Dali::Toolkit::ImageView imageView = Dali::Toolkit::ImageView::New(url.GetUrl());
  imageView.SetProperty(Dali::Actor::Property::SIZE, Vector2(pixel.GetWidth(), pixel.GetHeight()));
  return imageView;
}

void WebView::RegisterPageLoadStartedCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterPageLoadStartedCallback(callback);
  }
}

void WebView::RegisterPageLoadInProgressCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterPageLoadInProgressCallback(callback);
  }
}

void WebView::RegisterPageLoadFinishedCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterPageLoadFinishedCallback(callback);
  }
}

void WebView::RegisterPageLoadErrorCallback(Dali::WebEnginePlugin::WebEnginePageLoadErrorCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterPageLoadErrorCallback(callback);
  }
}

void WebView::RegisterScrollEdgeReachedCallback(Dali::WebEnginePlugin::WebEngineScrollEdgeReachedCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterScrollEdgeReachedCallback(callback);
  }
}

void WebView::RegisterUrlChangedCallback(Dali::WebEnginePlugin::WebEngineUrlChangedCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterUrlChangedCallback(callback);
  }
}

void WebView::RegisterFormRepostDecidedCallback(Dali::WebEnginePlugin::WebEngineFormRepostDecidedCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterFormRepostDecidedCallback(callback);
  }
}

void WebView::RegisterFrameRenderedCallback(Dali::WebEnginePlugin::WebEngineFrameRenderedCallback callback)
{
  mFrameRenderedCallback = callback;
}

void WebView::RegisterConsoleMessageReceivedCallback(Dali::WebEnginePlugin::WebEngineConsoleMessageReceivedCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterConsoleMessageReceivedCallback(callback);
  }
}

void WebView::RegisterResponsePolicyDecidedCallback(Dali::WebEnginePlugin::WebEngineResponsePolicyDecidedCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterResponsePolicyDecidedCallback(callback);
  }
}

void WebView::RegisterNavigationPolicyDecidedCallback(Dali::WebEnginePlugin::WebEngineNavigationPolicyDecidedCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterNavigationPolicyDecidedCallback(callback);
  }
}

void WebView::RegisterNewWindowPolicyDecidedCallback(Dali::WebEnginePlugin::WebEngineNewWindowPolicyDecidedCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterNewWindowPolicyDecidedCallback(callback);
  }
}

void WebView::RegisterNewWindowCreatedCallback(Dali::WebEnginePlugin::WebEngineNewWindowCreatedCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterNewWindowCreatedCallback(callback);
  }
}

void WebView::RegisterCertificateConfirmedCallback(Dali::WebEnginePlugin::WebEngineCertificateCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterCertificateConfirmedCallback(callback);
  }
}

void WebView::RegisterSslCertificateChangedCallback(Dali::WebEnginePlugin::WebEngineCertificateCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterSslCertificateChangedCallback(callback);
  }
}

void WebView::RegisterHttpAuthHandlerCallback(Dali::WebEnginePlugin::WebEngineHttpAuthHandlerCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterHttpAuthHandlerCallback(callback);
  }
}

void WebView::RegisterContextMenuShownCallback(Dali::WebEnginePlugin::WebEngineContextMenuShownCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterContextMenuShownCallback(callback);
  }
}

void WebView::RegisterContextMenuHiddenCallback(Dali::WebEnginePlugin::WebEngineContextMenuHiddenCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterContextMenuHiddenCallback(callback);
  }
}

void WebView::RegisterFullscreenEnteredCallback(Dali::WebEnginePlugin::WebEngineFullscreenEnteredCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterFullscreenEnteredCallback(callback);
  }
}

void WebView::RegisterFullscreenExitedCallback(Dali::WebEnginePlugin::WebEngineFullscreenExitedCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterFullscreenExitedCallback(callback);
  }
}

void WebView::RegisterTextFoundCallback(Dali::WebEnginePlugin::WebEngineTextFoundCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterTextFoundCallback(callback);
  }
}

void WebView::GetPlainTextAsynchronously(Dali::WebEnginePlugin::PlainTextReceivedCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.GetPlainTextAsynchronously(callback);
  }
}

void WebView::WebAuthenticationCancel()
{
  if(mWebEngine)
  {
    mWebEngine.WebAuthenticationCancel();
  }
}

void WebView::RegisterWebAuthDisplayQRCallback(Dali::WebEnginePlugin::WebEngineWebAuthDisplayQRCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterWebAuthDisplayQRCallback(std::move(callback));
  }
}

void WebView::RegisterWebAuthResponseCallback(Dali::WebEnginePlugin::WebEngineWebAuthResponseCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterWebAuthResponseCallback(std::move(callback));
  }
}

void WebView::OnFrameRendered()
{
  if(mFrameRenderedCallback)
  {
    mFrameRenderedCallback();
  }

  // Make sure that mVisual is created only once.
  if (mVisual)
    return;

  // Get webVisual for checking corner radius
  Toolkit::Visual::Base webVisual = Dali::Toolkit::DevelControl::GetVisual(*this, Toolkit::WebView::Property::URL);
  Property::Map webMap;
  webVisual.CreatePropertyMap(webMap);
  Property::Value* cornerRadiusValue =  webMap.Find(Dali::Toolkit::DevelVisual::Property::CORNER_RADIUS);
  if(cornerRadiusValue)
  {
    mCornerRadius = cornerRadiusValue->Get<Vector4>();
  }
  Property::Value* cornerRadiusValuePolicy =  webMap.Find(Dali::Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY);
  if(cornerRadiusValuePolicy)
  {
    mCornerRadiusPolicy = cornerRadiusValuePolicy->Get<int>();
  }

  Dali::Toolkit::ImageUrl nativeImageUrl = Dali::Toolkit::Image::GenerateUrl(mWebEngine.GetNativeImageSource());
  Property::Map propertyMap;
  propertyMap.Insert(Dali::Toolkit::Visual::Property::TYPE, Dali::Toolkit::Visual::IMAGE);
  propertyMap.Insert(Dali::Toolkit::ImageVisual::Property::URL, nativeImageUrl.GetUrl());
  propertyMap.Insert(Dali::Toolkit::DevelVisual::Property::CORNER_RADIUS, mCornerRadius);
  propertyMap.Insert(Dali::Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY, mCornerRadiusPolicy);
  mVisual = Toolkit::VisualFactory::Get().CreateVisual(propertyMap);
  if(mVisual)
  {
    DevelControl::RegisterVisual(*this, Toolkit::WebView::Property::URL, mVisual);
    EnableBlendMode(!mVideoHoleEnabled);
  }
}

void WebView::OnDisplayAreaUpdated(Dali::PropertyNotification& /*source*/)
{
  if(!mWebEngine)
    return;

  Actor self(Self());

  bool    positionUsesAnchorPoint = self.GetProperty<bool>(Actor::Property::POSITION_USES_ANCHOR_POINT);
  Vector3 actorSize               = self.GetCurrentProperty<Vector3>(Actor::Property::SIZE) * self.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  Vector3 anchorPointOffSet       = actorSize * (positionUsesAnchorPoint ? self.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT) : AnchorPoint::TOP_LEFT);
  Vector2 screenPosition          = self.GetProperty<Vector2>(Actor::Property::SCREEN_POSITION);

  Dali::Rect<int32_t> displayArea;
  displayArea.x      = screenPosition.x - anchorPointOffSet.x;
  displayArea.y      = screenPosition.y - anchorPointOffSet.y;
  displayArea.width  = actorSize.x;
  displayArea.height = actorSize.y;

  Size displaySize = Size(displayArea.width, displayArea.height);
  if(mWebViewSize != displaySize)
  {
    mWebViewSize = displaySize;
  }

  if(mWebViewArea != displayArea)
  {
    mWebViewArea = displayArea;
    mWebEngine.UpdateDisplayArea(mWebViewArea);
  }
}

void WebView::OnVisibilityChanged(Actor actor, bool isVisible, Dali::DevelActor::VisibilityChange::Type type)
{
  if(type == Dali::DevelActor::VisibilityChange::Type::SELF)
  {
    SetVisibility(isVisible);
  }
}

void WebView::OnScreenshotCaptured(Dali::PixelData pixel)
{
  if(mScreenshotCapturedCallback)
  {
    Dali::Toolkit::ImageView imageView = CreateImageView(pixel);
    mScreenshotCapturedCallback(imageView);
  }
}

void WebView::OnSceneConnection(int depth)
{
  Control::OnSceneConnection(depth);
  EnableBlendMode(!mVideoHoleEnabled);
}

bool WebView::OnTouchEvent(Actor actor, const Dali::TouchEvent& touch)
{
  bool result = false;

  if(mWebEngine)
  {
    result = mWebEngine.SendTouchEvent(touch);
  }
  return result;
}

bool WebView::OnKeyEvent(const Dali::KeyEvent& event)
{
  bool result = false;

  if(mWebEngine)
  {
    result = mWebEngine.SendKeyEvent(event);
  }
  return result;
}

bool WebView::OnHoverEvent(Actor actor, const Dali::HoverEvent& hover)
{
  bool result = false;
  if(mWebEngine && mMouseEventsEnabled)
  {
    result = mWebEngine.SendHoverEvent(hover);
  }
  return result;
}

bool WebView::OnWheelEvent(Actor actor, const Dali::WheelEvent& wheel)
{
  bool result = false;
  if(mWebEngine && mMouseEventsEnabled)
  {
    result = mWebEngine.SendWheelEvent(wheel);
  }
  return result;
}

void WebView::OnKeyInputFocusGained()
{
  if(mWebEngine)
  {
    mWebEngine.SetFocus(true);
  }

  EmitKeyInputFocusSignal(true); // Calls back into the Control hence done last.
}

void WebView::OnKeyInputFocusLost()
{
  if(mWebEngine)
  {
    mWebEngine.SetFocus(false);
  }

  EmitKeyInputFocusSignal(false); // Calls back into the Control hence done last.
}

Vector3 WebView::GetNaturalSize()
{
  if(mVisual)
  {
    Vector2 rendererNaturalSize;
    mVisual.GetNaturalSize(rendererNaturalSize);
    return Vector3(rendererNaturalSize);
  }

  return Vector3(mWebViewSize);
}

void WebView::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  Toolkit::WebView webView = Toolkit::WebView::DownCast(Dali::BaseHandle(object));

  if(webView)
  {
    WebView& impl = GetImpl(webView);
    switch(index)
    {
      case Toolkit::WebView::Property::URL:
      {
        std::string url;
        if(value.Get(url))
        {
          impl.LoadUrl(url);
        }
        break;
      }
      case Toolkit::WebView::Property::USER_AGENT:
      {
        std::string input;
        if(value.Get(input))
        {
          impl.SetUserAgent(input);
        }
        break;
      }
      case Toolkit::WebView::Property::SCROLL_POSITION:
      {
        Vector2 input;
        if(value.Get(input))
        {
          impl.SetScrollPosition(input.x, input.y);
        }
        break;
      }
      case Toolkit::WebView::Property::VIDEO_HOLE_ENABLED:
      {
        bool input;
        if(value.Get(input))
        {
          impl.EnableVideoHole(input);
        }
        break;
      }
      case Toolkit::WebView::Property::MOUSE_EVENTS_ENABLED:
      {
        bool input;
        if(value.Get(input))
        {
          impl.EnableMouseEvents(input);
        }
        break;
      }
      case Toolkit::WebView::Property::KEY_EVENTS_ENABLED:
      {
        bool input;
        if(value.Get(input))
        {
          impl.EnableKeyEvents(input);
        }
        break;
      }
      case Toolkit::WebView::Property::DOCUMENT_BACKGROUND_COLOR:
      {
        Vector4 input;
        if(value.Get(input))
        {
          impl.SetDocumentBackgroundColor(input);
        }
        break;
      }
      case Toolkit::WebView::Property::TILES_CLEARED_WHEN_HIDDEN:
      {
        bool input;
        if(value.Get(input))
        {
          impl.ClearTilesWhenHidden(input);
        }
        break;
      }
      case Toolkit::WebView::Property::TILE_COVER_AREA_MULTIPLIER:
      {
        float input;
        if(value.Get(input))
        {
          impl.SetTileCoverAreaMultiplier(input);
        }
        break;
      }
      case Toolkit::WebView::Property::CURSOR_ENABLED_BY_CLIENT:
      {
        bool input;
        if(value.Get(input))
        {
          impl.EnableCursorByClient(input);
        }
        break;
      }
      case Toolkit::WebView::Property::PAGE_ZOOM_FACTOR:
      {
        float input;
        if(value.Get(input))
        {
          impl.SetPageZoomFactor(input);
        }
        break;
      }
      case Toolkit::WebView::Property::TEXT_ZOOM_FACTOR:
      {
        float input;
        if(value.Get(input))
        {
          impl.SetTextZoomFactor(input);
        }
        break;
      }
      default:
        break;
    }
  }
}

Property::Value WebView::GetProperty(BaseObject* object, Property::Index propertyIndex)
{
  Property::Value value;

  Toolkit::WebView webView = Toolkit::WebView::DownCast(Dali::BaseHandle(object));

  if(webView)
  {
    WebView& impl = GetImpl(webView);
    switch(propertyIndex)
    {
      case Toolkit::WebView::Property::URL:
      {
        value = impl.GetUrl();
        break;
      }
      case Toolkit::WebView::Property::USER_AGENT:
      {
        value = impl.GetUserAgent();
        break;
      }
      case Toolkit::WebView::Property::SCROLL_POSITION:
      {
        value = impl.GetScrollPosition();
        break;
      }
      case Toolkit::WebView::Property::SCROLL_SIZE:
      {
        value = impl.GetScrollSize();
        break;
      }
      case Toolkit::WebView::Property::CONTENT_SIZE:
      {
        value = impl.GetContentSize();
        break;
      }
      case Toolkit::WebView::Property::TITLE:
      {
        value = impl.GetTitle();
        break;
      }
      case Toolkit::WebView::Property::VIDEO_HOLE_ENABLED:
      {
        value = impl.mVideoHoleEnabled;
        break;
      }
      case Toolkit::WebView::Property::MOUSE_EVENTS_ENABLED:
      {
        value = impl.mMouseEventsEnabled;
        break;
      }
      case Toolkit::WebView::Property::KEY_EVENTS_ENABLED:
      {
        value = impl.mKeyEventsEnabled;
        break;
      }
      case Toolkit::WebView::Property::SELECTED_TEXT:
      {
        value = impl.GetSelectedText();
        break;
      }
      case Toolkit::WebView::Property::PAGE_ZOOM_FACTOR:
      {
        value = impl.GetPageZoomFactor();
        break;
      }
      case Toolkit::WebView::Property::TEXT_ZOOM_FACTOR:
      {
        value = impl.GetTextZoomFactor();
        break;
      }
      case Toolkit::WebView::Property::LOAD_PROGRESS_PERCENTAGE:
      {
        value = impl.GetLoadProgressPercentage();
        break;
      }
      default:
        break;
    }
  }

  return value;
}

void WebView::SetScrollPosition(int32_t x, int32_t y)
{
  if(mWebEngine)
  {
    mWebEngine.SetScrollPosition(x, y);
  }
}

Dali::Vector2 WebView::GetScrollPosition() const
{
  return mWebEngine ? mWebEngine.GetScrollPosition() : Dali::Vector2::ZERO;
}

Dali::Vector2 WebView::GetScrollSize() const
{
  return mWebEngine ? mWebEngine.GetScrollSize() : Dali::Vector2::ZERO;
}

Dali::Vector2 WebView::GetContentSize() const
{
  return mWebEngine ? mWebEngine.GetContentSize() : Dali::Vector2::ZERO;
}

std::string WebView::GetTitle() const
{
  return mWebEngine ? mWebEngine.GetTitle() : std::string();
}

void WebView::SetDocumentBackgroundColor(Dali::Vector4 color)
{
  if(mWebEngine)
  {
    mWebEngine.SetDocumentBackgroundColor(color);
  }
}

void WebView::ClearTilesWhenHidden(bool cleared)
{
  if(mWebEngine)
  {
    mWebEngine.ClearTilesWhenHidden(cleared);
  }
}

void WebView::SetTileCoverAreaMultiplier(float multiplier)
{
  if(mWebEngine)
  {
    mWebEngine.SetTileCoverAreaMultiplier(multiplier);
  }
}

void WebView::EnableCursorByClient(bool enabled)
{
  if(mWebEngine)
  {
    mWebEngine.EnableCursorByClient(enabled);
  }
}

std::string WebView::GetSelectedText() const
{
  return mWebEngine ? mWebEngine.GetSelectedText() : std::string();
}

std::string WebView::GetUrl() const
{
  return mWebEngine ? mWebEngine.GetUrl() : std::string();
}

std::string WebView::GetUserAgent() const
{
  return mWebEngine ? mWebEngine.GetUserAgent() : std::string();
}

void WebView::SetUserAgent(const std::string& userAgent)
{
  if(mWebEngine)
  {
    mWebEngine.SetUserAgent(userAgent);
  }
}

void WebView::EnableMouseEvents(bool enabled)
{
  if(mWebEngine)
  {
    mMouseEventsEnabled = enabled;
    mWebEngine.EnableMouseEvents(enabled);
  }
}

void WebView::EnableKeyEvents(bool enabled)
{
  if(mWebEngine)
  {
    mKeyEventsEnabled = enabled;
    mWebEngine.EnableKeyEvents(enabled);
  }
}

void WebView::SetPageZoomFactor(float zoomFactor)
{
  if(mWebEngine)
  {
    mWebEngine.SetPageZoomFactor(zoomFactor);
  }
}

float WebView::GetPageZoomFactor() const
{
  return mWebEngine ? mWebEngine.GetPageZoomFactor() : 0.0f;
}

void WebView::SetTextZoomFactor(float zoomFactor)
{
  if(mWebEngine)
  {
    mWebEngine.SetTextZoomFactor(zoomFactor);
  }
}

float WebView::GetTextZoomFactor() const
{
  return mWebEngine ? mWebEngine.GetTextZoomFactor() : 0.0f;
}

float WebView::GetLoadProgressPercentage() const
{
  return mWebEngine ? mWebEngine.GetLoadProgressPercentage() : 0.0f;
}

bool WebView::SetVisibility(bool visible)
{
  return mWebEngine ? mWebEngine.SetVisibility(visible) : false;
}

WebView::WebViewAccessible::WebViewAccessible(Dali::Actor self, Dali::WebEngine& webEngine)
: ControlAccessible(self), mRemoteChild{}, mWebEngine{webEngine}
{
  mRemoteChild.SetParent(this);

  Dali::Accessibility::Bridge::EnabledSignal().Connect(this, &WebViewAccessible::OnAccessibilityEnabled);
  Dali::Accessibility::Bridge::DisabledSignal().Connect(this, &WebViewAccessible::OnAccessibilityDisabled);

  if(Dali::Accessibility::IsUp())
  {
    OnAccessibilityEnabled();
  }
  else
  {
    OnAccessibilityDisabled();
  }
}

void WebView::WebViewAccessible::UpdateAttributes(Dali::Accessibility::Attributes& attributes) const
{
  static const std::string childBusKey = "child_bus";

  ControlAccessible::UpdateAttributes(attributes);

  if(mRemoteChild.GetAddress())
  {
    attributes.insert_or_assign(childBusKey, mRemoteChild.GetAddress().GetBus());
  }
  else
  {
    attributes.erase(childBusKey);
  }
}

void WebView::WebViewAccessible::DoGetChildren(std::vector<Dali::Accessibility::Accessible*>& children)
{
  if(!mRemoteChild.GetAddress())
  {
    DALI_LOG_DEBUG_INFO("Try setting address as it is not set on initialize");
    SetRemoteChildAddress(mWebEngine.GetAccessibilityAddress());
  }

  if(mRemoteChild.GetAddress())
  {
    // DoGetChildren is called at most once per every OnChildrenChanged.
    // We have only one OnChildrenChanged in this case, so EmbedSocket will be called only once.
    Accessibility::Bridge::GetCurrentBridge()->EmbedSocket(GetAddress(), mRemoteChild.GetAddress());
    children.push_back(&mRemoteChild);
  }
}

void WebView::WebViewAccessible::OnAccessibilityEnabled()
{
  if(!mWebEngine)
  {
    return;
  }

  mWebEngine.ActivateAccessibility(true);
  SetRemoteChildAddress(mWebEngine.GetAccessibilityAddress());
}

void WebView::WebViewAccessible::OnAccessibilityDisabled()
{
  if(!mWebEngine)
  {
    return;
  }

  SetRemoteChildAddress({});
  mWebEngine.ActivateAccessibility(false);
}

void WebView::WebViewAccessible::SetRemoteChildAddress(Dali::Accessibility::Address address)
{
  mRemoteChild.SetAddress(address);
  OnChildrenChanged();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
