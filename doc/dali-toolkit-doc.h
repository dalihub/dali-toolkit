#ifndef DALI_TOOLKIT_DOC_H
#define DALI_TOOLKIT_DOC_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
 * <tr>
 *    <td>@ref dali_scene3d</td>
 *    <td>DALi Scene3D provides functionalities for rendering 3D models and managing the 3D scene environment.</td>
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
 * @section dali_toolkit_overview Overview
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
 *    <td>ImageView is a control displaying an image.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_toolkit_controls_model3d_view</td>
 *    <td>Model3dView is a control for displaying 3d geometry.</td>
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
 *    <td style="padding-left:1em">@ref dali_toolkit_controls_progress_bar</td>
 *    <td>ProgressBar is a control to give the user an indication of the progress of an operation.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_toolkit_controls_camera_view</td>
 *    <td>CameraView is a control for camera display.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_toolkit_controls_web_view</td>
 *    <td>WebView is a control for displaying web content.</td>
 * </tr>
 * <tr>
 *    <td>@ref dali_toolkit_managers</td>
 *    <td>Singleton classes for managing application-wide functionalities.</td>
 * </tr>
 * <tr>
 *    <td>@ref dali_toolkit_visuals</td>
 *    <td>Visuals can control rendering the contents as using Property.</td>
 * </tr>
 * <tr>
 *    <td>@ref dali_toolkit_image_loader</td>
 *    <td>The ImageLoader is used to load pixel data from a URL.</td>
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
 * More details on featuring your application can be found from <a href="https://docs.tizen.org/application/tizen-studio/native-tools/manifest-text-editor#feature-element"><b>Feature Element</b>.</a>
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

 *     @defgroup dali_toolkit_controls_canvas_view CanvasView
 *     @brief CanvasView is a class for displaying an vector primitives.
 *
 *     @defgroup dali_toolkit_controls_gaussian_blur_view Gaussian Blur View
 *     @brief GaussianBlurView provides a render process that blurs an image.

 *     @defgroup dali_toolkit_controls_image_view Image View
 *     @brief ImageView is a control displaying an image.

 *     @defgroup dali_toolkit_controls_model3d_view Model3dView
 *     @brief Model3dView is a control for displaying 3d geometry.

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

 *     @defgroup dali_toolkit_controls_progress_bar Progress Bar
 *     @brief ProgressBar is a control to give the user an indication of the progress of an operation.

 *     @defgroup dali_toolkit_controls_camera_view Camera View
 *     @brief CameraView is a control for camera display.

 *     @defgroup dali_toolkit_controls_web_view Web View
 *     @brief WebView is a control for displaying web content.

 *   @}

 *   @defgroup dali_toolkit_managers Managers
 *   @brief Singleton classes for managing application-wide functionalities.

 *   @defgroup dali_toolkit_visuals Visuals
 *   @brief Visuals can control rendering the contents as using Property.

 *   @defgroup dali_toolkit_image_loader Image Loader
 *   @brief The ImageLoader is used to load pixel data from a URL.

 * @}

 * @defgroup dali_scene3d DALi Scene3D
 *
 * @brief DALi Scene3D provides functionalities for rendering 3D models and managing the 3D scene environment.
 *
 * @section dali_scene3d_overview Overview
 *
 * DALi Scene3D consists of the following groups of API:
 *
 * <table>
 * <tr>
 *    <th>API Group</th>
 *    <th>Description</th>
 * </tr>
 * <tr>
 *    <td>@ref dali_scene3d_model_components</td>
 *    <td>Classes for manipulating 3D model objects.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_scene3d_model_node</td>
 *    <td>ModelNode is a class for representing the Node of Model in Scene3D.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_scene3d_model_primitive</td>
 *    <td>ModelPrimitive is required to draw the mesh geometry defined by the user.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_scene3d_material</td>
 *    <td>Material is for setting material properties of 3D models.</td>
 * </tr>
 * <tr>
 *    <td>@ref dali_scene3d_model_motion</td>
 *    <td>Classes for controlling 3D model animations.</td>
 * </tr>
 * <tr>
 *    <td>@ref dali_scene3d_light</td>
 *    <td>Light defines a 3D light source.</td>
 * </tr>
 * <tr>
 *    <td>@ref dali_scene3d_controls</td>
 *    <td>Components for building the 3D scene.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_scene3d_controls_model</td>
 *    <td>Model is a control to show 3D model objects.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_scene3d_controls_scene_view</td>
 *    <td>SceneView is a control to show multiple 3D objects in a single 3D scene.</td>
 * </tr>
 * <tr>
 *    <td style="padding-left:1em">@ref dali_scene3d_controls_panel</td>
 *    <td>Panel is a control to show 2D UI on 3D Scene.</td>
 * </tr>
 * </table>

 * @ingroup dali
 * @{
 *   @defgroup dali_scene3d_model_components Model Components
 *   @brief Classes for manipulating 3D model objects.

 *   @{
 *     @defgroup dali_scene3d_model_node Model Node
 *     @brief ModelNode is a class for representing the Node of Model in Scene3D.

 *     @defgroup dali_scene3d_model_primitive Model Primitive
 *     @brief ModelPrimitive is required to draw the mesh geometry defined by the user.

 *     @defgroup dali_scene3d_material Material
 *     @brief Material is for setting material properties of 3D models.

 *   @}

 *   @defgroup dali_scene3d_model_motion Model Motions
 *   @brief Classes for controlling 3D model animations.

 *   @defgroup dali_scene3d_light Light
 *   @brief Light defines a 3D light source.

 *   @defgroup dali_scene3d_controls Controls
 *   @brief Components for building the 3D scene.

 *   @{
 *     @defgroup dali_scene3d_controls_model Model
 *     @brief Model is a control to show 3D model objects.

 *     @defgroup dali_scene3d_controls_scene_view Scene View
 *     @brief SceneView is a control to show multiple 3D objects in a single 3D scene.

 *     @defgroup dali_scene3d_controls_panel Panel
 *     @brief Panel is a control to show 2D UI on 3D Scene.

 *   @}

 * @}

 */

#endif // DALI_TOOLKIT_DOC_H
