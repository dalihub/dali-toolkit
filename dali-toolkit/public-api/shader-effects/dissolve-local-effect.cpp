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

#include <sstream>

#include <dali-toolkit/public-api/shader-effects/dissolve-local-effect.h>

#include <dali-toolkit/public-api/shader-effects/dissolve-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace
{
  const std::string DISTORTION_PROPERTY_NAME( "uPercentage" );
  const std::string CENTER_PROPERTY_NAME( "uCenter" );
  const std::string RADIUS_PROPERTY_NAME( "uRadius" );
  const std::string TRANSPARENCY_PROPERTY_NAME( "uTransparency" );
}

DissolveLocalEffect::DissolveLocalEffect()
: mNumberOfDimples( 1 )
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
DissolveLocalEffect::DissolveLocalEffect( ShaderEffect handle )
: ShaderEffect( handle ),
  mNumberOfDimples( 1 )
{
}

DissolveLocalEffect::~DissolveLocalEffect()
{
}

DissolveLocalEffect DissolveLocalEffect::New( unsigned int numberOfDimples )
{
  std::ostringstream vertexShaderStringStream;
  vertexShaderStringStream << "#define NUMBER_OF_DIMPLE "<< numberOfDimples << "\n";
  std::string vertexShader(
    "precision highp float;\n"
    "uniform vec2 uCenter[ NUMBER_OF_DIMPLE ];\n"
    "uniform float uRadius[ NUMBER_OF_DIMPLE ]; \n"
    "uniform float uPercentage[ NUMBER_OF_DIMPLE ]; \n"
    "varying float vPercentage;\n"
    "void main()\n"
    "{\n"
    "  vec4 position = uModelView * vec4( aPosition, 1.0 );\n"
    "  float percentage = 0.0;\n"
    "  for( int i=0; i<NUMBER_OF_DIMPLE; ++i )\n"
    "  {\n"
    "    float distance = distance(uCenter[i], position.xy);\n"
    "    percentage = max(percentage, uPercentage[i] * cos(clamp( distance/uRadius[i], 0.0, 1.0 )*1.57) );"
    "  }\n"
    "  vPercentage = clamp( percentage, 0.0, 1.0 );\n"
    "  gl_Position = uProjection * position;\n"
    "  vTexCoord = aTexCoord;\n"
    "}\n");
  vertexShaderStringStream << vertexShader;

  std::string fragmentShader(
    "precision highp float;\n"
    "uniform float uTransparency;\n"
    "varying float vPercentage;\n"
    "float rand(vec2 co) \n"
    "{\n"
    "  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); \n"
    "}\n"
    "void main()\n"
    "{\n"
      //Calculate the randomness
    "  float offsetS = rand( vTexCoord * vPercentage ); \n"
    "  float offsetT = rand( vec2(vTexCoord.t*vPercentage, vTexCoord.s * vPercentage) ); \n"
    "  vec2 lookupCoord = vTexCoord + vec2(offsetS, offsetT) * vPercentage; \n"
    "  gl_FragColor = texture2D( sTexture, lookupCoord ) * uColor; \n"
    "  gl_FragColor.a *= 1.0 - uTransparency*vPercentage; \n"
    "}\n");

  ShaderEffect shaderEffect = ShaderEffect::New( vertexShaderStringStream.str(), fragmentShader,
                                                 GeometryType( GEOMETRY_TYPE_IMAGE),
                                                 ShaderEffect::GeometryHints( ShaderEffect::HINT_GRID | ShaderEffect::HINT_BLENDING ) );

  DissolveLocalEffect handle( shaderEffect );
  handle.mNumberOfDimples = numberOfDimples;

  //Register uniform properties
  for( unsigned int i = 0; i < numberOfDimples; i++ )
  {
    handle.SetCenter(i, Vector2(0.f,0.f));
    handle.SetRadius(i, 0.f);
    handle.SetDistortion( i, 0.f );
  }
  handle.SetProperty( ShaderEffect::Property::GRID_DENSITY, Dali::Property::Value(5.f) );
  handle.SetTransparency( 0.5f );

  return handle;
}

unsigned int DissolveLocalEffect::GetNumberOfDimples() const
{
  return mNumberOfDimples;
}

void DissolveLocalEffect::SetTransparency( float transparency)
{
  SetUniform( TRANSPARENCY_PROPERTY_NAME, transparency );
}

void DissolveLocalEffect::SetCenter( unsigned int index, const Vector2& center )
{
  DALI_ASSERT_ALWAYS( index < mNumberOfDimples );
  SetUniform( GetCenterPropertyName( index ), center, ShaderEffect::COORDINATE_TYPE_VIEWPORT_POSITION );
}

void DissolveLocalEffect::SetRadius( unsigned int index, float radius )
{
  DALI_ASSERT_ALWAYS( index < mNumberOfDimples );
  SetUniform( GetRadiusPropertyName( index ), radius );
}

void DissolveLocalEffect::SetDistortion( unsigned int index, float distortion )
{
  DALI_ASSERT_ALWAYS( index < mNumberOfDimples );
  SetUniform( GetDistortionPropertyName( index ), distortion );
}

std::string DissolveLocalEffect::GetCenterPropertyName( unsigned int index ) const
{
  DALI_ASSERT_ALWAYS( index < mNumberOfDimples );
  std::ostringstream oss;
  oss<< CENTER_PROPERTY_NAME << "[" << index << "]";
  return oss.str();
}

std::string DissolveLocalEffect::GetRadiusPropertyName( unsigned int index ) const
{
  DALI_ASSERT_ALWAYS( index < mNumberOfDimples );
  std::ostringstream oss;
  oss<< RADIUS_PROPERTY_NAME << "[" << index << "]";
  return oss.str();
}

std::string DissolveLocalEffect::GetDistortionPropertyName( unsigned int index ) const
{
  DALI_ASSERT_ALWAYS( index < mNumberOfDimples );
  std::ostringstream oss;
  oss<< DISTORTION_PROPERTY_NAME << "["<< index << "]";
  return oss.str();
}

} // namespace Toolkit

} // namespace Dali
