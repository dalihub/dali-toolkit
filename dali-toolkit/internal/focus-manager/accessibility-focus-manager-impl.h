#ifndef __DALI_TOOLKIT_INTERNAL_ACCESSIBILITY_FOCUS_MANAGER_H__
#define __DALI_TOOLKIT_INTERNAL_ACCESSIBILITY_FOCUS_MANAGER_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/devel-api/adaptor-framework/accessibility-action-handler.h>
#include <dali/devel-api/adaptor-framework/accessibility-gesture-handler.h>
#include <dali/public-api/object/base-object.h>
#include <dali/integration-api/events/pan-gesture-event.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/focus-manager/accessibility-focus-manager.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class AccessibilityFocusManager;

/**
 * @copydoc Toolkit::AccessibilityFocusManager
 */
class AccessibilityFocusManager : public Dali::BaseObject, Dali::AccessibilityActionHandler, Dali::AccessibilityGestureHandler
{
public:

  struct ActorAdditionalInfo
  {
    ActorAdditionalInfo()
    : mFocusOrder(0)
    {
    }

    unsigned int mFocusOrder; ///< The focus order of the actor. It is undefined by default.

    std::string mAccessibilityAttributes[Toolkit::AccessibilityFocusManager::ACCESSIBILITY_ATTRIBUTE_NUM]; ///< The array of attribute texts
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
   * Construct a new AccessibilityFocusManager.
   */
  AccessibilityFocusManager();

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::SetAccessibilityAttribute
   */
  void SetAccessibilityAttribute(Actor actor, Toolkit::AccessibilityFocusManager::AccessibilityAttribute type, const std::string& text);

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::GetAccessibilityAttribute
   */
  std::string GetAccessibilityAttribute(Actor actor, Toolkit::AccessibilityFocusManager::AccessibilityAttribute type) const;

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::SetFocusOrder
   */
  void SetFocusOrder(Actor actor, const unsigned int order);

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::GetFocusOrder
   */
  unsigned int GetFocusOrder(Actor actor) const;

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::GenerateNewFocusOrder
   */
  unsigned int GenerateNewFocusOrder() const;

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::GetActorByFocusOrder
   */
  Actor GetActorByFocusOrder(const unsigned int order);

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::SetCurrentFocusActor
   */
  bool SetCurrentFocusActor(Actor actor);

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::GetCurrentFocusActor
   */
  Actor GetCurrentFocusActor();

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::GetCurrentFocusGroup
   */
  Actor GetCurrentFocusGroup();

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::GetCurrentFocusOrder
   */
  unsigned int GetCurrentFocusOrder();

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::MoveFocusForward
   */
  bool MoveFocusForward();

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::MoveFocusBackward
   */
  bool MoveFocusBackward();

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::ClearFocus
   */
  void ClearFocus();

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::Reset
   */
  void Reset();

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::SetFocusGroup
   */
  void SetFocusGroup(Actor actor, bool isFocusGroup);

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::IsFocusGroup
   */
  bool IsFocusGroup(Actor actor) const;

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::SetGroupMode
   */
  void SetGroupMode(bool enabled);

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::GetGroupMode
   */
  bool GetGroupMode() const;

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::SetWrapMode
   */
  void SetWrapMode(bool wrapped);

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::GetWrapMode
   */
  bool GetWrapMode() const;

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::SetFocusIndicatorActor
   */
  void SetFocusIndicatorActor(Actor indicator);

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::GetFocusIndicatorActor
   */
  Actor GetFocusIndicatorActor();

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::GetFocusGroup
   */
  Actor GetFocusGroup(Actor actor);

public:

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::FocusChangedSignal()
   */
  Toolkit::AccessibilityFocusManager::FocusChangedSignalType& FocusChangedSignal();

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::FocusOvershotSignal()
   */
  Toolkit::AccessibilityFocusManager::FocusOvershotSignalType& FocusOvershotSignal();

  /**
   * @copydoc Toolkit::AccessibilityFocusManager::FocusedActorActivatedSignal()
   */
  Toolkit::AccessibilityFocusManager::FocusedActorActivatedSignalType& FocusedActorActivatedSignal();

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
  virtual ~AccessibilityFocusManager();

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
  AccessibilityFocusManager(const AccessibilityFocusManager&);

  AccessibilityFocusManager& operator=(const AccessibilityFocusManager& rhs);

private:

  Toolkit::AccessibilityFocusManager::FocusChangedSignalType mFocusChangedSignal; ///< The signal to notify the focus change
  Toolkit::AccessibilityFocusManager::FocusOvershotSignalType mFocusOvershotSignal; ///< The signal to notify the focus overshooted
  Toolkit::AccessibilityFocusManager::FocusedActorActivatedSignalType mFocusedActorActivatedSignal; ///< The signal to notify the activation of focused actor

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

inline Internal::AccessibilityFocusManager& GetImpl(Dali::Toolkit::AccessibilityFocusManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::AccessibilityFocusManager&>(handle);
}

inline const Internal::AccessibilityFocusManager& GetImpl(const Dali::Toolkit::AccessibilityFocusManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::AccessibilityFocusManager&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_ACCESSIBILITY_FOCUS_MANAGER_H__
