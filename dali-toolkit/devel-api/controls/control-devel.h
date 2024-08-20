#ifndef DALI_TOOLKIT_CONTROL_DEVEL_H
#define DALI_TOOLKIT_CONTROL_DEVEL_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/accessibility-bridge.h>
#include <dali/devel-api/adaptor-framework/input-method-context.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/time-period.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-accessible.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{
namespace Toolkit
{
class TransitionData;

namespace Visual
{
class Base;
}

namespace DevelControl
{
/// @brief AccessibilityActivate signal type.
typedef Signal<void()> AccessibilityActivateSignalType;

/// @brief AccessibilityReadingSkipped signal type.
typedef Signal<void()> AccessibilityReadingSkippedSignalType;

/// @brief AccessibilityReadingPaused signal type.
typedef Signal<void()> AccessibilityReadingPausedSignalType;

/// @brief AccessibilityReadingResumed signal type.
typedef Signal<void()> AccessibilityReadingResumedSignalType;

/// @brief AccessibilityReadingCancelled signal type.
typedef Signal<void()> AccessibilityReadingCancelledSignalType;

/// @brief AccessibilityReadingStopped signal type.
typedef Signal<void()> AccessibilityReadingStoppedSignalType;

/// @brief AccessibilityGetName signal type.
typedef Signal<void(std::string&)> AccessibilityGetNameSignalType;

/// @brief AccessibilityGetDescription signal type.
typedef Signal<void(std::string&)> AccessibilityGetDescriptionSignalType;

/// @brief AccessibilityDoGesture signal type.
typedef Signal<void(std::pair<Dali::Accessibility::GestureInfo, bool>&)> AccessibilityDoGestureSignalType;

/// @brief AccessibilityAction signal type.
typedef Signal<bool(const Dali::Accessibility::ActionInfo&)> AccessibilityActionSignalType;

enum State
{
  NORMAL,
  FOCUSED,
  DISABLED
};

namespace Property
{
enum
{
  STYLE_NAME      = Control::Property::STYLE_NAME,
  KEY_INPUT_FOCUS = Control::Property::KEY_INPUT_FOCUS,
  BACKGROUND      = Control::Property::BACKGROUND,
  MARGIN          = Control::Property::MARGIN,
  PADDING         = Control::Property::PADDING,

  /**
   * @brief Displays a tooltip when the control is hovered over.
   * @details Name "tooltip", type Property::STRING, Property::ARRAY or Property::MAP.
   *          If Property::STRING, then the style specified in the stylesheet is used.
   *          If Property::ARRAY of Visuals then all are displayed in one row.
   *          If Property::MAP, then it should be a map of Tooltip properties.
   * @note The tooltip is only activated if display content is received, i.e. a string (text) or visual to show.
   *       The rest is used to just build up the style of the tooltip (i.e. background, text color/point-size etc.)
   * @note When retrieved, a Property::MAP is returned.
   * @see Toolkit::Tooltip
   */
  TOOLTIP = PADDING + 1,

  /**
   * @brief The current state of the control.
   * @details Name "state", type DevelControl::State ( Property::INTEGER ) or Property::STRING
   *
   * @see DevelControl::State
   */
  STATE = PADDING + 2,

  /**
   * @brief The current sub state of the control.
   * @details Name "subState", type Property::INTEGER or Property::STRING. The enumeration used is dependent on the derived control.
   *
   * @see DevelControl::State
   */
  SUB_STATE = PADDING + 3,

  /**
   * @brief The actor ID of the left focusable control.
   * @details Name "leftFocusableActorId", type Property::INTEGER.
   *
   */
  LEFT_FOCUSABLE_ACTOR_ID = PADDING + 4,

  /**
   * @brief The actor ID of the right focusable control.
   * @details Name "rightFocusableActorId", type Property::INTEGER.
   *
   */
  RIGHT_FOCUSABLE_ACTOR_ID = PADDING + 5,

  /**
   * @brief The actor ID of the up focusable control.
   * @details Name "upFocusableActorId", type Property::INTEGER.
   *
   */
  UP_FOCUSABLE_ACTOR_ID = PADDING + 6,

  /**
   * @brief The actor ID of the down focusable control.
   * @details Name "downFocusableActorId", type Property::INTEGER.
   *
   */
  DOWN_FOCUSABLE_ACTOR_ID = PADDING + 7,

  /**
   * @brief The shadow of the control.
   * @details Name "shadow", type Property::MAP.
   */
  SHADOW = PADDING + 8,

  /**
   * @brief The name of object visible in accessibility tree.
   * @details Name "accessibilityName", type Property::STRING.
   */
  ACCESSIBILITY_NAME,

  /**
   * @brief The description of object visible in accessibility tree.
   * @details Name "accessibilityDescription", type Property::STRING.
   */
  ACCESSIBILITY_DESCRIPTION,

  /**
   * @brief Deprecated. Current translation domain for accessibility clients.
   * @details Name "accessibilityTranslationDomain", type Property::STRING.
   */
  ACCESSIBILITY_TRANSLATION_DOMAIN,

  /**
   * @brief Role being performed in accessibility hierarchy.
   * @details Name "accessibilityRole", type Property::INTEGER.
   * @note Property is stored as INTEGER, however its interpretaton
   * depend on enumeration Dali::Accessibility::Role and should be read and written
   * with usage of enumerated values.
   * @see Dali::Accessibility::Role
   */
  ACCESSIBILITY_ROLE,

  /**
   * @brief Mark of able to highlight object.
   * @details Name "accessibilityHighlightable", type Property::BOOLEAN.
   */
  ACCESSIBILITY_HIGHLIGHTABLE,

  /**
   * @brief Set of accessibility attributes describing object in accessibility hierarchy
   * @details Name "accessibilityAttributes", type Property::MAP
   */
  ACCESSIBILITY_ATTRIBUTES,

  /**
   * @brief Whether a Control and its descendants can emit key signals.
   * @details Name "dispatchKeyEvents", type Property::BOOLEAN
   * @note If a control's dispatchKeyEvents is set to false, then it's children will not emit a key event signal either.
   */
  DISPATCH_KEY_EVENTS,

  /**
   * @brief Marks the object as invisible to AT-SPI clients.
   * @details Name "accessibilityHidden", type Property::BOOLEAN.
   * @note The representative Accessible object will not appear in the AT-SPI tree.
   */
  ACCESSIBILITY_HIDDEN,

  /**
   * @brief The actor ID of the clockwise focusable control.
   * @details Name "clockwiseFocusableActorId", type Property::INTEGER.
   *
   */
  CLOCKWISE_FOCUSABLE_ACTOR_ID,

  /**
   * @brief The actor ID of the conter-clockwise focusable control.
   * @details Name "counterClockwiseFocusableActorId", type Property::INTEGER.
   *
   */
  COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID,

  /**
   * @brief Identifier that allows the automation framework to find and interact with this element.
   * @details Name "automationId", type Property::STRING.
   * @note This is a string identifier (compared to @c Actor::Property::ID which is an integer).
   * It will also appear in the AT-SPI tree under the key "automationId".
   */
  AUTOMATION_ID,

  /**
   * @brief The accessibility value represented by the control. For example, "60%" for a slider object.
   * @details Name "accessibilityValue", type Property::STRING.
   */
  ACCESSIBILITY_VALUE,

  /**
   * @brief Indicates the accessibility services treat the controla as scrollable.
   * @details Name "accessibilityScrollable", type Property::BOOLEAN.
   */
  ACCESSIBILITY_SCROLLABLE,
};

} // namespace Property

/**
 * @brief Register a visual by Property Index.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual, used to reference visual
 * @param[in] visual The visual to register
 *
 * @note Derived class should not call visual.SetOnScene(actor). It is the responsibility of the base class to connect/disconnect registered visual to stage.
 *       Use below API with enabled set to false if derived class wishes to control when visual is staged.
 * @note depth-index be used to Dali::Toolkit::DepthIndex::Ranges::AUTO_INDEX.
 */
DALI_TOOLKIT_API void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual);

/**
 * @brief Register a visual by Property Index with a depth index.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual, used to reference visual
 * @param[in] visual The visual to register
 * @param[in] depthIndex The visual's depth-index is set to this. If the depth-index is set to DepthIndex::Ranges::AUTO_INDEX,
 *                       the actual depth-index of visual will be determind automatically (Use previous visuals depth-index, or placed on top of all other visuals.)
 *                       Otherwise, the visual's depth-index is set to clamped value, between DepthIndex::Ranges::MINIMUM_DEPTH_INDEX and DepthIndex::Ranges::MAXIMUM_DEPTH_INDEX.
 *
 * @note Derived class should not call visual.SetOnScene(actor). It is the responsibility of the base class to connect/disconnect registered visual to stage.
 *       Use below API with enabled set to false if derived class wishes to control when visual is staged.
 *
 * @see Visual::Base::GetDepthIndex()
 * @see Visual::Base::SetDepthIndex()
 */
DALI_TOOLKIT_API void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual, int depthIndex);

/**
 * @brief Register a visual by Property Index with the option of enabling/disabling it.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual, used to reference visual
 * @param[in] visual The visual to register
 * @param[in] enabled false if derived class wants to control when visual is set on stage.
 *
 * @note depth-index be used to Dali::Toolkit::DepthIndex::Ranges::AUTO_INDEX.
 *
 * @see EnableVisual()
 */
DALI_TOOLKIT_API void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual, bool enabled);

/**
 * @brief Register a visual by Property Index with a depth index with the option of enabling/disabling it.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual, used to reference visual
 * @param[in] visual The visual to register
 * @param[in] enabled false if derived class wants to control when visual is set on stage.
 * @param[in] depthIndex The visual's depth-index is set to this. If the depth-index is set to DepthIndex::Ranges::AUTO_INDEX,
 *                       the actual depth-index of visual will be determind automatically (Use previous visuals depth-index, or placed on top of all other visuals.)
 *                       Otherwise, the visual's depth-index is set to clamped value, between DepthIndex::Ranges::MINIMUM_DEPTH_INDEX and DepthIndex::Ranges::MAXIMUM_DEPTH_INDEX.
 *
 * @see EnableVisual()
 * @see Visual::Base::GetDepthIndex()
 * @see Visual::Base::SetDepthIndex()
 */
DALI_TOOLKIT_API void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual, bool enabled, int depthIndex);

/**
 * @brief Erase the entry matching the given index from the list of registered visuals
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual, used to reference visual
 */
DALI_TOOLKIT_API void UnregisterVisual(Internal::Control& control, Dali::Property::Index index);

/**
 * @brief Retrieve the visual associated with the given property index.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual.
 * @return The registered visual if exist, otherwise empty handle.
 * @note For managing object life-cycle, do not store the returned visual as a member which increments its reference count.
 */
DALI_TOOLKIT_API Toolkit::Visual::Base GetVisual(const Internal::Control& control, Dali::Property::Index index);

/**
 * @brief Sets the given visual to be displayed or not when parent staged.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual
 * @param[in] enable flag to set enabled or disabled.
 */
DALI_TOOLKIT_API void EnableVisual(Internal::Control& control, Dali::Property::Index index, bool enable);

/**
 * @brief Queries if the given visual is to be displayed when parent staged.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual
 * @return bool whether visual is enabled or not
 */
DALI_TOOLKIT_API bool IsVisualEnabled(const Internal::Control& control, Dali::Property::Index index);

/**
 * @brief Add a transition effect on the control to the given animation
 *
 * Only generates an animator if the properties described in the transition
 * data are staged (e.g. the visual is Enabled and the control is on stage).
 * Otherwise the target values are stored, and will get set onto the properties
 * when the visual is next staged.
 *
 * @param[in] control The control
 * @param[in] animation The Animation to add valid transitions to
 * @param[in] transitionData The transition data describing the effect to create
 */
DALI_TOOLKIT_API void AddTransitions(Internal::Control&             control,
                                     Dali::Animation                animation,
                                     const Toolkit::TransitionData& transitionData);

/**
 * @brief Create a transition effect on the control.
 *
 * Only generates an animation if the properties described in the transition
 * data are staged (e.g. the visual is Enabled and the control is on stage).
 * Otherwise the target values are stored, and will get set onto the properties
 * when the visual is next staged.
 *
 * @param[in] control The control
 * @param[in] transitionData The transition data describing the effect to create
 * @return A handle to an animation defined with the given effect, or an empty
 * handle if no properties match.
 */
DALI_TOOLKIT_API Dali::Animation CreateTransition(Internal::Control&             control,
                                                  const Toolkit::TransitionData& transitionData);

/**
 * @brief Perform an action on a visual registered to this control.
 *
 * Visuals will have actions, this API is used to perform one of these actions with the given attributes.
 *
 * @param[in] control The control.
 * @param[in] visualIndex The Property index of the visual.
 * @param[in] actionId The action to perform.  See Visual to find supported actions.
 * @param[in] attributes Optional attributes for the action.
 */
DALI_TOOLKIT_API void DoAction(Control& control, Dali::Property::Index visualIndex, Dali::Property::Index actionId, const Dali::Property::Value attributes);

/**
 * @brief Perform an action on a visual registered to this control.
 *
 * Visuals will have actions, this API is used to perform one of these actions with the given attributes.
 *
 * @param[in] control The control.
 * @param[in] visualIndex The Property index of the visual.
 * @param[in] actionId The action to perform.  See Visual to find supported actions.
 * @param[in] attributes Optional attributes for the action.
 */
DALI_TOOLKIT_API void DoActionExtension(Control& control, Dali::Property::Index visualIndex, Dali::Property::Index actionId, Dali::Any attributes);

/**
 * @brief Set input method context.
 *
 * @param[in] control The control.
 * @param[in] inputMethodContext The input method context.
 */
DALI_TOOLKIT_API void SetInputMethodContext(Internal::Control& control, InputMethodContext& inputMethodContext);

/**
 * @brief Visual Event signal type
 */
using VisualEventSignalType = Signal<void(Control, Dali::Property::Index, Dali::Property::Index)>;

/**
 * @brief This signal is emitted when a visual has an event to notify.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName( Control control, Dali::Property::Index visualIndex, Dali::Property::Index signalId );
 * @endcode
 * @return The signal to connect to
 */
DALI_TOOLKIT_API VisualEventSignalType& VisualEventSignal(Control control);

/**
 * @brief Retrieve the property object associated with the given property index and the visual property key.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual.
 * @param[in] visualPropertyKey The key of the visual's property.
 * @return The Property object
 */
DALI_TOOLKIT_API Dali::Property GetVisualProperty(Control control, Dali::Property::Index index, Dali::Property::Key visualPropertyKey);

/**
 * @brief The signal is emmited as a succession of "activate" signal send by accessibility client.
 * @return The signal to connect to
 */
DALI_TOOLKIT_API AccessibilityActivateSignalType& AccessibilityActivateSignal(Toolkit::Control control);

/**
 * @brief The signal is emmited when text send via Dali::Accessibility::Bridge::Say
 * was placed in TTS queue but other text with higher priority prevented it from being read.
 *
 * @return The signal to connect to
 */
DALI_TOOLKIT_API AccessibilityReadingSkippedSignalType& AccessibilityReadingSkippedSignal(Toolkit::Control control);

/**
 * @brief
 *
 * @return The signal to connect to
 */
DALI_TOOLKIT_API AccessibilityReadingPausedSignalType& AccessibilityReadingPausedSignal(Toolkit::Control control);

/**
 * @brief
 *
 * @return The signal to connect to
 */
DALI_TOOLKIT_API AccessibilityReadingResumedSignalType& AccessibilityReadingResumedSignal(Toolkit::Control control);

/**
 * @brief The signal is emmited when text send via Dali::Accessibility::Bridge::Say
 * was placed in TTS queue and reading was started but other text with higher priority cancelled it.
 *
 * @return The signal to connect to
 */
DALI_TOOLKIT_API AccessibilityReadingCancelledSignalType& AccessibilityReadingCancelledSignal(Toolkit::Control control);

/**
 * @brief The signal is emmited when text send via Dali::Accessibility::Bridge::Say
 * was fully read by TTS module.
 *
 * @return The signal to connect to
 */
DALI_TOOLKIT_API AccessibilityReadingStoppedSignalType& AccessibilityReadingStoppedSignal(Toolkit::Control control);

/**
 * @brief The signal is emmited when accessibility client asks for object's name.
 *
 * Connected callback should assign name to std::string call parameter.
 * Accessibility name can be stored in two ways:
 *     this signal,
 *     ACCESSIBILITY_NAME property,
 * Priority is as above. If none is used, default implementation is provided.
 * @return [description]
 */
DALI_TOOLKIT_API AccessibilityGetNameSignalType& AccessibilityGetNameSignal(Toolkit::Control control);

/**
 * @brief The signal is emmited when accessibility client asks for object's description.
 *
 * Connected callback should assign description to std::string call parameter.
 * Accessibility description can be stored in two ways:
 *     this signal,
 *     ACCESSIBILITY_DESCRIPTION property,
 * Priority is as above. If none is used, default implementation is provided.
 * @return signal handler
 */
DALI_TOOLKIT_API AccessibilityGetDescriptionSignalType& AccessibilityGetDescriptionSignal(Toolkit::Control control);

/**
 * @brief The signal is emitted when accessibility client call "DoGesture" method via IPC mechanism.
 *
 * This signal allows developers to serve incoming gesture in specific way.
 * @return signal handler
 */
DALI_TOOLKIT_API AccessibilityDoGestureSignalType& AccessibilityDoGestureSignal(Toolkit::Control control);

/**
 * @brief The signal is emitted when accessibility client call "DoAction" or "DoActionName" method via IPC mechanism.
 * @return The signal to connect to
 */
DALI_TOOLKIT_API AccessibilityActionSignalType& AccessibilityActionSignal(Toolkit::Control control);

/**
 * @brief The method allows connection with other actor with usage of concrete accessibility relation type.
 *
 * @param control object to append attribute to
 * @param destination Actor object
 * @param relation    enumerated value describing relation
 */
DALI_TOOLKIT_API void AppendAccessibilityRelation(Toolkit::Control control, Dali::Actor destination, Dali::Accessibility::RelationType relation);

/**
 * @brief The method allows removing relation
 *
 * @param control object to append attribute to
 * @param destination Actor object
 * @param relation    enumerated value describing relation
 */
DALI_TOOLKIT_API void RemoveAccessibilityRelation(Toolkit::Control control, Dali::Actor destination, Dali::Accessibility::RelationType relation);

/**
 * @brief Returns a collection of Accessible objects related to current object and grouped by relation type.
 *
 * @param control object to query
 * @return collection of relations
 *
 * @see Dali::Accessibility::Accessible::GetRelationSet()
 */
DALI_TOOLKIT_API std::vector<Accessibility::Relation> GetAccessibilityRelations(Toolkit::Control control);

/**
 * @brief The method removes all previously appended relations
 *
 * @param control object to append attribute to
 */
DALI_TOOLKIT_API void ClearAccessibilityRelations(Toolkit::Control control);

/**
 * @brief The method allows to add or modify value matched with given key.
 * Modification take place if key was previously set.
 *
 * @param control object to append attribute to
 * @param key   std::string value
 * @param value std::string value
 */
DALI_TOOLKIT_API void AppendAccessibilityAttribute(Toolkit::Control control, const std::string& key, const std::string& value);

/**
 * @brief The method erases key with its value from accessibility attributes
 * @param control object to append attribute to
 * @param key std::string value
 */
DALI_TOOLKIT_API void RemoveAccessibilityAttribute(Toolkit::Control control, const std::string& key);

/**
 * @brief The method clears accessibility attributes
 *
 * @param control object to append attribute to
 */
DALI_TOOLKIT_API void ClearAccessibilityAttributes(Toolkit::Control control);

/**
 * @brief The method inserts reading information of an accessible object into attributes
 *
 * @param control object to append attribute to
 * @param types Reading information types
 */
DALI_TOOLKIT_API void SetAccessibilityReadingInfoType(Toolkit::Control control, const Dali::Accessibility::ReadingInfoTypes types);

/**
 * @brief The method returns reading information of an accessible object
 *
 * @param control object to append attribute to
 * @return Reading information types
 */
DALI_TOOLKIT_API Dali::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType(Toolkit::Control control);

/**
 * @brief The method erases highlight.
 *
 * @param control object to append attribute to
 * @return bool value, false when it is not possible or something went wrong, at the other way true.
 */
DALI_TOOLKIT_API bool ClearAccessibilityHighlight(Toolkit::Control control);

/**
 * @brief The method grabs highlight.
 *
 * @param control object to append attribute to
 * @return bool value, false when it is not possible or something went wrong, at the other way true.
 */
DALI_TOOLKIT_API bool GrabAccessibilityHighlight(Toolkit::Control control);

/**
 * @brief The metod presents bitset of control's states.
 *
 * @param control object to append attribute to
 * @return Dali::Accessibility::States is vector of enumerated State.
 */
DALI_TOOLKIT_API Dali::Accessibility::States GetAccessibilityStates(Toolkit::Control control);

/**
 * @brief The method force sending notifications about current states to accessibility clients
 *
 * @param control object to append attribute to
 * @param states  mask with states expected to broadcast
 * @param recurse flag pointing if notifications of children's state would be sent
 */
DALI_TOOLKIT_API void NotifyAccessibilityStateChange(Toolkit::Control control, Dali::Accessibility::States states, bool recurse);

/**
 * @brief The method to get the control's accessibility created or not.
 *
 * @param[in] control object to append attribute to
 * @return True if accessible were created. False otherwise.
 */
DALI_TOOLKIT_API bool IsAccessibleCreated(Toolkit::Control control);

/**
 * @brief The method to set creatable of control's accessibility.
 * @note This method doesn't remove that already created accessible.
 *
 * @param[in] control object to append attribute to
 * @param[in] enable True if we allow to create accessible. False otherwise.
 */
DALI_TOOLKIT_API void EnableCreateAccessible(Toolkit::Control control, bool enable);

/**
 * @brief The method to get creatable of control's accessibility.
 *
 * @param[in] control object to append attribute to
 * @return True if we allow to create accessible. False otherwise.
 */
DALI_TOOLKIT_API bool IsCreateAccessibleEnabled(Toolkit::Control control);

} // namespace DevelControl

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_CONTROL_DEVEL_H
