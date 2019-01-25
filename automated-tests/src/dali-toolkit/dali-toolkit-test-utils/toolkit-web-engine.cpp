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

#include "toolkit-timer.h"

#include <dali/devel-api/adaptor-framework/web-engine.h>
#include <dali/public-api/object/any.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/adaptor-framework/native-image-source.h>
#include <dali/public-api/images/native-image.h>
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
  {
    gInstanceCount++;
    gInstance = this;
  }

  virtual ~WebEngine()
  {
    gInstanceCount--;
    if ( !gInstanceCount )
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

  void ClearHistory()
  {
    ConnectToGlobalSignal( &OnClearHistory );
  }

  Dali::WebEnginePlugin::WebEngineSignalType& PageLoadStartedSignal()
  {
    return mPageLoadStartedSignal;
  }

  Dali::WebEnginePlugin::WebEngineSignalType& PageLoadFinishedSignal()
  {
    return mPageLoadFinishedSignal;
  }

  std::string mUrl;
  std::vector< std::string > mHistory;
  size_t mCurrentPlusOnePos;
  Dali::WebEnginePlugin::WebEngineSignalType mPageLoadStartedSignal;
  Dali::WebEnginePlugin::WebEngineSignalType mPageLoadFinishedSignal;
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

  if ( gInstance && gInstance->CanGoBack() )
  {
    gInstance->mCurrentPlusOnePos--;
  }
  return false;
}

bool OnGoForward()
{
  DisconnectFromGlobalSignal( &OnGoForward );

  if ( gInstance && gInstance->CanGoForward() )
  {
    gInstance->mCurrentPlusOnePos++;
  }
  return false;
}

bool OnLoadUrl()
{
  DisconnectFromGlobalSignal( &OnLoadUrl );

  if ( gInstance )
  {
    if ( gInstance->mHistory.size() > gInstance->mCurrentPlusOnePos )
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

bool OnClearHistory()
{
  DisconnectFromGlobalSignal( &OnClearHistory );

  if ( gInstance && gInstance->mCurrentPlusOnePos ) {
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

void WebEngine::EvaluateJavaScript( const std::string& script )
{
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

void WebEngine::SetSize( int width, int height )
{
}

bool WebEngine::SendTouchEvent( const TouchData& touch )
{
  return true;
}

bool WebEngine::SendKeyEvent( const KeyEvent& event )
{
  return true;
}

Dali::WebEnginePlugin::WebEngineSignalType& WebEngine::PageLoadStartedSignal()
{
  return Internal::Adaptor::GetImplementation( *this ).PageLoadStartedSignal();
}

Dali::WebEnginePlugin::WebEngineSignalType& WebEngine::PageLoadFinishedSignal()
{
  return Internal::Adaptor::GetImplementation( *this ).PageLoadFinishedSignal();
}

} // namespace Dali;

