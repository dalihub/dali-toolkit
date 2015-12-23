#ifndef __DALI_TOOLKIT_SHADER_EFFECT_SPOT_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_SPOT_H__

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
 * @brief Creates a new SpotEffect
 *
 * SpotEffect is a custom shader effect to achieve spot effects on Image actors
 *
 * Animatable/Constrainable uniforms:
 *  "uCenter" - The center of the spot. Default value (0.0,0.0)
 *  "uRadius" - The radius of the spot. Default value 0.0
 *
 * @return A handle to a newly allocated ShaderEffect
 */
inline ShaderEffect CreateSpotEffect()
{
  // append the default version
  std::string vertexShader(
      "uniform mediump   vec2  uCenter;\n"
      "uniform mediump   float  uRadius;\n"
      "varying mediump   float  vRange;\n"
      "\n"
      "void main()\n"
      "{\n"
      "  mediump vec4 world = vec4(aPosition*uSize.xy, 0.0, 1.0);\n"
      "  \n"
      "  mediump vec2 d = vec2(world.xy - uCenter);\n"
      "  mediump float dist = length(d);\n"
      "  \n"
      "  mediump float range = (uRadius - dist) / (uRadius);\n"
      "  vRange = max(0.1, range);\n"
      "  \n"
      "  gl_Position = uMvpMatrix * world;\n"
      "  vTexCoord = mix( uTextureRect.xy, uTextureRect.zw, aPosition + vec2(0.5) );\n;\n"
      "}");

  std::string fragmentShader(
      "varying mediump   float  vRange;\n"
      "\n"
      "void main()\n"
      "{\n"
      "  gl_FragColor = texture2D(sTexture, vTexCoord) * vec4(vRange, vRange, vRange, 1.0) * uColor;\n"
      "}" );

  Dali::ShaderEffect shaderEffect =  Dali::ShaderEffect::New(
      vertexShader, fragmentShader,
      ShaderEffect::GeometryHints( ShaderEffect::HINT_GRID ));

  shaderEffect.SetUniform( "uCenter", Vector2(0.0f, 0.0f) );
  shaderEffect.SetUniform( "uRadius", 0.0f );

  return shaderEffect;
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_SPOT_H__
