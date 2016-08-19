#ifndef __DALI_TOOLKIT_DOC_H__
#define __DALI_TOOLKIT_DOC_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

/**
 * @defgroup dali DALi
 * @ingroup CAPI_UI_FRAMEWORK
 *
 * @brief DALi is a cross-platform 3D UI Toolkit for embedded systems.
 *
 * @section dali-overview Overview
 *
 * DALi's 3D user interface engine enables you to create rich and high-performance
 * UI applications. DALi is based on OpenGL ES 2.0, but provides a clean
 * cross-platform C++ framework.
 * This means that you can use high-level DALi APIs instead of accessing
 * low-level OpenGL APIs directly.
 *
 * DALi consists of the following modules:
 * <table>
 * <tr>
 *    <th>Module</th>
 *    <th>Description</th>
 * </tr>
 * <tr>
 *    <td>@ref dali_core</td>
 *    <td>DALi Core provides core functionalities such as scene graph-based rendering, animation, and event handling.</td>
 * </tr>
 * <tr>
 *    <td>@ref dali_adaptor</td>
 *    <td>DALi Adaptor is a platform adaptation layer.</td>
 * </tr>
 * <tr>
 *    <td>@ref dali_toolkit</td>
 *    <td>DALi Toolkit provides UI components and various effects on top of the DALi Core.</td>
 * </tr>
 * </table>
 *
 * The layer diagram for DALi modules is shown below:
 * @image html dali-modules.png "Figure: DALi modules"
 *
 * @defgroup dali_toolkit DALi Toolkit
 *
 * @brief DALi Toolkit provides UI components and various effects on top of the DALi Core.
 *
 * @section dali_core_overview Overview
 *
 * DALi Toolkit consists of the following groups of API:
 *
 * <table>
 * <tr>
 *    <th>API Group</th>
 *    <th>Description</th>
 * </tr>
 * <tr>
 *    <td>@ref dali_toolkit_controls</td>
 *    <td>Controls are interactive components for layouting the user interface.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_toolkit_controls_alignment</td>
 *    <td>Alignment is a container which provides an easy way to align other actors inside its boundary.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_toolkit_controls_buttons</td>
 *    <td>Button is a small object on UI that you can press.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_toolkit_controls_gaussian_blur_view</td>
 *    <td>GaussianBlurView provides a render process that blurs an image.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_toolkit_controls_image_view</td>
 *    <td>ImageView is a control displying an image.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_toolkit_controls_scroll_bar</td>
 *    <td>ScrollBar is a component that can be linked to the scrollable objects.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_toolkit_controls_scrollable</td>
 *    <td>Scrollable contains scrolled controls.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:2em">@ref dali_toolkit_controls_item_view</td>
 *    <td>ItemView class is a scrollable container that can contain many items.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:2em">@ref dali_toolkit_controls_scroll_view</td>
 *    <td>ScrollView class provides scrollable view which contains actors and can be scrolled automatically or manually by panning.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_toolkit_controls_table_view</td>
 *    <td>TableView class is a layout container for aligning child actors in a grid like layout.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_toolkit_controls_text_controls</td>
 *    <td>Controls for displaying text or text input.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_toolkit_controls_flex_container</td>
 *    <td>FlexContainer is a container for Flexbox layout.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_toolkit_controls_video_view</td>
 *    <td>VideoView is a control for video playback and display.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_toolkit_controls_slider</td>
 *    <td>Slider is a control to enable sliding an indicator between two values.</td>
 * </tr>
 * <tr>
 *    <td>@ref dali_toolkit_managers</td>
 *    <td>Singleton classes for managing application-wide functionalities.</td>
 * </tr>
 * </table>
 *
 * \ifnot show_tizen_feature
 *
 * @section dali_toolkit_feature Related Features
 * This API is related with the following features:\n
 *    - http://tizen.org/feature/opengles.version.2_0\n
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature Element</b>.</a>
 *
 * \endif
 *
 * @ingroup dali
 * @{
 *   @defgroup dali_toolkit_controls Controls
 *   @brief Controls are interactive components for layouting the user interface.

 *   @{
 *     @defgroup dali_toolkit_controls_alignment Alignment
 *     @brief Alignment is a container which provides an easy way to align other actors inside its boundary.

 *     @defgroup dali_toolkit_controls_buttons Buttons
 *     @brief Button is a small object on UI that you can press.

 *     @defgroup dali_toolkit_controls_gaussian_blur_view Gaussian Blur View
 *     @brief GaussianBlurView provides a render process that blurs an image.

 *     @defgroup dali_toolkit_controls_image_view Image View
 *     @brief ImageView is a control displying an image.

 *     @defgroup dali_toolkit_controls_scroll_bar Scroll Bar
 *     @brief ScrollBar is a component that can be linked to the scrollable objects.

 *     @defgroup dali_toolkit_controls_scrollable Scrollable
 *     @brief Scrollable contains scrolled controls.

 *     @{
 *       @defgroup dali_toolkit_controls_item_view Item View
 *       @brief ItemView class is a scrollable container that can contain many items.

 *       @defgroup dali_toolkit_controls_scroll_view Scroll View
 *       @brief ScrollView class provides scrollable view which contains actors and can be scrolled automatically or manually by panning.

 *     @}
 *     @defgroup dali_toolkit_controls_table_view Table View
 *     @brief TableView class is a layout container for aligning child actors in a grid like layout.

 *     @defgroup dali_toolkit_controls_text_controls Text Controls
 *     @brief Controls for displaying text or text input.

 *     @defgroup dali_toolkit_controls_flex_container Flex Container
 *     @brief FlexContainer is a container for Flexbox layout.

 *     @defgroup dali_toolkit_controls_video_view Video View
 *     @brief VideoView is a control for video playback and display.

 *     @defgroup dali_toolkit_controls_slider Slider
 *     @brief Slider is a control to enable sliding an indicator between two values.

 *   @}

 *   @defgroup dali_toolkit_managers Managers
 *   @brief Singleton classes for managing application-wide functionalities.

 * @}
 */


#endif  /* __DALI_TOOLKIT_DOC_H__ */
