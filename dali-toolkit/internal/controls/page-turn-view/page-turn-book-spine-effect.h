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

// EXTERNAL INCLUDES
#include <dali/public-api/shader-effects/shader-effect.h>

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
 * correctly after the imageActor been rotated 180 degrees.  To
 * display the pages visually consistent with its turning state,
 * please set the uniforms with the same values as the PageTurnEffect.
 *
 * Animatable/Constrainable uniforms:
 *  "uShadowWidth"          - The width of shadow to be pageSize * shadowWidth. This shadow appears at the edges of the actor
 *                            which is not visible on static pages
 *  "uSpineShadowParameter" - The two parameters are the major&minor radius (in pixels) to form an ellipse shape. The top-left
 *                            quarter of this ellipse is used to calculate spine normal for simulating shadow
 *  "uIsBackImageVisible"   - Set whether the current page is with its backside visible. Need to pass the parameter as true for
 *                            the page which is turned over but still visible in Landscape
 *  "uPageWidth"            - The page width of the PageTurnBookSpineEffect
 *
 * @return A handle to a newly allocated ShaderEffect
 **/
inline ShaderEffect CreatePageTurnBookSpineEffect()
{
  std::string vertexSource(
      "precision mediump float;\n"
      "uniform float uShadowWidth;\n"
      "  void main()\n"
      "  {\n"
      "    gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n"
      "    vTexCoord.x = (aTexCoord.x-sTextureRect.s) /( 1.0 - uShadowWidth ) + sTextureRect.s;\n"
      "    vTexCoord.y = ( aTexCoord.y-sTextureRect.t-0.5*uShadowWidth*(sTextureRect.q-sTextureRect.t) )/( 1.0 - uShadowWidth ) + sTextureRect.t;\n"
      "  }");

  // the simplified version of the fragment shader of page turn effect
  std::string fragmentSource(
      "precision mediump float;\n"
      "uniform float uIsBackImageVisible;\n"
      "uniform float uPageWidth;\n"
      "uniform vec2 uSpineShadowParameter;\n"
      "  void main()\n"
      "  {\n"
      // leave the border for display shadow, not visible( out of the screen ) when the page is static
      "    if( vTexCoord.y > sTextureRect.q || vTexCoord.y < sTextureRect.t || vTexCoord.x > sTextureRect.p  )\n"
      "    {\n"
      "      gl_FragColor = vec4( 0.0 );\n"
      "    }\n"
      "    else \n"
      "    { \n"
      // flip the image horizontally by changing the x component of the texture coordinate
      "      if( uIsBackImageVisible == 1.0 )  gl_FragColor = texture2D( sTexture, vec2( sTextureRect.p+sTextureRect.s-vTexCoord.x, vTexCoord.y ) ) * uColor; \n"
      "      else gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n"
      "      \n"
      // display book spine, a stripe of shadowed texture
      "      float pixelPos = (vTexCoord.x-sTextureRect.s)*uPageWidth; \n"
      "      if(pixelPos < uSpineShadowParameter.x) \n"
      "      {\n"
      "        float x = pixelPos - uSpineShadowParameter.x;\n"
      "        float y = sqrt( uSpineShadowParameter.x*uSpineShadowParameter.x - x*x );\n"
      "        vec2 spineNormal = normalize(vec2(uSpineShadowParameter.y*x/uSpineShadowParameter.x, y));\n"
      "        gl_FragColor.rgb *= spineNormal.y; \n"
      "      }"
      "    }\n"
      "  }" );

  const float DEFAULT_SHADOW_WIDTH(0.15f);
  const Vector2 DEFAULT_SPINE_SHADOW_PARAMETER(50.0f, 20.0f);

  ShaderEffect shaderEffect = ShaderEffect::New( vertexSource, fragmentSource );

  shaderEffect.SetUniform( "uIsBackImageVisible", -1.f );
  shaderEffect.SetUniform( "uShadowWidth", DEFAULT_SHADOW_WIDTH );
  shaderEffect.SetUniform( "uSpineShadowParameter", DEFAULT_SPINE_SHADOW_PARAMETER );

  float defaultPageWidth = Dali::Stage::GetCurrent().GetSize().x;
  shaderEffect.SetUniform( "uPageWidth", defaultPageWidth/(1.f-DEFAULT_SHADOW_WIDTH) );

  return shaderEffect;
}

} //namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_PAGE_TURN_BOOK_SPINE_EFFECT_H__ */
