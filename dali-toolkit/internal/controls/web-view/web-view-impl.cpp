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
#include <dali/devel-api/adaptor-framework/web-engine-back-forward-list.h>
#include <dali/devel-api/adaptor-framework/web-engine-context.h>
#include <dali/devel-api/adaptor-framework/web-engine-cookie-manager.h>
#include <dali/devel-api/adaptor-framework/web-engine-settings.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/devel-api/common/stage.h>
#include <dali/public-api/adaptor-framework/native-image-source.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/web-view/web-back-forward-list.h>
#include <dali-toolkit/devel-api/controls/web-view/web-context.h>
#include <dali-toolkit/devel-api/controls/web-view/web-cookie-manager.h>
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

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::WebView, Toolkit::Control, Create )

DALI_PROPERTY_REGISTRATION( Toolkit, WebView, "url",                     STRING,  URL                        )
DALI_PROPERTY_REGISTRATION( Toolkit, WebView, "userAgent",               STRING,  USER_AGENT                 )
DALI_PROPERTY_REGISTRATION( Toolkit, WebView, "scrollPosition",          VECTOR2, SCROLL_POSITION            )
DALI_PROPERTY_REGISTRATION( Toolkit, WebView, "scrollSize",              VECTOR2, SCROLL_SIZE                )
DALI_PROPERTY_REGISTRATION( Toolkit, WebView, "contentSize",             VECTOR2, CONTENT_SIZE               )
DALI_PROPERTY_REGISTRATION( Toolkit, WebView, "title",                   STRING,  TITLE                      )
DALI_PROPERTY_REGISTRATION( Toolkit, WebView, "videoHoleEnabled",        BOOLEAN, VIDEO_HOLE_ENABLED         )

DALI_SIGNAL_REGISTRATION(   Toolkit, WebView, "pageLoadStarted",         PAGE_LOAD_STARTED_SIGNAL            )
DALI_SIGNAL_REGISTRATION(   Toolkit, WebView, "pageLoadFinished",        PAGE_LOAD_FINISHED_SIGNAL           )
DALI_SIGNAL_REGISTRATION(   Toolkit, WebView, "pageLoadError",           PAGE_LOAD_ERROR_SIGNAL              )
DALI_SIGNAL_REGISTRATION(   Toolkit, WebView, "scrollEdgeReached",       SCROLL_EDGE_REACHED_SIGNAL          )

DALI_TYPE_REGISTRATION_END()

const std::string kEmptyString;

const char* DEFAULT_SAMPLER_TYPENAME = "sampler2D";

const char* FRAGMENT_SHADER_TEXTURE = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  uniform lowp vec3 mixColor;\n
  uniform lowp float preMultipliedAlpha;\n
  \n
  void main()\n
  {\n
      gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor * vec4( mixColor, 1.0 );\n
  }\n
);

Dali::Toolkit::Visual::Base CreateNativeImageVisual( NativeImageInterfacePtr nativeImageInterface )
{
  std::string fragmentShader;

  const char* fragmentPrefix = nativeImageInterface->GetCustomFragmentPrefix();
  if( fragmentPrefix )
  {
    fragmentShader = fragmentPrefix;
    fragmentShader += FRAGMENT_SHADER_TEXTURE;
  }
  else
  {
    fragmentShader = FRAGMENT_SHADER_TEXTURE;
  }

  const char* customSamplerTypename = nativeImageInterface->GetCustomSamplerTypename();
  if( customSamplerTypename )
  {
    fragmentShader.replace( fragmentShader.find( DEFAULT_SAMPLER_TYPENAME ), strlen( DEFAULT_SAMPLER_TYPENAME ), customSamplerTypename );
  }

  Texture texture = Dali::Texture::New( *nativeImageInterface );
  const std::string nativeImageUrl = Dali::Toolkit::TextureManager::AddTexture( texture );

  return Toolkit::VisualFactory::Get().CreateVisual(
    { { Toolkit::Visual::Property::TYPE,  Toolkit::Visual::IMAGE } ,
      { Toolkit::Visual::Property::SHADER, { { Toolkit::Visual::Shader::Property::FRAGMENT_SHADER, fragmentShader } } },
      { Toolkit::ImageVisual::Property::URL, nativeImageUrl } } );
}

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
  mPageLoadErrorSignal(),
  mVideoHoleEnabled( true ),
  mWebViewArea ( 0, 0, mWebViewSize.width, mWebViewSize.height )
{
  mWebEngine = Dali::WebEngine::New();

  // WebEngine is empty when it is not properly initialized.
  if( mWebEngine )
  {
    mWebEngine.Create( mWebViewSize.width, mWebViewSize.height, locale, timezoneId );
  }
}

WebView::WebView( int argc, char** argv )
: Control( ControlBehaviour( ACTOR_BEHAVIOUR_DEFAULT | DISABLE_STYLE_CHANGE_SIGNALS ) ),
  mUrl(),
  mVisual(),
  mWebViewSize( Stage::GetCurrent().GetSize() ),
  mWebEngine(),
  mPageLoadStartedSignal(),
  mPageLoadFinishedSignal(),
  mPageLoadErrorSignal(),
  mVideoHoleEnabled( true ),
  mWebViewArea ( 0, 0, mWebViewSize.width, mWebViewSize.height )
{
  mWebEngine = Dali::WebEngine::New();

  // WebEngine is empty when it is not properly initialized.
  if ( mWebEngine )
  {
    mWebEngine.Create( mWebViewSize.width, mWebViewSize.height, argc, argv );
  }
}

WebView::WebView()
: WebView( "", "" )
{
}

WebView::~WebView()
{
  if( mWebEngine )
  {
    mWebEngine.Destroy();
  }
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

Toolkit::WebView WebView::New( int argc, char** argv )
{
  WebView* impl = new WebView( argc, argv );
  Toolkit::WebView handle = Toolkit::WebView( *impl );

  impl->Initialize();
  return handle;
}

void WebView::OnInitialize()
{
  Actor self = Self();

  self.SetProperty( Actor::Property::KEYBOARD_FOCUSABLE, true );
  self.TouchedSignal().Connect( this, &WebView::OnTouchEvent );

  mPositionUpdateNotification = self.AddPropertyNotification( Actor::Property::WORLD_POSITION, StepCondition( 1.0f, 1.0f ) );
  mSizeUpdateNotification = self.AddPropertyNotification( Actor::Property::SIZE, StepCondition( 1.0f, 1.0f ) );
  mScaleUpdateNotification = self.AddPropertyNotification( Actor::Property::WORLD_SCALE, StepCondition( 0.1f, 1.0f ) );
  mPositionUpdateNotification.NotifySignal().Connect( this, &WebView::UpdateDisplayArea );
  mSizeUpdateNotification.NotifySignal().Connect( this, &WebView::UpdateDisplayArea );
  mScaleUpdateNotification.NotifySignal().Connect( this, &WebView::UpdateDisplayArea );

  if( mWebEngine )
  {
    mWebEngine.PageLoadStartedSignal().Connect( this, &WebView::OnPageLoadStarted );
    mWebEngine.PageLoadFinishedSignal().Connect( this, &WebView::OnPageLoadFinished );
    mWebEngine.PageLoadErrorSignal().Connect( this, &WebView::OnPageLoadError );
    mWebEngine.ScrollEdgeReachedSignal().Connect( this, &WebView::OnScrollEdgeReached );

    mWebContext = std::unique_ptr<Dali::Toolkit::WebContext>( new WebContext( mWebEngine.GetContext() ) );
    mWebCookieManager = std::unique_ptr<Dali::Toolkit::WebCookieManager>( new WebCookieManager( mWebEngine.GetCookieManager() ) );
    mWebSettings = std::unique_ptr<Dali::Toolkit::WebSettings>( new WebSettings( mWebEngine.GetSettings() ) );
    mWebBackForwardList = std::unique_ptr<Dali::Toolkit::WebBackForwardList>( new WebBackForwardList( mWebEngine.GetBackForwardList() ) );
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

Dali::Toolkit::ImageView WebView::GetFavicon() const
{
  Dali::Toolkit::ImageView faviconView;
  if(mWebEngine)
  {
    Dali::PixelData pixelData = mWebEngine.GetFavicon();
    if(pixelData)
    {
      std::string url = Dali::Toolkit::Image::GenerateUrl(pixelData);
      faviconView     = Dali::Toolkit::ImageView::New(url);
      faviconView.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    }
  }
  return faviconView;
}

void WebView::LoadUrl( const std::string& url )
{
  mUrl = url;
  if( mWebEngine )
  {
    mVisual = CreateNativeImageVisual( mWebEngine.GetNativeImageSource() );

    if( mVisual )
    {
      // Clean up previously registered visual and add new one.
      DevelControl::RegisterVisual( *this, Toolkit::WebView::Property::URL, mVisual );
      mWebEngine.LoadUrl( url );
    }

    if ( mVideoHoleEnabled )
    {
      EnableBlendMode( false );
    }
  }
}

void WebView::LoadHtmlString( const std::string& htmlString )
{
  if( mWebEngine )
  {
    mVisual = CreateNativeImageVisual( mWebEngine.GetNativeImageSource() );

    if( mVisual )
    {
      DevelControl::RegisterVisual( *this, Toolkit::WebView::Property::URL, mVisual );
      mWebEngine.LoadHtmlString( htmlString );
    }

    if ( mVideoHoleEnabled )
    {
      EnableBlendMode( false );
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

void WebView::ScrollBy( int deltaX, int deltaY )
{
  if ( mWebEngine )
  {
    mWebEngine.ScrollBy( deltaX, deltaY );
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

void WebView::ClearAllTilesResources()
{
  if( mWebEngine )
  {
    mWebEngine.ClearAllTilesResources();
  }
}

void WebView::ClearHistory()
{
  if( mWebEngine )
  {
    mWebEngine.ClearHistory();
  }
}

void WebView::SetTtsFocus(bool focused)
{
  if(mWebEngine && !HasKeyInputFocus())
  {
    mWebEngine.SetFocus(focused);
  }
}

void WebView::UpdateDisplayArea( Dali::PropertyNotification& /*source*/ )
{
  if( !mWebEngine )
    return;

  Actor self( Self() );

  bool positionUsesAnchorPoint = self.GetProperty< bool >( Actor::Property::POSITION_USES_ANCHOR_POINT );
  Vector3 actorSize = self.GetCurrentProperty< Vector3 >( Actor::Property::SIZE ) * self.GetCurrentProperty< Vector3 >( Actor::Property::SCALE );
  Vector3 anchorPointOffSet = actorSize * ( positionUsesAnchorPoint ? self.GetCurrentProperty< Vector3 >( Actor::Property::ANCHOR_POINT ) : AnchorPoint::TOP_LEFT );
  Vector2 screenPosition = self.GetProperty< Vector2 >( Actor::Property::SCREEN_POSITION );

  Dali::Rect< int > displayArea;
  displayArea.x = screenPosition.x - anchorPointOffSet.x;
  displayArea.y = screenPosition.y - anchorPointOffSet.y;
  displayArea.width = actorSize.x;
  displayArea.height = actorSize.y;

  Size displaySize = Size( displayArea.width, displayArea.height );
  if ( mWebViewSize != displaySize )
  {
    mWebViewSize = displaySize;
  }

  if (mWebViewArea != displayArea )
  {
    mWebViewArea = displayArea;
    mWebEngine.UpdateDisplayArea( mWebViewArea );
  }
}

void WebView::EnableVideoHole( bool enabled )
{
  mVideoHoleEnabled = enabled;

  EnableBlendMode( !mVideoHoleEnabled );

  if( mWebEngine )
  {
    mWebEngine.EnableVideoHole( mVideoHoleEnabled );
  }
}

void WebView::EnableBlendMode( bool blendEnabled )
{
  Actor self = Self();
  for (uint32_t i = 0; i < self.GetRendererCount(); i++)
  {
    Dali::Renderer render = self.GetRendererAt( i );
    render.SetProperty( Renderer::Property::BLEND_MODE, blendEnabled ? BlendMode::ON : BlendMode::OFF );
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

Dali::Toolkit::WebView::WebViewScrollEdgeReachedSignalType& WebView::ScrollEdgeReachedSignal()
{
  return mScrollEdgeReachedSignal;
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

void WebView::OnScrollEdgeReached( Dali::WebEnginePlugin::ScrollEdge edge )
{
  if( !mScrollEdgeReachedSignal.Empty() )
  {
    Dali::Toolkit::WebView handle( GetOwner() );
    mScrollEdgeReachedSignal.Emit( handle, edge );
  }
}

void WebView::GetPlainTextAsynchronously(Dali::WebEnginePlugin::PlainTextReceivedCallback callback)
{
  if(mWebEngine)
  {
    mWebEngine.GetPlainTextAsynchronously(callback);
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
  else if( 0 == strcmp( signalName.c_str(), SCROLL_EDGE_REACHED_SIGNAL ) )
  {
    webView.ScrollEdgeReachedSignal().Connect( tracker, functor );
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

void WebView::OnSceneConnection( int depth )
{
  Control::OnSceneConnection( depth );

  EnableBlendMode( !mVideoHoleEnabled );
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
      case Toolkit::WebView::Property::USER_AGENT:
      {
        std::string input;
        if( value.Get( input ) )
        {
          impl.SetUserAgent( input );
        }
        break;
      }
      case Toolkit::WebView::Property::SCROLL_POSITION:
      {
        Vector2 input;
        if ( value.Get( input ) )
        {
          impl.SetScrollPosition( input.x, input.y );
        }
        break;
      }
      case Toolkit::WebView::Property::VIDEO_HOLE_ENABLED:
      {
        bool input;
        if( value.Get( input ) )
        {
          impl.EnableVideoHole( input );
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
      case Toolkit::WebView::Property::USER_AGENT:
      {
        value = impl.GetUserAgent();
        break;
      }
      case Toolkit::WebView::Property::SCROLL_POSITION:
      {
        int x, y;
        impl.GetScrollPosition( x, y );
        value = Vector2( x, y );
        break;
      }
      case Toolkit::WebView::Property::SCROLL_SIZE:
      {
        int width, height;
        impl.GetScrollSize( width, height );
        value = Vector2( width, height );
        break;
      }
      case Toolkit::WebView::Property::CONTENT_SIZE:
      {
        int width, height;
        impl.GetContentSize( width, height );
        value = Vector2( width, height );
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

void WebView::OnKeyInputFocusGained()
{
  if( mWebEngine )
  {
    mWebEngine.SetFocus( true );
  }

  EmitKeyInputFocusSignal( true ); // Calls back into the Control hence done last.
}

void WebView::OnKeyInputFocusLost()
{
  if( mWebEngine )
  {
    mWebEngine.SetFocus( false );
  }

  EmitKeyInputFocusSignal( false ); // Calls back into the Control hence done last.
}

void WebView::SetScrollPosition( int x, int y )
{
  if( mWebEngine )
  {
    mWebEngine.SetScrollPosition( x, y );
  }
}

void WebView::GetScrollPosition( int& x, int& y ) const
{
  if( mWebEngine )
  {
    mWebEngine.GetScrollPosition( x, y );
  }
}

void WebView::GetScrollSize( int& width, int& height ) const
{
  if( mWebEngine )
  {
    mWebEngine.GetScrollSize( width, height );
  }
}

void WebView::GetContentSize( int& width, int& height ) const
{
  if( mWebEngine )
  {
    mWebEngine.GetContentSize( width, height );
  }
}

std::string WebView::GetTitle() const
{
  return mWebEngine ?  mWebEngine.GetTitle() : kEmptyString;
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

#undef GET_ENUM_STRING
#undef GET_ENUM_VALUE

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
