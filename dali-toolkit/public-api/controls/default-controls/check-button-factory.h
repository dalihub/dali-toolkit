#ifndef __DALI_TOOLKIT_CHECK_BUTTON_FACTORY_H__
#define __DALI_TOOLKIT_CHECK_BUTTON_FACTORY_H__

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
#include <dali-toolkit/public-api/controls/buttons/check-box-button.h>

namespace Dali
{

namespace Toolkit
{

/**
 * Creates a check box button with the given images.
 * Images will be shrunk to fit the button size keeping their aspect ratio.
 * @note Images won't be scaled to fill the whole button size.
 * @note If an image path is empty, this image is not set to the button.
 *
 * @param[in] backgroundImagePath Image path to be shown as button background.
 * @param[in] selectedImagePath Image path to be shown as selected button.
 * @param[in] disabledBackgroundImagePath Image path to be shown as button isabled background.
 * @param[in] disabledSelectedImagePath Image path to be shown as disabled selected button.
 */
DALI_IMPORT_API CheckBoxButton CreateCheckBoxButton( const std::string& backgroundImagePath, const std::string& selectedImagePath, const std::string& disabledBackgroundImagePath, const std::string& disabledSelectedImagePath );

/**
 * Creates a check box button with the given images.
 * Images will be shrunk to fit the button size keeping their aspect ratio.
 * @note Images won't be scaled to fill the whole button size.
 * @note If an image is an empty handle, this image is not set to the button.
 *
 * @param[in] backgroundImageActor  Image to be shown as button background.
 * @param[in] selectedImageActor  Image to be shown as selected button.
 * @param[in] disabledBackgroundImageActor Image to be shown as button disabled background.
 * @param[in] disabledSelectedImagActor Image to be shown as disabled selected button.
 */
DALI_IMPORT_API CheckBoxButton CreateCheckBoxButton( Actor backgroundImageActor, Actor selectedImageActor, Actor disabledBackgroundImageActor, Actor disabledSelectedImagActor );

/**
 * Creates a check box button with the given background and selected images.
 * Background and selected images will be shrunk to fit the button size keeping their aspect ratio.
 * @note Background and selected images won't be scaled to fill the whole button size.
 *
 * @param[in] backgroundImagePath  Image path to be shown as button background.
 * @param[in] selectedImagePath  Image path to be shown as selected button.
 */
DALI_IMPORT_API CheckBoxButton CreateCheckBoxButton( const std::string& backgroundImagePath, const std::string& selectedImagePath );

/**
 * Creates a check box button with the given background and selected images.
 * Background and selected images will be shrunk to fit the button size keeping their aspect ratio.
 * @note Background and selected images won't be scaled to fill the whole button size.
 *
 * @param[in] backgroundImageActor  Image to be shown as button background.
 * @param[in] selectedImageActor  Image to be shown as selected button.
 */
DALI_IMPORT_API CheckBoxButton CreateCheckBoxButton( Actor backgroundImageActor, Actor selectedImageActor );

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_CHECK_BUTTON_FACTORY_H__
