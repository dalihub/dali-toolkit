#ifndef __DALI_TOOLKIT_INTERNAL_ACCESSIBILITY_MANAGER_H__
#define __DALI_TOOLKIT_INTERNAL_ACCESSIBILITY_MANAGER_H__

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
#include <dali-toolkit/public-api/accessibility-manager/accessibility-manager.h>
#include <dali/public-api/adaptor-framework/tts-player.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class AccessibilityManager;

/**
 * @copydoc Toolkit::AccessibilityManager
 */
class AccessibilityManager : public Dali::BaseObject, Dali::AccessibilityActionHandler, Dali::AccessibilityGestureHandler, public Dali::ConnectionTracker
{
public:

  typedef Dali::Toolkit::AccessibilityManager::AccessibilityActionSignalType       AccessibilityActionSignalType;
  typedef Dali::Toolkit::AccessibilityManager::AccessibilityActionScrollSignalType AccessibilityActionScrollSignalType;

  struct ActorAdditionalInfo
  {
    ActorAdditionalInfo()
    : mFocusOrder(0)
    {
    }

    unsigned int mFocusOrder; ///< The focus order of the actor. It is undefined by default.

    std::string mAccessibilityAttributes[Toolkit::AccessibilityManager::ACCESSIBILITY_ATTRIBUTE_NUM]; ///< The array of attribute texts
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
   * Construct a new AccessibilityManager.
   */
  AccessibilityManager();

  /**
   * Initialise the AccessibilityManager
   */
  void Initialise();

  /**
   * @copydoc Toolkit::AccessibilityManager::SetAccessibilityAttribute
   */
  void SetAccessibilityAttribute(Actor actor, Toolkit::AccessibilityManager::AccessibilityAttribute type, const std::string& text);

  /**
   * @copydoc Toolkit::AccessibilityManager::GetAccessibilityAttribute
   */
  std::string GetAccessibilityAttribute(Actor actor, Toolkit::AccessibilityManager::AccessibilityAttribute type) const;

  /**
   * @copydoc Toolkit::AccessibilityManager::SetFocusOrder
   */
  void SetFocusOrder(Actor actor, const unsigned int order);

  /**
   * @copydoc Toolkit::AccessibilityManager::GetFocusOrder
   */
  unsigned int GetFocusOrder(Actor actor) const;

  /**
   * @copydoc Toolkit::AccessibilityManager::GenerateNewFocusOrder
   */
  unsigned int GenerateNewFocusOrder() const;

  /**
   * @copydoc Toolkit::AccessibilityManager::GetActorByFocusOrder
   */
  Actor GetActorByFocusOrder(const unsigned int order);

  /**
   * @copydoc Toolkit::AccessibilityManager::SetCurrentFocusActor
   */
  bool SetCurrentFocusActor(Actor actor);

  /**
   * @copydoc Toolkit::AccessibilityManager::GetCurrentFocusActor
   */
  Actor GetCurrentFocusActor();

  /**
   * @copydoc Toolkit::AccessibilityManager::GetCurrentFocusGroup
   */
  Actor GetCurrentFocusGroup();

  /**
   * @copydoc Toolkit::AccessibilityManager::GetCurrentFocusOrder
   */
  unsigned int GetCurrentFocusOrder();

  /**
   * @copydoc Toolkit::AccessibilityManager::MoveFocusForward
   */
  bool MoveFocusForward();

  /**
   * @copydoc Toolkit::AccessibilityManager::MoveFocusBackward
   */
  bool MoveFocusBackward();

  /**
   * @copydoc Toolkit::AccessibilityManager::ClearFocus
   */
  void ClearFocus();

  /**
   * @copydoc Toolkit::AccessibilityManager::Reset
   */
  void Reset();

  /**
   * @copydoc Toolkit::AccessibilityManager::SetFocusGroup
   */
  void SetFocusGroup(Actor actor, bool isFocusGroup);

  /**
   * @copydoc Toolkit::AccessibilityManager::IsFocusGroup
   */
  bool IsFocusGroup(Actor actor) const;

  /**
   * @copydoc Toolkit::AccessibilityManager::SetGroupMode
   */
  void SetGroupMode(bool enabled);

  /**
   * @copydoc Toolkit::AccessibilityManager::GetGroupMode
   */
  bool GetGroupMode() const;

  /**
   * @copydoc Toolkit::AccessibilityManager::SetWrapMode
   */
  void SetWrapMode(bool wrapped);

  /**
   * @copydoc Toolkit::AccessibilityManager::GetWrapMode
   */
  bool GetWrapMode() const;

  /**
   * @copydoc Toolkit::AccessibilityManager::SetFocusIndicatorActor
   */
  void SetFocusIndicatorActor(Actor indicator);

  /**
   * @copydoc Toolkit::AccessibilityManager::GetFocusIndicatorActor
   */
  Actor GetFocusIndicatorActor();

  /**
   * @copydoc Toolkit::AccessibilityManager::GetFocusGroup
   */
  Actor GetFocusGroup(Actor actor);

  /**
   * @copydoc Toolkit::AccessibilityManager::GetReadPosition
   */
  Vector2 GetReadPosition() const;

public:

  /**
   * @copydoc Toolkit::AccessibilityManager::FocusChangedSignal()
   */
  Toolkit::AccessibilityManager::FocusChangedSignalType& FocusChangedSignal();

  /**
   * @copydoc Toolkit::AccessibilityManager::FocusOvershotSignal()
   */
  Toolkit::AccessibilityManager::FocusOvershotSignalType& FocusOvershotSignal();

  /**
   * @copydoc Toolkit::AccessibilityManager::FocusedActorActivatedSignal()
   */
  Toolkit::AccessibilityManager::FocusedActorActivatedSignalType& FocusedActorActivatedSignal();

public:  // Signals

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::StatusChangedSignal
   */
  AccessibilityActionSignalType& StatusChangedSignal()
  {
    return mStatusChangedSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::AccessibilityAdaptor::ActionNextSignal
   */
  AccessibilityActionSignalType& ActionNextSignal()
  {
    return mActionNextSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionPreviousSignal
   */
  AccessibilityActionSignalType& ActionPreviousSignal()
  {
    return mActionPreviousSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionActivateSignal
   */
  AccessibilityActionSignalType& ActionActivateSignal()
  {
    return mActionActivateSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionOverSignal
   */
  AccessibilityActionSignalType& ActionOverSignal()
  {
    return mActionOverSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionReadSignal
   */
  AccessibilityActionSignalType& ActionReadSignal()
  {
    return mActionReadSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionReadNextSignal
   */
  AccessibilityActionSignalType& ActionReadNextSignal()
  {
    return mActionReadNextSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionReadPreviousSignal
   */
  AccessibilityActionSignalType& ActionReadPreviousSignal()
  {
    return mActionReadPreviousSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionUpSignal
   */
  AccessibilityActionSignalType& ActionUpSignal()
  {
    return mActionUpSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionDownSignal
   */
  AccessibilityActionSignalType& ActionDownSignal()
  {
    return mActionDownSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionClearFocusSignal
   */
  AccessibilityActionSignalType& ActionClearFocusSignal()
  {
    return mActionClearFocusSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionBackSignal
   */
  AccessibilityActionSignalType& ActionBackSignal()
  {
    return mActionBackSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionScrollUpSignal
   */
  AccessibilityActionSignalType& ActionScrollUpSignal()
  {
    return mActionScrollUpSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionScrollDownSignal
   */
  AccessibilityActionSignalType& ActionScrollDownSignal()
  {
    return mActionScrollDownSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionPageLeftSignal
   */
  AccessibilityActionSignalType& ActionPageLeftSignal()
  {
    return mActionPageLeftSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionPageRightSignal
   */
  AccessibilityActionSignalType& ActionPageRightSignal()
  {
    return mActionPageRightSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionPageUpSignal
   */
  AccessibilityActionSignalType& ActionPageUpSignal()
  {
    return mActionPageUpSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionPageDownSignal
   */
  AccessibilityActionSignalType& ActionPageDownSignal()
  {
    return mActionPageDownSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionMoveToFirstSignal
   */
  AccessibilityActionSignalType& ActionMoveToFirstSignal()
  {
    return mActionMoveToFirstSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionMoveToLastSignal
   */
  AccessibilityActionSignalType& ActionMoveToLastSignal()
  {
    return mActionMoveToLastSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionReadFromTopSignal
   */
  AccessibilityActionSignalType& ActionReadFromTopSignal()
  {
    return mActionReadFromTopSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionReadFromNextSignal
   */
  AccessibilityActionSignalType& ActionReadFromNextSignal()
  {
    return mActionReadFromNextSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionZoomSignal
   */
  AccessibilityActionSignalType& ActionZoomSignal()
  {
    return mActionZoomSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionReadIndicatorInformationSignal
   */
  AccessibilityActionSignalType& ActionReadIndicatorInformationSignal()
  {
    return mActionReadIndicatorInformationSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionReadPauseResumeSignal
   */
  AccessibilityActionSignalType& ActionReadPauseResumeSignal()
  {
    return mActionReadPauseResumeSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionStartStopSignal
   */
  AccessibilityActionSignalType& ActionStartStopSignal()
  {
    return mActionStartStopSignal;
  }

  /**
   * @copydoc Dali::Toolkit::AccessibilityManager::ActionScrollSignal
   */
  AccessibilityActionScrollSignalType& ActionScrollSignal()
  {
    return mActionScrollSignal;
  }

protected:

  /**
   * Destructor
   */
  virtual ~AccessibilityManager();

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
   * Activate the actor. If the actor is control, call OnAccessibilityActivated virtual function.
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
   * Perform the accessibility action associated with a scroll event.
   * @param touchEvent The touch point (and time) of the event.
   * @return whether the focus is cleared or not.
   */
  virtual bool AccessibilityActionScroll( Dali::TouchEvent& touchEvent );

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
   * Perform the accessibility action to scroll up the list and focus on the first item on the list
   * after the scrolling and read the item (by two finger swipe up).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionScrollUp();

  /**
   * Perform the accessibility action to scroll down the list and focus on the first item on the list
   * after the scrolling and read the item (by two finger swipe down).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionScrollDown();

  /**
   * Perform the accessibility action to scroll left to the previous page (by two finger swipe left).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionPageLeft();

  /**
   * Perform the accessibility action to scroll right to the next page (by two finger swipe right).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionPageRight();

  /**
   * Perform the accessibility action to scroll up to the previous page (by one finger swipe left and right).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionPageUp();

  /**
   * Perform the accessibility action to scroll down to the next page (by one finger swipe right and left).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionPageDown();

  /**
   * Perform the accessibility action to move the focus to the first item on the screen
   * (by one finger swipe up and down).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionMoveToFirst();

  /**
   * Perform the accessibility action to move the focus to the last item on the screen
   * (by one finger swipe down and up).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionMoveToLast();

  /**
   * Perform the accessibility action to move the focus to the first item on the top
   * and read from the top item continously (by three fingers single tap).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionReadFromTop();

  /**
   * Perform the accessibility action to move the focus to and read from the next item
   * continously (by three fingers double tap).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionReadFromNext();

  /**
   * Perform the accessibility action to move the focus to do the zooming (by one finger triple tap).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionZoom();

  /**
   * Perform the accessibility action to read the information in the indicator (by two fingers triple tap).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionReadIndicatorInformation();

  /**
   * Perform the accessibility action to pause/resume the current read out (by two fingers single tap).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionReadPauseResume();

  /**
   * Perform the accessibility action to start/stop the current action (by two fingers double tap).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionStartStop();

  /**
   * Perform the accessibility action to mouse move (by one finger tap & hold and move).
   * @param touchEvent touch event structure
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionTouch(const TouchEvent& touchEvent);

  /**
   * This function is connected to the TtsPlayer StateChangeSignal.
   * It is called when the TTS players state changes.
   * @param previousState The previous state of the TTS player (for comparison)
   * @param currentState  The current state of the TTS player
   */
  void TtsStateChanged( const Dali::TtsPlayer::State previousState, const Dali::TtsPlayer::State currentState );

private:

  // Undefined
  AccessibilityManager(const AccessibilityManager&);

  AccessibilityManager& operator=(const AccessibilityManager& rhs);

private:

  Toolkit::AccessibilityManager::FocusChangedSignalType mFocusChangedSignal; ///< The signal to notify the focus change
  Toolkit::AccessibilityManager::FocusOvershotSignalType mFocusOvershotSignal; ///< The signal to notify the focus overshooted
  Toolkit::AccessibilityManager::FocusedActorActivatedSignalType mFocusedActorActivatedSignal; ///< The signal to notify the activation of focused actor

  // Action signals.
  AccessibilityActionSignalType       mStatusChangedSignal;
  AccessibilityActionSignalType       mActionNextSignal;
  AccessibilityActionSignalType       mActionPreviousSignal;
  AccessibilityActionSignalType       mActionActivateSignal;
  AccessibilityActionSignalType       mActionOverSignal;
  AccessibilityActionSignalType       mActionReadSignal;
  AccessibilityActionSignalType       mActionReadNextSignal;
  AccessibilityActionSignalType       mActionReadPreviousSignal;
  AccessibilityActionSignalType       mActionUpSignal;
  AccessibilityActionSignalType       mActionDownSignal;
  AccessibilityActionSignalType       mActionClearFocusSignal;
  AccessibilityActionSignalType       mActionBackSignal;
  AccessibilityActionSignalType       mActionScrollUpSignal;
  AccessibilityActionSignalType       mActionScrollDownSignal;
  AccessibilityActionSignalType       mActionPageLeftSignal;
  AccessibilityActionSignalType       mActionPageRightSignal;
  AccessibilityActionSignalType       mActionPageUpSignal;
  AccessibilityActionSignalType       mActionPageDownSignal;
  AccessibilityActionSignalType       mActionMoveToFirstSignal;
  AccessibilityActionSignalType       mActionMoveToLastSignal;
  AccessibilityActionSignalType       mActionReadFromTopSignal;
  AccessibilityActionSignalType       mActionReadFromNextSignal;
  AccessibilityActionSignalType       mActionZoomSignal;
  AccessibilityActionSignalType       mActionReadIndicatorInformationSignal;
  AccessibilityActionSignalType       mActionReadPauseResumeSignal;
  AccessibilityActionSignalType       mActionStartStopSignal;
  AccessibilityActionScrollSignalType mActionScrollSignal;

  FocusIDContainer mFocusIDContainer;       ///< The container to look up actor ID by focus order
  IDAdditionalInfoContainer mIDAdditionalInfoContainer; ///< The container to look up additional information by actor ID
  FocusIDPair mCurrentFocusActor;           ///< The focus order and actor ID of current focused actor
  Actor mCurrentGesturedActor;              ///< The actor that will handle the gesture
  Actor mFocusIndicatorActor;               ///< The focus indicator actor shared by all the focusable actors for highlight
  Vector2 mPreviousPosition;                ///< The previous pan position; useful for calculating velocity for Gesture::Finished events
  unsigned int mRecursiveFocusMoveCounter;  ///< The counter to count the number of recursive focus movement attempted before the focus movement is successful.

  bool mIsWrapped:1;                        ///< Whether the focus movement is wrapped around or not
  bool mIsFocusWithinGroup:1;               ///< Whether the focus movement is limited to the current focus group or not
  bool mIsEndcapFeedbackEnabled:1;          ///< Whether the endcap feedback need to be played when the focus leaves the end or vice versa
  bool mIsEndcapFeedbackPlayed:1;           ///< Whether the endcap feedback was played or not
  bool mIsAccessibilityTtsEnabled:1;        ///< Whether accessibility feature(screen-reader) turned on/off
  bool mTtsCreated:1;                       ///< Whether the TTS Player has been accessed
  bool mIsFocusIndicatorEnabled:1;          ///< Whether indicator should be shown / hidden. It could be enabled when TTS enabled or 'Tab' key operated.
  bool mContinuousPlayMode:1;               ///< Keeps track of whether or not we are in continuous play mode

};

} // namespace Internal

inline Internal::AccessibilityManager& GetImpl(Dali::Toolkit::AccessibilityManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::AccessibilityManager&>(handle);
}

inline const Internal::AccessibilityManager& GetImpl(const Dali::Toolkit::AccessibilityManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::AccessibilityManager&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_ACCESSIBILITY_MANAGER_H__
