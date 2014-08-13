#ifndef __DALI_TOOLKIT_SCROLL_BAR_H__
#define __DALI_TOOLKIT_SCROLL_BAR_H__

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
 *
 */

// INTERNAL INCLUDES
#include <dali/dali.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-component.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

// Forward declarations
class ScrollConnector;

namespace Internal DALI_INTERNAL
{
// Forward declarations

class ScrollBar;
}

/**
 * ScrollBar is a UI component that can be added to the scrollable controls
 * indicating the current scroll position of the scrollable content.
 */
class ScrollBar : public ScrollComponent
{
public:

  // Signals
  static const char* const SCROLL_POSITION_NOTIFIED_SIGNAL_NAME;        ///< "scroll-position-notified" signal name
  typedef SignalV2< void ( float ) > ScrollPositionNotifiedSignalType;

  // Properties
  static const Property::Index PROPERTY_INDICATOR_HEIGHT_POLICY;         ///< name "indicator-height-policy", type STRING
  static const Property::Index PROPERTY_INDICATOR_FIXED_HEIGHT;          ///< name "indicator-fixed-height",  type FLOAT

public:

  /**
   * @brief Indicator height policy.
   */
  enum IndicatorHeightPolicy
  {
    Variable = 0,  ///< Variable height changed dynamically according to the length of scroll content
    Fixed          ///< Fixed height regardless of the length of scroll content
  };

  /**
   * @brief Create an uninitialized ScrollBar; this can be initialized with ScrollBar::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   * or horizontally (false)
   */
  ScrollBar();

  /**
   * @brief Copy constructor.
   */
  ScrollBar( const ScrollBar& scrollBar );

  /**
   * @brief Assignment operator.
   */
  ScrollBar& operator=( const ScrollBar& scrollBar );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ScrollBar();

  /**
   * @brief Create an initialized ScrollBar
   * @return A pointer to the created ScrollBar.
   */
  static ScrollBar New();

  /**
   * @brief Downcast an Object handle to ScrollBar. If handle points to a ScrollBar the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ScrollBar or an uninitialized handle
   */
  static ScrollBar DownCast( BaseHandle handle );

  /**
   * @brief Sets the image for the indicator of scroll bar.
   *
   * @pre The scroll bar actor has been initialised.
   *
   * The indicator image is resized (stretched according to scale settings),
   * to reflect the size of the scroll indicator and minimum/maximum limits
   * of the scroll position.
   *
   * @param[in] image The image of indicator that moves to indicate the current scroll position.
   */
  void SetIndicatorImage( Image image );

  /**
   * @brief Gets the indicator of scroll bar.
   *
   * @pre The scroll bar actor has been initialised.
   *
   * @return The indicator indicates the current scroll position of the scrollable content.
   */
  Actor GetScrollIndicator();

  /**
   * @brief Sets the values to get notification when the current scroll position of the scrollable
   * content goes above or below any of these values.
   *
   * @pre The scroll bar actor has been initialised.
   *
   * @param[in] positions List of values to receive notifications for when the current scroll position crosses them
   */
  void SetPositionNotifications( const std::vector<float>& positions );

  /**
   * @brief Sets the height policy of scroll indicator to have either variable or fixed height.
   *
   * @pre The scroll bar actor has been initialised.
   *
   * @param[in] policy The height policy of scroll indicator
   */
  void SetIndicatorHeightPolicy( IndicatorHeightPolicy policy );

  /**
   * @brief Gets the height policy of scroll indicator.
   *
   * @return The height policy of scroll indicator
   */
  IndicatorHeightPolicy GetIndicatorHeightPolicy();

  /**
   * @brief Sets the fixed height of scroll indicator.
   * Normally the height of scroll indicator is changed dynamically according to the length of scroll content.
   * However, when the height policy of scroll indicator is set to be fixed, the height will keep fixed
   * regardless of the length of scroll content.
   *
   * @pre The scroll bar actor has been initialised.
   *
   * @param[in] height The fixed height of the scroll indicator
   */
  void SetIndicatorFixedHeight( float height );

  /**
   * @brief Gets the fix height of scroll indicator.
   * @return The fixed height of the scroll indicator
   */
  float GetIndicatorFixedHeight();

  /**
   * @brief Shows the scroll indicator
   */
  void Show();

  /**
   * @brief Hides the scroll indicator
   */
  void Hide();

  /**
   * @brief Signal emitted when the current scroll position of the scrollable content goes above or below the values
   * specified by SetPositionNotifications.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(float currentScrollPosition);
   * @endcode
   * @pre The Object has been initialized.
   * @return The signal to connect to.
   */
  ScrollBar::ScrollPositionNotifiedSignalType& ScrollPositionNotifiedSignal();

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  ScrollBar( Internal::ScrollBar& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  ScrollBar( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_SCROLL_BAR_H__
