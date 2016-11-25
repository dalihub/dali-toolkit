#ifndef __DALI_TOOLKIT_INTERNAL_POPUP_H__
#define __DALI_TOOLKIT_INTERNAL_POPUP_H__

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
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/animation/animation.h>
#include <dali/devel-api/animation/animation-data.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/table-view/table-view.h>
#include <dali-toolkit/devel-api/controls/popup/popup.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class Popup;

typedef IntrusivePtr< Popup > PopupPtr;

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
   * @copydoc Toolkit::Popup::SetPopupBackgroundImage
   */
  void SetPopupBackgroundImage( Actor image );

  /**
   * @copydoc Toolkit::Popup::GetPopupBackgroundImage
   */
  Actor GetPopupBackgroundImage() const;

  /**
   * @copydoc Toolkit::Popup::SetTitle( Actor titleActor )
   */
  void SetTitle( Actor titleActor );

  /**
   * @copydoc Toolkit::Popup::GetTitle
   */
  Actor GetTitle() const;

  /**
   * @copydoc Toolkit::Popup::SetContent
   */
  void SetContent( Actor content );

  /**
   * @copydoc Toolkit::Popup::GetContent
   */
  Actor GetContent() const;

  /**
   * @copydoc Toolkit::Popup::SetFooter
   */
  void SetFooter( Actor control );

  /**
   * @copydoc Toolkit::Popup::GetFooter
   */
  Actor GetFooter() const;

  /**
   * @copydoc Toolkit::Popup::SetDisplayState
   */
  void SetDisplayState( Toolkit::Popup::DisplayState displayState );

  /**
   * @copydoc Toolkit::Popup::GetDisplayState
   */
  Toolkit::Popup::DisplayState GetDisplayState() const;

  /**
   * @copydoc Toolkit::Popup::SetTailVisibility
   */
  void SetTailVisibility( bool visible );

  /**
   * @copydoc Toolkit::Popup::IsTailVisible
   */
  const bool IsTailVisible() const;

  /**
   * @copydoc Toolkit::Popup::SetTailPosition
   */
  void SetTailPosition( Vector3 position );

  /**
   * @copydoc Toolkit::Popup::GetTailPosition
   */
  const Vector3& GetTailPosition() const;

  /**
   * @copydoc Toolkit::Popup::SetContextualMode
   */
  void SetContextualMode( Toolkit::Popup::ContextualMode mode );

  /**
   * @copydoc Toolkit::Popup::GetContextualMode
   */
  Toolkit::Popup::ContextualMode GetContextualMode() const;

  /**
   * @copydoc Toolkit::Popup::SetAnimationDuration
   */
  void SetAnimationDuration( float duration );

  /**
   * @copydoc Toolkit::Popup::GetAnimationDuration
   */
  float GetAnimationDuration() const;

  /**
   * @copydoc Toolkit::Popup::SetAnimationMode
   */
  void SetAnimationMode( Toolkit::Popup::AnimationMode animationMode );

  /**
   * @copydoc Toolkit::Popup::GetAnimationMode
   */
  Toolkit::Popup::AnimationMode GetAnimationMode() const;

  /**
   * @copydoc Toolkit::Popup::SetAutoHideDelay
   */
  void SetAutoHideDelay( int delay );

  /**
   * @copydoc Toolkit::Popup::GetAutoHideDelay
   */
  int GetAutoHideDelay() const;

  /**
   * @copydoc Toolkit::Popup::SetBackingEnabled
   */
  void SetBackingEnabled( bool enabled );

  /**
   * @copydoc Toolkit::Popup::IsBackingEnabled
   */
  const bool IsBackingEnabled() const;

  /**
   * @copydoc Toolkit::Popup::SetBackingColor
   */
  void SetBackingColor( Vector4 color );

  /**
   * @copydoc Toolkit::Popup::GetBackingColor
   */
  const Vector4& GetBackingColor() const;

  /**
   * @copydoc Toolkit::Popup::SetTailUpImage
   */
  void SetTailUpImage( std::string image );

  /**
   * @copydoc Toolkit::Popup::GetTailUpImage
   */
  const std::string& GetTailUpImage() const;

  /**
   * @copydoc Toolkit::Popup::SetTailDownImage
   */
  void SetTailDownImage( std::string image );

  /**
   * @copydoc Toolkit::Popup::GetTailDownImage
   */
  const std::string& GetTailDownImage() const;

  /**
   * @copydoc Toolkit::Popup::SetTailLeftImage
   */
  void SetTailLeftImage( std::string image );

  /**
   * @copydoc Toolkit::Popup::GetTailLeftImage
   */
  const std::string& GetTailLeftImage() const;

  /**
   * @copydoc Toolkit::Popup::SetTailRightImage
   */
  void SetTailRightImage( std::string image );

  /**
   * @copydoc Toolkit::Popup::GetTailRightImage
   */
  const std::string& GetTailRightImage() const;

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] propertyIndex The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Property::Index propertyIndex, const Property::Value& value );

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] propertyIndex The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index propertyIndex );

protected:

  /**
   * Construct a new Popup.
   */
  Popup();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Popup();

private:

  /**
   * @brief Creates the layout of the popup, to be done just before showing for the first time.
   * Also calls OnLayoutSetup() to allow derived classes to perform layout at this stage.
   */
  void LayoutPopup();

  /**
   * @brief Creates or destroys the popup tail based on the current TAIL_DISPLAYED property.
   * Also uses the TAIL_POSITION property to position it.
   */
  void LayoutTail();

  /**
   * @brief Performs any relative positioning required based on the current contextual mode, if set.
   * If contextual mode is not enabled, this method has no effect.
   * @param[in] size The Popups current size (can be accessed from within the OnRelayout() method).
   */
  void LayoutContext( const Vector2& size );

  /**
   * @brief All transition-in animation setup and layout is done here.
   * Different types of animation mode require different layouts to work,
   * this function encapsulates anything animation-mode specific.
   * This is called once for multiple displays/hides of the pops.
   * It is only re-called when the layout becomes dirty.
   */
  void LayoutAnimation();

  /**
   * @brief Initiates a transition-in or transition-out animation based
   * on the current animation settings.
   * @param[in] transitionIn True to perform a transition-in, false for transition out.
   * @param[in] instantaneous Optional - If set to true will override the duration to provide an instant animation.
   */
  void StartTransitionAnimation( bool transitionIn, bool instantaneous = false );

  /**
   * @brief Invoked once a display state change has completed.
   */
  void DisplayStateChangeComplete();

  /**
   * @brief This is called when the auto-hide timer finishes.
   * It performs a display-state change to HIDDEN.
   * @return True as signal is consumed.
   */
  bool OnAutoHideTimeReached();

  /**
   * @brief Create Dimmed Backing (covers all content behind the dialog).
   *
   * @return The backing control.
   */
  Toolkit::Control CreateBacking();

  /**
   * @brief Creates the lower area within the popup.
   */
  void CreateFooter();

  /**
   * @brief Sets if the popup allows touch events to pass through or not.
   *
   * @param[in] enabled Set to true to make the popup touch-transparent.
   */
  void SetTouchTransparent( bool enabled );

  /**
   * @brief Returns if the popup allows touch events to pass through or not.
   *
   * @return True if the popup is touch-transparent.
   */
  const bool IsTouchTransparent() const;

  /**
   * @brief Allows the popup entry animation to be setup from a Property::Map that could
   * originate, for example, from a JSON file.
   *
   * @param[in] map A Property::Map containing a description of an animation
   */
  void SetEntryAnimationData( const Property::Map& map );

  /**
   * @brief Allows the popup exit animation to be setup from a Property::Map that could
   * originate, for example, from a JSON file.
   *
   * @param[in] map A Property::Map containing a description of an animation
   */
  void SetExitAnimationData( const Property::Map& map );

  /**
   * @briefs Updates the popup background's position and size.
   */
  void UpdateBackgroundPositionAndSize();

public: // Signals

  /**
   * @copydoc Dali::Toolkit::Popup::OutsideTouchedSignal()
   */
  Toolkit::Popup::TouchedOutsideSignalType& OutsideTouchedSignal();

  /**
   * @copydoc Dali::Toolkit::Popup::ShowingSignal()
   */
  Toolkit::Popup::DisplayStateChangeSignalType& ShowingSignal();

  /**
   * @copydoc Dali::Toolkit::Popup::ShownSignal()
   */
  Toolkit::Popup::DisplayStateChangeSignalType& ShownSignal();

  /**
   * @copydoc Dali::Toolkit::Popup::HidingSignal()
   */
  Toolkit::Popup::DisplayStateChangeSignalType& HidingSignal();

  /**
   * @copydoc Dali::Toolkit::Popup::HiddenSignal()
   */
  Toolkit::Popup::DisplayStateChangeSignalType& HiddenSignal();

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
   * Signal occurs when the State animation (transition from hide <-> show) finishes.
   * @param[in] source The animation that just finished.
   */
  void OnDisplayChangeAnimationFinished( Animation& source );

  /**
   * Signal occurs when the dimmed backing for the Popup is touched.
   * @param[in] actor The Actor Touched
   * @param[in] touch The Touch Data.
   * @return Whether to consume event or not.
   */
  bool OnBackingTouched(Actor actor, const TouchData& touch);

  /**
   * Signal occurs when a mouse wheel event occurs on the dimmed backing.
   * @param[in] actor The Actor that got the wheel event.
   * @param[in] event The Wheel Event.
   * @return Whether to consume event or not.
   */
  bool OnBackingWheelEvent(Actor actor, const WheelEvent& event);

  /**
   * Signal occurs when the dialog has been touched.
   * @param[in] actor The Actor Touched
   * @param[in] touch The Touch Data.
   * @return Whether to consume event or not.
   */
  bool OnDialogTouched( Actor actor, const TouchData& touch );

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * Called whenever the popup layout is re-set up.
   * Normally due to a change in contents.
   * Note: This is only done when the popup is shown.
   */
  virtual void OnLayoutSetup() {}

  /**
   * Called when the popup is directly or indirectly parented to the stage.
   */
  virtual void OnStageConnection( int depth );

  /**
   * From Control; called after a child has been added to the owning actor.
   * @param[in] child The child which has been added.
   */
  virtual void OnChildAdd( Actor& child );

  /**
   * @copydoc Control::OnRelayOut()
   */
  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container );

  /**
   * @copydoc Control::OnSetResizePolicy()
   */
  virtual void OnSetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension );

  /**
   * @copydoc Control::GetNaturalSize()
   */
  virtual Vector3 GetNaturalSize();

  /**
   * @copydoc Control::GetHeightForWidth()
   */
  virtual float GetHeightForWidth( float width );

  /**
   * @copydoc Control::GetWidthForHeight()
   */
  virtual float GetWidthForHeight( float height );

  /**
   * @copydoc Control::OnKeyEvent()
   */
  virtual bool OnKeyEvent( const KeyEvent& event );

  /**
   * @copydoc Control::GetNextKeyboardFocusableActor()
   */
  Actor GetNextKeyboardFocusableActor( Actor currentFocusedActor, Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled );

private:

  /**
   * Recursively add any focusable actors or layout containers to the provided vector.
   * Include the top level actor if it is a layout container.
   *
   * @param[in]     parent          The actor to start from
   * @param[in/out] focusableActors The vector to add focusable actors to
   */
  void AddFocusableChildren( Actor parent, std::vector< Actor >& focusableActors );

  /**
   * Recursively add any focusable actors or layout containers to the provided vector.
   *
   * @param[in]     parent          The actor to start from
   * @param[in/out] focusableActors The vector to add focusable actors to
   */
  void AddFocusableChildrenRecursive( Actor parent, std::vector< Actor >& focusableActors );

private:

  // Undefined.
  Popup(const Popup&);

  // Undefined.
  Popup& operator=(const Popup& rhs);

private:

  Toolkit::Popup::TouchedOutsideSignalType     mTouchedOutsideSignal;
  Toolkit::Popup::DisplayStateChangeSignalType mShowingSignal;
  Toolkit::Popup::DisplayStateChangeSignalType mShownSignal;
  Toolkit::Popup::DisplayStateChangeSignalType mHidingSignal;
  Toolkit::Popup::DisplayStateChangeSignalType mHiddenSignal;

  Layer mLayer;                                      ///< Popup Layer (i.e. Dim backing and PopupBg reside in this).
  Toolkit::TableView mPopupLayout;                   ///< Popup Background (i.e. dialog reside in this).
  Toolkit::Control mBacking;                         ///< Backing actor (dim effect).
  Actor mPreviousFocusedActor;                       ///< Store the previous focused actor to restore the focus when popup hide.
  Actor mTailImage;                                  ///< Stores the tail image.
  Actor mPopupContainer;                             ///< This actor is used to house the background image and the main popup layout.
  Animation mAnimation;                              ///< The current animation in use used to manage display state changing.
  bool mAlterAddedChild;                             ///< Flag used to control whether children are reparented or not.
  bool mLayoutDirty;                                 ///< Set to true whenever any property that would require a layout update is modified.
  Timer mAutoHideTimer;                              ///< Used to perform an auto-hide of the popup if desired.
  bool mTouchTransparent;                            ///< Allows all events to pass through the popup.

  // Main Content related properties:
  Actor mTitle;                                      ///< Stores the text title.
  Actor mContent;                                    ///< Stores the unselected content.
  Actor mFooter;                                     ///< Stores the footer content (typically controls).

  // Display related properties.
  Toolkit::Popup::DisplayState mDisplayState;        ///< The current display state of the popup.
  bool mTailVisible;                                 ///< True if the popup tail should be visible.
  Vector3 mTailPosition;                             ///< The position of the tail.
  Toolkit::Popup::ContextualMode mContextualMode;    ///< Allows the popup to be layed out adjacent to its parent in different directions.
  float mAnimationDuration;                          ///< The duration of the transition in and out animations.
  Toolkit::Popup::AnimationMode mAnimationMode;      ///< The animation to use to transition in and out.
  Dali::AnimationData mEntryAnimationData;           ///< Stores description data that can be used for generating a custom entry animation.
  Dali::AnimationData mExitAnimationData;            ///< Stores description data that can be used for generating a custom exit animation.
  unsigned int mAutoHideDelay;                       ///< If set, will auto-hide the popup after a specified amount of time.

  // Style related properties:
  bool mBackingEnabled;                              ///< True if a dimmed backing will be used.
  Vector4 mBackingColor;                             ///< The color of the backing.
  Actor mPopupBackgroundImage;                       ///< Stores the background image.
  Rect<int> mBackgroundBorder;                       ///< Background border.
  float mMargin;                                     ///< Internal margin for popup contents.
  std::string mTailUpImage;                          ///< Image used for the tail for the up direction.
  std::string mTailDownImage;                        ///< Image used for the tail for the down direction.
  std::string mTailLeftImage;                        ///< Image used for the tail for the left direction.
  std::string mTailRightImage;                       ///< Image used for the tail for the right direction.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::Popup& GetImpl( Toolkit::Popup& publicObject )
{
  DALI_ASSERT_ALWAYS( publicObject );

  Dali::RefObject& handle = publicObject.GetImplementation();

  return static_cast<Toolkit::Internal::Popup&>( handle );
}

inline const Toolkit::Internal::Popup& GetImpl( const Toolkit::Popup& publicObject )
{
  DALI_ASSERT_ALWAYS( publicObject );

  const Dali::RefObject& handle = publicObject.GetImplementation();

  return static_cast<const Toolkit::Internal::Popup&>( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_POPUP_H__
