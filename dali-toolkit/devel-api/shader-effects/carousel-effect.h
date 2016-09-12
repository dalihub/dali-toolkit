#ifndef __DALI_TOOLKIT_CAROUSEL_EFFECT_H__
#define __DALI_TOOLKIT_CAROUSEL_EFFECT_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/shader-effects/shader-effect.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Creates a new Carousel effect
 *
 * CarouselEffect is a custom shader effect to achieve Carousel effects in actors
 *
 * A Carousel has a Radius property which can be +ve (appear as if viewing from the outside of
 * a cylinder/sphere)
 * or -ve (appear as if viewing from the inside of a cylinder/sphere).
 *
 * It can be a horizontal or vertical (cylindrical) or both (spherical). The AnglePerUnit
 * property provides this functionality as a Vector2.
 *
 * Finally, the carousel's center position can be specified as a Screen coordinate (top-left being
 * the origin).
 *
 * Animatable/Constrainable uniforms:
 *  "uRadius"       - The radius of the Carousel effect. A positive Radius will bend toward the camera,
 *                    while a negative Radius will bend away from the camera.
 *  "uAnglePerUnit" - The angle deviation of Carousel in degrees per geometric unit for each axis
                      For example if you wish for the horizontal angle deviation to vary from +/- 10
                      degrees, then a Value of 20.0f / stageWidth for the X component should be specified.
 *  "uCenter"       - The center point of the carousel (in screen coordinates) this is where the peek of the carousel should appear.
 *                    Defaults value is top-left corner (0.0f, 0.0f).
 *
 * @return A handle to a newly allocated ShaderEffect
 */
inline ShaderEffect CreateCarouselEffect()
{
  // append the default version
    std::string vertexShader(
                "uniform float uRadius;\n"
                "uniform mediump vec2 uCenter;\n"
                "uniform mediump vec2 uAnglePerUnit;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    mediump vec4 world = uModelView * vec4(aPosition, 1.0);\n"
                "    mediump vec2 d = (world.xy - uCenter) * uAnglePerUnit;\n"
                "    mediump float a = length(d);\n"
                "    mediump float cs = cos(radians(a));\n"
                "    world.z -= cs * uRadius;\n"
                "    gl_Position = uProjection * world;\n"
                "    \n"
                "    vTexCoord = mix( sTextureRect.xy, sTextureRect.zw, aTexCoord );\n"
                "}\n");

    ShaderEffect shaderEffect = ShaderEffect::New(
        vertexShader,
        "",
        ShaderEffect::GeometryHints( ShaderEffect::HINT_GRID | ShaderEffect::HINT_DEPTH_BUFFER ));


    shaderEffect.SetUniform( "uRadius", 0.0f );
    shaderEffect.SetUniform( "uCenter", Vector2( 0.0f, 0.0f ) );
    shaderEffect.SetUniform( "uAnglePerUnit", Vector2( 0.0f, 0.0f ) );

    return shaderEffect;
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_CAROUSEL_EFFECT_H__
