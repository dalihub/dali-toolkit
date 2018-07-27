/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
 */

#include <dali/public-api/rendering/shader.h>
#include <dali-toolkit/devel-api/shader-effects/dissolve-effect.h>
#include <dali-toolkit/devel-api/builder/base64-encoding.h>
#include <dali-toolkit/devel-api/graphics/builtin-shader-extern-gen.h>

namespace Dali
{

namespace Toolkit
{

void DissolveEffectSetCentralLine( Actor& actor, const Vector2& position, const Vector2& displacement, float initialProgress )
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

Property::Map CreateDissolveEffect( bool useHighPrecision )
{
  Property::Map map;
  Property::Map customShader;

  Property::Value vertexShader;
  Property::Value fragmentShader;
  EncodeBase64PropertyData(vertexShader, GraphicsGetBuiltinShader("SHADER_DISSOLVE_EFFECT_VERT"));
  EncodeBase64PropertyData(fragmentShader, GraphicsGetBuiltinShader("SHADER_DISSOLVE_EFFECT_FRAG"));

  customShader[ Visual::Shader::Property::VERTEX_SHADER ] = vertexShader;
  customShader[ Visual::Shader::Property::FRAGMENT_SHADER ] = fragmentShader;

  customShader[ Visual::Shader::Property::SUBDIVIDE_GRID_X ] = 20;
  customShader[ Visual::Shader::Property::SUBDIVIDE_GRID_Y ] = 20;

  customShader[ Visual::Shader::Property::HINTS ] = Shader::Hint::OUTPUT_IS_TRANSPARENT;

  map[ Toolkit::Visual::Property::SHADER ] = customShader;
  return map;
}

} // namespace Toolkit

} // namespace Dali
