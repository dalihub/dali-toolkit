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

#include "toolkit-timer.h"

#include <dali/devel-api/adaptor-framework/web-engine.h>
#include <dali/devel-api/adaptor-framework/web-engine-back-forward-list.h>
#include <dali/devel-api/adaptor-framework/web-engine-back-forward-list-item.h>
#include <dali/devel-api/adaptor-framework/web-engine-context.h>
#include <dali/devel-api/adaptor-framework/web-engine-cookie-manager.h>
#include <dali/devel-api/adaptor-framework/web-engine-form-repost-decision.h>
#include <dali/devel-api/adaptor-framework/web-engine-settings.h>
#include <dali/public-api/adaptor-framework/native-image-source.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/object/any.h>
#include <dali/public-api/object/base-object.h>
#include <memory>
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

// Generally only one WebEngine instance exists.
// If > 1, a new web engine has been created by CreateWindowSignal.
static WebEngine* gInstance = 0;
static int gInstanceCount = 0;

bool OnGoBack();
bool OnGoForward();
bool OnLoadUrl();
bool OnEvaluteJavaScript();
bool OnJavaScriptAlert();
bool OnJavaScriptConfirm();
bool OnJavaScriptPrompt();
bool OnScrollEdge();
bool OnClearHistory();

static void ConnectToGlobalSignal( bool ( *func )() )
{
  Dali::Timer timer = Dali::Timer::New( 0 );
  timer.TickSignal().Connect( func );
}

static void DisconnectFromGlobalSignal( bool ( *func )() )
{
  Dali::Timer timer = Dali::Timer::New( 0 );
  timer.TickSignal().Disconnect( func );
}
} // namespace anonymous

class MockWebEngineContext : public Dali::WebEngineContext
{
public:
  MockWebEngineContext()
    : mockModel( Dali::WebEngineContext::CacheModel::DOCUMENT_VIEWER )
  {
  }

  Dali::WebEngineContext::CacheModel GetCacheModel() const override
  {
    return mockModel;
  }

  void SetCacheModel( Dali::WebEngineContext::CacheModel cacheModel ) override
  {
    mockModel = cacheModel;
  }

  void SetProxyUri( const std::string& uri ) override
  {
  }

  void SetDefaultProxyAuth( const std::string& username, const std::string& password ) override
  {
  }

  void SetCertificateFilePath( const std::string& certificatePath ) override
  {
  }

  void DeleteWebDatabase() override
  {
  }

  void DeleteWebStorage() override
  {
  }

  void DeleteLocalFileSystem() override
  {
  }

  void DisableCache( bool cacheDisabled ) override
  {
  }

  void ClearCache() override
  {
  }

private:
  Dali::WebEngineContext::CacheModel mockModel;
};

class MockWebEngineCookieManager : public Dali::WebEngineCookieManager
{
public:
  MockWebEngineCookieManager()
    : mockCookieAcceptPolicy( Dali::WebEngineCookieManager::CookieAcceptPolicy::NO_THIRD_PARTY )
  {
  }

  void SetCookieAcceptPolicy( Dali::WebEngineCookieManager::CookieAcceptPolicy policy ) override
  {
    mockCookieAcceptPolicy = policy;
  }

  Dali::WebEngineCookieManager::CookieAcceptPolicy GetCookieAcceptPolicy() const override
  {
    return mockCookieAcceptPolicy;
  }

  void ClearCookies() override
  {
  }

  void SetPersistentStorage( const std::string& path, Dali::WebEngineCookieManager::CookiePersistentStorage storage ) override
  {
  }

private:
  Dali::WebEngineCookieManager::CookieAcceptPolicy mockCookieAcceptPolicy;
};

class MockWebEngineBackForwardListItem : public Dali::WebEngineBackForwardListItem
{
public:
  MockWebEngineBackForwardListItem()
    : mockUrl( "http://url" ),
      mockTitle( "title" ),
      mockOriginalUrl( "http://originalurl" )
  {
  }

  std::string GetUrl() const override
  {
    return mockUrl;
  }

  std::string GetTitle() const override
  {
    return mockTitle;
  }

  std::string GetOriginalUrl() const override
  {
    return mockOriginalUrl;
  }

private:
  std::string mockUrl;
  std::string mockTitle;
  std::string mockOriginalUrl;
};

class MockWebEngineBackForwardList : public Dali::WebEngineBackForwardList
{
public:
  MockWebEngineBackForwardList( )
    : mockItem(),
      pMockItem( &mockItem )
  {
  }

  Dali::WebEngineBackForwardListItem& GetCurrentItem() const override
  {
    return *pMockItem;
  }

  Dali::WebEngineBackForwardListItem& GetItemAtIndex( uint32_t index ) const override
  {
    return *pMockItem;
  }

  uint32_t GetItemCount() const override
  {
    return 1;
  }

private:
  MockWebEngineBackForwardListItem mockItem;
  WebEngineBackForwardListItem* pMockItem;
};

class MockWebEngineFormRepostDecision : public WebEngineFormRepostDecision
{
public:
  MockWebEngineFormRepostDecision()
  {
  }

  void Reply(bool allowed) override {}
};

class MockWebEngineSettings : public WebEngineSettings
{
public:
  MockWebEngineSettings()
    : mockDefaultFontSize( 16 ),
      mockJavaScriptEnabled( true ),
      mockAutoFittingEnabled ( true ),
      mockPluginsEnabled ( true ),
      mockPrivateBrowsingEnabled( true ),
      mockLinkMagnifierEnabled( true ),
      mockKeypadWithoutUserActionUsed( true ),
      mockAutofillPasswordFormEnabled( true ),
      mockFormCandidateDataEnabled( true ),
      mockTextSelectionEnabled( true ),
      mockTextAutosizingEnable( true ),
      mockArrowScrollEnable( true ),
      mockClipboardEnabled( true ),
      mockImePanelEnabled( true ),
      mockImageLoadedAutomatically( true ),
      mockDefaultTextEncodingName()
  {
  }

  uint32_t GetDefaultFontSize() const override
  {
    return mockDefaultFontSize;
  }

  void SetDefaultFontSize( uint32_t size ) override
  {
    mockDefaultFontSize = size;
  }

  bool IsJavaScriptEnabled() const override
  {
    return mockJavaScriptEnabled;
  }

  void EnableJavaScript( bool enabled ) override
  {
    mockJavaScriptEnabled = enabled;
  }

  bool IsAutoFittingEnabled() const override
  {
    return mockAutoFittingEnabled;
  }

  void EnableAutoFitting( bool enabled ) override
  {
    mockAutoFittingEnabled = enabled;
  }

  bool ArePluginsEnabled() const override
  {
    return mockPluginsEnabled;
  }

  void EnablePlugins( bool enabled ) override
  {
    mockPluginsEnabled = enabled;
  }

  bool IsPrivateBrowsingEnabled() const override
  {
    return mockPrivateBrowsingEnabled;
  }

  void EnablePrivateBrowsing( bool enabled ) override
  {
    mockPrivateBrowsingEnabled = enabled;
  }

  bool IsLinkMagnifierEnabled() const override
  {
    return mockLinkMagnifierEnabled;
  }

  void EnableLinkMagnifier( bool enabled ) override
  {
    mockLinkMagnifierEnabled = enabled;
  }

  bool IsKeypadWithoutUserActionUsed() const override
  {
    return mockKeypadWithoutUserActionUsed;
  }

  void UseKeypadWithoutUserAction( bool used ) override
  {
    mockKeypadWithoutUserActionUsed = used;
  }

  bool IsAutofillPasswordFormEnabled() const override
  {
    return mockAutofillPasswordFormEnabled;
  }

  void EnableAutofillPasswordForm( bool enabled ) override
  {
    mockAutofillPasswordFormEnabled = enabled;
  }

  bool IsFormCandidateDataEnabled() const override
  {
    return mockFormCandidateDataEnabled;
  }

  void EnableFormCandidateData( bool enabled ) override
  {
    mockFormCandidateDataEnabled = enabled;
  }

  bool IsTextSelectionEnabled() const override
  {
    return mockTextSelectionEnabled;
  }

  void EnableTextSelection( bool enabled ) override
  {
    mockTextSelectionEnabled = enabled;
  }

  bool IsTextAutosizingEnabled() const override
  {
    return mockTextAutosizingEnable;
  }

  void EnableTextAutosizing( bool enabled ) override
  {
    mockTextAutosizingEnable = enabled;
  }

  bool IsArrowScrollEnabled() const override
  {
    return mockArrowScrollEnable;
  }

  void EnableArrowScroll( bool enabled ) override
  {
    mockArrowScrollEnable = enabled;
  }

  bool IsClipboardEnabled() const override
  {
    return mockClipboardEnabled;
  }

  void EnableClipboard( bool enabled ) override
  {
    mockClipboardEnabled = enabled;
  }

  bool IsImePanelEnabled() const override
  {
    return mockImePanelEnabled;
  }

  void EnableImePanel( bool enabled ) override
  {
    mockImePanelEnabled = enabled;
  }

  bool AreImagesLoadedAutomatically() const override
  {
    return mockImageLoadedAutomatically;
  }

  void AllowImagesLoadAutomatically( bool automatic ) override
  {
    mockImageLoadedAutomatically = automatic;
  }

  std::string GetDefaultTextEncodingName() const override
  {
    return mockDefaultTextEncodingName;
  }

  void SetDefaultTextEncodingName( const std::string& defaultTextEncodingName ) override
  {
    mockDefaultTextEncodingName = defaultTextEncodingName;
  }

  void AllowMixedContents( bool allowed ) override
  {
  }

  void EnableSpatialNavigation( bool enabled ) override
  {
  }

  void EnableWebSecurity( bool enabled ) override
  {
  }

  void EnableCacheBuilder( bool enabled ) override
  {
  }

  void UseScrollbarThumbFocusNotifications( bool used ) override
  {
  }

  void EnableDoNotTrack( bool enabled ) override
  {
  }

  void AllowFileAccessFromExternalUrl( bool allowed ) override
  {
  }

  void AllowScriptsOpenWindows( bool allowed ) override
  {
  }

private:
  int mockDefaultFontSize;
  bool mockJavaScriptEnabled;
  bool mockAutoFittingEnabled;
  bool mockPluginsEnabled;
  bool mockPrivateBrowsingEnabled;
  bool mockLinkMagnifierEnabled;
  bool mockKeypadWithoutUserActionUsed;
  bool mockAutofillPasswordFormEnabled;
  bool mockFormCandidateDataEnabled;
  bool mockTextSelectionEnabled;
  bool mockTextAutosizingEnable;
  bool mockArrowScrollEnable;
  bool mockClipboardEnabled;
  bool mockImePanelEnabled;
  bool mockImageLoadedAutomatically;
  std::string mockDefaultTextEncodingName;
};

class WebEngine: public Dali::BaseObject
{
public:

  using JavaScriptEvaluatedResultCallback = std::function<void(const std::string&)>;
  using JavaScriptAlertCallback = std::function<bool(const std::string&)>;
  using JavaScriptConfirmCallback = std::function<bool(const std::string&)>;
  using JavaScriptPromptCallback = std::function<bool(const std::string&, const std::string&)>;

  WebEngine()
    : mUrl()
    , mCurrentPlusOnePos( 0 )
    , mUserAgent()
    , mEvaluating( false )
    , mScrollPosition( 0, 0 )
    , mScrollSize( 500, 500 )
    , mContentSize( 500, 500 )
  {
    gInstanceCount++;
    if ( gInstanceCount == 1 ) // only first web engine need be saved.
    {
      gInstance = this;
    }

    mockWebEngineSettings = new MockWebEngineSettings();
    mockWebEngineContext = new MockWebEngineContext();
    mockWebEngineCookieManager = new MockWebEngineCookieManager();
    mockWebEngineBackForwardList = new MockWebEngineBackForwardList();
  }

  virtual ~WebEngine()
  {
    gInstanceCount--;
    if( !gInstanceCount )
    {
      gInstance = 0;
    }

    delete mockWebEngineSettings;
    delete mockWebEngineContext;
    delete mockWebEngineCookieManager;
    delete mockWebEngineBackForwardList;
  }

  Dali::WebEngineSettings& GetSettings() const
  {
    return *mockWebEngineSettings;
  }

  Dali::WebEngineContext& GetContext() const
  {
    return *mockWebEngineContext;
  }

  Dali::WebEngineCookieManager& GetCookieManager() const
  {
    return *mockWebEngineCookieManager;
  }

  Dali::WebEngineBackForwardList& GetBackForwardList() const
  {
    return *mockWebEngineBackForwardList;
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

  std::string GetTitle() const
  {
    return std::string("title");
  }

  Dali::PixelData GetFavicon() const
  {
    uint8_t* faviconData = new uint8_t[ 16 ];

    faviconData[ 0 ] = 0xff;
    faviconData[ 1 ] = 0x00;
    faviconData[ 2 ] = 0x00;
    faviconData[ 3 ] = 0xff;
    faviconData[ 4 ] = 0xff;
    faviconData[ 5 ] = 0x00;
    faviconData[ 6 ] = 0x00;
    faviconData[ 7 ] = 0xff;
    faviconData[ 8 ] = 0xff;
    faviconData[ 9 ] = 0x00;
    faviconData[ 10 ] = 0x00;
    faviconData[ 11 ] = 0xff;
    faviconData[ 12 ] = 0xff;
    faviconData[ 13 ] = 0x00;
    faviconData[ 14 ] = 0x00;
    faviconData[ 15 ] = 0xff;

    return Dali::PixelData::New( faviconData, 16, 2, 2,
                                 Dali::Pixel::Format::RGBA8888,
                                 Dali::PixelData::ReleaseFunction::DELETE_ARRAY );
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

  void RegisterJavaScriptAlertCallback( Dali::WebEnginePlugin::JavaScriptAlertCallback callback )
  {
    if ( callback )
    {
      ConnectToGlobalSignal( &OnJavaScriptAlert );
      mJavaScriptAlertCallback = callback;
    }
  }

  void RegisterJavaScriptConfirmCallback( Dali::WebEnginePlugin::JavaScriptConfirmCallback callback )
  {
    if ( callback )
    {
      ConnectToGlobalSignal( &OnJavaScriptConfirm );
      mJavaScriptConfirmCallback = callback;
    }
  }

  void RegisterJavaScriptPromptCallback( Dali::WebEnginePlugin::JavaScriptPromptCallback callback )
  {
    if ( callback )
    {
      ConnectToGlobalSignal( &OnJavaScriptPrompt );
      mJavaScriptPromptCallback = callback;
    }
  }

  void ClearHistory()
  {
    ConnectToGlobalSignal( &OnClearHistory );
  }

  const std::string& GetUserAgent() const
  {
    return mUserAgent;
  }

  void SetUserAgent( const std::string& userAgent )
  {
    mUserAgent = userAgent;
  }

  void ScrollBy( int dx, int dy )
  {
    mScrollPosition += Dali::Vector2( dx, dy );
    if ( mScrollPosition.y + mScrollSize.height > mContentSize.height )
    {
      ConnectToGlobalSignal( &OnScrollEdge );
    }
  }

  void SetScrollPosition( int x, int y )
  {
    mScrollPosition.x = x;
    mScrollPosition.y = y;
  }

  Dali::Vector2 GetScrollPosition() const
  {
    return mScrollPosition;
  }

  Dali::Vector2 GetScrollSize() const
  {
    return mScrollSize;
  }

  Dali::Vector2 GetContentSize() const
  {
    return  mContentSize;
  }

  Dali::WebEnginePlugin::WebEnginePageLoadSignalType& PageLoadStartedSignal()
  {
    return mPageLoadStartedSignal;
  }

  Dali::WebEnginePlugin::WebEnginePageLoadSignalType& PageLoadInProgressSignal()
  {
    return mPageLoadInProgressSignal;
  }

  Dali::WebEnginePlugin::WebEnginePageLoadSignalType& PageLoadFinishedSignal()
  {
    return mPageLoadFinishedSignal;
  }

  Dali::WebEnginePlugin::WebEnginePageLoadErrorSignalType& PageLoadErrorSignal()
  {
    return mPageLoadErrorSignal;
  }

  Dali::WebEnginePlugin::WebEngineScrollEdgeReachedSignalType& ScrollEdgeReachedSignal()
  {
    return mScrollEdgeReachedSignal;
  }

  Dali::WebEnginePlugin::WebEngineUrlChangedSignalType& UrlChangedSignal()
  {
    return mUrlChangedSignal;
  }

  Dali::WebEnginePlugin::WebEngineFormRepostDecisionSignalType& FormRepostDecisionSignal()
  {
    return mFormRepostDecisionSignal;
  }

  Dali::WebEnginePlugin::WebEngineFrameRenderedSignalType& FrameRenderedSignal()
  {
    return mFrameRenderedSignal;
  }

  std::string                                                mUrl;
  std::vector< std::string >                                 mHistory;
  size_t                                                     mCurrentPlusOnePos;
  std::string                                                mUserAgent;
  Dali::WebEnginePlugin::WebEnginePageLoadSignalType         mPageLoadStartedSignal;
  Dali::WebEnginePlugin::WebEnginePageLoadSignalType         mPageLoadInProgressSignal;
  Dali::WebEnginePlugin::WebEnginePageLoadSignalType         mPageLoadFinishedSignal;
  Dali::WebEnginePlugin::WebEnginePageLoadErrorSignalType    mPageLoadErrorSignal;
  std::vector<JavaScriptEvaluatedResultCallback>             mResultCallbacks;
  bool                                                       mEvaluating;

  Dali::WebEnginePlugin::WebEngineScrollEdgeReachedSignalType  mScrollEdgeReachedSignal;
  Dali::Vector2                                                mScrollPosition;
  Dali::Vector2                                                mScrollSize;
  Dali::Vector2                                                mContentSize;
  WebEngineBackForwardList*                                    mockWebEngineBackForwardList;
  WebEngineContext*                                            mockWebEngineContext;
  WebEngineCookieManager*                                      mockWebEngineCookieManager;
  WebEngineSettings*                                           mockWebEngineSettings;
  Dali::WebEnginePlugin::WebEngineUrlChangedSignalType         mUrlChangedSignal;
  Dali::WebEnginePlugin::WebEngineFormRepostDecisionSignalType mFormRepostDecisionSignal;
  Dali::WebEnginePlugin::WebEngineFrameRenderedSignalType      mFrameRenderedSignal;

  JavaScriptAlertCallback                                     mJavaScriptAlertCallback;
  JavaScriptConfirmCallback                                   mJavaScriptConfirmCallback;
  JavaScriptPromptCallback                                    mJavaScriptPromptCallback;
};


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
    gInstance->mPageLoadInProgressSignal.Emit( gInstance->mUrl );
    gInstance->mPageLoadFinishedSignal.Emit( gInstance->mUrl );
    gInstance->mUrlChangedSignal.Emit( "http://new-test" );

    std::shared_ptr<Dali::WebEngineFormRepostDecision> decision(new MockWebEngineFormRepostDecision());
    gInstance->mFormRepostDecisionSignal.Emit(decision);
    gInstance->mFrameRenderedSignal.Emit();
  }
  return false;
}

bool OnScrollEdge()
{
  DisconnectFromGlobalSignal( &OnScrollEdge );

  if( gInstance )
  {
    gInstance->mScrollEdgeReachedSignal.Emit( Dali::WebEnginePlugin::ScrollEdge::BOTTOM );
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

bool OnJavaScriptAlert()
{
  DisconnectFromGlobalSignal( &OnJavaScriptAlert );
  if ( gInstance )
  {
    gInstance->mJavaScriptAlertCallback( "this is an alert popup." );
  }
  return false;
}

bool OnJavaScriptConfirm()
{
  DisconnectFromGlobalSignal( &OnJavaScriptConfirm );
  if ( gInstance )
  {
    gInstance->mJavaScriptConfirmCallback( "this is a confirm popup." );
  }
  return false;
}

bool OnJavaScriptPrompt()
{
  DisconnectFromGlobalSignal( &OnJavaScriptPrompt );
  if ( gInstance )
  {
    gInstance->mJavaScriptPromptCallback( "this is a prompt pompt.", "" );
  }
  return false;
}

bool OnClearHistory()
{
  DisconnectFromGlobalSignal( &OnClearHistory );

  if( gInstance && gInstance->mCurrentPlusOnePos )
  {
    std::string url = gInstance->mHistory[ gInstance->mCurrentPlusOnePos - 1 ];
    std::vector< std::string >().swap( gInstance->mHistory );
    gInstance->mHistory.push_back( url );
    gInstance->mCurrentPlusOnePos = 1;
  }
  return false;
}

} // namespace

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

void WebEngine::Create( int width, int height, int argc, char** argv )
{
}

void WebEngine::Destroy()
{
}

WebEngineSettings& WebEngine::GetSettings() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetSettings();
}

WebEngineContext& WebEngine::GetContext() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetContext();
}

WebEngineCookieManager& WebEngine::GetCookieManager() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetCookieManager();
}

WebEngineBackForwardList& WebEngine::GetBackForwardList() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetBackForwardList();
}

void WebEngine::LoadUrl( const std::string& url )
{
  return Internal::Adaptor::GetImplementation( *this ).LoadUrl( url );
}

std::string WebEngine::GetTitle() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetTitle();
}

Dali::PixelData WebEngine::GetFavicon() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetFavicon();
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

void WebEngine::LoadHtmlString( const std::string& htmlString )
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

void WebEngine::RegisterJavaScriptAlertCallback( Dali::WebEnginePlugin::JavaScriptAlertCallback callback )
{
  Internal::Adaptor::GetImplementation( *this ).RegisterJavaScriptAlertCallback( callback );
}

void WebEngine::JavaScriptAlertReply()
{
}

void WebEngine::RegisterJavaScriptConfirmCallback( Dali::WebEnginePlugin::JavaScriptConfirmCallback callback )
{
  Internal::Adaptor::GetImplementation( *this ).RegisterJavaScriptConfirmCallback( callback );
}

void WebEngine::JavaScriptConfirmReply( bool confirmed )
{
}

void WebEngine::RegisterJavaScriptPromptCallback( Dali::WebEnginePlugin::JavaScriptPromptCallback callback )
{
  Internal::Adaptor::GetImplementation( *this ).RegisterJavaScriptPromptCallback( callback );
}

void WebEngine::JavaScriptPromptReply( const std::string& result )
{
}

void WebEngine::ClearAllTilesResources()
{
}

void WebEngine::ClearHistory()
{
  Internal::Adaptor::GetImplementation( *this ).ClearHistory();
}

const std::string& WebEngine::GetUserAgent() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetUserAgent();
}

void WebEngine::SetUserAgent( const std::string& userAgent )
{
  Internal::Adaptor::GetImplementation( *this ).SetUserAgent( userAgent );
}

void WebEngine::ScrollBy( int dx, int dy )
{
  Internal::Adaptor::GetImplementation( *this ).ScrollBy( dx, dy );
}

void WebEngine::SetScrollPosition( int x, int y )
{
  Internal::Adaptor::GetImplementation( *this ).SetScrollPosition( x, y );
}

Dali::Vector2 WebEngine::GetScrollPosition() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetScrollPosition();
}

Dali::Vector2 WebEngine::GetScrollSize() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetScrollSize();
}

Dali::Vector2 WebEngine::GetContentSize() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetContentSize();
}

void WebEngine::SetSize( int width, int height )
{
}

void WebEngine::SetDocumentBackgroundColor(Dali::Vector4 color)
{
}

void WebEngine::ClearTilesWhenHidden(bool cleared)
{
}

void WebEngine::SetTileCoverAreaMultiplier(float multiplier)
{
}

void WebEngine::EnableCursorByClient(bool enabled)
{
}

std::string WebEngine::GetSelectedText() const
{
  return "test";
}

bool WebEngine::SendTouchEvent( const TouchEvent& touch )
{
  return true;
}

bool WebEngine::SendKeyEvent( const KeyEvent& event )
{
  return true;
}

bool WebEngine::SendHoverEvent( const HoverEvent& event )
{
  return true;
}

bool WebEngine::SendWheelEvent( const WheelEvent& event )
{
  return true;
}

void WebEngine::SetFocus( bool focused )
{
}

void WebEngine::UpdateDisplayArea( Dali::Rect< int > displayArea )
{
}

void WebEngine::EnableVideoHole( bool enabled )
{
}

void WebEngine::EnableMouseEvents( bool enabled )
{
}

void WebEngine::EnableKeyEvents( bool enabled )
{
}

Dali::WebEnginePlugin::WebEnginePageLoadSignalType& WebEngine::PageLoadStartedSignal()
{
  return Internal::Adaptor::GetImplementation( *this ).PageLoadStartedSignal();
}

Dali::WebEnginePlugin::WebEnginePageLoadSignalType& WebEngine::PageLoadInProgressSignal()
{
  return Internal::Adaptor::GetImplementation( *this ).PageLoadInProgressSignal();
}

Dali::WebEnginePlugin::WebEnginePageLoadSignalType& WebEngine::PageLoadFinishedSignal()
{
  return Internal::Adaptor::GetImplementation( *this ).PageLoadFinishedSignal();
}

Dali::WebEnginePlugin::WebEnginePageLoadErrorSignalType& WebEngine::PageLoadErrorSignal()
{
  return Internal::Adaptor::GetImplementation( *this ).PageLoadErrorSignal();
}

Dali::WebEnginePlugin::WebEngineScrollEdgeReachedSignalType& WebEngine::ScrollEdgeReachedSignal()
{
  return Internal::Adaptor::GetImplementation( *this ).ScrollEdgeReachedSignal();
}

Dali::WebEnginePlugin::WebEngineUrlChangedSignalType& WebEngine::UrlChangedSignal()
{
  return Internal::Adaptor::GetImplementation( *this ).UrlChangedSignal();
}

Dali::WebEnginePlugin::WebEngineFormRepostDecisionSignalType& WebEngine::FormRepostDecisionSignal()
{
  return Internal::Adaptor::GetImplementation( *this ).FormRepostDecisionSignal();
}

Dali::WebEnginePlugin::WebEngineFrameRenderedSignalType& WebEngine::FrameRenderedSignal()
{
  return Internal::Adaptor::GetImplementation( *this ).FrameRenderedSignal();
}

} // namespace Dali;

