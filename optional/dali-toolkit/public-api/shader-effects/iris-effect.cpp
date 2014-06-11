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

#include <dali-toolkit/public-api/shader-effects/iris-effect.h>
namespace Dali
{

namespace Toolkit
{

namespace
{
const std::string RADIUS_PROPERTY_NAME( "uRadius" );
const std::string CENTER_PROPERTY_NAME( "uCenter" );
const std::string BLEND_FACTOR_PROPERTY_NAME( "uBlendFactor" );
} // namespace

IrisEffect::IrisEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
IrisEffect::IrisEffect(ShaderEffect handle)
:ShaderEffect(handle)
{
}

IrisEffect::~IrisEffect()
{
}

IrisEffect IrisEffect::New()
{
  // append the default version
  std::string vertexShader(
              "uniform mediump vec2 uCenter;\n"
              "varying vec2 vRelativePosition;\n"
              "\n"
              "void main()\n"
              "{\n"
              "    vec4 world = uModelView * vec4(aPosition,1.0);\n"
              "    gl_Position = uProjection * world;\n"
              "    \n"
              "    vTexCoord = aTexCoord;\n"
              "    vRelativePosition = aTexCoord - uCenter;\n"
              "}\n");

  std::string fragmentShader(
              "uniform float uRadius;                                                           \n"
              "uniform float uBlendFactor;                                                      \n"
              "varying vec2 vRelativePosition;                                                  \n"
              "void main()                                                                      \n"
              "{                                                                                \n"
              "   float delta = (length(vRelativePosition) - uRadius);                          \n"
              "   delta = clamp(0.0 - delta * uBlendFactor, 0.0, 1.0);                          \n"
              "   gl_FragColor = texture2D(sTexture, vTexCoord) * uColor;                       \n"
              "   gl_FragColor.a *= delta;                                                      \n"
              "}                                                                                \n"
              );

  Dali::ShaderEffect shaderEffectCustom =  Dali::ShaderEffect::New(
      vertexShader,
      fragmentShader,
      GeometryType( GEOMETRY_TYPE_IMAGE ),
      ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING ));

  Dali::Toolkit::IrisEffect handle( shaderEffectCustom );
  handle.SetUniform( RADIUS_PROPERTY_NAME, 0.0f );
  handle.SetUniform( BLEND_FACTOR_PROPERTY_NAME, 100.0f );
  handle.SetUniform( CENTER_PROPERTY_NAME, Vector2(0.5f, 0.5f) );
  return handle;
}

void IrisEffect::SetRadius( float radius )
{
  SetUniform( RADIUS_PROPERTY_NAME, radius );
}

void IrisEffect::SetBlendFactor(float value )
{
  SetUniform( BLEND_FACTOR_PROPERTY_NAME, value );
}

void IrisEffect::SetCenter( const Vector2& center )
{
  SetUniform( CENTER_PROPERTY_NAME, center );
}

const std::string& IrisEffect::GetRadiusPropertyName() const
{
  return RADIUS_PROPERTY_NAME;
}

const std::string& IrisEffect::GetBlendFactorPropertyName() const
{
  return BLEND_FACTOR_PROPERTY_NAME;
}

const std::string& IrisEffect::GetCenterPropertyName() const
{
  return CENTER_PROPERTY_NAME;
}


}
}

