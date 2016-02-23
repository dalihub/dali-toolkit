#ifndef __DALI_TOOLKIT_MIRROR_EFFECT_H__
#define __DALI_TOOLKIT_MIRROR_EFFECT_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/shader-effects/shader-effect.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Creates a new MirrorEffect
 *
 * MirrorEffect is a custom shader effect to achieve square effects in Image actors
 *
 * Animatable/Constrainable uniforms:
 *  "uDepth"  - The depth of the mirror effect. Default value 0.5
 *  "uAlpha"  - The alpha of the mirror effect. Default value 1.0
 *
 * @return A handle to a newly allocated ShaderEffect
 */
inline ShaderEffect CreateMirrorEffect()
{
  std::string vertexShader(
      "void main()                                  \n"
      "{                                            \n"
      "  mediump vec3 pos = vec3(aPosition, 0.0)*uSize;              \n"
      "  pos.y = pos.y * 3.0;                       \n"
      "  mediump vec4 world = uModelView * vec4(pos,1.0); \n"
      "  gl_Position = uProjection * world;         \n"
      "  vTexCoord = mix( uTextureRect.xy, uTextureRect.zw, aPosition + vec2(0.5) ); \n"
      "}                                            \n" );

  std::string fragmentShader(
      "uniform  mediump float  uDepth;              \n"
      "uniform  mediump float  uAlpha;              \n"
      "void main()                                  \n"
      "{                                            \n"
      " if(vTexCoord.y < 1.0 / 3.0)                 \n"
      " {                                           \n"
      "   gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);  \n"
      " }                                           \n"
      " else if(vTexCoord.y < 2.0 / 3.0)            \n"
      " {                                           \n"
      "   gl_FragColor = texture2D(sTexture, vec2(vTexCoord.x, vTexCoord.y * 3.0 - 1.0)) * uColor;    \n"
      "   gl_FragColor.a *= uAlpha;                 \n"
      " }                                           \n"
      " else                                        \n"
      " {                                           \n"
      "   highp float darkness = 3.0 - vTexCoord.y * 3.0;                                                   \n"
      "   darkness = (1.0 - 1.0 / uDepth + darkness * 1.0/ uDepth) * 0.65;                            \n"
      "   highp vec4 color = texture2D(sTexture, vec2(vTexCoord.x, -vTexCoord.y *3.0 + 3.0)) * uColor;      \n"
      "   color.a *= uAlpha;                                                                          \n"
      "   gl_FragColor = color * vec4(darkness, darkness, darkness, darkness);                        \n"
      " }                                           \n"
      "}                                            \n" );

  Dali::ShaderEffect shaderEffect =  Dali::ShaderEffect::New(
      vertexShader,
      fragmentShader,
      ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING ));

  shaderEffect.SetUniform("uAlpha", 1.0f);
  shaderEffect.SetUniform("uDepth", 0.5f);

  return shaderEffect;
}


} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_MIRROR_EFFECT_H__
