#ifndef __DALI_TOOLKIT_IMAGE_REGION_EFFECT_H__
#define __DALI_TOOLKIT_IMAGE_REGION_EFFECT_H__

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
 * @brief Creates a new ImageRegionEffect
 *
 * ImageRegionEffect is a custom shader effect to show only a region of an Image actor.
 *
 * Animatable/Constrainable uniforms:
 *  "uTopLeft"      - The top-left corner of the image region. The coordinates are in percentage,
 *                    (0,0) being the top-left and (1,1) the bottom right of the original image
 *  "uBottomRight"  - The bottom-right corner of the image region. The coordinates are in percentage,
 *                    (0,0) being the top-left and (1,1) the bottom right of the original image
 *
 * @return A handle to a newly allocated ShaderEffect
 */
inline ShaderEffect CreateImageRegionEffect()
{
  std::string vertexShader(
      "uniform mediump vec2 uTopLeft;\n"
      "uniform mediump vec2 uBottomRight;\n"
      "void main()\n"
      "{\n"
      "  mediump vec4 position = vec4(aPosition,1.0);\n"
      "  gl_Position = uMvpMatrix * position;\n"
      // The line below is doing the same as the following commented lines:
      //"  vec2 imageSize = sTextureRect.zw - sTextureRect.xy;\n"
      //"  vec2 topLeft = sTextureRect.xy + uTopLeft * imageSize;\n"
      //"  vec2 bottomRight = sTextureRect.xy + uBottomRight * imageSize;\n"
      //"  vec2 texCoord = (aTexCoord - sTextureRect.xy) / imageSize;\n"
      //"  vTexCoord = topLeft + texCoord * ( bottomRight - topLeft );\n"
      "  vTexCoord = sTextureRect.xy + uTopLeft * ( sTextureRect.zw - sTextureRect.xy ) + ( aTexCoord - sTextureRect.xy ) * ( uBottomRight - uTopLeft );\n"
      "}\n"
  );

  Dali::ShaderEffect shaderEffect = Dali::ShaderEffect::New( vertexShader, "" );
  shaderEffect.SetUniform( "uTopLeft", Vector2( 0.f, 0.f ) );
  shaderEffect.SetUniform( "uBottomRight", Vector2( 1.f, 1.f ) );

  return shaderEffect;
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_IMAGE_REGION_EFFECT_H__
