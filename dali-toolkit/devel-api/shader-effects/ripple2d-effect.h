#ifndef __DALI_TOOLKIT_SHADER_EFFECT_RIPPLE2D_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_RIPPLE2D_H__

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
 * @brief Creates a new Ripple2DEffect
 *
 * Ripple2DEffect is a custom shader effect to achieve 2d ripple effects on Image actors.
 *
 * Animatable/Constrainable uniforms:
 *  "uTime"       - The time duration for the 2d ripple
 *  "uAmplitude"  - The amplitude of the 2d ripple
 *
 * @return A handle to a newly allocated ShaderEffect
 */
inline ShaderEffect CreateRipple2DEffect()
{
  // append the default version
  std::string fragmentShader(
      "precision mediump float;\n"
      "uniform float uAmplitude;\n"
      "uniform float uTime;\n"
      "void main()\n"
      "{\n"
      "  highp vec2 textureSize = uTextureRect.zw - uTextureRect.xy;\n"
      "  highp vec2 pos = -1.0 + 2.0 * vTexCoord.st/textureSize;\n"
      "  highp float len = length(pos);\n"
      "  highp vec2 texCoord = vTexCoord.st/textureSize + pos/len * sin( len * 12.0 - uTime * 4.0 ) * uAmplitude;\n"
      "  gl_FragColor = texture2D(sTexture, texCoord) * uColor;\n"
      "}" );

  Dali::ShaderEffect shaderEffect =  Dali::ShaderEffect::New(
      "", fragmentShader,
      ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING | ShaderEffect::HINT_GRID ));



  shaderEffect.SetUniform( "uTextureSize", Vector2(0.0f, 0.0f) ); //@note: Is this needed?
  shaderEffect.SetUniform( "uAmplitude", 0.0f );
  shaderEffect.SetUniform( "uTime", 0.0f );

  return shaderEffect;
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_RIPPLE2D_H__
