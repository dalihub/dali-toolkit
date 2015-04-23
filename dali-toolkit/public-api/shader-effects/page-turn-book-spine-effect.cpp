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

// CLASS HEADER
#include <dali-toolkit/public-api/shader-effects/page-turn-book-spine-effect.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/stage.h>

namespace Dali
{

namespace Toolkit
{

namespace
{

const std::string SHADOW_WIDTH_PROPERTY_NAME("uShadowWidth");
const std::string SPINE_SHADOW_PARAMETER_PROPERTY_NAME("uSpineShadowParameter");
const std::string IS_BACK_IMAGE_VISIBLE_PROPERTY_NAME( "uIsBackImageVisible" );
const std::string PAGE_WIDTH_PROPERTY_NAME( "uPageWidth" );

// fake shadow is used to enhance the effect, with its default maximum width to be pageSize * 0.15
const float DEFAULT_SHADOW_WIDTH(0.15f);

// the major&minor radius (in pixels) to form an ellipse shape
// the top-left quarter of this ellipse is used to calculate spine normal for simulating shadow
const Vector2 DEFAULT_SPINE_SHADOW_PARAMETER(50.0f, 20.0f);

}

PageTurnBookSpineEffect::PageTurnBookSpineEffect()
{
}

PageTurnBookSpineEffect::PageTurnBookSpineEffect( ShaderEffect handle )
: ShaderEffect( handle )
{
}

PageTurnBookSpineEffect::~PageTurnBookSpineEffect()
{
}

PageTurnBookSpineEffect PageTurnBookSpineEffect::New()
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

  ShaderEffect shader;
  shader = ShaderEffect::New( vertexSource, fragmentSource );
  PageTurnBookSpineEffect handle( shader );
  handle.SetUniform( IS_BACK_IMAGE_VISIBLE_PROPERTY_NAME, -1.f );
  handle.SetUniform( SHADOW_WIDTH_PROPERTY_NAME, DEFAULT_SHADOW_WIDTH );
  handle.SetUniform( SPINE_SHADOW_PARAMETER_PROPERTY_NAME, DEFAULT_SPINE_SHADOW_PARAMETER );
  float defaultPageWidth = Dali::Stage::GetCurrent().GetSize().x;
  handle.SetUniform( PAGE_WIDTH_PROPERTY_NAME, defaultPageWidth/(1.f-DEFAULT_SHADOW_WIDTH) );
  return handle;
}

void PageTurnBookSpineEffect::SetIsBackImageVisible( bool isBackVisible )
{
  float direction = isBackVisible ? 1.0f : -1.0f;
  SetUniform( IS_BACK_IMAGE_VISIBLE_PROPERTY_NAME, direction );
}

void PageTurnBookSpineEffect::SetPageWidth( float pageWidth )
{
  SetUniform( PAGE_WIDTH_PROPERTY_NAME, pageWidth );
}

void PageTurnBookSpineEffect::SetShadowWidth( float shadowWidth )
{
  SetUniform( SHADOW_WIDTH_PROPERTY_NAME, shadowWidth );
}

void PageTurnBookSpineEffect::SetSpineShadowParameter( const Vector2& spineShadowParameter )
{
  SetUniform( SPINE_SHADOW_PARAMETER_PROPERTY_NAME, spineShadowParameter);
}

} // namespace Toolkit

} // namespace Dali
