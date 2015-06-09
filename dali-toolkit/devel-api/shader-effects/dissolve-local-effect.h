#ifndef __DALI_TOOLKIT_SHADER_EFFECT_LOCAL_DISSOLVE_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_LOCAL_DISSOLVE_H__

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
#include <dali/public-api/shader-effects/shader-effect.h>
#include <sstream>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Create a new DissolveLocalEffect
 *
 * DissolveLocalEffect is a custom shader effect to achieve Dissolve effects in multiple small areas of Image actors
 *
 * Animatable/Constrainable uniforms:
 *  "uTransparency"
 *  "uCenter"     - The center positions of each dimples
 *  "uRadius"     - The propagation radius of each dimple
 *  "uPercentage" - The distortion applied to the effect texture. A value of zero means no distortion
 *
 * @param[in] numberOfDimples The number of dimples
 * @return A handle to a newly allocated ShaderEffect
 */
inline ShaderEffect CreateDissolveLocalEffect( unsigned int numberOfDimples )
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

  ShaderEffect shaderEffect = ShaderEffect::New(
      vertexShaderStringStream.str(), fragmentShader,
      ShaderEffect::GeometryHints( ShaderEffect::HINT_GRID | ShaderEffect::HINT_BLENDING ) );

  //Register uniform properties
  std::ostringstream oss;
  for( unsigned int i = 0; i < numberOfDimples; i++ )
  {
    oss.str("");
    oss<< "uCenter["<< i << "]";
    shaderEffect.SetUniform(oss.str(), Vector2(0.f,0.f));

    oss.str("");
    oss<< "uRadius["<< i << "]";
    shaderEffect.SetUniform(oss.str(), 0.f);

    oss.str("");
    oss<< "uPercentage["<< i << "]";
    shaderEffect.SetUniform( oss.str(), 0.f );
  }

  shaderEffect.SetProperty( ShaderEffect::Property::GRID_DENSITY, Dali::Property::Value(5.f) );
  shaderEffect.SetUniform( "uTransparency", 0.5f );

  return shaderEffect;
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_LOCAL_DISSOLVE_H__
