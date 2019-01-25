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
#include "web-view-impl.h"

// EXTERNAL INCLUDES
#include <cstring>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/images/native-image.h>
#include <dali/public-api/adaptor-framework/native-image-source.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>

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

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::WebView, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, WebView, "url", STRING, URL );

DALI_SIGNAL_REGISTRATION( Toolkit, WebView, "pageLoadStarted", PAGE_LOAD_STARTED_SIGNAL );
DALI_SIGNAL_REGISTRATION( Toolkit, WebView, "pageLoadFinished", PAGE_LOAD_FINISHED_SIGNAL );

DALI_TYPE_REGISTRATION_END()

} // anonymous namepsace

WebView::WebView( const std::string& locale, const std::string& timezoneId )
: Control( ControlBehaviour( ACTOR_BEHAVIOUR_DEFAULT | DISABLE_STYLE_CHANGE_SIGNALS ) ),
  mUrl(),
  mVisual(),
  mWebViewSize( Stage::GetCurrent().GetSize() ),
  mWebEngine(),
  mPageLoadStartedSignal(),
  mPageLoadFinishedSignal()
{
  mWebEngine = Dali::WebEngine::New();
  if ( mWebEngine )
  {
    mWebEngine.Create( mWebViewSize.width, mWebViewSize.height, locale, timezoneId );
  }
}

WebView::WebView()
: WebView( "", "" )
{
}

WebView::~WebView()
{
}

Toolkit::WebView WebView::New()
{
  WebView* impl = new WebView();
  Toolkit::WebView handle = Toolkit::WebView( *impl );

  impl->Initialize();
  return handle;
}

Toolkit::WebView WebView::New( const std::string& locale, const std::string& timezoneId )
{
  WebView* impl = new WebView( locale, timezoneId );
  Toolkit::WebView handle = Toolkit::WebView( *impl );

  impl->Initialize();
  return handle;
}

void WebView::OnInitialize()
{
  Self().SetKeyboardFocusable( true );
  Self().TouchSignal().Connect( this, &WebView::OnTouchEvent );

  if ( mWebEngine )
  {
    mWebEngine.PageLoadStartedSignal().Connect( this, &WebView::OnPageLoadStarted );
    mWebEngine.PageLoadFinishedSignal().Connect( this, &WebView::OnPageLoadFinished );
  }
}

void WebView::LoadUrl( const std::string& url )
{
  mUrl = url;
  if ( mWebEngine )
  {
    Dali::Image image = Dali::NativeImage::New( *mWebEngine.GetNativeImageSource() );
    mVisual = Toolkit::VisualFactory::Get().CreateVisual( image );

    if ( mVisual )
    {
      // Clean up previously registered visual and add new one.
      DevelControl::RegisterVisual( *this, Toolkit::WebView::Property::URL, mVisual );
      mWebEngine.LoadUrl( url );
    }
  }
}

const std::string& WebView::GetUrl()
{
  return mWebEngine ? mWebEngine.GetUrl() : mUrl;
}

void WebView::LoadHTMLString( const std::string& htmlString )
{
  if ( mWebEngine )
  {
    Dali::Image image = Dali::NativeImage::New( *mWebEngine.GetNativeImageSource() );
    mVisual = Toolkit::VisualFactory::Get().CreateVisual( image );

    if ( mVisual )
    {
      DevelControl::RegisterVisual( *this, Toolkit::WebView::Property::URL, mVisual );
      mWebEngine.LoadHTMLString( htmlString );
    }
  }
}

void WebView::Reload()
{
  if ( mWebEngine )
  {
    mWebEngine.Reload();
  }
}

void WebView::StopLoading()
{
  if ( mWebEngine )
  {
    mWebEngine.StopLoading();
  }
}

bool WebView::CanGoForward()
{
  return mWebEngine ? mWebEngine.CanGoForward() : false;
}

void WebView::GoForward()
{
  if ( mWebEngine )
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
  if ( mWebEngine )
  {
    mWebEngine.GoBack();
  }
}

void WebView::EvaluateJavaScript( const std::string& script )
{
  if ( mWebEngine )
  {
    mWebEngine.EvaluateJavaScript( script );
  }
}

void WebView::AddJavaScriptMessageHandler( const std::string& exposedObjectName, std::function< void( const std::string& ) > handler )
{
  if ( mWebEngine )
  {
    mWebEngine.AddJavaScriptMessageHandler( exposedObjectName, handler );
  }
}

void WebView::ClearHistory()
{
  if ( mWebEngine )
  {
    mWebEngine.ClearHistory();
  }
}

void WebView::ClearCache()
{
  if ( mWebEngine )
  {
    mWebEngine.ClearCache();
  }
}

Dali::Toolkit::WebView::WebViewSignalType& WebView::PageLoadStartedSignal()
{
  return mPageLoadStartedSignal;
}

Dali::Toolkit::WebView::WebViewSignalType& WebView::PageLoadFinishedSignal()
{
  return mPageLoadFinishedSignal;
}

void WebView::OnPageLoadStarted( const std::string& url )
{
  if( !mPageLoadStartedSignal.Empty() )
  {
    Dali::Toolkit::WebView handle( GetOwner() );
    mPageLoadStartedSignal.Emit( handle, url );
  }
}

void WebView::OnPageLoadFinished( const std::string& url )
{
  if( !mPageLoadFinishedSignal.Empty() )
  {
    Dali::Toolkit::WebView handle( GetOwner() );
    mPageLoadFinishedSignal.Emit( handle, url );
  }
}

bool WebView::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected = false;
  Toolkit::WebView webView = Toolkit::WebView::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), PAGE_LOAD_STARTED_SIGNAL ) )
  {
    webView.PageLoadStartedSignal().Connect( tracker, functor );
    connected = true;
  }
  else if( 0 == strcmp( signalName.c_str(), PAGE_LOAD_FINISHED_SIGNAL ) )
  {
    webView.PageLoadFinishedSignal().Connect( tracker, functor );
    connected = true;
  }

  return connected;
}

Vector3 WebView::GetNaturalSize()
{
  if( mVisual )
  {
    Vector2 rendererNaturalSize;
    mVisual.GetNaturalSize( rendererNaturalSize );
    return Vector3( rendererNaturalSize );
  }

  return Vector3( mWebViewSize );
}

void WebView::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  Control::OnRelayout( size, container );

  if( size.width > 0 && size.height > 0 && mWebViewSize != size )
  {
    mWebViewSize = size;

    if( mWebEngine )
    {
      mWebEngine.SetSize( size.width, size.height );
    }
  }
}

void WebView::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::WebView webView = Toolkit::WebView::DownCast( Dali::BaseHandle( object ) );

  if( webView )
  {
    WebView& impl = GetImpl( webView );
    switch( index )
    {
      case Toolkit::WebView::Property::URL:
      {
        std::string url;
        if( value.Get( url ) )
        {
          impl.LoadUrl( url );
        }
        break;
      }
    }
  }
}

Property::Value WebView::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Toolkit::WebView webView = Toolkit::WebView::DownCast( Dali::BaseHandle( object ) );

  if( webView )
  {
    WebView& impl = GetImpl( webView );
    switch( propertyIndex )
    {
      case Toolkit::WebView::Property::URL:
      {
        value = impl.GetUrl();
        break;
      }
    }
  }

  return value;
}

bool WebView::OnTouchEvent( Actor actor, const Dali::TouchData& touch )
{
  bool result = false;

  if ( mWebEngine )
  {
    result = mWebEngine.SendTouchEvent( touch );
  }
  return result;
}

bool WebView::OnKeyEvent( const Dali::KeyEvent& event )
{
  bool result = false;

  if ( mWebEngine )
  {
    result = mWebEngine.SendKeyEvent( event );
  }
  return result;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
