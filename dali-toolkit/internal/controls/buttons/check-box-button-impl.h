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
#include <dali/public-api/shader-effects/shader-effect.h>


// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/buttons/check-box-button.h>
#include <dali-toolkit/devel-api/shader-effects/image-region-effect.h>

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
   * @copydoc Toolkit::Internal::Button::OnButtonInitialize()
   */
  virtual void OnButtonInitialize();

  /**
   * @copydoc Toolkit::Internal::Button::OnLabelSet()
   */
  virtual void OnLabelSet();

  /**
   * @copydoc Toolkit::Internal::Button::OnSelected()
   */
  virtual bool OnSelected();

  /**
   * @copydoc Toolkit::Internal::Button::OnDisabled()
   */
  virtual bool OnDisabled();

  /**
   * @copydoc Toolkit::Internal::Button::StopAllAnimations()
   */
  virtual void StopAllAnimations();

private:

  /**
   * Adds the actor to the transition animation.
   * It creates a transition animation if needed and starts the animation.
   * @param[in] actor The actor.
   */
  void StartTransitionAnimation( Actor& actor );

  /**
   * Stops the transition animation.
   * @param[in] remove If true, removes the fadeout actor from root.
   */
  void StopTransitionAnimation( bool remove = true );

  // slots

  /**
   * Called when the transition animation finishes.
   */
  void TransitionAnimationFinished( Dali::Animation& source );

private:

  // Undefined
  CheckBoxButton( const CheckBoxButton& );

  // Undefined
  CheckBoxButton& operator=( const CheckBoxButton& );

private:
  Animation                 mTransitionAnimation;  ///< Animation used in the state transitions.
  ShaderEffect              mTickUVEffect;         ///< ImageRegionEffect to expand the tick across
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
