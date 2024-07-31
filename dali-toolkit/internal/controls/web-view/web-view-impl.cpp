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
#include <dali-toolkit/internal/controls/web-view/web-view-impl.h>

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
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/public-api/adaptor-framework/native-image-source.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/web-view/web-back-forward-list.h>
#include <dali-toolkit/devel-api/controls/web-view/web-settings.h>
#include <dali-toolkit/devel-api/visuals/visual-actions-devel.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/public-api/image-loader/image-url.h>
#include <dali-toolkit/public-api/image-loader/image.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>

#include <functional>
#include <memory>
#include <unordered_map>

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

std::unordered_map<Dali::WebEnginePlugin*, Dali::WeakHandle<Toolkit::WebView>>& GetPluginWebViewTable()
{
  static std::unordered_map<Dali::WebEnginePlugin*, Dali::WeakHandle<Toolkit::WebView>> pluginWebViewMap;
  return pluginWebViewMap;
}

enum class DisplayAreaCalculateOption
{
  PROPERTY         = 0, ///< Calculate display update area by property
  CURRENT_PROPERTY = 1, ///< Calculate display update area by current property
};

/**
 * @brief Helper function to calculate exact display area, offset and size.
 * It will be useful when view size is not integer value, or view size is not matched with texture size.
 *
 * @param[in] self The view itself.
 * @param[in] option Option of this calculation. Let we decide what kind of property will be used.
 * @return DisplayArea for this view.
 */
Rect<int32_t> CalculateDisplayArea(Dali::Actor self, DisplayAreaCalculateOption option)
{
  bool    positionUsesAnchorPoint = self.GetProperty<bool>(Actor::Property::POSITION_USES_ANCHOR_POINT);
  Vector3 actorSize               = (option == DisplayAreaCalculateOption::CURRENT_PROPERTY) ? self.GetCurrentProperty<Vector3>(Actor::Property::SIZE) * self.GetCurrentProperty<Vector3>(Actor::Property::SCALE)
                                                                                             : self.GetProperty<Vector3>(Actor::Property::SIZE) * self.GetProperty<Vector3>(Actor::Property::SCALE);
  Vector3 anchorPointOffSet       = actorSize * (positionUsesAnchorPoint ? self.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT) : AnchorPoint::TOP_LEFT);
  Vector2 screenPosition          = (option == DisplayAreaCalculateOption::CURRENT_PROPERTY) ? self.GetProperty<Vector2>(Actor::Property::SCREEN_POSITION)
                                                                                             : Dali::DevelActor::CalculateScreenPosition(self);

  Dali::Rect<int32_t> displayArea;
  displayArea.x      = screenPosition.x - anchorPointOffSet.x;
  displayArea.y      = screenPosition.y - anchorPointOffSet.y;
  displayArea.width  = actorSize.x;
  displayArea.height = actorSize.y;

  return displayArea;
}

constexpr Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

/**
 * @brief Helper function to calculate exact pixel area value by view and texture size.
 * It will be useful when view size is not integer value, or view size is not matched with texture size.
 *
 * @param[in] viewSize The size of view.
 * @param[in] textureWidth The width of texture, that must be integer type.
 * @param[in] textureHeight The height of texture, that must be integer type.
 * @return PixelArea value that image visual can use.
 */
Vector4 CalculatePixelArea(const Size& viewSize, const uint32_t textureWidth, const uint32_t textureHeight)
{
  float widthRatio  = textureWidth == 0u ? 1.0f : viewSize.width / static_cast<float>(textureWidth);
  float heightRatio = textureHeight == 0u ? 1.0f : viewSize.height / static_cast<float>(textureHeight);
  return Vector4(0.0f, 0.0f, widthRatio, heightRatio);
}

} // namespace

WebView::WebView(const std::string& locale, const std::string& timezoneId)
: Control(ControlBehaviour(ACTOR_BEHAVIOUR_DEFAULT | DISABLE_STYLE_CHANGE_SIGNALS)),
  mVisual(),
  mWebViewSize(Stage::GetCurrent().GetSize()),
  mWebEngine(),
  mLastRenderedNativeImageWidth(0u),
  mLastRenderedNativeImageHeight(0u),
  mWebViewArea(0, 0, mWebViewSize.width, mWebViewSize.height),
  mVideoHoleEnabled(false),
  mMouseEventsEnabled(true),
  mKeyEventsEnabled(true),
  mVisualChangeRequired(false),
  mScreenshotCapturedCallback{nullptr},
  mFrameRenderedCallback{nullptr}
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
  mLastRenderedNativeImageWidth(0u),
  mLastRenderedNativeImageHeight(0u),
  mWebViewArea(0, 0, mWebViewSize.width, mWebViewSize.height),
  mVideoHoleEnabled(false),
  mMouseEventsEnabled(true),
  mKeyEventsEnabled(true),
  mVisualChangeRequired(false),
  mScreenshotCapturedCallback{nullptr},
  mFrameRenderedCallback{nullptr}
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
    auto iter = GetPluginWebViewTable().find(mWebEngine.GetPlugin());
    if(iter != GetPluginWebViewTable().end())
    {
      GetPluginWebViewTable().erase(iter);
    }
    mWebEngine.Destroy();
  }
}

Toolkit::WebView WebView::New()
{
  WebView*         impl   = new WebView();
  Toolkit::WebView handle = Toolkit::WebView(*impl);
  if(impl->GetPlugin())
  {
    GetPluginWebViewTable()[impl->GetPlugin()] = handle;
  }
  impl->Initialize();
  return handle;
}

Toolkit::WebView WebView::New(const std::string& locale, const std::string& timezoneId)
{
  WebView*         impl   = new WebView(locale, timezoneId);
  Toolkit::WebView handle = Toolkit::WebView(*impl);
  if(impl->GetPlugin())
  {
    GetPluginWebViewTable()[impl->GetPlugin()] = handle;
  }
  impl->Initialize();
  return handle;
}

Toolkit::WebView WebView::New(uint32_t argc, char** argv)
{
  WebView*         impl   = new WebView(argc, argv);
  Toolkit::WebView handle = Toolkit::WebView(*impl);
  if(impl->GetPlugin())
  {
    GetPluginWebViewTable()[impl->GetPlugin()] = handle;
  }
  impl->Initialize();
  return handle;
}

Toolkit::WebView WebView::FindWebView(Dali::WebEnginePlugin* plugin)
{
  auto iter = GetPluginWebViewTable().find(plugin);
  if(iter != GetPluginWebViewTable().end())
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
  self.SetProperty(DevelActor::Property::TOUCH_FOCUSABLE, true);
  self.TouchedSignal().Connect(this, &WebView::OnTouchEvent);
  self.HoveredSignal().Connect(this, &WebView::OnHoverEvent);
  self.WheelEventSignal().Connect(this, &WebView::OnWheelEvent);
  Dali::DevelActor::VisibilityChangedSignal(self).Connect(this, &WebView::OnVisibilityChanged);

  mWebViewVisibleState |= WebViewVisibleStateFlag::SELF_SHOW;

  mPositionUpdateNotification = self.AddPropertyNotification(Actor::Property::WORLD_POSITION, StepCondition(1.0f, 1.0f));
  mSizeUpdateNotification     = self.AddPropertyNotification(Actor::Property::SIZE, StepCondition(1.0f, 1.0f));
  mScaleUpdateNotification    = self.AddPropertyNotification(Actor::Property::WORLD_SCALE, StepCondition(0.1f, 1.0f));
  mPositionUpdateNotification.NotifySignal().Connect(this, &WebView::OnDisplayAreaUpdated);
  mSizeUpdateNotification.NotifySignal().Connect(this, &WebView::OnDisplayAreaUpdated);
  mScaleUpdateNotification.NotifySignal().Connect(this, &WebView::OnDisplayAreaUpdated);

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

void WebView::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  if(!mWebEngine)
  {
    return;
  }

  auto displayArea = CalculateDisplayArea(Self(), DisplayAreaCalculateOption::PROPERTY);

  SetDisplayArea(displayArea);
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
    mWebEngine.EvaluateJavaScript(script, std::move(resultHandler));
  }
}

void WebView::AddJavaScriptMessageHandler(const std::string& exposedObjectName, std::function<void(const std::string&)> handler)
{
  if(mWebEngine)
  {
    mWebEngine.AddJavaScriptMessageHandler(exposedObjectName, std::move(handler));
  }
}

void WebView::RegisterJavaScriptAlertCallback(Dali::WebEnginePlugin::JavaScriptAlertCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterJavaScriptAlertCallback(std::move(callback));
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
    mWebEngine.RegisterJavaScriptConfirmCallback(std::move(callback));
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
    mWebEngine.RegisterJavaScriptPromptCallback(std::move(callback));
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
    result = mWebEngine.CreateHitTestAsynchronously(x, y, mode, std::move(callback));
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
  mScreenshotCapturedCallback = std::move(callback);
  return mWebEngine ? mWebEngine.GetScreenshotAsynchronously(viewArea, scaleFactor, std::bind(&WebView::OnScreenshotCaptured, this, std::placeholders::_1)) : false;
}

bool WebView::CheckVideoPlayingAsynchronously(Dali::WebEnginePlugin::VideoPlayingCallback callback)
{
  return mWebEngine ? mWebEngine.CheckVideoPlayingAsynchronously(std::move(callback)) : false;
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
    mWebEngine.RegisterGeolocationPermissionCallback(std::move(callback));
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
    mWebEngine.RegisterPageLoadStartedCallback(std::move(callback));
  }
}

void WebView::RegisterPageLoadInProgressCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterPageLoadInProgressCallback(std::move(callback));
  }
}

void WebView::RegisterPageLoadFinishedCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterPageLoadFinishedCallback(std::move(callback));
  }
}

void WebView::RegisterPageLoadErrorCallback(Dali::WebEnginePlugin::WebEnginePageLoadErrorCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterPageLoadErrorCallback(std::move(callback));
  }
}

void WebView::RegisterScrollEdgeReachedCallback(Dali::WebEnginePlugin::WebEngineScrollEdgeReachedCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterScrollEdgeReachedCallback(std::move(callback));
  }
}

void WebView::RegisterUrlChangedCallback(Dali::WebEnginePlugin::WebEngineUrlChangedCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterUrlChangedCallback(std::move(callback));
  }
}

void WebView::RegisterFormRepostDecidedCallback(Dali::WebEnginePlugin::WebEngineFormRepostDecidedCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterFormRepostDecidedCallback(std::move(callback));
  }
}

void WebView::RegisterFrameRenderedCallback(Dali::WebEnginePlugin::WebEngineFrameRenderedCallback callback)
{
  mFrameRenderedCallback = std::move(callback);
}

void WebView::RegisterConsoleMessageReceivedCallback(Dali::WebEnginePlugin::WebEngineConsoleMessageReceivedCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterConsoleMessageReceivedCallback(std::move(callback));
  }
}

void WebView::RegisterResponsePolicyDecidedCallback(Dali::WebEnginePlugin::WebEngineResponsePolicyDecidedCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterResponsePolicyDecidedCallback(std::move(callback));
  }
}

void WebView::RegisterNavigationPolicyDecidedCallback(Dali::WebEnginePlugin::WebEngineNavigationPolicyDecidedCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterNavigationPolicyDecidedCallback(std::move(callback));
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
    mWebEngine.RegisterNewWindowCreatedCallback(std::move(callback));
  }
}

void WebView::RegisterCertificateConfirmedCallback(Dali::WebEnginePlugin::WebEngineCertificateCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterCertificateConfirmedCallback(std::move(callback));
  }
}

void WebView::RegisterSslCertificateChangedCallback(Dali::WebEnginePlugin::WebEngineCertificateCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterSslCertificateChangedCallback(std::move(callback));
  }
}

void WebView::RegisterHttpAuthHandlerCallback(Dali::WebEnginePlugin::WebEngineHttpAuthHandlerCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterHttpAuthHandlerCallback(std::move(callback));
  }
}

void WebView::RegisterContextMenuShownCallback(Dali::WebEnginePlugin::WebEngineContextMenuShownCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterContextMenuShownCallback(std::move(callback));
  }
}

void WebView::RegisterContextMenuHiddenCallback(Dali::WebEnginePlugin::WebEngineContextMenuHiddenCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterContextMenuHiddenCallback(std::move(callback));
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
    mWebEngine.GetPlainTextAsynchronously(std::move(callback));
  }
}

void WebView::OnFrameRendered()
{
  if(mFrameRenderedCallback)
  {
    mFrameRenderedCallback();
  }

  // Make sure that mVisual is created only if required.
  if(mVisualChangeRequired || !mVisual)
  {
    // Reset flag
    mVisualChangeRequired = false;

    auto nativeImageSourcePtr = mWebEngine.GetNativeImageSource();

    mLastRenderedNativeImageWidth  = nativeImageSourcePtr->GetWidth();
    mLastRenderedNativeImageHeight = nativeImageSourcePtr->GetHeight();

    Dali::Toolkit::ImageUrl nativeImageUrl = Dali::Toolkit::Image::GenerateUrl(nativeImageSourcePtr);

    mVisual = Toolkit::VisualFactory::Get().CreateVisual(
      {{Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE},
       {Toolkit::ImageVisual::Property::URL, nativeImageUrl.GetUrl()},
       {Toolkit::ImageVisual::Property::PIXEL_AREA, FULL_TEXTURE_RECT},
       {Toolkit::ImageVisual::Property::WRAP_MODE_U, Dali::WrapMode::CLAMP_TO_EDGE},
       {Toolkit::ImageVisual::Property::WRAP_MODE_V, Dali::WrapMode::CLAMP_TO_EDGE}});

    if(mVisual)
    {
      DevelControl::RegisterVisual(*this, Toolkit::WebView::Property::URL, mVisual);
      EnableBlendMode(!mVideoHoleEnabled);
    }
  }
}

void WebView::OnDisplayAreaUpdated(Dali::PropertyNotification& /*source*/)
{
  if(!mWebEngine)
  {
    return;
  }

  auto displayArea = CalculateDisplayArea(Self(), DisplayAreaCalculateOption::CURRENT_PROPERTY);

  SetDisplayArea(displayArea);
}

void WebView::OnVisibilityChanged(Actor actor, bool isVisible, Dali::DevelActor::VisibilityChange::Type type)
{
  if(type == Dali::DevelActor::VisibilityChange::Type::SELF)
  {
    if(isVisible)
    {
      mWebViewVisibleState |= WebViewVisibleStateFlag::SELF_SHOW;
    }
    else
    {
      mWebViewVisibleState &= ~WebViewVisibleStateFlag::SELF_SHOW;
    }
  }
  else if(type == Dali::DevelActor::VisibilityChange::Type::PARENT)
  {
    if(isVisible)
    {
      mWebViewVisibleState |= WebViewVisibleStateFlag::PARENT_SHOW;
      // TODO : We should consider double-hide called from parent
    }
    else
    {
      mWebViewVisibleState &= ~WebViewVisibleStateFlag::PARENT_SHOW;
    }
  }
  ApplyVisibilityCheck();
}

void WebView::OnWindowVisibilityChanged(Window window, bool visible)
{
  if(visible)
  {
    mWebViewVisibleState |= WebViewVisibleStateFlag::WINDOW_SHOW;
  }
  else
  {
    mWebViewVisibleState &= ~WebViewVisibleStateFlag::WINDOW_SHOW;
  }
  ApplyVisibilityCheck();
}

void WebView::OnScreenshotCaptured(Dali::PixelData pixel)
{
  if(mScreenshotCapturedCallback)
  {
    Dali::Toolkit::ImageView imageView = CreateImageView(pixel);
    mScreenshotCapturedCallback(imageView);
  }
}

void WebView::SetDisplayArea(const Dali::Rect<int32_t>& displayArea)
{
  Size displaySize = Size(displayArea.width, displayArea.height);
  if(mWebViewSize != displaySize)
  {
    mWebViewSize = displaySize;
  }

  if(mWebViewArea != displayArea)
  {
    // WebEngine visual size changed. we have to re-create visual.
    mVisualChangeRequired = true;

    // Change old visual's pixel area matched as changed web view size
    if(mVisual)
    {
      auto pixelArea = CalculatePixelArea(mWebViewSize, mLastRenderedNativeImageWidth, mLastRenderedNativeImageHeight);
      Toolkit::GetImplementation(mVisual).DoAction(Toolkit::DevelVisual::Action::UPDATE_PROPERTY, {{Toolkit::ImageVisual::Property::PIXEL_AREA, pixelArea}});
    }

    mWebViewArea = displayArea;
    mWebEngine.UpdateDisplayArea(mWebViewArea);
  }
}

void WebView::OnSceneConnection(int depth)
{
  mWebViewVisibleState |= WebViewVisibleStateFlag::SCENE_ON;
  mWebViewVisibleState |= WebViewVisibleStateFlag::PARENT_SHOW;
  // TODO : We should consider already hided parent
  Window window = DevelWindow::Get(Self());
  if(window)
  {
    // Hold the weak handle of the placement window.
    mPlacementWindow = window;
    if(window.IsVisible())
    {
      mWebViewVisibleState |= WebViewVisibleStateFlag::WINDOW_SHOW;
    }
    else
    {
      mWebViewVisibleState &= ~WebViewVisibleStateFlag::WINDOW_SHOW;
    }
    DevelWindow::VisibilityChangedSignal(window).Connect(this, &WebView::OnWindowVisibilityChanged);
  }
  ApplyVisibilityCheck();
  Control::OnSceneConnection(depth);
  EnableBlendMode(!mVideoHoleEnabled);
}

void WebView::OnSceneDisconnection()
{
  mWebViewVisibleState &= ~WebViewVisibleStateFlag::SCENE_ON;
  mWebViewVisibleState &= ~WebViewVisibleStateFlag::WINDOW_SHOW;
  mWebViewVisibleState &= ~WebViewVisibleStateFlag::PARENT_SHOW;
  Window window = mPlacementWindow.GetHandle();
  if(window)
  {
    DevelWindow::VisibilityChangedSignal(window).Disconnect(this, &WebView::OnWindowVisibilityChanged);
    mPlacementWindow.Reset();
  }
  ApplyVisibilityCheck();
  Control::OnSceneDisconnection();
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

void WebView::ApplyVisibilityCheck()
{
  SetVisibility(mWebViewVisibleState == WebViewVisibleStateFlag::VISIBLE);
}

WebView::WebViewAccessible::WebViewAccessible(Dali::Actor self, Dali::WebEngine& webEngine)
: ControlAccessible(self),
  mRemoteChild{},
  mWebEngine{webEngine}
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

Dali::Accessibility::Attributes WebView::WebViewAccessible::GetAttributes() const
{
  auto attributes = DevelControl::ControlAccessible::GetAttributes();

  if(mRemoteChild.GetAddress())
  {
    attributes.insert_or_assign("child_bus", mRemoteChild.GetAddress().GetBus());
  }

  return attributes;
}

void WebView::WebViewAccessible::DoGetChildren(std::vector<Dali::Accessibility::Accessible*>& children)
{
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
