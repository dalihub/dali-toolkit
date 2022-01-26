#ifndef DALI_TOOLKIT_INTERNAL_KEYBOARD_FOCUS_MANAGER_H
#define DALI_TOOLKIT_INTERNAL_KEYBOARD_FOCUS_MANAGER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/focus-manager/keyboard-focus-manager-devel.h>
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>

namespace Dali
{
namespace Integration
{
class SceneHolder;

} // namespace Integration

namespace Toolkit
{
namespace Internal
{
/**
 * @copydoc Toolkit::KeyboardFocusManager
 */
class KeyboardFocusManager : public Dali::BaseObject, public ConnectionTracker
{
public:
  typedef Toolkit::DevelKeyboardFocusManager::CustomAlgorithmInterface CustomAlgorithmInterface;

  enum FocusIndicatorState
  {
    UNKNOWN = -1, ///< Unknown state
    HIDE    = 0,  ///< FocusIndicator is hidden
    SHOW    = 1,  ///< FocusIndicator is shown
  };

  enum EnableFocusedIndicatorState
  {
    DISABLE = 0, ///< FocusIndicator is disable
    ENABLE  = 1, ///< FocusIndicator is enable
  };

  enum FocusedIndicatorModeState
  {
    NONE        = 0, ///< Set nothing
    ALWAYS_SHOW = 1, ///< FocusIndicator is always shown
  };

  /**
   * @copydoc Toolkit::KeyboardFocusManager::Get
   */
  static Toolkit::KeyboardFocusManager Get();

  /**
   * Construct a new KeyboardFocusManager.
   */
  KeyboardFocusManager();

  /**
   * @copydoc Toolkit::KeyboardFocusManager::SetCurrentFocusActor
   */
  bool SetCurrentFocusActor(Actor actor);

  /**
   * @copydoc Toolkit::KeyboardFocusManager::GetCurrentFocusActor
   */
  Actor GetCurrentFocusActor();

  /**
   * @copydoc Toolkit::KeyboardFocusManager::MoveFocus
   */
  bool MoveFocus(Toolkit::Control::KeyboardFocus::Direction direction);

  /**
   * @copydoc Toolkit::KeyboardFocusManager::ClearFocus
   */
  void ClearFocus();

  /**
   * @copydoc Toolkit::KeyboardFocusManager::SetAsFocusGroup
   */
  void SetAsFocusGroup(Actor actor, bool isFocusGroup);

  /**
   * @copydoc Toolkit::KeyboardFocusManager::IsFocusGroup
   */
  bool IsFocusGroup(Actor actor) const;

  /**
   * @copydoc Toolkit::KeyboardFocusManager::GetFocusGroup
   */
  Actor GetFocusGroup(Actor actor);

  /**
   * @copydoc Toolkit::KeyboardFocusManager::SetFocusGroupLoop
   */
  void SetFocusGroupLoop(bool enabled);

  /**
   * @copydoc Toolkit::KeyboardFocusManager::GetFocusGroupLoop
   */
  bool GetFocusGroupLoop() const;

  /**
   * @copydoc Toolkit::KeyboardFocusManager::SetFocusIndicatorActor
   */
  void SetFocusIndicatorActor(Actor indicator);

  /**
   * @copydoc Toolkit::KeyboardFocusManager::GetFocusIndicatorActor
   */
  Actor GetFocusIndicatorActor();

  /**
   * Move current focus to backward
   */
  void MoveFocusBackward();

  /**
   * @copydoc Toolkit::DevelKeyboardFocusManager::SetCustomAlgorithm
   */
  void SetCustomAlgorithm(CustomAlgorithmInterface& interface);

  /**
   * @copydoc Toolkit::DevelKeyboardFocusManager::UseFocusIndicator
   */
  void EnableFocusIndicator(bool enable);

  /**
   * @copydoc Toolkit::DevelKeyboardFocusManager::UseFocusIndicator
   */
  bool IsFocusIndicatorEnabled() const;

  /**
   * @copydoc Toolkit::DevelKeyboardFocusManager::EnableDefaultAlgorithm
   */
  void EnableDefaultAlgorithm(bool enable);

  /**
   * @copydoc Toolkit::DevelKeyboardFocusManager::IsDefaultAlgorithmEnabled
   */
  bool IsDefaultAlgorithmEnabled() const;

public:
  /**
   * @copydoc Toolkit::KeyboardFocusManager::PreFocusChangeSignal()
   */
  Toolkit::KeyboardFocusManager::PreFocusChangeSignalType& PreFocusChangeSignal();

  /**
   * @copydoc Toolkit::KeyboardFocusManager::FocusChangedSignal()
   */
  Toolkit::KeyboardFocusManager::FocusChangedSignalType& FocusChangedSignal();

  /**
   * @copydoc Toolkit::KeyboardFocusManager::FocusGroupChangedSignal()
   */
  Toolkit::KeyboardFocusManager::FocusGroupChangedSignalType& FocusGroupChangedSignal();

  /**
   * @copydoc Toolkit::KeyboardFocusManager::FocusedActorEnterKeySignal()
   */
  Toolkit::KeyboardFocusManager::FocusedActorEnterKeySignalType& FocusedActorEnterKeySignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor);

protected:
  /**
   * Destructor
   */
  virtual ~KeyboardFocusManager();

private:
  typedef std::vector<WeakHandle<Actor> > FocusStack;         ///< Define Dali::Vector< Dali::BaseObject* > as FocusStack to contain focus history
  typedef FocusStack::iterator            FocusStackIterator; ///< Define FocusStack::Iterator as FocusStackIterator to navigate FocusStack

  /**
   * This will be called when the adaptor is initialized
   */
  void OnAdaptorInit();

  /**
   * This will be called when a new scene holder is created
   * @param sceneHolder The new scene holder
   */
  void OnSceneHolderCreated(Dali::Integration::SceneHolder& sceneHolder);

  /**
   * Get configuration from StyleManager.
   */
  void GetConfigurationFromStyleManger();

  /**
   * Get the focus group of current focused actor.
   * @pre The FocusManager has been initialized.
   * @return A handle to the parent of the current focused actor which is a focus group,
   * or an empty handle if no actor is focused.
   */
  Actor GetCurrentFocusGroup();

  /**
   * Move the focus to the specified actor and send notification for the focus change.
   * @param actor The actor to be queried
   * @return Whether the focus is successful or not
   */
  bool DoSetCurrentFocusActor(Actor actor);

  /**
   * Move the focus to the next actor towards the specified direction within the layout control
   * @param control The layout control to move the focus in
   * @param actor The current focused actor
   * @param direction The direction of focus movement
   * @return Whether the focus is successful or not
   */
  bool DoMoveFocusWithinLayoutControl(Toolkit::Control control, Actor actor, Toolkit::Control::KeyboardFocus::Direction direction);

  /**
   * Move the focus to the first focusable actor in the next focus group in the forward
   * or backward direction. The "Tab" key changes the focus group in the forward direction
   * and the "Shift-Tab" key changes it in the backward direction.
   * @param forward Whether the direction of focus group change is forward or backward
   * @return Whether the focus group change is successful or not
   */
  bool DoMoveFocusToNextFocusGroup(bool forward);

  /**
   * Enter has been pressed on the actor. If the actor is control, call the OnKeybaordEnter virtual function.
   * This function will emit FocusedActorEnterKeySignal.
   * @param actor The actor to notify
   */
  void DoKeyboardEnter(Actor actor);

  /**
   * Check whether the actor is a layout control that supports two dimensional keyboard navigation.
   * The layout control needs to internally set the focus order for the child actor and be able to
   * tell KeyboardFocusmanager the next focusable actor in the given direction.
   * @pre The KeyboardFocusManager has been initialized.
   * @pre The Actor has been initialized.
   * @param actor The actor to be checked.
   * @return Whether the actor is a layout control or not.
   */
  bool IsLayoutControl(Actor actor) const;

  /**
   * Returns the closest ancestor of the given actor that is a layout control.
   * @param actor The actor to be checked for its parent layout control
   * @return The parent layout control the given actor belongs to or an empty handle if the given actor doesn't belong to a layout control
   */
  Toolkit::Control GetParentLayoutControl(Actor actor) const;

  /**
   * Callback for the key event when no actor in the stage has gained the key input focus
   * @param[in] event The KeyEvent event.
   */
  void OnKeyEvent(const KeyEvent& event);

  /**
   * Callback for the touch event when the screen is touched and when the touch ends
   * (i.e. the down & up touch events only).
   * @param[in] touch The touch information
   */
  void OnTouch(const TouchEvent& touch);

  /**
   * Callback for the wheel event when the custom wheel event occurs.
   * @param[in] wheel The WheelEvent information
   */
  bool OnWheelEvent(const WheelEvent& wheel);

  /**
   * Called when the window focus is changed.
   * @param[in] window The window whose focus is changed
   * @param[in] focusIn Whether the focus is in/out
   */
  void OnWindowFocusChanged(Window window, bool focusIn);

  /**
   * Get the focus Actor from current window
   */
  Actor GetFocusActorFromCurrentWindow();

  /**
   * Recursively deliver events to the control and its parents, until the event is consumed or the stage is reached.
   * @param[in]  actor  The actor got WheelEvent.
   * @param[in]  event  The WheelEvent.
   * @return True if WheelEvent is consumed.
   */
  bool EmitWheelSignals(Actor actor, const WheelEvent& event);

private:
  // Undefined
  KeyboardFocusManager(const KeyboardFocusManager&);

  KeyboardFocusManager& operator=(const KeyboardFocusManager& rhs);

private:
  Toolkit::KeyboardFocusManager::PreFocusChangeSignalType       mPreFocusChangeSignal;       ///< The signal to notify the focus will be changed
  Toolkit::KeyboardFocusManager::FocusChangedSignalType         mFocusChangedSignal;         ///< The signal to notify the focus change
  Toolkit::KeyboardFocusManager::FocusGroupChangedSignalType    mFocusGroupChangedSignal;    ///< The signal to notify the focus group change
  Toolkit::KeyboardFocusManager::FocusedActorEnterKeySignalType mFocusedActorEnterKeySignal; ///< The signal to notify that enter has been pressed on the focused actor

  WeakHandle<Actor> mCurrentFocusActor; ///< A weak handle to the current focused actor

  Actor mFocusIndicatorActor; ///< The focus indicator actor shared by all the keyboard focusable actors for highlight

  FocusStack mFocusHistory; ///< Stack to contain pre-focused actor's BaseObject*

  SlotDelegate<KeyboardFocusManager> mSlotDelegate;

  CustomAlgorithmInterface* mCustomAlgorithmInterface; ///< The user's (application / toolkit) implementation of CustomAlgorithmInterface

  typedef std::vector<std::pair<WeakHandle<Layer>, WeakHandle<Actor> > > FocusActorContainer;

  FocusActorContainer mCurrentFocusActors; ///< A container of focused actors

  WeakHandle<Layer> mCurrentFocusedWindow; ///< A weak handle to the current focused window's root layer

  FocusIndicatorState mIsFocusIndicatorShown; ///< Whether indicator should be shown / hidden when getting focus. It could be enabled when keyboard focus feature is enabled and navigation keys or 'Tab' key are pressed.

  EnableFocusedIndicatorState mEnableFocusIndicator; ///< Whether use focus indicator

  FocusedIndicatorModeState mAlwaysShowIndicator; ///< Whether always show indicator. If true, the indicator would be directly shown when focused

  bool mFocusGroupLoopEnabled : 1; ///< Whether the focus movement is looped within the same focus group

  bool mIsWaitingKeyboardFocusChangeCommit : 1; /// A flag to indicate PreFocusChangeSignal emitted but the proposed focus actor is not commited by the application yet.

  bool mClearFocusOnTouch : 1; ///< Whether clear focus on touch.

  bool mEnableDefaultAlgorithm : 1; ///< Whether use default algorithm focus
};

} // namespace Internal

inline Internal::KeyboardFocusManager& GetImpl(Dali::Toolkit::KeyboardFocusManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::KeyboardFocusManager&>(handle);
}

inline const Internal::KeyboardFocusManager& GetImpl(const Dali::Toolkit::KeyboardFocusManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::KeyboardFocusManager&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_KEYBOARD_FOCUS_MANAGER_H
