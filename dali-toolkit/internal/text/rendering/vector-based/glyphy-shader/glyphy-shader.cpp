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
#include <dali-toolkit/internal/text/rendering/vector-based/glyphy-shader/glyphy-shader.h>

// EXTERNAL HEADERS
#include <sstream>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/rendering/vector-based/glyphy-shader/glyphy-common-glsl.h>
#include <dali-toolkit/internal/text/rendering/vector-based/glyphy-shader/glyphy-sdf-glsl.h>

using namespace Dali;

namespace
{

const char* const ENABLE_EXTENSION_PREFIX =
"#extension GL_OES_standard_derivatives : enable\n"
"precision highp float;\n"
"precision highp int;\n";

const char* const VERTEX_SHADER_MAIN =
"uniform   mediump mat4    uProjection;\n"
"uniform   mediump mat4    uModelView;\n"
"uniform   mediump mat4    uMvpMatrix;\n"
"uniform           bool    uTextureMapped;\n"
"uniform   mediump vec4    uCustomTextureCoords;\n"
"attribute highp   vec2    aTexCoord;\n"
"varying   mediump vec2    vTexCoord;\n"
"uniform   mat3            uModelViewIT;\n"
"attribute mediump vec3    aNormal;\n"
"varying   mediump vec3    vNormal;\n"
"attribute mediump vec2    aPosition;\n"
"varying   mediump vec4    vVertex;\n"
"attribute mediump vec4    aColor;\n"
"varying   mediump vec4    vColor;\n"
"varying vec4 v_glyph;\n"
"\n"
"vec4\n"
"glyph_vertex_transcode (vec2 v)\n"
"{\n"
"  ivec2 g = ivec2 (v);\n"
"  ivec2 corner = ivec2 (mod (v, 2.));\n"
"  g /= 2;\n"
"  ivec2 nominal_size = ivec2 (mod (vec2(g), 64.));\n"
"  return vec4 (corner * nominal_size, g * 4);\n"
"}\n"
"\n"
"void\n"
"main()\n"
"{\n"
"  gl_Position = uMvpMatrix * vec4 (aPosition, 0.0, 1.0);\n"
"  v_glyph = glyph_vertex_transcode (aTexCoord);\n"
"  vColor = aColor;\n"
"}\n"
;

const char* const FRAGMENT_SHADER_PREFIX =
"struct Material\n"
"{\n"
"  mediump float mOpacity;\n"
"  mediump float mShininess;\n"
"  lowp    vec4  mAmbient;\n"
"  lowp    vec4  mDiffuse;\n"
"  lowp    vec4  mSpecular;\n"
"  lowp    vec4  mEmissive;\n"
"};\n"
"uniform sampler2D     sTexture;\n"
"uniform sampler2D     sOpacityTexture;\n"
"uniform sampler2D     sNormalMapTexture;\n"
"uniform sampler2D     sEffect;\n"
"varying mediump vec2 vTexCoord;\n"
"uniform Material      uMaterial;\n"
"uniform lowp  vec4    uColor;\n"
"varying highp vec4    vVertex;\n"
"varying highp vec3    vNormal;\n"
"varying mediump vec4  vColor;\n"
"uniform vec4 u_atlas_info;\n"
"\n"
"#define GLYPHY_TEXTURE1D_EXTRA_DECLS , sampler2D _tex, ivec4 _atlas_info, ivec2 _atlas_pos\n"
"#define GLYPHY_TEXTURE1D_EXTRA_ARGS , _tex, _atlas_info, _atlas_pos\n"
"#define GLYPHY_DEMO_EXTRA_ARGS , sTexture, uu_atlas_info, gi.atlas_pos\n"
"\n"
"vec4\n"
"glyphy_texture1D_func (int offset GLYPHY_TEXTURE1D_EXTRA_DECLS)\n"
"{\n"
"  ivec2 item_geom = _atlas_info.zw;\n"
"  vec2 pos = (vec2 (_atlas_pos.xy * item_geom +\n"
"                    ivec2 (mod (float (offset), float (item_geom.x)), offset / item_geom.x)) +\n"
"             + vec2 (.5, .5)) / vec2(_atlas_info.xy);\n"
"  return texture2D (_tex, pos);\n"
"}\n"
;

static const char* FRAGMENT_SHADER_MAIN =
"uniform float u_contrast;\n"
"uniform float u_gamma_adjust;\n"
"uniform float u_outline_thickness;\n"
"uniform float u_outline;\n"
"uniform float u_boldness;\n"
"\n"
"varying vec4 v_glyph;\n"
"\n"
"\n"
"#define SQRT2_2 0.70711 /* 1 / sqrt(2.) */\n"
"#define SQRT2   1.41421\n"
"\n"
"struct glyph_info_t {\n"
"  ivec2 nominal_size;\n"
"  ivec2 atlas_pos;\n"
"};\n"
"\n"
"glyph_info_t\n"
"glyph_info_decode (vec4 v)\n"
"{\n"
"  glyph_info_t gi;\n"
"  gi.nominal_size = (ivec2 (mod (v.zw, 256.)) + 2) / 4;\n"
"  gi.atlas_pos = ivec2 (v_glyph.zw) / 256;\n"
"  return gi;\n"
"}\n"
"\n"
"\n"
"float\n"
"antialias (float d)\n"
"{\n"
"  return smoothstep (-.75, +.75, d);\n"
"}\n"
"\n"
"vec4\n"
"source_over (const vec4 src, const vec4 dst)\n"
"{\n"
"  // http://dev.w3.org/fxtf/compositing-1/#porterduffcompositingoperators_srcover\n"
"  float alpha = src.a + (dst.a * (1. - src.a));\n"
"  return vec4 (((src.rgb * src.a) + (dst.rgb * dst.a * (1. - src.a))) / alpha, alpha);\n"
"}\n"
"\n"
"void\n"
"main()\n"
"{\n"
"  vec2 p = v_glyph.xy;\n"
"  glyph_info_t gi = glyph_info_decode (v_glyph);\n"
"\n"
"  /* isotropic antialiasing */\n"
"  vec2 dpdx = dFdx (p);\n"
"  vec2 dpdy = dFdy (p);\n"
"  float m = length (vec2 (length (dpdx), length (dpdy))) * SQRT2_2;\n"
"\n"
"  vec4 color = vec4( vColor.rgb * uColor.rgb, vColor.a * uColor.a );\n"
"\n"
"  ivec4 uu_atlas_info = ivec4( u_atlas_info );"
"  float gsdist = glyphy_sdf (p, gi.nominal_size GLYPHY_DEMO_EXTRA_ARGS);\n"
"  float sdist = gsdist / m * u_contrast;\n"
"\n"
"    sdist -= u_boldness * 10.;\n"
"    if ( glyphy_iszero( u_outline ) )\n"
"      sdist = abs (sdist) - u_outline_thickness * .5;\n"
"    if (sdist > 1.)\n"
"      discard;\n"
"    float alpha = antialias (-sdist);\n"
"    if (u_gamma_adjust != 1.)\n"
"      alpha = pow (alpha, 1./u_gamma_adjust);\n"
"    color = vec4 (color.rgb,color.a * alpha);\n"
"\n"
"  gl_FragColor = color;\n"
"}\n"
;

} // namespace

namespace Dali
{

namespace Toolkit
{

namespace Text
{

GlyphyShader::GlyphyShader()
{
}

GlyphyShader::GlyphyShader( Shader handle )
: Shader( handle )
{
}

GlyphyShader::~GlyphyShader()
{
}

GlyphyShader GlyphyShader::New( const Dali::Vector4& atlasInfo )
{
  std::ostringstream vertexShaderStringStream;
  std::ostringstream fragmentShaderStringStream;

  vertexShaderStringStream << ENABLE_EXTENSION_PREFIX << VERTEX_SHADER_MAIN;

  fragmentShaderStringStream << ENABLE_EXTENSION_PREFIX
                             << FRAGMENT_SHADER_PREFIX
                             << glyphy_common_glsl
                             << "#define GLYPHY_SDF_PSEUDO_DISTANCE 1\n"
                             << glyphy_sdf_glsl
                             << FRAGMENT_SHADER_MAIN;

  Shader shaderEffectCustom = Shader::New( vertexShaderStringStream.str(),
                                           fragmentShaderStringStream.str(),
                                           Shader::Hint::OUTPUT_IS_TRANSPARENT );

  GlyphyShader handle( shaderEffectCustom );

  handle.RegisterProperty( "u_atlas_info",  atlasInfo );
  handle.RegisterProperty( "u_contrast",          1.f );
  handle.RegisterProperty( "u_gamma_adjust",      1.f );
  handle.RegisterProperty( "u_outline_thickness", 1.f );
  handle.RegisterProperty( "u_outline",           1.f );
  handle.RegisterProperty( "u_boldness",          0.f );

  return handle;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
