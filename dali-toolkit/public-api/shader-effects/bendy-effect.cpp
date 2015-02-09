/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/public-api/shader-effects/bendy-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace
{

const std::string CENTER_PROPERTY_NAME( "uCenter" );
const std::string DIRECTION_PROPERTY_NAME( "uDirection" );
const std::string RADIUS_PROPERTY_NAME( "uRadius" );

} // namespace

BendyEffect::BendyEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
BendyEffect::BendyEffect(ShaderEffect handle)
:ShaderEffect(handle)
{
}

BendyEffect::~BendyEffect()
{
}


BendyEffect BendyEffect::New()
{
  // append the default version
  std::string vertextShader(
              "uniform mediump   vec2  uCenter;\n"
              "uniform mediump   vec2  uDirection;\n"
              "uniform mediump   float uRadius;\n"
              "\n"
              "varying mediump   float vShade;\n"
              "\n"
              "void main()\n"
              "{\n"
                  " mediump float lighting = 0.25;\n"
                  " mediump vec4 position = uModelView * vec4(aPosition,1.0);\n"
                  "\n"
                  " mediump vec2 d = position.xy - uCenter;\n"
                  " mediump float dist = max( 0.0, dot(d,uDirection) );\n"
                  " mediump float radius = max(0.0, uRadius - dist * 0.01);\n"
                  "\n"
                  " mediump float cs = cos(dist / radius / 2.0);\n"
                  " mediump float sn = sin(dist / radius / 2.0);\n"
                  "\n"
                  "position.xy = position.xy - uDirection * dist;\n"
                  "\n"
                  "position.xy += uDirection * sn * radius;\n"
                  "position.z += (1.0 - cs) * radius;\n"
                  "\n"
                  "gl_Position = uProjection * position;\n"
                  "\n"
                  "vShade = 1.0 - abs(sn) * lighting;\n"
                  "\n"
                  "vTexCoord = aTexCoord;\n"
              "}" );

  std::string fragmentShader(
              "varying mediump float  vShade;\n"
              "\n"
              "void main()\n"
              "{\n"
              "  gl_FragColor = texture2D(sTexture, vTexCoord) * uColor * vec4(vShade,vShade,vShade,1.0);\n"
              "}" );

  // Create the implementation, temporarily owned on stack,
  Dali::ShaderEffect shaderEffectCustom =  Dali::ShaderEffect::New(
      vertextShader,
      fragmentShader,
      GeometryType( GEOMETRY_TYPE_IMAGE ),
      ShaderEffect::GeometryHints( HINT_GRID | HINT_DEPTH_BUFFER ));

  /* Pass ownership to BendyEffect through overloaded constructor, So that it now has access to the
     Dali::ShaderEffect implementation */
  Dali::Toolkit::BendyEffect handle( shaderEffectCustom );

  handle.SetUniform( CENTER_PROPERTY_NAME, Vector2(0.0f, 0.0f), COORDINATE_TYPE_VIEWPORT_POSITION );
  handle.SetUniform( DIRECTION_PROPERTY_NAME, Vector2(0.0f, 0.0f), COORDINATE_TYPE_VIEWPORT_DIRECTION );
  handle.SetUniform( RADIUS_PROPERTY_NAME, 0.0f );

  return handle;
}

void BendyEffect::SetCenter( const Vector2& center )
{
  SetUniform( CENTER_PROPERTY_NAME, center, ShaderEffect::COORDINATE_TYPE_VIEWPORT_POSITION );
}

void BendyEffect::SetDirection( const Vector2& direction )
{
  Vector2 temp(direction);
  temp.Normalize();

  Vector2 newDirection(temp.x, temp.y);

  SetUniform( DIRECTION_PROPERTY_NAME, newDirection, ShaderEffect::COORDINATE_TYPE_VIEWPORT_DIRECTION );
}

void BendyEffect::SetRadius( float radius )
{
  SetUniform( RADIUS_PROPERTY_NAME, radius );
}

const std::string& BendyEffect::GetCenterPropertyName() const
{
  return CENTER_PROPERTY_NAME;
}

const std::string& BendyEffect::GetDirectionPropertyName() const
{
  return DIRECTION_PROPERTY_NAME;
}

const std::string& BendyEffect::GetRadiusPropertyName() const
{
  return RADIUS_PROPERTY_NAME;
}

} // namespace Toolkit

} // namespace Dali
