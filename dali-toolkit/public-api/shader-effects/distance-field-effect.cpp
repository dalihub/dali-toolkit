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

#include <dali-toolkit/public-api/shader-effects/distance-field-effect.h>

#define STRINGIFY(...) #__VA_ARGS__

namespace Dali
{

namespace Toolkit
{

namespace
{
// generic uniforms
const std::string COLOR_PROPERTY_NAME( "uColor" );
const std::string SMOOTHING_PROPERTY_NAME( "uSmoothing" );

// outline uniforms
const std::string OUTLINE_ENABLE_PROPERTY_NAME( "uDoOutline" );
const std::string OUTLINECOLOR_PROPERTY_NAME( "uOutlineColor" );
const std::string OUTLINE_SIZE_PROPERTY_NAME( "uOutlineParams" );

// glow related
const std::string GLOW_ENABLE_PROPERTY_NAME( "uDoGlow" );
const std::string GLOW_COLOR_PROPERTY_NAME( "uGlowColor" );
const std::string GLOW_BOUNDARY_PROPERTY_NAME( "uGlowBoundary" );

// shadow related
const std::string SHADOW_ENABLE_PROPERTY_NAME( "uDoShadow" );
const std::string SHADOW_COLOR_PROPERTY_NAME( "uShadowColor" );
const std::string SHADOW_OFFSET_PROPERTY_NAME( "uShadowOffset" );
} // namespace

DistanceFieldEffect::DistanceFieldEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
DistanceFieldEffect::DistanceFieldEffect(ShaderEffect handle)
:ShaderEffect(handle)
{
}

DistanceFieldEffect::~DistanceFieldEffect()
{
}

DistanceFieldEffect DistanceFieldEffect::New()
{
  std::string fragmentShaderPrefix(
      "#extension GL_OES_standard_derivatives : enable\n"
      "\n"
      );

  std::string fragmentShader(
      "uniform mediump float uSmoothing;\n"
      "uniform mediump float uGlowBoundary;\n"
      "uniform mediump vec2  uOutlineParams;\n"
      "uniform lowp    vec4  uOutlineColor;\n"
      "uniform lowp    vec4  uShadowColor;\n"
      "uniform mediump vec2  uShadowOffset;\n"
      "uniform lowp    vec4  uGlowColor;\n"
      "uniform lowp    float uDoOutline;\n"
      "uniform lowp    float uDoShadow;\n"
      "uniform lowp    float uDoGlow;\n"
      "\n"
      "void main()\n"
      "{\n"
      "  // sample distance field\n"
      "  mediump float distance = texture2D(sTexture, vTexCoord).a;\n"
      "  mediump float smoothWidth = fwidth(distance);\n"
      "  mediump float alphaFactor = smoothstep(uSmoothing - smoothWidth, uSmoothing + smoothWidth, distance);\n"
      "  lowp    vec4  color;\n"
      "  if (uDoShadow == 0.0)\n"
      "  {\n"
      "    mediump float alpha = uColor.a * alphaFactor;\n"
      "    lowp    vec4  rgb = uColor;\n"
      "\n"
      "    if (uDoOutline > 0.0)\n"
      "    {\n"
      "      mediump float outlineWidth = uOutlineParams[1] + smoothWidth;\n"
      "      mediump float outlineBlend = smoothstep(uOutlineParams[0] - outlineWidth, uOutlineParams[0] + outlineWidth, distance);\n"
      "      alpha = smoothstep(uSmoothing - smoothWidth, uSmoothing + smoothWidth, distance);\n"
      "      rgb = mix(uOutlineColor, uColor, outlineBlend);\n"
      "    }\n"
      "\n"
      "    if (uDoGlow > 0.0)\n"
      "    {\n"
      "      rgb = mix(uGlowColor, rgb, alphaFactor);\n"
      "      alpha = smoothstep(uGlowBoundary, uSmoothing, distance);\n"
      "    }\n"
      "\n"
      "    // set fragment color\n"
      "    color = vec4(rgb.rgb, alpha);\n"
      "  }\n"
      "\n"
      "  else // (uDoShadow > 0.0)\n"
      "  {\n"
      "    float shadowDistance = texture2D(sTexture, vTexCoord - uShadowOffset).a;\n"
      "    mediump float inText = alphaFactor;\n"
      "    mediump float inShadow = smoothstep(uSmoothing - smoothWidth, uSmoothing + smoothWidth, shadowDistance);\n"
      "\n"
      "    // inside object, outside shadow\n"
      "    if (inText == 1.0)\n"
      "    {\n"
      "      color = uColor;\n"
      "    }\n"
      "    // inside object, outside shadow\n"
      "    else if ((inText != 0.0) && (inShadow == 0.0))\n"
      "    {\n"
      "      color = uColor;\n"
      "      color.a *= inText;\n"
      "    }\n"
      "    // outside object, completely inside shadow\n"
      "    else if ((inText == 0.0) && (inShadow == 1.0))\n"
      "    {\n"
      "      color = uShadowColor;\n"
      "    }\n"
      "    // inside object, completely inside shadow\n"
      "    else if ((inText != 0.0) && (inShadow == 1.0))\n"
      "    {\n"
      "      color = mix(uShadowColor, uColor, inText);\n"
      "      color.a = uShadowColor.a;\n"
      "    }\n"
      "    // inside object, inside shadow's border\n"
      "    else if ((inText != 0.0) && (inShadow != 0.0))\n"
      "    {\n"
      "      color = mix(uShadowColor, uColor, inText);\n"
      "      color.a *= max(inText, inShadow);\n"
      "    }\n"
      "    // inside shadow's border\n"
      "    else if (inShadow != 0.0)\n"
      "    {\n"
      "      color = uShadowColor;\n"
      "      color.a *= inShadow;\n"
      "    }\n"
      "    // outside shadow and object\n"
      "    else \n"
      "    {\n"
      "      color.a = 0.0;\n"
      "    }\n"
      "\n"
      "  }\n"
      "\n"
      "  gl_FragColor = color;\n"
      "\n"
      "}\n"
      );

  // Create the implementation, temporarily owned on stack
  Dali::ShaderEffect shaderEffect =  Dali::ShaderEffect::NewWithPrefix("", "",
                                                                       fragmentShaderPrefix, fragmentShader,
                                                                       Dali::GeometryType( GEOMETRY_TYPE_IMAGE ),
                                                                       ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING));

  /* Pass ownership to DistanceFieldEffect through overloaded constructor, So that it now has access to the
     Dali::ShaderEffect implementation */

  // TODO: move default values to... Constants?
  Dali::Toolkit::DistanceFieldEffect handle( shaderEffect );

  handle.SetSmoothingEdge(0.5f);
  handle.SetOutlineColor(Color::BLACK);
  handle.SetOutlineParams(Vector2(0.51f, 0.0f));
  handle.SetGlowBoundary(0.4f);
  handle.SetGlowColor(Color::GREEN);
  handle.SetShadowColor(Vector4(0.0f, 0.0f, 0.0f, 0.4f));

  // TODO: find a way to set the shadow offset in texel coordinates instead of UVs.
  handle.SetShadowOffset(Vector2(0.05f, 0.05f));

  // Default:
  handle.SetOutline(false);
  handle.SetGlow(false);
  handle.SetShadow(false);

  return handle;

}

void DistanceFieldEffect::SetGlowColor(const Vector4& color)
{
  SetUniform(GLOW_COLOR_PROPERTY_NAME, color);
}

void DistanceFieldEffect::SetGlow(bool glowEnable)
{
  const float a = glowEnable ? 1.0f : 0.0f;
  SetUniform(GLOW_ENABLE_PROPERTY_NAME, a);
}

void DistanceFieldEffect::SetGlowBoundary(float glowBoundary)
{
  SetUniform(GLOW_BOUNDARY_PROPERTY_NAME, glowBoundary);
}

void DistanceFieldEffect::SetOutline(bool outlineEnable)
{
  const float a = outlineEnable ? 1.0f : 0.0f;
  SetUniform(OUTLINE_ENABLE_PROPERTY_NAME, a);
}

void DistanceFieldEffect::SetOutlineColor(const Vector4& color)
{
  SetUniform(OUTLINECOLOR_PROPERTY_NAME, color);
}

void DistanceFieldEffect::SetOutlineParams(const Vector2& outlineParams)
{
  SetUniform(OUTLINE_SIZE_PROPERTY_NAME, outlineParams);
}

void DistanceFieldEffect::SetShadow(bool shadowEnable)
{
  if (shadowEnable)
  {
    SetGlow(false);
    SetOutline(false);
  }

  const float a = shadowEnable ? 1.0f : 0.0f;
  SetUniform(SHADOW_ENABLE_PROPERTY_NAME, a);
}

void DistanceFieldEffect::SetShadowColor(const Vector4& color)
{
  SetUniform(SHADOW_COLOR_PROPERTY_NAME, color);
}

void DistanceFieldEffect::SetShadowOffset(const Vector2& offset)
{
  SetUniform(SHADOW_OFFSET_PROPERTY_NAME, offset);
}

void DistanceFieldEffect::SetSmoothingEdge(const float smoothing)
{
  SetUniform(SMOOTHING_PROPERTY_NAME, smoothing);
}

const std::string& DistanceFieldEffect::GetColorPropertyName() const
{
  return COLOR_PROPERTY_NAME;
}

const std::string& DistanceFieldEffect::GetOutlineColorPropertyName() const
{
  return OUTLINECOLOR_PROPERTY_NAME;
}

const std::string& DistanceFieldEffect::GetShadowColorPropertyName() const
{
  return SHADOW_COLOR_PROPERTY_NAME;
}

const std::string& DistanceFieldEffect::GetShadowOffsetPropertyName() const
{
  return SHADOW_OFFSET_PROPERTY_NAME;
}

const std::string& DistanceFieldEffect::GetGlowColorPropertyName() const
{
  return GLOW_COLOR_PROPERTY_NAME;
}

const std::string& DistanceFieldEffect::GetGlowBoundaryPropertyName() const
{
  return GLOW_BOUNDARY_PROPERTY_NAME;
}

const std::string& DistanceFieldEffect::GetOutlineSizePropertyName() const
{
  return OUTLINE_SIZE_PROPERTY_NAME;
}

const std::string& DistanceFieldEffect::GetOutlineEnablePropertyName() const
{
  return OUTLINE_ENABLE_PROPERTY_NAME;
}

const std::string& DistanceFieldEffect::GetGlowEnablePropertyName() const
{
  return GLOW_ENABLE_PROPERTY_NAME;
}

const std::string& DistanceFieldEffect::GetShadowEnablePropertyName() const
{
  return SHADOW_ENABLE_PROPERTY_NAME;
}

const std::string& DistanceFieldEffect::GetSmoothingPropertyName() const
{
  return SMOOTHING_PROPERTY_NAME;
}

} // namespace Toolkit

} // namespace Dali
