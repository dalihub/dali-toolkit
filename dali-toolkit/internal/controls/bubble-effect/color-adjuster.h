#ifndef __DALI_TOOLKIT_INTERNAL_COLOR_ADJUSTER_H_
#define __DALI_TOOLKIT_INTERNAL_COLOR_ADJUSTER_H_

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
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/shader-effects/shader-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
* Creates a new ColorAdjuster effect.
* ColorAdjuster is a custom shader effect to adjust the image color in HSV space.
* @param[in] hsvDelta The color difference to apply to the HSV channel.
* @param[in] ignoreAlpha If true, the result color will be opaque even though source has alpha value
* @return A handle to a newly allocated Dali resource.
*/
inline ShaderEffect CreateColorAdjuster( const Vector3& hsvDelta, bool ignoreAlpha = false )
{
  std::string fragmentShader = DALI_COMPOSE_SHADER(
    precision highp float;\n
    uniform vec3 uHSVDelta;\n
    uniform float uIgnoreAlpha;\n
    float rand(vec2 co) \n
    {\n
      return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); \n}
    \n
    vec3 rgb2hsv(vec3 c)\n
    {\n
      vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);\n
      vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));\n
      vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));\n
      \n
      float d = q.x - min(q.w, q.y);\n
      float e = 1.0e-10;\n
      return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);\n
    }\n
    vec3 hsv2rgb(vec3 c)\n
    {\n
      vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n
      vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\n
      return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\n
    }\n
    void main() {\n
      vec4 color = texture2D(sTexture, vTexCoord); \n
      vec3 hsvColor = rgb2hsv( color.rgb );\n
      // modify the hsv Value
      hsvColor += uHSVDelta * rand(vTexCoord); \n
      // if the new vale exceeds one, then decrease it
      hsvColor -= max(hsvColor*2.0 - vec3(2.0), 0.0);\n
      // if the new vale drops below zero, then increase it
      hsvColor -= min(hsvColor*2.0, 0.0);\n
      color.rgb = hsv2rgb( hsvColor ); \n
      // uIgnoreAlpha decide the result alpha will be 1.0 or source's alpha
      color.a += uIgnoreAlpha;\n
      gl_FragColor = color; \n
    }\n
  );

  ShaderEffect shaderEffect = ShaderEffect::New("", fragmentShader);
  shaderEffect.SetUniform( "uHSVDelta", hsvDelta );
  shaderEffect.SetUniform( "uIgnoreAlpha", ignoreAlpha?1.0f:0.0f );

  return shaderEffect;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_COLOR_ADJUSTER_H_ */
