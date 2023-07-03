#ifndef DALI_TOOLKIT_STYLE_MANAGER_DEVEL_H
#define DALI_TOOLKIT_STYLE_MANAGER_DEVEL_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/styling/style-manager.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelStyleManager
{
/**
 * @brief The Type of BrokenImage
 */
enum class BrokenImageType
{
  SMALL,
  NORMAL,
  LARGE
};

using BrokenImageChangedSignalType = Signal<void(StyleManager)>;

/**
 * @brief Gets all currently defined configurations.
 *
 * @pre The Builder has been initialized.
 * @param[in] styleManager The instance of StyleManager
 * @return A property map to the currently defined configurations
**/
DALI_TOOLKIT_API const Property::Map& GetConfigurations(StyleManager styleManager);

/**
   * @brief Sets an image to be used when a visual has failed to correctly render
   * @param[in] styleManager The instance of StyleManager
   * @param[in] brokenImageType The type of broken image
   * @param[in] brokenImageUrl The broken image url
   */
DALI_TOOLKIT_API void SetBrokenImageUrl(StyleManager styleManager, DevelStyleManager::BrokenImageType brokenImageType, const std::string& brokenImageUrl);

/**
   * @brief Gets an image to be used when a visual has failed to correctly render
   * @param[in] styleManager The instance of StyleManager
   * @param[in] brokenImageType BrokenImage type
   */
DALI_TOOLKIT_API std::string GetBrokenImageUrl(StyleManager styleManager, DevelStyleManager::BrokenImageType brokenImageType);

/**
 * @brief Get the Broken Image Url List
 *
 * This list is broken images stored in order of SMALL, NORMAL, and LARGE values except if the value is empty.
 * This API just makes the list without comparing size, so the application must set a value that matches the size.
 * @param styleManager The instance of StyleManager
 * @return A List of Broken Image Url
 */
DALI_TOOLKIT_API std::vector<std::string> GetBrokenImageUrlList(StyleManager styleManager);

/**
 * @brief This signal is emitted when the URL of the broken image is set
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName(StyleManager styleManager)
 * @endcode
 * @param styleManager The instance of StyleManager
 * @return The signal to connect to
 */
DALI_TOOLKIT_API BrokenImageChangedSignalType& BrokenImageChangedSignal(StyleManager styleManager);

} // namespace DevelStyleManager

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_STYLE_MANAGER_DEVEL_H
