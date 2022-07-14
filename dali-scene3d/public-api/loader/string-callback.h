#ifndef DALI_SCENE3D_LOADER_STRING_CALLBACK_H
#define DALI_SCENE3D_LOADER_STRING_CALLBACK_H
/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include "dali-scene3d/public-api/api.h"

// EXTERNAL INCLUDES
#include <functional>
#include <string>

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
/*
 * @brief A callback to post strings to.
 */
using StringCallback = std::function<void(const std::string&)>;

/*
 * @brief Simply passes the formatted message to LOGE().
 */
DALI_SCENE3D_API void DefaultErrorCallback(const std::string& message);

} // namespace Loader
} // namespace Scene3D
} // namespace Dali

#endif //DALI_SCENE3D_LOADER_STRING_CALLBACK_H
