#ifndef DALI_TOOLKIT_TOOLKIT_CANVAS_RENDERER_H
#define DALI_TOOLKIT_TOOLKIT_CANVAS_RENDERER_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/adaptor-framework/async-task-manager.h>

namespace Test::CanvasRenderer
{
// Set global rasterization result for CanvaseRender Rasterize() function.
void MarkRasterizationResult(bool result);

// Reset CanvaseRender Rasterize() function called signal.
void ResetRasterizationFlag();

// Get CanvaseRender Rasterize() function called or not.
bool IsRasterizationCalled();

} // namespace Test::CanvasRenderer

#endif // DALI_TOOLKIT_TOOLKIT_CANVAS_RENDERER_H
