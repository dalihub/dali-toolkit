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
 * @brief Button is a base class for different kind of buttons.
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
 * The button's appearance can be modified by setting properties for the various image filenames.
 *
 * The \e background is always shown and doesn't change if the button is pressed or released. The \e button image is shown over the \e background image when the
 * button is not pressed and is replaced by the \e selected image when the button is pressed. The text label is placed always on the top of all images.
 *
 * When the button is disabled, \e background, \e button and \e selected images are replaced by their \e disabled images.
 *
 * Is not mandatory set all images. A button could be defined only by setting its \e background image or by setting its \e background and \e selected images.
 *
 * Signals
 * | %Signal Name      | Method                      |
 * |-------------------|-----------------------------|
 * | pressed           | @ref PressedSignal()        |
 * | released          | @ref ReleasedSignal()       |
 * | clicked           | @ref ClickedSignal()        |
 * | state-changed     | @ref StateChangedSignal()   |
 *
 * Actions
 * | %Action Name      | %Button method called       |
 * |-------------------|-----------------------------|
 * | button-click      | %DoClickAction()            |
 */
class DALI_IMPORT_API Button : public Control
{
public:

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the Button class.
   */
  struct Property
  {
    enum
    {
      DISABLED = PROPERTY_START_INDEX, ///< name "disabled",                     @see SetDisabled(),                  type bool
      AUTO_REPEATING,                  ///< name "auto-repeating",               @see SetAutoRepeating(),             type bool
      INITIAL_AUTO_REPEATING_DELAY,    ///< name "initial-auto-repeating-delay", @see SetInitialAutoRepeatingDelay(), type float
      NEXT_AUTO_REPEATING_DELAY,       ///< name "next-auto-repeating-delay",    @see SetNextAutoRepeatingDelay(),    type float
      TOGGLABLE,                       ///< name "togglable",                    @see SetTogglableButton(),           type bool
      SELECTED,                        ///< name "selected",                     @see SetSelected(),                  type bool
      UNSELECTED_STATE_IMAGE,          ///< name "unselected-state-image",       @see SetUnselectedImage(),           type std::string
      SELECTED_STATE_IMAGE,            ///< name "selected-state-image",         @see SetSelectedImage(),             type std::string
      DISABLED_STATE_IMAGE,            ///< name "disabled-state-image",         @see SetDisabledImage(),             type std::string
      UNSELECTED_COLOR,                ///< name "unselected-color",             @see SetUnselectedColor(),           type Vector4
      SELECTED_COLOR,                  ///< name "selected-color",               @see SetSelectedColor(),             type Vector4
      LABEL_TEXT,                      ///< name "label-text",                   @see SetLabelText(),                 type std::string
    };
  };

public:

  /**
   * @brief Create an uninitialized Button.
   *
   * Only derived versions can be instantiated.  Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  Button();

  /**
   * @brief Copy constructor.
   */
  Button( const Button& button );

  /**
   * @brief Assignment operator.
   */
  Button& operator=( const Button& button );

  /**
   * @brief Downcast an Object handle to Button.
   *
   * If handle points to a Button the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return handle to a Button or an uninitialized handle
   */
  static Button DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Button();

  /**
   * @brief Sets the button as \e disabled.
   *
   * No signals are emitted when the \e disabled property is set.
   *
   * @param[in] disabled property.
   */
  void SetDisabled( bool disabled );

  /**
   * @return \e true if the button is \e disabled.
   */
  bool IsDisabled() const;

  /**
   * @brief Sets the \e autorepeating property.
   *
   * If the \e autorepeating property is set to \e true, then the \e togglable property is set to false
   * but no signal is emitted.
   *
   * @param[in] autoRepeating \e autorepeating property.
   */
  void SetAutoRepeating( bool autoRepeating );

  /**
   * @return \e true if the \e autorepeating property is set.
   */
  bool IsAutoRepeating() const;

  /**
   * @brief Sets the initial autorepeating delay.
   *
   * By default this value is set to 0.15 seconds.
   *
   * @pre initialAutoRepeatingDelay must be greater than zero.
   * @param[in] initialAutoRepeatingDelay in seconds.
   */
  void SetInitialAutoRepeatingDelay( float initialAutoRepeatingDelay );

  /**
   * @return the initial autorepeating delay in seconds.
   */
  float GetInitialAutoRepeatingDelay() const;

  /**
   * @brief Sets the next autorepeating delay.
   *
   * By default this value is set to 0.05 seconds.
   *
   * @pre nextAutoRepeatingDelay must be greater than zero.
   * @param[in] nextAutoRepeatingDelay in seconds.
   */
  void SetNextAutoRepeatingDelay( float nextAutoRepeatingDelay );

  /**
   * @return the next autorepeating delay in seconds.
   */
  float GetNextAutoRepeatingDelay() const;

  /**
   * @brief Sets the \e togglable property.
   *
   * If the \e togglable property is set to \e true, then the \e autorepeating property is set to false.
   *
   * @param[in] togglable property.
   */
  void SetTogglableButton( bool togglable );

  /**
   * @return \e true if the \e togglable property is set.
   */
  bool IsTogglableButton() const;

  /**
   * Sets the button as selected or unselected.
   *
   * \e togglable property must be set to \e true.
   *
   * Emits a Button::StateChangedSignal() signal.
   *
   * @param[in] selected property.
   */
  void SetSelected( bool selected );

  /**
   * @return \e true if the \e selected property is set and the button is togglable.
   */
  bool IsSelected() const;

  /**
   * @brief Sets the animation time.
   *
   * @param[in] animationTime The animation time in seconds.
   */
  void SetAnimationTime( float animationTime );

  /**
   * @brief Retrieves button's animation time.
   *
   * @return The animation time in seconds.
   */
  float GetAnimationTime() const;

  /**
   * @brief Sets the button's label.
   *
   * @param[in] label The label text.
   */
  void SetLabelText( const std::string& label );

  /**
   * @brief Gets the label.
   *
   * @return The label text.
   */
  std::string GetLabelText() const;

  /**
   * @brief Sets the unselected button image.
   *
   * @param[in] filename The button image.
   */
  void SetUnselectedImage( const std::string& filename );

  /**
   * @brief Sets the background image.
   *
   * @param[in] filename The background image.
   */
  void SetBackgroundImage( const std::string& filename );

  /**
   * @brief Sets the selected image.
   *
   * @param[in] filename The selected image.
   */
  void SetSelectedImage( const std::string& filename );

  /**
   * @brief Sets the selected background image.
   *
   * @param[in] filename The selected background image.
   */
  void SetSelectedBackgroundImage( const std::string& filename );

  /**
   * @brief Sets the disabled background image.
   *
   * @param[in] filename The disabled background image.
   */
  void SetDisabledBackgroundImage( const std::string& filename );

  /**
   * @brief Sets the disabled button image.
   *
   * @param[in] filename The disabled button image.
   */
  void SetDisabledImage( const std::string& filename );

  /**
   * @brief Sets the disabled selected button image.
   *
   * @param[in] filename The disabled selected button image.
   */
  void SetDisabledSelectedImage( const std::string& filename );

  // Deprecated API

  /**
   * @deprecated Sets the label with an actor.
   * @param[in]  label The actor to use as a label
   */
  void SetLabel( Actor label );

  /**
   * @deprecated Sets the button image.
   * @param[in]  image The button image.
   */
  void SetButtonImage( Image image );

  /**
   * @deprecated Sets the selected image.
   * @param[in]  image The selected image.
   */
  void SetSelectedImage( Image image );

  /**
   * @deprecated Gets the button image.
   * @return     An actor with the button image.
   */
  Actor GetButtonImage() const;

  /**
   * @deprecated Gets the selected image.
   * @return     An actor with the selected image.
   */
  Actor GetSelectedImage() const;

public: //Signals

  /**
   * @brief Button signal type
   */
  typedef Signal< bool ( Button ) > ButtonSignalType;

  /**
   * @brief This signal is emitted when the button is touched.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( Button button );
   * @endcode
   * @return The signal to connect to.
   */
  ButtonSignalType& PressedSignal();

  /**
   * @brief This signal is emitted when the button is touched and the touch point leaves the boundary of the button.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( Button button );
   * @endcode
   * @return The signal to connect to.
   */
  ButtonSignalType& ReleasedSignal();

  /**
   * @brief This signal is emitted when the button is touched and the touch point doesn't leave the boundary of the button.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( Button button );
   * @endcode
   * @return The signal to connect to.
   */
  ButtonSignalType& ClickedSignal();

  /**
   * @brief This signal is emitted when the button's state is changed.
   * The application can get the state by calling IsSelected().
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( Button button );
   * @endcode
   * @return The signal to connect to.
   */
  ButtonSignalType& StateChangedSignal();

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL Button( Internal::Button& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  DALI_INTERNAL Button( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_BUTTON_H__
