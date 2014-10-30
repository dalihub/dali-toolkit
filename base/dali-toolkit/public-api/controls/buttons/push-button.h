#ifndef __DALI_TOOLKIT_PUSH_BUTTON_H__
#define __DALI_TOOLKIT_PUSH_BUTTON_H__

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
#include "button.h"

namespace Dali
{

namespace Toolkit
{

// Forward declarations

namespace Internal DALI_INTERNAL
{
// Forward declarations

class PushButton;
}

/**
 * @brief A PushButton changes its appearance when is pressed and returns to its original when is released.
 *
 * By default a PushButton emits a PushButton::PressedSignal() signal when the button is pressed, a Button::ClickedSignal() signal when it's clicked
 * and a PushButton::ReleasedSignal() signal when it's released or having pressed it, the touch point leaves the boundary of the button.
 *
 * PushButton provides the following properties which modify signals emitted:
 * <ul>
 *   <li>\e autorepeating
 *
 *       When \e autorepeating is set to \e true, a PushButton::PressedSignal(), PushButton::ReleasedSignal() and Button::ClickedSignal() signals are emitted at regular
 *       intervals while the button is touched.
 *
 *       The intervals could be modified with the PushButton::SetInitialAutoRepeatingDelay and PushButton::SetNextAutoRepeatingDelay methods.
 *
 *       A \e toggle button can't be \e autorepeating. If the \e autorepeating property is set to \e true, then the \e toggle property is set to
 *       false but no signal is emitted.
 *
 *   <li>\e toggle
 *
 *       When \e toggle is set to \e true, a PushButton::ToggledSignal() signal is emitted, with the toggle state, every time the button is touched instead
 *       of emit PushButton::PressedSignal(), Button::ClickedSignal() and PushButton::ReleasedSignal() signals.
 *
 *       An \e autorepeating button can't be \e toggle. If the \e toggle property is set to \e true, then the \e autorepeating property is set to false.
 * </ul>
 *
 * The button's appearance could be modified by setting images or actors with PushButton::SetButtonImage, PushButton::SetBackgroundImage,
 * PushButton::SetPressedImage, PushButton::SetDimmedBackgroundImage and  PushButton::SetDimmedImage or setting a text with
 * PushButton::SetLabelText.
 *
 * The \e background is always shown and doesn't change if the button is pressed or released. The \e button image is shown over the \e background image when the
 * button is not pressed and is replaced by the \e pressed image when the button is pressed. The text label is placed always on the top of all images.
 *
 * When the button is dimmed, \e background, \e button and \e pressed images are replaced by their \e dimmed images.
 *
 * The methods used to modify the button's appearance could receive Dali::Actor objects as a parameter, so more complex images could be defined.
 *
 * Is not mandatory set all images. A button could be defined only by setting its \e background image or by setting its \e background and \e pressed images.
 */
class DALI_IMPORT_API PushButton : public Button
{
public:

  //Signal Names
  static const char* const SIGNAL_PRESSED; ///< name "pressed"
  static const char* const SIGNAL_RELEASED; ///< name "released"

  //Action Names
  static const char* const ACTION_PUSH_BUTTON_CLICK; ///< name "push-button-click"

  // Properties
  static const Property::Index PROPERTY_AUTO_REPEATING;               ///< name "auto-repeating",               @see SetAutoRepeating(),             type BOOLEAN
  static const Property::Index PROPERTY_INITIAL_AUTO_REPEATING_DELAY; ///< name "initial-auto-repeating-delay", @see SetInitialAutoRepeatingDelay(), type FLOAT
  static const Property::Index PROPERTY_NEXT_AUTO_REPEATING_DELAY;    ///< name "next-auto-repeating-delay",    @see SetNextAutoRepeatingDelay(),    type FLOAT
  static const Property::Index PROPERTY_TOGGLABLE;                    ///< name "togglable",                    @see SetToggleButton(),              type BOOLEAN
  static const Property::Index PROPERTY_TOGGLE;                       ///< name "toggle",                       @see SetToggled(),                   type BOOLEAN
  static const Property::Index PROPERTY_NORMAL_STATE_ACTOR;           ///< name "button-state-actor",           @see SetButtonImage(),               type MAP
  static const Property::Index PROPERTY_PRESSED_STATE_ACTOR;          ///< name "pressed-state-actor",          @see SetPressedImage(),              type MAP
  static const Property::Index PROPERTY_DIMMED_STATE_ACTOR;           ///< name "dimmed-state-actor",           @see SetDimmedImage(),               type MAP
  static const Property::Index PROPERTY_LABEL_ACTOR;                  ///< name "label-actor",                  @see SetLabelText(),                 type STRING

public:

  /**
   * @brief Create an uninitialized PushButton; this can be initialized with PushButton::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  PushButton();

  /**
   * @brief Copy constructor.
   */
  PushButton( const PushButton& pushButton );

  /**
   * @brief Assignment operator.
   */
  PushButton& operator=( const PushButton& pushButton );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~PushButton();

  /**
   * @brief Create an initialized PushButton.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static PushButton New();

  /**
   * @brief Downcast an Object handle to PushButton.
   *
   * If handle points to a PushButton the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return handle to a PushButton or an uninitialized handle
   */
  static PushButton DownCast( BaseHandle handle );

  /**
   * @brief Sets the \e autorepeating property.
   *
   * If the \e autorepeating property is set to \e true, then the \e toggle property is set to false
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
   * @brief Sets the \e toggle property.
   *
   * If the \e toggle property is set to \e true, then the \e autorepeating property is set to false.
   *
   * @param[in] toggle property.
   */
  void SetToggleButton( bool toggle );

  /**
   * @return \e true if the \e toggle property is set.
   */
  bool IsToggleButton() const;

  /**
   * @brief Sets the button as toggled or not toggled.
   *
   * \e toggle property must be set to \e true.
   *
   * Emits a PushButton::ToggledSignal() signal.
   *
   * @param[in] toggle state.
   */
  void SetToggled( bool toggle );

  /**
   * @return \e true if the \e toggle property is set and the button is toggled.
   */
  bool IsToggled() const;

  /**
   * @brief Sets the button image.
   *
   * @param[in] image The button image.
   */
  void SetButtonImage( Image image );

  /**
   * @copydoc SetButtonImage( Image image )
   */
  void SetButtonImage( Actor image );

  /**
   * @brief Gets the button image.
   *
   * @return An actor with the button image.
   */
  Actor GetButtonImage() const;

  /**
   * @brief Sets the background image.
   *
   * @param[in] image The background image.
   */
  void SetBackgroundImage( Image image );

  /**
   * @copydoc SetBackgroundImage( Image image )
   */
  void SetBackgroundImage( Actor image );

  /**
   * @brief Gets the background image.
   *
   * @return An actor with the background image.
   */
  Actor GetBackgroundImage() const;

  /**
   * @brief Sets the pressed image.
   *
   * @param[in] image The pressed image.
   */
  void SetPressedImage( Image image );

  /**
   * @copydoc SetPressedImage( Image image )
   */
  void SetPressedImage( Actor image );

  /**
   * @brief Gets the pressed image.
   *
   * @return An actor with the pressed image.
   */
  Actor GetPressedImage() const;

  /**
   * @brief Sets the dimmed background image.
   *
   * @param[in] image The dimmed background image.
   */
  void SetDimmedBackgroundImage( Image image );

  /**
   * @copydoc SetDimmedBackgroundImage( Image image )
   */
  void SetDimmedBackgroundImage( Actor image );

  /**
   * @brief Gets the dimmed background image.
   *
   * @return An actor with the dimmed background image.
   */
  Actor GetDimmedBackgroundImage() const;

  /**
   * @brief Sets the dimmed button image.
   *
   * @param[in] image The dimmed button image.
   */
  void SetDimmedImage( Image image );

  /**
   * @copydoc SetDimmedImage( Image image )
   */
  void SetDimmedImage( Actor image );

  /**
   * @brief Gets the dimmed image.
   *
   * @return An actor with the dimmed image.
   */
  Actor GetDimmedImage() const;

  /**
   * @brief Sets the text label.
   *
   * @param[in] text Label text.
   */
  void SetLabelText( const std::string& text );

  /**
   * @copydoc SetLabelText( const std::string& text )
   */
  void SetLabelText( Actor text );

  /**
   * @brief Gets the label text.
   *
   * @return An actor with the label text.
   */
  Actor GetLabelText() const;

public: //Signals

  /// @brief PushButton Pressed signal type.
  typedef SignalV2< bool ( Button ) > PressedSignalV2;

  /// @brief PushButton Released signal type.
  typedef SignalV2< bool ( Button ) > ReleasedSignalV2;

  /**
   * @brief Signal emitted when the button is touched.
   */
  PressedSignalV2& PressedSignal();

  /**
   * @brief Signal emitted when the button is touched and the touch point leaves the boundary of the button.
   */
  ReleasedSignalV2& ReleasedSignal();

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL PushButton( Internal::PushButton& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  DALI_INTERNAL PushButton( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_PUSH_BUTTON_H__
