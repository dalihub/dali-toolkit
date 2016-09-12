#ifndef __DALI_TOOLKIT_SHADER_EFFECT_IRIS_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_IRIS_H__

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
 * @brief Creates a new IrisEffect
 *
 * IrisEffect is a custom shader effect to achieve iris effects in Image actors
 *
 * Animatable/Constrainable uniforms:
 *
 *  "uRadius"       - The radius of the iris effect in texture coordinate distance,
 *                    i.e. 0.0 (no circle) to 1.0 (complete circle), to > 1.0 (extending outside of texture).
 *                    @note For Atlas Textures results may be unpredictable.
 *
 *  "uBlendFactor"  - The blend factor of the iris effect. The lower the value, the larger the blending portion
 *                    (between Opaque & Transparent). Blending will account for 1 / blendFactor of the radius
 *                    of the texture.
 *
 *  "uCenter"       - The center point of the iris (in texture coordinates)
 *
 * @return A handle to a newly allocated ShaderEffect
 */
inline ShaderEffect CreateIrisEffect()
{
  // append the default version
  std::string vertexShader(
      "uniform mediump vec2 uCenter;\n"
      "varying mediump vec2 vRelativePosition;\n"
      "\n"
      "void main()\n"
      "{\n"
      "    mediump vec4 world = uModelView * vec4(aPosition, 1.0);\n"
      "    gl_Position = uProjection * world;\n"
      "    \n"
      "    vTexCoord = mix( sTextureRect.xy, sTextureRect.zw, aTexCoord );\n"
      "    vRelativePosition = vTexCoord - uCenter;\n"
      "}\n");

  std::string fragmentShader(
      "uniform mediump float uRadius;                                                           \n"
      "uniform mediump float uBlendFactor;                                                      \n"
      "varying mediump vec2 vRelativePosition;                                                  \n"
      "void main()                                                                      \n"
      "{                                                                                \n"
      "   mediump float delta = (length(vRelativePosition) - uRadius);                          \n"
      "   delta = clamp(0.0 - delta * uBlendFactor, 0.0, 1.0);                          \n"
      "   gl_FragColor = texture2D(sTexture, vTexCoord) * uColor;                       \n"
      "   gl_FragColor.a *= delta;                                                      \n"
      "}                                                                                \n"
  );

  Dali::ShaderEffect shaderEffect =  Dali::ShaderEffect::New(
      vertexShader,
      fragmentShader,
      ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING ));

  shaderEffect.SetUniform( "uRadius", 0.0f );
  shaderEffect.SetUniform( "uBlendFactor", 100.0f );
  shaderEffect.SetUniform( "uCenter", Vector2(0.5f, 0.5f) );

  return shaderEffect;
}

}
}

#endif
