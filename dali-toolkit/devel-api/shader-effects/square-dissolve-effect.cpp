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

#include <dali-toolkit/devel-api/shader-effects/square-dissolve-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace
{

const std::string STEP_PROPERTY_NAME( "uStep" );
const std::string ROWS_PROPERTY_NAME( "uRows" );
const std::string COLUMNS_PROPERTY_NAME( "uColumns" );
const std::string TEXSIZE_PROPERTY_NAME( "texSize" );

} // namespace

SquareDissolveEffect::SquareDissolveEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
SquareDissolveEffect::SquareDissolveEffect(ShaderEffect handle)
:ShaderEffect(handle)
{
}

SquareDissolveEffect::~SquareDissolveEffect()
{
}

SquareDissolveEffect SquareDissolveEffect::New()
{
  // variable "uStep" range scope : [0.0, 1.0]
  std::string fragmentShader(
      "uniform  mediump vec2   texSize;\n"
      "uniform  mediump float  uStep;\n"
      "uniform  mediump float  uRows;\n"
      "uniform  mediump float  uColumns;\n"
      "void main()\n"
      "{\n"
      "  mediump vec2 mosaicSize = vec2(1.0 / uRows, 1.0 / uColumns);                               \n"
      "  mediump vec2 intXY = vec2(vTexCoord.x * texSize.x, vTexCoord.y * texSize.y);               \n"
      "  mediump vec2 XYMosaic = vec2(floor(intXY.x / mosaicSize.x) * mosaicSize.x, floor(intXY.y / mosaicSize.y) * mosaicSize.y); \n"
      "  mediump vec2 UVMosaic = vec2(XYMosaic.x /texSize.x, XYMosaic.y / texSize.y);               \n"
      "  mediump vec4 noiseVec = texture2D(sEffect, UVMosaic);                                      \n"
      "  mediump float intensity = (noiseVec[0] + noiseVec[1] + noiseVec[2] + noiseVec[3]) / 4.0;   \n"
      "  if(intensity < uStep)                                                              \n"
      "    gl_FragColor = vec4(0.1, 0.1, 0.1, 1.0);                                         \n"
      "  else                                                                               \n"
      "    gl_FragColor = texture2D(sTexture, vTexCoord);                                   \n"
      "  gl_FragColor *= uColor;                                                            \n"
      "}                                                                                    \n" );

  // Create the implementation, temporarily owned on stack
  Dali::ShaderEffect shaderEffectCustom =  Dali::ShaderEffect::New(
      "",
      fragmentShader,
      GeometryType( GEOMETRY_TYPE_IMAGE ),
      ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING | ShaderEffect::HINT_GRID ));

  /* Pass ownership to SquareDissolveEffect through overloaded constructor, So that it now has access to the
     Dali::ShaderEffect implementation */
  Dali::Toolkit::SquareDissolveEffect handle( shaderEffectCustom );

  handle.SetUniform( TEXSIZE_PROPERTY_NAME, Vector2(1.0f, 1.0f) );//COORDINATE_TYPE_DEFAULT
  handle.SetUniform( STEP_PROPERTY_NAME, 0.1f);
  handle.SetUniform( ROWS_PROPERTY_NAME, 25.0f);
  handle.SetUniform( COLUMNS_PROPERTY_NAME, 25.0f);
  return handle;
}

void SquareDissolveEffect::SetStep( float step )
{
  SetUniform( STEP_PROPERTY_NAME, step );
}

void SquareDissolveEffect::SetRows(float rows)
{
  SetUniform(ROWS_PROPERTY_NAME, rows);
}

void SquareDissolveEffect::SetColumns(float columns)
{
  SetUniform(COLUMNS_PROPERTY_NAME, columns);
}

void SquareDissolveEffect::SetTextureSize(const Vector2& textureSize)
{
  SetUniform(TEXSIZE_PROPERTY_NAME, textureSize);
}

const std::string& SquareDissolveEffect::GetStepPropertyName() const
{
  return STEP_PROPERTY_NAME;
}

const std::string& SquareDissolveEffect::GetRowsPropertyName() const
{
  return ROWS_PROPERTY_NAME;
}

const std::string& SquareDissolveEffect::GetColumnsPropertyName() const
{
  return COLUMNS_PROPERTY_NAME;
}

const std::string& SquareDissolveEffect::GetTexSizePropertyName() const
{
  return TEXSIZE_PROPERTY_NAME;
}

} // namespace Toolkit

} // namespace Dali
