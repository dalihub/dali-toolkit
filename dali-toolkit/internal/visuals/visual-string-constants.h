#ifndef DALI_TOOLKIT_INTERNAL_VISUAL_STRING_CONSTANTS_H
#define DALI_TOOLKIT_INTERNAL_VISUAL_STRING_CONSTANTS_H

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
#include <dali/devel-api/scripting/enum-helper.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
// Visual type
extern const char* const                 VISUAL_TYPE;
extern const Dali::Scripting::StringEnum VISUAL_TYPE_TABLE[];
extern const unsigned int                VISUAL_TYPE_TABLE_COUNT;

// Custom shader
extern const char* const CUSTOM_SHADER;
extern const char* const CUSTOM_VERTEX_SHADER;
extern const char* const CUSTOM_FRAGMENT_SHADER;
extern const char* const CUSTOM_SUBDIVIDE_GRID_X;
extern const char* const CUSTOM_SUBDIVIDE_GRID_Y;
extern const char* const CUSTOM_SHADER_HINTS;

// Transform
extern const char* const TRANSFORM;
extern const char* const SIZE;
extern const char* const OFFSET;
extern const char* const OFFSET_SIZE_MODE;
extern const char* const ORIGIN;
extern const char* const ANCHOR_POINT;
extern const char* const EXTRA_SIZE;

// Premultiplied alpha
extern const char* const PREMULTIPLIED_ALPHA;

// Mix color
extern const char* const MIX_COLOR;
extern const char* const OPACITY;

// Fitting mode
extern const char* const VISUAL_FITTING_MODE;

// Border line
extern const char* const BORDERLINE_WIDTH;
extern const char* const BORDERLINE_COLOR;
extern const char* const BORDERLINE_OFFSET;

// Corner radius
extern const char* const CORNER_RADIUS;
extern const char* const CORNER_RADIUS_POLICY;

// Color visual
extern const char* const BLUR_RADIUS_NAME;

// Image visual
extern const char* const IMAGE_URL_NAME;
extern const char* const ATLAS_RECT_UNIFORM_NAME;
extern const char* const PIXEL_AREA_UNIFORM_NAME;
extern const char* const WRAP_MODE_UNIFORM_NAME;
extern const char* const IMAGE_WRAP_MODE_U;
extern const char* const IMAGE_WRAP_MODE_V;
extern const char* const IMAGE_BORDER;
extern const char* const ANIMATED_IMAGE_URLS_NAME;
extern const char* const BATCH_SIZE_NAME;
extern const char* const CACHE_SIZE_NAME;
extern const char* const FRAME_DELAY_NAME;
extern const char* const LOOP_COUNT_NAME;
extern const char* const MASK_CONTENT_SCALE_NAME;
extern const char* const CROP_TO_MASK_NAME;
extern const char* const LOAD_POLICY_NAME;
extern const char* const RELEASE_POLICY_NAME;
extern const char* const ORIENTATION_CORRECTION_NAME;
extern const char* const AUXILLARY_IMAGE_NAME;
extern const char* const AUXILLARY_IMAGE_ALPHA_NAME;
extern const char* const PLAY_RANGE_NAME;
extern const char* const PLAY_STATE_NAME;
extern const char* const CURRENT_FRAME_NUMBER_NAME;
extern const char* const TOTAL_FRAME_NUMBER_NAME;
extern const char* const STOP_BEHAVIOR_NAME;
extern const char* const LOOPING_MODE_NAME;
extern const char* const IMAGE_ATLASING;
extern const char* const SYNCHRONOUS_LOADING;
extern const char* const IMAGE_FITTING_MODE;
extern const char* const IMAGE_SAMPLING_MODE;
extern const char* const IMAGE_DESIRED_WIDTH;
extern const char* const IMAGE_DESIRED_HEIGHT;
extern const char* const ALPHA_MASK_URL;
extern const char* const REDRAW_IN_SCALING_DOWN_NAME;
extern const char* const MASKING_TYPE_NAME;
extern const char* const MASK_TEXTURE_RATIO_NAME;
extern const char* const FAST_TRACK_UPLOADING_NAME;
extern const char* const ENABLE_BROKEN_IMAGE;

// Text visual
extern const char* const TEXT_PROPERTY;
extern const char* const FONT_FAMILY_PROPERTY;
extern const char* const FONT_STYLE_PROPERTY;
extern const char* const POINT_SIZE_PROPERTY;
extern const char* const MULTI_LINE_PROPERTY;
extern const char* const HORIZONTAL_ALIGNMENT_PROPERTY;
extern const char* const VERTICAL_ALIGNMENT_PROPERTY;
extern const char* const TEXT_COLOR_PROPERTY;
extern const char* const ENABLE_MARKUP_PROPERTY;
extern const char* const SHADOW_PROPERTY;
extern const char* const UNDERLINE_PROPERTY;
extern const char* const OUTLINE_PROPERTY;
extern const char* const BACKGROUND_PROPERTY;
extern const char* const STRIKETHROUGH_PROPERTY;

//NPatch visual
extern const char* const BORDER_ONLY;
extern const char* const BORDER;
extern const char* const AUXILIARY_IMAGE_NAME;
extern const char* const AUXILIARY_IMAGE_ALPHA_NAME;

// non-animated property
extern const char* const GRADIENT_TYPE_NAME;
extern const char* const UNIT_TYPE_NAME;
extern const char* const SPREAD_TYPE_NAME;

// animated property
extern const char* const START_POSITION_NAME;
extern const char* const START_COLOR_NAME;
extern const char* const END_POSITION_NAME;
extern const char* const END_COLOR_NAME;
extern const char* const ROTATE_CENTER_NAME;
extern const char* const ROTATE_AMOUNT_NAME;
extern const char* const OFFSET_NAME;

// animation parameter property
extern const char* const START_VALUE_NAME;
extern const char* const TARGET_VALUE_NAME;
extern const char* const DIRECTION_TYPE_NAME;
extern const char* const DURATION_NAME;
extern const char* const DELAY_NAME;
extern const char* const REPEAT_NAME;
extern const char* const REPEAT_DELAY_NAME;
extern const char* const MOTION_TYPE_NAME;
extern const char* const EASING_TYPE_NAME;

// common shader property
extern const char* const UNIFORM_START_POINT_NAME;
extern const char* const UNIFORM_START_COLOR_NAME;
extern const char* const UNIFORM_END_POINT_NAME;
extern const char* const UNIFORM_END_COLOR_NAME;
extern const char* const UNIFORM_ROTATE_CENTER_NAME;
extern const char* const UNIFORM_ROTATE_ANGLE_NAME;
extern const char* const UNIFORM_OFFSET_NAME;

// Border visual
extern const char* const COLOR_NAME;
extern const char* const SIZE_NAME;
extern const char* const ANTI_ALIASING;

// properties: radial gradient
extern const char* const CENTER_NAME; // Property::VECTOR2
extern const char* const RADIUS_NAME; // Property::FLOAT

// properties: linear&radial gradient
extern const char* const STOP_OFFSET_NAME;   // Property::Array FLOAT
extern const char* const STOP_COLOR_NAME;    // Property::Array VECTOR4
extern const char* const UNITS_NAME;         // Property::String  "userSpaceOnUse | objectBoundingBox"
extern const char* const SPREAD_METHOD_NAME; // Property::String  "pad | reflect | repeat"

//mesh visual
extern const char* const OBJECT_URL_NAME;
extern const char* const MATERIAL_URL_NAME;
extern const char* const TEXTURES_PATH_NAME;
extern const char* const SHADING_MODE_NAME;
extern const char* const USE_MIPMAPPING_NAME;
extern const char* const USE_SOFT_NORMALS_NAME;
extern const char* const LIGHT_POSITION_NAME;

//Primitive properties
extern const char* const PRIMITIVE_SHAPE;
extern const char* const SLICES;
extern const char* const STACKS;
extern const char* const SCALE_TOP_RADIUS;
extern const char* const SCALE_BOTTOM_RADIUS;
extern const char* const SCALE_HEIGHT;
extern const char* const SCALE_RADIUS;
extern const char* const SCALE_DIMENSIONS;
extern const char* const BEVEL_PERCENTAGE;
extern const char* const BEVEL_SMOOTHNESS;
extern const char* const LIGHT_POSITION_UNIFORM_NAME;

// Arc visual
extern const char* const THICKNESS_NAME;
extern const char* const START_ANGLE_NAME;
extern const char* const SWEEP_ANGLE_NAME;
extern const char* const CAP_NAME;

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_VISUAL_STRING_CONSTANTS_H */
