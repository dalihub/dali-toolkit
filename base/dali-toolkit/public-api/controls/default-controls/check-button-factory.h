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

namespace Dali DALI_IMPORT_API
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
 * @param[in] checkedImagePath Image path to be shown as checked button.
 * @param[in] dimmedBackgroundImagePath Image path to be shown as button dimmed background.
 * @param[in] dimmedCheckedImagePath Image path to be shown as dimmed checked button.
 */
CheckBoxButton CreateCheckBoxButton( const std::string& backgroundImagePath, const std::string& checkedImagePath, const std::string& dimmedBackgroundImagePath, const std::string& dimmedCheckedImagePath );

/**
 * Creates a check box button with the given images.
 * Images will be shrunk to fit the button size keeping their aspect ratio.
 * @note Images won't be scaled to fill the whole button size.
 * @note If an image is an empty handle, this image is not set to the button.
 *
 * @param[in] backgroundImageActor  Image to be shown as button background.
 * @param[in] checkedImageActor  Image to be shown as checked button.
 * @param[in] dimmedBackgroundImageActor Image to be shown as button dimmed background.
 * @param[in] dimmedCheckedImagActor Image to be shown as dimmed checked button.
 */
CheckBoxButton CreateCheckBoxButton( Actor backgroundImageActor, Actor checkedImageActor, Actor dimmedBackgroundImageActor, Actor dimmedCheckedImagActor );

/**
 * Creates a check box button with the given background and checked images.
 * Background and checked images will be shrunk to fit the button size keeping their aspect ratio.
 * @note Background and checked images won't be scaled to fill the whole button size.
 *
 * @param[in] backgroundImagePath  Image path to be shown as button background.
 * @param[in] checkedImagePath  Image path to be shown as checked button.
 */
CheckBoxButton CreateCheckBoxButton( const std::string& backgroundImagePath, const std::string& checkedImagePath );

/**
 * Creates a check box button with the given background and checked images.
 * Background and checked images will be shrunk to fit the button size keeping their aspect ratio.
 * @note Background and checked images won't be scaled to fill the whole button size.
 *
 * @param[in] backgroundImageActor  Image to be shown as button background.
 * @param[in] checkedImageActor  Image to be shown as checked button.
 */
CheckBoxButton CreateCheckBoxButton( Actor backgroundImageActor, Actor checkedImageActor );
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_CHECK_BUTTON_FACTORY_H__
