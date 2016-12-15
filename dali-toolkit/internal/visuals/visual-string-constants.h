#ifndef DALI_TOOLKIT_INTERNAL_VISUAL_STRING_CONSTANTS_H
#define DALI_TOOLKIT_INTERNAL_VISUAL_STRING_CONSTANTS_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/scripting/enum-helper.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

// Visual type
extern const char * const VISUAL_TYPE;
extern const Dali::Scripting::StringEnum VISUAL_TYPE_TABLE[];
extern const unsigned int VISUAL_TYPE_TABLE_COUNT;

// Custom shader
extern const char * const CUSTOM_SHADER;
extern const char * const CUSTOM_VERTEX_SHADER;
extern const char * const CUSTOM_FRAGMENT_SHADER;
extern const char * const CUSTOM_SUBDIVIDE_GRID_X;
extern const char * const CUSTOM_SUBDIVIDE_GRID_Y;
extern const char * const CUSTOM_SHADER_HINTS;

// Transform
extern const char * const TRANSFORM;
extern const char * const SIZE;
extern const char * const OFFSET;
extern const char * const OFFSET_SIZE_MODE;
extern const char * const ORIGIN;
extern const char * const ANCHOR_POINT;

// Premultiplied alpha
extern const char * const PREMULTIPLIED_ALPHA;

// Mix color
extern const char * const MIX_COLOR;

// Image visual
extern const char * const IMAGE_URL_NAME;
extern const char * const ATLAS_RECT_UNIFORM_NAME;

// Text visual
extern const char * const TEXT_PROPERTY;

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_VISUAL_STRING_CONSTANTS_H */
