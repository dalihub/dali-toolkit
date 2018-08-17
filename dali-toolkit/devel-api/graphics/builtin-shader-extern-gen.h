#ifndef GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H
#define GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H
#include <cstdint>
#include <vector>
#include <string>

extern std::vector<uint32_t> SHADER_ALPHA_DISCARD_EFFECT_FRAG;
extern std::vector<uint32_t> SHADER_BASIC_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_BASIC_SHADER_VERT;
extern std::vector<uint32_t> SHADER_BLUR_TWO_PASS_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_BORDER_VISUAL_ANTI_ALIASING_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_BORDER_VISUAL_ANTI_ALIASING_SHADER_VERT;
extern std::vector<uint32_t> SHADER_BORDER_VISUAL_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_BORDER_VISUAL_SHADER_VERT;
extern std::vector<uint32_t> SHADER_COLOR_VISUAL_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_COLOR_VISUAL_SHADER_VERT;
extern std::vector<uint32_t> SHADER_DISSOLVE_EFFECT_FRAG;
extern std::vector<uint32_t> SHADER_DISSOVE_EFFECT_VERT;
extern std::vector<uint32_t> SHADER_DISTANCE_FIELD_EFFECT_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_EXPERIMENTAL_SHADER_VERT;
extern std::vector<uint32_t> SHADER_GAUSSIAN_BLUR_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_GRADIENT_VISUAL_SHADER_0_FRAG;
extern std::vector<uint32_t> SHADER_GRADIENT_VISUAL_SHADER_0_VERT;
extern std::vector<uint32_t> SHADER_GRADIENT_VISUAL_SHADER_1_FRAG;
extern std::vector<uint32_t> SHADER_GRADIENT_VISUAL_SHADER_1_VERT;
extern std::vector<uint32_t> SHADER_IMAGE_REGION_EFFECT_VERT;
extern std::vector<uint32_t> SHADER_IMAGE_VISUAL_ATLAS_CLAMP_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_IMAGE_VISUAL_ATLAS_VARIOUS_WRAP_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_IMAGE_VISUAL_NO_ATLAS_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_IMAGE_VISUAL_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_IMAGE_VISUAL_SHADER_VERT;
extern std::vector<uint32_t> SHADER_MESH_VISUAL_NORMAL_MAP_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_MESH_VISUAL_NORMAL_MAP_SHADER_VERT;
extern std::vector<uint32_t> SHADER_MESH_VISUAL_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_MESH_VISUAL_SHADER_VERT;
extern std::vector<uint32_t> SHADER_MESH_VISUAL_SIMPLE_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_MESH_VISUAL_SIMPLE_SHADER_VERT;
extern std::vector<uint32_t> SHADER_MOTION_BLUR_EFFECT_FRAG;
extern std::vector<uint32_t> SHADER_MOTION_BLUR_EFFECT_VERT;
extern std::vector<uint32_t> SHADER_MOTION_STRETCH_EFFECT_FRAG;
extern std::vector<uint32_t> SHADER_MOTION_STRETCH_EFFECT_VERT;
extern std::vector<uint32_t> SHADER_NPATCH_VISUAL_3X3_SHADER_VERT;
extern std::vector<uint32_t> SHADER_NPATCH_VISUAL_MASK_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_NPATCH_VISUAL_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_NPATCH_VISUAL_SHADER_VERT;
extern std::vector<uint32_t> SHADER_PRIMITIVE_VISUAL_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_PRIMITIVE_VISUAL_SHADER_VERT;
extern std::vector<uint32_t> SHADER_SHADOW_VIEW_RENDER_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_SHADOW_VIEW_RENDER_SHADER_VERT;
extern std::vector<uint32_t> SHADER_TEXT_ATLAS_L8_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_TEXT_ATLAS_RGBA_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_TEXT_ATLAS_SHADER_VERT;
extern std::vector<uint32_t> SHADER_TEXT_DECORATOR_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_TEXT_DECORATOR_SHADER_VERT;
extern std::vector<uint32_t> SHADER_TEXT_SCROLLER_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_TEXT_SCROLLER_SHADER_VERT;
extern std::vector<uint32_t> SHADER_TEXT_VISUAL_MULTI_COLOR_TEXT_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_TEXT_VISUAL_MULTI_COLOR_TEXT_WITH_STYLE_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_TEXT_VISUAL_SHADER_VERT;
extern std::vector<uint32_t> SHADER_TEXT_VISUAL_SINGLE_COLOR_TEXT_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_TEXT_VISUAL_SINGLE_COLOR_TEXT_WITH_EMOJI_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_TEXT_VISUAL_SINGLE_COLOR_TEXT_WITH_STYLE_AND_EMOJI_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_TEXT_VISUAL_SINGLE_COLOR_TEXT_WITH_STYLE_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_VIDEO_VIEW_UNDERLAY_FRAG;
extern std::vector<uint32_t> SHADER_VIDEO_VIEW_UNDERLAY_VERT;
extern std::vector<uint32_t> SHADER_WIREFRAME_VISUAL_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_WIREFRAME_VISUAL_SHADER_VERT;
extern "C" std::vector<uint32_t> GraphicsGetBuiltinShader( const std::string& tag );
#define GraphicsGetBuiltinShaderId( x ) GraphicsGetBuiltinShader( #x )
#endif // GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H
