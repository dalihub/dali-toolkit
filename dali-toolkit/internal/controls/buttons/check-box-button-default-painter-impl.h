#ifndef __DALI_TOOLKIT_INTERNAL_CHECK_BOX_BUTTON_DEFAULT_PAINTER_H__
#define __DALI_TOOLKIT_INTERNAL_CHECK_BOX_BUTTON_DEFAULT_PAINTER_H__

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
#include <dali/public-api/images/image.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/animation/animation.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/shader-effects/image-region-effect.h>
#include "check-box-button-painter-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

// Forward declarations

class CheckBoxButton;
class CheckBoxButtonDefaultPainter;

// Type definitions

typedef IntrusivePtr<CheckBoxButtonDefaultPainter> CheckBoxButtonDefaultPainterPtr;

/**
 * CheckBoxButtonDefaultPainter controls the Dali::Toolkit::CheckBoxButton appearance.
 *
 * This class inherits from Dali::Toolkit::Internal::CheckBoxButtonPainter and is registered in a
 * Dali::Toolkit::Internal::CheckBoxButton object in order to receive the state changes.
 */
class CheckBoxButtonDefaultPainter : public CheckBoxButtonPainter
{
public:
  /**
   * Constructor.
   *
   * Set actors and animations to NULL.
   */
  CheckBoxButtonDefaultPainter();

  /**
   * Destructor.
   *
   * It clears all fade in or fade out animations.
   */
  ~CheckBoxButtonDefaultPainter();

  /////////////////////////////////////////////////////////////////////////////
  // ButtonPainter interface
  /////////////////////////////////////////////////////////////////////////////

  /**
   * @copydoc ButtonPainter::Initialize( Toolkit::Button& button )
   */
  virtual void Initialize( Toolkit::Button& button );

  /**
   * @copydoc ButtonPainter::SetSize( Toolkit::Button& button, const Vector3& size )
   */
  virtual void SetSize( Toolkit::Button& button, const Vector3& size );

  /**
   * @copydoc ButtonPainter::SetDisabled( Toolkit::Button& button, bool disabled )
   */
  virtual void SetDisabled( Toolkit::Button& button, bool disabled );

  /**
   * @copydoc ButtonPainter::SetAnimationTime( float animationTime )
   */
  virtual void SetAnimationTime( float animationTime );

  /**
   * @copydoc ButtonPainter::GetAnimationTime()
   */
  virtual float GetAnimationTime() const;

  /**
   * @copydoc ButtonPainter::SetSelectedImage( Toolkit::Button& button, Actor image )
   */
  virtual void SetSelectedImage( Toolkit::Button& button, Actor image );

  /**
   * @copydoc ButtonPainter::SetBackgroundImage( Toolkit::Button& button, Actor image )
   */
  virtual void SetBackgroundImage( Toolkit::Button& button, Actor image );

  /**
   * @copydoc ButtonPainter::SetDisabledSelectedImage( Toolkit::Button& button, Actor image )
   */
  virtual void SetDisabledSelectedImage( Toolkit::Button& button, Actor image );

  /**
   * @copydoc ButtonPainter::SetDisabledBackgroundImage( Toolkit::Button& button, Actor image )
   */
  virtual void SetDisabledBackgroundImage( Toolkit::Button& button, Actor image );

  /////////////////////////////////////////////////////////////////////////////
  // CheckBoxButtonPainter interface
  /////////////////////////////////////////////////////////////////////////////

  /**
   * This method is called when the Dali::Toolkit::Internal::CheckBoxButton in which this object is registered
   * changes its state.
   *
   * @param[inout] checkBox The Dali::Toolkit::CheckBoxButton in which this object is registered.
   */
  void Selected( Toolkit::Button& checkBox );

private:

  // Undefined
  CheckBoxButtonDefaultPainter( const CheckBoxButtonDefaultPainter& );

  // Undefined
  CheckBoxButtonDefaultPainter& operator=( const CheckBoxButtonDefaultPainter& );

private:

  /**
   * Default check box button painter states.
   */
  enum PaintState
  {
    UnselectedState,              ///< The check box button is unselected.
    SelectedState,                ///< The check box button is selected.
    DisabledUnselectedState,      ///< The check box button is disabled and unselected.
    DisabledSelectedState,        ///< The check box button is disabled and selected.
    UnselectedSelectedTransition, ///< The check box button is in transition from unselected to selected.
    SelectedUnselectedTransition, ///< The check box button is in transition from selected to unselected.
    UnselectedDisabledTransition, ///< The check box button is in transition from unselected to disabled.
    DisabledUnselectedTransition, ///< The check box button is in transition from disabled to unselected.
    SelectedDisabledTransition,   ///< The check box button is in transition from selected to disabled.
    DisabledSelectedTransition    ///< The check box button is in transition from disabled to selected.
  };

  /**
   * Used in the FadeOut functions.
   */
  enum ImageLayer
  {
    Background, ///< Fade out the background.
    Foreground  ///< Fade out the foreground.
  };

private:
  /**
   * Apply size and position constraints to painter actors.
   *
   * @param[inout] actor The actor.
   * @param[in] depth Depth position.
   */
  void ApplyConstraint( Actor& actor, float depth );

  /**
   * Apply size constraint to check tick
   *
   * @param[inout] actor The actor.
   * @param[in] depth Depth position.
   */
  void ApplySelectedConstraint( Actor& actor, float depth );

  /**
   * Adds the actor to the fade in animation. It creates a fade in animation if needed.
   *
   * @param[in] actor The actor.
   */
  void AddToFadeInAnimation( const Actor& actor );

  /**
   * Starts the check in animation.
   *
   * CheckBoxButtonDefaultPainter::CheckInAnimationFinished slot is called when the animation finishes.
   */
  void StartCheckInAnimation();

  /**
   * Stops the check in animation.
   */
  void StopCheckInAnimation();

  /**
   * Adds the actor to the fade out animation. It creates a fade out animation if needed.
   *
   * @param[in] actor The actor.
   */
  void AddToFadeOutAnimation( const Actor& actor );

  /**
   * Starts the check out animation.
   *
   * CheckBoxButtonDefaultPainter::CheckOutAnimationFinished slot is called when the animation finishes.
   *
   * @param[inout] checkBox The button which holds images.
   */
  void StartCheckOutAnimation( Toolkit::CheckBoxButton& checkBox );

  /**
   * Stops the fade out animation.
   *
   * It removes the actor stored in CheckBoxButtonDefaultPainter::mFadeOutBackgroundImage and
   * CheckBoxButtonDefaultPainter::mFadeOutSelectedImage.
   *
   * @param[inout] checkBox The button which holds images.
   * @param[in] remove If true, removes the fadeout actor from root.
   */
  void StopCheckOutAnimation( Toolkit::CheckBoxButton& checkBox, bool remove = true );

  /**
   * It adds the actor to the root actor and to the fade in animation.
   *
   * @param[inout] checkBox The button which holds images.
   * @param[inout] image The actor.
   * @param[in] opacity The initial opacity.
   */
  void FadeInImage( Toolkit::CheckBoxButton& checkBox, Actor& image, float opacity = 0.f );

  /**
   * It adds the actor fade out animation and stores it to be removed when the animation finishes.
   *
   * @param[inout] checkBox The button which holds images.
   * @param[in] layer Defines if the actor is going to be stored in the mFadeOutBackgroundImage or mFadeOutSelectedImage member.
   * @param[inout] image The actor.
   * @param[in] opacity The initial opacity.
   */
  void FadeOutImage( Toolkit::CheckBoxButton& checkBox, ImageLayer layer, Actor& image, float opacity = 1.f );

  /**
   * Adds the actor to the fade in animation. It creates a fade in animation if needed.
   *
   * @param[in] actor The actor.
   */
  void AddToCheckInAnimation( const Actor& actor );

  /**
   * It adds the actor to the root actor and to the fade in animation.
   *
   * @param[inout] checkBox The button which holds images.
   * @param[inout] image The actor.
   */
  void SetupSelectedAnimation( Toolkit::CheckBoxButton& checkBox, Actor& image );

  /**
   * Signal end of check out animation
   */
  void EndCheckOutAnimation();

  // slots

  /**
   * Called when the fade out animation finishes.
   *
   * It changes the check button painter state and removes actors from the root.
   */
  void CheckOutAnimationFinished( Dali::Animation& source );

  /**
   * Called when the fade in animation finishes.
   *
   * It changes the check button painter state.
   */
  void CheckInAnimationFinished( Dali::Animation& source );

private:
  bool                      mDisabled;           ///< Stores the disabled property.

  PaintState                mPaintState;       ///< The painter state.

  Animation                 mCheckInAnimation;  ///< Animation used in the state transitions.
  Animation                 mCheckOutAnimation; ///< Animation used in the state transitions.
  Internal::CheckBoxButton* mButton;           ///< Temporary pointer used to remove fade out images from button.
  float                     mAnimationTime;    ///< The animation time.

  ImageRegionEffect         mTickUVEffect;     ///< ImageRegionEffect to expand the tick across

  Property::Index           mPercentageParentSizeProperty;  ///< Dynamic property on the image actor
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_CHECK_BOX_BUTTON_DEFAULT_PAINTER_H__
