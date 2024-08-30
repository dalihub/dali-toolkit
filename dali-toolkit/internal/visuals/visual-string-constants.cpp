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

// CLASS HEADER
#include <dali-toolkit/internal/visuals/visual-string-constants.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
DALI_ENUM_TO_STRING_TABLE_BEGIN(VISUAL_TYPE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Visual, BORDER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Visual, COLOR)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Visual, GRADIENT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Visual, IMAGE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Visual, MESH)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Visual, PRIMITIVE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Visual, TEXT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Visual, N_PATCH)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Visual, SVG)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Visual, ANIMATED_IMAGE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Visual, WIREFRAME)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::DevelVisual, ANIMATED_GRADIENT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::DevelVisual, ANIMATED_VECTOR_IMAGE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::DevelVisual, ARC)
DALI_ENUM_TO_STRING_TABLE_END(VISUAL_TYPE)

DALI_ENUM_TO_STRING_TABLE_BEGIN(VISUAL_SHADER_TYPE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, COLOR_SHADER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, COLOR_SHADER_ROUNDED_CORNER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, COLOR_SHADER_BORDERLINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, COLOR_SHADER_ROUNDED_BORDERLINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, COLOR_SHADER_BLUR_EDGE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, COLOR_SHADER_ROUNDED_CORNER_BLUR_EDGE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, COLOR_SHADER_CUTOUT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, COLOR_SHADER_CUTOUT_ROUNDED_CORNER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, COLOR_SHADER_CUTOUT_BORDERLINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, COLOR_SHADER_CUTOUT_ROUNDED_BORDERLINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, COLOR_SHADER_CUTOUT_BLUR_EDGE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, COLOR_SHADER_CUTOUT_ROUNDED_CORNER_BLUR_EDGE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, BORDER_SHADER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, BORDER_SHADER_ANTI_ALIASING)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, GRADIENT_SHADER_LINEAR_BOUNDING_BOX)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, GRADIENT_SHADER_LINEAR_BOUNDING_BOX_ROUNDED_CORNER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, GRADIENT_SHADER_LINEAR_BOUNDING_BOX_BORDERLINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, GRADIENT_SHADER_LINEAR_BOUNDING_BOX_ROUNDED_BORDERLINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, GRADIENT_SHADER_LINEAR_USER_SPACE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, GRADIENT_SHADER_LINEAR_USER_SPACE_ROUNDED_CORNER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, GRADIENT_SHADER_LINEAR_USER_SPACE_BORDERLINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, GRADIENT_SHADER_LINEAR_USER_SPACE_ROUNDED_BORDERLINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, GRADIENT_SHADER_RADIAL_BOUNDING_BOX)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, GRADIENT_SHADER_RADIAL_BOUNDING_BOX_ROUNDED_CORNER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, GRADIENT_SHADER_RADIAL_BOUNDING_BOX_BORDERLINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, GRADIENT_SHADER_RADIAL_BOUNDING_BOX_ROUNDED_BORDERLINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, GRADIENT_SHADER_RADIAL_USER_SPACE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, GRADIENT_SHADER_RADIAL_USER_SPACE_ROUNDED_CORNER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, GRADIENT_SHADER_RADIAL_USER_SPACE_BORDERLINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, GRADIENT_SHADER_RADIAL_USER_SPACE_ROUNDED_BORDERLINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER_ROUNDED_CORNER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER_BORDERLINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER_ROUNDED_BORDERLINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER_MASKING)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER_ROUNDED_CORNER_MASKING)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER_BORDERLINE_MASKING)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER_ROUNDED_BORDERLINE_MASKING)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER_ATLAS_DEFAULT_WRAP)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER_ATLAS_CUSTOM_WRAP)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER_YUV_TO_RGB)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER_ROUNDED_CORNER_YUV_TO_RGB)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER_BORDERLINE_YUV_TO_RGB)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER_ROUNDED_BORDERLINE_YUV_TO_RGB)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER_YUV_AND_RGB)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER_ROUNDED_CORNER_YUV_AND_RGB)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER_BORDERLINE_YUV_AND_RGB)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, IMAGE_SHADER_ROUNDED_BORDERLINE_YUV_AND_RGB)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, NATIVE_IMAGE_SHADER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, NATIVE_IMAGE_SHADER_ROUNDED_CORNER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, NATIVE_IMAGE_SHADER_BORDERLINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, NATIVE_IMAGE_SHADER_ROUNDED_BORDERLINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, NATIVE_IMAGE_SHADER_MASKING)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, NATIVE_IMAGE_SHADER_ROUNDED_CORNER_MASKING)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, NATIVE_IMAGE_SHADER_BORDERLINE_MASKING)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, NATIVE_IMAGE_SHADER_ROUNDED_BORDERLINE_MASKING)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, NATIVE_IMAGE_SHADER_ATLAS_DEFAULT_WRAP)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, NATIVE_IMAGE_SHADER_ATLAS_CUSTOM_WRAP)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, NINE_PATCH_SHADER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, NINE_PATCH_MASK_SHADER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, TEXT_SHADER_SINGLE_COLOR_TEXT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_OVERLAY)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_OVERLAY)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_EMOJI)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_EMOJI)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_OVERLAY_AND_EMOJI)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_OVERLAY_AND_EMOJI)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, TEXT_SHADER_MULTI_COLOR_TEXT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, TEXT_SHADER_MULTI_COLOR_TEXT_WITH_STYLE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, TEXT_SHADER_MULTI_COLOR_TEXT_WITH_OVERLAY)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, TEXT_SHADER_MULTI_COLOR_TEXT_WITH_STYLE_AND_OVERLAY)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, ANIMATED_GRADIENT_SHADER_LINEAR_BOUNDING_REFLECT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, ANIMATED_GRADIENT_SHADER_LINEAR_BOUNDING_REPEAT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, ANIMATED_GRADIENT_SHADER_LINEAR_BOUNDING_CLAMP)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, ANIMATED_GRADIENT_SHADER_LINEAR_USER_REFLECT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, ANIMATED_GRADIENT_SHADER_LINEAR_USER_REPEAT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, ANIMATED_GRADIENT_SHADER_LINEAR_USER_REPEAT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, ANIMATED_GRADIENT_SHADER_LINEAR_USER_CLAMP)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, ANIMATED_GRADIENT_SHADER_RADIAL_BOUNDING_REFLECT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, ANIMATED_GRADIENT_SHADER_RADIAL_BOUNDING_REPEAT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, ANIMATED_GRADIENT_SHADER_RADIAL_BOUNDING_CLAMP)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, ANIMATED_GRADIENT_SHADER_RADIAL_USER_REFLECT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, ANIMATED_GRADIENT_SHADER_RADIAL_USER_REPEAT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, ANIMATED_GRADIENT_SHADER_RADIAL_USER_CLAMP)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, WIREFRAME_SHADER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, ARC_BUTT_CAP_SHADER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::VisualFactoryCache::ShaderType, ARC_ROUND_CAP_SHADER)
DALI_ENUM_TO_STRING_TABLE_END(VISUAL_SHADER_TYPE)

// Visual Type
const char* const VISUAL_TYPE("visualType");

// Custom shader
const char* const CUSTOM_SHADER("shader");
const char* const CUSTOM_VERTEX_SHADER("vertexShader");
const char* const CUSTOM_FRAGMENT_SHADER("fragmentShader");
const char* const CUSTOM_SUBDIVIDE_GRID_X("subdivideGridX");
const char* const CUSTOM_SUBDIVIDE_GRID_Y("subdivideGridY");
const char* const CUSTOM_SHADER_HINTS("hints");

// Transform
const char* const TRANSFORM("transform");
const char* const SIZE("size");
const char* const OFFSET("offset");
const char* const OFFSET_SIZE_MODE("offsetSizeMode");
const char* const ORIGIN("origin");
const char* const ANCHOR_POINT("anchorPoint");
const char* const EXTRA_SIZE("extraSize");

// Premultipled alpha
const char* const PREMULTIPLIED_ALPHA("premultipliedAlpha");

// Mix color
const char* const MIX_COLOR("mixColor");
const char* const OPACITY("opacity");

// Fitting mode
const char* const VISUAL_FITTING_MODE("visualFittingMode");

// Border line
const char* const BORDERLINE_WIDTH("borderlineWidth");
const char* const BORDERLINE_COLOR("borderlineColor");
const char* const BORDERLINE_OFFSET("borderlineOffset");

// Corner radius
const char* const CORNER_RADIUS("cornerRadius");
const char* const CORNER_RADIUS_POLICY("cornerRadiusPolicy");

// Color visual
const char* const BLUR_RADIUS_NAME("blurRadius");
const char* const CUTOUT_POLICY_NAME("cutoutPolicy");

// Image visual
const char* const IMAGE_URL_NAME("url");
const char* const ATLAS_RECT_UNIFORM_NAME("uAtlasRect");
const char* const PIXEL_AREA_UNIFORM_NAME("pixelArea");
const char* const WRAP_MODE_UNIFORM_NAME("wrapMode");
const char* const IMAGE_WRAP_MODE_U("wrapModeU");
const char* const IMAGE_WRAP_MODE_V("wrapModeV");
const char* const IMAGE_BORDER("border");
const char* const ANIMATED_IMAGE_URLS_NAME("urls");
const char* const BATCH_SIZE_NAME("batchSize");
const char* const CACHE_SIZE_NAME("cacheSize");
const char* const FRAME_DELAY_NAME("frameDelay");
const char* const LOOP_COUNT_NAME("loopCount");
const char* const MASK_CONTENT_SCALE_NAME("maskContentScale");
const char* const CROP_TO_MASK_NAME("cropToMask");
const char* const LOAD_POLICY_NAME("loadPolicy");
const char* const RELEASE_POLICY_NAME("releasePolicy");
const char* const ORIENTATION_CORRECTION_NAME("orientationCorrection");
const char* const AUXILIARY_IMAGE_NAME("auxiliaryImage");
const char* const AUXILIARY_IMAGE_ALPHA_NAME("auxiliaryImageAlpha");
const char* const PLAY_RANGE_NAME("playRange");
const char* const PLAY_STATE_NAME("playState");
const char* const CURRENT_FRAME_NUMBER_NAME("currentFrameNumber");
const char* const TOTAL_FRAME_NUMBER_NAME("totalFrameNumber");
const char* const STOP_BEHAVIOR_NAME("stopBehavior");
const char* const LOOPING_MODE_NAME("loopingMode");
const char* const IMAGE_ATLASING("atlasing");
const char* const SYNCHRONOUS_LOADING("synchronousLoading");
const char* const IMAGE_FITTING_MODE("fittingMode");
const char* const IMAGE_SAMPLING_MODE("samplingMode");
const char* const IMAGE_DESIRED_WIDTH("desiredWidth");
const char* const IMAGE_DESIRED_HEIGHT("desiredHeight");
const char* const ALPHA_MASK_URL("alphaMaskUrl");
const char* const REDRAW_IN_SCALING_DOWN_NAME("redrawInScalingDown");
const char* const MASKING_TYPE_NAME("maskingType");
const char* const MASK_TEXTURE_RATIO_NAME("maskTextureRatio");
const char* const FAST_TRACK_UPLOADING_NAME("fastTrackUploading");
const char* const ENABLE_BROKEN_IMAGE("enableBrokenImage");
const char* const ENABLE_FRAME_CACHE("enableFrameCache");
const char* const NOTIFY_AFTER_RASTERIZATION("notifyAfterRasterization");
const char* const SYNCHRONOUS_SIZING("synchronousSizing");
const char* const FRAME_SPEED_FACTOR("frameSpeedFactor");

// Text visual
const char* const TEXT_PROPERTY("text");
const char* const FONT_FAMILY_PROPERTY("fontFamily");
const char* const FONT_STYLE_PROPERTY("fontStyle");
const char* const POINT_SIZE_PROPERTY("pointSize");
const char* const MULTI_LINE_PROPERTY("multiLine");
const char* const HORIZONTAL_ALIGNMENT_PROPERTY("horizontalAlignment");
const char* const VERTICAL_ALIGNMENT_PROPERTY("verticalAlignment");
const char* const TEXT_COLOR_PROPERTY("textColor");
const char* const ENABLE_MARKUP_PROPERTY("enableMarkup");
const char* const SHADOW_PROPERTY("shadow");
const char* const UNDERLINE_PROPERTY("underline");
const char* const OUTLINE_PROPERTY("outline");
const char* const BACKGROUND_PROPERTY("textBackground");
const char* const STRIKETHROUGH_PROPERTY("strikethrough");

//NPatch visual
const char* const BORDER_ONLY("borderOnly");
const char* const BORDER("border");

// non-animated property
const char* const GRADIENT_TYPE_NAME("gradientType");
const char* const UNIT_TYPE_NAME("unitType");
const char* const SPREAD_TYPE_NAME("spreadType");

// animated property
const char* const START_POSITION_NAME("startPosition");
const char* const START_COLOR_NAME("startColor");
const char* const END_POSITION_NAME("endPosition");
const char* const END_COLOR_NAME("endColor");
const char* const ROTATE_CENTER_NAME("rotateCenter");
const char* const ROTATE_AMOUNT_NAME("rotateAmount");
const char* const OFFSET_NAME("offset");

// animation parameter property
const char* const START_VALUE_NAME("startValue");
const char* const TARGET_VALUE_NAME("targetValue");
const char* const DIRECTION_TYPE_NAME("directionType");
const char* const DURATION_NAME("duration");
const char* const DELAY_NAME("delay");
const char* const REPEAT_NAME("repeat");
const char* const REPEAT_DELAY_NAME("repeatDelay");
const char* const MOTION_TYPE_NAME("motionType");
const char* const EASING_TYPE_NAME("easingType");

// common shader property
const char* const UNIFORM_START_POINT_NAME("start_point");
const char* const UNIFORM_START_COLOR_NAME("start_color");
const char* const UNIFORM_END_POINT_NAME("end_point");
const char* const UNIFORM_END_COLOR_NAME("end_color");
const char* const UNIFORM_ROTATE_CENTER_NAME("rotate_center");
const char* const UNIFORM_ROTATE_ANGLE_NAME("rotate_angle");
const char* const UNIFORM_OFFSET_NAME("gradient_offset");

// Border visual
const char* const COLOR_NAME("borderColor");
const char* const SIZE_NAME("borderSize");
const char* const ANTI_ALIASING("antiAliasing");

// properties: radial gradient
const char* const CENTER_NAME("center"); // Property::VECTOR2
const char* const RADIUS_NAME("radius"); // Property::FLOAT

// properties: linear&radial gradient
const char* const STOP_OFFSET_NAME("stopOffset");     // Property::Array FLOAT
const char* const STOP_COLOR_NAME("stopColor");       // Property::Array VECTOR4
const char* const UNITS_NAME("units");                // Property::String  "userSpaceOnUse | objectBoundingBox"
const char* const SPREAD_METHOD_NAME("spreadMethod"); // Property::String  "pad | reflect | repeat"

//mesh visual
const char* const OBJECT_URL_NAME("objectUrl");
const char* const MATERIAL_URL_NAME("materialUrl");
const char* const TEXTURES_PATH_NAME("texturesPath");
const char* const SHADING_MODE_NAME("shadingMode");
const char* const USE_MIPMAPPING_NAME("useMipmapping");
const char* const USE_SOFT_NORMALS_NAME("useSoftNormals");
const char* const LIGHT_POSITION_NAME("lightPosition");

//Primitive properties
const char* const PRIMITIVE_SHAPE("shape");
const char* const SLICES("slices");
const char* const STACKS("stacks");
const char* const SCALE_TOP_RADIUS("scaleTopRadius");
const char* const SCALE_BOTTOM_RADIUS("scaleBottomRadius");
const char* const SCALE_HEIGHT("scaleHeight");
const char* const SCALE_RADIUS("scaleRadius");
const char* const SCALE_DIMENSIONS("scaleDimensions");
const char* const BEVEL_PERCENTAGE("bevelPercentage");
const char* const BEVEL_SMOOTHNESS("bevelSmoothness");
const char* const LIGHT_POSITION_UNIFORM_NAME("lightPosition");

// Arc visual
const char* const THICKNESS_NAME("thickness");
const char* const START_ANGLE_NAME("startAngle");
const char* const SWEEP_ANGLE_NAME("sweepAngle");
const char* const CAP_NAME("cap");

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
