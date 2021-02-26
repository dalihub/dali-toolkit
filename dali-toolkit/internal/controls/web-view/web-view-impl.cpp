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

// CLASS HEADER
#include "web-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/web-engine-back-forward-list.h>
#include <dali/devel-api/adaptor-framework/web-engine-context.h>
#include <dali/devel-api/adaptor-framework/web-engine-cookie-manager.h>
#include <dali/devel-api/adaptor-framework/web-engine-form-repost-decision.h>
#include <dali/devel-api/adaptor-framework/web-engine-request-interceptor.h>
#include <dali/devel-api/adaptor-framework/web-engine-settings.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/public-api/adaptor-framework/native-image-source.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <cstring>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/web-view/web-back-forward-list.h>
#include <dali-toolkit/devel-api/controls/web-view/web-context.h>
#include <dali-toolkit/devel-api/controls/web-view/web-cookie-manager.h>
#include <dali-toolkit/devel-api/controls/web-view/web-form-repost-decision.h>
#include <dali-toolkit/devel-api/controls/web-view/web-settings.h>
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/public-api/image-loader/image.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>

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

DALI_SIGNAL_REGISTRATION(Toolkit, WebView, "pageLoadStarted",    PAGE_LOAD_STARTED_SIGNAL    )
DALI_SIGNAL_REGISTRATION(Toolkit, WebView, "pageLoadInProgress", PAGE_LOAD_IN_PROGRESS_SIGNAL)
DALI_SIGNAL_REGISTRATION(Toolkit, WebView, "pageLoadFinished",   PAGE_LOAD_FINISHED_SIGNAL   )
DALI_SIGNAL_REGISTRATION(Toolkit, WebView, "pageLoadError",      PAGE_LOAD_ERROR_SIGNAL      )
DALI_SIGNAL_REGISTRATION(Toolkit, WebView, "scrollEdgeReached",  SCROLL_EDGE_REACHED_SIGNAL  )
DALI_SIGNAL_REGISTRATION(Toolkit, WebView, "urlChanged",         URL_CHANGED_SIGNAL          )
DALI_SIGNAL_REGISTRATION(Toolkit, WebView, "formRepostDecision", FORM_REPOST_DECISION_SIGNAL )
DALI_SIGNAL_REGISTRATION(Toolkit, WebView, "frameRendered",      FRAME_RENDERED_SIGNAL       )
DALI_SIGNAL_REGISTRATION(Toolkit, WebView, "requestInterceptor", REQUEST_INTERCEPTOR_SIGNAL  )

DALI_TYPE_REGISTRATION_END()
// clang-format on

const std::string kEmptyString;

} // namespace

#define GET_ENUM_STRING(structName, inputExp) \
  Scripting::GetLinearEnumerationName<Toolkit::WebView::structName::Type>(static_cast<Toolkit::WebView::structName::Type>(inputExp), structName##_TABLE, structName##_TABLE_COUNT)

#define GET_ENUM_VALUE(structName, inputExp, outputExp) \
  Scripting::GetEnumerationProperty<Toolkit::WebView::structName::Type>(inputExp, structName##_TABLE, structName##_TABLE_COUNT, outputExp)

WebView::WebView(const std::string& locale, const std::string& timezoneId)
: Control(ControlBehaviour(ACTOR_BEHAVIOUR_DEFAULT | DISABLE_STYLE_CHANGE_SIGNALS)),
  mUrl(),
  mVisual(),
  mWebViewSize(Stage::GetCurrent().GetSize()),
  mWebEngine(),
  mPageLoadStartedSignal(),
  mPageLoadFinishedSignal(),
  mPageLoadErrorSignal(),
  mUrlChangedSignal(),
  mVideoHoleEnabled(true),
  mWebViewArea(0, 0, mWebViewSize.width, mWebViewSize.height),
  mMouseEventsEnabled(true),
  mKeyEventsEnabled(true)
{
  mWebEngine = Dali::WebEngine::New();

  // WebEngine is empty when it is not properly initialized.
  if(mWebEngine)
  {
    mWebEngine.Create(mWebViewSize.width, mWebViewSize.height, locale, timezoneId);
  }
}

WebView::WebView(int argc, char** argv)
: Control(ControlBehaviour(ACTOR_BEHAVIOUR_DEFAULT | DISABLE_STYLE_CHANGE_SIGNALS)),
  mUrl(),
  mVisual(),
  mWebViewSize(Stage::GetCurrent().GetSize()),
  mWebEngine(),
  mPageLoadStartedSignal(),
  mPageLoadFinishedSignal(),
  mPageLoadErrorSignal(),
  mUrlChangedSignal(),
  mVideoHoleEnabled(true),
  mWebViewArea(0, 0, mWebViewSize.width, mWebViewSize.height),
  mMouseEventsEnabled(true),
  mKeyEventsEnabled(true)
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
    mWebEngine.Destroy();
  }
}

Toolkit::WebView WebView::New()
{
  WebView*         impl   = new WebView();
  Toolkit::WebView handle = Toolkit::WebView(*impl);

  impl->Initialize();
  return handle;
}

Toolkit::WebView WebView::New(const std::string& locale, const std::string& timezoneId)
{
  WebView*         impl   = new WebView(locale, timezoneId);
  Toolkit::WebView handle = Toolkit::WebView(*impl);

  impl->Initialize();
  return handle;
}

Toolkit::WebView WebView::New(int argc, char** argv)
{
  WebView*         impl   = new WebView(argc, argv);
  Toolkit::WebView handle = Toolkit::WebView(*impl);

  impl->Initialize();
  return handle;
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
  mPositionUpdateNotification.NotifySignal().Connect(this, &WebView::UpdateDisplayArea);
  mSizeUpdateNotification.NotifySignal().Connect(this, &WebView::UpdateDisplayArea);
  mScaleUpdateNotification.NotifySignal().Connect(this, &WebView::UpdateDisplayArea);

  if(mWebEngine)
  {
    mWebEngine.PageLoadStartedSignal().Connect(this, &WebView::OnPageLoadStarted);
    mWebEngine.PageLoadInProgressSignal().Connect(this, &WebView::OnPageLoadInProgress);
    mWebEngine.PageLoadFinishedSignal().Connect(this, &WebView::OnPageLoadFinished);
    mWebEngine.PageLoadErrorSignal().Connect(this, &WebView::OnPageLoadError);
    mWebEngine.ScrollEdgeReachedSignal().Connect(this, &WebView::OnScrollEdgeReached);
    mWebEngine.UrlChangedSignal().Connect(this, &WebView::OnUrlChanged);
    mWebEngine.FormRepostDecisionSignal().Connect(this, &WebView::OnFormRepostDecision);
    mWebEngine.FrameRenderedSignal().Connect(this, &WebView::OnFrameRendered);
    mWebEngine.RequestInterceptorSignal().Connect(this, &WebView::OnInterceptRequest);

    mWebContext         = std::unique_ptr<Dali::Toolkit::WebContext>(new WebContext(mWebEngine.GetContext()));
    mWebCookieManager   = std::unique_ptr<Dali::Toolkit::WebCookieManager>(new WebCookieManager(mWebEngine.GetCookieManager()));
    mWebSettings        = std::unique_ptr<Dali::Toolkit::WebSettings>(new WebSettings(mWebEngine.GetSettings()));
    mWebBackForwardList = std::unique_ptr<Dali::Toolkit::WebBackForwardList>(new WebBackForwardList(mWebEngine.GetBackForwardList()));
  }
}

Dali::Toolkit::WebSettings* WebView::GetSettings() const
{
  return mWebSettings.get();
}

Dali::Toolkit::WebContext* WebView::GetContext() const
{
  return mWebContext.get();
}

Dali::Toolkit::WebCookieManager* WebView::GetCookieManager() const
{
  return mWebCookieManager.get();
}

Dali::Toolkit::WebBackForwardList* WebView::GetBackForwardList() const
{
  return mWebBackForwardList.get();
}

Dali::Toolkit::ImageView& WebView::GetFavicon()
{
  if(mWebEngine)
  {
    Dali::PixelData pixelData = mWebEngine.GetFavicon();
    mFaviconView              = CreateImageView(pixelData);
  }
  return mFaviconView;
}

void WebView::LoadUrl(const std::string& url)
{
  mUrl = url;
  if(mWebEngine)
  {
    Texture           texture        = Dali::Texture::New(*mWebEngine.GetNativeImageSource());
    const std::string nativeImageUrl = Dali::Toolkit::TextureManager::AddTexture(texture);
    mVisual                          = Toolkit::VisualFactory::Get().CreateVisual(
      {{Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE},
       {Toolkit::ImageVisual::Property::URL, nativeImageUrl}});

    if(mVisual)
    {
      // Clean up previously registered visual and add new one.
      DevelControl::RegisterVisual(*this, Toolkit::WebView::Property::URL, mVisual);
      mWebEngine.LoadUrl(url);
    }

    if(mVideoHoleEnabled)
    {
      EnableBlendMode(false);
    }
  }
}

void WebView::LoadHtmlString(const std::string& htmlString)
{
  if(mWebEngine)
  {
    Texture           texture        = Dali::Texture::New(*mWebEngine.GetNativeImageSource());
    const std::string nativeImageUrl = Dali::Toolkit::TextureManager::AddTexture(texture);
    mVisual                          = Toolkit::VisualFactory::Get().CreateVisual(
      {{Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE},
       {Toolkit::ImageVisual::Property::URL, nativeImageUrl}});

    if(mVisual)
    {
      DevelControl::RegisterVisual(*this, Toolkit::WebView::Property::URL, mVisual);
      mWebEngine.LoadHtmlString(htmlString);
    }

    if(mVideoHoleEnabled)
    {
      EnableBlendMode(false);
    }
  }
}

bool WebView::LoadHtmlStringOverrideCurrentEntry(const std::string& html, const std::string& basicUri, const std::string& unreachableUrl)
{
  if(!mWebEngine)
    return false;

  Texture           texture        = Dali::Texture::New(*mWebEngine.GetNativeImageSource());
  const std::string nativeImageUrl = Dali::Toolkit::TextureManager::AddTexture(texture);
  mVisual                          = Toolkit::VisualFactory::Get().CreateVisual(
    {{Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE},
     {Toolkit::ImageVisual::Property::URL, nativeImageUrl}});

  bool result = false;
  if(mVisual)
  {
    DevelControl::RegisterVisual(*this, Toolkit::WebView::Property::URL, mVisual);
    result = mWebEngine.LoadHtmlStringOverrideCurrentEntry(html, basicUri, unreachableUrl);
  }

  if(mVideoHoleEnabled)
  {
    EnableBlendMode(false);
  }
  return result;
}

bool WebView::LoadContents(const std::string& contents, uint32_t contentSize, const std::string& mimeType, const std::string& encoding, const std::string& baseUri)
{
  if(!mWebEngine)
    return false;

  Texture           texture        = Dali::Texture::New(*mWebEngine.GetNativeImageSource());
  const std::string nativeImageUrl = Dali::Toolkit::TextureManager::AddTexture(texture);
  mVisual                          = Toolkit::VisualFactory::Get().CreateVisual(
    {{Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE},
     {Toolkit::ImageVisual::Property::URL, nativeImageUrl}});

  bool result = false;
  if(mVisual)
  {
    DevelControl::RegisterVisual(*this, Toolkit::WebView::Property::URL, mVisual);
    result = mWebEngine.LoadContents(contents, contentSize, mimeType, encoding, baseUri);
  }

  if(mVideoHoleEnabled)
  {
    EnableBlendMode(false);
  }
  return result;
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

void WebView::ScrollBy(int deltaX, int deltaY)
{
  if(mWebEngine)
  {
    mWebEngine.ScrollBy(deltaX, deltaY);
  }
}

bool WebView::ScrollEdgeBy(int deltaX, int deltaY)
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

Dali::Toolkit::ImageView WebView::GetScreenshot(Dali::Rect<int> viewArea, float scaleFactor)
{
  Dali::Toolkit::ImageView imageView;
  if(mWebEngine)
  {
    Dali::PixelData pixelData = mWebEngine.GetScreenshot(viewArea, scaleFactor);
    imageView                 = CreateImageView(pixelData);
  }
  return imageView;
}

bool WebView::GetScreenshotAsynchronously(Dali::Rect<int> viewArea, float scaleFactor, Dali::Toolkit::WebView::WebViewScreenshotCapturedCallback callback)
{
  mScreenshotCapturedCallback = callback;
  return mWebEngine ? mWebEngine.GetScreenshotAsynchronously(viewArea, scaleFactor, std::bind(&WebView::OnScreenshotCaptured, this, std::placeholders::_1)) : false;
}

bool WebView::CheckVideoPlayingAsynchronously(Dali::WebEnginePlugin::VideoPlayingCallback callback)
{
  return mWebEngine ? mWebEngine.CheckVideoPlayingAsynchronously(callback) : false;
}

void WebView::RegisterGeolocationPermissionCallback(Dali::WebEnginePlugin::GeolocationPermissionCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.RegisterGeolocationPermissionCallback(callback);
  }
}

void WebView::UpdateDisplayArea(Dali::PropertyNotification& /*source*/)
{
  if(!mWebEngine)
    return;

  Actor self(Self());

  bool    positionUsesAnchorPoint = self.GetProperty<bool>(Actor::Property::POSITION_USES_ANCHOR_POINT);
  Vector3 actorSize               = self.GetCurrentProperty<Vector3>(Actor::Property::SIZE) * self.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  Vector3 anchorPointOffSet       = actorSize * (positionUsesAnchorPoint ? self.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT) : AnchorPoint::TOP_LEFT);
  Vector2 screenPosition          = self.GetProperty<Vector2>(Actor::Property::SCREEN_POSITION);

  Dali::Rect<int> displayArea;
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

Dali::Toolkit::ImageView WebView::CreateImageView(Dali::PixelData pixel)
{
  std::string              url       = Dali::Toolkit::Image::GenerateUrl(pixel);
  Dali::Toolkit::ImageView imageView = Dali::Toolkit::ImageView::New(url);
  imageView.SetProperty(Dali::Actor::Property::SIZE, Vector2(pixel.GetWidth(), pixel.GetHeight()));
  return imageView;
}

Dali::Toolkit::WebView::WebViewPageLoadSignalType& WebView::PageLoadStartedSignal()
{
  return mPageLoadStartedSignal;
}

Dali::Toolkit::WebView::WebViewPageLoadSignalType& WebView::PageLoadInProgressSignal()
{
  return mPageLoadInProgressSignal;
}

Dali::Toolkit::WebView::WebViewPageLoadSignalType& WebView::PageLoadFinishedSignal()
{
  return mPageLoadFinishedSignal;
}

Dali::Toolkit::WebView::WebViewPageLoadErrorSignalType& WebView::PageLoadErrorSignal()
{
  return mPageLoadErrorSignal;
}

Dali::Toolkit::WebView::WebViewScrollEdgeReachedSignalType& WebView::ScrollEdgeReachedSignal()
{
  return mScrollEdgeReachedSignal;
}

Dali::Toolkit::WebView::WebViewUrlChangedSignalType& WebView::UrlChangedSignal()
{
  return mUrlChangedSignal;
}

Dali::Toolkit::WebView::WebViewFormRepostDecisionSignalType& WebView::FormRepostDecisionSignal()
{
  return mFormRepostDecisionSignal;
}

Dali::Toolkit::WebView::WebViewFrameRenderedSignalType& WebView::FrameRenderedSignal()
{
  return mFrameRenderedSignal;
}

Dali::Toolkit::WebView::WebViewRequestInterceptorSignalType& WebView::RequestInterceptorSignal()
{
  return mRequestInterceptorSignal;
}

void WebView::OnPageLoadStarted(const std::string& url)
{
  if(!mPageLoadStartedSignal.Empty())
  {
    Dali::Toolkit::WebView handle(GetOwner());
    mPageLoadStartedSignal.Emit(handle, url);
  }
}

void WebView::OnPageLoadInProgress(const std::string& url)
{
  if(!mPageLoadInProgressSignal.Empty())
  {
    Dali::Toolkit::WebView handle(GetOwner());
    mPageLoadInProgressSignal.Emit(handle, url);
  }
}

void WebView::OnPageLoadFinished(const std::string& url)
{
  if(!mPageLoadFinishedSignal.Empty())
  {
    Dali::Toolkit::WebView handle(GetOwner());
    mPageLoadFinishedSignal.Emit(handle, url);
  }
}

void WebView::OnPageLoadError(const std::string& url, int errorCode)
{
  if(!mPageLoadErrorSignal.Empty())
  {
    Dali::Toolkit::WebView handle(GetOwner());
    mPageLoadErrorSignal.Emit(handle, url, static_cast<Toolkit::WebView::LoadErrorCode>(errorCode));
  }
}

void WebView::OnScrollEdgeReached(Dali::WebEnginePlugin::ScrollEdge edge)
{
  if(!mScrollEdgeReachedSignal.Empty())
  {
    Dali::Toolkit::WebView handle(GetOwner());
    mScrollEdgeReachedSignal.Emit(handle, edge);
  }
}

void WebView::OnUrlChanged(const std::string& url)
{
  if(!mUrlChangedSignal.Empty())
  {
    Dali::Toolkit::WebView handle(GetOwner());
    mUrlChangedSignal.Emit(handle, url);
  }
}

void WebView::OnFormRepostDecision(std::shared_ptr<Dali::WebEngineFormRepostDecision> decision)
{
  if(!mFormRepostDecisionSignal.Empty())
  {
    Dali::Toolkit::WebView                                handle(GetOwner());
    std::shared_ptr<Dali::Toolkit::WebFormRepostDecision> repostDecision(new Dali::Toolkit::WebFormRepostDecision(decision));
    mFormRepostDecisionSignal.Emit(handle, std::move(repostDecision));
  }
}

void WebView::OnFrameRendered()
{
  if(!mFrameRenderedSignal.Empty())
  {
    Dali::Toolkit::WebView handle(GetOwner());
    mFrameRenderedSignal.Emit(handle);
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

void WebView::OnInterceptRequest(std::shared_ptr<Dali::WebEngineRequestInterceptor> interceptor)
{
  if(!mRequestInterceptorSignal.Empty())
  {
    Dali::Toolkit::WebView handle(GetOwner());
    mRequestInterceptorSignal.Emit(handle, std::move(interceptor));
  }
}

bool WebView::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  Dali::BaseHandle handle(object);

  bool             connected = false;
  Toolkit::WebView webView   = Toolkit::WebView::DownCast(handle);

  if(0 == strcmp(signalName.c_str(), PAGE_LOAD_STARTED_SIGNAL))
  {
    webView.PageLoadStartedSignal().Connect(tracker, functor);
    connected = true;
  }
  else if(0 == strcmp(signalName.c_str(), PAGE_LOAD_IN_PROGRESS_SIGNAL))
  {
    webView.PageLoadInProgressSignal().Connect(tracker, functor);
    connected = true;
  }
  else if(0 == strcmp(signalName.c_str(), PAGE_LOAD_FINISHED_SIGNAL))
  {
    webView.PageLoadFinishedSignal().Connect(tracker, functor);
    connected = true;
  }
  else if(0 == strcmp(signalName.c_str(), PAGE_LOAD_ERROR_SIGNAL))
  {
    webView.PageLoadErrorSignal().Connect(tracker, functor);
    connected = true;
  }
  else if(0 == strcmp(signalName.c_str(), SCROLL_EDGE_REACHED_SIGNAL))
  {
    webView.ScrollEdgeReachedSignal().Connect(tracker, functor);
    connected = true;
  }
  else if(0 == strcmp(signalName.c_str(), URL_CHANGED_SIGNAL))
  {
    webView.UrlChangedSignal().Connect(tracker, functor);
    connected = true;
  }
  else if(0 == strcmp(signalName.c_str(), FORM_REPOST_DECISION_SIGNAL))
  {
    webView.FormRepostDecisionSignal().Connect(tracker, functor);
    connected = true;
  }
  else if(0 == strcmp(signalName.c_str(), FRAME_RENDERED_SIGNAL))
  {
    webView.FrameRenderedSignal().Connect(tracker, functor);
    connected = true;
  }
  else if(0 == strcmp(signalName.c_str(), REQUEST_INTERCEPTOR_SIGNAL))
  {
    webView.RequestInterceptorSignal().Connect(tracker, functor);
    connected = true;
  }

  return connected;
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

void WebView::OnSceneConnection(int depth)
{
  Control::OnSceneConnection(depth);

  EnableBlendMode(!mVideoHoleEnabled);
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
        value = impl.mUrl;
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
  if(mWebEngine)
  {
    result = mWebEngine.SendHoverEvent(hover);
  }
  return result;
}

bool WebView::OnWheelEvent(Actor actor, const Dali::WheelEvent& wheel)
{
  bool result = false;
  if(mWebEngine)
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

void WebView::SetScrollPosition(int x, int y)
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
  return mWebEngine ? mWebEngine.GetTitle() : kEmptyString;
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
  return mWebEngine ? mWebEngine.GetSelectedText() : kEmptyString;
}

const std::string& WebView::GetUserAgent() const
{
  return mWebEngine ? mWebEngine.GetUserAgent() : kEmptyString;
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

#undef GET_ENUM_STRING
#undef GET_ENUM_VALUE

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
