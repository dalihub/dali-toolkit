#ifndef DALI_TOOLKIT_INTERNAL_WEB_VIEW_H
#define DALI_TOOLKIT_INTERNAL_WEB_VIEW_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/web-engine.h>
#include <dali/public-api/images/image-operations.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/controls/web-view/web-view.h>

namespace Dali
{

namespace Toolkit
{

class KeyEvent;
class TouchData;
class WebView;

namespace Internal
{

class WebView : public Control
{
protected:

  WebView();

  WebView( const std::string& locale, const std::string& timezoneId );

  virtual ~WebView();

public:

  /**
   * @copydoc Dali::Toolkit::WebView::New()
   */
  static Toolkit::WebView New();

  /**
   * @copydoc Dali::Toolkit::WebView::New( const std::string&, const std::string& )
   */
  static Toolkit::WebView New( const std::string& locale, const std::string& timezoneId );

  /**
   * @copydoc Dali::Toolkit::WebView::LoadUrl()
   */
  void LoadUrl( const std::string& url );

  /**
   * @copydoc Dali::WebEngine::LoadHTMLString()
   */
  void LoadHTMLString( const std::string& htmlString );

  /**
   * @copydoc Dali::Toolkit::WebView::Reload()
   */
  void Reload();

  /**
   * @copydoc Dali::Toolkit::WebView::StopLoading()
   */
  void StopLoading();

  /**
   * @copydoc Dali::Toolkit::WebView::StopLoading()
   */
  void Suspend();

  /**
   * @copydoc Dali::Toolkit::WebView::Resume()
   */
  void Resume();

  /**
   * @copydoc Dali::Toolkit::WebView::CanGoForward()
   */
  bool CanGoForward();

  /**
   * @copydoc Dali::Toolkit::WebView::GoForward()
   */
  void GoForward();

  /**
   * @copydoc Dali::Toolkit::WebView::CanGoBack()
   */
  bool CanGoBack();

  /**
   * @copydoc Dali::Toolkit::WebView::GoBack()
   */
  void GoBack();

  /**
   * @copydoc Dali::Toolkit::WebView::EvaluateJavaScript()
   */
  void EvaluateJavaScript( const std::string& script, std::function< void( const std::string& ) > resultHandler );

  /**
   * @copydoc Dali::Toolkit::WebView::AddJavaScriptMessageHandler()
   */
  void AddJavaScriptMessageHandler( const std::string& exposedObjectName, std::function< void( const std::string& ) > handler );

  /**
   * @copydoc Dali::Toolkit::WebView::ClearHistory()
   */
  void ClearHistory();

  /**
   * @copydoc Dali::Toolkit::WebView::ClearCache()
   */
  void ClearCache();

  /**
   * @copydoc Dali::Toolkit::WebView::ClearCookies()
   */
  void ClearCookies();

  /**
   * @copydoc Dali::Toolkit::WebView::PageLoadStartedSignal()
   */
  Dali::Toolkit::WebView::WebViewPageLoadSignalType& PageLoadStartedSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::PageLoadFinishedSignal()
   */
  Dali::Toolkit::WebView::WebViewPageLoadSignalType& PageLoadFinishedSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::PageLoadErrorSignal()
   */
  Dali::Toolkit::WebView::WebViewPageLoadErrorSignalType& PageLoadErrorSignal();

public: // Properties

  /**
   * @brief Called when a property of an object of this type is set.
   *
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value );

  /**
   * @brief Called to retrieve a property of an object of this type.
   *
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Dali::Property::Value GetProperty( Dali::BaseObject* object, Dali::Property::Index propertyIndex );

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the c
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

private: // From Control

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::Control::GetNaturalSize
   */
  virtual Vector3 GetNaturalSize();

  /**
   * @copydoc Toolkit::Control::OnRelayout()
   */
  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container );

  /**
   * Signal occurs when the Web View has been touched.
   * @param[in] actor The Actor Touched
   * @param[in] touch The Touch Data.
   * @return Whether to consume event or not.
   */
  bool OnTouchEvent( Actor actor, const Dali::TouchData& touch );

  /**
   * @copydoc Toolkit::Control::OnKeyEvent()
   */
  virtual bool OnKeyEvent( const Dali::KeyEvent& event );

private:

  // Undefined
  WebView( const WebView& webView );

  WebView& operator=( const WebView& webView );

  /**
   * @brief Get cache model option. The default isToolkit::WebView::CacheModel::DOCUMENT_VIEWER.
   * @see Toolkit::WebView::CacheModel::Type
   */
  Toolkit::WebView::CacheModel::Type GetCacheModel() const;

  /**
   * @brief Set cache model option. The default isToolkit::WebView::CacheModel::DOCUMENT_VIEWER.
   * @param[in] cacheModel The cache model option
   * @see Toolkit::WebView::CacheModel::Type
   */
  void SetCacheModel( Toolkit::WebView::CacheModel::Type cacheModel );

  /**
   * @brief Gets the cookie acceptance policy. The default is Toolkit::WebView::CookieAcceptPolicy::NO_THIRD_PARTY.
   * @see Toolkit::WebView::CookieAcceptPolicy::Type
   */
  Toolkit::WebView::CookieAcceptPolicy::Type GetCookieAcceptPolicy() const;

  /**
   * @brief Sets the cookie acceptance policy. The default is Toolkit::WebView::CookieAcceptPolicy::NO_THIRD_PARTY.
   * @param[in] policy The cookie acceptance policy
   * @see Toolkit::WebView::CookieAcceptPolicy::Type
   */
  void SetCookieAcceptPolicy( Toolkit::WebView::CookieAcceptPolicy::Type policy );

  /**
   * @brief Get user agent string.
   * @return The string value of user agent
   */
  const std::string& GetUserAgent() const;

  /**
   * @brief Set user agent string.
   * @param[in] userAgent The string value of user agent
   */
  void SetUserAgent( const std::string& userAgent );

  /**
   * @brief Returns whether JavaScript can be executable. The default is true.
   *
   * @return true if JavaScript executing is enabled, false otherwise
   */
  bool IsJavaScriptEnabled() const;

  /**
   * @brief Enables/disables JavaScript executing. The default is enabled.
   *
   * @param[in] enabled True if JavaScript executing is enabled, false otherwise
   */
  void EnableJavaScript( bool enabled );

  /**
   * @brief Returns whether images can be loaded automatically. The default is true.
   *
   * @return true if images are loaded automatically, false otherwise
   */
  bool AreImagesAutomaticallyLoaded() const;

  /**
   * @brief Enables/disables auto loading of images. The default is enabled.
   *
   * @param[in] automatic True if images are loaded automatically, false otherwise
   */
  void LoadImagesAutomatically( bool automatic );

  /**
   * @brief Gets the default text encoding name (e.g. UTF-8).
   *
   * @return The default text encoding name
   */
  const std::string& GetDefaultTextEncodingName() const;

  /**
   * @brief Sets the default text encoding name (e.g. UTF-8).
   *
   * @param[in] defaultTextEncodingName The default text encoding name
   */
  void SetDefaultTextEncodingName( const std::string& defaultTextEncodingName );

  /**
   * @brief Returns the default font size in pixel. The default value is 16.
   *
   * @return The default font size
   */
  int GetDefaultFontSize() const;

  /**
   * @brief Sets the default font size in pixel. The default value is 16.
   *
   * @param[in] defaultFontSize A new default font size to set
   */
  void SetDefaultFontSize( int defaultFontSize );

  /**
   * @brief Callback function to be called when page load started.
   * @param[in] url The url currently being loaded
   */
  void OnPageLoadStarted( const std::string& url );

  /**
   * @brief Callback function to be called when page load finished.
   * @param[in] url The url currently being loaded
   */
  void OnPageLoadFinished( const std::string& url );

  /**
   * @brief Callback function to be called when there is an error in page loading.
   * @param[in] url The url currently being loaded
   * @param[in] errorCode The error code
   */
  void OnPageLoadError( const std::string& url, int errorCode );

private:

  std::string                                            mUrl;
  Dali::Toolkit::Visual::Base                            mVisual;
  Dali::Size                                             mWebViewSize;
  Dali::WebEngine                                        mWebEngine;

  Dali::Toolkit::WebView::WebViewPageLoadSignalType      mPageLoadStartedSignal;
  Dali::Toolkit::WebView::WebViewPageLoadSignalType      mPageLoadFinishedSignal;
  Dali::Toolkit::WebView::WebViewPageLoadErrorSignalType mPageLoadErrorSignal;
};

} // namespace Internal

inline Toolkit::Internal::WebView& GetImpl( Toolkit::WebView& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  Dali::RefObject& impl = handle.GetImplementation();
  return static_cast< Toolkit::Internal::WebView& >( impl );
}

inline const Toolkit::Internal::WebView& GetImpl( const Toolkit::WebView& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  const Dali::RefObject& impl = handle.GetImplementation();
  return static_cast< const Toolkit::Internal::WebView& >( impl );
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_WEB_VIEW_H
