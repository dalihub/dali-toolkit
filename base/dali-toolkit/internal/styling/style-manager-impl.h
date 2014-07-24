#ifndef __DALI_TOOLKIT_INTERNAL_STYLE_MANAGER_H__
#define __DALI_TOOLKIT_INTERNAL_STYLE_MANAGER_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <string>
#include <list>

// INTERNAL INCLUDES

#include <dali/dali.h>
#include <dali-toolkit/public-api/styling/style-manager.h>
#include <dali-toolkit/public-api/builder/builder.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
/**
 * @copydoc Toolkit::StyleManager
 */
class StyleManager : public Dali::BaseObject, public ConnectionTracker
{
public:

  /**
   * Singleton access
   *
   * @return The StyleManager object
   */
  static Toolkit::StyleManager Get();

  /**
   * Construct a new StyleManager.
   */
  StyleManager();

  /**
   * @copydoc Toolkit::StyleManager::SetOrientationValue
   */
  void SetOrientationValue( int orientation );

  /**
   * @copydoc Toolkit::StyleManager::GetOrientationValue
   */
  int GetOrientationValue();

  /**
   * @copydoc Toolkit::StyleManager::SetOrientation( Orientation orientation )
   */
  void SetOrientation( Orientation orientation );

  /**
   * @copydoc Toolkit::StyleManager::GetOrientation
   */
  Orientation GetOrientation();

  /**
   * @copydoc Toolkit::StyleManager::SetStyleConstant
   */
  void SetStyleConstant( const std::string& key, const Property::Value& value );

  /**
   * @copydoc Toolkit::StyleManager::GetStyleConstant
   */
  bool GetStyleConstant( const std::string& key, Property::Value& valueOut );

  /**
   * @copydoc Toolkit::StyleManager::RequestThemeChange
   */
  void RequestThemeChange( const std::string& themeFile );

  /**
   * @copydoc Toolkit::StyleManager::RequestDefaultTheme
   */
  void RequestDefaultTheme();

  /**
   * Determine if a theme change has been requested
   * @return Whether a theme request is pending
   */
  bool IsThemeRequestPending();

  /**
   * @brief Apply the theme style to a control.
   *
   * @param[in] control The control to apply style.
   */
  void ApplyThemeStyle( Toolkit::Control control );

  /**
   * @copydoc Toolkit::StyleManager::ApplyStyle
   */
  void ApplyStyle( Toolkit::Control control, const std::string& jsonFileName, const std::string& styleName );

public:
  // SIGNALS

  /**
   * @copydoc Toolkit::StyleManager::StyleChangeSignal
   */
  Toolkit::StyleManager::StyleChangeSignalType& StyleChangeSignal();

protected:

  /**
   * @brief Destructor
   */
  virtual ~StyleManager();


public:

  /**
   * @brief Set the current theme. Called only once per event processing cycle.
   */
  void SetTheme();

private:

  typedef std::list<std::string> StringList;

  /**
   * @brief Internal helper method to read a file from file system.
   * @param filename The name of the file to read.
   * @param[out] stringOut The string to return the file in
   *
   * @param Return true if file was found
   */
  bool LoadFile(const std::string& filename, std::string& stringOut);

  /**
   * @brief Create a new builder.
   *
   * @param[in] constants A map of constants to be used by the builder
   *
   * @return Return the newly created builder
   */
  Toolkit::Builder CreateBuilder( const PropertyValueMap& constants );

  /**
   * @brief Load a JSON file into given builder
   *
   * @param[in] builder The builder object to load the theme file
   * @param[in] jsonFileName The name of the JSON file to load
   * @return Return true if file was loaded
   */
  bool LoadJSON( Toolkit::Builder builder, const std::string& jsonFileName );

  /**
   * @brief Collect qualifiers (e.g. Landscape, portrait etc) for a given style
   *
   * @param[in,out] qualifiersOut The list to populate with qualifiers
   */
  void CollectQualifiers( StringList& qualifiersOut );

  /**
   * @brief Construct a qualified style name out of qualifiers
   *
   * A qualifed style name will be in the format: style-qualifier0-qualifier1-qualifierN
   *
   * @param[in] styleName The root name of the style
   * @param[in] qualifiers List of qualifier names
   * @param[out] qualifiedStyleOut The qualified style name
   */
  void BuildQualifiedStyleName( const std::string& styleName, const StringList& qualifiers, std::string& qualifiedStyleOut );

  /**
   * @brief Apply a style to the control using the given builder
   *
   * @param[in] builder The builder to apply the style from
   * @param[in] control The control to apply the style to
   */
  void ApplyStyle( Toolkit::Builder builder, Toolkit::Control control );

  /**
   * @brief Callback for orientation changes
   *
   * @param[in] orientation The orientation object
   */
  void OnOrientationChanged( Orientation orientation );

  /**
   * Search for a builder in the cache
   *
   * @param[in] key The key the builder was cached under
   * @return Return the cached builder if found or an empty builder object if not found
   */
  Toolkit::Builder FindCachedBuilder( const std::string& key );

  /**
   * Store a given builder in the cache keyed to the given key
   *
   * @param[in] builder The builder object to store
   * @param[in] key The key to store the builder under
   */
  void CacheBuilder( Toolkit::Builder builder, const std::string& key );

  /**
   * Callback for when style monitor raises a signal
   *
   * @param[in] styleMonitor The style monitor object
   * @param[in] styleChange The style change data
   */
  void StyleMonitorChange( StyleMonitor styleMonitor, StyleChange styleChange );

  // Undefined
  StyleManager(const StyleManager&);

  StyleManager& operator=(const StyleManager& rhs);

private:

  // Map to store builders keyed by JSON file name
  typedef std::map< std::string, Toolkit::Builder > BuilderMap;

  Toolkit::Builder mThemeBuilder;     ///< Builder for all default theme properties

  Orientation mOrientation;           ///< Handle to application orientation object
  int mOrientationDegrees;            ///< Directly set value of orientation

  std::string mThemeFile;             ///< The full path of the current theme file

  Toolkit::PropertyValueMap mThemeBuilderConstants;   ///< Contants to give the theme builder
  Toolkit::PropertyValueMap mStyleBuilderConstants;   ///< Constants specific to building styles

  BuilderMap mBuilderCache;           ///< Cache of builders keyed by JSON file name

  bool mSetThemeConnection;           ///< Has the callback to set the theme been set

  // Signals
  Toolkit::StyleManager::StyleChangeSignalType       mStyleChangeSignal;         ///< Emitted when the style( theme/font ) changes
};

} // namespace Internal

inline Internal::StyleManager& GetImpl( Dali::Toolkit::StyleManager& obj )
{
  DALI_ASSERT_ALWAYS( obj );

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast< Internal::StyleManager& >( handle );
}

inline const Internal::StyleManager& GetImpl( const Dali::Toolkit::StyleManager& obj )
{
  DALI_ASSERT_ALWAYS( obj );

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast< const Internal::StyleManager& >( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_STYLE_MANAGER_H__

