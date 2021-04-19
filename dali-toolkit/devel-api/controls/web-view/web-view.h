#ifndef DALI_TOOLKIT_WEB_VIEW_H
#define DALI_TOOLKIT_WEB_VIEW_H

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

// EXTERNAL INCLUDES
#include <functional>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali/devel-api/adaptor-framework/web-engine-plugin.h>

namespace Dali
{
namespace Toolkit
{
class ImageView;
class WebBackForwardList;
class WebContext;
class WebContextMenu;
class WebContextMenuItem;
class WebCookieManager;
class WebFormRepostDecision;
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

      /**
       * @brief Whether mouse event is enabled.
       * @details name "mouseEventsEnabled", type Property::BOOLEAN.
       * @note Default is true.
       */
      MOUSE_EVENTS_ENABLED,

      /**
       * @brief Whether key event is enabled.
       * @details name "keyEventsEnabled", type Property::BOOLEAN.
       * @note Default is true.
       */
      KEY_EVENTS_ENABLED,

      /**
       * @brief The background color of web page.
       * @details name "documentBackgroundColor", type Property::VECTOR4.
       */
      DOCUMENT_BACKGROUND_COLOR,

      /**
       * @brief Whether tiles can be cleared or not when hidden.
       * @details name "tilesClearedWhenHidden", type BOOLEAN.
       */
      TILES_CLEARED_WHEN_HIDDEN,

      /**
       * @brief The multiplier of cover area of tile when rendering web page.
       * @details name "tileCoverAreaMultiplier", type FLOAT.
       */
      TILE_COVER_AREA_MULTIPLIER,

      /**
       * @brief Whether cursor is enabled or not by client.
       * @details name "cursorEnabledByClient", type BOOLEAN.
       */
      CURSOR_ENABLED_BY_CLIENT,

      /**
       * @brief The selected text of web page.
       * @details name "selectedText", type Property::STRING.
       * @note The value is read-only.
       */
      SELECTED_TEXT,

      /**
       * @brief Zoom factor of web page.
       * @details name "pageZoomFactor", type Property::FLOAT.
       */
      PAGE_ZOOM_FACTOR,

      /**
       * @brief Zoom factor of text.
       * @details name "textZoomFactor", type Property::FLOAT.
       */
      TEXT_ZOOM_FACTOR,

      /**
       * @brief progress percentage of loading a web page.
       * @details name "loadProgressPercentage", type Property::FLOAT.
       * @note The value is read-only.
       */
      LOAD_PROGRESS_PERCENTAGE,
    };
  };

  /**
   * @brief WebView callback related with screen-shot captured.
   */
  using WebViewScreenshotCapturedCallback = std::function<void(Dali::Toolkit::ImageView)>;

  /**
   * @brief WebView signal type related with page loading.
   */
  using WebViewPageLoadSignalType = Signal<void(WebView, const std::string&)>;

  /**
   * @brief WebView signal type related with page loading error.
   */
  using WebViewPageLoadErrorSignalType = Signal<void(WebView, std::shared_ptr<Dali::WebEngineLoadError>)>;

  /**
   * @brief WebView signal type related with scroll edge reached.
   */
  using WebViewScrollEdgeReachedSignalType = Signal<void(WebView, Dali::WebEnginePlugin::ScrollEdge)>;

  /**
   * @brief WebView signal type related with url changed.
   */
  using WebViewUrlChangedSignalType = Signal<void(WebView, const std::string&)>;

  /**
   * @brief WebView signal type related with form repost decision.
   */
  using WebViewFormRepostDecisionSignalType = Signal<void(WebView, std::shared_ptr<Dali::WebEngineFormRepostDecision>)>;

  /**
   * @brief WebView signal type related with frame rendered.
   */
  using WebViewFrameRenderedSignalType = Signal<void(WebView)>;

  /**
   * @brief WebView signal type related with http request interceptor.
   */
  using WebViewRequestInterceptorSignalType = Signal<void(WebView, std::shared_ptr<Dali::WebEngineRequestInterceptor>)>;

  /**
   * @brief WebView signal type related with console message.
   */
  using WebViewConsoleMessageSignalType = Signal<void(WebView, std::shared_ptr<Dali::WebEngineConsoleMessage>)>;

  /**
   * @brief WebView signal type related with policy decision.
   */
  using WebViewPolicyDecisionSignalType = Signal<void(WebView, std::shared_ptr<Dali::WebEnginePolicyDecision>)>;

  /**
   * @brief WebView signal type related with certificate changed.
   */
  using WebViewCertificateSignalType = Signal<void(WebView, std::shared_ptr<Dali::WebEngineCertificate>)>;

  /**
   * @brief WebView signal type related with http authentication.
   */
  using WebViewHttpAuthHandlerSignalType = Signal<void(WebView, std::shared_ptr<Dali::WebEngineHttpAuthHandler>)>;

  /**
   * @brief WebView signal type related with context menu customized.
   */
  using WebViewContextMenuCustomizedSignalType = Signal<void(WebView, std::shared_ptr<Dali::WebEngineContextMenu>)>;

  /**
   * @brief WebView signal type related with context menu item selected.
   */
  using WebViewContextMenuItemSelectedSignalType = Signal<void(WebView, std::shared_ptr<Dali::WebEngineContextMenuItem>)>;

public:
  /**
   * @brief Create an initialized WebView.
   * @return A handle to a newly allocated Dali WebView
   *
   * @note WebView will not display anything
   */
  static WebView New();

  /**
   * @brief Create an initialized WebView.
   *
   * @param [in] locale The locale of Web
   * @param [in] timezoneId The timezoneId of Web
   */
  static WebView New(const std::string& locale, const std::string& timezoneId);

  /**
   * @brief Create an initialized WebView.
   *
   * @param [in] argc The count of arguments of Applications
   * @param [in] argv The string array of arguments of Applications
   */
  static WebView New(uint32_t argc, char** argv);

  /**
   * @brief Create an uninitialized WebView.
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
   * @brief Downcast a handle to WebView handle.
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
   * @brief Get Favicon of web page.
   *
   * @return Handle to a fav icon
   */
  Dali::Toolkit::ImageView& GetFavicon();

  /**
   * @brief Load a web page based on a given URL.
   *
   * @param [in] url The URL of the resource to load
   */
  void LoadUrl(const std::string& url);

  /**
   * @brief Load a given string as web contents.
   *
   * @param [in] htmlString The string to use as the contents of the web page
   */
  void LoadHtmlString(const std::string& htmlString);

  /**
   * @brief Load the specified html string as the content of the view overriding current history entry
   *
   * @param[in] html HTML data to load
   * @param[in] basicUri Base URL used for relative paths to external objects
   * @param[in] unreachableUrl URL that could not be reached
   *
   * @return true if successfully loaded, false otherwise
   */
  bool LoadHtmlStringOverrideCurrentEntry(const std::string& html, const std::string& basicUri, const std::string& unreachableUrl);

  /**
   * @brief Request loading the given contents by MIME type into the view object
   *
   * @param[in] contents The content to load
   * @param[in] contentSize The size of contents (in bytes)
   * @param[in] mimeType The type of contents, if 0 is given "text/html" is assumed
   * @param[in] encoding The encoding for contents, if 0 is given "UTF-8" is assumed
   * @param[in] baseUri The base URI to use for relative resources
   *
   * @return true if successfully request, false otherwise
   */
  bool LoadContents(const std::string& contents, uint32_t contentSize, const std::string& mimeType, const std::string& encoding, const std::string& baseUri);

  /**
   * @brief Reload the Web.
   */
  void Reload();

  /**
   * @brief Reload the current page's document without cache
   */
  bool ReloadWithoutCache();

  /**
   * @brief Stop loading web contents on the current page.
   */
  void StopLoading();

  /**
   * @brief Suspend the operation associated with the view.
   */
  void Suspend();

  /**
   * @brief Resume the operation associated with the view object after calling Suspend().
   */
  void Resume();

  /**
   * @brief To suspend all url loading
   */
  void SuspendNetworkLoading();

  /**
   * @brief To resume new url network loading
   */
  void ResumeNetworkLoading();

  /**
   * @brief Add custom header
   *
   * @param[in] name custom header name to add the custom header
   * @param[in] value custom header value to add the custom header
   *
   * @return true if succeeded, false otherwise
   */
  bool AddCustomHeader(const std::string& name, const std::string& value);

  /**
   * @brief Remove custom header
   *
   * @param[in] name custom header name to remove the custom header
   *
   * @return true if succeeded, false otherwise
   */
  bool RemoveCustomHeader(const std::string& name);

  /**
   * @brief Start the inspector server
   *
   * @param[in] port port number
   *
   * @return the port number
   */
  uint32_t StartInspectorServer(uint32_t port);

  /**
   * @brief Stop the inspector server
   *
   * @return true if succeeded, false otherwise
   */
  bool StopInspectorServer();

  /**
   * @brief Scroll web page of view by deltaX and deltaY.
   * @param[in] deltaX The delta x of scroll
   * @param[in] deltaY The delta y of scroll
   */
  void ScrollBy(int32_t deltaX, int32_t deltaY);

  /**
   * @brief Scroll edge of view by deltaX and deltaY.
   *
   * @param[in] deltaX horizontal offset to scroll
   * @param[in] deltaY vertical offset to scroll
   *
   * @return true if succeeded, false otherwise
   */
  bool ScrollEdgeBy(int32_t deltaX, int32_t deltaY);

  /**
   * @brief Return whether forward is possible.
   *
   * @return True if forward is possible, false otherwise
   */
  bool CanGoForward();

  /**
   * @brief Go forward in the navigation history.
   */
  void GoForward();

  /**
   * @brief Return whether backward is possible.
   *
   * @return True if backward is possible, false otherwise
   */
  bool CanGoBack();

  /**
   * @brief Go back in the navigation history.
   */
  void GoBack();

  /**
   * @brief Evaluate JavaScript code represented as a string.
   *
   * @param[in] script The JavaScript code
   * @param[in] resultHandler The callback function to be called by the JavaScript runtime. This carries evaluation result
   */
  void EvaluateJavaScript(const std::string& script, std::function<void(const std::string&)> resultHandler);

  /**
   * @brief Evaluate JavaScript code represented as a string.
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
   * @brief Register alert callback for javascript.
   *
   * @param[in] callback The callback function to be called by the JavaScript runtime.
   */
  void RegisterJavaScriptAlertCallback(Dali::WebEnginePlugin::JavaScriptAlertCallback callback);

  /**
   * @brief Reply for JavaScript alert.
   */
  void JavaScriptAlertReply();

  /**
   * @brief Register confirm callback for javascript.
   *
   * @param[in] callback The callback function to be called by the JavaScript runtime.
   */
  void RegisterJavaScriptConfirmCallback(Dali::WebEnginePlugin::JavaScriptConfirmCallback callback);

  /**
   * @brief Reply for JavaScript confirm.
   * @param[in] confirmed True if confirmed, false otherwise
   */
  void JavaScriptConfirmReply(bool confirmed);

  /**
   * @brief Register prompt callback for javascript.
   *
   * @param[in] callback The callback function to be called by the JavaScript runtime.
   */
  void RegisterJavaScriptPromptCallback(Dali::WebEnginePlugin::JavaScriptPromptCallback callback);

  /**
   * @brief Reply for JavaScript prompt.
   * @param[in] result The result from input-field of prompt popup.
   */
  void JavaScriptPromptReply(const std::string& result);

  /**
   * @brief Create a new hit test.
   *
   * @param[in] x the horizontal position to query
   * @param[in] y the vertical position to query
   * @param[in] mode the mode of hit test
   *
   * @return a new hit test object.
   */
  std::unique_ptr<Dali::WebEngineHitTest> CreateHitTest(int32_t x, int32_t y, Dali::WebEngineHitTest::HitTestMode mode);

  /**
   * @brief Create a hit test asynchronously.
   *
   * @param[in] x the horizontal position to query
   * @param[in] y the vertical position to query
   * @param[in] mode the mode of hit test
   * @param[in] callback The callback function
   *
   * @return true if succeeded, false otherwise.
   */
  bool CreateHitTestAsynchronously(int32_t x, int32_t y, Dali::WebEngineHitTest::HitTestMode mode, Dali::WebEnginePlugin::WebEngineHitTestCreatedCallback callback);

  /**
   * @brief Clear the history of Web.
   */
  void ClearHistory();

  /**
   * @brief Clear all tiles resources of Web.
   */
  void ClearAllTilesResources();

  /**
   * @brief Scale the current page, centered at the given point.
   * @param[in] scaleFactor a new factor to be scaled.
   * @param[in] point a center coordinate.
   */
  void SetScaleFactor(float scaleFactor, Dali::Vector2 point);

  /**
   * @brief Get the current scale factor of the page.
   * @return The current scale factor.
   */
  float GetScaleFactor() const;

  /**
   * @brief Request to activate/deactivate the accessibility usage set by web app.
   * @param[in] activated Activate accessibility or not.
   */
  void ActivateAccessibility(bool activated);

  /**
   * @brief Search and highlights the given string in the document.
   * @param[in] text The text to find
   * @param[in] options The options to find
   * @param[in] maxMatchCount The maximum match count to find
   *
   * @return true if found & highlighted, false otherwise
   */
  bool HighlightText(const std::string& text, Dali::WebEnginePlugin::FindOption options, uint32_t maxMatchCount);

  /**
   * @brief Add dynamic certificate path.
   * @param[in] host host that required client authentication
   * @param[in] certPath the file path stored certificate
   */
  void AddDynamicCertificatePath(const std::string& host, const std::string& certPath);

  /**
   * @brief Get snapshot of the specified viewArea of page.
   *
   * @param[in] viewArea The rectangle of screen shot
   * @param[in] scaleFactor The scale factor
   *
   * @return image view of screen shot
   */
  Dali::Toolkit::ImageView GetScreenshot(Dali::Rect<int32_t> viewArea, float scaleFactor);

  /**
   * @brief Request to get snapshot of the specified viewArea of page asynchronously.
   *
   * @param[in] viewArea The rectangle of screen shot
   * @param[in] scaleFactor The scale factor
   * @param[in] callback The callback for screen shot
   *
   * @return true if requested successfully, false otherwise
   */
  bool GetScreenshotAsynchronously(Dali::Rect<int32_t> viewArea, float scaleFactor, WebViewScreenshotCapturedCallback callback);

  /**
   * @brief Asynchronous request to check if there is a video playing in the given view.
   *
   * @param[in] callback The callback called after checking if video is playing or not
   *
   * @return true if requested successfully, false otherwise
   */
  bool CheckVideoPlayingAsynchronously(Dali::WebEnginePlugin::VideoPlayingCallback callback);

  /**
   * @brief Set callback which will be called upon geolocation permission request.
   *
   * @param[in] callback The callback for requesting geolocation permission
   */
  void RegisterGeolocationPermissionCallback(Dali::WebEnginePlugin::GeolocationPermissionCallback callback);

  /**
   * @brief Connect to this signal to be notified when page loading is started.
   *
   * @return A signal object to connect with
   */
  WebViewPageLoadSignalType& PageLoadStartedSignal();

  /**
   * @brief Connect to this signal to be notified when page loading is in progress.
   *
   * @return A signal object to connect with
   */
  WebViewPageLoadSignalType& PageLoadInProgressSignal();

  /**
   * @brief Connect to this signal to be notified when page loading is finished.
   *
   * @return A signal object to connect with
   */
  WebViewPageLoadSignalType& PageLoadFinishedSignal();

  /**
   * @brief Connect to this signal to be notified when an error occurs in page loading.
   *
   * @return A signal object to connect with
   */
  WebViewPageLoadErrorSignalType& PageLoadErrorSignal();

  /**
   * @brief Connect to this signal to be notified when scroll edge is reached.
   *
   * @return A signal object to connect with
   */
  WebViewScrollEdgeReachedSignalType& ScrollEdgeReachedSignal();

  /**
   * @brief Connect to this signal to be notified when url is changed.
   *
   * @return A signal object to connect with
   */
  WebViewUrlChangedSignalType& UrlChangedSignal();

  /**
   * @brief Connect to this signal to be notified when form repost decision is requested.
   *
   * @return A signal object to connect with.
   */
  WebViewFormRepostDecisionSignalType& FormRepostDecisionSignal();

  /**
   * @brief Connect to this signal to be notified when frame is rendered.
   *
   * @return A signal object to connect with.
   */
  WebViewFrameRenderedSignalType& FrameRenderedSignal();

  /**
   * @brief Connect to this signal to be notified when http request need be intercepted.
   *
   * @return A signal object to connect with.
   */
  WebViewRequestInterceptorSignalType& RequestInterceptorSignal();

  /**
   * @brief Connect to this signal to be notified when console message will be logged.
   *
   * @return A signal object to connect with.
   */
  WebViewConsoleMessageSignalType& ConsoleMessageSignal();

  /**
   * @brief Connect to this signal to be notified when new policy would be decided.
   *
   * @return A signal object to connect with.
   */
  WebViewPolicyDecisionSignalType& PolicyDecisionSignal();

  /**
   * @brief Connect to this signal to be notified when certificate need be confirmed.
   *
   * @return A signal object to connect with.
   */
  WebViewCertificateSignalType& CertificateConfirmSignal();

  /**
   * @brief Connect to this signal to be notified when ssl certificate is changed.
   *
   * @return A signal object to connect with.
   */
  WebViewCertificateSignalType& SslCertificateChangedSignal();

  /**
   * @brief Connect to this signal to be notified when http authentication need be confirmed.
   *
   * @return A signal object to connect with.
   */
  WebViewHttpAuthHandlerSignalType& HttpAuthHandlerSignal();

  /**
   * @brief Connect to this signal to be notified when context menu would be customized.
   *
   * @return A signal object to connect with.
   */
  WebViewContextMenuCustomizedSignalType& ContextMenuCustomizedSignal();

  /**
   * @brief Connect to this signal to be notified when context menu item is selected.
   *
   * @return A signal object to connect with.
   */
  WebViewContextMenuItemSelectedSignalType& ContextMenuItemSelectedSignal();

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Create a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The WebView implementation
   */
  DALI_INTERNAL WebView(Internal::WebView& implementation);

  /**
   * @brief Allow the creation of this WebView from an Internal::CustomActor pointer.
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
