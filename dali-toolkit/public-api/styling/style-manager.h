#ifndef __DALI_TOOLKIT_STYLE_MANAGER_H__
#define __DALI_TOOLKIT_STYLE_MANAGER_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/style-change.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class StyleManager;
}

/**
 * @addtogroup dali_toolkit_managers
 * @{
 */

/**
 * @brief StyleManager informs applications of system theme change,
 * and supports application theme change at runtime.
 *
 * Applies various styles to Controls using the properties system.
 *
 * On theme change, it automatically updates all controls, then raises
 * a signal to inform the application.
 *
 * The default theme is automatically loaded and applied, followed by
 * any application specific theme defined in Application::New().
 *
 * If the application wants to customize the theme, RequestThemeChange
 * needs to be called.
 *
 * Signals
 * | %Signal Name            | Method                           |
 * |------------------------------------------------------------|
 * | styleChanged            | @ref StyleChangedSignal()        |
 * @SINCE_1_1.32
 */
class DALI_IMPORT_API StyleManager : public BaseHandle
{
public:

  /// @brief Style Changed signal. Emitted after controls have been updated
  typedef Signal< void ( StyleManager, StyleChange::Type ) >  StyleChangedSignalType;

  /**
   * @brief Create a StyleManager handle; this can be initialised with StyleManager::Get()
   *
   * Calling member functions with an uninitialised handle is not allowed.
   * @SINCE_1_1.32
   */
  StyleManager();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_1.32
   */
  ~StyleManager();

  /**
   * @brief Get the singleton of StyleManager object.
   *
   * @SINCE_1_1.32
   * @return A handle to the StyleManager control.
   */
  static StyleManager Get();

  /**
   * @brief Apply a new theme to the application. This will be merged
   * on top of the default Toolkit theme.
   *
   * If the application theme file doesn't style all controls that the
   * application uses, then the default Toolkit theme will be used
   * instead for those controls.
   *
   * On application startup, it is suggested that the theme file name is
   * passed to Application::New instead of using this API to prevent
   * controls being styled more than once.
   * @sa Application::New()
   *
   * @SINCE_1_1.32
   * @param[in] themeFile If a relative path is specified, then this is relative
   * to the directory returned by app_get_resource_path().
   */
  void ApplyTheme( const std::string& themeFile );

  /**
   * @brief Apply the default Toolkit theme.
   *
   * Request that any application specific styling is removed
   * and that the default Toolkit theme is re-applied.
   *
   * @SINCE_1_1.32
   */
  void ApplyDefaultTheme();

  /**
   * @brief Set a constant for use when building styles
   *
   * A constant is used in JSON files e.g. "myImage":"{RELATIVE_PATH}/image.jpg"
   * where the string "{RELATIVE_PATH}" is substituted with the value.
   *
   * @SINCE_1_1.32
   * @param[in] key The key of the constant
   * @param[in] value The value of the constant
   */
  void SetStyleConstant( const std::string& key, const Property::Value& value );

  /**
   * @brief Return the style constant set for a specific key
   *
   * @SINCE_1_1.32
   * @param[in] key The key of the constant
   * @param[out] valueOut The value of the constant if it exists
   *
   * @return If the constant for key exists then return the constant in valueOut and return true
   */
  bool GetStyleConstant( const std::string& key, Property::Value& valueOut );

  /**
   * @brief Apply the specified style to the control.
   *
   * @SINCE_1_1.32
   * @param[in] control The control to which to apply the style.
   * @param[in] jsonFileName The name of the JSON style file to apply. If a
   * relative path is specified, then this is relative to the directory
   * returned by app_get_resource_path().
   * @param[in] styleName The name of the style within the JSON file to apply.
   */
  void ApplyStyle( Toolkit::Control control, const std::string& jsonFileName, const std::string& styleName );

public: // Signals

  /**
   * @brief This signal is emitted after the style (e.g. theme/font change) has changed
   * and the controls have been informed.
   *
   * @SINCE_1_1.32
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( StyleManager styleManager, StyleChange change );
   * @endcode
   * @return The signal to connect to.
   */
  StyleChangedSignalType& StyleChangedSignal();

public:

  /**
   * @brief Allows the creation of a StyleManager handle from an internal pointer.
   *
   * @note Not intended for application developers
   * @SINCE_1_1.32
   * @param[in] impl A pointer to the object.
   */
  explicit DALI_INTERNAL StyleManager( Internal::StyleManager *impl );

}; // class StyleManager

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali


#endif /* __DALI_TOOLKIT_STYLE_MANAGER_H__ */
