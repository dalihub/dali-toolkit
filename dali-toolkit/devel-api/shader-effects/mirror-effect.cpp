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

#include <dali-toolkit/devel-api/shader-effects/mirror-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace
{

const std::string DEPTH_PROPERTY_NAME( "uDepth" );
const std::string ALPHA_PROPERTY_NAME( "uAlpha" );

} // namespace

MirrorEffect::MirrorEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
MirrorEffect::MirrorEffect(ShaderEffect handle)
:ShaderEffect(handle)
{
}

MirrorEffect::~MirrorEffect()
{
}

MirrorEffect MirrorEffect::New()
{

  std::string vertexShader(
      "void main()                                  \n"
      "{                                            \n"
      "  mediump vec3 pos = aPosition;              \n"
      "  pos.y = pos.y * 3.0;                       \n"
      "  mediump vec4 world = uModelView * vec4(pos,1.0); \n"
      "  gl_Position = uProjection * world;         \n"
      "  vTexCoord = aTexCoord;                     \n"
      "}                                            \n" );

  std::string fragmentShader(
      "uniform  mediump float  uDepth;              \n"
      "uniform  mediump float  uAlpha;              \n"
      "void main()                                  \n"
      "{                                            \n"
      " if(vTexCoord.y < 1.0 / 3.0)                 \n"
      " {                                           \n"
      "   gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);  \n"
      " }                                           \n"
      " else if(vTexCoord.y < 2.0 / 3.0)            \n"
      " {                                           \n"
      "   gl_FragColor = texture2D(sTexture, vec2(vTexCoord.x, vTexCoord.y * 3.0 - 1.0)) * uColor;    \n"
      "   gl_FragColor.a *= uAlpha;                 \n"
      " }                                           \n"
      " else                                        \n"
      " {                                           \n"
      "   highp float darkness = 3.0 - vTexCoord.y * 3.0;                                                   \n"
      "   darkness = (1.0 - 1.0 / uDepth + darkness * 1.0/ uDepth) * 0.65;                            \n"
      "   highp vec4 color = texture2D(sTexture, vec2(vTexCoord.x, -vTexCoord.y *3.0 + 3.0)) * uColor;      \n"
      "   color.a *= uAlpha;                                                                          \n"
      "   gl_FragColor = color * vec4(darkness, darkness, darkness, darkness);                        \n"
      " }                                           \n"
      "}                                            \n" );

  // Create the implementation, temporarily owned on stack,
  Dali::ShaderEffect shaderEffectCustom =  Dali::ShaderEffect::New(
      vertexShader,
      fragmentShader,
      GeometryType( GEOMETRY_TYPE_IMAGE ),
      ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING ));

  /* Pass ownership to MirrorEffect through overloaded constructor, So that it now has access to the
     Dali::ShaderEffect implementation */
  Dali::Toolkit::MirrorEffect handle( shaderEffectCustom );
  handle.SetUniform(ALPHA_PROPERTY_NAME, 1.0f);
  handle.SetUniform(DEPTH_PROPERTY_NAME, 0.5f);
  return handle;
}

void MirrorEffect::SetDepth( float depth )
{
  SetUniform( DEPTH_PROPERTY_NAME, depth );
}

void MirrorEffect::SetAlpha( float alpha )
{
  SetUniform( ALPHA_PROPERTY_NAME, alpha );
}

const std::string& MirrorEffect::GetDepthPropertyName() const
{
  return DEPTH_PROPERTY_NAME;
}

const std::string& MirrorEffect::GetAlphaPropertyName() const
{
  return ALPHA_PROPERTY_NAME;
}

} // namespace Toolkit

} // namespace Dali
