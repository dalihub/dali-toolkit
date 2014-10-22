#ifndef __DALI_TOOLKIT_INTERNAL_FOCUS_MANAGER_H__
#define __DALI_TOOLKIT_INTERNAL_FOCUS_MANAGER_H__

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
#include <string>
#include <dali/dali.h>
#include <dali/public-api/common/map-wrapper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/focus-manager/focus-manager.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class FocusManager;

/**
 * @copydoc Toolkit::FocusManager
 */
class FocusManager : public Dali::BaseObject, Dali::AccessibilityActionHandler, Dali::AccessibilityGestureHandler
{
public:

  struct ActorAdditionalInfo
  {
    ActorAdditionalInfo()
    : mFocusOrder(0)
    {
    }

    unsigned int mFocusOrder; ///< The focus order of the actor. It is undefined by default.

    std::string mAccessibilityAttributes[Toolkit::FocusManager::ACCESSIBILITY_ATTRIBUTE_NUM]; ///< The array of attribute texts
  };

  typedef std::pair<unsigned int, unsigned int>        FocusIDPair;
  typedef std::map<unsigned int, unsigned int>         FocusIDContainer;
  typedef FocusIDContainer::iterator                   FocusIDIter;
  typedef FocusIDContainer::const_iterator             FocusIDConstIter;

  typedef std::pair<unsigned int, ActorAdditionalInfo> IDAdditionalInfoPair;
  typedef std::map<unsigned int, ActorAdditionalInfo>  IDAdditionalInfoContainer;
  typedef IDAdditionalInfoContainer::iterator          IDAdditionalInfoIter;
  typedef IDAdditionalInfoContainer::const_iterator    IDAdditionalInfoConstIter;

  /**
   * Construct a new FocusManager.
   */
  FocusManager();

  /**
   * @copydoc Toolkit::FocusManager::SetAccessibilityAttribute
   */
  void SetAccessibilityAttribute(Actor actor, Toolkit::FocusManager::AccessibilityAttribute type, const std::string& text);

  /**
   * @copydoc Toolkit::FocusManager::GetAccessibilityAttribute
   */
  std::string GetAccessibilityAttribute(Actor actor, Toolkit::FocusManager::AccessibilityAttribute type) const;

  /**
   * @copydoc Toolkit::FocusManager::SetFocusOrder
   */
  void SetFocusOrder(Actor actor, const unsigned int order);

  /**
   * @copydoc Toolkit::FocusManager::GetFocusOrder
   */
  unsigned int GetFocusOrder(Actor actor) const;

  /**
   * @copydoc Toolkit::FocusManager::GenerateNewFocusOrder
   */
  unsigned int GenerateNewFocusOrder() const;

  /**
   * @copydoc Toolkit::FocusManager::GetActorByFocusOrder
   */
  Actor GetActorByFocusOrder(const unsigned int order);

  /**
   * @copydoc Toolkit::FocusManager::SetCurrentFocusActor
   */
  bool SetCurrentFocusActor(Actor actor);

  /**
   * @copydoc Toolkit::FocusManager::GetCurrentFocusActor
   */
  Actor GetCurrentFocusActor();

  /**
   * @copydoc Toolkit::FocusManager::GetCurrentFocusGroup
   */
  Actor GetCurrentFocusGroup();

  /**
   * @copydoc Toolkit::FocusManager::GetCurrentFocusOrder
   */
  unsigned int GetCurrentFocusOrder();

  /**
   * @copydoc Toolkit::FocusManager::MoveFocusForward
   */
  bool MoveFocusForward();

  /**
   * @copydoc Toolkit::FocusManager::MoveFocusBackward
   */
  bool MoveFocusBackward();

  /**
   * @copydoc Toolkit::FocusManager::ClearFocus
   */
  void ClearFocus();

  /**
   * @copydoc Toolkit::FocusManager::Reset
   */
  void Reset();

  /**
   * @copydoc Toolkit::FocusManager::SetFocusGroup
   */
  void SetFocusGroup(Actor actor, bool isFocusGroup);

  /**
   * @copydoc Toolkit::FocusManager::IsFocusGroup
   */
  bool IsFocusGroup(Actor actor) const;

  /**
   * @copydoc Toolkit::FocusManager::SetGroupMode
   */
  void SetGroupMode(bool enabled);

  /**
   * @copydoc Toolkit::FocusManager::GetGroupMode
   */
  bool GetGroupMode() const;

  /**
   * @copydoc Toolkit::FocusManager::SetWrapMode
   */
  void SetWrapMode(bool wrapped);

  /**
   * @copydoc Toolkit::FocusManager::GetWrapMode
   */
  bool GetWrapMode() const;

  /**
   * @copydoc Toolkit::FocusManager::SetFocusIndicatorActor
   */
  void SetFocusIndicatorActor(Actor indicator);

  /**
   * @copydoc Toolkit::FocusManager::GetFocusIndicatorActor
   */
  Actor GetFocusIndicatorActor();

  /**
   * @copydoc Toolkit::FocusManager::GetFocusGroup
   */
  Actor GetFocusGroup(Actor actor);

public:

  /**
   * @copydoc Toolkit::FocusManager::FocusChangedSignal()
   */
  Toolkit::FocusManager::FocusChangedSignalV2& FocusChangedSignal();

  /**
   * @copydoc Toolkit::FocusManager::FocusOvershotSignal()
   */
  Toolkit::FocusManager::FocusOvershotSignalV2& FocusOvershotSignal();

  /**
   * @copydoc Toolkit::FocusManager::FocusedActorActivatedSignal()
   */
  Toolkit::FocusManager::FocusedActorActivatedSignalV2& FocusedActorActivatedSignal();

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

protected:

  /**
   * Destructor
   */
  virtual ~FocusManager();

private:

  /**
   * Get the additional information (e.g. focus order and description) of the given actor.
   * @param actorID The ID of the actor to be queried
   * @return The additional information of the actor
   */
  ActorAdditionalInfo GetActorAdditionalInfo(const unsigned int actorID) const;

  /**
   * Synchronize the actor's additional information to reflect its latest focus order
   * @param actorID The ID of the actor
   * @param order The focus order of the actor
   * @return The additional information of the actor
   */
  void SynchronizeActorAdditionalInfo(const unsigned int actorID, const unsigned int order);

  /**
   * Move the focus to the specified actor and send notification for the focus change.
   * @param actorID The ID of the actor to be queried
   * @return Whether the focus is successful or not
   */
  bool DoSetCurrentFocusActor(const unsigned int actorID);

  /**
   * Move the focus to the next actor in the focus chain towards the specified direction.
   * @param focusIDIter The iterator pointing to the current focused actor
   * @param forward Whether the focus movement is forward or not. The focus movement will be backward if this is false.
   * @param wrapped Whether the focus shoule be moved wrapped around or not
   * @return Whether the focus is successful or not
   */
  bool DoMoveFocus(FocusIDIter focusIDIter, bool forward, bool wrapped);

  /**
   * Activate the actor. If the actor is control, call OnActivated virtual function.
   * This function will emit FocusedActorActivatedSignal.
   * @param actor The actor to activate
   */
  void DoActivate(Actor actor);

  /**
   * Create the default indicator actor to highlight the focused actor.
   */
  void CreateDefaultFocusIndicatorActor();

  /**
   * Set whether the actor is focusable or not. A focusable property will be registered for
   * the actor if not yet.
   * @param actor The actor to be focused
   * @param focusable Whether the actor is focusable or not
   */
  void SetFocusable(Actor actor, bool focusable);

  /**
   * Handle the accessibility pan gesture.
   * @param[in]  panEvent  The pan event to be handled.
   * @return whether the gesture is handled successfully or not.
   */
  virtual bool HandlePanGesture(const Integration::PanGestureEvent& panEvent);

  /**
   * Change the accessibility status when Accessibility feature(screen-reader) turned on or off.
   * @return whether the status is changed or not.
   */
  virtual bool ChangeAccessibilityStatus();

  /**
   * Clear the accessibility focus from the current focused actor.
   * @return whether the focus is cleared or not.
   */
  virtual bool ClearAccessibilityFocus();

  /**
   * Perform the accessibility action to move focus to the previous focusable actor (by one finger flick up).
   * @param allowEndFeedback true if end of list feedback should be played when the focus is alread reached to the end
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionPrevious(bool allowEndFeedback);

  /**
   * Perform the accessibility action to move focus to the next focusable actor (by one finger flick down).
   * @param allowEndFeedback true if end of list feedback should be played when the focus is alread reached to the end
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionNext(bool allowEndFeedback);

  /**
   * Perform the accessibility action to move focus to the previous focusable actor (by one finger flick left).
   * @param allowEndFeedback true if end of list feedback should be played when the focus is alread reached to the end
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionReadPrevious(bool allowEndFeedback);

  /**
   * Perform the accessibility action to move focus to the next focusable actor (by one finger flick right).
   * @param allowEndFeedback true if end of list feedback should be played when the focus is alread reached to the end
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionReadNext(bool allowEndFeedback);

  /**
   * Perform the accessibility action to focus and read the actor (by one finger tap or move).
   * @param allowReadAgain true if the action read again the same object (i.e. read action)
   *                       false if the action just read when the focus object is changed (i.e. over action)
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionRead(bool allowReadAgain);

  /**
   * Perform the accessibility action to activate the current focused actor (by one finger double tap).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionActivate();

  /**
   * Perform the accessibility action to change the value when the current focused actor is a slider
   * (by double finger down and move up and right).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionUp();

  /**
   * Perform the accessibility action to change the value when the current focused actor is a slider
   * (by double finger down and move down and left).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionDown();

  /**
   * Perform the accessibility action to navigate back (by two fingers circle draw).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionBack();

  /**
   * Perform the accessibility action to mouse move (by one finger tap & hold and move).
   * @param touchEvent touch event structure
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionTouch(const TouchEvent& touchEvent);

private:

  // Undefined
  FocusManager(const FocusManager&);

  FocusManager& operator=(const FocusManager& rhs);

private:

  Toolkit::FocusManager::FocusChangedSignalV2 mFocusChangedSignalV2; ///< The signal to notify the focus change
  Toolkit::FocusManager::FocusOvershotSignalV2 mFocusOvershotSignalV2; ///< The signal to notify the focus overshooted
  Toolkit::FocusManager::FocusedActorActivatedSignalV2 mFocusedActorActivatedSignalV2; ///< The signal to notify the activation of focused actor

  bool mIsWrapped; ///< Whether the focus movement is wrapped around or not
  bool mIsFocusWithinGroup; ///< Whether the focus movement is limited to the current focus group or not

  bool mIsEndcapFeedbackEnabled; ///< Whether the endcap feedback need to be played when the focus leaves the end or vice versa
  bool mIsEndcapFeedbackPlayed; ///< Whether the endcap feedback was played or not

  FocusIDContainer mFocusIDContainer; ///< The container to look up actor ID by focus order
  IDAdditionalInfoContainer mIDAdditionalInfoContainer; ///< The container to look up additional information by actor ID

  FocusIDPair mCurrentFocusActor; ///< The focus order and actor ID of current focused actor
  Actor mCurrentGesturedActor; ///< The actor that will handle the gesture

  Actor mFocusIndicatorActor; ///< The focus indicator actor shared by all the focusable actors for highlight

  Vector2 mPreviousPosition; ///< The previous pan position; useful for calculating velocity for Gesture::Finished events

  unsigned int mRecursiveFocusMoveCounter; ///< The counter to count the number of recursive focus movement attempted before the focus movement is successful.

  bool mIsAccessibilityTtsEnabled; ///< Whether accessibility feature(screen-reader) turned on/off

  bool mIsFocusIndicatorEnabled; ///< Whether indicator should be shown / hidden. It could be enabled when TTS enabled or 'Tab' key operated.
};

} // namespace Internal

inline Internal::FocusManager& GetImpl(Dali::Toolkit::FocusManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::FocusManager&>(handle);
}

inline const Internal::FocusManager& GetImpl(const Dali::Toolkit::FocusManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::FocusManager&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_FOCUS_MANAGER_H__
