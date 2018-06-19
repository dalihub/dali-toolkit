#ifndef __DALI_TOOLKIT_WEB_VIEW_LITE_H__
#define __DALI_TOOLKIT_WEB_VIEW_LITE_H__

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
  class WebViewLite;
} // namespace Internal

class DALI_TOOLKIT_API WebViewLite: public Control
{
public:

  typedef Signal< void (WebViewLite&) > WebViewLiteSignalType;

public:

  /**
   * @brief Creates an initialized WebViewLite.
   *
   * @return A handle to a newly allocated Dali WebViewLite
   *
   * @note WebViewLite will not display anything
   */
  static WebViewLite New();

  /**
   * @brief Creates an uninitialized WebViewLite.
   */
  WebViewLite();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handel types must not contain data or virtual methods.
   */
  ~WebViewLite();

  /*
   * @brief Copy constructor.
   *
   * @param[in] webViewLite WebViewLite to copy. The copied WebViewLite will point at the same implementation
   */
  WebViewLite( const WebViewLite& webViewLite );

  /**
   * @brief Assignment operator.
   *
   * @param[in] webViewLite The WebViewLite to assign from
   * @return The updated WebViewLite
   */
  WebViewLite& operator=( const WebViewLite& webViewLite );

  /**
   * @brief Downcasts a handle to WebViewLite handle.
   *
   * If handle points to a WebViewLite, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Handle to a WebViewLite or an uninitialized handle
   */
  static WebViewLite DownCast( BaseHandle handle );

  /**
   * @brief Creates an internal web view instance (e.g. minimized web engine instance)
   *
   * @param [in] width The width of Web
   * @param [in] height The height of Web
   * @param [in] windowX The x position of window
   * @param [in] windowY The y position of window
   * @param [in] locale The locale of Web
   * @param [in] timezoneID The timezoneID of Web
   */
  void CreateInstance(int width, int height, int windowX, int windowY, const std::string& locale, const std::string& timezoneID);

  /**
   * @brief Destroys an internal web view instance (e.g. minimized web engine instance)
   */
  void DestroyInstance();

  /**
   * @brief Loads a html file
   *
   * @param [in] path The path of Web
   */
  void LoadHtml(const std::string& path);

  /**
   * @brief Connects to this signal to be notified when a web view is finished.
   *
   * @return A signal object to connect with
   */
  WebViewLiteSignalType& FinishedSignal();

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The WebViewLite implementation
   */
  DALI_INTERNAL WebViewLite( Internal::WebViewLite& implementation );

  /**
   * @brief Allows the creation of this WebViewLite from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  DALI_INTERNAL WebViewLite( Dali::Internal::CustomActor* internal );
  /// @endcond

};

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_WEB_VIEW_LITE_H__
