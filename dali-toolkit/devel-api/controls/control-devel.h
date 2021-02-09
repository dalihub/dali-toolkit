#ifndef DALI_TOOLKIT_CONTROL_DEVEL_H
#define DALI_TOOLKIT_CONTROL_DEVEL_H

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
#include <dali/devel-api/adaptor-framework/accessibility-impl.h>
#include <dali/devel-api/adaptor-framework/input-method-context.h>

// INTERNAL INCLUDES
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
   * @brief Current translation domain for accessibility clients.
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
   * @brief Boolean flag describing object as animated
   * @details Name "accessibilityAnimated", type Property::BOOLEAN
   * @note Flag set to true will prevent BoundChanged accessibility signal from emiting
   */
  ACCESSIBILITY_ANIMATED
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
 * @note If the depth-index is not set on the visual, then it is set to be above the currently registered visuals.
 * @note If replacing a visual, then the depth-index of the visual being replaced is used for the visual.
 */
DALI_TOOLKIT_API void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual);

/**
 * @brief Register a visual by Property Index with a depth index.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual, used to reference visual
 * @param[in] visual The visual to register
 * @param[in] depthIndex The visual's depth-index is set to this
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
 * @note If the depth-index is not set on the visual, then it is set to be above the currently registered visuals.
 * @note If replacing a visual, then the depth-index of the visual being replaced is used for the visual.
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
 * @param[in] depthIndex The visual's depth-index is set to this
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
 * @brief The method allows connection with other actor with usage of concrete accessibility relation type.
 *
 * @param control object to append attribute to
 * @param destination Actor object
 * @param relation    enumerated value describing relation
 */
DALI_TOOLKIT_API void AppendAccessibilityRelation(Dali::Actor control, Actor destination, Dali::Accessibility::RelationType relation);

/**
 * @brief The method allows removing relation
 *
 * @param control object to append attribute to
 * @param destination Actor object
 * @param relation    enumerated value describing relation
 */
DALI_TOOLKIT_API void RemoveAccessibilityRelation(Dali::Actor control, Actor destination, Dali::Accessibility::RelationType relation);

/**
 * @brief The method returns collection accessibility addresses representing objects connected with current object
 *
 * @param control object to append attribute to
 * @return std::vector, where index is casted value of Accessibility::RelationType and value is std::vector of type Accessibility::Address
 */
DALI_TOOLKIT_API std::vector<std::vector<Accessibility::Address>> GetAccessibilityRelations(Dali::Actor control);

/**
 * @brief The method removes all previously appended relations
 *
 * @param control object to append attribute to
 */
DALI_TOOLKIT_API void ClearAccessibilityRelations(Dali::Actor control);

/**
 * @brief The method allows to add or modify value matched with given key.
 * Modification take place if key was previously set.
 *
 * @param control object to append attribute to
 * @param key   std::string value
 * @param value std::string value
 */
DALI_TOOLKIT_API void AppendAccessibilityAttribute(Dali::Actor control, const std::string& key, const std::string value);

/**
 * @brief The method erases key with its value from accessibility attributes
 * @param control object to append attribute to
 * @param key std::string value
 */
DALI_TOOLKIT_API void RemoveAccessibilityAttribute(Dali::Actor control, const std::string& key);

/**
 * @brief The method clears accessibility attributes
 *
 * @param control object to append attribute to
 */
DALI_TOOLKIT_API void ClearAccessibilityAttributes(Dali::Actor control);

/**
 * @brief The method inserts reading information of an accessible object into attributes
 *
 * @param control object to append attribute to
 * @param types Reading information types
 */
DALI_TOOLKIT_API void SetAccessibilityReadingInfoType(Dali::Actor control, const Dali::Accessibility::ReadingInfoTypes types);

/**
 * @brief The method returns reading information of an accessible object
 *
 * @param control object to append attribute to
 * @return Reading information types
 */
DALI_TOOLKIT_API Dali::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType(Dali::Actor control);

/**
 * @brief The method erases highlight.
 *
 * @param control object to append attribute to
 * @return bool value, false when it is not possible or something went wrong, at the other way true.
 */
DALI_TOOLKIT_API bool ClearAccessibilityHighlight(Dali::Actor control);

/**
 * @brief The method grabs highlight.
 *
 * @param control object to append attribute to
 * @return bool value, false when it is not possible or something went wrong, at the other way true.
 */
DALI_TOOLKIT_API bool GrabAccessibilityHighlight(Dali::Actor control);

/**
 * @brief The metod presents bitset of control's states.
 *
 * @param control object to append attribute to
 * @return Dali::Accessibility::States is vector of enumerated State.
 */
DALI_TOOLKIT_API Dali::Accessibility::States GetAccessibilityStates(Dali::Actor control);

/**
 * @brief The method force sending notifications about current states to accessibility clients
 *
 * @param control object to append attribute to
 * @param states      mask with states expected to broadcast
 * @param doRecursive flag pointing if notifications of children's state would be sent
 */
DALI_TOOLKIT_API void NotifyAccessibilityStateChange(Dali::Actor control, Dali::Accessibility::States states, bool doRecursive);

/**
 * The method allows to set specific constructor for creating accessibility structure
 *
 * Thank to this method hierarchy of accessibility objects can be based on internal hierarchy of Actors.
 * It prevents from necessity of keeping two trees synchronized.
 * The method should be called inside OnInitialize method of all classes inheriting from Control.
 *
 * Possible usage can be as follows:
 * @code
 *   SetAccessibilityConstructor( []( Dali::Actor actor ) {
       return std::unique_ptr< Dali::Accessibility::Accessible >(
       new AccessibleImpl( actor, Dali::Accessibility::Role::DIALOG, true ) );
      } );
  * @endcode
  *
  * param constructor callback creating Accessible object
  */
DALI_TOOLKIT_API void SetAccessibilityConstructor(Dali::Actor control, std::function<std::unique_ptr<Dali::Accessibility::Accessible>(Dali::Actor)> constructor);

/**
 * Returns accessibility object bound to actor, if any
 *
 * This method won't bound new accessibility object. Use Dali::Accessibility::Accessible::Get in that case.
 */
DALI_TOOLKIT_API Dali::Accessibility::Accessible* GetBoundAccessibilityObject(Dali::Actor control);

} // namespace DevelControl

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_CONTROL_DEVEL_H
