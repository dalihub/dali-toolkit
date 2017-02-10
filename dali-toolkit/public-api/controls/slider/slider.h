#ifndef __DALI_TOOLKIT_SLIDER_H__
#define __DALI_TOOLKIT_SLIDER_H__

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
class Slider;
}
/**
 * @addtogroup dali_toolkit_controls_slider
 * @{
 */

/**
 * @brief Slider is a control to enable sliding an indicator between two values.
 *
 * Signals
 * | %Signal Name      | Method                        |
 * |-------------------|-------------------------------|
 * | valueChanged      | @ref ValueChangedSignal()     |
 * | markReached       | @ref MarkReachedSignal()      |
 * | slidingFinished   | @ref SlidingFinishedSignal()  |
 * @SINCE_1_1.39
 */
class DALI_IMPORT_API Slider : public Control
{
public:

  // Properties

  /**
   * @brief Enumeration for the start and end property ranges for this control.
   * @SINCE_1_1.39
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1, ///< @SINCE_1_1.39
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices @SINCE_1_1.39
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the Slider class.
   * @SINCE_1_1.39
   */
  struct Property
  {
    /**
     * @brief Enumeration for the instance of properties belonging to the Slider class.
     * @SINCE_1_1.39
     */
    enum
    {

      /**
       * @brief name "lowerBound", type float.
       * @SINCE_1_1.39
       */
      LOWER_BOUND = PROPERTY_START_INDEX,

      /**
       * @brief name "upperBound", type float.
       * @SINCE_1_1.39
       */
      UPPER_BOUND,

      /**
       * @brief name "value", type float.
       * @SINCE_1_1.39
       */
      VALUE,

      /**
       * @brief name "trackVisual", type string if it is a url, map otherwise.
       * @SINCE_1_1.39
       */
      TRACK_VISUAL,

      /**
       * @brief name "handleVisual", type string if it is a url, map otherwise.
       * @SINCE_1_1.39
       */
      HANDLE_VISUAL,

      /**
       * @brief name "progressVisual", type string if it is a url, map otherwise.
       * @SINCE_1_1.39
       */
      PROGRESS_VISUAL,

      /**
       * @brief name "popupVisual", type string if it is a url, map otherwise.
       * @SINCE_1_1.39
       */
      POPUP_VISUAL,

      /**
       * @brief name "popupArrowVisual", type string if it is a url, map otherwise.
       * @SINCE_1_1.39
       */
      POPUP_ARROW_VISUAL,

      /**
       * @brief name "disabledColor", type Vector4.
       * @SINCE_1_1.39
       */
      DISABLED_COLOR,

      /**
       * @brief name "valuePrecision", type int.
       * @SINCE_1_1.39
       */
      VALUE_PRECISION,

      /**
       * @brief name "showPopup", type bool.
       * @SINCE_1_1.39
       */
      SHOW_POPUP,

      /**
       * @brief name "showValue", type bool.
       * @SINCE_1_1.39
       */
      SHOW_VALUE,

      /**
       * @brief name "marks", type Property::Array<float>.
       * @SINCE_1_1.39
       */
      MARKS,

      /**
       * @brief name "snapToMarks", type bool.
       * @SINCE_1_1.39
       */
      SNAP_TO_MARKS,

      /**
       * @brief name "markTolerance", type float.
       * @SINCE_1_1.39
       */
      MARK_TOLERANCE,
    };
  };

public:

  /**
   * @brief Creates the Slider control.
   * @SINCE_1_1.39
   * @return A handle to the Slider control
   */
  static Slider New();

  /**
   * @brief Creates an empty Slider handle.
   * @SINCE_1_1.39
   */
  Slider();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @SINCE_1_1.39
   * @param[in] handle Handle to an object
   */
  Slider( const Slider& handle );

  /**
   * @brief Assignment operator.
   *
   * Changes this handle to point to another real object.
   * @SINCE_1_1.39
   * @param[in] handle Handle to an object
   * @return A reference to this
   */
  Slider& operator=( const Slider& handle );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_1.39
   */
  ~Slider();

  /**
   * @brief Downcasts an Object handle to Slider.
   *
   * If handle points to a Slider, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   * @SINCE_1_1.39
   * @param[in] handle Handle to an object
   * @return handle to a Slider or an uninitialized handle
   */
  static Slider DownCast( BaseHandle handle );

public:  // Signals

  /**
   * @brief Value changed signal type.
   * @SINCE_1_1.39
   */
  typedef Signal< bool ( Slider, float ) > ValueChangedSignalType;

  /**
   * @brief Mark reached signal type.
   * @SINCE_1_1.39
   */
  typedef Signal< bool ( Slider, int ) > MarkReachedSignalType;

  /**
   * @brief Signal emitted when the slider value changes.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( Slider slider, float value );
   * @endcode
   * @SINCE_1_1.39
   * @return The signal to connect to
   */
  ValueChangedSignalType& ValueChangedSignal();

  /**
   * @brief Signal emitted when the sliding is finished.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( Slider slider, float value );
   * @endcode
   * @SINCE_1_1.39
   * @return The signal to connect to
   */
  ValueChangedSignalType& SlidingFinishedSignal();

  /**
   * @brief Signal emitted when the slider handle reaches a mark.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( Slider slider, int value );
   * @endcode
   * @SINCE_1_1.39
   * @return The signal to connect to
   */
  MarkReachedSignalType& MarkReachedSignal();

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @SINCE_1_1.39
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL Slider(Internal::Slider& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @SINCE_1_1.39
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL Slider( Dali::Internal::CustomActor* internal );
  /// @endcond
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SLIDER_H__
