#ifndef __DALI_TOOLKIT_BUTTON_H__
#define __DALI_TOOLKIT_BUTTON_H__

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
class DALI_IMPORT_API Button : public Control
{
public:

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
       * @DEPRECATED_1_1.XX Use UNSELECTED_VISUAL
       * @brief name "unselectedStateImage", type string if it is a url, map otherwise
       * @details Sets the unselected button foreground image
       * @SINCE_1_0.0
       */
      UNSELECTED_STATE_IMAGE,

      /**
       * @DEPRECATED_1_1.XX Use SELECTED_VISUAL
       * @brief name "selectedStateImage", type string if it is a url, map otherwise
       * @details Sets the selected button foreground image
       * @SINCE_1_0.0
       */
      SELECTED_STATE_IMAGE,

      /**
       * @DEPRECATED_1_1.XX Use DISABLED_UNSELECTED_VISUAL
       * @brief name "disabledStateImage", type string if it is a url, map otherwise
       * @details Sets the disabled whilst unselected foreground button visual
       * @SINCE_1_0.0
       */
      DISABLED_STATE_IMAGE,

      /**
       * @DEPRECATED_1_1.XX Use UNSELECTED_BACKGROUND_VISUAL
       * @brief name "unselectedColor", type Vector4
       * @SINCE_1_0.0
       */
      UNSELECTED_COLOR,

      /**
       * @DEPRECATED_1_1.XX Use SELECTED_BACKGROUND_VISUAL
       * @brief name "selectedColor", type Vector4
       * @SINCE_1_0.0
       */
      SELECTED_COLOR,

      /**
       * @brief name "label", type Property::Map or std::string
       * @SINCE_1_0.0
       */
      LABEL,

      /**
       * @DEPRECATED_1_1.32 Use LABEL
       * @brief name "labelText", type std::string
       * @SINCE_1_0.0
       */
      LABEL_TEXT
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
  Button( const Button& button );

  /**
   * @brief Assignment operator.
   * @SINCE_1_0.0
   * @param[in] button Handle to an object
   * @return A reference to this
   */
  Button& operator=( const Button& button );

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
  static Button DownCast( BaseHandle handle );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~Button();

  // Deprecated API

  /**
   * @DEPRECATED_1_1.32 Use SetProperty DISABLED or Styling file
   *
   * @brief Sets the button as \e disabled.
   *
   * No signals are emitted when the \e disabled property is set.
   *
   * @SINCE_1_0.0
   * @param[in] disabled Disabled property
   */
  void SetDisabled( bool disabled ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 Use GetProperty DISABLED
   *
   * @brief Returns if the button is disabled.
   * @SINCE_1_0.0
   * @return \e true if the button is \e disabled
   */
  bool IsDisabled() const DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 SetProperty AUTO_REPEATING or Styling file
   *
   * @brief Sets the \e autorepeating property.
   *
   * If the \e autorepeating property is set to \e true, then the \e togglable property is set to false
   * but no signal is emitted.
   *
   * @SINCE_1_0.0
   * @param[in] autoRepeating \e autorepeating property
   */
  void SetAutoRepeating( bool autoRepeating ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 GetProperty AUTO_REPEATING
   *
   * @brief Returns if the autorepeating property is set.
   * @SINCE_1_0.0
   * @return \e true if the \e autorepeating property is set
   */
  bool IsAutoRepeating() const DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 SetProperty INITIAL_AUTO_REPEATING_DELAY or Styling file
   *
   * @brief Sets the initial autorepeating delay.
   *
   * By default, this value is set to 0.15 seconds.
   *
   * @SINCE_1_0.0
   * @param[in] initialAutoRepeatingDelay in seconds
   * @pre initialAutoRepeatingDelay must be greater than zero.
   */
  void SetInitialAutoRepeatingDelay( float initialAutoRepeatingDelay ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 GetProperty INITIAL_AUTO_REPEATING_DELAY
   *
   * @brief Gets the initial autorepeating delay in seconds.
   * @SINCE_1_0.0
   * @return The initial autorepeating delay in seconds
   */
  float GetInitialAutoRepeatingDelay() const DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 SetProperty NEXT_AUTO_REPEATING_DELAY or Styling file
   *
   * @brief Sets the next autorepeating delay.
   *
   * By default, this value is set to 0.05 seconds.
   *
   * @SINCE_1_0.0
   * @param[in] nextAutoRepeatingDelay in seconds
   * @pre nextAutoRepeatingDelay must be greater than zero.
   */
  void SetNextAutoRepeatingDelay( float nextAutoRepeatingDelay ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 GetProperty NEXT_AUTO_REPEATING_DELAY
   *
   * @brief Gets the next autorepeating delay in seconds.
   * @SINCE_1_0.0
   * @return The next autorepeating delay in seconds
   */
  float GetNextAutoRepeatingDelay() const DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 SetProperty TOGGLABLE or Styling file
   *
   * @brief Sets the \e togglable property.
   *
   * If the \e togglable property is set to \e true, then the \e autorepeating property is set to false.
   *
   * @SINCE_1_0.0
   * @param[in] togglable Togglable property
   */
  void SetTogglableButton( bool togglable ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 GetProperty TOGGLABLE
   *
   * @brief Returns if the togglable property is set.
   * @SINCE_1_0.0
   * @return \e true if the \e togglable property is set
   */
  bool IsTogglableButton() const DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 SetProperty SELECTED
   *
   * @brief Sets the button as selected or unselected.
   *
   * \e togglable property must be set to \e true.
   *
   * Emits a Button::StateChangedSignal() signal.
   *
   * @SINCE_1_0.0
   * @param[in] selected Selected property
   */
  void SetSelected( bool selected ) DALI_DEPRECATED_API;

  /**
   * DEPRECATED_1_1.32  GetProperty SELECTED
   *
   * @brief Returns if the selected property is set and the button is togglable.
   * @SINCE_1_0.0
   * @return \e true if the button is \e selected
   */
  bool IsSelected() const DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 Use Styling file to set animation
   *
   * @brief Sets the animation time.
   *
   * @SINCE_1_0.0
   * @param[in] animationTime The animation time in seconds
   */
  void SetAnimationTime( float animationTime ) DALI_DEPRECATED_API;

  /**
   * DEPRECATED_1_1.32 Use Styling file to set animation
   *
   * @brief Retrieves button's animation time.
   *
   * @SINCE_1_0.0
   * @return The animation time in seconds
   */
  float GetAnimationTime() const DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 SetProperty Property::LABEL or Styling file
   *
   * @brief Sets the button's label.
   *
   * @SINCE_1_0.0
   * @param[in] label The label text
   */
  void SetLabelText( const std::string& label ) DALI_DEPRECATED_API;

  /**
   * DEPRECATED_1_1.32 GetProperty LABEL
   *
   * @brief Gets the label.
   *
   * @SINCE_1_0.0
   * @return The label text
   */
  std::string GetLabelText() const DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 Use Styling file Property::UNSELECTED_STATE_IMAGE
   *
   * @brief Sets the unselected button image.
   *
   * @SINCE_1_0.0
   * @param[in] filename The button image
   */
  void SetUnselectedImage( const std::string& filename ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 Use styling Set Control::Property::BACKGROUND which is same as un-selected background
   * Or use DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL
   *
   * @brief Sets the background image.
   *
   * @SINCE_1_0.0
   * @param[in] filename The background image
   */
  void SetBackgroundImage( const std::string& filename ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 Use styling file, Property::SELECTED_STATE_IMAGE
   *
   * @brief Sets the selected image.
   *
   * @SINCE_1_0.0
   * @param[in] filename The selected image
   */
  void SetSelectedImage( const std::string& filename ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 Set Control::Property::BACKGROUND which is same as un-selected background
   * Or use DevelButton::Property::SELECTED_BACKGROUND_VISUAL
   *
   * @brief Sets the selected background image.
   *
   * @SINCE_1_0.0
   * @param[in] filename The selected background image
   */
  void SetSelectedBackgroundImage( const std::string& filename ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 Only able to set Property::DISABLED_STATE_IMAGE
   * Or use DevelButton::Property::DISABLED_SELECTED_BACKGROUND_VISUAL
   *
   * @brief Sets the disabled background image.
   *
   * @SINCE_1_0.0
   * @param[in] filename The disabled background image
   */
  void SetDisabledBackgroundImage( const std::string& filename ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 Use styling file Property::DISABLED_STATE_IMAGE
   *
   * @brief Sets the disabled button image.
   *
   * @SINCE_1_0.0
   * @param[in] filename The disabled button image
   */
  void SetDisabledImage( const std::string& filename ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_1.32 Use Property::DISABLED_STATE_IMAGE which provides same disabled image for un/selected
   * Or DevelButton::Property::DISABLED_SELECTED_VISUAL to disable selected visual.
   *
   * @brief Sets the disabled selected button image.
   *
   * @SINCE_1_0.0
   * @param[in] filename The disabled selected button image
   */
  void SetDisabledSelectedImage( const std::string& filename ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_0.50. Instead, use SetLabelText
   *
   * @brief Sets the label with an actor.
   *
   * @SINCE_1_0.0
   * @param[in] label The actor to use as a label
   */
  void SetLabel( Actor label ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_0.50. Instead, use SetUnselectedImage.
   *
   * @brief Sets the button image.
   *
   * @SINCE_1_0.0
   * @param[in] image The button image
   */
  void SetButtonImage( Image image ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_0.50. Instead, use SetSelectedImage( const std::string& filename ).
   *
   * @brief Sets the selected image.
   *
   * @SINCE_1_0.0
   * @param[in] image The selected image
   */
  void SetSelectedImage( Image image ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_0.50
   *
   * @brief Gets the button image.
   *
   * @SINCE_1_0.0
   * @remarks Avoid using this method as it's a legacy code.
   * @return An actor with the button image
   */
  Actor GetButtonImage() const DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_0.50
   *
   * @brief Gets the selected image.
   *
   * @SINCE_1_0.0
   * @remarks Avoid using this method as it's a legacy code.
   * @return An actor with the selected image
   */
  Actor GetSelectedImage() const DALI_DEPRECATED_API;

public: //Signals

  /**
   * @brief Button signal type.
   * @SINCE_1_0.0
   */
  typedef Signal< bool ( Button ) > ButtonSignalType;

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
  DALI_INTERNAL Button( Internal::Button& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @SINCE_1_0.0
   * @param[in] internal A pointer to the internal CustomActor
   */
  DALI_INTERNAL Button( Dali::Internal::CustomActor* internal );
  /// @endcond
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_BUTTON_H__
