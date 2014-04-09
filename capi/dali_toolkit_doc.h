#ifndef __DALI_TOOLKIT_DOC_H__
#define __DALI_TOOLKIT_DOC_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @ingroup CAPI_DALI_FRAMEWORK
 * @defgroup CAPI_DALI_TOOLKIT_MODULE Dali Toolkit
 * @brief A library to provide a user interface toolkit on top of Dali.
 * @section CAPI_DALI_TOOLKIT_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_MODULE_OVERVIEW Overview
 *   A library to provide a user interface toolkit running on top of Dali.
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_CONTROLS_MODULE</td><td>User interface controls</td></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_FACTORY_MODULE</td><td>Factory methods for creating controls</td></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_FOCUS_MANAGER_MODULE</td><td>Classes for managing focus between controls and actors</td></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_MARKUP_PROCESSOR_MODULE</td><td>Text markup classes</td></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_SHADER_EFFECTS_MODULE</td><td>Shader effects classes</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_CONTROLS_MODULE Controls
 * @brief
 * @section  CAPI_DALI_TOOLKIT_CONTROLS_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section  CAPI_DALI_TOOLKIT_CONTROLS_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_ALIGNMENT_MODULE </td><td>Layout class</td></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_BUBBLE_EFFECT_MODULE </td><td>Particle emitter</td></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_BUTTONS_MODULE </td><td>Button</td></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_CLUSTER_MODULE </td><td>Styles for clusters of actors</td></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_DEFAULT_CONTROLS_MODULE </td><td>Factory methods for generating controls</td></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_IMAGE_VIEW_MODULE </td><td>Classes for displaying images</td></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_POPUP_MODULE </td><td></td>Popup control</tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_SCROLLABLE_MODULE </td><td>Scrollable containers</td></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_SUPER_BLUR_VIEW_MODULE </td><td>Control to perform blurring on an image</td></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_TEXT_INPUT_MODULE </td><td>Input control using the virtual keyboard</td></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_TEXT_VIEW_MODULE </td><td>Control to display text in different styles</td></tr>
 * <tr><td>@ref Dali::Toolkit::Control </td><td>Control is the base class for all controls.</td></tr>
 * <tr><td>@ref Dali::Toolkit::ControlImpl </td><td>This is the internal base class for all controls.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_CONTROLS_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_ALIGNMENT_MODULE Alignment
 * @brief
 * @section CAPI_DALI_TOOLKIT_ALIGNMENT_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_ALIGNMENT_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Toolkit::Alignment </td><td>Alignment is a container which provides an easy way to align other actors inside its boundary.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_CONTROLS_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_BUBBLE_EFFECT_MODULE Bubble Effect
 * @brief
 * @section  CAPI_DALI_TOOLKIT_BUBBLE_EFFECT_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_BUBBLE_EFFECT_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Toolkit::BubbleEmitter </td><td>BubbleEmitter is used to display lots of moving bubbles on the stage.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_CONTROLS_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_BUTTONS_MODULE Buttons
 * @brief
 * @section  CAPI_DALI_TOOLKIT_BUTTONS_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_BUTTONS_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Toolkit::Button </td><td>Button is a base class for different kind of buttons.</td></tr>
 * <tr><td>@ref Dali::Toolkit::PushButton </td><td>A PushButton changes its appearance when is pressed and returns to its original when is released.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_CONTROLS_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_CLUSTER_MODULE Cluster
 * @brief
 * @section CAPI_DALI_TOOLKIT_CLUSTER_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_CLUSTER_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Toolkit::ClusterStyle </td><td>A ClusterStyle describes the constraints, which are imposed on the child actors in the cluster.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_CONTROLS_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_DEFAULT_CONTROLS_MODULE Default Controls
 * @brief
 * @section CAPI_DALI_TOOLKIT_DEFAULT_CONTROLS_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_DEFAULT_CONTROLS_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td> Factory methods </td><td>Dali::Toolkit::CreateSolidColorActor(), Dali::Toolkit::CreatePushButton()</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_CONTROLS_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_IMAGE_VIEW_MODULE Image View
 * @brief
 * @section  CAPI_DALI_TOOLKIT_IMAGE_VIEW_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_IMAGE_VIEW_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Toolkit::MaskedImageView </td><td>MaskedImageView displays the result of an image created from a masking operation.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_CONTROLS_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_POPUP_MODULE Popup
 * @brief
 * @section  CAPI_DALI_TOOLKIT_POPUP_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_POPUP_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Toolkit::Popup </td><td>Popup contains content that can come into focus when activated, and out of focus when deactivated.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_CONTROLS_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_SCROLLABLE_MODULE Scrollable
 * @brief
 * @section  CAPI_DALI_TOOLKIT_SCROLLABLE_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_SCROLLABLE_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * (via touch) or automatically. </td></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_ITEM_VIEW_MODULE</td><td> Item view </td></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_SCROLL_VIEW_MODULE</td><td> Scroll view </td></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_SCROLL_CONNECTOR_MODULE</td><td> Scroll Connector </td></tr>
 * <tr><td>@ref Dali::Toolkit::Scrollable </td><td>Base class for derived Scrollables that contains actors that can be scrolled manually
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_SCROLLABLE_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_ITEM_VIEW_MODULE   Item view
 * @brief
 * @section  CAPI_DALI_TOOLKIT_ITEM_VIEW_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_ITEM_VIEW_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Toolkit::GridLayout </td><td>An ItemView layout which arranges items in a grid.</td></tr>
 * <tr><td>@ref Dali::Toolkit::ItemFactory </td><td>ItemFactory is an abstract interface for providing actors to ItemView.</td></tr>
 * <tr><td>@ref Dali::Toolkit::ItemRange </td><td>A support class for managing ranges of items.</td></tr>
 * <tr><td>@ref Dali::Toolkit::ItemLayout </td><td>An ItemLayout describes the constraints which are imposed on items in the layout.</td></tr>
 * <tr><td>@ref Dali::Toolkit::ItemView </td><td>ItemView is a scrollable layout container.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_SCROLLABLE_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_SCROLL_CONNECTOR_MODULE   Scroll Connector
 * @brief
 * @section  CAPI_DALI_TOOLKIT_SCROLL_CONNECTOR_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_SCROLL_CONNECTOR_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Toolkit::ScrollConnector </td><td>ScrollConnector is used to connect scrollable containers with components such as scroll bars, indicators etc.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_SCROLLABLE_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_SCROLL_VIEW_MODULE Scroll view
 * @brief
 * @section  CAPI_DALI_TOOLKIT_SCROLL_VIEW_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_SCROLL_VIEW_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Toolkit::ScrollViewCubeEffect </td><td>This effect causes Actors to appear to rotate around a 3D cube.</td></tr>
 * <tr><td>@ref Dali::Toolkit::ScrollViewCustomEffect </td><td>This class has many transition effects for use when scrolling pages, e.g opacity, rotation, swing, translation.</td></tr>
 * <tr><td>@ref Dali::Toolkit::ScrollViewEffect </td><td>ScrollView Effect base class, used to apply custom effects to a ScrollView instance.</td></tr>
 * <tr><td>@ref Dali::Toolkit::ScrollViewPageSpiralEffect </td><td>This effect cause each page in a scroll-view to move along a spiral.</td></tr>
 * <tr><td>@ref Dali::Toolkit::ScrollViewSlideEffect </td><td>ScrollView effect that uses slides for transitioning pages.</td></tr>
 * <tr><td>@ref Dali::Toolkit::ScrollViewTwistEffect </td><td>ScrollView effect that twists pages onto screen when transitioning.</td></tr>
 * <tr><td>@ref Dali::Toolkit::RulerDomain </td><td>Used for specifying minimum/maximum extents of a ruler.</td></tr>
 * <tr><td>@ref Dali::Toolkit::Ruler </td><td>Abstract class to define scroll axes.</td></tr>
 * <tr><td>@ref Dali::Toolkit::DefaultRuler </td><td>Concrete implementation of Dali::Toolkit::Ruler that has no snapping and has one single page.</td></tr>
 * <tr><td>@ref Dali::Toolkit::FixedRuler </td><td>Concrete implementation of Ruler that has fixed snapping.</td></tr>
 * <tr><td>@ref Dali::Toolkit::ScrollView </td><td>ScrollView contains actors that can be scrolled manually (via touch) or automatically.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_CONTROLS_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_SUPER_BLUR_VIEW_MODULE Super Blur View
 * @brief
 * @section  CAPI_DALI_TOOLKIT_SUPER_BLUR_VIEW_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_SUPER_BLUR_VIEW_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Toolkit::SuperBlurView </td><td>SuperBlurView accepts an image as input, and displays/animates it with various blur strengths.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_CONTROLS_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_TEXT_INPUT_MODULE Text Input
 * @brief
 * @section  CAPI_DALI_TOOLKIT_TEXT_INPUT_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_TEXT_INPUT_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Toolkit::TextInput </td><td>TextInput Actor takes input one character at a time and displays it as a string within an input box.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_CONTROLS_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_TEXT_VIEW_MODULE Text view
 * @brief
 * @section CAPI_DALI_TOOLKIT_TEXT_VIEW_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_TEXT_VIEW_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Toolkit::TextView </td><td>TextView is a layout container for text with alignment, multi-line wrapping and formatting support.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_FACTORY_MODULE Factory
 * @brief
 * @section CAPI_DALI_TOOLKIT_FACTORY_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_FACTORY_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Toolkit::LocalizedControlFactory </td><td>This class provides functionality for creating controls which have localized text.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_FOCUS_MANAGER_MODULE Focus Manager
 * @brief
 * @section  CAPI_DALI_TOOLKIT_FOCUS_MANAGER_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_FOCUS_MANAGER_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Toolkit::FocusManager </td><td>Manages registration of actors in a focus chain and changing the focused
 * actor within that chain.</td></tr>
 * <tr><td>@ref Dali::Toolkit::KeyboardFocusManager </td><td>Provides the functionality of handling keyboard navigation and maintaining the two dimensional keyboard focus chain.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_MARKUP_PROCESSOR_MODULE Markup Processor
 * @brief
 * @section CAPI_DALI_TOOLKIT_MARKUP_PROCESSOR_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_MARKUP_PROCESSOR_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Toolkit::MarkupProcessor::StyledText </td><td>A pair of Dali::Text and Dali::TextStyle.</td></tr>
 * <tr><td>@ref Dali::Toolkit::MarkupProcessor::StyledTextArray </td><td> Vector of StyledText </td></tr>
 * <tr><td>@ref Dali::Toolkit::MarkupProcessor </td><td>
Dali::Toolkit::MarkupProcessor::GetStyledTextArray()
Dali::Toolkit::MarkupProcessor::GetPlainString()
Dali::Toolkit::MarkupProcessor::GetMarkupString()
Dali::Toolkit::MarkupProcessor::SetTextStyle()
Dali::Toolkit::MarkupProcessor::SetTextStyle()
Dali::Toolkit::MarkupProcessor::SetTextStyleToRange()
</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_TOOLKIT_MODULE
 * @defgroup CAPI_DALI_TOOLKIT_SHADER_EFFECTS_MODULE Shader Effects
 * @brief
 * @section  CAPI_DALI_TOOLKIT_SHADER_EFFECTS_MODULE_HEADER Required Header
 *  \#include <dali-toolkit/dali-toolkit.h>
 * @section CAPI_DALI_TOOLKIT_SHADER_EFFECTS_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Toolkit::DissolveEffect </td><td>DissolveEffect is a custom shader effect to achieve Dissolve effects in Image actors.</td></tr>
 * <tr><td>@ref Dali::Toolkit::ImageRegionEffect </td><td>ImageRegionEffect is a custom shader effect to show only a region of an Image actor.</td></tr>
 * <tr><td>@ref Dali::Toolkit::IrisEffect </td><td>IrisEffect is a custom shader effect to achieve iris effects in Image actors.</td></tr>
 * <tr><td>@ref Dali::Toolkit::MaskEffect </td><td>MaskEffect is used to control which parts of an image are visible, using the alpha channel of a separate mask image.</td></tr>
 * <tr><td>@ref Dali::Toolkit::NinePatchMaskEffect </td><td>NinePatchMaskEffect is used to control which parts of an image are visible, using the alpha channel of a separate mask image.</td></tr>
 * <tr><td>@ref Dali::Toolkit::PageTurnBookSpineEffect </td><td>This is an assisting effect of PageTurnEffect to display a book spine on _static_ pages, and also to flip the image horizontally when needed.</td></tr>
 * <tr><td>@ref Dali::Toolkit::PageTurnEffect </td><td>PageTurnEffect is a custom shader effect to achieve a page turn effect for Image actors.</td></tr>
 * <tr><td>@ref Dali::Toolkit::RippleEffect </td><td>RippleEffect is a custom shader effect to achieve ripple effects on Image actors</td></tr>
 * <tr><td>@ref Dali::Toolkit::Ripple2DEffect </td><td>Ripple2DEffect is a custom shader effect to achieve 2d ripple effects on Image actors</td></tr>
 * <tr><td>@ref Dali::Toolkit::SwirlEffect </td><td>SwirlEffect is a custom shader effect to achieve swirl effects in Image actors.</td></tr>
 * </table>
 */


#endif //__DALI_TOOLKIT_DOC_H__
