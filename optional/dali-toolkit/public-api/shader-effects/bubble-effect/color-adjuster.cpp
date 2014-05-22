/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include "color-adjuster.h"

namespace Dali
{

namespace Toolkit
{
namespace
{
  const std::string HSVDELTA_PROPERTY_NAME("uHSVDelta");
}
ColorAdjuster::ColorAdjuster()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
ColorAdjuster::ColorAdjuster( ShaderEffect handle )
: ShaderEffect( handle )
{
}

ColorAdjuster::~ColorAdjuster()
{
}

ColorAdjuster ColorAdjuster::New( const Vector3& hsvDelta, bool ignoreAlpha )
{
  std::string fragmentShader(
  "  precision highp float;\n"
  "  uniform vec3 uHSVDelta;\n"
  "  uniform float uIgnoreAlpha;\n"
  "  float rand(vec2 co) \n"
  "  {\n"
  "    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); \n"
  "  }\n"
  "  vec3 rgb2hsv(vec3 c)\n"
  "  {\n"
  "    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);\n"
  "    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));\n"
  "    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));\n"
  "    \n"
  "    float d = q.x - min(q.w, q.y);\n"
  "    float e = 1.0e-10;\n"
  "    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);\n"
  "  }\n"
  "  vec3 hsv2rgb(vec3 c)\n"
  "  {\n"
  "   vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n"
  "   vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\n"
  "   return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\n"
  "  }\n"
  "  void main() {\n"
  "    vec4 color = texture2D(sTexture, vTexCoord); \n"
  "    vec3 hsvColor = rgb2hsv( color.rgb );\n"
  // modify the hsv Value
  "    hsvColor += uHSVDelta * rand(vTexCoord); \n"
  // if the new vale exceeds one, then decrease it
  "    hsvColor -= max(hsvColor*2.0 - vec3(2.0), 0.0);\n"
  // if the new vale drops below zero, then increase it
  "    hsvColor -= min(hsvColor*2.0, 0.0);\n"
  "    color.rgb = hsv2rgb( hsvColor ); \n"
  // uIgnoreAlpha decide the result alpha will be 1.0 or source's alpha
  "    color.a = clamp(color.a + uIgnoreAlpha, 0.0, 1.0);\n"
  "    gl_FragColor = color; \n"
  "  }\n");

  ShaderEffect effect = ShaderEffect::New("", fragmentShader);
  ColorAdjuster handle( effect );
  handle.SetUniform( "uHSVDelta", hsvDelta );
  handle.SetUniform( "uIgnoreAlpha", ignoreAlpha?1.0f:0.0f );
  return handle;
}

std::string ColorAdjuster::GetHsvDeltaPropertyName() const
{
  return HSVDELTA_PROPERTY_NAME;
}

} // namespace Toolkit

} // namespace Dali
