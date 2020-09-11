#ifndef DALI_TOOLKIT_BUTTON_H
#define DALI_TOOLKIT_BUTTON_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class Button;
}
/**
 * @addtogroup dali_toolkit_controls_buttons
 * @{
 */

/**
 * @brief Button is a base class for different kinds of buttons.
 *
 * This class provides the disabled property and the clicked signal.
 *
 * A ClickedSignal() is emitted when the button is touched and the touch point doesn't leave the boundary of the button.
 *
 * When the \e disabled property is set to \e true, no signal is emitted.
 *
 * Button provides the following properties which modify the signals emitted:
 * <ul>
 *   <li>\e autorepeating
 *       When \e autorepeating is set to \e true, a Button::PressedSignal(), Button::ReleasedSignal() and Button::ClickedSignal() signals are emitted at regular
 *       intervals while the button is touched.
 *       The intervals could be modified with the Button::SetInitialAutoRepeatingDelay and Button::SetNextAutoRepeatingDelay methods.
 *
 *       A \e togglable button can't be \e autorepeating. If the \e autorepeating property is set to \e true, then the \e togglable property is set to
 *       false but no signal is emitted.
 *
 *   <li>\e togglable
 *       When \e togglable is set to \e true, a Button::StateChangedSignal() signal is emitted, with the selected state.
 * </ul>
 *
 * 'Visual' describes not just traditional images like png, bmp but refers to whatever is used to show the button, it could be a color, gradient or some other kind of renderer.
 *
 * The button's appearance can be modified by setting properties for the various visuals/images.
 *
 * It is not mandatory to set all visuals. A button could be defined only by setting its \e background visual or by setting its \e background and \e selected visuals.
 *
 * The \e button visual is shown over the \e background visual.
 * When pressed the unselected visuals are replaced by the \e selected visual. The text label is always placed on the top of all images.
 *
 * When the button is disabled, \e background, \e button and \e selected visuals are replaced by their \e disabled visuals.
 *
 *
 * Signals
 * | %Signal Name     | Method                      |
 * |------------------|-----------------------------|
 * | pressed          | @ref PressedSignal()        |
 * | released         | @ref ReleasedSignal()       |
 * | clicked          | @ref ClickedSignal()        |
 * | stateChanged     | @ref StateChangedSignal()   |
 *
 * Actions
 * | %Action Name     | Attributes              | Description                                   |
 * |------------------|-------------------------|-----------------------------------------------|
 * | buttonClick      | Doesn't have attributes | Simulates a button click. See @ref DoAction() |
 * @SINCE_1_0.0
 */
class DALI_TOOLKIT_API Button : public Control
{
public:
  /**
   * @brief Enumeration for the start and end property ranges for this control.
   * @SINCE_1_0.0
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1, ///< @SINCE_1_0.0
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000              ///< Reserve property indices @SINCE_1_0.0
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the Button class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    /**
     * @brief Enumeration for the instance of properties belonging to the Button class.
     * @SINCE_1_0.0
     */
    enum
    {
      /**
       * @brief name "disabled", type bool
       * @details Sets the button as \e disabled.
       * @SINCE_1_0.0
       */
      DISABLED = PROPERTY_START_INDEX,

      /**
       * @brief name "autoRepeating", type bool
       * @details If the \e autorepeating property is set to \e true then the \e togglable property is set to false
       * @SINCE_1_0.0
       */
      AUTO_REPEATING,

      /**
       * @brief name "initialAutoRepeatingDelay", type float
       * @details By default this value is set to 0.15 seconds.
       * @SINCE_1_0.0
       */
      INITIAL_AUTO_REPEATING_DELAY,

      /**
       * @brief name "nextAutoRepeatingDelay", type float
       * @details default this value is set to 0.05 seconds
       * @SINCE_1_0.0
       */
      NEXT_AUTO_REPEATING_DELAY,

      /**
       * @brief name "togglable", type bool
       * @details If the \e togglable property is set to \e true, then the \e autorepeating property is set to false.
       * @SINCE_1_0.0
       */
      TOGGLABLE,

      /**
       * @brief name "selected", type bool
       * @details Sets the togglable button as either selected or unselected, \e togglable property must be set to \e true.
       * @SINCE_1_0.0
       */
      SELECTED,

      /**
       * @brief name "unselectedVisual", type string if it is a url, map otherwise.
       * @details Sets the unselected button foreground/icon visual
       * @SINCE_1_4.32
       */
      UNSELECTED_VISUAL,

      /**
       * @brief name "selectedImage", type string if it is a url, map otherwise
       * @details Sets the selected button foreground/icon visual
       * @SINCE_1_4.32
       */
      SELECTED_VISUAL,

      /**
       * @brief name "disabledSelectedVisual", type string if it is a url, map otherwise
       * @details Sets the disabled selected state foreground/icon button visual
       * @SINCE_1_4.32
       */
      DISABLED_SELECTED_VISUAL,

      /**
       * @brief name "disabledUnselectedVisual", type string if it is a url, map otherwise
       * @details Sets the disabled unselected state foreground/icon visual
       * @SINCE_1_4.32
       */
      DISABLED_UNSELECTED_VISUAL,

      /**
       * @brief name "unselectedBackgroundVisual", type string if it is a url, map otherwise
       * @details Sets the disabled in the unselected state background, button visual
       * @SINCE_1_4.32
       */
      UNSELECTED_BACKGROUND_VISUAL,

      /**
       * @brief name "label", type Property::Map or std::string
       * @SINCE_1_0.0
       */
      LABEL,

      /**
       * @brief name "selectedBackgroundVisual", type string if it is a url, map otherwise
       * @details Sets the selected background button visual
       * @SINCE_1_4.32
       */
      SELECTED_BACKGROUND_VISUAL,

      /**
       * @brief name "disabledUnselectedBackgroundVisual", type string if it is a url, map otherwise
       * @details Sets the disabled while unselected background button visual
       * @SINCE_1_4.32
       */
      DISABLED_UNSELECTED_BACKGROUND_VISUAL,

      /**
       * @brief name "disabledSelectedBackgroundVisual", type string if it is a url, map otherwise
       * @details Sets the disabled while selected background button visual
       * @SINCE_1_4.32
       */
      DISABLED_SELECTED_BACKGROUND_VISUAL,
    };
  };

public:
  /**
   * @brief Creates an uninitialized Button.
   *
   * Only derived versions can be instantiated.  Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   * @SINCE_1_0.0
   */
  Button();

  /**
   * @brief Copy constructor.
   * @SINCE_1_0.0
   * @param[in] button Handle to an object
   */
  Button(const Button& button);

  /**
   * @brief Move constructor
   * @SINCE_1_9.23
   *
   * @param[in] rhs A reference to the moved handle
   */
  Button(Button&& rhs);

  /**
   * @brief Assignment operator.
   * @SINCE_1_0.0
   * @param[in] button Handle to an object
   * @return A reference to this
   */
  Button& operator=(const Button& button);

  /**
   * @brief Move assignment
   * @SINCE_1_9.23
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  Button& operator=(Button&& rhs);

  /**
   * @brief Downcasts a handle to Button handle.
   *
   * If handle points to a Button, the downcast produces valid handle.
   * If not the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return A handle to a Button or an uninitialized handle
   */
  static Button DownCast(BaseHandle handle);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~Button();

public: //Signals
  /**
   * @brief Button signal type.
   * @SINCE_1_0.0
   */
  typedef Signal<bool(Button)> ButtonSignalType;

  /**
   * @brief This signal is emitted when the button is touched.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( Button button );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   */
  ButtonSignalType& PressedSignal();

  /**
   * @brief This signal is emitted when the button is touched and the touch point leaves the boundary of the button.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( Button button );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   */
  ButtonSignalType& ReleasedSignal();

  /**
   * @brief This signal is emitted when the button is touched and the touch point doesn't leave the boundary of the button.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( Button button );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   */
  ButtonSignalType& ClickedSignal();

  /**
   * @brief This signal is emitted when the button's state is changed.
   *
   * The application can get the state by calling IsSelected().
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( Button button );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   */
  ButtonSignalType& StateChangedSignal();

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @SINCE_1_0.0
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL Button(Internal::Button& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @SINCE_1_0.0
   * @param[in] internal A pointer to the internal CustomActor
   */
  DALI_INTERNAL Button(Dali::Internal::CustomActor* internal);
  /// @endcond
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_BUTTON_H
