#ifndef DALI_TOOLKIT_WEB_SETTINGS_H
#define DALI_TOOLKIT_WEB_SETTINGS_H

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
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
class WebEngineSettings;

namespace Toolkit
{

/**
 * @addtogroup dali_toolkit_controls_web_view
 * @{
 */

/**
 * @brief WebEngineSettings is a control for settings of WebView.
 *
 *
 * For working WebEngineSettings, a WebView should be provided.
 *
 */
class DALI_TOOLKIT_API WebSettings
{
public:
  /**
   * @brief Creates a WebEngineSettings.
   *
   * @param[in] settings A settings of web engine.
   */
  WebSettings( Dali::WebEngineSettings& settings );

  /**
   * @brief Destructor.
   */
  virtual ~WebSettings() final;

  /**
   *@brief Allow running mixed contents or not.
   *
   * @param[in] allowed if true, allow to run mixed contents,
   *      otherwise not allow
   */
  void AllowMixedContents( bool allowed );

  /**
   * @brief Enable the spatial navigation or not.
   *
   * @param[in] enabled if true, use spatial navigation,
   *       otherwise to disable
   */
  void EnableSpatialNavigation( bool enabled );

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
   * @brief Enables/disables web security.
   *
   * @param[in] enabled if true, to enable the web security
   *       otherwise to disable
   */
  void EnableWebSecurity( bool enabled );

  /**
   * @brief Allow/Disallow file access from external url
   *
   * @param[in] allowed if true, to allow file access from external url
   *       otherwise to disallow
   */
  void AllowFileAccessFromExternalUrl( bool allowed );

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
   * @brief Allow if the scripts can open new windows.
   *
   * @param[in] allowed if true, the scripts can open new windows,
   *       otherwise not
   */
  void AllowScriptsOpenWindows( bool allowed );

  /**
   * @brief Returns whether images can be loaded automatically. The default is true.
   *
   * @return true if images are loaded automatically, false otherwise
   */
  bool AreImagesLoadedAutomatically() const;

  /**
   * @brief Enables/disables auto loading of images. The default is enabled.
   *
   * @param[in] automatic True if images are loaded automatically, false otherwise
   */
  void AllowImagesLoadAutomatically( bool automatic );

  /**
   * @brief Gets the default text encoding name (e.g. UTF-8).
   *
   * @return The default text encoding name
   */
  std::string GetDefaultTextEncodingName() const;

  /**
   * @brief Sets the default text encoding name (e.g. UTF-8).
   *
   * @param[in] defaultTextEncodingName The default text encoding name
   */
  void SetDefaultTextEncodingName( const std::string& defaultTextEncodingName );

private:
  Dali::WebEngineSettings& mWebEngineSettings;
};

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_WEB_SETTINGS_H
