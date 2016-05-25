#ifndef __DALI_TOOLKIT_INTERNAL_CHECK_BOX_BUTTON_H__
#define __DALI_TOOLKIT_INTERNAL_CHECK_BOX_BUTTON_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/buttons/check-box-button.h>

#include "button-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * CheckBoxButton implementation class.
 *
 * \sa Dali::Toolkit::CheckBoxButton
 */
class CheckBoxButton : public Button
{
public:

  /**
   * Create a new CheckBoxButton.
   * @return A smart-pointer to the newly allocated CheckBoxButton.
   */
  static Dali::Toolkit::CheckBoxButton New();

private:

  /**
   * Construct a new CheckBoxButton.
   */
  CheckBoxButton();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~CheckBoxButton();

private: // From Button


  /**
   * @copydoc Toolkit::Internal::Button::OnInitialize
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::Internal::Button::PrepareForTranstionIn( Actor actor )
   */
  virtual void PrepareForTransitionIn( Actor actor );

  /**
   * @copydoc Toolkit::Internal::Button::PrepareForTranstionOut( Actor actor )
   */
  virtual void PrepareForTransitionOut( Actor actor );

  /**
   * @copydoc Toolkit::Internal::Button::OnTransitionInImage( Actor actor )
   */
  virtual void OnTransitionIn( Actor actor );

  /**
   * @copydoc Toolkit::Internal::Button::OnTransitionOut( Actor actor )
   */
  virtual void OnTransitionOut( Actor actor );

  /**
   * @copydoc Toolkit::Internal::Button::OnButtonVisualRemoval( Property::Index visualIndex )
   */
 // virtual void OnButtonVisualRemoval( Property::Index visualIndex );

private:

  // Undefined
  CheckBoxButton( const CheckBoxButton& );

  // Undefined
  CheckBoxButton& operator=( const CheckBoxButton& );

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::CheckBoxButton& GetImplementation( Toolkit::CheckBoxButton& button )
{
  DALI_ASSERT_ALWAYS( button );

  Dali::RefObject& handle = button.GetImplementation();

  return static_cast<Toolkit::Internal::CheckBoxButton&>( handle );
}

inline const Toolkit::Internal::CheckBoxButton& GetImplementation( const Toolkit::CheckBoxButton& button )
{
  DALI_ASSERT_ALWAYS( button );

  const Dali::RefObject& handle = button.GetImplementation();

  return static_cast<const Toolkit::Internal::CheckBoxButton&>( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_CHECK_BOX_BUTTON_H__
