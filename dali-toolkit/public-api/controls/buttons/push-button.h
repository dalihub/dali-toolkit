#ifndef DALI_TOOLKIT_PUSH_BUTTON_H
#define DALI_TOOLKIT_PUSH_BUTTON_H

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
 * @addtogroup dali_toolkit_controls_buttons
 * @{
 */

/**
 * @brief A PushButton changes its appearance when is pressed and returns to its original when is released.
 *
 * By default, a PushButton emits a Button::PressedSignal() signal when the button is pressed, a Button::ClickedSignal() signal when it's clicked.
 * and a Button::ReleasedSignal() signal when it's released or having pressed it, the touch point leaves the boundary of the button.
 *
 * Usage example: -
 *
 * @code
 * // in Creating a DALi Application
 * void HelloWorldExample::Create( Application& application )
 * {
 *   PushButton button = PushButton::New();
 *   button.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
 *   button.SetProperty( Button::Property::LABEL, "Press" );
 *   Stage::GetCurrent().Add( button );
 *
 *   // Connect to button signals emitted by the button
 *   button.ClickedSignal().Connect( this, &HelloWorldExample::OnButtonClicked );
 *   button.PressedSignal().Connect( this, &HelloWorldExample::OnButtonPressed );
 *   button.ReleasedSignal().Connect( this, &HelloWorldExample::OnButtonReleased );
 * }
 *
 * bool HelloWorldExample::OnButtonClicked( Button button )
 * {
 *   // Do something when the button is clicked
 *   return true;
 * }
 *
 * bool HelloWorldExample::OnButtonPressed( Button button )
 * {
 *   // Do something when the button is pressed
 *   return true;
 * }
 *
 * bool HelloWorldExample::OnButtonReleased( Button button )
 * {
 *   // Do something when the button is released
 *   return true;
 * }
 * @endcode
 *
 * See Button for more details on signals and modifying appearance via properties.
 * @SINCE_1_0.0
 */
class DALI_TOOLKIT_API PushButton : public Button
{
public:

  /**
   * @brief Enumeration for the start and end property ranges for this control.
   * @SINCE_1_0.0
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Button::PROPERTY_END_INDEX + 1,          ///< @SINCE_1_0.0
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserving 1000 property indices @SINCE_1_0.0
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the PushButton class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    /**
     * @brief Enumeration for the instance of properties belonging to the PushButton class.
     * @SINCE_1_0.0
     */
    enum
    {
      LABEL_PADDING = PROPERTY_START_INDEX,   ///< Property, name "labelPadding",    type Vector4 @SINCE_1_0.0
      ICON_PADDING,                           ///< Property, name "iconPadding",     type Vector4 @SINCE_1_0.0
    };
  };

public:

  /**
   * @brief Creates an uninitialized PushButton; this can be initialized with PushButton::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   * @SINCE_1_0.0
   */
  PushButton();

  /**
   * @brief Copy constructor.
   * @SINCE_1_0.0
   * @param[in] pushButton Handle to an object
   */
  PushButton( const PushButton& pushButton );

  /**
   * @brief Move constructor
   * @SINCE_1_9.23
   *
   * @param[in] rhs A reference to the moved handle
   */
  PushButton( PushButton&& rhs );

  /**
   * @brief Assignment operator.
   * @SINCE_1_0.0
   * @param[in] pushButton Handle to an object
   * @return A reference to this
   */
  PushButton& operator=( const PushButton& pushButton );

  /**
   * @brief Move assignment
   * @SINCE_1_9.23
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  PushButton& operator=( PushButton&& rhs );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~PushButton();

  /**
   * @brief Creates an initialized PushButton.
   *
   * @SINCE_1_0.0
   * @return A handle to a newly allocated Dali resource
   */
  static PushButton New();

  /**
   * @brief Downcasts a handle to PushButton handle.
   *
   * If handle points to a PushButton, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return handle to a PushButton or an uninitialized handle
   */
  static PushButton DownCast( BaseHandle handle );

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @SINCE_1_0.0
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL PushButton( Internal::PushButton& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @SINCE_1_0.0
   * @param[in] internal A pointer to the internal CustomActor
   */
  DALI_INTERNAL PushButton( Dali::Internal::CustomActor* internal );
  /// @endcond
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_PUSH_BUTTON_H
