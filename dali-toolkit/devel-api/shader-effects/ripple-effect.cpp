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

#include <dali-toolkit/devel-api/shader-effects/ripple-effect.h>


namespace Dali
{

namespace Toolkit
{

namespace
{

const std::string AMPLITUDE_PROPERTY_NAME( "uAmplitude" );
const std::string CENTER_PROPERTY_NAME( "uCenter" );
const std::string TIME_PROPERTY_NAME( "uTime" );

} // namespace

RippleEffect::RippleEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
RippleEffect::RippleEffect(ShaderEffect handle)
:ShaderEffect(handle)
{
}

RippleEffect::~RippleEffect()
{
}


RippleEffect RippleEffect::New()
{

  std::string vertexShader(
       "precision mediump float;\n"
       "uniform mediump   vec2  uCenter;\n"
       "uniform mediump   float uTime;\n"
       "uniform mediump   float uAmplitude;\n"
       "uniform mediump   float uLighting;\n"
       "uniform mediump   float uWaveLength;\n"
       "varying mediump   float vLight;\n"
       "varying mediump   float vShade;\n"
       "void main()\n"
       "{\n"
          "float lighting = uAmplitude * 0.02;\n"
          "float waveLength = uAmplitude * 0.0016;\n"
          "vec4 world = uModelView * vec4(aPosition,1.0);\n"
          "vec2 d = vec2(world.x - uCenter.x, world.y - uCenter.y);\n"
          "float dist = length(d);\n"
          "float amplitude = cos(uTime - dist*waveLength);\n"
          "float slope     = sin(uTime - dist*waveLength);\n"
          "world.z += amplitude * uAmplitude;\n"
          "gl_Position = uProjection * world;\n"
          "vec2 lightDirection = vec2(-0.707,0.707);\n"
          "float dot = 0.0;\n"
          "if(dist > 0.0)\n"
          "{\n"
          "  dot = dot(normalize(d),lightDirection) * lighting;\n"
          "}\n"
          "vShade = 1.0 - (dot * slope);\n"
          "vLight = max(0.0, dot * -slope);\n"
          "vTexCoord = aTexCoord;\n"
        "}" );

  // append the default version
  std::string imageFragmentShader(
        "precision mediump float;\n"
        "varying mediump float  vLight;\n"
        "varying mediump float  vShade;\n"
        "void main()\n"
        "{\n"
        "  gl_FragColor = texture2D(sTexture, vTexCoord) * uColor * vec4(vShade,vShade,vShade,1.0) + vec4(vLight, vLight, vLight,0.0);\n"
        "}" );

  // Create the implementation, temporarily owned on stack
  Dali::ShaderEffect shaderEffect =  Dali::ShaderEffect::New(
      vertexShader, imageFragmentShader, GeometryType(GEOMETRY_TYPE_IMAGE), GeometryHints(HINT_GRID) );

  /* Pass ownership to RippleEffect through overloaded constructor, So that it now has access to the
     Dali::ShaderEffect implementation */
  Dali::Toolkit::RippleEffect handle( shaderEffect );

  handle.SetUniform( AMPLITUDE_PROPERTY_NAME, 0.0f );
  handle.SetUniform( CENTER_PROPERTY_NAME, Vector2(0.0f, 0.0f));
  handle.SetUniform( TIME_PROPERTY_NAME, 0.0f );

  return handle;
}

void RippleEffect::SetAmplitude(float amplitude)
{
  SetUniform( AMPLITUDE_PROPERTY_NAME, amplitude );
}

void RippleEffect::SetCenter(const Vector2& center)
{
  SetUniform( CENTER_PROPERTY_NAME, center );
}

void RippleEffect::SetTime(float time)
{
  SetUniform( TIME_PROPERTY_NAME, time );
}

const std::string& RippleEffect::GetAmplitudePropertyName() const
{
  return AMPLITUDE_PROPERTY_NAME;
}

const std::string& RippleEffect::GetCenterPropertyName() const
{
  return CENTER_PROPERTY_NAME;
}

const std::string& RippleEffect::GetTimePropertyName() const
{
  return TIME_PROPERTY_NAME;
}

} // namespace Toolkit

} // namespace Dali
