#ifndef __DALI_TOOLKIT_SHADER_EFFECT_BENDY_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_BENDY_H__

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
#include <dali/devel-api/shader-effects/shader-effect.h>

namespace Dali
{

namespace Toolkit
{

/**
 * Creates a new Bendy effect
 *
 * BendyEffect is a custom shader effect to achieve bendy effects in Image actors
 *
 * Animatable/Constrainable uniforms:
 *  "uCenter"    - The center point of the bendy effect
 *  "uDirection" - The direction of the bendy effect
 *  "uRadius"    - The radius of the bendy effect
 *
 * @return A handle to a newly allocated ShaderEffect.
 */
inline ShaderEffect CreateBendyEffect()
{
  // append the default version
  std::string vertextShader(
      "uniform mediump   vec2  uCenter;\n"
      "uniform mediump   vec2  uDirection;\n"
      "uniform mediump   float uRadius;\n"
      "\n"
      "varying mediump   float vShade;\n"
      "\n"
      "void main()\n"
      "{\n"
      " mediump float lighting = 0.25;\n"
      " mediump vec4 position = uModelView * vec4(aPosition, 1.0);\n"
      "\n"
      " mediump vec2 d = position.xy - uCenter;\n"
      " mediump float dist = max( 0.0, dot(d,uDirection) );\n"
      " mediump float radius = max(0.0, uRadius - dist * 0.01);\n"
      "\n"
      " mediump float cs = cos(dist / radius / 2.0);\n"
      " mediump float sn = sin(dist / radius / 2.0);\n"
      "\n"
      "position.xy = position.xy - uDirection * dist;\n"
      "\n"
      "position.xy += uDirection * sn * radius;\n"
      "position.z += (1.0 - cs) * radius;\n"
      "\n"
      "gl_Position = uProjection * position;\n"
      "\n"
      "vShade = 1.0 - abs(sn) * lighting;\n"
      "\n"
      "vTexCoord = mix( sTextureRect.xy, sTextureRect.zw, aTexCoord );\n"
      "}" );

  std::string fragmentShader(
      "varying mediump float  vShade;\n"
      "\n"
      "void main()\n"
      "{\n"
      "  gl_FragColor = texture2D(sTexture, vTexCoord) * uColor * vec4(vShade,vShade,vShade,1.0);\n"
      "}" );

  // Create the implementation, temporarily owned on stack,
  Dali::ShaderEffect shaderEffect =  Dali::ShaderEffect::New(
      vertextShader,
      fragmentShader,
      ShaderEffect::GeometryHints( ShaderEffect::HINT_GRID | ShaderEffect::HINT_DEPTH_BUFFER ));

  shaderEffect.SetUniform( "uCenter", Vector2(0.0f, 0.0f), ShaderEffect::COORDINATE_TYPE_VIEWPORT_POSITION );
  shaderEffect.SetUniform( "uDirection", Vector2(0.0f, 0.0f), ShaderEffect::COORDINATE_TYPE_VIEWPORT_DIRECTION );
  shaderEffect.SetUniform( "uRadius", 0.0f );


  return shaderEffect;
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_BENDY_H__
