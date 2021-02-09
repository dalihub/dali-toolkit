#ifndef DALI_TOOLKIT_INTERNAL_ACCESSIBILITY_MANAGER_H
#define DALI_TOOLKIT_INTERNAL_ACCESSIBILITY_MANAGER_H

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
#include <dali/public-api/object/base-object.h>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/accessibility-manager/accessibility-manager.h>

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
class AccessibilityManager : public Dali::BaseObject, public Dali::ConnectionTracker
{
public:
  typedef Dali::Toolkit::AccessibilityManager::AccessibilityActionSignalType       AccessibilityActionSignalType;
  typedef Dali::Toolkit::AccessibilityManager::AccessibilityActionScrollSignalType AccessibilityActionScrollSignalType;

  /**
   * Construct a new AccessibilityManager.
   */
  AccessibilityManager();

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
   * @copydoc Toolkit::AccessibilityManager::GetFocusGroup
   */
  Actor GetFocusGroup(Actor actor);

  /**
   * @copydoc Toolkit::AccessibilityManager::GetReadPosition
   */
  Vector2 GetReadPosition() const;

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

public: // Signals
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
  // Undefined
  AccessibilityManager(const AccessibilityManager&);

  AccessibilityManager& operator=(const AccessibilityManager& rhs);

private:
  std::vector<Actor> mFocusOrder;

  Toolkit::AccessibilityManager::FocusChangedSignalType          mFocusChangedSignal;          ///< The signal to notify the focus change
  Toolkit::AccessibilityManager::FocusOvershotSignalType         mFocusOvershotSignal;         ///< The signal to notify the focus overshooted
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

#endif // DALI_TOOLKIT_INTERNAL_ACCESSIBILITY_MANAGER_H
