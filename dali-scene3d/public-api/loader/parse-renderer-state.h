#ifndef DALI_SCENE3D_LOADERER_INTERPRET_RENDERER_STATE_H
#define DALI_SCENE3D_LOADERER_INTERPRET_RENDERER_STATE_H
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

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/renderer-state.h>
#include <dali-scene3d/public-api/loader/string-callback.h>

namespace Dali::Scene3D::Loader
{
namespace RendererState
{
/*
 * @brief Attempts to interpret a string for renderer states, which can be a combination of the following
 *  (using '|' as a delimiter, if multiple specified):<br/>
 *  - one of Value, exc. for NONE or those with the _BITS, _SHIFT, _MASK or _ITEMS suffix;<br/>
 *  - DEPTH_FUNC:${one of Comparison::Type, exc. OMIT};<br/>
 *  - BLEND_(SRC_DST)_(RGB|ALPHA):${one of BlendFactor::Type, exc. OMIT};<br/>
 *  - BUFFER_MODE:${one of BufferMode::Type, exc. OMIT};
 * @param string The string to parse;
 * @param length The length of the string; If left at 0, strlen() will be called, which of
 *   course is only suitable i the string is null-terminated.
 * @param onError The callback to call when an error occurred.
 * @note No other characters are allowed.
 */
DALI_SCENE3D_API Type Parse(const char* string, size_t length = 0, StringCallback onError = DefaultErrorCallback);

} // namespace RendererState
} // namespace Dali::Scene3D::Loader

#endif //DALI_SCENE3D_LOADERER_INTERPRET_RENDERER_STATE_H
