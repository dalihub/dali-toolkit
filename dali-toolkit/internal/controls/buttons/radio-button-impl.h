#ifndef DALI_TOOLKIT_INTERNAL_RADIO_BUTTON_H
#define DALI_TOOLKIT_INTERNAL_RADIO_BUTTON_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/buttons/radio-button.h>
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include "button-impl.h"

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

/**
 * RadioButton implementation class.
 *
 * \sa Dali::Toolkit::RadioButton
 */
class RadioButton: public Button
{
public:

  /**
   * Create a new RadioButton.
   *
   * @return A smart-pointer to the newly allocated RadioButton.
   */
  static Dali::Toolkit::RadioButton New();

private:

  /**
   * Construct a new RadioButton.
   */
  RadioButton();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~RadioButton();

private: // From Button

  /**
   * @copydoc Toolkit::Internal::Button::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Toolkit::Internal::Button::OnStateChange
   */
  void OnStateChange( State newState ) override;

  /**
   * @copydoc Toolkit::Internal::Button::OnToggleReleased
   */
  bool OnToggleReleased() override;

private:

  // Undefined
  RadioButton( const RadioButton& origin );

  // Undefined
  RadioButton& operator=( const RadioButton& origin );

protected:
  struct AccessibleImpl : public Button::AccessibleImpl
  {
    using Button::AccessibleImpl::AccessibleImpl;

    Dali::Accessibility::States CalculateStates() override;
  };
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::RadioButton& GetImplementation(Toolkit::RadioButton& button)
{
  DALI_ASSERT_ALWAYS(button);

  Dali::RefObject& handle = button.GetImplementation();

  return static_cast<Toolkit::Internal::RadioButton&> (handle);
}

inline const Toolkit::Internal::RadioButton& GetImplementation(const Toolkit::RadioButton& button)
{
  DALI_ASSERT_ALWAYS(button);

  const Dali::RefObject& handle = button.GetImplementation();

  return static_cast<const Toolkit::Internal::RadioButton&> (handle);
}

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_RADIO_BUTTON_H
