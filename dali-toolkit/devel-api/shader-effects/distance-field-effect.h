#ifndef __DALI_TOOLKIT_SHADER_EFFECT_DISTANCEFIELD_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_DISTANCEFIELD_H__

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
 * Creates a new DistanceFieldEffect
 *
 * DistanceFieldEffect is a custom shader effect to achieve distance field on Image actors
 *
 * Animatable/Constrainable uniforms:
 *
 *  "uSmoothing"    - Soft edge smoothing. Specify the distance field value for the center of the edge.

 *  "uDoGlow"       - The glow state. If true, glow is enabled
 *  "uGlowBoundary" - The glow boundary factor
 *  "uGlowColor"    - The glow color multiplier

 *  "uDoShadow"     - The shadow state. If true, shadows is enabled. Cannot be used with glow/and or outline
 *  "uShadowColor"  - The shadow color multiplier
 *  "uShadowOffset" - The shadow offset

 *  "uDoOutline"    - The outline state. If true, outline is enabled
 *  "uOutlineColor" - The outline color multiplier
 *  "uOutlineParams"- Thickness of outline. The outline thickness is determined by two values.
 *                    First value [0-1] Specifies the distance field value for the center of the outline.
 *                    Second value [0-1] Specifies the softness/width/anti-aliasing of the outlines inner edge.
 *
 * @return A handle to a newly allocated ShaderEffect
 */
inline ShaderEffect CreateDistanceFieldEffect()
{
  std::string fragmentShaderPrefix(
      "#extension GL_OES_standard_derivatives : enable\n"
      "\n"
  );

  std::string fragmentShader(
      "uniform mediump float uSmoothing;\n"
      "uniform mediump float uGlowBoundary;\n"
      "uniform mediump vec2  uOutlineParams;\n"
      "uniform lowp    vec4  uOutlineColor;\n"
      "uniform lowp    vec4  uShadowColor;\n"
      "uniform mediump vec2  uShadowOffset;\n"
      "uniform lowp    vec4  uGlowColor;\n"
      "uniform lowp    float uDoOutline;\n"
      "uniform lowp    float uDoShadow;\n"
      "uniform lowp    float uDoGlow;\n"
      "\n"
      "void main()\n"
      "{\n"
      "  // sample distance field\n"
      "  mediump float distance = texture2D(sTexture, vTexCoord).a;\n"
      "  mediump float smoothWidth = fwidth(distance);\n"
      "  mediump float alphaFactor = smoothstep(uSmoothing - smoothWidth, uSmoothing + smoothWidth, distance);\n"
      "  lowp    vec4  color;\n"
      "  if (uDoShadow == 0.0)\n"
      "  {\n"
      "    mediump float alpha = uColor.a * alphaFactor;\n"
      "    lowp    vec4  rgb = uColor;\n"
      "\n"
      "    if (uDoOutline > 0.0)\n"
      "    {\n"
      "      mediump float outlineWidth = uOutlineParams[1] + smoothWidth;\n"
      "      mediump float outlineBlend = smoothstep(uOutlineParams[0] - outlineWidth, uOutlineParams[0] + outlineWidth, distance);\n"
      "      alpha = smoothstep(uSmoothing - smoothWidth, uSmoothing + smoothWidth, distance);\n"
      "      rgb = mix(uOutlineColor, uColor, outlineBlend);\n"
      "    }\n"
      "\n"
      "    if (uDoGlow > 0.0)\n"
      "    {\n"
      "      rgb = mix(uGlowColor, rgb, alphaFactor);\n"
      "      alpha = smoothstep(uGlowBoundary, uSmoothing, distance);\n"
      "    }\n"
      "\n"
      "    // set fragment color\n"
      "    color = vec4(rgb.rgb, alpha);\n"
      "  }\n"
      "\n"
      "  else // (uDoShadow > 0.0)\n"
      "  {\n"
      "    mediump float shadowDistance = texture2D(sTexture, vTexCoord - uShadowOffset).a;\n"
      "    mediump float inText = alphaFactor;\n"
      "    mediump float inShadow = smoothstep(uSmoothing - smoothWidth, uSmoothing + smoothWidth, shadowDistance);\n"
      "\n"
      "    // inside object, outside shadow\n"
      "    if (inText == 1.0)\n"
      "    {\n"
      "      color = uColor;\n"
      "    }\n"
      "    // inside object, outside shadow\n"
      "    else if ((inText != 0.0) && (inShadow == 0.0))\n"
      "    {\n"
      "      color = uColor;\n"
      "      color.a *= inText;\n"
      "    }\n"
      "    // outside object, completely inside shadow\n"
      "    else if ((inText == 0.0) && (inShadow == 1.0))\n"
      "    {\n"
      "      color = uShadowColor;\n"
      "    }\n"
      "    // inside object, completely inside shadow\n"
      "    else if ((inText != 0.0) && (inShadow == 1.0))\n"
      "    {\n"
      "      color = mix(uShadowColor, uColor, inText);\n"
      "      color.a = uShadowColor.a;\n"
      "    }\n"
      "    // inside object, inside shadow's border\n"
      "    else if ((inText != 0.0) && (inShadow != 0.0))\n"
      "    {\n"
      "      color = mix(uShadowColor, uColor, inText);\n"
      "      color.a *= max(inText, inShadow);\n"
      "    }\n"
      "    // inside shadow's border\n"
      "    else if (inShadow != 0.0)\n"
      "    {\n"
      "      color = uShadowColor;\n"
      "      color.a *= inShadow;\n"
      "    }\n"
      "    // outside shadow and object\n"
      "    else \n"
      "    {\n"
      "      color.a = 0.0;\n"
      "    }\n"
      "\n"
      "  }\n"
      "\n"
      "  gl_FragColor = color;\n"
      "\n"
      "}\n"
  );

  // Create the implementation, temporarily owned on stack
  Dali::ShaderEffect shaderEffect =  Dali::ShaderEffect::NewWithPrefix(
      "", "",
      fragmentShaderPrefix, fragmentShader,
      GeometryType( GEOMETRY_TYPE_IMAGE ),
      ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING));

  shaderEffect.SetUniform("uSmoothing",0.5f);
  shaderEffect.SetUniform("uOutlineColor",Color::BLACK);
  shaderEffect.SetUniform("uOutlineParams",Vector2(0.51f, 0.0f));
  shaderEffect.SetUniform("uGlowBoundary",0.4f);
  shaderEffect.SetUniform("uGlowColor",Color::GREEN);
  shaderEffect.SetUniform("uShadowColor",Vector4(0.0f, 0.0f, 0.0f, 0.4f));

  // TODO: find a way to set the shadow offset in texel coordinates instead of UVs.
  shaderEffect.SetUniform("uShadowOffset",Vector2(0.05f, 0.05f));

  // Default:
  shaderEffect.SetUniform("uDoOutline",false);
  shaderEffect.SetUniform("uDoGlow",false);
  shaderEffect.SetUniform("uDoShadow",false);

  return shaderEffect;
}


} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_SPOT_H__
