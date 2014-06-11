#ifndef __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_DEFAULT_PAINTER_H__
#define __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_DEFAULT_PAINTER_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

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

  /**
   * Sets the button image.
   *
   * It adds the button image to the root actor and creates the image transition if needed.
   *
   * @param[inout] pushButton The button in which all actors that form its appearance are going to be added.
   * @param[in] image The button image.
   */
  void SetButtonImage( Toolkit::PushButton& pushButton, Actor image );

  /**
   * Sets the background image.
   *
   * It adds the background image to the root actor and creates the image transition if needed.
   *
   * @param[inout] pushButton The button in which all actors that form its appearance are going to be added.
   * @param[in] image The background image.
   */
  void SetBackgroundImage( Toolkit::PushButton& pushButton, Actor image );

  /**
   * Sets the pressed image.
   *
   * It adds the pressed image to the root actor and creates the image transition if needed.
   *
   * @param[inout] pushButton The button in which all actors that form its appearance are going to be added.
   * @param[in] image The pressed image.
   */
  void SetPressedImage( Toolkit::PushButton& pushButton, Actor image );

  /**
   * Sets the dimmed background image.
   *
   * It adds the dimmed background image to the root actor and creates the image transition if needed.
   *
   * @param[inout] pushButton The button in which all actors that form its appearance are going to be added.
   * @param[in] image The dimmed background image.
   */
  void SetDimmedBackgroundImage( Toolkit::PushButton& pushButton, Actor image );

  /**
   * Sets the dimmed image.
   *
   * It adds the dimmed image to the root actor and creates the image transition if needed.
   *
   * @param[inout] pushButton The button in which all actors that form its appearance are going to be added.
   * @param[in] image The  image.
   */
  void SetDimmedImage( Toolkit::PushButton& pushButton, Actor image );

  /**
   * Sets the text label.
   *
   * It adds the text to the root actor.
   *
   * @param[inout] pushButton The button in which all actors that form its appearance are going to be added.
   * @param[in] text Label text.
   */
  void SetLabelText( Toolkit::PushButton& pushButton, Actor text );

  /////////////////////////////////////////////////////////////////////////////
  // ButtonPainter interface
  /////////////////////////////////////////////////////////////////////////////

  /**
   * Initializes the painter by setting the default images.
   *
   * @param[inout] button The button in which all actors that form its appearance are going to be added.
   */
  void Initialize( Toolkit::Button& button );

  /**
   * Sets the new size.
   *
   * Resizes actors. It applies size constraints.
   *
   * @param[inout] button The button which stores button's images.
   * @param[in] size The new size.
   */
  void SetSize( Toolkit::Button& button, const Vector3& size );

  /**
   * This method is called when the \e dimmed property in the Dali::Toolkit::PushButton changes.
   *
   * Creates image transitions if needed.
   *
   * @param[inout] button The button in which all actors that form its appearance are going to be added.
   * @param[in] dimmed property.
   */
  void SetDimmed( Toolkit::Button& button, bool dimmed );

  /**
   * Sets the animation time.
   * @param[in] animationTime The animation time.
   */
  void SetAnimationTime( float animationTime );

  /**
   * Retrieves the animation time.
   * @return The animation time.
   */
  float GetAnimationTime() const;

  /////////////////////////////////////////////////////////////////////////////
  // PushButtonPainter interface
  /////////////////////////////////////////////////////////////////////////////

  /**
   * This method is called when the \e autorepeating property in the Dali::Toolkit::PushButton changes.
   * @param[in] autorepeating property.
   */
  void SetAutoRepeating( bool autorepeating );

  /**
   * This method is called when the Dali::Toolkit::Internal::PushButton in which this object is registered
   * is pressed. It changes to the pressed image with a transition.
   *
   * @param[inout] button The Dali::Toolkit::PushButton in which this object is registered.
   */
  void Pressed( Toolkit::PushButton& button );

  /**
   * This method is called when the Dali::Toolkit::Internal::PushButton in which this object is registered
   * is released. It changes to the button image with a transition.
   *
   * @param[inout] button The Dali::Toolkit::PushButton in which this object is registered.
   */
  void Released( Toolkit::PushButton& button );

  /**
   * This method is called when the Dali::Toolkit::Internal::PushButton in which this object is registered
   * is clicked.
   *
   * @param[inout] button The Dali::Toolkit::PushButton in which this object is registered.
   */
  void Clicked( Toolkit::PushButton& button );

  /**
   * This method is called when the Dali::Toolkit::Internal::PushButton in which this object is registered
   * is toggled.
   *
   * @param[inout] button The Dali::Toolkit::PushButton in which this object is registered.
   */
  void Toggled( Toolkit::PushButton& button );

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
    ReleasedState,             ///< The push button is released.
    PressedState,              ///< The push button is pressed.
    DimmedReleasedState,       ///< The push button is dimmed and released.
    DimmedPressedState,        ///< The push button is dimemd and pressed.
    ReleasedPressedTransition, ///< The push button is in transition from released to pressed.
    PressedReleasedTransition, ///< The push button is in transition from pressed to released.
    ReleasedDimmedTransition,  ///< The push button is in transition from released to dimmed.
    DimmedReleasedTransition,  ///< The push button is in transition from dimmed to released.
    PressedDimmedTransition,   ///< The push button is in transition from pressed to dimmed.
    DimmedPressedTransition    ///< The push button is in transition from dimmed to pressed.
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
  bool                  mDimmed;           ///< Stores the dimmed property.

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
