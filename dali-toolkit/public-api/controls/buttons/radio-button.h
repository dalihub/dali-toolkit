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
 * @brief A RadioButton provides a radio button which two states \e selected or \e unselected.
 *
 * Radio buttons are designed to select one of many option at the same time.
 *
 * Every button have its own \e label and \e state, which can be modified by RadioButton::SetLabel and Button::SetSelected.
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
 */
class DALI_IMPORT_API RadioButton: public Button
{
 public:
  /**
   * @brief Create an uninitialized RadioButton; this can be initialized with RadioButton::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  RadioButton();

  /**
   * @brief Copy constructor.
   */
  RadioButton( const RadioButton& radioButton );

  /**
   * @brief Assignment operator.
   */
  RadioButton& operator=( const RadioButton& radioButton );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~RadioButton();

  /**
   * @brief Create an initialized RadioButton.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static RadioButton New();

  /**
   * @brief Create an initialized RadioButton with given label.
   *
   * @param[in] label The button label.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static RadioButton New( const std::string& label );

  /**
   * @brief Create an initialized RadioButton with existing Actor.
   *
   * @param[in] label An Actor with the label.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static RadioButton New( Actor label );

  /**
   * @brief Downcast an Object handle to RadioButton.
   *
   * If handle points to a RadioButton the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return handle to a RadioButton or an uninitialized handle
   */
  static RadioButton DownCast( BaseHandle handle );

 public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL RadioButton( Internal::RadioButton& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  DALI_INTERNAL RadioButton( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_RADIO_BUTTON_H__
