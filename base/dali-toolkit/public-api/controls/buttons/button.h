#ifndef __DALI_TOOLKIT_BUTTON_H__
#define __DALI_TOOLKIT_BUTTON_H__

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
 * A ClickedSignal() is emitted when the button is touched and the touch
 * point doesn't leave the boundary of the button.
 *
 * When the \e disabled property is set to \e true, no signal is emitted.
 */
class DALI_IMPORT_API Button : public Control
{
public:

  // Signal Names
  static const char* const SIGNAL_CLICKED;       ///< name "clicked"
  static const char* const SIGNAL_STATE_CHANGED; ///< name "state-changed"

  // Properties
  static const Property::Index PROPERTY_DISABLED;                     ///< name "disabled",                     @see SetDisabled(),                  type BOOLEAN
  static const Property::Index PROPERTY_AUTO_REPEATING;               ///< name "auto-repeating",               @see SetAutoRepeating(),             type BOOLEAN
  static const Property::Index PROPERTY_INITIAL_AUTO_REPEATING_DELAY; ///< name "initial-auto-repeating-delay", @see SetInitialAutoRepeatingDelay(), type FLOAT
  static const Property::Index PROPERTY_NEXT_AUTO_REPEATING_DELAY;    ///< name "next-auto-repeating-delay",    @see SetNextAutoRepeatingDelay(),    type FLOAT
  static const Property::Index PROPERTY_TOGGLABLE;                    ///< name "togglable",                    @see SetToggleButton(),              type BOOLEAN
  static const Property::Index PROPERTY_TOGGLED;                      ///< name "toggled",                      @see SetToggled(),                   type BOOLEAN
  static const Property::Index PROPERTY_NORMAL_STATE_ACTOR;           ///< name "button-state-actor",           @see SetButtonImage(),               type MAP
  static const Property::Index PROPERTY_SELECTED_STATE_ACTOR;         ///< name "selected-state-actor",         @see SetSelectedImage(),             type MAP
  static const Property::Index PROPERTY_DISABLED_STATE_ACTOR;         ///< name "disabled-state-actor",         @see SetDisabledImage(),             type MAP
  static const Property::Index PROPERTY_LABEL_ACTOR;                  ///< name "label-actor",                  @see SetLabel(),                     type MAP

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
   * @brief Sets the animation time.
   *
   * @param [in] animationTime The animation time in seconds.
   */
  void SetAnimationTime( float animationTime );

  /**
   * @brief Retrieves button's animation time.
   *
   * @return The animation time in seconds.
   */
  float GetAnimationTime() const;

public: //Signals

  /**
   * @brief Button Clicked signal type
   */
  typedef Signal< bool ( Button ) > ClickedSignalType;

  /**
   * @brief Button state changed signal type
   */
  typedef Signal< bool ( Button, bool ) > StateChangedSignalType;

  /**
   * @brief Signal emitted when the button is touched and the touch point doesn't leave the boundary of the button.
   */
  ClickedSignalType& ClickedSignal();

  /**
   * @brief Signal emitted when the button's state is changed.
   */
  StateChangedSignalType& StateChangedSignal();

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
