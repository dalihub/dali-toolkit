#ifndef __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_DEFAULT_PAINTER_H__
#define __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_DEFAULT_PAINTER_H__

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
#include <dali/public-api/images/image.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/animation/animation.h>

#include "push-button-painter-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

// Forward declarations

class PushButton;
class PushButtonDefaultPainter;

// Type definitions

typedef IntrusivePtr<PushButtonDefaultPainter> PushButtonDefaultPainterPtr;

/**
 * PushButtonDefaultPainter controls the Dali::Toolkit::PushButton appearance.
 *
 * This class inherits from Dali::Toolkit::Internal::PushButtonPainter and is registered in a
 * Dali::Toolkit::Internal::PushButton object in order to receive the state changes.
 */
class PushButtonDefaultPainter : public PushButtonPainter
{
public:

  /**
   * Constructor.
   *
   * Set actors and animations to NULL.
   */
  PushButtonDefaultPainter();

  /**
   * Destructor.
   *
   * It clears all fade in or fade out animations.
   */
  ~PushButtonDefaultPainter();

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
   * @copydoc ButtonPainter::SetLabel( Toolkit::Button& button, Actor label )
   */
  virtual void SetLabel( Toolkit::Button& button, Actor label );

  /**
   * @copydoc ButtonPainter::SetAutoRepeating( bool autorepeating )
   */
  virtual void SetAutoRepeating( bool autorepeating );

  /**
   * @copydoc ButtonPainter::SetButtonImage( Toolkit::Button& button, Actor image )
   */
  virtual void SetButtonImage( Toolkit::Button& button, Actor image );

  /**
   * @copydoc ButtonPainter::SetSelectedImage( Toolkit::Button& button, Actor image )
   */
  virtual void SetSelectedImage( Toolkit::Button& button, Actor image );

  /**
   * @copydoc ButtonPainter::SetBackgroundImage( Toolkit::Button& button, Actor image )
   */
  virtual void SetBackgroundImage( Toolkit::Button& button, Actor image );

  /**
   * @copydoc ButtonPainter::SetDisabledImage( Toolkit::Button& button, Actor image )
   */
  virtual void SetDisabledImage( Toolkit::Button& button, Actor image );

  /**
   * @copydoc ButtonPainter::SetDisabledBackgroundImage( Toolkit::Button& button, Actor image )
   */
  virtual void SetDisabledBackgroundImage( Toolkit::Button& button, Actor image );

  /**
   * @copydoc ButtonPainter::Pressed( Toolkit::Button& button )
   */
  void Pressed( Toolkit::Button& button );

  /**
   * @copydoc ButtonPainter::Released( Toolkit::Button& button )
   */
  void Released( Toolkit::Button& button );

  /**
   * @copydoc ButtonPainter::Clicked( Toolkit::Button& button )
   */
  void Clicked( Toolkit::Button& button );

  /**
   * @copydoc ButtonPainter::Selected( Toolkit::Button& button )
   */
  void Selected( Toolkit::Button& button );

private:

  // Undefined
  PushButtonDefaultPainter( const PushButtonDefaultPainter& );

  // Undefined
  PushButtonDefaultPainter& operator=( const PushButtonDefaultPainter& );

private:

  /**
   * Default push button painter states.
   */
  enum PaintState
  {
    ReleasedState,              ///< The push button is released.
    PressedState,               ///< The push button is pressed.
    DisabledReleasedState,      ///< The push button is disabled and released.
    DisabledPressedState,       ///< The push button is Disabled and pressed.
    ReleasedPressedTransition,  ///< The push button is in transition from released to pressed.
    PressedReleasedTransition,  ///< The push button is in transition from pressed to released.
    ReleasedDisabledTransition, ///< The push button is in transition from released to disabled.
    DisabledReleasedTransition, ///< The push button is in transition from disabled to released.
    PressedDisabledTransition,  ///< The push button is in transition from pressed to disabled.
    DisabledPressedTransition   ///< The push button is in transition from disabled to pressed.
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
   * Adds the actor to the fade in animation. It creates a fade in animation if needed.
   *
   * @param[in] actor The actor.
   */
  void AddToFadeInAnimation( const Actor& actor );

  /**
   * Starts the fade in animation.
   *
   * PushButtonDefaultPainter::PressedReleasedFadeInAnimationFinished slot is called when the animation finishes.
   */
  void StartFadeInAnimation();

  /**
   * Stops the fade in animation.
   */
  void StopFadeInAnimation();

  /**
   * Adds the actor to the fade out animation. It creates a fade out animation if needed.
   */
  void AddToFadeOutAnimation( const Actor& actor );

  /**
   * Starts the fade out animation.
   *
   * PushButtonDefaultPainter::PressedReleasedFadeOutAnimationFinished slot is called when the animation finishes.
   * @param[inout] pushButton The button which holds images.
   */
  void StartFadeOutAnimation( Toolkit::PushButton& pushButton );

  /**
   * Stops the fade out animation.
   *
   * It removes the actor stored in PushButtonDefaultPainter::mFadeOutBackgroundImage and
   * PushButtonDefaultPainter::mFadeOutCheckedImage.
   *
   * @param[inout] pushButton The button which holds images.
   * @param[in] remove If true, removes the fadeout actor from root.
   */
  void StopFadeOutAnimation( Toolkit::PushButton& pushButton, bool remove = true );

  /**
   * It adds the actor to the root actor and to the fade in animation.
   *
   * @param[inout] pushButton The button which holds images.
   * @param[inout] image The actor.
   * @param[in] opacity The initial opacity.
   */
  void FadeInImage( Toolkit::PushButton& pushButton, Actor& image, float opacity = 0.f );

  /**
   * It adds the actor fade out animation and stores it to be removed when the animation finishes.
   * @param[inout] pushButton The button which holds images.
   * @param[in] layer Defines if the actor is going to be stored in the mFadeOutBackgroundImage or mFadeOutCheckedImage member.
   * @param[inout] image The actor.
   * @param[in] opacity The initial opacity.
   */
  void FadeOutImage( Toolkit::PushButton& pushButton, ImageLayer layer, Actor& image, float opacity = 1.f );

  // slots

  /**
   * Called when the fade out animation finishes.
   *
   * It changes the check button painter state and removes actors from the root.
   */
  void PressedReleasedFadeOutAnimationFinished( Dali::Animation& source );

  /**
   * Called when the fade in animation finishes.
   *
   * It changes the check button painter state.
   */
  void PressedReleasedFadeInAnimationFinished( Dali::Animation& source );

private:
  bool                  mAutoRepeating;    ///< Stores the autorepeating property.
  bool                  mDisabled;         ///< Stores the disabled property.

  PaintState            mPaintState;       ///< The painter state.
  Animation             mFadeInAnimation;  ///< Animation used in the state transitions.
  Animation             mFadeOutAnimation; ///< Animation used in the state transitions.
  Internal::PushButton* mButton;           ///< Temporary pointer used to remove fade out images from button.
  float                 mAnimationTime;    ///< The animation time.
  Vector3               mSize;             ///< The button's size.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_DEFAULT_PAINTER_H__
