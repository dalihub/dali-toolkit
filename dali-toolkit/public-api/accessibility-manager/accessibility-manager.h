#ifndef __DALI_TOOLKIT_ACCESSIBILITY_MANAGER_H__
#define __DALI_TOOLKIT_ACCESSIBILITY_MANAGER_H__

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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/image-actor.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class AccessibilityManager;
}
/**
 * @addtogroup dali_toolkit_accessibility_manager
 * @{
 */

/**
 * @brief Manages registration of actors in a accessibility focus chain and changing the
 * focused actor within that chain.
 *
 * This class provides the functionality of registering the focus order and description
 * of actors and maintaining the focus chain.
 *
 * It provides functionality of setting the
 * focus and moving the focus forward and backward. It also draws a highlight for the
 * focused actor and emits a signal when the focus is changed.
 *
 * Signals
 * | %Signal Name          | Method                             |
 * |-----------------------|------------------------------------|
 * | focusChanged          | @ref FocusChangedSignal()          |
 * | focusOvershot         | @ref FocusOvershotSignal()         |
 * | focusedActorActivated | @ref FocusedActorActivatedSignal() |
 */
class DALI_IMPORT_API AccessibilityManager : public BaseHandle
{
public:

  // Typedefs

  /**
   * @brief Accessibility Action Signal.
   *
   * The connected signal callback should return true if handled.
   */
  typedef Signal< bool ( AccessibilityManager& ) > AccessibilityActionSignalType; ///< Generic signal type
  typedef Signal< bool ( AccessibilityManager&, const Dali::TouchEvent& )> AccessibilityActionScrollSignalType; ///< Scroll signal type

  /**
   * @brief Accessibility needs four information which will be read by screen-reader.
   *
   * Reading order : Label -> Trait -> Optional (Value and Hint)
   */
  enum AccessibilityAttribute
  {
    ACCESSIBILITY_LABEL = 0, ///< Simple text which contained in ui-control
    ACCESSIBILITY_TRAIT,     ///< Description of ui-control trait
    ACCESSIBILITY_VALUE,     ///< Current value of ui-control (Optional)
    ACCESSIBILITY_HINT,      ///< Hint for action (Optional)
    ACCESSIBILITY_ATTRIBUTE_NUM ///< Number of attributes
  };

   /**
    * @brief Overshoot direction.
    */
  enum FocusOvershotDirection
  {
    OVERSHOT_PREVIOUS = -1, ///< Try to move previous of the first actor
    OVERSHOT_NEXT = 1,      ///< Try to move next of the last actor
  };

 public:

  /// @brief Focus changed signal
  typedef Signal< void ( Actor, Actor ) > FocusChangedSignalType;

  /// @brief Focus overshooted signal
  typedef Signal< void ( Actor, FocusOvershotDirection ) > FocusOvershotSignalType;

  /// @brief Focused actor activated signal
  typedef Signal< void ( Actor ) > FocusedActorActivatedSignalType;

  /**
   * @brief Create a AccessibilityManager handle; this can be initialised with AccessibilityManager::New().
   *
   * Calling member functions with an uninitialised handle is not allowed.
   */
  AccessibilityManager();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~AccessibilityManager();

  /**
   * @brief Get the singleton of AccessibilityManager object.
   *
   * @return A handle to the AccessibilityManager control.
   */
  static AccessibilityManager Get();

  /**
   * @brief Set the information of the specified actor's accessibility attribute.
   *
   * @pre The AccessibilityManager has been initialized.
   * @pre The Actor has been initialized.
   * @param actor The actor the text to be set with
   * @param type The attribute type the text to be set with
   * @param text The text for the actor's accessibility information
   */
  void SetAccessibilityAttribute(Actor actor, AccessibilityAttribute type, const std::string& text);

  /**
   * @brief Get the text of the specified actor's accessibility attribute.
   *
   * @pre The AccessibilityManager has been initialized.
   * @pre The Actor has been initialized.
   * @param actor The actor to be queried
   * @param type The attribute type to be queried
   * @return The text of the actor's accessibility information
   */
  std::string GetAccessibilityAttribute(Actor actor, AccessibilityAttribute type) const;

  /**
   * @brief Set the focus order of the actor.
   *
   * The focus order of each actor in the focus chain is unique. If
   * there is another actor assigned with the same focus order
   * already, the new actor will be inserted to the focus chain with
   * that focus order, and the focus order of the original actor and
   * all the actors followed in the focus chain will be increased
   * accordingly. If the focus order assigned to the actor is 0, it
   * means that actor's focus order is undefined (e.g. the actor has a
   * description but with no focus order being set yet) and therefore
   * that actor is not focusable.
   *
   * @pre The AccessibilityManager has been initialized.
   * @pre The Actor has been initialized.
   * @param actor The actor the focus order to be set with
   * @param order The focus order of the actor
   */
  void SetFocusOrder(Actor actor, const unsigned int order);

  /**
   * @brief Get the focus order of the actor.
   *
   * When the focus order is 0, it means the focus order of the actor
   * is undefined.
   *
   * @pre The AccessibilityManager has been initialized.
   * @pre The Actor has been initialized.
   * @param actor The actor to be queried
   * @return The focus order of the actor
   */
  unsigned int GetFocusOrder(Actor actor) const;

  /**
   * @brief Generates a new focus order number which can be used to
   * assign to actors which need to be appended to the end of the
   * current focus order chain.
   *
   * The new number will be an increment over the very last focus
   * order number in the focus chain. If the focus chain is empty then
   * the function returns 1, else the number returned will be FOLast +
   * 1 where FOLast is the focus order of the very last control in the
   * focus chain.
   *
   * @pre The AccessibilityManager has been initialized.
   * @return The focus order of the actor
   */
  unsigned int GenerateNewFocusOrder() const;

  /**
   * @brief Get the actor that has the specified focus order.
   *
   * It will return an empty handle if the actor is not in the stage
   * or has a focus order of 0.
   *
   * @pre The AccessibilityManager has been initialized.
   * @param order The focus order of the actor
   *
   * @return The actor that has the specified focus order or an empty
   * handle if no actor in the stage has the specified focus order.
   */
  Actor GetActorByFocusOrder(const unsigned int order);

  /**
   * @brief Move the focus to the specified actor.
   *
   * Only one actor can be focused at the same time.  The actor must
   * have a defined focus order and must be focusable, visible and in
   * the stage.
   *
   * @pre The AccessibilityManager has been initialized.
   * @pre The Actor has been initialized.
   * @param actor The actor to be focused
   * @return Whether the focus is successful or not
   */
  bool SetCurrentFocusActor(Actor actor);

  /**
   * @brief Get the current focused actor.
   *
   * @pre The AccessibilityManager has been initialized.
   * @return A handle to the current focused actor or an empty handle if no actor is focused.
   */
  Actor GetCurrentFocusActor();

  /**
   * @brief Get the focus group of current focused actor.
   *
   * @pre The AccessibilityManager has been initialized.
   *
   * @return A handle to the immediate parent of the current focused
   * actor which is also a focus group, or an empty handle if no actor
   * is focused.
   */
  Actor GetCurrentFocusGroup();

  /**
   * @brief Get the focus order of currently focused actor.
   * @pre The AccessibilityManager has been initialized.
   *
   * @return The focus order of the currently focused actor or 0 if no
   * actor is in focus.
   */
  unsigned int GetCurrentFocusOrder();

  /**
   * @brief Move the focus to the next focusable actor in the focus
   * chain (according to the focus traversal order).
   *
   * When the focus movement is wrapped around, the focus will be moved
   * to the first focusable actor when it reaches the end of the focus chain.
   *
   * @pre The AccessibilityManager has been initialized.
   * @return true if the moving was successful
   */
  bool MoveFocusForward();

  /**
   * @brief Move the focus to the previous focusable actor in the
   * focus chain (according to the focus traversal order).
   *
   * When the focus movement is wrapped around, the focus will be
   * moved to the last focusable actor when it reaches the beginning
   * of the focus chain.
   *
   * @pre The AccessibilityManager has been initialized.
   * @return true if the moving was successful
   */
  bool MoveFocusBackward();

  /**
   * @brief Clear the focus from the current focused actor if any, so
   * that no actor is focused in the focus chain.
   *
   * It will emit focus changed signal without current focused actor
   * @pre The AccessibilityManager has been initialized.
   */
  void ClearFocus();

  /**
   * @brief Clear the every registered focusable actor from focus-manager.
   * @pre The AccessibilityManager has been initialized.
   */
  void Reset();

  /**
   * @brief Set whether an actor is a focus group that can limit the
   * scope of focus movement to its child actors in the focus chain.
   *
   * @pre The AccessibilityManager has been initialized.
   * @pre The Actor has been initialized.
   * @param actor The actor to be set as a focus group.
   * @param isFocusGroup Whether to set the actor to be a focus group or not.
   */
  void SetFocusGroup(Actor actor, bool isFocusGroup);

  /**
   * @brief Check whether the actor is set as a focus group or not.
   *
   * @pre The AccessibilityManager has been initialized.
   * @pre The Actor has been initialized.
   * @param actor The actor to be checked.
   * @return Whether the actor is set as a focus group.
   */
  bool IsFocusGroup(Actor actor) const;

  /**
   * @brief Set whether the group mode is enabled or not.
   *
   * When the group mode is enabled, the focus movement will be limited to the child actors
   * of the current focus group including the current focus group itself. The current focus
   * group is the closest ancestor of the current focused actor that set as a focus group.
   * @pre The AccessibilityManager has been initialized.
   * @param enabled Whether the group mode is enabled or not
   */
  void SetGroupMode(bool enabled);

  /**
   * @brief Get whether the group mode is enabled or not.
   *
   * @pre The AccessibilityManager has been initialized.
   * @return Whether the group mode is enabled or not.
   */
  bool GetGroupMode() const;

  /**
   * @brief Set whether focus will be moved to the beginning of the
   * focus chain when it reaches the end or vice versa.
   *
   * When both the wrap mode and the group mode are enabled, focus will be
   * wrapped within the current focus group. Focus will not be wrapped in default.
   * @pre The AccessibilityManager has been initialized.
   * @param wrapped Whether the focus movement is wrapped around or not
   */
  void SetWrapMode(bool wrapped);

  /**
   * @brief Get whether the wrap mode is enabled or not.
   *
   * @pre The AccessibilityManager has been initialized.
   * @return Whether the wrap mode is enabled or not.
   */
  bool GetWrapMode() const;

  /**
   * @brief Set the focus indicator actor.
   *
   * This will replace the default focus indicator actor in
   * AccessibilityManager and will be added to the focused actor as a
   * highlight.
   *
   * @pre The AccessibilityManager has been initialized.
   * @pre The indicator actor has been initialized.
   * @param indicator The indicator actor to be added
   */
  void SetFocusIndicatorActor(Actor indicator);

  /**
   * @brief Get the focus indicator actor.
   *
   * @pre The AccessibilityManager has been initialized.
   * @return A handle to the focus indicator actor
   */
  Actor GetFocusIndicatorActor();

  /**
   * @brief Returns the closest ancestor of the given actor that is a focus group.
   *
   * @param actor The actor to be checked for its focus group
   * @return The focus group the given actor belongs to or an empty handle if the given actor doesn't belong to any focus group
   */
  Actor GetFocusGroup(Actor actor);

  /**
   * @brief Returns the current position of the read action.
   * @return The current event position.
   */
  Vector2 GetReadPosition() const;

 public: // Signals

  /**
   * @brief This signal is emitted when the current focused actor is changed.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(Actor originalFocusedActor, Actor currentFocusedActor);
   * @endcode
   * @pre The Object has been initialized.
   * @return The signal to connect to.
   */
  FocusChangedSignalType& FocusChangedSignal();

  /**
   * @brief This signal is emitted when there is no way to move focus further.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(Actor currentFocusedActor, FocusOvershotDirection direction);
   * @endcode
   * @pre The Object has been initialized.
   * @return The signal to connect to.
   */
  FocusOvershotSignalType& FocusOvershotSignal();

  /**
   * @brief This signal is emitted when the current focused actor is activated.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(Actor activatedActor);
   * @endcode
   * @pre The Object has been initialized.
   * @return The signal to connect to.
   */
  FocusedActorActivatedSignalType& FocusedActorActivatedSignal();

 public: // Accessibility action signals.

  /**
   * @brief This is emitted when accessibility(screen-reader) feature turned on or off.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& StatusChangedSignal();

  /**
   * @brief This is emitted when accessibility action is received to move focus to the next
   * focusable actor (by one finger flick down).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionNextSignal();

  /**
   * @brief This is emitted when accessibility action is received to move focus to the previous
   * focusable actor (by one finger flick up).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionPreviousSignal();

  /**
   * @brief This is emitted when accessibility action is received to activate the current focused
   * actor (by one finger double tap).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionActivateSignal();

  /**
   * @brief This is emitted when accessibility action is received to focus and read the actor
   * (by one finger tap).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionReadSignal();

  /**
   * @brief This is emitted when accessibility action is received to focus and read the actor
   * (by one finger move).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionOverSignal();

  /**
   * @brief This is emitted when accessibility action is received to move focus to the next
   * focusable actor (by one finger flick right).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionReadNextSignal();

  /**
   * @brief This is emitted when accessibility action is received to move focus to the previous
   * focusable actor (by one finger flick left).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionReadPreviousSignal();

  /**
   * @brief This is emitted when accessibility action is received to change the value when the
   * current focused actor is a slider (by double finger down and move up and right).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionUpSignal();

  /**
   * @brief This is emitted when accessibility action is received to change the value when the
   * current focused actor is a slider (by double finger down and move down and left).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionDownSignal();

  /**
   * @brief This is emitted when accessibility action is received to clear the focus from the
   * current focused actor if any, so that no actor is focused in the focus chain.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionClearFocusSignal();

  /**
   * @brief This is emitted when accessibility action is received to navigate back (by two
   * fingers circle draw).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionBackSignal();

  /**
   * @brief This is emitted when accessibility action is received to scroll up the list
   * (by two finger swipe up).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionScrollUpSignal();

  /**
   * @brief This is emitted when accessibility action is received to scroll down the list
   * (by two finger swipe down).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionScrollDownSignal();

  /**
   * @brief This is emitted when accessibility action is received to scroll left to the
   * previous page (by two finger swipe left)
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionPageLeftSignal();

  /**
   * @brief This is emitted when accessibility action is received to scroll right to the
   * next page (by two finger swipe right)
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionPageRightSignal();

  /**
   * @brief This is emitted when accessibility action is received to scroll up to the
   * previous page (by one finger swipe left and right)
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionPageUpSignal();

  /**
   * @brief This is emitted when accessibility action is received to scroll down to the
   * next page (by one finger swipe right and left)
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionPageDownSignal();

  /**
   * @brief This is emitted when accessibility action is received to move the focus to
   * the first item on the screen (by one finger swipe up and down)
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionMoveToFirstSignal();

  /**
   * @brief This is emitted when accessibility action is received to move the focus to
   * the last item on the screen (by one finger swipe down and up)
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionMoveToLastSignal();

  /**
   * @brief This is emitted when accessibility action is received to focus and read from the
   * first item on the top continously (by three fingers single tap)
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionReadFromTopSignal();

  /**
   * @brief This is emitted when accessibility action is received to move the focus to and
   * read from the next item continously (by three fingers double tap)
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionReadFromNextSignal();

  /**
   * @brief This is emitted when accessibility action is received to zoom (by one finger
   * triple tap)
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionZoomSignal();

  /**
   * @brief This is emitted when accessibility action is received to read the information
   * in the indicator (by two fingers triple tap).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionReadIndicatorInformationSignal();

  /**
   * @brief This is emitted when accessibility action is received to pause/resume the
   * current speech (by two fingers single tap).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionReadPauseResumeSignal();

  /**
   * @brief This is emitted when accessibility action is received to start/stop the
   * current action (by two fingers double tap).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionStartStopSignal();

  /**
   * @brief This is emitted when accessibility action is received to handle scroll event (by two
   * fingers drag).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager, const TouchEvent& event );
   * @endcode
   * @return The signal to connect to.
   */
  AccessibilityActionScrollSignalType& ActionScrollSignal();

public:

  explicit DALI_INTERNAL AccessibilityManager( Internal::AccessibilityManager *impl );

}; // class AccessibilityManager

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_ACCESSIBILITY_MANAGER_H__
