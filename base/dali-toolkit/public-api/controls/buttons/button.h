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

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class CustomActor;
}

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class Button;
}

/**
 * @brief Button is a base class for different kind of buttons.
 *
 * This class provides the dimmed property and the clicked signal.
 *
 * A ClickedSignal() is emitted when the button is touched and the touch
 * point doesn't leave the boundary of the button.
 *
 * When the \e dimmed property is set to \e true, no signal is emitted.
 */
class Button : public Control
{
public:

  // Signal Names
  static const char* const SIGNAL_CLICKED; ///< name "clicked"
  static const char* const SIGNAL_TOGGLED; ///< name "toggled"

  // Properties
  static const Property::Index PROPERTY_DIMMED; ///< name "dimmed", @see SetDimmed(), type BOOLEAN

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
   * @brief Sets the button as \e dimmed.
   *
   * No signals are emitted when the \e dimmed property is set.
   *
   * @param[in] dimmed property.
   */
  void SetDimmed( bool dimmed );

  /**
   * @return \e true if the button is \e dimmed.
   */
  bool IsDimmed() const;

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
  typedef SignalV2< bool ( Button ) > ClickedSignalV2;

  /**
   * @brief Button toggled signal type
   */
  typedef SignalV2< bool ( Button, bool ) > ToggledSignalV2;

  /**
   * @brief Signal emitted when the button is touched and the touch point doesn't leave the boundary of the button.
   */
  ClickedSignalV2& ClickedSignal();

  /**
   * @brief Signal emitted when the button's state is toggled.
   */
  ToggledSignalV2& ToggledSignal();

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  Button( Internal::Button& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  Button( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_BUTTON_H__
