#ifndef __DALI_TOOLKIT_IMAGE_REGION_EFFECT_H__
#define __DALI_TOOLKIT_IMAGE_REGION_EFFECT_H__

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
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/visual-properties.h>

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
inline Property::Map CreateImageRegionEffect()
{
  std::string vertexShader(
      "attribute mediump vec2 aPosition;\n"
      "\n"
      "uniform mediump mat4 uMvpMatrix;\n"
      "uniform vec3 uSize;\n"
      "uniform vec4 uTextureRect;"
      "\n"
      "varying vec2 vTexCoord;\n"

      "uniform mediump vec2 uTopLeft;\n"
      "uniform mediump vec2 uBottomRight;\n"
      "void main()\n"
      "{\n"
      "  mediump vec4 position = vec4(aPosition, 0.0, 1.0);\n"
      "  position.xyz *= uSize;\n"
      "  gl_Position = uMvpMatrix * position;\n"
      // The line below is doing the same as the following commented lines:
      //"  vec2 imageSize = uTextureRect.zw - uTextureRect.xy;\n"
      //"  vec2 topLeft = uTextureRect.xy + uTopLeft * imageSize;\n"
      //"  vec2 bottomRight = uTextureRect.xy + uBottomRight * imageSize;\n"
      //"  vec2 texCoord = (aTexCoord - uTextureRect.xy) / imageSize;\n"
      //"  vTexCoord = topLeft + texCoord * ( bottomRight - topLeft );\n"

      "  vec2 texCoord = aPosition + vec2(0.5);\n"
      "  vTexCoord = uTextureRect.xy + uTopLeft * ( uTextureRect.zw - uTextureRect.xy ) + ( texCoord - uTextureRect.xy ) * ( uBottomRight - uTopLeft );\n"
      "}\n"
  );

  Property::Map map;

  Property::Map customShader;
  customShader[ Visual::Shader::Property::VERTEX_SHADER ] = vertexShader;

  map[ Visual::Property::SHADER ] = customShader;
  return map;
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_IMAGE_REGION_EFFECT_H__
