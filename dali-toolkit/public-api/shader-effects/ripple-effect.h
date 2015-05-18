#ifndef __DALI_TOOLKIT_SHADER_EFFECT_RIPPLE_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_RIPPLE_H__

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
 * @brief Creates a new RippleEffect
 *
 * RippleEffect is a custom shader effect to achieve ripple effects on Image actors.
 *
 * Animatable/Constrainable uniforms:
 *  "uAmplitude"  - The amplitude of the effect
 *  "uCenter"     - The center point of the effect as screen coordinates
 *  "uTime"       - The time duration for the ripple
 *
 * @return A handle to a newly allocated ShaderEffect
 */
inline ShaderEffect CreateRippleEffect()
{
  std::string vertexShader(
      "precision mediump float;\n"
      "uniform mediump   vec2  uCenter;\n"
      "uniform mediump   float uTime;\n"
      "uniform mediump   float uAmplitude;\n"
      "varying mediump   float vLight;\n"
      "varying mediump   float vShade;\n"
      "void main()\n"
      "{\n"
      "float lighting = uAmplitude * 0.02;\n"
      "float waveLength = uAmplitude * 0.0016;\n"
      "vec4 world = uModelView * vec4(aPosition,1.0);\n"
      "vec2 d = vec2(world.x - uCenter.x, world.y - uCenter.y);\n"
      "float dist = length(d);\n"
      "float amplitude = cos(uTime - dist*waveLength);\n"
      "float slope     = sin(uTime - dist*waveLength);\n"
      "world.z += amplitude * uAmplitude;\n"
      "gl_Position = uProjection * world;\n"
      "vec2 lightDirection = vec2(-0.707,0.707);\n"
      "float dot = 0.0;\n"
      "if(dist > 0.0)\n"
      "{\n"
      "  dot = dot(normalize(d),lightDirection) * lighting;\n"
      "}\n"
      "vShade = 1.0 - (dot * slope);\n"
      "vLight = max(0.0, dot * -slope);\n"
      "vTexCoord = aTexCoord;\n"
      "}" );

  // append the default version
  std::string imageFragmentShader(
      "precision mediump float;\n"
      "varying mediump float  vLight;\n"
      "varying mediump float  vShade;\n"
      "void main()\n"
      "{\n"
      "  gl_FragColor = texture2D(sTexture, vTexCoord) * uColor * vec4(vShade,vShade,vShade,1.0) + vec4(vLight, vLight, vLight,0.0);\n"
      "}" );


  Dali::ShaderEffect shaderEffect =  Dali::ShaderEffect::New(
      vertexShader, imageFragmentShader,
      GeometryType(GEOMETRY_TYPE_IMAGE), ShaderEffect::GeometryHints(ShaderEffect::HINT_GRID) );


  shaderEffect.SetUniform( "uAmplitude", 0.0f );
  shaderEffect.SetUniform( "uCenter", Vector2(0.0f, 0.0f));
  shaderEffect.SetUniform( "uTime", 0.0f );

  return shaderEffect;
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_RIPPLE_H__
