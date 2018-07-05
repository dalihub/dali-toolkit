#ifndef DALI_TOOLKIT_WEB_VIEW_H
#define DALI_TOOLKIT_WEB_VIEW_H

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
} // namespace Internal

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
    PROPERTY_END_INDEX = PROPERTY_START_INDEX + 1000,
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the WebView class.
   */
  struct Property
  {
    enum
    {
      /**
       * @brief name "url", type string
       *
       * @details Sets the url to load
       */
      URL = PROPERTY_START_INDEX
    };
  };

  typedef Signal< void ( WebView, const std::string& ) > WebViewSignalType;

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
  static WebView New( const std::string& locale, const std::string& timezoneId );

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
  WebView( const WebView& WebView );

  /**
   * @brief Assignment operator.
   *
   * @param[in] WebView The WebView to assign from
   * @return The updated WebView
   */
  WebView& operator=( const WebView& WebView );

  /**
   * @brief Downcasts a handle to WebView handle.
   *
   * If handle points to a WebView, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Handle to a WebView or an uninitialized handle
   */
  static WebView DownCast( BaseHandle handle );

  /**
   * @brief Loads a web page based on a given URL.
   *
   * @param [in] url The URL of the resource to load
   */
  void LoadUrl( const std::string& url );

  /**
   * @brief Returns the URL of the Web.
   *
   * @return Url of string type
   */
  const std::string& GetUrl();

  /**
   * @brief Loads a given string as web contents.
   *
   * @param [in] htmlString The string to use as the contents of the web page
   */
  void LoadHTMLString( const std::string& htmlString );

  /**
   * @brief Reloads the Web.
   */
  void Reload();

  /**
   * @brief Stops loading web contents on the current page.
   */
  void StopLoading();

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
    */
  void EvaluateJavaScript( const std::string& script );

  /**
    * @brief Adds a JavaScript interface.
    *
    * @param[in] exposedObjectName The name of exposed object
    * @param[in] jsFunctionName The name of JavaScript function
    * @param[in] callback The callback function
    */
  void AddJavaScriptInterface( const std::string& exposedObjectName, const std::string& jsFunctionName, std::function< std::string(const std::string&) > callback );

  /**
    * @brief Removes a JavaScript interface.
    *
    * @param[in] exposedObjectName The name of exposed object
    * @param[in] jsFunctionName The name of JavaScript function
    */
  void RemoveJavascriptInterface( const std::string& exposedObjectName, const std::string& jsFunctionName );

  /**
   * @brief Clears the history of Web.
   */
  void ClearHistory();

  /**
   * @brief Clears the cache of Web.
   */
  void ClearCache();

  /**
   * @brief Connects to this signal to be notified when page loading is started.
   *
   * @return A signal object to connect with
   */
  WebViewSignalType& PageLoadStartedSignal();

  /**
   * @brief Connects to this signal to be notified when page loading is finished.
   *
   * @return A signal object to connect with
   */
  WebViewSignalType& PageLoadFinishedSignal();

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The WebView implementation
   */
  DALI_INTERNAL WebView( Internal::WebView& implementation );

  /**
   * @brief Allows the creation of this WebView from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL WebView( Dali::Internal::CustomActor* internal );
  /// @endcond

};

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_WEB_VIEW_H
