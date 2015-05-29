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

#include <dali-toolkit/devel-api/shader-effects/blind-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace
{

const std::string STEP_PROPERTY_NAME( "uStep" );

} // namespace

BlindEffect::BlindEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
BlindEffect::BlindEffect(ShaderEffect handle)
:ShaderEffect(handle)
{
}

BlindEffect::~BlindEffect()
{
}


BlindEffect BlindEffect::New()
{
  std::string fragmentShader(
    "uniform mediump float uStep;                                                        \n"
    "void main()                                                                         \n"
    "{                                                                                   \n"
    "    mediump vec4 alphaColor;                                                        \n"
    "    mediump vec4 baseColor;                                                         \n"
    "    baseColor = texture2D( sTexture, vTexCoord);                                    \n"
    "    alphaColor = vec4(0.1,0.1,0.1,1.0);                                             \n"
    "    lowp float index = 0.0;                                                         \n"
    "    index = floor(vTexCoord.y/0.1);                                                 \n"
    "    if((vTexCoord.y < (index * 0.1 + uStep * 0.005)) && (vTexCoord.y > index * 0.1))\n"
    "    {                                                                               \n"
    "      gl_FragColor = alphaColor;                                                    \n"
    "    }                                                                               \n"
    "    else                                                                            \n"
    "    {                                                                               \n"
    "      gl_FragColor = baseColor;                                                     \n"
    "    }                                                                               \n"
    "    gl_FragColor*=uColor;                                                           \n"
    "}                                                                                   \n"
  );

  Dali::ShaderEffect shaderEffectCustom =  Dali::ShaderEffect::New(
      "",
      fragmentShader,
      Dali::GeometryType( GEOMETRY_TYPE_IMAGE ),
      ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING | ShaderEffect::HINT_GRID ));

  Dali::Toolkit::BlindEffect handle( shaderEffectCustom );

  handle.SetUniform( STEP_PROPERTY_NAME, 0.0f );

  return handle;
}

void BlindEffect::SetStep(float step)
{
  SetUniform( STEP_PROPERTY_NAME, step );
}

const std::string& BlindEffect::GetStepPropertyName() const
{
  return STEP_PROPERTY_NAME;
}

} // namespace Toolkit

} // namespace Dali
