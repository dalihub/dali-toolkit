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

#include <dali-toolkit/public-api/shader-effects/shear-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace
{

const std::string CENTER_PROPERTY_NAME( "uCenter" );
const std::string ANGLE_X_AXIS_PROPERTY_NAME( "uAngleXAxis" );
const std::string ANGLE_Y_AXIS_PROPERTY_NAME( "uAngleYAxis" );

} // namespace

ShearEffect::ShearEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
ShearEffect::ShearEffect(ShaderEffect handle)
:ShaderEffect(handle)
{
}

ShearEffect::~ShearEffect()
{
}


ShearEffect ShearEffect::New()
{
  // append the default version
  std::string vertexShader(
              "uniform mediump  vec2  uCenter;\n"
              "uniform mediump  float uAngleXAxis;\n"
              "uniform mediump  float uAngleYAxis;\n"
              "\n"
              "void main()\n"
              "{\n"
                  "mediump vec4 world = uModelView * vec4(aPosition,1.0);\n"
                  "\n"
                  "world.x = world.x + tan(radians(uAngleXAxis)) * (world.y - uCenter.y * world.w);\n"
                  "world.y = world.y + tan(radians(uAngleYAxis)) * (world.x - uCenter.x * world.w);\n"
                  "\n"
                  "gl_Position = uProjection * world;\n"
                  "\n"
                  "vTexCoord = aTexCoord;\n"
              "}" );

  // Create the implementation, temporarily owned on stack,
  ShaderEffect shaderEffectCustom =  Dali::ShaderEffect::New(
      vertexShader,
      "",
      GeometryType( GEOMETRY_TYPE_IMAGE | GEOMETRY_TYPE_TEXT ),
      GeometryHints( HINT_GRID ));

  // Pass ownership to ShearEffect through overloaded constructor, So that it now has access to the
  // Dali::ShaderEffect implementation
  Dali::Toolkit::ShearEffect handle( shaderEffectCustom );

  handle.SetUniform( CENTER_PROPERTY_NAME, Vector2(0.0f, 0.0f), COORDINATE_TYPE_VIEWPORT_POSITION );
  handle.SetUniform( ANGLE_X_AXIS_PROPERTY_NAME, 0.0f);
  handle.SetUniform( ANGLE_Y_AXIS_PROPERTY_NAME, 0.0f);

  return handle;
}

void ShearEffect::SetCenter( const Vector2& center )
{
  SetUniform( CENTER_PROPERTY_NAME, center, COORDINATE_TYPE_VIEWPORT_POSITION );
}

void ShearEffect::SetAngleXAxis( float angle )
{
  SetUniform( ANGLE_X_AXIS_PROPERTY_NAME, angle );
};

void ShearEffect::SetAngleYAxis( float angle )
{
  SetUniform( ANGLE_Y_AXIS_PROPERTY_NAME, angle );
};

const std::string& ShearEffect::GetCenterPropertyName() const
{
  return CENTER_PROPERTY_NAME;
}

const std::string& ShearEffect::GetAngleXAxisPropertyName() const
{
  return ANGLE_X_AXIS_PROPERTY_NAME;
}

const std::string& ShearEffect::GetAngleYAxisPropertyName() const
{
  return ANGLE_Y_AXIS_PROPERTY_NAME;
}

} // namespace Toolkit

} // namespace Dali
