#ifndef __DALI_PAGE_TURN_BOOK_SPINE_EFFECT_H__
#define __DALI_PAGE_TURN_BOOK_SPINE_EFFECT_H__

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

#define DALI_COMPOSE_SHADER(STR) #STR

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * @brief Creates a new PageTurnBookSpineEffect
 * This is an assisting effect of PageTurnEffect to display a book spine on _static_ pages, and also to flip the image horizontally when needed.
 *
 * When the page is turned over in landscape, call
 * SetIsBackImageVisible(true), this effect can display the back image
 * correctly after the page been rotated 180 degrees.  To
 * display the pages visually consistent with its turning state,
 * please set the uniforms with the same values as the PageTurnEffect.
 *
 * Animatable/Constrainable uniforms:
 *  "uSpineShadowParameter" - The two parameters are the major&minor radius (in pixels) to form an ellipse shape. The top-left
 *                            quarter of this ellipse is used to calculate spine normal for simulating shadow *
 *  "uTextureWidth" - 1.0 for single sided page,
 *                    2.0 for double sided image which has left half part as page front side and right half part as page back side.
 *
 * @return The newly created Property::Map with the page turn book spine effect
 **/
inline Property::Map CreatePageTurnBookSpineEffect()
{
  const char* vertexSource = DALI_COMPOSE_SHADER(
      precision mediump float;\n
      attribute mediump vec2 aPosition;\n
      uniform mediump mat4 uMvpMatrix;\n
      uniform vec3 uSize;\n
      uniform float uTextureWidth;\n
      varying vec2 vTexCoord;\n
      void main()\n
      {\n
        mediump vec4 vertexPosition = vec4(aPosition*uSize.xy, 0.0, 1.0);\n
        gl_Position = uMvpMatrix * vertexPosition;\n
        vTexCoord = aPosition + vec2(0.5);\n
        vTexCoord.x /= uTextureWidth;
      }\n);

  // the simplified version of the fragment shader of page turn effect
  const char* fragmentSource = DALI_COMPOSE_SHADER(
      precision mediump float;\n
      varying mediump vec2 vTexCoord;\n
      uniform vec3 uSize;\n
      uniform vec2 uSpineShadowParameter;\n
      uniform sampler2D sTexture;\n
      uniform lowp vec4 uColor;\n

      void main()\n
      {\n
        if( gl_FrontFacing )\n // display front side
        {\n
          gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
        }\n
        else\n // display back side, flip the image horizontally by changing the x component of the texture coordinate
        {\n
          gl_FragColor = texture2D( sTexture, vec2( 1.0 - vTexCoord.x, vTexCoord.y ) ) * uColor;\n
        }\n
        // display book spine, a stripe of shadowed texture
        float pixelPos = vTexCoord.x * uSize.x;\n
        if( pixelPos < uSpineShadowParameter.x )\n
        {\n
          float x = pixelPos - uSpineShadowParameter.x;\n
          float y = sqrt( uSpineShadowParameter.x*uSpineShadowParameter.x - x*x );\n
          vec2 spineNormal = normalize(vec2(uSpineShadowParameter.y*x/uSpineShadowParameter.x, y));\n
          gl_FragColor.rgb *= spineNormal.y; \n
        }\n
      } );

  Property::Map map;

  Property::Map customShader;

  customShader[ "vertexShader" ] = vertexSource;
  customShader[ "fragmentShader" ] = fragmentSource;

  map[ "shader" ] = customShader;
  return map;
}

} //namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_PAGE_TURN_BOOK_SPINE_EFFECT_H__ */
