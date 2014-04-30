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

#include <dali-toolkit/public-api/shader-effects/swirl-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace
{

const std::string ANGLE_PROPERTY_NAME( "uAngle" );
const std::string CENTER_PROPERTY_NAME( "uCenter" );
const std::string RADIUS_PROPERTY_NAME( "uRadius" );

} // namespace

SwirlEffect::SwirlEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
SwirlEffect::SwirlEffect(ShaderEffect handle)
:ShaderEffect(handle)
{
}

SwirlEffect::~SwirlEffect()
{
}


SwirlEffect SwirlEffect::New(bool warp)
{
  // append the default version
  std::string fragmentShader(
      "uniform vec2  uTextureSize;\n"
      "uniform float uRadius;\n"
      "uniform float uAngle;\n"
      "uniform vec2  uCenter;\n"
      "void main()\n"
      "{\n"
      "  vec2 textureCenter = (sTextureRect.xy + sTextureRect.zw) * 0.5;\n"
      "  textureCenter = vTexCoord.st - textureCenter;\n"
      "  float distance = length(textureCenter);\n"
      "  if (distance >= uRadius)\n"
      "     discard;\n"
      "  float percent = (uRadius - distance) / uRadius;\n"
      "  float theta = percent * percent * uAngle * 4.0;\n"
      "  float sinTheta = sin(theta);\n"
      "  float cosTheta = cos(theta);\n" );
      // if warp, loose the sign from sin
  if( warp )
  {
    fragmentShader.append(
      "  textureCenter = vec2( dot( textureCenter, vec2(cosTheta, sinTheta) ), "
      "                        dot( textureCenter, vec2(sinTheta, cosTheta) ) );\n" );
  }
  else
  {
    fragmentShader.append(
      "  textureCenter = vec2( dot( textureCenter, vec2(cosTheta, -sinTheta) ), "
      "                        dot( textureCenter, vec2(sinTheta, cosTheta) ) );\n" );
  }
  fragmentShader.append(
    "  textureCenter += uCenter;\n"
    "  gl_FragColor = texture2D( sTexture, textureCenter ) * uColor;\n"
    "}" );

  // Create the implementation, temporarily owned on stack,
  Dali::ShaderEffect shaderEffectCustom =  Dali::ShaderEffect::New(
      "",
      fragmentShader,
      Dali::GeometryType( GEOMETRY_TYPE_IMAGE ),
      ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING | ShaderEffect::HINT_GRID ));

  /* Pass ownership to SwirlEffect through overloaded constructor, So that it now has access to the
     Dali::ShaderEffect implementation */
  Dali::Toolkit::SwirlEffect handle( shaderEffectCustom );

  handle.SetUniform( ANGLE_PROPERTY_NAME, 0.0f );
  handle.SetUniform( CENTER_PROPERTY_NAME, Vector2(0.5f, 0.5f) );
  handle.SetUniform( RADIUS_PROPERTY_NAME, 1.0f );

  return handle;
}

void SwirlEffect::SetAngle(float angle)
{
  SetUniform( ANGLE_PROPERTY_NAME, angle );
}

void SwirlEffect::SetCenter(const Vector2& center)
{
  SetUniform( CENTER_PROPERTY_NAME, center );
}

void SwirlEffect::SetRadius(float radius)
{
  SetUniform( RADIUS_PROPERTY_NAME, radius );
}

const std::string& SwirlEffect::GetAnglePropertyName() const
{
  return ANGLE_PROPERTY_NAME;
}

const std::string& SwirlEffect::GetCenterPropertyName() const
{
  return CENTER_PROPERTY_NAME;
}

const std::string& SwirlEffect::GetRadiusPropertyName() const
{
  return RADIUS_PROPERTY_NAME;
}

} // namespace Toolkit

} // namespace Dali
