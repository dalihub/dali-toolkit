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

// CLASS HEADER
#include <dali-scene3d/public-api/loader/string-callback.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali::Scene3D::Loader
{
void DefaultErrorCallback(const std::string& message)
{
  DALI_LOG_ERROR("%s", message.c_str());
}

} // namespace Dali::Scene3D::Loader
