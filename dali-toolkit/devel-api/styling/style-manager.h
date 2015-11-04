#ifndef __DALI_TOOLKIT_STYLE_MANAGER_H__
#define __DALI_TOOLKIT_STYLE_MANAGER_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/orientation.h>
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
 * @brief StyleManager provides the following functionalities:
 *
 * Applies various styles to Controls using the properties system.
 * On theme change a signal is raised that controls can be configured to listen to.
 *
 * The default theme is automatically loaded and applied.
 *
 * If the application wants to customize the theme, RequestThemeChange needs to be called.
 * Also, the default orientation is Portrait, if the application wants to adapt to the orientation change, call SetOrientation or SetOrienationValue.
 * @code
 *   const char* CUSTOM_THEME = DALI_SCRIPT_DIR "tizen-dark-theme.json";
 *
 *   void OnInit(Application& app)
 *   {
 *      Toolkit::StyleManager::Get().RequestThemeChange( CUSTOM_THEME );
 *      Toolkit::StyleManager::Get().SetOrientation( ... );
 *      ...
 *   }
 * @endcode
 *
 * Internal::Control can be configured to register for the signals that are required from StyleManager,
 * such as theme change.
 */
class DALI_IMPORT_API StyleManager : public BaseHandle
{
public:

  // Signals
  typedef Signal< void ( StyleManager, StyleChange::Type ) >  StyleChangeSignalType;

  /**
   * @brief Create a StyleManager handle; this can be initialised with StyleManager::Get()
   * Calling member functions with an uninitialised handle is not allowed.
   */
  StyleManager();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~StyleManager();

  /**
   * @brief Get the singleton of StyleManager object.
   *
   * @return A handle to the StyleManager control.
   */
  static StyleManager Get();

  /**
   * @brief Specify the orientation value directly for the style manager
   *
   * @param[in] orientation The orientation in degrees
   */
  void SetOrientationValue( int orientation );

  /**
   * @brief Return the orientation value
   *
   * @return The orientation value
   */
  int GetOrientationValue();

  /**
   * @brief Set the orientation object. This will take precedence over setting the orientation value.
   *
   * @param[in] orientation Device orientation
   */
  void SetOrientation( Orientation orientation );

  /**
   * @brief Return the orientation object
   *
   * @return The orientation.
   */
  Orientation GetOrientation();

  /**
   * @brief Retrieves the default font family.
   * @return The default font family.
   */
  std::string GetDefaultFontFamily() const;

  /**
   * @brief Make a request to set the theme JSON file to one that exists in the Toolkit package.
   *
   * Multiple requests per event processing cycle can be made, but only the final one will be acted
   * on in the event processing finished callback.
   *
   * @param[in] themeFile This is just the JSON theme file name and not the full path.
   */
  void RequestThemeChange( const std::string& themeFile );

  /**
   * @brief Request a change to the default theme
   */
  void RequestDefaultTheme();

  /**
   * @brief Set a constant for use when building styles
   *
   * A constant is used in JSON files e.g. "myImage":"{ROOT_PATH}/mypath/image.jpg"
   * where the string "{ROOT_PATH}" is substituted with the value.
   *
   * @param[in] key The key of the constant
   * @param[in] value The value of the constant
   */
  void SetStyleConstant( const std::string& key, const Property::Value& value );

  /**
   * @brief Return the style constant set for a specific key
   *
   * @param[in] key The key of the constant
   * @param[out] valueOut The value of the constant if it exists
   *
   * @return If the constant for key exists then return the constant in valueOut and return true
   */
  bool GetStyleConstant( const std::string& key, Property::Value& valueOut );

  /**
   * @brief Apply the specified style to the control.
   *
   * The JSON file will be cached and subsequent calls using the same JSON file name will
   * use the already loaded cached values instead.
   *
   * @param[in] control The control to apply style.
   * @param[in] jsonFileName The name of the JSON style file to apply.
   * @param[in] styleName The name of the style within the JSON file to apply.
   */
  void ApplyStyle( Toolkit::Control control, const std::string& jsonFileName, const std::string& styleName );

public: // Signals

  /**
   * @brief This signal is emitted whenever the style (e.g. theme/font change) is changed on device
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( StyleManager styleManager, StyleChange change );
   * @endcode
   * @return The signal to connect to.
   */
  StyleChangeSignalType& StyleChangeSignal();

public:

  /**
   * @brief Creates a new handle from the implementation.
   *
   * @param[in] impl A pointer to the object.
   */
  explicit DALI_INTERNAL StyleManager( Internal::StyleManager *impl );

}; // class StyleManager

} // namespace Toolkit

} // namespace Dali


#endif /* __DALI_TOOLKIT_STYLE_MANAGER_H__ */
