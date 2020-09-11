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
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{
namespace Toolkit
{
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
   * @brief A structure used to contain the cache model enumeration.
   */
  struct CacheModel
  {
    /**
     * @brief Enumeration for cache model options.
     */
    enum Type
    {
      /**
       * @brief Use the smallest cache capacity.
       */
      DOCUMENT_VIEWER,

      /**
       * @brief Use the bigger cache capacity than DocumentBrowser.
       */
      DOCUMENT_BROWSER,

      /**
       * @brief Use the biggest cache capacity.
       */
      PRIMARY_WEB_BROWSER
    };
  };

  /**
   * @brief A structure used to contain the cookie acceptance policy enumeration.
   */
  struct CookieAcceptPolicy
  {
    /**
     * @brief Enumeration for the cookies accept policies.
     */
    enum Type
    {
      /**
       * @brief Accepts every cookie sent from any page.
       */
      ALWAYS,

      /**
       * @brief Rejects all the cookies.
       */
      NEVER,

      /**
       * @brief Accepts only cookies set by the main document that is loaded.
       */
      NO_THIRD_PARTY
    };
  };

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
       * @brief The cache model.
       * @details Name "cacheModel", type WebView::CacheModel::Type (Property::INTEGER) or Property::STRING.
       * @note Default is WebView::CacheModel::DOCUMENT_VIEWER.
       * @see WebView::CacheModel::Type
       */
      CACHE_MODEL,

      /**
       * @brief The cookie acceptance policy.
       * @details Name "cookieAcceptPolicy", type WebView::CookieAcceptPolicy::Type (Property::INTEGER) or Property::STRING.
       * @note Default is WebView::CookieAcceptPolicy::NO_THIRD_PARTY.
       * @see WebView::CookieAcceptPolicy::Type
       */
      COOKIE_ACCEPT_POLICY,

      /**
       * @brief The user agent string.
       * @details Name "userAgent", type Property::STRING.
       */
      USER_AGENT,

      /**
       * @brief Whether JavaScript is enabled.
       * @details Name "enableJavaScript", type Property::BOOLEAN.
       * @note Default is true.
       */
      ENABLE_JAVASCRIPT,

      /**
       * @brief Whether images can be loaded automatically.
       * @details Name "loadImagesAutomatically", type Property::BOOLEAN.
       * @note Default is true.
       */
      LOAD_IMAGES_AUTOMATICALLY,

      /**
       * @brief The default text encoding name.
       * @details Name "defaultTextEncodingName", type Property::STRING.
       * @note If the value is not set, the web engine detects web page's text encoding.
       */
      DEFAULT_TEXT_ENCODING_NAME,

      /**
       * @brief The default font size in pixel.
       * @details Name "defaultFontSize", type Property::INT.
       * @note Default is 16.
       */
      DEFAULT_FONT_SIZE
    };
  };

  /**
   * @brief Enumeration for indicating error code of page loading.
   */
  enum class LoadErrorCode
  {
    /**
     * @brief Unknown.
     */
    UNKNOWN = 0,

    /**
     * @brief User canceled.
     */
    CANCELED,

    /**
     * @brief Can't show the page for this MIME type.
     */
    CANT_SUPPORT_MIMETYPE,

    /**
     * @brief File IO error.
     */
    FAILED_FILE_IO,

    /**
     * @brief Cannot connect to the network.
     */
    CANT_CONNECT,

    /**
     * @brief Fail to look up host from the DNS.
     */
    CANT_LOOKUP_HOST,

    /**
     * @brief Fail to SSL/TLS handshake.
     */
    FAILED_TLS_HANDSHAKE,

    /**
     * @brief Received certificate is invalid.
     */
    INVALID_CERTIFICATE,

    /**
     * @brief Connection timeout.
     */
    REQUEST_TIMEOUT,

    /**
     * @brief Too many redirects.
     */
    TOO_MANY_REDIRECTS,

    /**
     * @brief Too many requests during this load.
     */
    TOO_MANY_REQUESTS,

    /**
     * @brief Malformed URL.
     */
    BAD_URL,

    /**
     * @brief Unsupported scheme.
     */
    UNSUPPORTED_SCHEME,

    /**
     * @brief User authentication failed on the server.
     */
    AUTHENTICATION,

    /**
     * @brief Web server has an internal server error.
     */
    INTERNAL_SERVER
  };

  /**
   * @brief WebView signal type related with page loading.
   */
  typedef Signal<void(WebView, const std::string&)> WebViewPageLoadSignalType;

  /**
   * @brief WebView signal type related with page loading error.
   */
  typedef Signal<void(WebView, const std::string&, LoadErrorCode)> WebViewPageLoadErrorSignalType;

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
  void LoadHTMLString(const std::string& htmlString);

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
   * @brief Clears the history of Web.
   */
  void ClearHistory();

  /**
   * @brief Clears the cache of Web.
   */
  void ClearCache();

  /**
   * @brief Clears all the cookies of Web.
   */
  void ClearCookies();

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
