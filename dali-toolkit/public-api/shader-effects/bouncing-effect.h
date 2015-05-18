#ifndef __DALI_TOOLKIT_SHADER_EFFECT_BOUNCING_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_BOUNCING_H__

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
 * @brief Creates a new bouncing effect
 *
 * BouncingEffect is a custom overscroll effect with two waves appearing at two sides then moving towards center and overlapping.
 *
 * Usage Example:
 *
 *  // Create the an imageActor, set shader effect, and add it to the stage
 *  ImageActor imageActor = ImageActor::New( BufferImage::New( 1, 1 ) );
 *  imageActor.SetSize(720.f,58.f);
 *  Toolkit::ShaderEffect bouncingEffect = CreateBouncingEffect( Vector4(0.f,1.f,1.f,0.5f) );
 *  imageActor.SetShaderEffect( bouncingEffect );
 *  imageActor.SetParentOrigin( ParentOrigin::CENTER );
 *  Stage::GetCurrent().Add( imageActor );
 *
 *   // Start the animation
 *   Animation animation = Animation::New(1.f);
 *   animation.AnimateTo( Property( bouncingEffect,"uProgressRate" ),
 *                        1.f, AlphaFunction::BOUNCE );
 *   animation.Play();
 *
 * Animatable/Constrainable uniforms:
 *  "uProgressRate" - The progress rate to the effect
 *
 * @param[in] color The color used on the bouncing stripe
 * @return A handle to a newly allocated ShaderEffect
 */

inline ShaderEffect CreateBouncingEffect(const Vector4& color)
{
  std::string fragmentShader = DALI_COMPOSE_SHADER(
      precision mediump float;\n
      uniform float uProgressRate;\n
      uniform vec4 uAssignedColor;\n
      void main()\n
      {\n
        float progressRate = abs(uProgressRate)*0.5;\n
        float amplitude = 0.15 - progressRate*0.15 ;\n
        float x1 = 7.5 * (vTexCoord.x - progressRate);\n
        float x2 = 7.5 * (vTexCoord.x - 1.0 + progressRate);\n
        float height1 = max(0.00001, 0.3 - amplitude * ( exp(x1) + exp(-x1) ) );\n
        float height2 = max(0.00001, 0.3 - amplitude * ( exp(x2) + exp(-x2) ) );\n
        float height3 = max(0.00001, 1.0 - 3.0 * amplitude * ( exp(x1*0.5) + exp(-x1*0.5) ) );\n
        float height4 = max(0.00001, 1.0 - 3.0 * amplitude * ( exp(x2*0.5) + exp(-x2*0.5) ) );\n
        vec4 fragColor = vec4(0.0);\n
        float y = vTexCoord.y/(height1+height2);\n
        float y2 = vTexCoord.y/max(height3,height4);\n
        float coef = max(height1,height2)*5.0/( 1.0+exp(y*12.0-6.0) );\n
        float alpha = pow( max(0.0,(1.0-y2))*(1.0-min(abs(x1),abs(x2))/5.0), 2.0);\n
        if( vTexCoord.y < 0.075 )\n
        {\n
          fragColor= mix(uAssignedColor, vec4(1.0), coef);\n
          fragColor += (vec4(1.0)-fragColor) * alpha;\n
        }\n
        else if (y2<1.0)\n
        {\n
          fragColor =vec4(1.0,1.0,1.0, alpha + (1.0-alpha)*coef);\n
          fragColor.rgb -= ( vec3(1.0)-uAssignedColor.rgb )*min(clamp(y*1.2-0.3, 0.0, 0.3),clamp(0.9-y*1.2,0.0,0.3));\n
        }\n
        fragColor.a *= 10.0*min(min(vTexCoord.x, 1.0-vTexCoord.x),0.1)*min(1.0, progressRate/0.2);\n
        gl_FragColor =  fragColor;\n
      }
  );

  ShaderEffect shaderEffect;
  shaderEffect = ShaderEffect::New( "", fragmentShader,
                                    GeometryType( GEOMETRY_TYPE_IMAGE),
                                    ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING ) );

  shaderEffect.SetUniform( "uAssignedColor", color );
  shaderEffect.SetUniform( "uProgressRate", 0.0f );

  return shaderEffect;
}


} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_BOUNCING_H__
