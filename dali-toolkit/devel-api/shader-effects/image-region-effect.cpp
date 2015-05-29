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

#include <dali-toolkit/devel-api/shader-effects/image-region-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace
{

const std::string TOP_LEFT_PROPERTY_NAME( "uTopLeft" );
const std::string BOTTOM_RIGHT_PROPERTY_NAME( "uBottomRight" );

} // namespace

ImageRegionEffect::ImageRegionEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
ImageRegionEffect::ImageRegionEffect(ShaderEffect handle)
:ShaderEffect(handle)
{
}

ImageRegionEffect::~ImageRegionEffect()
{
}


ImageRegionEffect ImageRegionEffect::New()
{
  std::string vertexShader(
    "uniform mediump vec2 uTopLeft;\n"
    "uniform mediump vec2 uBottomRight;\n"
    "void main()\n"
    "{\n"
    "  mediump vec4 position = vec4(aPosition,1.0);\n"
    "  gl_Position = uMvpMatrix * position;\n"
    // The line below is doing the same as the following commented lines:
    //"  vec2 imageSize = sTextureRect.zw - sTextureRect.xy;\n"
    //"  vec2 topLeft = sTextureRect.xy + uTopLeft * imageSize;\n"
    //"  vec2 bottomRight = sTextureRect.xy + uBottomRight * imageSize;\n"
    //"  vec2 texCoord = (aTexCoord - sTextureRect.xy) / imageSize;\n"
    //"  vTexCoord = topLeft + texCoord * ( bottomRight - topLeft );\n"
    "  vTexCoord = sTextureRect.xy + uTopLeft * ( sTextureRect.zw - sTextureRect.xy ) + ( aTexCoord - sTextureRect.xy ) * ( uBottomRight - uTopLeft );\n"
    "}\n"
  );

  Dali::ShaderEffect shaderEffectCustom = Dali::ShaderEffect::New( vertexShader, "" );

  Dali::Toolkit::ImageRegionEffect handle( shaderEffectCustom );

  handle.SetUniform( TOP_LEFT_PROPERTY_NAME, Vector2( 0.f, 0.f ) );
  handle.SetUniform( BOTTOM_RIGHT_PROPERTY_NAME, Vector2( 1.f, 1.f ) );

  return handle;
}

void ImageRegionEffect::SetTopLeft(const Vector2& point)
{
  SetUniform( TOP_LEFT_PROPERTY_NAME, point );
}

void ImageRegionEffect::SetBottomRight(const Vector2& point)
{
  SetUniform( BOTTOM_RIGHT_PROPERTY_NAME, point );
}

const std::string& ImageRegionEffect::GetTopLeftPropertyName() const
{
  return TOP_LEFT_PROPERTY_NAME;
}

const std::string& ImageRegionEffect::GetBottomRightPropertyName() const
{
  return BOTTOM_RIGHT_PROPERTY_NAME;
}

} // namespace Toolkit

} // namespace Dali
