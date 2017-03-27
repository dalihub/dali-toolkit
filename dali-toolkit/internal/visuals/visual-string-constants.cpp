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

// CLASS HEADER
#include "visual-string-constants.h"

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

DALI_ENUM_TO_STRING_TABLE_BEGIN( VISUAL_TYPE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Visual, BORDER )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Visual, COLOR )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Visual, GRADIENT )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Visual, IMAGE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Visual, MESH )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Visual, PRIMITIVE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelVisual, TEXT )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelVisual, N_PATCH )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelVisual, SVG )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelVisual, ANIMATED_IMAGE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Visual, WIREFRAME )
DALI_ENUM_TO_STRING_TABLE_END( VISUAL_TYPE )

// Visual Type
const char * const VISUAL_TYPE( "visualType" );

// Custom shader
const char * const CUSTOM_SHADER( "shader" );
const char * const CUSTOM_VERTEX_SHADER( "vertexShader" );
const char * const CUSTOM_FRAGMENT_SHADER( "fragmentShader" );
const char * const CUSTOM_SUBDIVIDE_GRID_X( "subdivideGridX" );
const char * const CUSTOM_SUBDIVIDE_GRID_Y( "subdivideGridY" );
const char * const CUSTOM_SHADER_HINTS( "hints" );

// Transform
const char * const TRANSFORM( "transform" );
const char * const SIZE( "size" );
const char * const OFFSET( "offset" );
const char * const OFFSET_SIZE_MODE( "offsetSizeMode" );
const char * const ORIGIN( "origin" );
const char * const ANCHOR_POINT( "anchorPoint" );

// Premultipled alpha
const char * const PREMULTIPLIED_ALPHA( "premultipliedAlpha" );

// Mix color
const char * const MIX_COLOR( "mixColor" );
const char * const OPACITY( "opacity" );

// Image visual
const char * const IMAGE_URL_NAME( "url" );
const char * const ATLAS_RECT_UNIFORM_NAME( "uAtlasRect" );
const char * const PIXEL_AREA_UNIFORM_NAME( "pixelArea" );
const char * const WRAP_MODE_UNIFORM_NAME( "wrapMode" );
const char * const IMAGE_WRAP_MODE_U("wrapModeU");
const char * const IMAGE_WRAP_MODE_V("wrapModeV");
const char * const IMAGE_BORDER( "border" );

// Text visual
const char * const TEXT_PROPERTY( "text" );

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
