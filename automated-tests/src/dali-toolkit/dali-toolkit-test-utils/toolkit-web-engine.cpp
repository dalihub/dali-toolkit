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

#include "toolkit-timer.h"

#include <dali/devel-api/adaptor-framework/web-engine.h>
#include <dali/public-api/object/any.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/adaptor-framework/native-image-source.h>
#include <toolkit-application.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class WebEngine;

namespace
{
static WebEngine* gInstance = NULL;
static int gInstanceCount = 0;

bool OnGoBack();
bool OnGoForward();
bool OnLoadUrl();
bool OnEvaluteJavaScript();
bool OnClearHistory();

static void ConnectToGlobalSignal( bool (*func)() )
{
  Dali::Timer timer = Dali::Timer::New( 0 );
  timer.TickSignal().Connect( func );
}

static void DisconnectFromGlobalSignal( bool (*func)() )
{
  Dali::Timer timer = Dali::Timer::New( 0 );
  timer.TickSignal().Disconnect( func );
}
}

class WebEngine: public Dali::BaseObject
{
public:

  WebEngine()
    : mUrl()
    , mCurrentPlusOnePos( 0 )
    , mCacheModel( Dali::WebEnginePlugin::CacheModel::DOCUMENT_VIEWER )
    , mCookieAcceptPolicy( Dali::WebEnginePlugin::CookieAcceptPolicy::NO_THIRD_PARTY )
    , mUserAgent()
    , mEnableJavaScript( true )
    , mLoadImagesAutomatically( true )
    , mDefaultTextEncodingName()
    , mDefaultFontSize( 16 )
    , mEvaluating( false )
  {
    gInstanceCount++;
    gInstance = this;
  }

  virtual ~WebEngine()
  {
    gInstanceCount--;
    if( !gInstanceCount )
    {
      gInstance = NULL;
    }
  }

  void LoadUrl( const std::string& url )
  {
    mUrl = url;
    ConnectToGlobalSignal( &OnLoadUrl );
  }

  const std::string& GetUrl() const
  {
    return mUrl;
  }

  bool CanGoForward() const
  {
    return mHistory.size() > mCurrentPlusOnePos;
  }

  void GoForward()
  {
    ConnectToGlobalSignal( &OnGoForward );
  }

  bool CanGoBack() const
  {
    return mCurrentPlusOnePos > 1;
  }

  void GoBack()
  {
    ConnectToGlobalSignal( &OnGoBack );
  }

  void EvaluateJavaScript( const std::string& script, std::function< void( const std::string& ) > resultHandler )
  {
    if( resultHandler )
    {
      if( !mEvaluating )
      {
        ConnectToGlobalSignal( &OnEvaluteJavaScript );
      }
      mResultCallbacks.push_back( resultHandler );
    }
  }

  void ClearHistory()
  {
    ConnectToGlobalSignal( &OnClearHistory );
  }

  Dali::WebEnginePlugin::CacheModel GetCacheModel() const
  {
    return mCacheModel;
  }

  void SetCacheModel( Dali::WebEnginePlugin::CacheModel cacheModel )
  {
    mCacheModel = cacheModel;
  }

  Dali::WebEnginePlugin::CookieAcceptPolicy GetCookieAcceptPolicy() const
  {
    return mCookieAcceptPolicy;
  }

  void SetCookieAcceptPolicy( Dali::WebEnginePlugin::CookieAcceptPolicy policy )
  {
    mCookieAcceptPolicy = policy;
  }

  const std::string& GetUserAgent() const
  {
    return mUserAgent;
  }

  void SetUserAgent( const std::string& userAgent )
  {
    mUserAgent = userAgent;
  }

  bool IsJavaScriptEnabled() const
  {
    return mEnableJavaScript;
  }

  void EnableJavaScript( bool enabled )
  {
    mEnableJavaScript = enabled;
  }

  bool AreImagesAutomaticallyLoaded() const
  {
    return mLoadImagesAutomatically;
  }

  void LoadImagesAutomatically( bool automatic )
  {
    mLoadImagesAutomatically = automatic;
  }

  const std::string& GetDefaultTextEncodingName() const
  {
    return mDefaultTextEncodingName;
  }

  void SetDefaultTextEncodingName( const std::string& defaultTextEncodingName )
  {
    mDefaultTextEncodingName = defaultTextEncodingName;
  }

  int GetDefaultFontSize() const
  {
    return mDefaultFontSize;
  }

  void SetDefaultFontSize( int defaultFontSize )
  {
    mDefaultFontSize = defaultFontSize;
  }

  Dali::WebEnginePlugin::WebEnginePageLoadSignalType& PageLoadStartedSignal()
  {
    return mPageLoadStartedSignal;
  }

  Dali::WebEnginePlugin::WebEnginePageLoadSignalType& PageLoadFinishedSignal()
  {
    return mPageLoadFinishedSignal;
  }

  Dali::WebEnginePlugin::WebEnginePageLoadErrorSignalType& PageLoadErrorSignal()
  {
    return mPageLoadErrorSignal;
  }

  std::string                                                mUrl;
  std::vector< std::string >                                 mHistory;
  size_t                                                     mCurrentPlusOnePos;
  Dali::WebEnginePlugin::CacheModel                          mCacheModel;
  Dali::WebEnginePlugin::CookieAcceptPolicy                  mCookieAcceptPolicy;
  std::string                                                mUserAgent;
  bool                                                       mEnableJavaScript;
  bool                                                       mLoadImagesAutomatically;
  std::string                                                mDefaultTextEncodingName;
  int                                                        mDefaultFontSize;
  Dali::WebEnginePlugin::WebEnginePageLoadSignalType         mPageLoadStartedSignal;
  Dali::WebEnginePlugin::WebEnginePageLoadSignalType         mPageLoadFinishedSignal;
  Dali::WebEnginePlugin::WebEnginePageLoadErrorSignalType    mPageLoadErrorSignal;
  std::vector< std::function< void( const std::string& ) > > mResultCallbacks;
  bool                                                       mEvaluating;
};

inline WebEngine& GetImplementation( Dali::WebEngine& webEngine )
{
  DALI_ASSERT_ALWAYS( webEngine && "WebEngine handle is empty." );
  BaseObject& handle = webEngine.GetBaseObject();
  return static_cast< Internal::Adaptor::WebEngine& >( handle );
}

inline const WebEngine& GetImplementation( const Dali::WebEngine& webEngine )
{
  DALI_ASSERT_ALWAYS( webEngine && "WebEngine handle is empty." );
  const BaseObject& handle = webEngine.GetBaseObject();
  return static_cast< const Internal::Adaptor::WebEngine& >( handle );
}

namespace
{

bool OnGoBack()
{
  DisconnectFromGlobalSignal( &OnGoBack );

  if( gInstance && gInstance->CanGoBack() )
  {
    gInstance->mCurrentPlusOnePos--;
  }
  return false;
}

bool OnGoForward()
{
  DisconnectFromGlobalSignal( &OnGoForward );

  if( gInstance && gInstance->CanGoForward() )
  {
    gInstance->mCurrentPlusOnePos++;
  }
  return false;
}

bool OnLoadUrl()
{
  DisconnectFromGlobalSignal( &OnLoadUrl );

  if( gInstance )
  {
    if( gInstance->mHistory.size() > gInstance->mCurrentPlusOnePos )
    {
      gInstance->mHistory.erase( gInstance->mHistory.begin() + gInstance->mCurrentPlusOnePos, gInstance->mHistory.end() );
    }
    gInstance->mHistory.push_back( gInstance->mUrl );
    gInstance->mCurrentPlusOnePos++;
    gInstance->mPageLoadStartedSignal.Emit( gInstance->mUrl );
    gInstance->mPageLoadFinishedSignal.Emit( gInstance->mUrl );
  }
  return false;
}

bool OnEvaluteJavaScript()
{
  DisconnectFromGlobalSignal( &OnEvaluteJavaScript );

  if( gInstance )
  {
    for( auto& func : gInstance->mResultCallbacks )
    {
      func("undefined");
    }
    gInstance->mResultCallbacks.clear();
  }
  return false;
}

bool OnClearHistory()
{
  DisconnectFromGlobalSignal( &OnClearHistory );

  if( gInstance && gInstance->mCurrentPlusOnePos ) {
    std::string url = gInstance->mHistory[ gInstance->mCurrentPlusOnePos - 1 ];
    std::vector< std::string >().swap( gInstance->mHistory );
    gInstance->mHistory.push_back( url );
    gInstance->mCurrentPlusOnePos = 1;
  }
  return false;
}
} // namespace

} // namespace Adaptor

} // namespace Internal


// Dali::WebEngine Implementation
WebEngine::WebEngine()
{
}

WebEngine::WebEngine( Internal::Adaptor::WebEngine* internal )
: BaseHandle( internal )
{
}

WebEngine::~WebEngine()
{
}

WebEngine WebEngine::New()
{
  Internal::Adaptor::WebEngine* baseObject = new Internal::Adaptor::WebEngine();

  return WebEngine( baseObject );
}

WebEngine::WebEngine( const WebEngine& WebEngine )
: BaseHandle( WebEngine )
{
}

WebEngine& WebEngine::operator=( const WebEngine& webEngine )
{
  BaseHandle::operator=( webEngine );
  return *this;
}

WebEngine WebEngine::DownCast( BaseHandle handle )
{
  return WebEngine( dynamic_cast< Internal::Adaptor::WebEngine* >( handle.GetObjectPtr() ) );
}

void WebEngine::Create( int width, int height, const std::string& locale, const std::string& timezoneId )
{
}

void WebEngine::Destroy()
{
}

void WebEngine::LoadUrl( const std::string& url )
{
  return Internal::Adaptor::GetImplementation( *this ).LoadUrl( url );
}

const std::string& WebEngine::GetUrl()
{
  return Internal::Adaptor::GetImplementation( *this ).GetUrl();
}

NativeImageInterfacePtr WebEngine::GetNativeImageSource()
{
  Any source;
  Dali::NativeImageSourcePtr sourcePtr = Dali::NativeImageSource::New( source );
  return sourcePtr;
}

void WebEngine::LoadHTMLString( const std::string& htmlString )
{
}

void WebEngine::Reload()
{
}

void WebEngine::StopLoading()
{
}

void WebEngine::Suspend()
{
}

void WebEngine::Resume()
{
}

bool WebEngine::CanGoForward()
{
  return Internal::Adaptor::GetImplementation( *this ).CanGoForward();
}

void WebEngine::GoForward()
{
  Internal::Adaptor::GetImplementation( *this ).GoForward();
}

bool WebEngine::CanGoBack()
{
  return Internal::Adaptor::GetImplementation( *this ).CanGoBack();
}

void WebEngine::GoBack()
{
  Internal::Adaptor::GetImplementation( *this ).GoBack();
}

void WebEngine::EvaluateJavaScript( const std::string& script, std::function< void( const std::string& ) > resultHandler )
{
  Internal::Adaptor::GetImplementation( *this ).EvaluateJavaScript( script, resultHandler );
}

void WebEngine::AddJavaScriptMessageHandler( const std::string& exposedObjectName, std::function< void(const std::string&) > handler )
{
}

void WebEngine::ClearHistory()
{
  Internal::Adaptor::GetImplementation( *this ).ClearHistory();
}

void WebEngine::ClearCache()
{
}

void WebEngine::ClearCookies()
{
}

Dali::WebEnginePlugin::CacheModel WebEngine::GetCacheModel() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetCacheModel();
}

void WebEngine::SetCacheModel( Dali::WebEnginePlugin::CacheModel cacheModel )
{
  Internal::Adaptor::GetImplementation( *this ).SetCacheModel( cacheModel );
}

Dali::WebEnginePlugin::CookieAcceptPolicy WebEngine::GetCookieAcceptPolicy() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetCookieAcceptPolicy();
}

void WebEngine::SetCookieAcceptPolicy( Dali::WebEnginePlugin::CookieAcceptPolicy policy )
{
  Internal::Adaptor::GetImplementation( *this ).SetCookieAcceptPolicy( policy );
}

const std::string& WebEngine::GetUserAgent() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetUserAgent();
}

void WebEngine::SetUserAgent( const std::string& userAgent )
{
  Internal::Adaptor::GetImplementation( *this ).SetUserAgent( userAgent );
}

bool WebEngine::IsJavaScriptEnabled() const
{
  return Internal::Adaptor::GetImplementation( *this ).IsJavaScriptEnabled();
}

void WebEngine::EnableJavaScript( bool enabled )
{
  Internal::Adaptor::GetImplementation( *this ).EnableJavaScript( enabled );
}

bool WebEngine::AreImagesAutomaticallyLoaded() const
{
  return Internal::Adaptor::GetImplementation( *this ).AreImagesAutomaticallyLoaded();
}

void WebEngine::LoadImagesAutomatically( bool automatic )
{
  Internal::Adaptor::GetImplementation( *this ).LoadImagesAutomatically( automatic );
}

const std::string& WebEngine::GetDefaultTextEncodingName() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetDefaultTextEncodingName();
}

void WebEngine::SetDefaultTextEncodingName( const std::string& defaultTextEncodingName )
{
  Internal::Adaptor::GetImplementation( *this ).SetDefaultTextEncodingName( defaultTextEncodingName );
}

int WebEngine::GetDefaultFontSize() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetDefaultFontSize();
}

void WebEngine::SetDefaultFontSize( int defaultFontSize )
{
  Internal::Adaptor::GetImplementation( *this ).SetDefaultFontSize( defaultFontSize );
}

void WebEngine::SetSize( int width, int height )
{
}

bool WebEngine::SendTouchEvent( const TouchEvent& touch )
{
  return true;
}

bool WebEngine::SendKeyEvent( const KeyEvent& event )
{
  return true;
}

void WebEngine::SetFocus( bool focused )
{
}

Dali::WebEnginePlugin::WebEnginePageLoadSignalType& WebEngine::PageLoadStartedSignal()
{
  return Internal::Adaptor::GetImplementation( *this ).PageLoadStartedSignal();
}

Dali::WebEnginePlugin::WebEnginePageLoadSignalType& WebEngine::PageLoadFinishedSignal()
{
  return Internal::Adaptor::GetImplementation( *this ).PageLoadFinishedSignal();
}

Dali::WebEnginePlugin::WebEnginePageLoadErrorSignalType& WebEngine::PageLoadErrorSignal()
{
  return Internal::Adaptor::GetImplementation( *this ).PageLoadErrorSignal();
}

} // namespace Dali;

