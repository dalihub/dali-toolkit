//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali-toolkit/public-api/shader-effects/displacement-effect.h>
namespace Dali
{

namespace Toolkit
{

namespace
{

const std::string DISPLACEMENT_EFFECT_LIGHT_DIRECTION_PROPERTY_NAME( "uLightDirection" );
const std::string DISPLACEMENT_EFFECT_AMBIENT_LIGHT_COLOR_PROPERTY_NAME( "uAmbientLightColor" );
const std::string DISPLACEMENT_EFFECT_DIFFUSE_LIGHT_COLOR_PROPERTY_NAME( "uDiffuseLightColor" );
const std::string DISPLACEMENT_EFFECT_LIGHT_MULTIPLIER_PROPERTY_NAME( "uLightMultiplier" );
const std::string DISPLACEMENT_EFFECT_STATE_PROPERTY_NAME( "uState" );
const std::string DISPLACEMENT_EFFECT_HEIGHT_SCALE_PROPERTY_NAME( "uHightScale" );
const std::string DISPLACEMENT_EFFECT_FIXED_NORMAL_PROPERTY_NAME( "uFixedNormal" );

// factors that scale the look, defaults
const Vector3 DISPLACEMENT_EFFECT_LIGHT_DIRECTION_DEFAULT = Vector3(0.0, 0.7070168f, 0.7071068f);
const Vector3 DISPLACEMENT_EFFECT_AMBIENT_LIGHT_COLOR_DEFAULT = Vector3(0.15f, 0.15f, 0.15f);
const Vector3 DISPLACEMENT_EFFECT_DIFFUSE_LIGHT_COLOR_DEFAULT = Vector3(1.0f, 1.0f, 1.0f);
const float   DISPLACEMENT_EFFECT_LIGHT_MULTIPLIER_DEFAULT = 1.0f;
const float   DISPLACEMENT_EFFECT_STATE_DEFAULT = 0.0f;
const float   DISPLACEMENT_EFFECT_HEIGHT_SCALE_DEFAULT = 0.1f;
const Vector3 DISPLACEMENT_EFFECT_FIXED_NORMAL_DEFAULT = Vector3(0.0f, 0.0f, 1.0f);

} // namespace


////////////////////////////////////////////////////
//
// Soft button shader / actor tweaking parameters
//


DisplacementEffect::DisplacementEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
DisplacementEffect::DisplacementEffect(ShaderEffect handle)
:ShaderEffect(handle)
{
}

DisplacementEffect::~DisplacementEffect()
{
}

DisplacementEffect DisplacementEffect::New(Type type)
{

  std::string fragmentSourceFixed;
  fragmentSourceFixed =  "precision mediump float;\n"
    "uniform vec3 uLightDirection;\n"
    "uniform vec3 uAmbientLightColor;\n"
    "uniform vec3 uDiffuseLightColor;\n"
    "uniform float uLightMultiplier;\n"
    "uniform float uState;\n"
    "uniform float uHightScale;\n"
    "uniform vec3 uFixedNormal;\n"

    "void main()\n"
    "{\n"
    "   vec4 col = texture2D(sTexture, vTexCoord);\n"
    // calc lighting
    "  float intensity = dot(uLightDirection, uFixedNormal);"
    "  vec3 lighting = (intensity * uDiffuseLightColor) + uAmbientLightColor;\n"
    "  lighting *= uLightMultiplier;\n"
    // output col = image * light
    "  gl_FragColor = vec4(col.rgb * lighting * uColor.rgb, col.a * uColor.a);\n"
    "}\n";



  std::string fragmentSourceDisplaced(
      "precision mediump float;\n"
      "uniform vec3 uLightDirection;\n"
      "uniform vec3 uAmbientLightColor;\n"
      "uniform vec3 uDiffuseLightColor;\n"
      "uniform float uLightMultiplier;\n"
      "uniform float uState;\n"
      "uniform float uHightScale;\n"
      "void main()\n"
      "{\n"
      "  highp vec4 displacementMap1 = texture2D(sEffect, vec2(vTexCoord.s, vTexCoord.t/2.0));\n"
      "  highp vec4 displacementMap2 = texture2D(sEffect, vec2(vTexCoord.s, 0.5+vTexCoord.t/2.0));\n"
      "  highp vec4 displacementMap = mix(displacementMap1, displacementMap2, uState);\n"

      "  vec3 normalAdjusted = normalize(displacementMap.rgb*2.0-1.0);\n"
      "  float height = uHightScale * (displacementMap.a*2.0 - 1.0);\n"
      "  vec2 displacement = vec2(0.0);\n"
      "  displacement += (vec2(0.5)-vTexCoord.st)*height;\n"
      "  vec2 newCoord = vTexCoord.st + displacement.xy;\n"

      "  vec4 col = texture2D(sTexture, newCoord);\n"
      // Y-Axis for the normal map is taken as in Y-Down format, So inverting it for GL
      "  float intensity = dot(uLightDirection, vec3(1.0,-1.0, 1.0) * normalAdjusted);"
      "  vec3 lighting = (intensity * uDiffuseLightColor) + uAmbientLightColor;\n"
      "  lighting *= uLightMultiplier;\n"
      "  vec3 color = col.rgb * lighting * uColor.rgb;\n"
      "  gl_FragColor = vec4(color, col.a * uColor.a);\n"
      "}\n");

  //////////////////////////////////////
  // Create shader effect
  //
  //

  ShaderEffect shader;
  switch(type)
  {
    case DISPLACED:
      shader = ShaderEffect::New( "", fragmentSourceDisplaced);
      break;

    case FIXED:
    default:
      shader = ShaderEffect::New( "", fragmentSourceFixed);
      break;
  }
  DisplacementEffect handle( shader );


  //////////////////////////////////////
  // Register uniform properties
  //
  //
  // factors that scale the look, defaults

  handle.SetLightDirection(DISPLACEMENT_EFFECT_LIGHT_DIRECTION_DEFAULT);
  handle.SetAmbientLightColorProperty(DISPLACEMENT_EFFECT_AMBIENT_LIGHT_COLOR_DEFAULT);
  handle.SetDiffuseLightColorProperty(DISPLACEMENT_EFFECT_DIFFUSE_LIGHT_COLOR_DEFAULT);
  handle.SetLightingMultiplierProperty(DISPLACEMENT_EFFECT_LIGHT_MULTIPLIER_DEFAULT);
  handle.SetStateProperty(DISPLACEMENT_EFFECT_STATE_DEFAULT);
  handle.SetHeightScaleProperty(DISPLACEMENT_EFFECT_HEIGHT_SCALE_DEFAULT);

  if(type == FIXED)
  {
    handle.SetFixedNormalProperty(DISPLACEMENT_EFFECT_FIXED_NORMAL_DEFAULT);
  }


  return handle;
}

const std::string& DisplacementEffect::GetLightDirectionPropertyName() const
{
  return DISPLACEMENT_EFFECT_LIGHT_DIRECTION_PROPERTY_NAME;
}

const std::string& DisplacementEffect::GetAmbientLightColorPropertyName() const
{
  return DISPLACEMENT_EFFECT_AMBIENT_LIGHT_COLOR_PROPERTY_NAME;
}

const std::string& DisplacementEffect::GetDiffuseLightColorPropertyName() const
{
  return DISPLACEMENT_EFFECT_DIFFUSE_LIGHT_COLOR_PROPERTY_NAME;
}

const std::string& DisplacementEffect::GetLightingMultiplierPropertyName() const
{
  return DISPLACEMENT_EFFECT_LIGHT_MULTIPLIER_PROPERTY_NAME;
}

const std::string& DisplacementEffect::GetStatePropertyName() const
{
  return DISPLACEMENT_EFFECT_STATE_PROPERTY_NAME;
}

const std::string& DisplacementEffect::GetHeightScalePropertyName() const
{
  return DISPLACEMENT_EFFECT_HEIGHT_SCALE_PROPERTY_NAME;
}

const std::string& DisplacementEffect::GetFixedNormalPropertyName() const
{
  return DISPLACEMENT_EFFECT_FIXED_NORMAL_PROPERTY_NAME;
}

void DisplacementEffect::SetLightDirection(const Vector3 lightDirection)
{
  SetUniform( DISPLACEMENT_EFFECT_LIGHT_DIRECTION_PROPERTY_NAME, lightDirection);
}

void DisplacementEffect::SetAmbientLightColorProperty(const Vector3 ambientLight)
{
  SetUniform( DISPLACEMENT_EFFECT_AMBIENT_LIGHT_COLOR_PROPERTY_NAME, ambientLight);
}

void DisplacementEffect::SetDiffuseLightColorProperty(const Vector3 diffuseLight)
{
  SetUniform( DISPLACEMENT_EFFECT_DIFFUSE_LIGHT_COLOR_PROPERTY_NAME, diffuseLight);
}

void DisplacementEffect::SetLightingMultiplierProperty(const float lightMultiplier)
{
  SetUniform( DISPLACEMENT_EFFECT_LIGHT_MULTIPLIER_PROPERTY_NAME, lightMultiplier);
}

void DisplacementEffect::SetStateProperty(const float state)
{
  SetUniform( DISPLACEMENT_EFFECT_STATE_PROPERTY_NAME, state);
}

void DisplacementEffect::SetHeightScaleProperty(const float heightScale)
{
  SetUniform( DISPLACEMENT_EFFECT_HEIGHT_SCALE_PROPERTY_NAME, heightScale);
}

void DisplacementEffect::SetFixedNormalProperty(const Vector3 fixedNormal)
{
  Vector3 newFixedNormal(fixedNormal);
  newFixedNormal.Normalize();

  SetUniform( DISPLACEMENT_EFFECT_FIXED_NORMAL_PROPERTY_NAME, newFixedNormal);
}

}

}

