#ifndef __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_H__
#define __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_H__

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
#include <dali/public-api/common/dali-vector.h>

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

  /**
   * @copydoc Dali::Toolkit::PushButton::SetAutoRepeating( bool autoRepeating )
   */
  void SetAutoRepeating( bool autoRepeating );

  /**
   * @copydoc Dali::Toolkit::PushButton::IsAutoRepeating() const
   */
  bool IsAutoRepeating() const;

  /**
   * @copydoc Dali::Toolkit::PushButton::SetInitialAutoRepeatingDelay( float initialAutoRepeatingDelay )
   */
  void SetInitialAutoRepeatingDelay( float initialAutoRepeatingDelay );

  /**
   * @copydoc Dali::Toolkit::PushButton::GetInitialAutoRepeatingDelay() const
   */
  float GetInitialAutoRepeatingDelay() const;

  /**
   * @copydoc Dali::Toolkit::PushButton::SetNextAutoRepeatingDelay( float nextAutoRepeatingDelay )
   */
  void SetNextAutoRepeatingDelay( float nextAutoRepeatingDelay );

  /**
   * @copydoc Dali::Toolkit::PushButton::GetNextAutoRepeatingDelay() const
   */
  float GetNextAutoRepeatingDelay() const;

  /**
   * @copydoc Dali::Toolkit::PushButton::SetToggleButton( bool toggle )
   */
  void SetToggleButton( bool toggle );

  /**
   * @copydoc Dali::Toolkit::PushButton::IsToggleButton() const
   */
  bool IsToggleButton() const;

  /**
   * @copydoc Dali::Toolkit::PushButton::SetToggled( bool toggle )
   */
  void SetToggled( bool toggle );

  /**
   * @copydoc Dali::Toolkit::PushButton::IsToggled() const
   */
  bool IsToggled() const;

  /**
   * @copydoc Dali::Toolkit::PushButton::SetButtonImage( const Image image )
   */
  void SetButtonImage( Image image );

  /**
   * @copydoc Dali::Toolkit::PushButton::SetButtonImage( Actor image )
   */
  void SetButtonImage( Actor image );

  /**
   * Used by the painter only.
   * @return A reference to the button image.
   */
  Actor& GetButtonImage();

  /**
   * @copydoc Dali::Toolkit::PushButton::
   */
  Actor GetButtonImage() const;

  /**
   * @copydoc Dali::Toolkit::PushButton::SetBackgroundImage( const Image image )
   */
  void SetBackgroundImage( Image image );

  /**
   * @copydoc Dali::Toolkit::PushButton::SetBackgroundImage( Actor image )
   */
  void SetBackgroundImage( Actor image );

  /**
   * Used by the painter only.
   * @return A reference to the background image.
   */
  Actor& GetBackgroundImage();

  /**
   * @copydoc Dali::Toolkit::PushButton::GetBackgroundImage()
   */
  Actor GetBackgroundImage() const;

  /**
   * @copydoc Dali::Toolkit::PushButton::SetPressedImage( const Image image )
   */
  void SetPressedImage( Image image );

  /**
   * @copydoc Dali::Toolkit::PushButton::SetPressedImage( Actor image )
   */
  void SetPressedImage( Actor image );

  /**
   * Used by the painter only.
   * @return A reference to the pressed image.
   */
  Actor& GetPressedImage();

  /**
   * @copydoc Dali::Toolkit::PushButton::GetPressedImage()
   */
  Actor GetPressedImage() const;

  /**
   * @copydoc Dali::Toolkit::PushButton::SetDimmedBackgroundImage( Image image )
   */
  void SetDimmedBackgroundImage( Image image );

  /**
   * @copydoc Dali::Toolkit::PushButton::SetDimmedBackgroundImage( Actor image )
   */
  void SetDimmedBackgroundImage( Actor image );

  /**
   * Used by the painter only.
   * @return A reference to the dimmed background image.
   */
  Actor& GetDimmedBackgroundImage();

  /**
   * @copydoc Dali::Toolkit::PushButton::GetDimmedBackgroundImage()
   */
  Actor GetDimmedBackgroundImage() const;

  /**
   * @copydoc Dali::Toolkit::PushButton::SetDimmedImage( Image image )
   */
  void SetDimmedImage( Image image );

  /**
   * @copydoc Dali::Toolkit::PushButton::SetDimmedImage( Actor image )
   */
  void SetDimmedImage( Actor image );

  /**
   * Used by the painter only.
   * @return A reference to the dimmed button image.
   */
  Actor& GetDimmedImage();

  /**
   * @copydoc Dali::Toolkit::PushButton::GetDimmedImage()
   */
  Actor GetDimmedImage() const;

  /**
   * @copydoc Dali::Toolkit::PushButton::SetLabelText( const std::string& text )
   */
  void SetLabelText( const std::string& text );

  /**
   * @copydoc Dali::Toolkit::PushButton::SetLabelText( Actor text )
   */
  void SetLabelText( Actor text );

  /**
   * Used by the painter only.
   * @return A reference to the label actor.
   */
  Actor& GetLabel();

  /**
   * @copydoc Dali::Toolkit::PushButton::GetLabelText()
   */
  Actor GetLabelText() const;

  /**
   * Used by the painter only.
   * @return A reference to the background image that is fading out.
   */
  Actor& GetFadeOutBackgroundImage();

  /**
   * Used by the painter only.
   * @return A reference to the button image that is fading out.
   */
  Actor& GetFadeOutButtonImage();

public:

  // Signals

  /**
   * @copydoc Dali::Toolkit::PushButton::ToggledSignal()
   */
  Toolkit::PushButton::ToggledSignalV2& ToggledSignal();

  /**
   * @copydoc Dali::Toolkit::PushButton::PressedSignal()
   */
  Toolkit::PushButton::PressedSignalV2& PressedSignal();

  /**
   * @copydoc Dali::Toolkit::PushButton::ReleasedSignal()
   */
  Toolkit::PushButton::ReleasedSignalV2& ReleasedSignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

  // Properties

  /**
   * @copydoc Button::SetProperty
   */
  static void SetProperty( BaseObject* object, Property::Index propertyIndex, const Property::Value& value );

  /**
   * @copydoc Button::GetProperty
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index propertyIndex );

protected: // From Button

  /**
   * Sets the Leave signal.
   */
  virtual void OnButtonInitialize();

  /**
   * Emits signals and notifies the painter accordingly with the set button
   * properties when the button is pressed.
   */
  virtual void OnButtonDown();

  /**
   * Emits signals and notifies the painter accordingly with the set button
   * properties when the button is released.
   */
  virtual void OnButtonUp();

  /**
   * Emits signals and notifies the painter accordingly with the set button
   * properties when the touch point leaves the boundary of the button.
   */
  virtual void OnTouchPointLeave();

  /**
   * Currently it doesn't need different behaviour than @see OnTouchPointLeave()
   */
  virtual void OnTouchPointInterrupted();

  /**
   * Sets the push button animation time.
   * @param animationTime The animation time in seconds.
   */
  virtual void OnAnimationTimeSet( float animationTime );

  /**
   * Retrieves the animation time.
   * @return The animation time in seconds.
   */
  virtual float OnAnimationTimeRequested() const;

protected: // From Control

  /**
   * Respond the activate notification.
   */
  virtual void OnActivated();

  /**
   * @copydoc Control::GetNaturalSize()
   */
  virtual Vector3 GetNaturalSize();

private:

  /**
   * Perform the click action to click the button.
   * @param[in] attributes The attributes to perfrom this action.
   */
  void DoClickAction(const PropertyValueContainer& attributes);

public:

  /**
   * Performs actions as requested using the action name.
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] attributes The attributes with which to perfrom this action.
   * @return true if action has been accepted by this control
   */
  static bool DoAction(BaseObject* object, const std::string& actionName, const std::vector<Property::Value>& attributes);

  /**
   * Construct a new PushButton.
   */
  PushButton();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~PushButton();

private:

  // Undefined
  PushButton( const PushButton& );

  // Undefined
  PushButton& operator=( const PushButton& );

  /**
   * Sets up the autorepeating timer.
   * @param[in] delay The delay time in seconds.
   */
  void SetUpTimer( float delay );

  /**
   * Slot called when Dali::Timer::SignalTick signal. Resets the autorepeating timer.
   */
  bool AutoRepeatingSlot();

private:
  bool  mAutoRepeating;             ///< Stores the autorepeating property.
  float mInitialAutoRepeatingDelay; ///< Stores the initial autorepeating delay in seconds.
  float mNextAutoRepeatingDelay;    ///< Stores the next autorepeating delay in seconds.
  bool  mToggleButton;              ///< Stores the toggle property.

  // AutoRepeating
  Timer mAutoRepeatingTimer;        ///< Timer used to implement the autorepeating property.

  // Toggle
  bool  mToggled;                   ///< Stores the toggle state.

  // Signals
  Toolkit::PushButton::ToggledSignalV2 mToggledSignalV2;   ///< Signal emitted when the button is toggled.
  Toolkit::PushButton::PressedSignalV2 mPressedSignalV2;   ///< Signal emitted when the button is pressed.
  Toolkit::PushButton::ReleasedSignalV2 mReleasedSignalV2; ///< Signal emitted when the button is released.

  Actor mButtonImage;               ///< Stores the released image.
  Actor mBackgroundImage;           ///< Stores the background image.
  Actor mPressedImage;              ///< Stores the pressed image.
  Actor mDimmedImage;               ///< Stores the dimmed image.
  Actor mDimmedBackgroundImage;     ///< Stores the dimmed background image.

  Actor mLabel;                     ///< Stores the text label.

  Actor mFadeOutBackgroundImage;    ///< Stores a background image, which is in a fade out animation, to be removed when the animation finishes.
  Actor mFadeOutButtonImage;        ///< Stores a foreground image, which is in a fade out animation, to be removed when the animation finishes.

  // Actions
  bool mClickActionPerforming;
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
