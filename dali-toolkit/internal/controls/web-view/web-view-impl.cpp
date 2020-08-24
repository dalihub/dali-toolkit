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
#include "web-view-impl.h"

// EXTERNAL INCLUDES
#include <cstring>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/devel-api/common/stage.h>
#include <dali/public-api/adaptor-framework/native-image-source.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
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

DALI_ENUM_TO_STRING_TABLE_BEGIN( CacheModel )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::WebView::CacheModel, DOCUMENT_VIEWER )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::WebView::CacheModel, DOCUMENT_BROWSER )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::WebView::CacheModel, PRIMARY_WEB_BROWSER )
DALI_ENUM_TO_STRING_TABLE_END( CacheModel )

DALI_ENUM_TO_STRING_TABLE_BEGIN( CookieAcceptPolicy )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::WebView::CookieAcceptPolicy, ALWAYS )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::WebView::CookieAcceptPolicy, NEVER )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::WebView::CookieAcceptPolicy, NO_THIRD_PARTY )
DALI_ENUM_TO_STRING_TABLE_END( CookieAcceptPolicy )

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::WebView, Toolkit::Control, Create )

DALI_PROPERTY_REGISTRATION( Toolkit, WebView, "url",                     STRING,  URL                        )
DALI_PROPERTY_REGISTRATION( Toolkit, WebView, "cacheModel",              STRING,  CACHE_MODEL                )
DALI_PROPERTY_REGISTRATION( Toolkit, WebView, "cookieAcceptPolicy",      STRING,  COOKIE_ACCEPT_POLICY       )
DALI_PROPERTY_REGISTRATION( Toolkit, WebView, "userAgent",               STRING,  USER_AGENT                 )
DALI_PROPERTY_REGISTRATION( Toolkit, WebView, "enableJavaScript",        BOOLEAN, ENABLE_JAVASCRIPT          )
DALI_PROPERTY_REGISTRATION( Toolkit, WebView, "loadImagesAutomatically", BOOLEAN, LOAD_IMAGES_AUTOMATICALLY  )
DALI_PROPERTY_REGISTRATION( Toolkit, WebView, "defaultTextEncodingName", STRING,  DEFAULT_TEXT_ENCODING_NAME )
DALI_PROPERTY_REGISTRATION( Toolkit, WebView, "defaultFontSize",         INTEGER, DEFAULT_FONT_SIZE          )

DALI_SIGNAL_REGISTRATION(   Toolkit, WebView, "pageLoadStarted",         PAGE_LOAD_STARTED_SIGNAL            )
DALI_SIGNAL_REGISTRATION(   Toolkit, WebView, "pageLoadFinished",        PAGE_LOAD_FINISHED_SIGNAL           )
DALI_SIGNAL_REGISTRATION(   Toolkit, WebView, "pageLoadError",           PAGE_LOAD_ERROR_SIGNAL              )

DALI_TYPE_REGISTRATION_END()

const std::string kEmptyString;

} // anonymous namepsace

#define GET_ENUM_STRING( structName, inputExp ) \
  Scripting::GetLinearEnumerationName< Toolkit::WebView::structName::Type >( static_cast< Toolkit::WebView::structName::Type >( inputExp ), structName##_TABLE, structName##_TABLE_COUNT )

#define GET_ENUM_VALUE( structName, inputExp, outputExp ) \
  Scripting::GetEnumerationProperty< Toolkit::WebView::structName::Type >( inputExp, structName##_TABLE, structName##_TABLE_COUNT, outputExp )

WebView::WebView( const std::string& locale, const std::string& timezoneId )
: Control( ControlBehaviour( ACTOR_BEHAVIOUR_DEFAULT | DISABLE_STYLE_CHANGE_SIGNALS ) ),
  mUrl(),
  mVisual(),
  mWebViewSize( Stage::GetCurrent().GetSize() ),
  mWebEngine(),
  mPageLoadStartedSignal(),
  mPageLoadFinishedSignal(),
  mPageLoadErrorSignal()
{
  mWebEngine = Dali::WebEngine::New();

  // WebEngine is empty when it is not properly initialized.
  if( mWebEngine )
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
  Self().SetProperty( Actor::Property::KEYBOARD_FOCUSABLE, true );
  Self().TouchSignal().Connect( this, &WebView::OnTouchEvent );

  if( mWebEngine )
  {
    mWebEngine.PageLoadStartedSignal().Connect( this, &WebView::OnPageLoadStarted );
    mWebEngine.PageLoadFinishedSignal().Connect( this, &WebView::OnPageLoadFinished );
    mWebEngine.PageLoadErrorSignal().Connect( this, &WebView::OnPageLoadError );
  }
}

void WebView::LoadUrl( const std::string& url )
{
  mUrl = url;
  if( mWebEngine )
  {
    Texture texture = Dali::Texture::New( *mWebEngine.GetNativeImageSource() );
    const std::string nativeImageUrl = Dali::Toolkit::TextureManager::AddTexture( texture );
    mVisual = Toolkit::VisualFactory::Get().CreateVisual(
      { { Toolkit::Visual::Property::TYPE,  Toolkit::Visual::IMAGE } ,
        { Toolkit::ImageVisual::Property::URL, nativeImageUrl } } );

    if( mVisual )
    {
      // Clean up previously registered visual and add new one.
      DevelControl::RegisterVisual( *this, Toolkit::WebView::Property::URL, mVisual );
      mWebEngine.LoadUrl( url );
    }
  }
}

void WebView::LoadHTMLString( const std::string& htmlString )
{
  if( mWebEngine )
  {
    Texture texture = Dali::Texture::New( *mWebEngine.GetNativeImageSource() );
    const std::string nativeImageUrl = Dali::Toolkit::TextureManager::AddTexture( texture );
    mVisual = Toolkit::VisualFactory::Get().CreateVisual(
      { { Toolkit::Visual::Property::TYPE,  Toolkit::Visual::IMAGE } ,
        { Toolkit::ImageVisual::Property::URL, nativeImageUrl } } );

    if( mVisual )
    {
      DevelControl::RegisterVisual( *this, Toolkit::WebView::Property::URL, mVisual );
      mWebEngine.LoadHTMLString( htmlString );
    }
  }
}

void WebView::Reload()
{
  if( mWebEngine )
  {
    mWebEngine.Reload();
  }
}

void WebView::StopLoading()
{
  if( mWebEngine )
  {
    mWebEngine.StopLoading();
  }
}

void WebView::Suspend()
{
  if( mWebEngine )
  {
    mWebEngine.Suspend();
  }
}

void WebView::Resume()
{
  if( mWebEngine )
  {
    mWebEngine.Resume();
  }
}

bool WebView::CanGoForward()
{
  return mWebEngine ? mWebEngine.CanGoForward() : false;
}

void WebView::GoForward()
{
  if( mWebEngine )
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
  if( mWebEngine )
  {
    mWebEngine.GoBack();
  }
}

void WebView::EvaluateJavaScript( const std::string& script, std::function< void( const std::string& ) > resultHandler )
{
  if( mWebEngine )
  {
    mWebEngine.EvaluateJavaScript( script, resultHandler );
  }
}

void WebView::AddJavaScriptMessageHandler( const std::string& exposedObjectName, std::function< void( const std::string& ) > handler )
{
  if( mWebEngine )
  {
    mWebEngine.AddJavaScriptMessageHandler( exposedObjectName, handler );
  }
}

void WebView::ClearHistory()
{
  if( mWebEngine )
  {
    mWebEngine.ClearHistory();
  }
}

void WebView::ClearCache()
{
  if( mWebEngine )
  {
    mWebEngine.ClearCache();
  }
}

void WebView::ClearCookies()
{
  if( mWebEngine )
  {
    mWebEngine.ClearCookies();
  }
}

Dali::Toolkit::WebView::WebViewPageLoadSignalType& WebView::PageLoadStartedSignal()
{
  return mPageLoadStartedSignal;
}

Dali::Toolkit::WebView::WebViewPageLoadSignalType& WebView::PageLoadFinishedSignal()
{
  return mPageLoadFinishedSignal;
}

Dali::Toolkit::WebView::WebViewPageLoadErrorSignalType& WebView::PageLoadErrorSignal()
{
  return mPageLoadErrorSignal;
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

void WebView::OnPageLoadError( const std::string& url, int errorCode )
{
  if( !mPageLoadErrorSignal.Empty() )
  {
    Dali::Toolkit::WebView handle( GetOwner() );
    mPageLoadErrorSignal.Emit( handle, url, static_cast< Toolkit::WebView::LoadErrorCode >( errorCode ) );
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
  else if( 0 == strcmp( signalName.c_str(), PAGE_LOAD_ERROR_SIGNAL ) )
  {
    webView.PageLoadErrorSignal().Connect( tracker, functor );
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
      case Toolkit::WebView::Property::CACHE_MODEL:
      {
        Toolkit::WebView::CacheModel::Type output = impl.GetCacheModel();
        GET_ENUM_VALUE( CacheModel, value, output );
        impl.SetCacheModel( output );
        break;
      }
      case Toolkit::WebView::Property::COOKIE_ACCEPT_POLICY:
      {
        Toolkit::WebView::CookieAcceptPolicy::Type output = impl.GetCookieAcceptPolicy();
        GET_ENUM_VALUE( CookieAcceptPolicy, value, output );
        impl.SetCookieAcceptPolicy( output );
        break;
      }
      case Toolkit::WebView::Property::USER_AGENT:
      {
        std::string input;
        if( value.Get( input ) )
        {
          impl.SetUserAgent( input );
        }
        break;
      }
      case Toolkit::WebView::Property::ENABLE_JAVASCRIPT:
      {
        bool input;
        if( value.Get( input ) )
        {
          impl.EnableJavaScript( input );
        }
        break;
      }
      case Toolkit::WebView::Property::LOAD_IMAGES_AUTOMATICALLY:
      {
        bool input;
        if( value.Get( input ) )
        {
          impl.LoadImagesAutomatically( input );
        }
        break;
      }
      case Toolkit::WebView::Property::DEFAULT_TEXT_ENCODING_NAME:
      {
        std::string input;
        if( value.Get( input ) )
        {
          impl.SetDefaultTextEncodingName( input );
        }
        break;
      }
      case Toolkit::WebView::Property::DEFAULT_FONT_SIZE:
      {
        int input;
        if( value.Get( input ) )
        {
          impl.SetDefaultFontSize( input );
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
        value = impl.mUrl;
        break;
      }
      case Toolkit::WebView::Property::CACHE_MODEL:
      {
        value = GET_ENUM_STRING( CacheModel, impl.GetCacheModel() );
        break;
      }
      case Toolkit::WebView::Property::COOKIE_ACCEPT_POLICY:
      {
        value = GET_ENUM_STRING( CookieAcceptPolicy, impl.GetCookieAcceptPolicy() );
        break;
      }
      case Toolkit::WebView::Property::USER_AGENT:
      {
        value = impl.GetUserAgent();
        break;
      }
      case Toolkit::WebView::Property::ENABLE_JAVASCRIPT:
      {
        value = impl.IsJavaScriptEnabled();
        break;
      }
      case Toolkit::WebView::Property::LOAD_IMAGES_AUTOMATICALLY:
      {
        value = impl.AreImagesAutomaticallyLoaded();
        break;
      }
      case Toolkit::WebView::Property::DEFAULT_TEXT_ENCODING_NAME:
      {
        value = impl.GetDefaultTextEncodingName();
        break;
      }
      case Toolkit::WebView::Property::DEFAULT_FONT_SIZE:
      {
        value = impl.GetDefaultFontSize();
        break;
      }
      default:
         break;
    }
  }

  return value;
}

bool WebView::OnTouchEvent( Actor actor, const Dali::TouchEvent& touch )
{
  bool result = false;

  if( mWebEngine )
  {
    result = mWebEngine.SendTouchEvent( touch );
  }
  return result;
}

bool WebView::OnKeyEvent( const Dali::KeyEvent& event )
{
  bool result = false;

  if( mWebEngine )
  {
    result = mWebEngine.SendKeyEvent( event );
  }
  return result;
}

Toolkit::WebView::CacheModel::Type WebView::GetCacheModel() const
{
  return mWebEngine ? static_cast< Toolkit::WebView::CacheModel::Type >( mWebEngine.GetCacheModel() ) : Toolkit::WebView::CacheModel::DOCUMENT_VIEWER;
}

void WebView::SetCacheModel( Toolkit::WebView::CacheModel::Type cacheModel )
{
  if( mWebEngine )
  {
    mWebEngine.SetCacheModel( static_cast< WebEnginePlugin::CacheModel >( cacheModel ) );
  }
}

Toolkit::WebView::CookieAcceptPolicy::Type WebView::GetCookieAcceptPolicy() const
{
  return mWebEngine ? static_cast< Toolkit::WebView::CookieAcceptPolicy::Type >( mWebEngine.GetCookieAcceptPolicy() ) : Toolkit::WebView::CookieAcceptPolicy::NO_THIRD_PARTY;
}

void WebView::SetCookieAcceptPolicy( Toolkit::WebView::CookieAcceptPolicy::Type policy )
{
  if( mWebEngine )
  {
    mWebEngine.SetCookieAcceptPolicy( static_cast< WebEnginePlugin::CookieAcceptPolicy >( policy ) );
  }
}

const std::string& WebView::GetUserAgent() const
{
  return mWebEngine ? mWebEngine.GetUserAgent() : kEmptyString;
}

void WebView::SetUserAgent( const std::string& userAgent )
{
  if( mWebEngine )
  {
    mWebEngine.SetUserAgent( userAgent );
  }
}

bool WebView::IsJavaScriptEnabled() const
{
  return mWebEngine ? mWebEngine.IsJavaScriptEnabled() : true;
}

void WebView::EnableJavaScript( bool enabled )
{
  if( mWebEngine )
  {
    mWebEngine.EnableJavaScript( enabled );
  }
}

bool WebView::AreImagesAutomaticallyLoaded() const
{
  return mWebEngine ? mWebEngine.AreImagesAutomaticallyLoaded() : true;
}

void WebView::LoadImagesAutomatically( bool automatic )
{
  if( mWebEngine )
  {
    mWebEngine.LoadImagesAutomatically( automatic );
  }
}

const std::string& WebView::GetDefaultTextEncodingName() const
{
  return mWebEngine ? mWebEngine.GetDefaultTextEncodingName() : kEmptyString;
}

void WebView::SetDefaultTextEncodingName( const std::string& defaultTextEncodingName )
{
  if( mWebEngine )
  {
    mWebEngine.SetDefaultTextEncodingName( defaultTextEncodingName );
  }
}

int WebView::GetDefaultFontSize() const
{
  return mWebEngine ? mWebEngine.GetDefaultFontSize() : 0;
}

void WebView::SetDefaultFontSize( int defaultFontSize )
{
  if( mWebEngine )
  {
    mWebEngine.SetDefaultFontSize( defaultFontSize );
  }
}

#undef GET_ENUM_STRING
#undef GET_ENUM_VALUE

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
