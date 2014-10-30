#ifndef __DALI_TOOLKIT_PUSH_BUTTON_FACTORY_H__
#define __DALI_TOOLKIT_PUSH_BUTTON_FACTORY_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/buttons/push-button.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Creates a push button with the given images.
 * Images will be shrunk to fit the button size keeping their aspect ratio.
 * @note Images won't be scaled to fill the whole button size.
 * @note If an image path is empty, this image is not set to the button.
 *
 * @param[in] releasedImagePath          Image path to be shown when the button is released.
 * @param[in] pressedImagePath           Image path to be shown when the button is pressed.
 * @param[in] backgroundImagePath        Image path to be shown as button background.
 * @param[in] dimmedReleasedImagePath    Image path to be shown when the button is released and dimmed.
 * @param[in] dimmedBackgroundImagePath  Image path to be shown as button background when the button is dimmed.
 * @return A handle to the new push button
 */
DALI_IMPORT_API PushButton CreatePushButton( const std::string& releasedImagePath, const std::string& pressedImagePath, const std::string& backgroundImagePath,
                             const std::string& dimmedReleasedImagePath, const std::string& dimmedBackgroundImagePath );

/**
 * @brief Creates a push button with the given images.
 *
 * Images will be shrunk to fit the button size keeping their aspect ratio.
 * @note Images won't be scaled to fill the whole button size.
 * @note If an image is an empty handle, this image is not set to the button.
 *
 * @param[in] releasedImageActor          Image to be shown when the button is released.
 * @param[in] pressedImageActor           Image to be shown when the button is pressed.
 * @param[in] backgroundImageActor        Image to be shown as button background.
 * @param[in] dimmedReleasedImageActor    Image to be shown when the button is released and dimmed.
 * @param[in] dimmedBackgroundImageActor  Image to be shown as button background when the button is dimmed.
 * @return A handle to the new pushbutton
 */
DALI_IMPORT_API PushButton CreatePushButton( Actor releasedImageActor, Actor pressedImageActor, Actor backgroundImageActor,
                             Actor dimmedReleasedImageActor, Actor dimmedBackgroundImageActor );

/**
 * @brief Creates a push button with the given background image.
 *
 * Background image will be shrunk to fit the button size keeping its aspect ratio.
 * @note Background image won't be scaled to fill the whole button size.
 *
 * @param[in] backgroundImagePath  Image path to be shown as button background.
 * @return a handle to the new push button
 */
DALI_IMPORT_API PushButton CreatePushButton( const std::string& backgroundImagePath );

/**
 * @brief Creates a push button with the given background image.
 *
 * Background image will be shrunk to fit the button size keeping its aspect ratio.
 * @note Background image won't be scaled to fill the whole button size.
 *
 * @param[in] backgroundImageActor  Image to be shown as button background.
 * @return a handle to the new push button
 */
DALI_IMPORT_API PushButton CreatePushButton( Actor backgroundImageActor );

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_PUSH_BUTTON_FACTORY_H__
