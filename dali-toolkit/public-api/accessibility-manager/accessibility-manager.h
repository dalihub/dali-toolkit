#ifndef __DALI_TOOLKIT_ACCESSIBILITY_MANAGER_H__
#define __DALI_TOOLKIT_ACCESSIBILITY_MANAGER_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class AccessibilityManager;
}
/**
 * @addtogroup dali_toolkit_managers
 * @{
 */

/**
 * @brief Manages registration of actors in an accessibility focus chain and changing the
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
 * @SINCE_1_0.0
 */
class DALI_TOOLKIT_API AccessibilityManager : public BaseHandle
{
public:

  // Typedefs

  /**
   * @brief Accessibility Action Signal.
   *
   * The connected signal callback should return true if handled.
   * @SINCE_1_0.0
   */
  typedef Signal< bool ( AccessibilityManager& ) > AccessibilityActionSignalType; ///< Generic signal type @SINCE_1_0.0
  typedef Signal< bool ( AccessibilityManager&, const Dali::TouchEvent& )> AccessibilityActionScrollSignalType; ///< Scroll signal type @SINCE_1_0.0

  /**
   * @brief Enumeration for accessibility that needs four information which will be read by screen-reader.
   *
   * Reading order : Label -> Trait -> Optional (Value and Hint)
   * @SINCE_1_0.0
   */
  enum AccessibilityAttribute
  {
    ACCESSIBILITY_LABEL = 0, ///< Simple text which contained in ui-control @SINCE_1_0.0
    ACCESSIBILITY_TRAIT,     ///< Description of ui-control trait @SINCE_1_0.0
    ACCESSIBILITY_VALUE,     ///< Current value of ui-control (Optional) @SINCE_1_0.0
    ACCESSIBILITY_HINT,      ///< Hint for action (Optional) @SINCE_1_0.0
    ACCESSIBILITY_ATTRIBUTE_NUM ///< Number of attributes @SINCE_1_0.0
  };

   /**
    * @brief Enumeration for overshoot direction.
    * @SINCE_1_0.0
    */
  enum FocusOvershotDirection
  {
    OVERSHOT_PREVIOUS = -1, ///< Try to move previous of the first actor @SINCE_1_0.0
    OVERSHOT_NEXT = 1,      ///< Try to move next of the last actor @SINCE_1_0.0
  };

 public:

  /// @brief Focus changed signal
  /// @SINCE_1_0.0
  typedef Signal< void ( Actor, Actor ) > FocusChangedSignalType;

  /// @brief Focus overshooted signal
  /// @SINCE_1_0.0
  typedef Signal< void ( Actor, FocusOvershotDirection ) > FocusOvershotSignalType;

  /// @brief Focused actor activated signal
  /// @SINCE_1_0.0
  typedef Signal< void ( Actor ) > FocusedActorActivatedSignalType;

  /**
   * @brief Creates an AccessibilityManager handle; this can be initialised with AccessibilityManager::New().
   *
   * Calling member functions with an uninitialized handle is not allowed.
   * @SINCE_1_0.0
   */
  AccessibilityManager();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~AccessibilityManager();

  /**
   * @brief Gets the singleton of AccessibilityManager object.
   *
   * @SINCE_1_0.0
   * @return A handle to the AccessibilityManager control
   */
  static AccessibilityManager Get();

  /**
   * @brief Sets the information of the specified actor's accessibility attribute.
   *
   * @SINCE_1_0.0
   * @param actor The actor, the text to be set with
   * @param type The attribute type the text to be set with
   * @param text The text for the actor's accessibility information
   * @pre The AccessibilityManager has been initialized.
   * @pre The Actor has been initialized.
   */
  void SetAccessibilityAttribute(Actor actor, AccessibilityAttribute type, const std::string& text);

  /**
   * @brief Gets the text of the specified actor's accessibility attribute.
   *
   * @SINCE_1_0.0
   * @param actor The actor to be queried
   * @param type The attribute type to be queried
   * @return The text of the actor's accessibility information
   * @pre The AccessibilityManager has been initialized.
   * @pre The Actor has been initialized.
   */
  std::string GetAccessibilityAttribute(Actor actor, AccessibilityAttribute type) const;

  /**
   * @brief Sets the focus order of the actor.
   *
   * The focus order of each actor in the focus chain is unique.
   * If there is another actor assigned with the same focus order
   * already, the new actor will be inserted to the focus chain with
   * that focus order, and the focus order of the original actor and
   * all the actors followed in the focus chain will be increased
   * accordingly. If the focus order assigned to the actor is 0, it
   * means that actor's focus order is undefined (e.g. the actor has a
   * description but with no focus order being set yet) and therefore
   * that actor is not focusable.
   *
   * @SINCE_1_0.0
   * @param actor The actor the focus order to be set with
   * @param order The focus order of the actor
   * @pre The AccessibilityManager has been initialized.
   * @pre The Actor has been initialized.
   */
  void SetFocusOrder(Actor actor, const unsigned int order);

  /**
   * @brief Gets the focus order of the actor.
   *
   * When the focus order is 0, it means the focus order of the actor
   * is undefined.
   *
   * @SINCE_1_0.0
   * @param actor The actor to be queried
   * @return The focus order of the actor
   * @pre The AccessibilityManager has been initialized.
   * @pre The Actor has been initialized.
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
   * @SINCE_1_0.0
   * @return The focus order of the actor
   * @pre The AccessibilityManager has been initialized.
   */
  unsigned int GenerateNewFocusOrder() const;

  /**
   * @brief Gets the actor that has the specified focus order.
   *
   * It will return an empty handle if no actor in the stage
   * has the specified focus order.
   *
   * @SINCE_1_0.0
   * @param order The focus order of the actor
   *
   * @return The actor that has the specified focus order or an empty
   * handle if no actor in the stage has the specified focus order
   * @pre The AccessibilityManager has been initialized.
   */
  Actor GetActorByFocusOrder(const unsigned int order);

  /**
   * @brief Moves the focus to the specified actor.
   *
   * Only one actor can be focused at the same time. The actor must
   * have a defined focus order and must be focusable, visible and in
   * the stage.
   *
   * @SINCE_1_0.0
   * @param actor The actor to be focused
   * @return Whether the focus is successful or not
   * @pre The AccessibilityManager has been initialized.
   * @pre The Actor has been initialized.
   */
  bool SetCurrentFocusActor(Actor actor);

  /**
   * @brief Gets the current focused actor.
   *
   * @SINCE_1_0.0
   * @return A handle to the current focused actor or an empty handle if no actor is focused
   * @pre The AccessibilityManager has been initialized.
   */
  Actor GetCurrentFocusActor();

  /**
   * @brief Gets the focus group of current focused actor.
   *
   * @SINCE_1_0.0
   * @return A handle to the immediate parent of the current focused
   * actor which is also a focus group, or an empty handle if no actor
   * is focused
   * @pre The AccessibilityManager has been initialized.
   *
   */
  Actor GetCurrentFocusGroup();

  /**
   * @brief Gets the focus order of currently focused actor.
   * @SINCE_1_0.0
   * @return The focus order of the currently focused actor or 0 if no
   * actor is in focus
   * @pre The AccessibilityManager has been initialized.
   *
   */
  unsigned int GetCurrentFocusOrder();

  /**
   * @brief Moves the focus to the next focusable actor in the focus
   * chain (according to the focus traversal order).
   *
   * When the focus movement is wrapped around, the focus will be moved
   * to the first focusable actor when it reaches the end of the focus chain.
   *
   * @SINCE_1_0.0
   * @return true if the moving was successful
   * @pre The AccessibilityManager has been initialized.
   */
  bool MoveFocusForward();

  /**
   * @brief Moves the focus to the previous focusable actor in the
   * focus chain (according to the focus traversal order).
   *
   * When the focus movement is wrapped around, the focus will be
   * moved to the last focusable actor when it reaches the beginning
   * of the focus chain.
   *
   * @SINCE_1_0.0
   * @return true if the moving was successful
   * @pre The AccessibilityManager has been initialized.
   */
  bool MoveFocusBackward();

  /**
   * @brief Clears the focus from the current focused actor if any, so
   * that no actor is focused in the focus chain.
   *
   * It will emit focus changed signal without current focused actor.
   * @SINCE_1_0.0
   * @pre The AccessibilityManager has been initialized.
   */
  void ClearFocus();

  /**
   * @brief Clears every registered focusable actor from focus-manager.
   * @SINCE_1_0.0
   * @pre The AccessibilityManager has been initialized.
   */
  void Reset();

  /**
   * @brief Sets whether an actor is a focus group that can limit the
   * scope of focus movement to its child actors in the focus chain.
   *
   * @SINCE_1_0.0
   * @param actor The actor to be set as a focus group
   * @param isFocusGroup Whether to set the actor to be a focus group or not
   * @pre The AccessibilityManager has been initialized.
   * @pre The Actor has been initialized.
   */
  void SetFocusGroup(Actor actor, bool isFocusGroup);

  /**
   * @brief Checks whether the actor is set as a focus group or not.
   *
   * @SINCE_1_0.0
   * @param actor The actor to be checked
   * @return Whether the actor is set as a focus group
   * @pre The AccessibilityManager has been initialized.
   * @pre The Actor has been initialized.
   */
  bool IsFocusGroup(Actor actor) const;

  /**
   * @brief Sets whether the group mode is enabled or not.
   *
   * When the group mode is enabled, the focus movement will be limited to the child actors
   * of the current focus group including the current focus group itself. The current focus
   * group is the closest ancestor of the current focused actor that is set as a focus group.
   * @SINCE_1_0.0
   * @param enabled Whether the group mode is enabled or not
   * @pre The AccessibilityManager has been initialized.
   */
  void SetGroupMode(bool enabled);

  /**
   * @brief Gets whether the group mode is enabled or not.
   *
   * @SINCE_1_0.0
   * @return Whether the group mode is enabled or not.
   * @pre The AccessibilityManager has been initialized.
   */
  bool GetGroupMode() const;

  /**
   * @brief Sets whether focus will be moved to the beginning of the
   * focus chain when it reaches the end or vice versa.
   *
   * When both the wrap mode and the group mode are enabled, focus will be
   * wrapped within the current focus group. Focus will not be wrapped in default.
   * @SINCE_1_0.0
   * @param wrapped Whether the focus movement is wrapped around or not
   * @pre The AccessibilityManager has been initialized.
   */
  void SetWrapMode(bool wrapped);

  /**
   * @brief Gets whether the wrap mode is enabled or not.
   *
   * @SINCE_1_0.0
   * @return Whether the wrap mode is enabled or not.
   * @pre The AccessibilityManager has been initialized.
   */
  bool GetWrapMode() const;

  /**
   * @brief Sets the focus indicator actor.
   *
   * This will replace the default focus indicator actor in
   * AccessibilityManager and will be added to the focused actor as a
   * highlight.
   *
   * @SINCE_1_0.0
   * @param indicator The indicator actor to be added
   * @pre The AccessibilityManager has been initialized.
   * @pre The indicator actor has been initialized.
   */
  void SetFocusIndicatorActor(Actor indicator);

  /**
   * @brief Gets the focus indicator actor.
   *
   * @SINCE_1_0.0
   * @return A handle to the focus indicator actor
   * @pre The AccessibilityManager has been initialized.
   */
  Actor GetFocusIndicatorActor();

  /**
   * @brief Returns the closest ancestor of the given actor that is a focus group.
   *
   * @SINCE_1_0.0
   * @param actor The actor to be checked for its focus group
   * @return The focus group the given actor belongs to or an empty handle if the given actor doesn't belong to any focus group
   */
  Actor GetFocusGroup(Actor actor);

  /**
   * @brief Returns the current position of the read action.
   * @SINCE_1_0.0
   * @return The current event position
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
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The Object has been initialized.
   */
  FocusChangedSignalType& FocusChangedSignal();

  /**
   * @brief This signal is emitted when there is no way to move focus further.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(Actor currentFocusedActor, FocusOvershotDirection direction);
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The Object has been initialized.
   */
  FocusOvershotSignalType& FocusOvershotSignal();

  /**
   * @brief This signal is emitted when the current focused actor is activated.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(Actor activatedActor);
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The Object has been initialized.
   */
  FocusedActorActivatedSignalType& FocusedActorActivatedSignal();

 public: // Accessibility action signals

  /**
   * @brief This is emitted when accessibility(screen-reader) feature turned on or off.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
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
   * @SINCE_1_0.0
   * @return The signal to connect to
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
   * @SINCE_1_0.0
   * @return The signal to connect to
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
   * @SINCE_1_0.0
   * @return The signal to connect to
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
   * @SINCE_1_0.0
   * @return The signal to connect to
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
   * @SINCE_1_0.0
   * @return The signal to connect to
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
   * @SINCE_1_0.0
   * @return The signal to connect to
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
   * @SINCE_1_0.0
   * @return The signal to connect to
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
   * @SINCE_1_0.0
   * @return The signal to connect to
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
   * @SINCE_1_0.0
   * @return The signal to connect to
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
   * @SINCE_1_0.0
   * @return The signal to connect to
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
   * @SINCE_1_0.0
   * @return The signal to connect to
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
   * @SINCE_1_0.0
   * @return The signal to connect to
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
   * @SINCE_1_0.0
   * @return The signal to connect to
   */
  AccessibilityActionSignalType& ActionScrollDownSignal();

  /**
   * @brief This is emitted when accessibility action is received to scroll left to the
   * previous page (by two finger swipe left).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   */
  AccessibilityActionSignalType& ActionPageLeftSignal();

  /**
   * @brief This is emitted when accessibility action is received to scroll right to the
   * next page (by two finger swipe right).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   */
  AccessibilityActionSignalType& ActionPageRightSignal();

  /**
   * @brief This is emitted when accessibility action is received to scroll up to the
   * previous page (by one finger swipe left and right).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   */
  AccessibilityActionSignalType& ActionPageUpSignal();

  /**
   * @brief This is emitted when accessibility action is received to scroll down to the
   * next page (by one finger swipe right and left).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   */
  AccessibilityActionSignalType& ActionPageDownSignal();

  /**
   * @brief This is emitted when accessibility action is received to move the focus to
   * the first item on the screen (by one finger swipe up and down).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   */
  AccessibilityActionSignalType& ActionMoveToFirstSignal();

  /**
   * @brief This is emitted when accessibility action is received to move the focus to
   * the last item on the screen (by one finger swipe down and up).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   */
  AccessibilityActionSignalType& ActionMoveToLastSignal();

  /**
   * @brief This is emitted when accessibility action is received to focus and read from the
   * first item on the top continuously (by three fingers single tap).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to.
   */
  AccessibilityActionSignalType& ActionReadFromTopSignal();

  /**
   * @brief This is emitted when accessibility action is received to move the focus to and
   * read from the next item continuously (by three fingers double tap).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   */
  AccessibilityActionSignalType& ActionReadFromNextSignal();

  /**
   * @brief This is emitted when accessibility action is received to zoom (by one finger
   * triple tap).
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( AccessibilityManager& manager );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
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
   * @SINCE_1_0.0
   * @return The signal to connect to
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
   * @SINCE_1_0.0
   * @return The signal to connect to
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
   * @SINCE_1_0.0
   * @return The signal to connect to
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
   * @SINCE_1_0.0
   * @return The signal to connect to
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
