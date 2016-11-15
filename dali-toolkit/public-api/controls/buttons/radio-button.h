#ifndef __DALI_TOOLKIT_RADIO_BUTTON_H__
#define __DALI_TOOLKIT_RADIO_BUTTON_H__

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
#include "button.h"

namespace Dali
{

namespace Toolkit
{

// Forward declarations

namespace Internal DALI_INTERNAL
{
// Forward declarations

class RadioButton;
}
/**
 * @addtogroup dali_toolkit_controls_buttons
 * @{
 */

/**
 * @brief A RadioButton provides a radio button which two states \e selected or \e unselected.
 *
 * Radio buttons are designed to select one of many option at the same time.
 *
 * Every button have its own \e label and \e state, which can be modified by Button::SetLabelText and Button::SetSelected.
 *
 * RadioButton can change its current state using Button::SetSelected.
 *
 * RadioButtons can be grouped.
 * Two or more RadioButtons are in one group when they have this same parent.
 * In each groups only one RadioButton can be \e selected at a given time.
 * So when RadioButton is set to \e selected, other RadioButtons in its group are set to \e unselected.
 * When \e selected RadioButton is set to \e unselected no other RadioButtons in his group is set to \e selected.
 *
 * A Button::StateChangedSignal() is emitted when the RadioButton change its state to \e selected or \e unselected.
 *
 * Usage example: -
 *
 * @code
 * // in Creating a DALi Application
 *
 * // Create a group to bind two or more RadioButtons together
 * Actor radioGroup = Actor::New();
 * radioGroup.SetParentOrigin( ParentOrigin::CENTER );
 * Stage::GetCurrent().Add( radioGroup );
 *
 * // Make the first RadioButton and add it to its parent
 * RadioButton button1 = RadioButton::New();
 * button1.SetLabelText( "button1" );
 * button1.SetBackgroundColor( Color::WHITE );
 * button1.SetPosition( 0, -40 );
 * radioGroup.Add( button1 );
 *
 * // Make more RadioButtons and add them to their parent
 * RadioButton button2 = RadioButton::New();
 * button2.SetLabelText( "button2" );
 * button2.SetBackgroundColor( Color::WHITE );
 * button2.SetPosition( 0, 40 );
 * radioGroup.Add( button2 );
 *
 * @endcode
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API RadioButton: public Button
{
 public:
  /**
   * @brief Create an uninitialized RadioButton; this can be initialized with RadioButton::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   * @SINCE_1_0.0
   */
  RadioButton();

  /**
   * @brief Copy constructor.
   * @SINCE_1_0.0
   * @param[in] radioButton Handle to an object
   */
  RadioButton( const RadioButton& radioButton );

  /**
   * @brief Assignment operator.
   * @SINCE_1_0.0
   * @param[in] radioButton Handle to an object
   * @return A reference to this
   */
  RadioButton& operator=( const RadioButton& radioButton );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~RadioButton();

  /**
   * @brief Create an initialized RadioButton.
   *
   * @SINCE_1_0.0
   * @return A handle to a newly allocated Dali resource.
   */
  static RadioButton New();

  /**
   * @brief Create an initialized RadioButton with given label.
   *
   * @SINCE_1_0.0
   * @param[in] label The button label.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static RadioButton New( const std::string& label );

  /**
   * @brief Downcast a handle to RadioButton handle.
   *
   * If handle points to a RadioButton the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return A handle to a RadioButton or an uninitialized handle
   */
  static RadioButton DownCast( BaseHandle handle );

 public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @SINCE_1_0.0
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL RadioButton( Internal::RadioButton& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @SINCE_1_0.0
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  DALI_INTERNAL RadioButton( Dali::Internal::CustomActor* internal );
  /// @endcond
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_RADIO_BUTTON_H__
