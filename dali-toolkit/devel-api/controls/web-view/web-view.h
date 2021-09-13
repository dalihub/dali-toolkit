#ifndef DALI_TOOLKIT_WEB_VIEW_H
#define DALI_TOOLKIT_WEB_VIEW_H

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
#include <functional>

// INTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/web-engine-plugin.h>
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{
namespace Toolkit
{
class ImageView;
class WebBackForwardList;
class WebContext;
class WebCookieManager;
class WebSettings;

namespace Internal DALI_INTERNAL
{
class WebView;
} // namespace DALI_INTERNAL

/**
 * @addtogroup dali_toolkit_controls_web_view
 * @{
 */

/**
 * @brief WebView is a control for displaying web content.
 *
 * This enables embedding web pages in the application.
 *
 * For working WebView, a web engine plugin for a platform should be provided.
 *
 */
class DALI_TOOLKIT_API WebView : public Control
{
public:

  /**
   * @brief Enumeration for the start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000,
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the WebView class.
   */
  struct Property
  {
    enum
    {
      /**
       * @brief The url to load.
       * @details name "url", type Property::STRING.
       */
      URL = PROPERTY_START_INDEX,

      /**
       * @brief The user agent string.
       * @details name "userAgent", type Property::STRING.
       */
      USER_AGENT,

      /**
       * @brief The current position of scroll.
       * @details name "scrollPosition", type Property::VECTOR2.
       */
      SCROLL_POSITION,

      /**
       * @brief The current size of scroll.
       * @details name "scrollSize", type Property::VECTOR2.
       * @note The value is read-only.
       */
      SCROLL_SIZE,

      /**
       * @brief The current size of content.
       * @details name "contentSize", type Property::VECTOR2.
       * @note The value is read-only.
       */
      CONTENT_SIZE,

      /**
       * @brief The title of web page.
       * @details name "title", type Property::STRING.
       * @note The value is read-only.
       */
      TITLE,

      /**
       * @brief Whether video hole is enabled or not.
       * @details name "videoHoleEnabled", type Property::BOOLEAN.
       * @note The value is read-only.
       */
      VIDEO_HOLE_ENABLED,
    };
  };

  /**
   * @brief Enumeration for indicating error code of page loading.
   */
  enum class LoadErrorCode
  {
    UNKNOWN = 0,            ///< Unknown.
    CANCELED,               ///< User canceled.
    CANT_SUPPORT_MIMETYPE,  ///< Can't show the page for this MIME type.
    FAILED_FILE_IO,         ///< File IO error.
    CANT_CONNECT,           ///< Cannot connect to the network.
    CANT_LOOKUP_HOST,       ///< Fail to look up host from the DNS.
    FAILED_TLS_HANDSHAKE,   ///< Fail to SSL/TLS handshake.
    INVALID_CERTIFICATE,    ///< Received certificate is invalid.
    REQUEST_TIMEOUT,        ///< Connection timeout.
    TOO_MANY_REDIRECTS,     ///< Too many redirects.
    TOO_MANY_REQUESTS,      ///< Too many requests during this load.
    BAD_URL,                ///< Malformed URL.
    UNSUPPORTED_SCHEME,     ///< Unsupported scheme.
    AUTHENTICATION,         ///< User authentication failed on the server.
    INTERNAL_SERVER         ///< Web server has an internal server error.
  };

  /**
   * @brief WebView signal type related with page loading.
   */
  typedef Signal<void(WebView, const std::string&)> WebViewPageLoadSignalType;

  /**
   * @brief WebView signal type related with page loading error.
   */
  typedef Signal<void(WebView, const std::string&, LoadErrorCode)> WebViewPageLoadErrorSignalType;

  /**
   * @brief WebView signal type related with scroll edge reached.
   */
  typedef Signal<void(WebView, Dali::WebEnginePlugin::ScrollEdge)> WebViewScrollEdgeReachedSignalType;

public:
  /**
   * @brief Creates an initialized WebView.
   * @return A handle to a newly allocated Dali WebView
   *
   * @note WebView will not display anything
   */
  static WebView New();

  /**
   * @brief Creates an initialized WebView.
   *
   * @param [in] locale The locale of Web
   * @param [in] timezoneId The timezoneId of Web
   */
  static WebView New(const std::string& locale, const std::string& timezoneId);

  /**
   * @brief Creates an initialized WebView.
   *
   * @param [in] argc The count of arguments of Applications
   * @param [in] argv The string array of arguments of Applications
   */
  static WebView New( int argc, char** argv );

  /**
   * @brief Creates an uninitialized WebView.
   */
  WebView();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handel types must not contain data or virtual methods.
   */
  ~WebView();

  /*
   * @brief Copy constructor.
   *
   * @param[in] WebView WebView to copy. The copied WebView will point at the same implementation
   */
  WebView(const WebView& WebView);

  /**
   * @brief Assignment operator.
   *
   * @param[in] WebView The WebView to assign from
   * @return The updated WebView
   */
  WebView& operator=(const WebView& WebView);

  /**
   * @brief Downcasts a handle to WebView handle.
   *
   * If handle points to a WebView, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Handle to a WebView or an uninitialized handle
   */
  static WebView DownCast(BaseHandle handle);

  /**
   * @brief Get WebSettings of WebEngine.
   */
  Dali::Toolkit::WebSettings* GetSettings() const;

  /**
   * @brief Get WebContext of WebEngine.
   */
  Dali::Toolkit::WebContext* GetContext() const;

  /**
   * @brief Get CookieManager of WebEngine.
   */
  Dali::Toolkit::WebCookieManager* GetCookieManager() const;

  /**
   * @brief Get WebBackForwardList of WebEngine.
   */
  Dali::Toolkit::WebBackForwardList* GetBackForwardList() const;

  /**
   * @brief Get favicon of web page.
   *
   * @return Handle to a favicon
   */
  Dali::Toolkit::ImageView GetFavicon() const;

  /**
   * @brief Loads a web page based on a given URL.
   *
   * @param [in] url The URL of the resource to load
   */
  void LoadUrl(const std::string& url);

  /**
   * @brief Loads a given string as web contents.
   *
   * @param [in] htmlString The string to use as the contents of the web page
   */
  void LoadHtmlString(const std::string& htmlString);

  /**
   * @brief Reloads the Web.
   */
  void Reload();

  /**
   * @brief Stops loading web contents on the current page.
   */
  void StopLoading();

  /**
   * @brief Suspends the operation associated with the view.
   */
  void Suspend();

  /**
   * @brief Resumes the operation associated with the view object after calling Suspend().
   */
  void Resume();

  /**
   * @brief Scrolls the webpage of view by deltaX and deltaY.
   * @param[in] deltaX The delta x of scroll
   * @param[in] deltaY The delta y of scroll
   */
  void ScrollBy( int deltaX, int deltaY );

  /**
   * @brief Returns whether forward is possible.
   *
   * @return True if forward is possible, false otherwise
   */
  bool CanGoForward();

  /**
   * @brief Goes forward in the navigation history.
   */
  void GoForward();

  /**
   * @brief Returns whether backward is possible.
   *
   * @return True if backward is possible, false otherwise
   */
  bool CanGoBack();

  /**
   * @brief Goes back in the navigation history.
   */
  void GoBack();

  /**
   * @brief Evaluates JavaScript code represented as a string.
   *
   * @param[in] script The JavaScript code
   * @param[in] resultHandler The callback function to be called by the JavaScript runtime. This carries evaluation result.
   */
  void EvaluateJavaScript(const std::string& script, std::function<void(const std::string&)> resultHandler);

  /**
   * @brief Evaluates JavaScript code represented as a string.
   *
   * @param[in] script The JavaScript code
   */
  void EvaluateJavaScript(const std::string& script);

  /**
   * @brief Inject a JavaScript object with a message handler into the WebView.
   *
   * @note The injected object will appear in the JavaScript context to be loaded next.
   *
   * Example:
   *
   * 1. Native
   *
   *     webview.AddJavaScriptMessageHandler( "myObject", []( const std::string& message ) {
   *         printf( "Received a message from JS: %s", message.c_str() );
   *     });
   *
   *     // Start WebView by loading URL
   *     webview.LoadUrl( url );
   *
   * 2. JavaScript
   *
   *     myObject.postMessage( "Hello World!" ); // "Received a message from JS: Hello World!"
   *
   *
   * @param[in] exposedObjectName The name of exposed object
   * @param[in] handler The callback function
   */
  void AddJavaScriptMessageHandler(const std::string& exposedObjectName, std::function<void(const std::string&)> handler);

  /**
   * @brief Clears all tiles resources of Web.
   */
  void ClearAllTilesResources();

  /**
   * @brief Clears the history of Web.
   */
  void ClearHistory();

  /**
   * @brief Set or unset TTS focus of the webview.
   * @param[in] focused True if it is gained, false lost.
   * @note It only works when the webview does not have keyinput focus. If it has keyinput focus, the TTS focus is set automatically.
   */
  void SetTtsFocus(bool focused);

  /**
   * @brief Connects to this signal to be notified when page loading is started.
   *
   * @return A signal object to connect with
   */
  WebViewPageLoadSignalType& PageLoadStartedSignal();

  /**
   * @brief Connects to this signal to be notified when page loading is finished.
   *
   * @return A signal object to connect with
   */
  WebViewPageLoadSignalType& PageLoadFinishedSignal();

  /**
   * @brief Connects to this signal to be notified when an error occurs in page loading.
   *
   * @return A signal object to connect with.
   */
  WebViewPageLoadErrorSignalType& PageLoadErrorSignal();

  /**
   * @brief Connects to this signal to be notified when scroll edge is reached.
   *
   * @return A signal object to connect with.
   */
  WebViewScrollEdgeReachedSignalType& ScrollEdgeReachedSignal();

  /**
   * @brief Get a plain text of current web page asynchronously.
   *
   * @param[in] callback The callback function called asynchronously.
   */
  void GetPlainTextAsynchronously(Dali::WebEnginePlugin::PlainTextReceivedCallback callback);

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The WebView implementation
   */
  DALI_INTERNAL WebView(Internal::WebView& implementation);

  /**
   * @brief Allows the creation of this WebView from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL WebView(Dali::Internal::CustomActor* internal);
  /// @endcond
};

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_WEB_VIEW_H
