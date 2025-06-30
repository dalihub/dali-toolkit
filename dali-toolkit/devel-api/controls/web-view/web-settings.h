#ifndef DALI_TOOLKIT_WEB_SETTINGS_H
#define DALI_TOOLKIT_WEB_SETTINGS_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
  WebSettings(Dali::WebEngineSettings& settings);

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
  void AllowMixedContents(bool allowed);

  /**
   * @brief Enable the spatial navigation or not.
   *
   * @param[in] enabled if true, use spatial navigation,
   *       otherwise to disable
   */
  void EnableSpatialNavigation(bool enabled);

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
  void SetDefaultFontSize(int defaultFontSize);

  /**
   * @brief Enables/disables web security.
   *
   * @param[in] enabled if true, to enable the web security
   *       otherwise to disable
   */
  void EnableWebSecurity(bool enabled);

  /**
   * @brief Enables/disables cache builder.
   *
   * @param[in] enabled if true, to enable the cache builder
   *       otherwise to disable
   */
  void EnableCacheBuilder( bool enabled );

  /**
   * @brief Used/Unused uses scrollbar thumb focus notifications. The default is used.
   *
   * @param[in] used True if uses scrollbar thumb focus notifications, false otherwise
   */
  void UseScrollbarThumbFocusNotifications ( bool used );

  /**
   * @brief Enable/disables do not track executing.
   *
   * @param[in] enabled if true, to enable do not track
   *        otherwise to disable
   */
  void EnableDoNotTrack( bool enabled );

  /**
   * @brief Allow/Disallow file access from external url
   *
   * @param[in] allowed if true, to allow file access from external url
   *       otherwise to disallow
   */
  void AllowFileAccessFromExternalUrl(bool allowed);

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
  void EnableJavaScript(bool enabled);

  /**
   * @brief Returns whether auto fitting can be executable. The default is true.
   *
   * @return true if auto fitting executing is enabled, false otherwise
   */
  bool IsAutoFittingEnabled() const;

   /**
   * @brief Enables/disables auto fitting executing. The default is enabled.
   *
   * @param[in] enabled True if auto fitting executing is enabled, false otherwise
   */
  void EnableAutoFitting( bool enabled );

  /**
   * @brief Returns whether plugins can be executable. The default is true.
   *
   * @return true if plugins executing is enabled, false otherwise
   */
  bool ArePluginsEnabled() const;

   /**
   * @brief Enables/disables Plugins executing. The default is enabled.
   *
   * @param[in] enabled True if Plugins executing is enabled, false otherwise
   */
  void EnablePlugins( bool enabled );

  /**
   * @brief Returns whether private browsing can be executable. The default is true.
   *
   * @return true if private browsing executing is enabled, false otherwise
   */
  bool IsPrivateBrowsingEnabled() const;

  /**
   * @brief Enables/disables private browsing executing. The default is enabled.
   *
   * @param[in] enabled True if private browsing executing is enabled, false otherwise
   */
  void EnablePrivateBrowsing( bool enabled );

  /**
   * @brief Returns whether link magnifier can be executable. The default is true.
   *
   * @return true if link magnifier executing is enabled, false otherwise
   */
  bool IsLinkMagnifierEnabled() const;

  /**
   * @brief Enables/disables link magnifier executing. The default is enabled.
   *
   * @param[in] enabled True if link magnifier executing is enabled, false otherwise
   */
  void EnableLinkMagnifier( bool enabled );

  /**
   * @brief Returns whether uses keypad without user action can be executable. The default is true.
   *
   * @return true if keypad without user action executing is used, false otherwise
   */
  bool IsKeypadWithoutUserActionUsed() const;

  /**
   * @brief Uses/Unused keypad without user action executing. The default is used.
   *
   * @param[in] used True if keypad without user action executing is used, false otherwise
   */
  void UseKeypadWithoutUserAction( bool used );

  /**
   * @brief Returns whether autofill password form can be executable. The default is true.
   *
   * @return true if autofill password form executing is enabled, false otherwise
   */
  bool IsAutofillPasswordFormEnabled() const;

  /**
   * @brief Enables/disables autofill password form executing. The default is enabled.
   *
   * @param[in] enabled True if autofill password form executing is enabled, false otherwise
   */
  void EnableAutofillPasswordForm( bool enabled );

  /**
   * @brief Returns whether form candidate data can be executable. The default is true.
   *
   * @return true if form candidate data executing is enabled, false otherwise
   */
  bool IsFormCandidateDataEnabled() const;

  /**
   * @brief Enables/disables form candidate data executing. The default is enabled.
   *
   * @param[in] enabled True if form candidate data executing is enabled, false otherwise
   */
  void EnableFormCandidateData( bool enabled );

  /**
   * @brief Returns whether text selection can be executable. The default is true.
   *
   * @return true if text selection executing is enabled, false otherwise
   */
  bool IsTextSelectionEnabled() const;

  /**
   * brief Enables/disables text selection executing. The default is enabled.
   *
   * @param[in] enabled True if text selection executing is enabled, false otherwise
   */
  void EnableTextSelection( bool enabled );

  /**
   * @brief Returns whether text autosizing can be executable. The default is true.
   *
   * @return true if text autosizing executing is enabled, false otherwise
   */
  bool IsTextAutosizingEnabled() const;

  /**
   * @brief Enables/disables text autosizing executing. The default is enabled.
   *
   * @param[in] enabled True if text autosizing executing is enabled, false otherwise
   */
  void EnableTextAutosizing( bool enabled );

  /**
   * @brief Returns whether arrow scroll can be executable. The default is true.
   *
   * @return true if arrow scroll executing is enabled, false otherwise
   */
  bool IsArrowScrollEnabled() const;

  /**
   * @brief Enables/disables arrow scroll executing. The default is enabled.
   *
   * @param[in] enabled True if arrow scroll executing is enabled, false otherwise
   */
  void EnableArrowScroll( bool enabled );

  /**
   * @brief Returns whether clipboard can be executable. The default is true.
   *
   * @return true if clipboard executing is enabled, false otherwise
   */
  bool IsClipboardEnabled() const;

  /**
   * @brief Enables/disables clipboard executing. The default is enabled.
   *
   * @param[in] enabled True if clipboard is enabled, false otherwise
   */
  void EnableClipboard( bool enabled );

  /**
   * @brief Returns whether ime panel can be executable. The default is true.
   *
   * @return true if ime panel executing is enabled, false otherwise
   */
  bool IsImePanelEnabled() const;

  /**
   * @brief Enables/disables ime panel executing. The default is enabled.
   *
   * @param[in] enabled True if ime panel executing is enabled, false otherwise
   */
  void EnableImePanel( bool enabled );

  /**
   * @brief Allow if the scripts can open new windows.
   *
   * @param[in] allowed if true, the scripts can open new windows,
   *       otherwise not
   */
  void AllowScriptsOpenWindows(bool allowed);

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
  void AllowImagesLoadAutomatically(bool automatic);

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
  void SetDefaultTextEncodingName(const std::string& defaultTextEncodingName);

  /**
   * @brief Enables/disables the viewport meta tag.
   *
   * By default, the viewport meta tag is enabled on mobile and wearable,
   * but it is disabled on TV.
   *
   * @param[in] enable @c true to enable the viewport meta tag
   *               @c false to disable
   *
   * @return @c true on success or @c false on failure
   */
  bool SetViewportMetaTag(bool enable);

  /**
   * @brief Requests setting of force zoom.
   *
   * @param[in] enable to force zoom
   *
   * @return @c true on success or @c false on failure
   */
  bool SetForceZoom(bool enable);

  /**
   * @brief Returns the force zoom status.
   *
   * @return @c true if enable force zoom or @c false.
   */
  bool IsZoomForced() const;

  /**
   * @brief Requests setting use of text zoom.
   *
   * @param[in] enable to text zoom.
   *
   * @return @c true on success or @c false on failure
   */
  bool SetTextZoomEnabled(bool enable);

  /**
   * @brief Returns whether text zoom is enabled or not.
   *
   * @return @c true if enable text zoom or @c false.
   */
  bool IsTextZoomEnabled() const;

  /**
   * @brief Requests enables/disables to the specific extra feature
   *
   * @param[in] feature feature name
   * @param[in] enable @c true to enable the specific extra feature
   *        @c false to disable
   */
  void SetExtraFeature(const std::string& feature, bool enable);

  /**
   * @brief Returns enable/disable to the specific extra feature
   *
   * @param[in] feature feature name
   *
   * @return @c true on enable or @c false on disable
   */
  bool IsExtraFeatureEnabled(const std::string& feature) const;

  /**
   * @brief Sets the style of IME.
   *
   * @param[in] style @c IME_STYLE_FULL       full IME style
   *                  @c IME_STYLE_FLOATING   floating IME style
   *                  @c IME_STYLE_DYNAMIC    dynamic IME style
   *
   */
  void SetImeStyle(int style);

  /**
   * @brief Gets the style of IME.
   *
   * @return @c IME_STYLE_FULL       full IME style
   *         @c IME_STYLE_FLOATING   floating IME style
   *         @c IME_STYLE_DYNAMIC    dynamic IME style
   */
  int GetImeStyle() const;

  /**
   * @brief Sets default audio input device
   *
   * @param[in] deviceId default device ID
   */
  void SetDefaultAudioInputDevice(const std::string& deviceId) const;

  /**
   * @brief Enables Drag&Drop or not
   *
   * @param[in] enable if true, to enable Drag&Drop
   *       otherwise to disable
   */
  void EnableDragAndDrop(bool enable);

private:
  Dali::WebEngineSettings& mWebEngineSettings;
};

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_WEB_SETTINGS_H
