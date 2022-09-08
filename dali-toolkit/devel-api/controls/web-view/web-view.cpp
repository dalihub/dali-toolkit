/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

WebView WebView::New( int argc, char** argv )
{
  return Internal::WebView::New( argc, argv );
}

WebView WebView::DownCast(BaseHandle handle)
{
  return Control::DownCast<WebView, Internal::WebView>(handle);
}

Dali::Toolkit::WebSettings* WebView::GetSettings() const
{
  return Dali::Toolkit::GetImpl( *this ).GetSettings();
}

Dali::Toolkit::WebContext* WebView::GetContext() const
{
  return Dali::Toolkit::GetImpl( *this ).GetContext();
}

Dali::Toolkit::WebCookieManager* WebView::GetCookieManager() const
{
  return Dali::Toolkit::GetImpl( *this ).GetCookieManager();
}

Dali::Toolkit::WebBackForwardList* WebView::GetBackForwardList() const
{
  return Dali::Toolkit::GetImpl( *this ).GetBackForwardList();
}

Dali::Toolkit::ImageView WebView::GetFavicon() const
{
  return Dali::Toolkit::GetImpl( *this ).GetFavicon();
}

void WebView::LoadUrl(const std::string& url)
{
  Dali::Toolkit::GetImpl(*this).LoadUrl(url);
}

void WebView::LoadHtmlString(const std::string& htmlString)
{
  Dali::Toolkit::GetImpl(*this).LoadHtmlString(htmlString);
}

void WebView::Reload()
{
  Dali::Toolkit::GetImpl(*this).Reload();
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

void WebView::ScrollBy( int deltaX, int deltaY )
{
  Dali::Toolkit::GetImpl( *this ).ScrollBy( deltaX, deltaY );
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

void WebView::EvaluateJavaScript(const std::string& script, Dali::WebEnginePlugin::JavaScriptMessageHandlerCallback resultHandler)
{
  Dali::Toolkit::GetImpl(*this).EvaluateJavaScript(script, resultHandler);
}

void WebView::EvaluateJavaScript(const std::string& script)
{
  Dali::Toolkit::GetImpl(*this).EvaluateJavaScript(script, nullptr);
}

void WebView::AddJavaScriptMessageHandler(const std::string& exposedObjectName, Dali::WebEnginePlugin::JavaScriptMessageHandlerCallback handler)
{
  Dali::Toolkit::GetImpl(*this).AddJavaScriptMessageHandler(exposedObjectName, handler);
}

void WebView::ClearAllTilesResources()
{
  Dali::Toolkit::GetImpl( *this ).ClearAllTilesResources();
}

void WebView::ClearHistory()
{
  Dali::Toolkit::GetImpl(*this).ClearHistory();
}

void WebView::SetTtsFocus(bool focused)
{
  Dali::Toolkit::GetImpl(*this).SetTtsFocus(focused);
}

void WebView::RegisterPageLoadStartedCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback)
{
  Dali::Toolkit::GetImpl(*this).RegisterPageLoadStartedCallback(callback);
}

void WebView::RegisterPageLoadFinishedCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback)
{
  Dali::Toolkit::GetImpl(*this).RegisterPageLoadFinishedCallback(callback);
}

void WebView::RegisterPageLoadErrorCallback(Dali::WebEnginePlugin::WebEnginePageLoadErrorCallback callback)
{
  Dali::Toolkit::GetImpl(*this).RegisterPageLoadErrorCallback(callback);
}

void WebView::RegisterScrollEdgeReachedCallback(Dali::WebEnginePlugin::WebEngineScrollEdgeReachedCallback callback)
{
  Dali::Toolkit::GetImpl(*this).RegisterScrollEdgeReachedCallback(callback);
}

void WebView::RegisterNavigationPolicyDecidedCallback(Dali::WebEnginePlugin::WebEngineNavigationPolicyDecidedCallback callback)
{
  Dali::Toolkit::GetImpl(*this).RegisterNavigationPolicyDecidedCallback(callback);
}

void WebView::GetPlainTextAsynchronously(Dali::WebEnginePlugin::PlainTextReceivedCallback callback)
{
  Dali::Toolkit::GetImpl(*this).GetPlainTextAsynchronously(callback);
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
