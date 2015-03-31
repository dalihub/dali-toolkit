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

#include <dali-toolkit/public-api/shader-effects/dissolve-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace
{

const std::string DISTORTION_PROPERTY_NAME( "uPercentage" );

} // namespace

DissolveEffect::DissolveEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
DissolveEffect::DissolveEffect(ShaderEffect handle)
:ShaderEffect(handle)
{
}

DissolveEffect::~DissolveEffect()
{
}


DissolveEffect DissolveEffect::New( bool useHighPrecision )
{
  std::string prefixHighPrecision( "precision highp float;\n");
  std::string prefixMediumPrecision( "precision mediump float;\n" );
  std::string vertexShader(
    "uniform float uPercentage;\n"
    "uniform vec3 uSaddleParam;\n"
    "uniform vec2 uTranslation;\n"
    "uniform vec2 uRotation; \n"
    "uniform float uToNext;\n"
    "varying float vPercentage;\n"
    "void main()\n"
    "{\n"
      "gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n"
      "vTexCoord = aTexCoord;\n"
      //Calculate the distortion value given the dissolve central line
      "vec2 texCoor = vec2( (aTexCoord.s - sTextureRect.s ) / (sTextureRect.p - sTextureRect.s), (aTexCoord.t- sTextureRect.t)/(sTextureRect.q - sTextureRect.t) ); \n"
      "vec2 value = texCoor + uTranslation; \n"
      "mat2 rotateMatrix = mat2( uRotation.s, uRotation.t, -uRotation.t, uRotation.s ); \n"
      "value = rotateMatrix * value; \n"
      "if(uToNext == 1.0)  \n"
      "  value.s = uSaddleParam[2] + value.s; \n"
      "float delay = value.t*value.t / uSaddleParam[0] - value.s*value.s/uSaddleParam[1];\n"
      "vPercentage = clamp( uPercentage*2.0 - 0.5*sin(delay*1.571) - 0.5, 0.0, 1.0 ); \n"
    "}\n");
  std::string fragmentShader(
    "varying float vPercentage;\n"
    "float rand(vec2 co) \n"
    "{\n"
    "  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); \n"
    "}\n"
    "void main()\n"
    "{\n"
      //Calculate the randomness
      "float offsetS = rand( vTexCoord * vPercentage ) * (sTextureRect.p - sTextureRect.s) - vTexCoord.s  + sTextureRect.s; \n"
      "float offsetT = rand( vec2(vTexCoord.t*vPercentage, vTexCoord.s * vPercentage) ) * (sTextureRect.q - sTextureRect.t) - vTexCoord.t + sTextureRect.t; \n"
      "vec2 lookupCoord = vTexCoord + vec2(offsetS, offsetT) * vPercentage; \n"
      "gl_FragColor = texture2D( sTexture, lookupCoord ) * uColor; \n"
      "gl_FragColor.a *= 1.0 - vPercentage; \n"
    "}" );

  // Create the implementation, temporarily owned on stack,
  Dali::ShaderEffect shaderEffectCustom;
  if( useHighPrecision )
  {
    shaderEffectCustom =  Dali::ShaderEffect::New( prefixHighPrecision+vertexShader, prefixHighPrecision + fragmentShader,
                                               GeometryType( GEOMETRY_TYPE_IMAGE),
                                               ShaderEffect::GeometryHints( ShaderEffect::HINT_GRID | ShaderEffect::HINT_BLENDING ) );
  }
  else
  {
    shaderEffectCustom =  Dali::ShaderEffect::New( prefixMediumPrecision+vertexShader, prefixMediumPrecision + fragmentShader,
                                               GeometryType( GEOMETRY_TYPE_IMAGE),
                                               ShaderEffect::GeometryHints( ShaderEffect::HINT_GRID | ShaderEffect::HINT_BLENDING ) );
  }

  /* Pass ownership to DissolveEffect through overloaded constructor, So that it now has access to the
     Dali::ShaderEffect implementation */
  Dali::Toolkit::DissolveEffect handle( shaderEffectCustom );

  handle.SetUniform( DISTORTION_PROPERTY_NAME, 0.0f );
  handle.SetProperty( ShaderEffect::Property::GRID_DENSITY, Dali::Property::Value(50.0f) );

  handle.SetCentralLine( Vector2(1.0f,0.5f), Vector2(-1.0f, 0.0f) );

  return handle;
}

void DissolveEffect::SetCentralLine( const Vector2& position, const Vector2& displacement )
{
  // the line passes through 'position' and has the direction of 'displacement'
  float coefA, coefB, coefC; //line equation: Ax+By+C=0;
  coefA = displacement.y;
  coefB = -displacement.x;
  coefC = -displacement.y*position.x + displacement.x*position.y;

  float inversedAABB = 1.f / (coefA*coefA+coefB*coefB);
  float inversedSqrtAABB = sqrtf(inversedAABB);
  float saddleA;

  //saddle surface(Hyperbolic paraboloid)function, used to calculate the dissolve starting time
  //z = y*y/a/a - x*x/b/b
  //with our selection of parameters(a and b), this value for any texture coordinate is between -1.0 and 1.0

  Vector3 saddleParam; // [0]: a*a, [1]: b*b, [2] b
  Vector2 translation;
  Vector2 rotation;
  float toNext = -1.f;
  if( displacement.x > 0.f || (EqualsZero(displacement.x) && displacement.y > 0.f) )
  {
    toNext = 1.f;
  }

  if( (displacement.y * displacement.x < 0.0f) )
  {
    //distance from (0,0) to the line
    float distanceTopLeft =  fabsf(coefC) * inversedSqrtAABB;
    //distance from (1, 1 ) to the line
    float distanceBottomRight = fabsf(coefA+coefB+coefC) * inversedSqrtAABB;
    saddleA = std::max( distanceTopLeft, distanceBottomRight );

    //foot of a perpendicular: (1,0) to the line
    float footX1 = ( coefB*coefB - coefA*coefC) * inversedAABB;
    float footY1 = (-coefA*coefB - coefB*coefC) * inversedAABB;
    //foot of a perpendicular: (0,1) to the line
    float footX2 = (-coefA*coefB - coefA*coefC) * inversedAABB;
    float footY2 = ( coefA*coefA - coefB*coefC) * inversedAABB;
    saddleParam[1] = (footX1-footX2)*(footX1-footX2) + (footY1-footY2)*(footY1-footY2);
    translation = Vector2(-footX2,-footY2);
  }
  else
  {
    //distance from(1,0) to the line
    float distanceTopRight = fabsf(coefA+coefC) * inversedSqrtAABB;
    //distance from(0,1) to the line
    float distanceBottomLeft = fabsf(coefB+coefC) * inversedSqrtAABB;
    saddleA = std::max( distanceTopRight, distanceBottomLeft );
    //foot of a perpendicular: (0,0) to the line
    float footX3 = (-coefA*coefC) * inversedAABB;
    float footY3 = (-coefB*coefC) * inversedAABB;
    //foot of a perpendicular: (1.0,1.0) to the line
    float footX4 = ( coefB*coefB - coefA*coefB - coefA*coefC) * inversedAABB;
    float footY4 = (-coefA*coefB + coefA*coefA- coefB*coefC) * inversedAABB;
    saddleParam[1] = (footX3-footX4)*(footX3-footX4) + (footY3-footY4)*(footY3-footY4);
    translation = Vector2(-footX3, -footY3);
  }

  saddleParam[2] = sqrtf(saddleParam[1]);
  saddleParam[0] = saddleA*saddleA;
  rotation = Vector2(-displacement.x, displacement.y);
  rotation.Normalize();

  SetUniform( "uSaddleParam", saddleParam );
  SetUniform( "uTranslation", translation );
  SetUniform( "uRotation", rotation );
  SetUniform( "uToNext", toNext );

}

void DissolveEffect::SetDistortion( float distortion )
{
  SetUniform( DISTORTION_PROPERTY_NAME, distortion );
}

const std::string& DissolveEffect::GetDistortionPropertyName() const
{
  return DISTORTION_PROPERTY_NAME;
}

} // namespace Toolkit

} // namespace Dali
