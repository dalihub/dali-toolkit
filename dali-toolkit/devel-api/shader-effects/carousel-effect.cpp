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

#include <dali-toolkit/devel-api/shader-effects/carousel-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace
{

const std::string RADIUS_PROPERTY_NAME( "uRadius" );
const std::string ANGLE_PER_UNIT_PROPERTY_NAME( "uAnglePerUnit" );
const std::string CENTER_PROPERTY_NAME( "uCenter" );

} // namespace

CarouselEffect::CarouselEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
CarouselEffect::CarouselEffect(ShaderEffect handle)
:ShaderEffect(handle)
{
}

CarouselEffect::~CarouselEffect()
{
}


CarouselEffect CarouselEffect::New()
{
  // append the default version
  std::string vertexShader(
              "uniform float uRadius;\n"
              "uniform mediump vec2 uCenter;\n"
              "uniform mediump vec2 uAnglePerUnit;\n"
              "\n"
              "void main()\n"
              "{\n"
              "    mediump vec4 world = uModelView * vec4(aPosition,1.0);\n"
              "    mediump vec2 d = (world.xy - uCenter) * uAnglePerUnit;\n"
              "    mediump float a = length(d);\n"
              "    mediump float cs = cos(radians(a));\n"
              "    world.z -= cs * uRadius;\n"
              "    gl_Position = uProjection * world;\n"
              "    \n"
              "    vTexCoord = aTexCoord;\n"
              "}\n");

  ShaderEffect shaderEffectCustom = ShaderEffect::New(vertexShader,
          "",
          GeometryType( GEOMETRY_TYPE_IMAGE ),
          ShaderEffect::GeometryHints( ShaderEffect::HINT_GRID | ShaderEffect::HINT_DEPTH_BUFFER ));

  // Pass ownership to CarouselEffect through overloaded constructor, So that it now has access to the
  // Dali::ShaderEffect implementation
  CarouselEffect handle( shaderEffectCustom );

  handle.SetUniform( RADIUS_PROPERTY_NAME, 0.0f );
  handle.SetUniform( CENTER_PROPERTY_NAME, Vector2( 0.0f, 0.0f ) );
  handle.SetUniform( ANGLE_PER_UNIT_PROPERTY_NAME, Vector2( 0.0f, 0.0f ) );

  return handle;
}

void CarouselEffect::SetRadius( float radius)
{
  SetUniform( RADIUS_PROPERTY_NAME, radius );
}

void CarouselEffect::SetCenter( const Vector2& center )
{
  SetUniform( CENTER_PROPERTY_NAME, center );
}

void CarouselEffect::SetAnglePerUnit( const Vector2& angle )
{
  SetUniform( ANGLE_PER_UNIT_PROPERTY_NAME, angle );
}

const std::string& CarouselEffect::GetRadiusPropertyName() const
{
  return RADIUS_PROPERTY_NAME;
}

const std::string& CarouselEffect::GetCenterPropertyName() const
{
  return CENTER_PROPERTY_NAME;
}

const std::string& CarouselEffect::GetAnglePerUnitPropertyName() const
{
  return ANGLE_PER_UNIT_PROPERTY_NAME;
}

} // namespace Toolkit

} // namespace Dali
