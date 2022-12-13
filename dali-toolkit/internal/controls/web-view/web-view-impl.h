#ifndef DALI_TOOLKIT_INTERNAL_WEB_VIEW_H
#define DALI_TOOLKIT_INTERNAL_WEB_VIEW_H

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

// EXTERNAL INCLUDES
#include <memory>
#include <dali/devel-api/adaptor-framework/web-engine.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/object/property-notification.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/web-view/web-view.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>

namespace Dali
{

namespace Toolkit
{

class KeyEvent;
class TouchEvent;
class WebBackForwardList;
class WebContext;
class WebCookieManager;
class WebSettings;
class WebView;

namespace Internal
{

class WebView : public Control
{
protected:

  WebView();

  WebView( const std::string& locale, const std::string& timezoneId );

  WebView( int argc, char** argv );

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
   * @brief Get settings of WebEngine.
   */
  Dali::Toolkit::WebSettings* GetSettings() const;

  /**
   * @brief Get context of WebEngine.
   */
  Dali::Toolkit::WebContext* GetContext() const;

  /**
   * @brief Get cookie manager of WebEngine.
   */
  Dali::Toolkit::WebCookieManager* GetCookieManager() const;

  /**
   * @brief Get WebBackForwardList of WebEngine.
   */
  Dali::Toolkit::WebBackForwardList* GetBackForwardList() const;

  /**
   * @copydoc Dali::Toolkit::WebView::New( int, char** )
   */
  static Toolkit::WebView New( int argc, char** argv );

  /**
   * @copydoc Dali::Toolkit::WebView::GetPlugin()
   */
  Dali::WebEnginePlugin* GetPlugin() const;

  /**
   * @brief Get favicon of web page.
   *
   * @return Handle to a favicon
   */
  Dali::Toolkit::ImageView GetFavicon() const;

  /**
   * @copydoc Dali::Toolkit::WebView::LoadUrl()
   */
  void LoadUrl( const std::string& url );

  /**
   * @copydoc Dali::WebEngine::LoadHTMLString()
   */
  void LoadHtmlString( const std::string& htmlString );

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
   * @copydoc Dali::Toolkit::WebView::ScrollBy()
   */
  void ScrollBy( int deltaX, int deltaY );

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
  void EvaluateJavaScript( const std::string& script, Dali::WebEnginePlugin::JavaScriptMessageHandlerCallback resultHandler );

  /**
   * @copydoc Dali::Toolkit::WebView::AddJavaScriptMessageHandler()
   */
  void AddJavaScriptMessageHandler( const std::string& exposedObjectName, Dali::WebEnginePlugin::JavaScriptMessageHandlerCallback handler );

  /**
   * @brief Clears all tiles resources of Web.
   */
  void ClearAllTilesResources();

  /**
   * @copydoc Dali::Toolkit::WebView::ClearHistory()
   */
  void ClearHistory();

  /**
   * @copydoc Dali::Toolkit::WebView::SetTtsFocus()
   */
  void SetTtsFocus(bool focused);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterPageLoadStartedCallback()
   */
  void RegisterPageLoadStartedCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterPageLoadFinishedCallback()
   */
  void RegisterPageLoadFinishedCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterPageLoadErrorCallback()
   */
  void RegisterPageLoadErrorCallback(Dali::WebEnginePlugin::WebEnginePageLoadErrorCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterScrollEdgeReachedCallback()
   */
  void RegisterScrollEdgeReachedCallback(Dali::WebEnginePlugin::WebEngineScrollEdgeReachedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterUrlChangedCallback()
   */
  void RegisterUrlChangedCallback(Dali::WebEnginePlugin::WebEngineUrlChangedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterNavigationPolicyDecidedCallback()
   */
  void RegisterNavigationPolicyDecidedCallback(Dali::WebEnginePlugin::WebEngineNavigationPolicyDecidedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterNewWindowCreatedCallback()
   */
  void RegisterNewWindowCreatedCallback(Dali::WebEnginePlugin::WebEngineNewWindowCreatedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::GetPlainTextAsynchronously()
   */
  void GetPlainTextAsynchronously(Dali::WebEnginePlugin::PlainTextReceivedCallback callback);

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

private: // From Control

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  void OnInitialize() override;

  /**
   * @copydoc Toolkit::Control::GetNaturalSize
   */
  Vector3 GetNaturalSize() override;

  /**
   * Signal occurs when the Web View has been touched.
   * @param[in] actor The Actor Touched
   * @param[in] touch The Touch Data.
   * @return Whether to consume event or not.
   */
  bool OnTouchEvent( Actor actor, const Dali::TouchEvent& touch );

  /**
   * @copydoc Toolkit::Control::OnKeyEvent()
   */
  bool OnKeyEvent( const Dali::KeyEvent& event ) override;

  /**
   * @copydoc Toolkit::Control::OnKeyInputFocusGained()
   */
  void OnKeyInputFocusGained() override;

  /**
   * @copydoc Toolkit::Control::OnKeyInputFocusLost()
   */
  void OnKeyInputFocusLost() override;

  /**
   * @brief Callback for notifying frame rendered.
   */
  void OnFrameRendered();

  /**
   * @brief Callback for updating display area of web view.
   * @param[in] source The soource triggers Notification.
   */
  void OnDisplayAreaUpdated(Dali::PropertyNotification& source);

  /**
   * @copydoc Toolkit::Control::OnSceneConnection()
   */
  void OnSceneConnection( int depth ) override;

private:

  // Undefined
  WebView( const WebView& webView );

  WebView& operator=( const WebView& webView );

  /**
   * @brief Sets an absolute scroll of the given view.
   * @param[in] x The coordinate x of scroll
   * @param[in] y The coordinate y of scroll
   */
  void SetScrollPosition( int x, int y );

  /**
   * @brief Gets the current scroll position of the given view.
   * @param[out] x The coordinate x of scroll
   * @param[out] y The coordinate y of scroll
   */
  void GetScrollPosition( int& x, int& y ) const;

  /**
   * @brief Gets the possible scroll size of the given view.
   * @param[out] width The width of scroll size
   * @param[out] height The height of scroll size
   */
  void GetScrollSize( int& width, int& height ) const;

  /**
   * @brief Gets the last known content's size.
   * @param[out] width The width of content's size
   * @param[out] height The height of content's size
   */
  void GetContentSize( int& width, int& height ) const;

  /**
   * @brief Get current url.
   * @return The string value of current url
   */
  std::string GetUrl() const;

  /**
   * @brief Returns the title of the Web.
   *
   * @return The title of web page
   */
  std::string GetTitle() const;

  /**
   * @brief Get user agent string.
   * @return The string value of user agent
   */
  std::string GetUserAgent() const;

  /**
   * @brief Set user agent string.
   * @param[in] userAgent The string value of user agent
   */
  void SetUserAgent( const std::string& userAgent );

  /**
   * @brief Enable/Disable video hole for video playing.
   * @param[in] enabled True if video hole is enabled, false otherwise.
   */
  void EnableVideoHole( bool enabled );

  /**
   * @brief Enable blend mode.
   * @param[in] blendEnabled True if turn on blend mode, false otherwise.
   */
  void EnableBlendMode( bool blendEnabled );

private:

  Dali::Toolkit::Visual::Base                        mVisual;
  Dali::Size                                         mWebViewSize;
  Dali::WebEngine                                    mWebEngine;

  std::unique_ptr<Dali::Toolkit::WebContext>         mWebContext;
  std::unique_ptr<Dali::Toolkit::WebCookieManager>   mWebCookieManager;
  std::unique_ptr<Dali::Toolkit::WebSettings>        mWebSettings;
  std::unique_ptr<Dali::Toolkit::WebBackForwardList> mWebBackForwardList;

  Dali::PropertyNotification                         mPositionUpdateNotification;
  Dali::PropertyNotification                         mSizeUpdateNotification;
  Dali::PropertyNotification                         mScaleUpdateNotification;
  bool                                               mVideoHoleEnabled;
  Dali::Rect< int >                                  mWebViewArea;
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
