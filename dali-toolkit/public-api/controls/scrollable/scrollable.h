#ifndef __DALI_TOOLKIT_SCROLLABLE_H__
#define __DALI_TOOLKIT_SCROLLABLE_H__

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
class Scrollable;
}
/**
 * @addtogroup dali_toolkit_controls_scrollable
 * @{
 */

/**
 * @brief Base class for derived Scrollables that contains actors that can be scrolled manually
 * (via touch) or automatically.
 *
 * Scrollables such as ScrollView and ItemView can be derived from this class.
 *
 * Signals
 * | %Signal Name     | Method                       |
 * |------------------|------------------------------|
 * | scrollStarted    | @ref ScrollStartedSignal()   |
 * | scrollCompleted  | @ref ScrollCompletedSignal() |
 * | scrollUpdated    | @ref ScrollUpdatedSignal()   |
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API Scrollable : public Control
{
public:

  /**
   * @brief The start and end property ranges for this control.
   * @SINCE_1_0.0
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1, ///< @SINCE_1_0.0
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000,             ///< Reserve property indices @SINCE_1_0.0

    ANIMATABLE_PROPERTY_START_INDEX = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,
    ANIMATABLE_PROPERTY_END_INDEX =   ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1000              ///< Reserve animatable property indices @SINCE_1_0.0
  };

  /**
   * @brief An enumeration of properties belonging to the Scrollable class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    /**
     * @brief An enumeration of properties belonging to the Scrollable class.
     * @SINCE_1_0.0
     */
    enum
    {
      // Event side properties
      OVERSHOOT_EFFECT_COLOR = PROPERTY_START_INDEX, ///< Property, name "overshootEffectColor",      @see SetOvershootEffectColor(),    type Vector4 @SINCE_1_0.0
      OVERSHOOT_ANIMATION_SPEED,                     ///< Property, name "overshootAnimationSpeed",   @see SetOvershootAnimationSpeed(), type float @SINCE_1_0.0
      OVERSHOOT_ENABLED,                             ///< Property, name "overshootEnabled",          @see SetOvershootEnabled(),        type bool,    @SINCE_1_1.18
      OVERSHOOT_SIZE,                                ///< Property, name "overshootSize",                                                type Vector2, @SINCE_1_1.31
      SCROLL_TO_ALPHA_FUNCTION,                      ///< Property, name "scrollToAlphaFunction",                                        type int,     @SINCE_1_1.33

      // Animatable properties
      SCROLL_RELATIVE_POSITION = ANIMATABLE_PROPERTY_START_INDEX, ///< Property, name "scrollRelativePosition",   type Vector2 @SINCE_1_0.0
      SCROLL_POSITION_MIN,                                        ///< Property, name "scrollPositionMin",        type Vector2 @SINCE_1_0.0
      SCROLL_POSITION_MIN_X,                                      ///< Property, name "scrollPositionMinX",       type float @SINCE_1_0.0
      SCROLL_POSITION_MIN_Y,                                      ///< Property, name "scrollPositionMinY",       type float @SINCE_1_0.0
      SCROLL_POSITION_MAX,                                        ///< Property, name "scrollPositionMax",        type Vector2 @SINCE_1_0.0
      SCROLL_POSITION_MAX_X,                                      ///< Property, name "scrollPositionMaxX",       type float @SINCE_1_0.0
      SCROLL_POSITION_MAX_Y,                                      ///< Property, name "scrollPositionMaxY",       type float @SINCE_1_0.0
      CAN_SCROLL_VERTICAL,                                        ///< Property, name "canScrollVertical",        type bool @SINCE_1_0.0
      CAN_SCROLL_HORIZONTAL                                       ///< Property, name "canScrollHorizontal",      type bool @SINCE_1_0.0
    };
  };

  // Typedefs

  typedef Signal< void ( const Vector2& ) > ScrollStartedSignalType;   ///< ScrollStarted signal type @SINCE_1_0.0
  typedef Signal< void ( const Vector2& ) > ScrollCompletedSignalType; ///< ScrollCompleted signal type @SINCE_1_0.0
  typedef Signal< void ( const Vector2& ) > ScrollUpdatedSignalType;   ///< Scroll updated signal type @SINCE_1_0.0

public:

  /**
   * @brief Creates an uninitialized Scrollable handle.
   * @SINCE_1_0.0
   */
  Scrollable();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   *
   * @SINCE_1_0.0
   * @param handle to copy from
   */
  Scrollable( const Scrollable& handle );

  /**
   * @brief Assignment operator.
   *
   * Changes this handle to point to another real object.
   * @SINCE_1_0.0
   * @param[in] handle to copy from
   * @return A reference to this
   */
  Scrollable& operator=( const Scrollable& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~Scrollable();

  /**
   * @brief Downcast a handle to Scrollable handle.
   *
   * If handle points to a Scrollable the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return A handle to a Scrollable or an uninitialized handle
   */
  static Scrollable DownCast( BaseHandle handle );

  /**
   * @brief Checks if scroll overshoot has been enabled or not.
   *
   * @SINCE_1_0.0
   * @return Whether the scroll overshoot is enabled
   */
  bool IsOvershootEnabled() const;

  /**
   * @brief Sets whether to enables or disable scroll overshoot.
   *
   * @SINCE_1_0.0
   * @param[in] enable Whether to enable the scroll overshoot or not
   */
  void SetOvershootEnabled(bool enable);

  /**
   * @brief Set the color of the overshoot effect.
   *
   * @SINCE_1_0.0
   * @param[in] color The color of the overshoot effect.
   */
  void SetOvershootEffectColor( const Vector4& color );

  /**
   * @brief Get the color of the overshoot effect.
   * @SINCE_1_0.0
   * @return The color of the overshoot effect.
   */
  Vector4 GetOvershootEffectColor() const;

  /**
   * @brief Set the speed of overshoot animation in pixels per second.
   *
   * When the speed is not greater than 0, the overshoot is set instantly with no animation.
   * @SINCE_1_0.0
   * @param[in] pixelsPerSecond The speed of the overshoot animation.
   */
  void SetOvershootAnimationSpeed( float pixelsPerSecond );

  /**
   * @brief Get the speed of overshoot animation in pixels per second.
   * @SINCE_1_0.0
   * @return The speed of the overshoot animation.
   */
  float GetOvershootAnimationSpeed() const;

public: // Signals

  /**
   * @brief Signal emitted when the Scrollable has moved (whether by touch or animation).
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(const Vector2& currentScrollPosition);
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to.
   * @pre The Object has been initialized.
   */
  ScrollStartedSignalType& ScrollStartedSignal();

  /**
   * @brief Signal emitted when the Scrollable has moved (whether by touch or animation).
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(const Vector2& currentScrollPosition);
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to.
   * @pre The Object has been initialized.
   */
  ScrollUpdatedSignalType& ScrollUpdatedSignal();

  /**
   * @brief Signal emitted when the Scrollable has completed movement (whether by touch or animation).
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(const Vector2& currentScrollPosition);
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to.
   * @pre The Object has been initialized.
   */
  ScrollCompletedSignalType& ScrollCompletedSignal();

public: // Not intended for application developers

  /**
   * @internal
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @SINCE_1_0.0
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL Scrollable(Internal::Scrollable& implementation);

  /**
   * @internal
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @SINCE_1_0.0
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL Scrollable( Dali::Internal::CustomActor* internal );
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLLABLE_H__
