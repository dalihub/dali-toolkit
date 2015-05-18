#ifndef __DALI_TOOLKIT_SHADER_EFFECT_BLIND_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_BLIND_H__

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
 * @brief BlindEffect is a custom shader effect to achieve blind effects in Image actors
 *
 * Animatable/Constrainable uniforms:
 *  "uStep" - The step of the blind effect.
 *
 * @return A handle to a newly allocated ShaderEffect
 */

inline ShaderEffect CreateBlindEffect()
{
  std::string fragmentShader(
      "uniform mediump float uStep;                                                        \n"
      "void main()                                                                         \n"
      "{                                                                                   \n"
      "    mediump vec4 alphaColor;                                                        \n"
      "    mediump vec4 baseColor;                                                         \n"
      "    baseColor = texture2D( sTexture, vTexCoord);                                    \n"
      "    alphaColor = vec4(0.1,0.1,0.1,1.0);                                             \n"
      "    lowp float index = 0.0;                                                         \n"
      "    index = floor(vTexCoord.y/0.1);                                                 \n"
      "    if((vTexCoord.y < (index * 0.1 + uStep * 0.005)) && (vTexCoord.y > index * 0.1))\n"
      "    {                                                                               \n"
      "      gl_FragColor = alphaColor;                                                    \n"
      "    }                                                                               \n"
      "    else                                                                            \n"
      "    {                                                                               \n"
      "      gl_FragColor = baseColor;                                                     \n"
      "    }                                                                               \n"
      "    gl_FragColor*=uColor;                                                           \n"
      "}                                                                                   \n"
  );

  Dali::ShaderEffect shaderEffect =  Dali::ShaderEffect::New(
      "",
      fragmentShader,
      GeometryType( GEOMETRY_TYPE_IMAGE ),
      ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING | ShaderEffect::HINT_GRID ));

  shaderEffect.SetUniform( "uStep", 0.0f );

  return shaderEffect;
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_BLIND_H__
