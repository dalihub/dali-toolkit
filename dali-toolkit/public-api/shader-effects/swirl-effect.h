#ifndef __DALI_TOOLKIT_SHADER_EFFECT_SWIRL_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_SWIRL_H__

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
 * @brief Creates a new SwirlEffect
 *
 * SwirlEffect is a custom shader effect to achieve swirl effects in Image actors.
 *
 * Animatable/Constrainable uniforms:
 *  "uAngle"  - The angle of the swirl
 *  "uCenter" - The center of the swirl
 *  "uRadius" - The radius of the swirl
 *
 * @param[in] warp True if the effect should warp
 * @return A handle to a newly allocated ShaderEffect
 */
inline ShaderEffect CreateSwirlEffect( bool warp )
{
  // append the default version
  std::string fragmentShader(
      "uniform mediump vec2  uTextureSize;\n"
      "uniform highp float uRadius;\n"
      "uniform highp float uAngle;\n"
      "uniform mediump vec2  uCenter;\n"
      "void main()\n"
      "{\n"
      "  highp vec2 textureCenter = (sTextureRect.xy + sTextureRect.zw) * 0.5;\n"
      "  textureCenter = vTexCoord.st - textureCenter;\n"
      "  highp float distance = length(textureCenter);\n"
      "  if (distance >= uRadius)\n"
      "     discard;\n"
      "  highp float percent = (uRadius - distance) / uRadius;\n"
      "  highp float theta = percent * percent * uAngle * 4.0;\n"
      "  highp float sinTheta = sin(theta);\n"
      "  highp float cosTheta = cos(theta);\n" );
  // if warp, loose the sign from sin
  if( warp )
  {
    fragmentShader.append(
        "  textureCenter = vec2( dot( textureCenter, vec2(cosTheta, sinTheta) ), "
        "                        dot( textureCenter, vec2(sinTheta, cosTheta) ) );\n" );
  }
  else
  {
    fragmentShader.append(
        "  textureCenter = vec2( dot( textureCenter, vec2(cosTheta, -sinTheta) ), "
        "                        dot( textureCenter, vec2(sinTheta, cosTheta) ) );\n" );
  }
  fragmentShader.append(
      "  textureCenter += uCenter;\n"
      "  gl_FragColor = texture2D( sTexture, textureCenter ) * uColor;\n"
      "}" );

  Dali::ShaderEffect shaderEffect =  Dali::ShaderEffect::New(
      "",
      fragmentShader,
      Dali::GeometryType( GEOMETRY_TYPE_IMAGE ),
      ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING | ShaderEffect::HINT_GRID ));

  shaderEffect.SetUniform( "uAngle", 0.0f );
  shaderEffect.SetUniform( "uCenter", Vector2(0.5f, 0.5f) );
  shaderEffect.SetUniform( "uRadius", 1.0f );

  return shaderEffect;
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_SWIRL_H__
