#ifndef __DALI_TOOLKIT_SCROLL_BAR_H__
#define __DALI_TOOLKIT_SCROLL_BAR_H__

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
// Forward declarations

class ScrollBar;
}
/**
 * @addtogroup dali_toolkit_controls_scroll_bar
 * @{
 */

/**
 * @brief ScrollBar is a UI component that can be linked to the scrollable objects
 * indicating the current scroll position of the scrollable object.
 *
 * Signals
 * | %Signal Name                  | Method                                     |
 * |-------------------------------|--------------------------------------------|
 * | panFinished                   | @ref PanFinishedSignal()                   |
 * | scrollPositionIntervalReached | @ref ScrollPositionIntervalReachedSignal() |
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API ScrollBar : public Control
{
public:

  // Properties

  /**
   * @brief Enumeration for the start and end property ranges for this control.
   * @SINCE_1_0.0
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1, ///< @SINCE_1_0.0
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices @SINCE_1_0.0
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the ScrollBar class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    /**
     * @brief Enumeration for the instance of properties belonging to the ScrollBar class.
     * @SINCE_1_0.0
     */
    enum
    {
      /**
       * @brief name "scrollDirection", type std::string.
       * @SINCE_1_0.0
       * @see SetScrollDirection()
       */
      SCROLL_DIRECTION = PROPERTY_START_INDEX,
      /**
       * @brief name "indicatorHeightPolicy", type std::string.
       * @SINCE_1_0.0
       * @see SetIndicatorHeightPolicy()
       */
      INDICATOR_HEIGHT_POLICY,
      /**
       * @brief name "indicatorFixedHeight", type float.
       * @SINCE_1_0.0
       * @see SetIndicatorFixedHeight()
       */
      INDICATOR_FIXED_HEIGHT,
      /**
       * @brief name "indicatorShowDuration", type float.
       * @SINCE_1_0.0
       * @see SetIndicatorShowDuration()
       */
      INDICATOR_SHOW_DURATION,
      /**
       * @brief name "indicatorHideDuration", type float.
       * @SINCE_1_0.0
       * @see SetIndicatorHideDuration()
       */
      INDICATOR_HIDE_DURATION,
      /**
       * @brief name "scrollPositionIntervals", type Property::Array.
       * @SINCE_1_0.0
       * @see SetScrollPositionIntervals()
       */
      SCROLL_POSITION_INTERVALS,
      /**
       * @brief name "indicatorMinimumHeight", type float.
       * The minimum height for a variable size indicator.
       * @SINCE_1_1.36
       */
      INDICATOR_MINIMUM_HEIGHT,
      /**
       * @brief name "indicatorStartPadding", type float.
       * The padding at the start of the indicator. For example, the top if scrollDirection is Vertical.
       * @SINCE_1_1.36
       */
      INDICATOR_START_PADDING,
      /**
       * @brief name "indicatorEndPadding", type float.
       * The padding at the end of the indicator. For example, the bottom if scrollDirection is Vertical.
       * @SINCE_1_1.36
       */
      INDICATOR_END_PADDING
    };
  };

  // Signals

  typedef Signal< void () > PanFinishedSignalType;
  typedef Signal< void ( float ) > ScrollPositionIntervalReachedSignalType;

public:

  /**
   * @brief Direction.
   * @SINCE_1_0.0
   */
  enum Direction
  {
    Vertical = 0,   ///< Scroll in the vertical direction @SINCE_1_0.0
    Horizontal      ///< Scroll in the horizontal direction @SINCE_1_0.0
  };

  /**
   * @brief Indicator height policy.
   * @SINCE_1_0.0
   */
  enum IndicatorHeightPolicy
  {
    Variable = 0,  ///< Variable height changed dynamically according to the length of scroll content @SINCE_1_0.0
    Fixed          ///< Fixed height regardless of the length of scroll content @SINCE_1_0.0
  };

  /**
   * @brief Creates an uninitialized ScrollBar; this can be initialized with ScrollBar::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   * @SINCE_1_0.0
   */
  ScrollBar();

  /**
   * @brief Copy constructor.
   * @SINCE_1_0.0
   * @param[in] scrollBar Handle to an object
   */
  ScrollBar( const ScrollBar& scrollBar );

  /**
   * @brief Assignment operator.
   * @SINCE_1_0.0
   * @param[in] scrollBar Handle to an object
   * @return A reference to this
   */
  ScrollBar& operator=( const ScrollBar& scrollBar );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~ScrollBar();

  /**
   * @brief Creates an initialized ScrollBar.
   * @SINCE_1_0.0
   * @param[in] direction The direction of scroll bar (either vertically or horizontally)
   * @return A pointer to the created ScrollBar
   */
  static ScrollBar New(Direction direction = Vertical);

  /**
   * @brief Downcasts a handle to ScrollBar handle.
   *
   * If handle points to a ScrollBar, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a ScrollBar or an uninitialized handle
   */
  static ScrollBar DownCast( BaseHandle handle );

  /**
   * @brief Sets the source of the scroll position properties.
   *
   * @SINCE_1_0.0
   * @param[in] handle The handle of the object owing the scroll properties
   * @param[in] propertyScrollPosition The index of the scroll position property (The scroll position, type float)
   * @param[in] propertyMinScrollPosition The index of the minimum scroll position property (The minimum scroll position, type float)
   * @param[in] propertyMaxScrollPosition The index of the maximum scroll position property (The maximum scroll position, type float)
   * @param[in] propertyScrollContentSize The index of the scroll content size property (The size of the scrollable content in actor coordinates, type float)
   * @pre The handle to the object owing the scroll properties has been initialised and the property index must be valid.
   */
  void SetScrollPropertySource( Handle handle, Dali::Property::Index propertyScrollPosition, Dali::Property::Index propertyMinScrollPosition, Dali::Property::Index propertyMaxScrollPosition, Dali::Property::Index propertyScrollContentSize );

  /**
   * @brief Sets the indicator of scroll bar.
   *
   * @SINCE_1_0.0
   * @param[in] indicator The indicator that moves to indicate the current scroll position
   * @pre The scroll bar actor has been initialized.
   */
  void SetScrollIndicator( Actor indicator );

  /**
   * @brief Gets the indicator of scroll bar.
   *
   * @SINCE_1_0.0
   * @return The indicator indicates the current scroll position of the scrollable content
   * @pre The scroll bar actor has been initialized.
   */
  Actor GetScrollIndicator();

  /**
   * @brief Sets the list of values to get notification when the current scroll position of the scrollable
   * object goes above or below any of these values.
   *
   * @SINCE_1_0.0
   * @param[in] positions List of values to receive notifications for when the current scroll position crosses them
   * @pre The scroll bar actor has been initialized.
   */
  void SetScrollPositionIntervals( const Dali::Vector<float>& positions );

  /**
   * @brief Gets the list of values to receive notifications when the current scroll position of the scrollable
   * object goes above or below any of these values.
   *
   * @SINCE_1_0.0
   * @return The list of values to receive notifications for when the current scroll position crosses them
   * @pre The scroll bar actor has been initialized.
   *
   */
  Dali::Vector<float> GetScrollPositionIntervals() const;

  /**
   * @brief Sets the direction of scroll bar to scroll either vertically or horizontally.
   *
   * @SINCE_1_0.0
   * @param[in] direction The direction of scroll bar (either vertically or horizontally)
   * @pre The scroll bar actor has been initialized.
   */
  void SetScrollDirection( Direction direction );

  /**
   * @brief Gets the direction of scroll bar.
   *
   * @SINCE_1_0.0
   * @return The direction of scroll bar
   */
  Direction GetScrollDirection() const;

  /**
   * @brief Sets the height policy of scroll indicator to have either variable or fixed height.
   *
   * @SINCE_1_0.0
   * @param[in] policy The height policy of scroll indicator
   * @pre The scroll bar actor has been initialized.
   */
  void SetIndicatorHeightPolicy( IndicatorHeightPolicy policy );

  /**
   * @brief Gets the height policy of scroll indicator.
   *
   * @SINCE_1_0.0
   * @return The height policy of scroll indicator
   */
  IndicatorHeightPolicy GetIndicatorHeightPolicy() const;

  /**
   * @brief Sets the fixed height of scroll indicator.
   *
   * Normally the height of scroll indicator is changed dynamically according to the length of scroll content.
   * However, when the height policy of scroll indicator is set to be fixed, the height will be kept fixed
   * regardless of the length of scroll content.
   *
   * @SINCE_1_0.0
   * @param[in] height The fixed height of the scroll indicator
   * @pre The scroll bar actor has been initialized.
   *
   */
  void SetIndicatorFixedHeight( float height );

  /**
   * @brief Gets the fix height of scroll indicator.
   * @SINCE_1_0.0
   * @return The fixed height of the scroll indicator
   */
  float GetIndicatorFixedHeight() const;

  /**
   * @brief Sets the duration in seconds for the scroll indicator to become fully visible.
   *
   * @SINCE_1_0.0
   * @param[in] durationSeconds The duration for the scroll indicator to become fully visible
   * @pre The scroll bar actor has been initialised; durationSeconds must be zero or greater; zero means the indicator will be shown instantly.
   *
   */
  void SetIndicatorShowDuration( float durationSeconds );

  /**
   * @brief Gets the duration in seconds for the scroll indicator to become fully visible.
   * @SINCE_1_0.0
   * @return The duration for the scroll indicator to become fully visible
   */
  float GetIndicatorShowDuration() const;

  /**
   * @brief Sets the duration in seconds for the scroll indicator to become fully invisible.
   *
   * @SINCE_1_0.0
   * @param[in] durationSeconds The duration for the scroll indicator to become fully invisible
   * @pre The scroll bar actor has been initialised; durationSeconds must be zero or greater; zero means the indicator will be hidden instantly.
   *
   */
  void SetIndicatorHideDuration( float durationSeconds );

  /**
   * @brief Gets the duration in seconds for the scroll indicator to become fully invisible.
   * @SINCE_1_0.0
   * @return The duration for the scroll indicator to become fully invisible
   */
  float GetIndicatorHideDuration() const;

  /**
   * @brief Shows the scroll indicator.
   * @SINCE_1_0.0
   */
  void ShowIndicator();

  /**
   * @brief Hides the scroll indicator.
   * @SINCE_1_0.0
   */
  void HideIndicator();

public: // Signals

  /**
   * @brief Signal emitted when panning is finished on the scroll indicator.
   *
   * Signal only emitted when the source of the scroll position properties are set.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName();
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The Object has been initialized.
   */
  ScrollBar::PanFinishedSignalType& PanFinishedSignal();

  /**
   * @brief Signal emitted when the current scroll position of the scrollable content
   * goes above or below the values specified by SCROLL_POSITION_INTERVALS property.
   *
   * Signal only emitted when the source of the scroll position properties are set.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(float currentScrollPosition);
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The Object has been initialized.
   */
  ScrollBar::ScrollPositionIntervalReachedSignalType& ScrollPositionIntervalReachedSignal();

public: // Not intended for application developers

  /**
   * @internal
   * @brief Creates a handle using the Toolkit::Internal implementation.
   * @SINCE_1_0.0
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL ScrollBar( Internal::ScrollBar& implementation );

  /**
   * @internal
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   * @SINCE_1_0.0
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL ScrollBar( Dali::Internal::CustomActor* internal );
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_BAR_H__
