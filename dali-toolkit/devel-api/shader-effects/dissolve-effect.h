#ifndef __DALI_TOOLKIT_SHADER_EFFECT_DISSOLVE_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_DISSOLVE_H__

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

// EXTERNAL INCLUDES
#include <string.h>
#include <dali/devel-api/rendering/shader.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Set the dissolve central line.
 *
 * Use one point (position) and one direction ( displacement ) vector to define this line
 * As we use the texture coordinate as pixel position to calculate random offset,
 * the line should passing through rectangle {(0,0),(0,1),(1,0),(1,1)},
 * so make the position parameter with two component values between 0.0 to 1.0
 * @param[in] position The point ( locates within rectangle {(0,0),(0,1),(1,0),(1,1)} ) passed through by the central line
 * @param[in] displacement The direction of the central line
 * @param[in] initialProgress, the normalised initial progress of the shader
 */
inline void DissolveEffectSetCentralLine( Actor& actor, const Vector2& position, const Vector2& displacement, float initialProgress )
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

  actor.RegisterProperty( "uSaddleParam", saddleParam );
  actor.RegisterProperty( "uTranslation", translation );
  actor.RegisterProperty( "uRotation", rotation );
  actor.RegisterProperty( "uToNext", toNext );
  actor.RegisterProperty( "uPercentage", initialProgress, Dali::Property::ANIMATABLE );
}
/**
 * @brief Create a new Dissolve effect
 *
 *  DissolveEffect is a custom shader effect to achieve Dissolve effects in Image actors.
 *
 *  Animatable/Constrainable uniforms:
 *    "uPercentage" - This value is proportional to the distortion applied; a value of zero means no distortion.
 *
 *  @param[in] useHighPrecision True if using high precision in fragment shader for fully random noise, false otherwise
 *  @return The newly created Property::Map with the dissolve effect
 */

inline Property::Map CreateDissolveEffect( bool useHighPrecision = true )
{
  const char* prefixHighPrecision( "precision highp float;\n");
  const char* prefixMediumPrecision( "precision mediump float;\n" );

  const char* vertexShader( DALI_COMPOSE_SHADER(
    attribute mediump vec2 aPosition;\n
    \n
    uniform mediump mat4 uMvpMatrix;\n
    uniform vec3 uSize;\n
    uniform vec4 uTextureRect;
    \n
    uniform float uPercentage;\n
    uniform vec3 uSaddleParam;\n
    uniform vec2 uTranslation;\n
    uniform vec2 uRotation; \n
    uniform float uToNext;\n
    \n
    varying float vPercentage;\n
    varying vec2 vTexCoord;\n

    void main()\n
    {\n
      mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n
      vertexPosition.xyz *= uSize;\n
      vertexPosition = uMvpMatrix * vertexPosition;\n
      gl_Position = vertexPosition;\n

      vec2 texCoord = aPosition + vec2(0.5);
      vTexCoord = texCoord;\n
      //Calculate the distortion value given the dissolve central line
      vec2 value = texCoord + uTranslation; \n
      mat2 rotateMatrix = mat2( uRotation.s, uRotation.t, -uRotation.t, uRotation.s ); \n
      value = rotateMatrix * value; \n
      if(uToNext == 1.0)  \n
        value.s = uSaddleParam[2] + value.s; \n
      float delay = value.t*value.t / uSaddleParam[0] - value.s*value.s/uSaddleParam[1];\n
      vPercentage = clamp( uPercentage*2.0 - 0.5*sin(delay*1.571) - 0.5, 0.0, 1.0 ); \n
    })
  );

  const char* fragmentShader( DALI_COMPOSE_SHADER(
    varying float vPercentage;\n
    varying mediump vec2 vTexCoord;\n
    \n
    uniform sampler2D sTexture;\n
    uniform lowp vec4 uColor;\n
    uniform vec4 uTextureRect;
    \n
    float rand(vec2 co) \n
    {\n
      return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); \n
    }\n
    \n
    void main()\n
    {\n

      //Calculate the randomness
      float offsetS = rand( vTexCoord * vPercentage ) - vTexCoord.s; \n
      float offsetT = rand( vec2(vTexCoord.t*vPercentage, vTexCoord.s * vPercentage) ) - vTexCoord.t; \n
      vec2 lookupCoord = vTexCoord + vec2(offsetS, offsetT) * vPercentage; \n
      gl_FragColor = texture2D( sTexture, lookupCoord ) * uColor; \n
      gl_FragColor.a *= 1.0 - vPercentage; \n
    } )
  );

  Property::Map map;

  Property::Map customShader;

  std::string vertexShaderString;
  std::string fragmentShaderString;
  if( useHighPrecision )
  {
    vertexShaderString.reserve(strlen( prefixHighPrecision ) + strlen( vertexShader ));
    vertexShaderString.append( prefixHighPrecision );

    fragmentShaderString.reserve(strlen( prefixHighPrecision ) + strlen( fragmentShader ));
    fragmentShaderString.append( prefixHighPrecision );
  }
  else
  {
    vertexShaderString.reserve(strlen( prefixMediumPrecision ) + strlen( vertexShader ));
    vertexShaderString.append( prefixMediumPrecision );

    fragmentShaderString.reserve(strlen( prefixMediumPrecision ) + strlen( fragmentShader ));
    fragmentShaderString.append( prefixMediumPrecision );
  }

  vertexShaderString.append( vertexShader );
  fragmentShaderString.append( fragmentShader );

  customShader[ "vertexShader" ] = vertexShaderString;
  customShader[ "fragmentShader" ] = fragmentShaderString;

  customShader[ "subdivideGridX" ] = 20;
  customShader[ "subdivideGridY" ] = 20;

  customShader[ "hints" ] = "outputIsTransparent";

  map[ "shader" ] = customShader;
  return map;
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_DISSOLVE_H__
