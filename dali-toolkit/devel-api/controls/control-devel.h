#ifndef DALI_TOOLKIT_CONTROL_DEVEL_H
#define DALI_TOOLKIT_CONTROL_DEVEL_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
  SHADOW = PADDING + 8
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

} // namespace DevelControl

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_CONTROL_DEVEL_H
