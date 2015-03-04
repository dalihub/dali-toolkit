#ifndef __DALI_TOOLKIT_INTERNAL_CHECK_BOX_BUTTON_H__
#define __DALI_TOOLKIT_INTERNAL_CHECK_BOX_BUTTON_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/animation/animation.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/buttons/check-box-button.h>
#include <dali-toolkit/public-api/shader-effects/image-region-effect.h>

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

public: // From Button

  /**
   * @copydoc Toolkit::Internal::Button::SetSelectedImage( Actor image )
   */
  virtual void SetSelectedImage( Actor image );

  /**
   * @copydoc Toolkit::Internal::Button::SetBackgroundImage( Actor image )
   */
  virtual void SetBackgroundImage( Actor image );

  /**
   * @copydoc Toolkit::Internal::Button::SetDisabledSelectedImage( Actor image )
   */
  virtual void SetDisabledSelectedImage( Actor image );

  /**
   * @copydoc Toolkit::Internal::Button::SetDisabledBackgroundImage( Actor image )
   */
  virtual void SetDisabledBackgroundImage( Actor image );

private: // From Button

  /**
   * @copydoc Toolkit::Internal::Button::OnLabelSet()
   */
  virtual void OnLabelSet();

  /**
   * @copydoc Toolkit::Internal::Button::OnSelected()
   */
  virtual void OnSelected( bool selected );

  /**
   * @copydoc Toolkit::Internal::Button::OnDisabled( bool disabled )
   */
  virtual void OnDisabled( bool disabled );

private: // From Control

  /**
   * @copydoc Dali::Toolkit::Control::OnRelayout()
   */
  virtual void OnRelayout( const Vector2& size, ActorSizeContainer& container );

private:

  /**
   * Construct a new CheckBoxButton.
   */
  CheckBoxButton();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~CheckBoxButton();

private:

  /**
   * Adds the actor to the button.
   */
  void AddChild( Actor& actor );

  /**
   * Removes the actor from the button.
   */
  void RemoveChild( Actor& actor );

  /**
   * Adds the actor to the check in animation.
   * It creates a check in animation if needed and starts the check in animation.
   * @param[in] actor The actor.
   */
  void StartCheckInAnimation( Actor& actor );

  /**
   * Stops the check in animation.
   */
  void StopCheckInAnimation();

  // slots

  /**
   * Called when the check in animation finishes.
   * It changes the check button paint state.
   */
  void CheckInAnimationFinished( Dali::Animation& source );

private:

  // Undefined
  CheckBoxButton( const CheckBoxButton& );

  // Undefined
  CheckBoxButton& operator=( const CheckBoxButton& );

private:
  Animation                 mCheckInAnimation;  ///< Animation used in the state transitions.
  ImageRegionEffect         mTickUVEffect;      ///< ImageRegionEffect to expand the tick across

  PaintState                mPaintState;        ///< The paint state.
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
