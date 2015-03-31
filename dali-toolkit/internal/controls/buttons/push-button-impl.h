#ifndef __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_H__
#define __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_H__

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
#include <dali-toolkit/public-api/controls/buttons/push-button.h>
#include "button-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * PushButton implementation class.
 *
 * \sa Dali::Toolkit::PushButton
 */
class PushButton : public Button
{
public:

  /**
   * Create a new PushButton.
   * @return A smart-pointer to the newly allocated PushButton.
   */
  static Dali::Toolkit::PushButton New();

protected:

  /**
   * Construct a new PushButton.
   */
  PushButton();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~PushButton();

public: // From Button

  /**
   * @copydoc Toolkit::Internal::Button::SetButtonImage( Actor image )
   */
  virtual void SetButtonImage( Actor image );

  /**
   * @copydoc Toolkit::Internal::Button::SetSelectedImage( Actor image )
   */
  virtual void SetSelectedImage( Actor image );

  /**
   * @copydoc Toolkit::Internal::Button::SetBackgroundImage( Actor image )
   */
  virtual void SetBackgroundImage( Actor image );

  /**
   * @copydoc Toolkit::Internal::Button::SetDisabledImage( Actor image )
   */
  virtual void SetDisabledImage( Actor image );

  /**
   * @copydoc Toolkit::Internal::Button::SetDisabledBackgroundImage( Actor image )
   */
  virtual void SetDisabledBackgroundImage( Actor image );

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
  virtual void OnSelected( bool selected );

  /**
   * @copydoc Toolkit::Internal::Button::OnDisabled( bool disabled )
   */
  virtual void OnDisabled( bool disabled );

  /**
   * @copydoc Toolkit::Internal::Button::OnPressed()
   */
  virtual void OnPressed();

  /**
   * @copydoc Toolkit::Internal::Button::OnReleased()
   */
  virtual void OnReleased();

  /**
   * @copydoc Toolkit::Internal::Button::OnClicked()
   */
  virtual void OnClicked();

private: // From Control

  /**
   * @copydoc Toolkit::Control::OnControlSizeSet( const Vector3& targetSize )
   */
  virtual void OnControlSizeSet( const Vector3& targetSize );

  /**
   * @copydoc Toolkit::Control::GetNaturalSize()
   */
  virtual Vector3 GetNaturalSize();

private:

  /**
   * Used in the FadeOut functions.
   */
  enum ImageLayer
  {
    Background, ///< Fade out the background.
    Foreground  ///< Fade out the foreground.
  };

  /**
   * Gets the button image that is fading out.
   * @return A reference to the button image that is fading out.
   */
  Actor& GetFadeOutButtonImage();

  /**
   * Gets the background image that is fading out.
   * @return A reference to the background image that is fading out.
   */
  Actor& GetFadeOutBackgroundImage();

  /**
   * Adds the actor to the fade in animation. It creates a fade in animation if needed.
   * @param[in] actor The actor.
   */
  void AddToFadeInAnimation( Actor& actor );

  /**
   * Starts the fade in animation.
   * PushButton::FadeInAnimationFinished slot is called when the animation finishes.
   */
  void StartFadeInAnimation();

  /**
   * Stops the fade in animation.
   */
  void StopFadeInAnimation();

  /**
   * Adds the actor to the fade out animation. It creates a fade out animation if needed.
   */
  void AddToFadeOutAnimation( Actor& actor );

  /**
   * Starts the fade out animation.
   * PushButton::FadeOutAnimationFinished slot is called when the animation finishes.
   */
  void StartFadeOutAnimation();

  /**
   * Stops the fade out animation.
   * It removes the actor stored in PushButton::mFadeOutBackgroundImage and PushButton::mFadeOutCheckedImage.
   * @param[in] remove If true, removes the fadeout actor from root.
   */
  void StopFadeOutAnimation( bool remove = true );

  /**
   * It adds the actor to the root actor and to the fade in animation.
   * @param[inout] image The actor.
   * @param[in] opacity The initial opacity.
   */
  void FadeInImage( Actor& image, float opacity = 0.f, int priority = -1 );

  /**
   * It adds the actor fade out animation and stores it to be removed when the animation finishes.
   * @param[in] layer Defines if the actor is going to be stored in the mFadeOutBackgroundImage or mFadeOutCheckedImage member.
   * @param[inout] image The actor.
   * @param[in] opacity The initial opacity.
   */
  void FadeOutImage( ImageLayer layer, Actor& image, float opacity = 1.f );

  // slots

  /**
   * Called when the fade out animation finishes.
   * It changes the check button paint state and removes actors from the root.
   */
  void FadeOutAnimationFinished( Dali::Animation& source );

  /**
   * Called when the fade in animation finishes.
   * It changes the check button paint state.
   */
  void FadeInAnimationFinished( Dali::Animation& source );

private:

  // Undefined
  PushButton( const PushButton& );

  // Undefined
  PushButton& operator=( const PushButton& );

private:

  Animation             mFadeInAnimation;           ///< Animation used in the state transitions.
  Animation             mFadeOutAnimation;          ///< Animation used in the state transitions.

  Actor                 mFadeOutButtonContent;      ///< Stores a foreground content, which is in a fade out animation, to be removed when the animation finishes.
  Actor                 mFadeOutBackgroundContent;  ///< Stores a background content, which is in a fade out animation, to be removed when the animation finishes.

  Vector3               mSize;                      ///< The button's size.

  PaintState            mPaintState;                ///< The paint state.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::PushButton& GetImplementation( Toolkit::PushButton& button )
{
  DALI_ASSERT_ALWAYS( button );

  Dali::RefObject& handle = button.GetImplementation();

  return static_cast<Toolkit::Internal::PushButton&>( handle );
}

inline const Toolkit::Internal::PushButton& GetImplementation( const Toolkit::PushButton& button )
{
  DALI_ASSERT_ALWAYS( button );

  const Dali::RefObject& handle = button.GetImplementation();

  return static_cast<const Toolkit::Internal::PushButton&>( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_H__
