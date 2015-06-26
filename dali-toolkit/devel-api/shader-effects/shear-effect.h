#ifndef __DALI_TOOLKIT_SHEAR_EFFECT_H__
#define __DALI_TOOLKIT_SHEAR_EFFECT_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/shader-effects/shader-effect.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Creates a new ShearEffect
 *
 * ShearEffect is a custom shader effect to achieve shear effects in Image actors
 *
 * Animatable/Constrainable uniforms:
 *  "uCenter"     - The center point of the shear effect in screen coordinates
 *  "uAngleXAxis" - The angle of the shear effect in the X axis
 *  "uAngleYAxis" - The angle of the shear effect in the Y axis
 *
 * @return A handle to a newly allocated ShaderEffect
 */
inline ShaderEffect CreateShearEffect()
{
  // append the default version
  std::string vertexShader(
      "uniform mediump  vec2  uCenter;\n"
      "uniform mediump  float uAngleXAxis;\n"
      "uniform mediump  float uAngleYAxis;\n"
      "\n"
      "void main()\n"
      "{\n"
      "mediump vec4 world = uModelView * vec4(aPosition,1.0);\n"
      "\n"
      "world.x = world.x + tan(radians(uAngleXAxis)) * (world.y - uCenter.y * world.w);\n"
      "world.y = world.y + tan(radians(uAngleYAxis)) * (world.x - uCenter.x * world.w);\n"
      "\n"
      "gl_Position = uProjection * world;\n"
      "\n"
      "vTexCoord = aTexCoord;\n"
      "}" );

  // Create the implementation, temporarily owned on stack,
  ShaderEffect shaderEffect =  Dali::ShaderEffect::New(
      vertexShader,
      "",
      ShaderEffect::GeometryHints( ShaderEffect::HINT_GRID ));


  shaderEffect.SetUniform( "uCenter", Vector2(0.0f, 0.0f), ShaderEffect::COORDINATE_TYPE_VIEWPORT_POSITION );
  shaderEffect.SetUniform( "uAngleXAxis", 0.0f);
  shaderEffect.SetUniform( "uAngleYAxis", 0.0f);

  return shaderEffect;
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHEAR_EFFECT_H__
