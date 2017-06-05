#ifndef DALI_TOOLKIT_CONTROL_DEVEL_H
#define DALI_TOOLKIT_CONTROL_DEVEL_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
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
  STYLE_NAME        = Control::Property::STYLE_NAME,
  BACKGROUND_COLOR  = Control::Property::BACKGROUND_COLOR,
  BACKGROUND_IMAGE  = Control::Property::BACKGROUND_IMAGE,
  KEY_INPUT_FOCUS   = Control::Property::KEY_INPUT_FOCUS,
  BACKGROUND        = Control::Property::BACKGROUND,

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
  TOOLTIP = BACKGROUND + 1,

  /**
   * @brief The current state of the control.
   * @details Name "state", type DevelControl::State ( Property::INTEGER ) or Property::STRING
   *
   * @see DevelControl::State
   */
  STATE = BACKGROUND + 2,

  /**
   * @brief The current sub state of the control.
   * @details Name "subState", type Property::INTEGER or Property::STRING. The enumeration used is dependent on the derived control.
   *
   * @see DevelControl::State
   */
  SUB_STATE = BACKGROUND + 3,

  /**
   * @brief The actor ID of the left focusable control.
   * @details Name "leftFocusableActorId", type Property::INTEGER.
   *
   */
  LEFT_FOCUSABLE_ACTOR_ID = BACKGROUND + 4,

  /**
   * @brief The actor ID of the right focusable control.
   * @details Name "rightFocusableActorId", type Property::INTEGER.
   *
   */
  RIGHT_FOCUSABLE_ACTOR_ID = BACKGROUND + 5,

  /**
   * @brief The actor ID of the up focusable control.
   * @details Name "upFocusableActorId", type Property::INTEGER.
   *
   */
  UP_FOCUSABLE_ACTOR_ID = BACKGROUND + 6,

  /**
   * @brief The actor ID of the down focusable control.
   * @details Name "downFocusableActorId", type Property::INTEGER.
   *
   */
  DOWN_FOCUSABLE_ACTOR_ID = BACKGROUND + 7
};

} // namespace Property

/// @brief ResourceReady signal type;
typedef Signal<void ( Control ) > ResourceReadySignalType;

/**
 * @brief This signal is emitted after all resources required
 * by a control are loaded and ready.
 * Most resources are only loaded when the control is placed on stage.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName( Control control );
 * @endcode
 */
DALI_IMPORT_API ResourceReadySignalType& ResourceReadySignal( Control& control );

/**
 * @brief Query if all resources required by a control are loaded and ready.
 * Most resources are only loaded when the control is placed on stage.
 * @return true if the resources are loaded and ready, false otherwise
 *
 */
DALI_IMPORT_API bool IsResourceReady( const Control& control );

/**
 * @brief Register a visual by Property Index, linking an Actor to visual when required.
 * In the case of the visual being an actor or control deeming visual not required then visual should be an empty handle.
 * No parenting is done during registration, this should be done by derived class.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual, used to reference visual
 * @param[in] visual The visual to register
 * @note Derived class should not call visual.SetOnStage(actor). It is the responsibility of the base class to connect/disconnect registered visual to stage.
 *       Use below API with enabled set to false if derived class wishes to control when visual is staged.
 */
DALI_IMPORT_API void RegisterVisual( Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual );

/**
 * @brief Register a visual by Property Index, linking an Actor to visual when required.
 *
 * In the case of the visual being an actor or control deeming visual not required then visual should be an empty handle.
 * If enabled is false then the visual is not set on stage until enabled by the derived class.
 * @see EnableVisual
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual, used to reference visual
 * @param[in] visual The visual to register
 * @param[in] enabled false if derived class wants to control when visual is set on stage.
 *
 */
DALI_IMPORT_API void RegisterVisual( Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual, bool enabled );

/**
 * @brief Erase the entry matching the given index from the list of registered visuals
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual, used to reference visual
 */
DALI_IMPORT_API void UnregisterVisual( Internal::Control& control, Dali::Property::Index index );

/**
 * @brief Retrieve the visual associated with the given property index.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual.
 * @return The registered visual if exist, otherwise empty handle.
 * @note For managing object life-cycle, do not store the returned visual as a member which increments its reference count.
 */
DALI_IMPORT_API Toolkit::Visual::Base GetVisual( const Internal::Control& control, Dali::Property::Index index );

/**
 * @brief Sets the given visual to be displayed or not when parent staged.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual
 * @param[in] enable flag to set enabled or disabled.
 */
DALI_IMPORT_API void EnableVisual( Internal::Control& control, Dali::Property::Index index, bool enable );

/**
 * @brief Queries if the given visual is to be displayed when parent staged.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual
 * @return bool whether visual is enabled or not
 */
DALI_IMPORT_API bool IsVisualEnabled( const Internal::Control& control, Dali::Property::Index index );

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
DALI_IMPORT_API Dali::Animation CreateTransition( Internal::Control& control, const Toolkit::TransitionData& transitionData );

} // namespace DevelControl

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_CONTROL_DEVEL_H
