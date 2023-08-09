#ifndef DALI_TOOLKIT_TOOLKIT_TEXTURE_UPLOAD_MANAGER_H
#define DALI_TOOLKIT_TOOLKIT_TEXTURE_UPLOAD_MANAGER_H

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
#include <dali/devel-api/adaptor-framework/texture-upload-manager.h>

namespace Dali
{
namespace Graphics DALI_INTERNAL
{
class Controller;
} // namespace DALI_INTERNAL
} // namespace Dali

namespace Test
{
namespace TextureUploadManager
{
void InitalizeGraphicsController(Dali::Graphics::Controller& graphicsController);
}
} // namespace Test

#endif // DALI_TOOLKIT_TOOLKIT_TEXTURE_UPLOAD_MANAGER_H
