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

#include <dali-toolkit/public-api/shader-effects/spot-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace
{

const float DEFAULT_RADIUS = 0.0f;
const std::string CENTER_PROPERTY_NAME( "uCenter" );
const std::string RADIUS_PROPERTY_NAME( "uRadius" );

} // namespace

SpotEffect::SpotEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
SpotEffect::SpotEffect(ShaderEffect handle)
:ShaderEffect(handle)
{
}

SpotEffect::~SpotEffect()
{
}

SpotEffect SpotEffect::New()
{
  // append the default version
  std::string vertexShader(
      "uniform mediump   vec2  uCenter;\n"
      "uniform mediump   float  uRadius;\n"
      "varying mediump   float  vRange;\n"
      "\n"
      "void main()\n"
      "{\n"
      "  vec4 world = vec4(aPosition, 1.0);\n"
      "  \n"
      "  vec2 d = vec2(world.xy - uCenter);\n"
      "  float dist = length(d);\n"
      "  \n"
      "  float range = (uRadius - dist) / (uRadius);\n"
      "  vRange = max(0.1, range);\n"
      "  \n"
      "  gl_Position = uMvpMatrix * world;\n"
      "  vTexCoord = aTexCoord;\n"
      "}");

  std::string fragmentShader(
      "varying mediump   float  vRange;\n"
      "\n"
      "void main()\n"
      "{\n"
      "  gl_FragColor = texture2D(sTexture, vTexCoord) * vec4(vRange, vRange, vRange, 1.0) * uColor;\n"
      "}" );

  // Create the implementation, temporarily owned on stack
  Dali::ShaderEffect shaderEffect =  Dali::ShaderEffect::New(vertexShader, fragmentShader,
                                                             Dali::GeometryType( GEOMETRY_TYPE_IMAGE ),
                                                             ShaderEffect::GeometryHints( ShaderEffect::HINT_GRID ));

  /* Pass ownership to SpotEffect through overloaded constructor, So that it now has access to the
     Dali::ShaderEffect implementation */
  Dali::Toolkit::SpotEffect handle( shaderEffect );

  handle.SetUniform( CENTER_PROPERTY_NAME, Vector2(0.0f, 0.0f) );
  handle.SetUniform( RADIUS_PROPERTY_NAME, DEFAULT_RADIUS );

  return handle;

}

void SpotEffect::SetCenter(const Vector2& center)
{
  SetUniform( CENTER_PROPERTY_NAME, center );
}

void SpotEffect::SetRadius(float radius)
{
  SetUniform( RADIUS_PROPERTY_NAME, radius );
}

const std::string& SpotEffect::GetCenterPropertyName() const
{
  return CENTER_PROPERTY_NAME;
}

const std::string& SpotEffect::GetRadiusPropertyName() const
{
  return RADIUS_PROPERTY_NAME;
}

} // namespace Toolkit

} // namespace Dali
