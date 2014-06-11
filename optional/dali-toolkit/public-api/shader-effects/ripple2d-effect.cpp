//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali-toolkit/public-api/shader-effects/ripple2d-effect.h>


namespace Dali
{

namespace Toolkit
{

namespace
{

const std::string TEXTURE_SIZE_PROPERTY_NAME( "uTextureSize" );
const std::string AMPLITUDE_PROPERTY_NAME( "uAmplitude" );
const std::string TIME_PROPERTY_NAME( "uTime" );

} // namespace

Ripple2DEffect::Ripple2DEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
Ripple2DEffect::Ripple2DEffect(ShaderEffect handle)
:ShaderEffect(handle)
{
}

Ripple2DEffect::~Ripple2DEffect()
{
}


Ripple2DEffect Ripple2DEffect::New()
{
    // append the default version
    std::string fragmentShader(
        "uniform float uAmplitude;\n"
        "uniform float uTime;\n"
        "void main()\n"
        "{\n"
        "  highp vec2 textureSize = sTextureRect.zw - sTextureRect.xy;\n"
        "  highp vec2 pos = -1.0 + 2.0 * vTexCoord.st/textureSize;\n"
        "  highp float len = length(pos);\n"
        "  highp vec2 texCoord = vTexCoord.st/textureSize + pos/len * sin( len * 12.0 - uTime * 4.0 ) * uAmplitude;\n"
        "  gl_FragColor = texture2D(sTexture, texCoord) * uColor;\n"
        "}" );

  // Create the implementation, temporarily owned on stack
  Dali::ShaderEffect shaderEffect =  Dali::ShaderEffect::New(
      "",
      fragmentShader,
      Dali::GeometryType( GEOMETRY_TYPE_IMAGE ),
      ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING | ShaderEffect::HINT_GRID ));

  /* Pass ownership to Ripple2DEffect through overloaded constructor, So that it now has access to the
     Dali::ShaderEffect implementation */
  Dali::Toolkit::Ripple2DEffect handle( shaderEffect );

  handle.SetUniform( TEXTURE_SIZE_PROPERTY_NAME, Vector2(0.0f, 0.0f) );
  handle.SetUniform( AMPLITUDE_PROPERTY_NAME, 0.0f );
  handle.SetUniform( TIME_PROPERTY_NAME, 0.0f );

  return handle;

}

void Ripple2DEffect::SetAmplitude(float amplitude)
{
  SetUniform( AMPLITUDE_PROPERTY_NAME, amplitude );
}

void Ripple2DEffect::SetTime(float time)
{
  SetUniform( TIME_PROPERTY_NAME, time );
}

const std::string& Ripple2DEffect::GetAmplitudePropertyName() const
{
  return AMPLITUDE_PROPERTY_NAME;
}

const std::string& Ripple2DEffect::GetTimePropertyName() const
{
  return TIME_PROPERTY_NAME;
}

} // namespace Toolkit

} // namespace Dali
