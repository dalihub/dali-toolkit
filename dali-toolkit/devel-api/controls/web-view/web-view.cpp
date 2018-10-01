/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

namespace Dali
{

namespace Toolkit
{

WebView::WebView()
{
}

WebView::WebView( const WebView& WebView )
: Control( WebView )
{
}

WebView& WebView::operator=( const WebView& view )
{
  if( &view != this )
  {
    Control::operator=( view );
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

WebView WebView::New( const std::string& locale, const std::string& timezoneId )
{
  return Internal::WebView::New( locale, timezoneId );
}

WebView WebView::DownCast( BaseHandle handle )
{
  return Control::DownCast< WebView, Internal::WebView >( handle );
}

void WebView::LoadUrl( const std::string& url )
{
  Dali::Toolkit::GetImpl( *this ).LoadUrl( url );
}

const std::string& WebView::GetUrl()
{
  return Dali::Toolkit::GetImpl( *this ).GetUrl();
}

void WebView::LoadHTMLString( const std::string& htmlString )
{
  Dali::Toolkit::GetImpl( *this ).LoadHTMLString( htmlString );
}

void WebView::Reload()
{
  Dali::Toolkit::GetImpl( *this ).Reload();
}

void WebView::StopLoading()
{
  Dali::Toolkit::GetImpl( *this ).StopLoading();
}

bool WebView::CanGoForward()
{
  return Dali::Toolkit::GetImpl( *this ).CanGoForward();
}

void WebView::GoForward()
{
  Dali::Toolkit::GetImpl( *this ).GoForward();
}

bool WebView::CanGoBack()
{
  return Dali::Toolkit::GetImpl( *this ).CanGoBack();
}

void WebView::GoBack()
{
  Dali::Toolkit::GetImpl( *this ).GoBack();
}

void WebView::EvaluateJavaScript( const std::string& script )
{
  Dali::Toolkit::GetImpl( *this ).EvaluateJavaScript( script );
}

void WebView::AddJavaScriptInterface( const std::string& exposedObjectName, const std::string& jsFunctionName, std::function< std::string(const std::string&) > callback )
{
  Dali::Toolkit::GetImpl( *this ).AddJavaScriptInterface( exposedObjectName, jsFunctionName, callback );
}

void WebView::RemoveJavascriptInterface( const std::string& exposedObjectName, const std::string& jsFunctionName )
{
  Dali::Toolkit::GetImpl( *this ).RemoveJavascriptInterface( exposedObjectName, jsFunctionName );
}

void WebView::ClearHistory()
{
  Dali::Toolkit::GetImpl( *this ).ClearHistory();
}

void WebView::ClearCache()
{
  Dali::Toolkit::GetImpl( *this ).ClearCache();
}

WebView::WebViewSignalType& WebView::PageLoadStartedSignal()
{
  return Dali::Toolkit::GetImpl( *this ).PageLoadStartedSignal();
}

WebView::WebViewSignalType& WebView::PageLoadFinishedSignal()
{
  return Dali::Toolkit::GetImpl( *this ).PageLoadFinishedSignal();
}

WebView::WebView( Internal::WebView& implementation )
: Control( implementation )
{
}

WebView::WebView( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer< Internal::WebView >( internal );
}

} // namespace Toolkit

} // namespace Dali
