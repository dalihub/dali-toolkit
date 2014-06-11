#ifndef __DALI_TOOLKIT_INTERNAL_POPUP_H__
#define __DALI_TOOLKIT_INTERNAL_POPUP_H__

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
#include <dali/dali.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/popup/popup.h>
#include <dali-toolkit/internal/controls/popup/popup-style-impl.h>

namespace Dali
{

namespace Toolkit
{

class Button;

namespace Internal
{

class Popup;
class PopupStyle;

typedef IntrusivePtr<Popup>    PopupPtr;
typedef IntrusivePtr<PopupStyle> PopupStylePtr;

/**
 * @copydoc Toolkit::Popup
 */
class Popup : public Control
{
public:

  /**
   * Create a new Popup.
   * @return A public handle to the newly allocated Popup.
   */
  static Dali::Toolkit::Popup New();

public:

  /**
   * Returns number of buttons added to Popup
   *
   * @return Number of buttons
   */
  size_t GetButtonCount() const;

  /**
   * @copydoc Toolkit::Popup::SetBackgroundImage
   */
  void SetBackgroundImage( Actor image );

  /**
   * @copydoc Toolkit::Popup::SetButtonAreaImage
   */
  void SetButtonAreaImage( Actor image );

  /**
   * @copydoc Toolkit::Popup::SetTitle( const std::string& text );
   */
  void SetTitle( const std::string& text );

  /**
   * @copydoc Toolkit::Popup::SetTitle( TextView titleActor )
   */
  void SetTitle( Toolkit::TextView titleActor );

  /**
   * @copydoc Toolkit::Popup::GetTitle
   */
  Toolkit::TextView GetTitle() const;

  /**
   * @copydoc Toolkit::Popup::AddButton
   */
  void AddButton( Toolkit::Button button );

  /**
   * @copydoc Toolkit::Popup::SetState( PopupState state )
   */
  void SetState( Toolkit::Popup::PopupState state );

  /**
   * @copydoc Toolkit::Popup::SetState( PopupState state, float duration )
   */
  void SetState( Toolkit::Popup::PopupState state, float duration );

  /**
   * @copydoc Toolkit::Popup::GetState( )
   */
  Toolkit::Popup::PopupState GetState() const;

  /**
   * @copydoc Toolkit::Popup::ShowTail
   */
  void ShowTail(const Vector3& position);

  /**
   * @copydoc Toolkit::Popup::HideTail
   */
  void HideTail();


  /**
   * Sets the style of the popup
   * @param[in] style The style of the popup
   */
  void SetStyle(PopupStyle& style);

  /**
   * Gets the style of the popup
   * @return style of the popup
   */
  PopupStylePtr GetStyle() const;

protected:

  /**
   * Construct a new Popup.
   * @param[in] style of the popup
   */
  Popup(PopupStyle& style);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Popup();

private:

  /**
   * Creates and applies the default background image.
   */
  void SetDefaultBackgroundImage();

  /**
   * Create Dim Backing
   * (covers all content behind the dialog)
   */
  void CreateBacking();

  /**
   * Create Dialog
   * (dialog content resides inside this - buttons, title etc.)
   */
  void CreateDialog();

  /**
   * Animate Popup by scaling uniformally from 0 to 100% and vice versa (default behaviour)
   * @param[in] state The desired state to change into.
   * @param[in] duration The time for this animation to take.
   */
  void HandleStateChange( Toolkit::Popup::PopupState state, float duration );

  /**
   * Invoked once StateChange has completed.
   */
  void HandleStateChangeComplete();

public: // Signals

  /**
   * @copydoc Dali::Toolkit::Popup::OutsideTouchedSignal()
   */
  Toolkit::Popup::TouchedOutsideSignalV2& OutsideTouchedSignal();

  /**
   * @copydoc Dali::Toolkit::Popup::HiddenSignal()
   */
  Toolkit::Popup::HiddenSignalV2& HiddenSignal();

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

private:

  /**
   * Signal occurs when the State animation (transition from hide<->show) finishes
   * @param[in] source The animation that just finished.
   */
  void OnStateAnimationFinished( Animation& source );

  /**
   * Signal occurs when the dimmed backing for the Popup is touched.
   * @param[in] actor The Actor Touched
   * @param[in] event The Touch Event.
   * @return Whether to consume event or not.
   */
  bool OnBackingTouched(Actor actor, const TouchEvent& event);

  /**
   * Signal occurs when the mouse wheel event is occured on dimmed backing for the Popup.
   * @param[in] actor The Actor got mouse wheel
   * @param[in] event The Mouse Wheel Event.
   * @return Whether to consume event or not.
   */
  bool OnBackingMouseWheelEvent(Actor actor, const MouseWheelEvent& event);

  /**
   * Signal occurs when the dialog has been touched.
   * @param[in] actor The Actor Touched
   * @param[in] event The Touch Event.
   * @return Whether to consume event or not.
   */
  bool OnDialogTouched(Actor actor, const TouchEvent& event);

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Dali::CustomActorImpl::OnPropertySet()
   */
  virtual void OnPropertySet( Property::Index index, Property::Value propertyValue );

  /**
   * From Control; called after a child has been added to the owning actor.
   * @param[in] child The child which has been added.
   */
  virtual void OnControlChildAdd( Actor& child );

  /**
   * @copydoc Control::OnRelaidOut()
   */
  virtual void OnRelaidOut( Vector2 size, ActorSizeContainer& container );

  /**
   * @copydoc Control::OnKeyEvent()
   */
  virtual bool OnKeyEvent(const KeyEvent& event);

  /**
   * @copydoc Control::GetNaturalSize()
   */
  virtual Vector3 GetNaturalSize();

  /**
   * @copydoc Control::GetHeightForWidth()
   */
  float GetHeightForWidth( float width );

  /**
   * @copydoc Control::GetWidthForHeight()
   */
  float GetWidthForHeight( float height );

  /**
   * @copydoc Control::GetNextKeyboardFocusableActor()
   */
  Actor GetNextKeyboardFocusableActor(Actor currentFocusedActor, Toolkit::Control::KeyboardFocusNavigationDirection direction, bool loopEnabled);

private:

  // Undefined
  Popup(const Popup&);

  // Undefined
  Popup& operator=(const Popup& rhs);

private:

  struct LayoutInfo
  {
    Vector3 mTitleSize;
    Vector3 mContentSize;
    Vector3 mButtonBgSize;
    std::vector<Vector3> mButtonSize;
  };

private:

  bool mShowing;                          ///< Popup is showing or not

  Layer mLayer;                           ///< Popup Layer (i.e. Dim backing and PopupBg reside in this)
  Actor mPopupBg;                         ///< Popup Background (i.e. dialog reside in this)
  ImageActor mBacking;                    ///< Backing actor (dim effect)

  Actor mPreviousFocusedActor;            ///< Store the previous focused actor to restore the focus when popup hide

  Actor mBackgroundImage;                 ///< Stores the background image.
  Actor mButtonAreaImage;                 ///< Stores the button background image.
  Toolkit::TextView mTitle;               ///< Stores the text title.
  Actor mContent;                         ///< Stores popup's content.
  Actor mBottomBg;                        ///< bottom button bar background. ImageActor is replaced with Actor due to hidden image.
  Actor mTailImage;                       ///< Stores the tail image

  ActorContainer mButtons;                ///< Keeps track of the buttons added to this popup.
  Toolkit::Popup::PopupState mState;      ///< Popup current state.
  Animation mAnimation;                   ///< The animation instance managing state changing.
  bool mAlterAddedChild;                  ///< Flag used to control whether children are reparented or not.
  PopupStylePtr mPopupStyle;              ///< The style to be used for this popup.

  LayoutInfo mLayoutInfo;                 ///< Stores sizes of all popup components.

  Toolkit::Popup::TouchedOutsideSignalV2 mTouchedOutsideSignalV2;
  Toolkit::Popup::HiddenSignalV2 mHiddenSignalV2;

  Property::Index mPropertyTitle;         ///< Property index for Title.
  Property::Index mPropertyState;         ///< Property index for popup state.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::Popup& GetImpl(Toolkit::Popup& pub)
{
  DALI_ASSERT_ALWAYS(pub);

  Dali::RefObject& handle = pub.GetImplementation();

  return static_cast<Toolkit::Internal::Popup&>(handle);
}

inline const Toolkit::Internal::Popup& GetImpl(const Toolkit::Popup& pub)
{
  DALI_ASSERT_ALWAYS(pub);

  const Dali::RefObject& handle = pub.GetImplementation();

  return static_cast<const Toolkit::Internal::Popup&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_POPUP_H__
