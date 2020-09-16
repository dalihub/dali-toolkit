#ifndef DALI_TOOLKIT_STYLE_MANAGER_DEVEL_H
#define DALI_TOOLKIT_STYLE_MANAGER_DEVEL_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/styling/style-manager.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelStyleManager
{
/**
 * @brief Gets all currently defined configurations.
 *
 * @pre The Builder has been initialized.
 * @param[in] styleManager The instance of StyleManager
 * @return A property map to the currently defined configurations
**/
DALI_TOOLKIT_API const Property::Map GetConfigurations(StyleManager styleManager);

} // namespace DevelStyleManager

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_STYLE_MANAGER_DEVEL_H
