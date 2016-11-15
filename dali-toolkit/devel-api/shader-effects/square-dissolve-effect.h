#ifndef __DALI_TOOLKIT_SHADER_EFFECT_SQUARE_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_SQUARE_H__

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
 * @brief Creates a new SquareDissolveEffect
 *
 * SquareDissolveEffect is a custom shader effect to achieve square effects in Image actors
 *
 * Animatable/Constrainable uniforms:
 *  "uStep"       - The step of the square effect
 *  "uRows"       - The rows of the square dissolve effect
 *  "uColumns"    - The columns of the square dissolve effect
 *  "uTextureSize"- The texture size of the square dissolve
 *
 * @return A handle to a newly allocated ShaderEffect
 */
inline ShaderEffect CreateSquareDissolveEffect()
{
  // variable "uStep" range scope : [0.0, 1.0]
  std::string fragmentShader(
      "uniform  mediump vec2   uTextureSize;\n"
      "uniform  mediump float  uStep;\n"
      "uniform  mediump float  uRows;\n"
      "uniform  mediump float  uColumns;\n"
      "void main()\n"
      "{\n"
      "  mediump vec2 mosaicSize = vec2(1.0 / uRows, 1.0 / uColumns);                               \n"
      "  mediump vec2 intXY = vec2(vTexCoord.x * uTextureSize.x, vTexCoord.y * uTextureSize.y);               \n"
      "  mediump vec2 XYMosaic = vec2(floor(intXY.x / mosaicSize.x) * mosaicSize.x, floor(intXY.y / mosaicSize.y) * mosaicSize.y); \n"
      "  mediump vec2 UVMosaic = vec2(XYMosaic.x /uTextureSize.x, XYMosaic.y / uTextureSize.y);               \n"
      "  mediump vec4 noiseVec = texture2D(sEffect, UVMosaic);                                      \n"
      "  mediump float intensity = (noiseVec[0] + noiseVec[1] + noiseVec[2] + noiseVec[3]) / 4.0;   \n"
      "  if(intensity < uStep)                                                              \n"
      "    gl_FragColor = vec4(0.1, 0.1, 0.1, 1.0);                                         \n"
      "  else                                                                               \n"
      "    gl_FragColor = texture2D(sTexture, vTexCoord);                                   \n"
      "  gl_FragColor *= uColor;                                                            \n"
      "}                                                                                    \n" );

  Dali::ShaderEffect shaderEffect =  Dali::ShaderEffect::New(
      "",
      fragmentShader,
      ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING | ShaderEffect::HINT_GRID ));

  shaderEffect.SetUniform( "uTextureSize", Vector2(1.0f, 1.0f) );//COORDINATE_TYPE_DEFAULT
  shaderEffect.SetUniform( "uStep", 0.1f);
  shaderEffect.SetUniform( "uRows", 25.0f);
  shaderEffect.SetUniform( "uColumns", 25.0f);

  return shaderEffect;
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_SQUARE_H__
