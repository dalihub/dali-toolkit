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

// CLASS HEADER
#include "water-effect-impl.h"

// EXTERNAL INCLUDES
#include <sstream>
#include <iomanip>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/shader-effects/water-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

static const unsigned int LIGHT_MAP_SIZE = 512; ///< Size of the bitmap created for the pre-calculated wave function
static const float MAX_WAVE_RADIUS = 80.0f;     ///< Maximum radius of the wave in percentage of the texture coordinates

} // namespace

WaterEffect::WaterEffect( unsigned int numberOfWaves )
: mNumberOfWaves( numberOfWaves )
{
}

WaterEffect::~WaterEffect()
{
}

unsigned int WaterEffect::GetNumberOfWaves() const
{
  return mNumberOfWaves;
}


Dali::Toolkit::WaterEffect WaterEffect::CreateShaderEffect( unsigned int numberOfWaves )
{
  std::ostringstream vertexShaderStringStream;
  vertexShaderStringStream << "#define NUMBER_OF_DROPS " << numberOfWaves << "\n";
  vertexShaderStringStream << "#define MAX_WAVE_RADIUS " << std::setprecision(1) << MAX_WAVE_RADIUS << "\n";

  std::string vertexShader(
      "mediump vec4 position = vec4( aPosition, 1.0 );\n"
      "\n"
      "struct Drops\n"
      "{\n"
      "  mediump vec2 center;\n"
      "  mediump float radius;\n"
      "  mediump float amplitude;\n"
      "};\n"
      "uniform Drops uDrops[NUMBER_OF_DROPS];\n"
      "varying mediump vec4 vColor;\n"
      "void main()\n"
      "{\n"
      "  position = uModelView * position;\n"
      "  mediump float refraction = 0.0;\n"
      "  for (int i=0; i<NUMBER_OF_DROPS; ++i)\n"
      "  {\n"
      "    mediump float distance = distance( uDrops[i].center, position.xy );\n"
      "    mediump float attenuation = clamp(distance / uDrops[i].radius, 0.0, 1.0) * 1.57;\n"
      "    refraction += uDrops[i].amplitude * cos( (distance - uDrops[i].radius) *0.075 ) * cos(attenuation);\n"
      "  }\n"
      "  vColor = uColor + vec4(vec3(clamp(refraction, -0.1, 1.0)), 1.0);\n"
      "  vTexCoord = aTexCoord + vec2( sin(refraction)/MAX_WAVE_RADIUS );\n"
      "  gl_Position = uProjection * position;\n"
      "}\n");
  vertexShaderStringStream << vertexShader;

  std::ostringstream fragmentShaderStringStream;

  std::string fragmentShader(
      "varying mediump vec4 vColor;\n"
      "void main()\n"
      "{\n"
      "  gl_FragColor = texture2D( sTexture, vTexCoord)*vColor;\n"
      "}\n");
  fragmentShaderStringStream << fragmentShader;

  // Create the implementation, temporarily owned on stack
  ShaderEffect shaderEffect = ShaderEffect::New(
      vertexShaderStringStream.str(),
      fragmentShaderStringStream.str(),
      GEOMETRY_TYPE_IMAGE,
      ShaderEffect::HINT_GRID );

  WaterEffect* shaderImpl = new WaterEffect( numberOfWaves );

  Dali::Toolkit::WaterEffect handle = Dali::Toolkit::WaterEffect( shaderEffect, shaderImpl );

  shaderImpl->Initialize( handle );

  for ( unsigned int index = 0; index < shaderImpl->mNumberOfWaves; ++index )
  {
    handle.SetUniform( shaderImpl->GetAmplitudePropertyName( index ), 0.0f );
    handle.SetUniform( shaderImpl->GetCenterPropertyName( index ), Vector2(0.0f, 0.0f), ShaderEffect::COORDINATE_TYPE_VIEWPORT_POSITION );
    handle.SetUniform( shaderImpl->GetPropagationPropertyName( index ), 0.0f );
  }

  return handle;
}

void WaterEffect::SetAmplitude( unsigned int index, float amplitude )
{
  DALI_ASSERT_ALWAYS( index < mNumberOfWaves );
  mShaderEffect.SetUniform( GetAmplitudePropertyName( index ), amplitude );
}

void WaterEffect::SetCenter( unsigned int index, const Vector2& center )
{
  DALI_ASSERT_ALWAYS( index < mNumberOfWaves );
  mShaderEffect.SetUniform( GetCenterPropertyName( index ), center, ShaderEffect::COORDINATE_TYPE_VIEWPORT_POSITION );
}

void WaterEffect::SetPropagation( unsigned int index, float radius )
{
  DALI_ASSERT_ALWAYS( index < mNumberOfWaves );
  mShaderEffect.SetUniform( GetPropagationPropertyName( index ) , radius );
}

float WaterEffect::GetAmplitude( unsigned int index ) const
{
  DALI_ASSERT_ALWAYS( index < mNumberOfWaves );
  Property::Index propertyIndex = mShaderEffect.GetPropertyIndex( GetAmplitudePropertyName( index ) );
  return mShaderEffect.GetProperty( propertyIndex ).Get<float>();
}

Vector2 WaterEffect::GetCenter( unsigned int index ) const
{
  DALI_ASSERT_ALWAYS( index < mNumberOfWaves );
  Property::Index propertyIndex = mShaderEffect.GetPropertyIndex( GetCenterPropertyName( index ) );
  return mShaderEffect.GetProperty( propertyIndex ).Get<Vector2>();
}

float WaterEffect::GetPropagation( unsigned int index ) const
{
  DALI_ASSERT_ALWAYS( index < mNumberOfWaves );
  Property::Index propertyIndex = mShaderEffect.GetPropertyIndex( GetPropagationPropertyName( index ) );
  return mShaderEffect.GetProperty( propertyIndex ).Get<float>();
}

std::string WaterEffect::GetAmplitudePropertyName( unsigned int index ) const
{
  DALI_ASSERT_ALWAYS( index < mNumberOfWaves );
  std::ostringstream oss;
  oss << "uDrops[" << index << "].amplitude";
  return oss.str();
}

std::string WaterEffect::GetCenterPropertyName( unsigned int index ) const
{
  DALI_ASSERT_ALWAYS( index < mNumberOfWaves );
  std::ostringstream oss;
  oss << "uDrops[" << index << "].center";
  return oss.str();
}

std::string WaterEffect::GetPropagationPropertyName( unsigned int index ) const
{
  DALI_ASSERT_ALWAYS( index < mNumberOfWaves );
  std::ostringstream oss;
  oss << "uDrops[" << index << "].radius";
  return oss.str();
}

void WaterEffect::Initialize( Dali::ShaderEffect shaderEffect )
{
  // Save a reference to the shader handle
  mShaderEffect = shaderEffect;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
