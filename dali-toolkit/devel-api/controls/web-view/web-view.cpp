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
#include <dali-toolkit/devel-api/controls/web-view/web-view.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/web-view/web-view-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>

namespace Dali
{
namespace Toolkit
{
WebView::WebView()
{
}

WebView::WebView(const WebView& WebView)
: Control(WebView)
{
}

WebView& WebView::operator=(const WebView& view)
{
  if(&view != this)
  {
    Control::operator=(view);
  }

  return *this;
}

WebView::~WebView()
{
}

WebView WebView::New()
{
  return Internal::WebView::New();
}

WebView WebView::New(const std::string& locale, const std::string& timezoneId)
{
  return Internal::WebView::New(locale, timezoneId);
}

WebView WebView::New(int argc, char** argv)
{
  return Internal::WebView::New(argc, argv);
}

WebView WebView::DownCast(BaseHandle handle)
{
  return Control::DownCast<WebView, Internal::WebView>(handle);
}

Dali::Toolkit::WebSettings* WebView::GetSettings() const
{
  return Dali::Toolkit::GetImpl(*this).GetSettings();
}

Dali::Toolkit::WebContext* WebView::GetContext() const
{
  return Dali::Toolkit::GetImpl(*this).GetContext();
}

Dali::Toolkit::WebCookieManager* WebView::GetCookieManager() const
{
  return Dali::Toolkit::GetImpl(*this).GetCookieManager();
}

Dali::Toolkit::WebBackForwardList* WebView::GetBackForwardList() const
{
  return Dali::Toolkit::GetImpl(*this).GetBackForwardList();
}

Dali::Toolkit::ImageView& WebView::GetFavicon()
{
  return Dali::Toolkit::GetImpl(*this).GetFavicon();
}

void WebView::LoadUrl(const std::string& url)
{
  Dali::Toolkit::GetImpl(*this).LoadUrl(url);
}

void WebView::LoadHtmlString(const std::string& htmlString)
{
  Dali::Toolkit::GetImpl(*this).LoadHtmlString(htmlString);
}

bool WebView::LoadHtmlStringOverrideCurrentEntry(const std::string& html, const std::string& basicUri, const std::string& unreachableUrl)
{
  return Dali::Toolkit::GetImpl(*this).LoadHtmlStringOverrideCurrentEntry(html, basicUri, unreachableUrl);
}

bool WebView::LoadContents(const std::string& contents, uint32_t contentSize, const std::string& mimeType, const std::string& encoding, const std::string& baseUri)
{
  return Dali::Toolkit::GetImpl(*this).LoadContents(contents, contentSize, mimeType, encoding, baseUri);
}

void WebView::Reload()
{
  Dali::Toolkit::GetImpl(*this).Reload();
}

bool WebView::ReloadWithoutCache()
{
  return Dali::Toolkit::GetImpl(*this).ReloadWithoutCache();
}

void WebView::StopLoading()
{
  Dali::Toolkit::GetImpl(*this).StopLoading();
}

void WebView::Suspend()
{
  Dali::Toolkit::GetImpl(*this).Suspend();
}

void WebView::Resume()
{
  Dali::Toolkit::GetImpl(*this).Resume();
}

void WebView::SuspendNetworkLoading()
{
  Dali::Toolkit::GetImpl(*this).SuspendNetworkLoading();
}

void WebView::ResumeNetworkLoading()
{
  Dali::Toolkit::GetImpl(*this).ResumeNetworkLoading();
}

bool WebView::AddCustomHeader(const std::string& name, const std::string& value)
{
  return Dali::Toolkit::GetImpl(*this).AddCustomHeader(name, value);
}

bool WebView::RemoveCustomHeader(const std::string& name)
{
  return Dali::Toolkit::GetImpl(*this).RemoveCustomHeader(name);
}

uint32_t WebView::StartInspectorServer(uint32_t port)
{
  return Dali::Toolkit::GetImpl(*this).StartInspectorServer(port);
}

bool WebView::StopInspectorServer()
{
  return Dali::Toolkit::GetImpl(*this).StopInspectorServer();
}

void WebView::ScrollBy(int deltaX, int deltaY)
{
  Dali::Toolkit::GetImpl(*this).ScrollBy(deltaX, deltaY);
}

bool WebView::ScrollEdgeBy(int deltaX, int deltaY)
{
  return Dali::Toolkit::GetImpl(*this).ScrollEdgeBy(deltaX, deltaY);
}

bool WebView::CanGoForward()
{
  return Dali::Toolkit::GetImpl(*this).CanGoForward();
}

void WebView::GoForward()
{
  Dali::Toolkit::GetImpl(*this).GoForward();
}

bool WebView::CanGoBack()
{
  return Dali::Toolkit::GetImpl(*this).CanGoBack();
}

void WebView::GoBack()
{
  Dali::Toolkit::GetImpl(*this).GoBack();
}

void WebView::EvaluateJavaScript(const std::string& script, std::function<void(const std::string&)> resultHandler)
{
  Dali::Toolkit::GetImpl(*this).EvaluateJavaScript(script, resultHandler);
}

void WebView::EvaluateJavaScript(const std::string& script)
{
  Dali::Toolkit::GetImpl(*this).EvaluateJavaScript(script, nullptr);
}

void WebView::AddJavaScriptMessageHandler(const std::string& exposedObjectName, std::function<void(const std::string&)> handler)
{
  Dali::Toolkit::GetImpl(*this).AddJavaScriptMessageHandler(exposedObjectName, handler);
}

void WebView::RegisterJavaScriptAlertCallback(Dali::WebEnginePlugin::JavaScriptAlertCallback callback)
{
  Dali::Toolkit::GetImpl(*this).RegisterJavaScriptAlertCallback(callback);
}

void WebView::JavaScriptAlertReply()
{
  Dali::Toolkit::GetImpl(*this).JavaScriptAlertReply();
}

void WebView::RegisterJavaScriptConfirmCallback(Dali::WebEnginePlugin::JavaScriptConfirmCallback callback)
{
  Dali::Toolkit::GetImpl(*this).RegisterJavaScriptConfirmCallback(callback);
}

void WebView::JavaScriptConfirmReply(bool confirmed)
{
  Dali::Toolkit::GetImpl(*this).JavaScriptConfirmReply(confirmed);
}

void WebView::RegisterJavaScriptPromptCallback(Dali::WebEnginePlugin::JavaScriptPromptCallback callback)
{
  Dali::Toolkit::GetImpl(*this).RegisterJavaScriptPromptCallback(callback);
}

void WebView::JavaScriptPromptReply(const std::string& result)
{
  Dali::Toolkit::GetImpl(*this).JavaScriptPromptReply(result);
}

void WebView::ClearHistory()
{
  Dali::Toolkit::GetImpl(*this).ClearHistory();
}

void WebView::ClearAllTilesResources()
{
  Dali::Toolkit::GetImpl(*this).ClearAllTilesResources();
}

void WebView::SetScaleFactor(float scaleFactor, Dali::Vector2 point)
{
  Dali::Toolkit::GetImpl(*this).SetScaleFactor(scaleFactor, point);
}

float WebView::GetScaleFactor() const
{
  return Dali::Toolkit::GetImpl(*this).GetScaleFactor();
}

void WebView::ActivateAccessibility(bool activated)
{
  Dali::Toolkit::GetImpl(*this).ActivateAccessibility(activated);
}

bool WebView::HighlightText(const std::string& text, Dali::WebEnginePlugin::FindOption options, uint32_t maxMatchCount)
{
  return Dali::Toolkit::GetImpl(*this).HighlightText(text, options, maxMatchCount);
}

void WebView::AddDynamicCertificatePath(const std::string& host, const std::string& certPath)
{
  Dali::Toolkit::GetImpl(*this).AddDynamicCertificatePath(host, certPath);
}

Dali::Toolkit::ImageView WebView::GetScreenshot(Dali::Rect<int> viewArea, float scaleFactor)
{
  return Dali::Toolkit::GetImpl(*this).GetScreenshot(viewArea, scaleFactor);
}

bool WebView::GetScreenshotAsynchronously(Dali::Rect<int> viewArea, float scaleFactor, Dali::Toolkit::WebView::WebViewScreenshotCapturedCallback callback)
{
  return Dali::Toolkit::GetImpl(*this).GetScreenshotAsynchronously(viewArea, scaleFactor, callback);
}

bool WebView::CheckVideoPlayingAsynchronously(Dali::WebEnginePlugin::VideoPlayingCallback callback)
{
  return Dali::Toolkit::GetImpl(*this).CheckVideoPlayingAsynchronously(callback);
}

void WebView::RegisterGeolocationPermissionCallback(Dali::WebEnginePlugin::GeolocationPermissionCallback callback)
{
  Dali::Toolkit::GetImpl(*this).RegisterGeolocationPermissionCallback(callback);
}

WebView::WebViewPageLoadSignalType& WebView::PageLoadStartedSignal()
{
  return Dali::Toolkit::GetImpl(*this).PageLoadStartedSignal();
}

WebView::WebViewPageLoadSignalType& WebView::PageLoadInProgressSignal()
{
  return Dali::Toolkit::GetImpl(*this).PageLoadInProgressSignal();
}

WebView::WebViewPageLoadSignalType& WebView::PageLoadFinishedSignal()
{
  return Dali::Toolkit::GetImpl(*this).PageLoadFinishedSignal();
}

WebView::WebViewPageLoadErrorSignalType& WebView::PageLoadErrorSignal()
{
  return Dali::Toolkit::GetImpl(*this).PageLoadErrorSignal();
}

WebView::WebViewScrollEdgeReachedSignalType& WebView::ScrollEdgeReachedSignal()
{
  return Dali::Toolkit::GetImpl(*this).ScrollEdgeReachedSignal();
}

WebView::WebViewUrlChangedSignalType& WebView::UrlChangedSignal()
{
  return Dali::Toolkit::GetImpl(*this).UrlChangedSignal();
}

WebView::WebViewFormRepostDecisionSignalType& WebView::FormRepostDecisionSignal()
{
  return Dali::Toolkit::GetImpl(*this).FormRepostDecisionSignal();
}

WebView::WebViewFrameRenderedSignalType& WebView::FrameRenderedSignal()
{
  return Dali::Toolkit::GetImpl(*this).FrameRenderedSignal();
}

WebView::WebViewRequestInterceptorSignalType& WebView::RequestInterceptorSignal()
{
  return Dali::Toolkit::GetImpl(*this).RequestInterceptorSignal();
}

WebView::WebView(Internal::WebView& implementation)
: Control(implementation)
{
}

WebView::WebView(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::WebView>(internal);
}

} // namespace Toolkit

} // namespace Dali
