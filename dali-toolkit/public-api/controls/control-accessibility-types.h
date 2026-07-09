#ifndef DALI_TOOLKIT_CONTROL_ACCESSIBILITY_TYPES_H
#define DALI_TOOLKIT_CONTROL_ACCESSIBILITY_TYPES_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <cstdint>

namespace Dali::Toolkit
{
/**
 * @brief Contains public accessibility semantic types used by Toolkit controls.
 *
 * These types describe the accessibility meaning exposed by controls without
 * requiring application code to depend on the platform accessibility backend.
 *
 * @SINCE_2_5.30
 */
namespace Accessibility
{
/**
 * @brief Represents an application-controlled accessibility state of a control.
 *
 * These states are semantic hints supplied by application or control code.
 * Toolkit combines them with runtime states such as visibility, focus, and
 * sensitivity before exposing the final accessibility state set.
 *
 * @SINCE_2_5.30
 */
enum class State : uint32_t
{
  ENABLED = 0, ///< The control is enabled for accessibility interaction. @SINCE_2_5.30
  SELECTED,    ///< The control or item is selected. @SINCE_2_5.30
  CHECKED,     ///< The control has a checked or toggled-on state. @SINCE_2_5.30
  BUSY,        ///< The control is busy and its value or contents may still be updating. @SINCE_2_5.30
  EXPANDED,    ///< The control is expanded and its child content is currently shown. @SINCE_2_5.30
  MAX_COUNT    ///< Sentinel value used to validate accessibility state values. @SINCE_2_5.30
};

/**
 * @brief First numeric value reserved for public Toolkit accessibility roles.
 *
 * @SINCE_2_5.30
 */
constexpr const uint32_t ROLE_START_INDEX = 200;

/**
 * @brief Represents the accessibility purpose of a control.
 *
 * A role describes what a control is, rather than how it is implemented.
 * For example, a custom control that behaves like a button should use
 * @c BUTTON even if it is not implemented by Toolkit::PushButton.
 *
 * @SINCE_2_5.30
 */
enum class Role : uint32_t
{
  ADJUSTABLE = ROLE_START_INDEX, ///< A control whose value can be adjusted within a range, such as a slider. @SINCE_2_5.30
  ALERT,                         ///< A control or region containing important time-sensitive information. @SINCE_2_5.30
  BUTTON,                        ///< A control that performs an action when activated. @SINCE_2_5.30
  CHECK_BOX,                     ///< A control that represents a checked or unchecked choice. @SINCE_2_5.30
  COMBO_BOX,                     ///< A control that combines an entry or button with a selectable list. @SINCE_2_5.30
  CONTAINER,                     ///< A grouping object that contains other accessible controls. @SINCE_2_5.30
  DIALOG,                        ///< A dialog window or dialog-like control. @SINCE_2_5.30
  ENTRY,                         ///< A control that accepts editable text input. @SINCE_2_5.30
  HEADER,                        ///< A heading or header that labels a section. @SINCE_2_5.30
  IMAGE,                         ///< A control that primarily presents image content. @SINCE_2_5.30
  LINK,                          ///< A control that navigates to another location when activated. @SINCE_2_5.30
  LIST,                          ///< A control that contains a list of selectable or readable items. @SINCE_2_5.30
  LIST_ITEM,                     ///< An item inside a list. @SINCE_2_5.30
  MENU,                          ///< A menu containing menu items or submenus. @SINCE_2_5.30
  MENU_BAR,                      ///< A bar that contains top-level menus. @SINCE_2_5.30
  MENU_ITEM,                     ///< An item inside a menu. @SINCE_2_5.30
  NONE,                          ///< No specific accessibility role is assigned. @SINCE_2_5.30
  NOTIFICATION,                  ///< A non-modal notification message. @SINCE_2_5.30
  PASSWORD_TEXT,                 ///< A text entry whose contents are obscured. @SINCE_2_5.30
  POPUP_MENU,                    ///< A temporary popup menu. @SINCE_2_5.30
  PROGRESS_BAR,                  ///< A control that displays progress toward completion. @SINCE_2_5.30
  RADIO_BUTTON,                  ///< A control that represents one choice from a mutually exclusive group. @SINCE_2_5.30
  SCROLL_BAR,                    ///< A control used to scroll content. @SINCE_2_5.30
  SPIN_BUTTON,                   ///< A control used to increment or decrement a value. @SINCE_2_5.30
  TAB,                           ///< A tab item that selects one page from a tab list. @SINCE_2_5.30
  TAB_LIST,                      ///< A control containing a set of tabs. @SINCE_2_5.30
  TEXT,                          ///< A control or region that primarily presents text. @SINCE_2_5.30
  TOGGLE_BUTTON,                 ///< A button that can remain in an on or off state. @SINCE_2_5.30
  TOOL_BAR,                      ///< A toolbar containing frequently used controls. @SINCE_2_5.30
  SCENE_3D,                      ///< A control that presents an interactive 3D scene. @SINCE_2_5.30
  MODEL,                         ///< A control that presents a model or model-like object. @SINCE_2_5.30
  MAX_COUNT                      ///< Sentinel value used to validate accessibility role values. @SINCE_2_5.30
};

/**
 * @brief Represents a semantic relation between accessible controls.
 *
 * Relations describe how one accessible control is connected to another, such
 * as a label describing a text entry or a popup belonging to a button.
 *
 * @SINCE_2_5.30
 */
enum class RelationType : uint32_t
{
  NULL_OF,          ///< No relation is specified. @SINCE_2_5.30
  LABEL_FOR,        ///< This control labels another control. @SINCE_2_5.30
  LABELLED_BY,      ///< This control is labelled by another control. @SINCE_2_5.30
  CONTROLLER_FOR,   ///< This control changes or controls another control. @SINCE_2_5.30
  CONTROLLED_BY,    ///< This control is changed or controlled by another control. @SINCE_2_5.30
  MEMBER_OF,        ///< This control is a member of a related group. @SINCE_2_5.30
  TOOLTIP_FOR,      ///< This control is a tooltip for another control. @SINCE_2_5.30
  NODE_CHILD_OF,    ///< This control is a node child of another control. @SINCE_2_5.30
  NODE_PARENT_OF,   ///< This control is a node parent of another control. @SINCE_2_5.30
  EXTENDED,         ///< This control has an implementation-specific extended relation. @SINCE_2_5.30
  FLOWS_TO,         ///< Reading or navigation flows from this control to another control. @SINCE_2_5.30
  FLOWS_FROM,       ///< Reading or navigation flows from another control to this control. @SINCE_2_5.30
  SUBWINDOW_OF,     ///< This control is a subwindow of another control. @SINCE_2_5.30
  EMBEDS,           ///< This control embeds another accessible control tree. @SINCE_2_5.30
  EMBEDDED_BY,      ///< This control is embedded by another accessible control tree. @SINCE_2_5.30
  POPUP_FOR,        ///< This control is a popup for another control. @SINCE_2_5.30
  PARENT_WINDOW_OF, ///< This control is the parent window of another control. @SINCE_2_5.30
  DESCRIPTION_FOR,  ///< This control provides a description for another control. @SINCE_2_5.30
  DESCRIBED_BY,     ///< This control is described by another control. @SINCE_2_5.30
  DETAILS,          ///< This control provides detailed information for another control. @SINCE_2_5.30
  DETAILS_FOR,      ///< This control has detailed information provided by another control. @SINCE_2_5.30
  ERROR_MESSAGE,    ///< This control provides an error message for another control. @SINCE_2_5.30
  ERROR_FOR,        ///< This control has an error described by another control. @SINCE_2_5.30
  MAX_COUNT         ///< Sentinel value used to validate accessibility relation values. @SINCE_2_5.30
};

} // namespace Accessibility
} // namespace Dali::Toolkit

#endif // DALI_TOOLKIT_CONTROL_ACCESSIBILITY_TYPES_H
