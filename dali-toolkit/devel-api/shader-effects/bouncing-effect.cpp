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

#include <dali-toolkit/devel-api/shader-effects/bouncing-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace
{

#define MAKE_STRING(A)#A

const std::string PROGRESS_RATE_PROPERTY_NAME( "uProgressRate" );

} // namespace

BouncingEffect::BouncingEffect()
{
}

BouncingEffect::BouncingEffect( ShaderEffect handle )
:ShaderEffect( handle )
{
}

BouncingEffect::~BouncingEffect()
{
}

BouncingEffect BouncingEffect::New( const Vector4& color )
{
  std::string fragmentShader = MAKE_STRING(
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
  BouncingEffect handle( shaderEffect );

  handle.SetUniform( "uAssignedColor", color );
  handle.SetProgressRate( 0.f );

  return handle;
}

void BouncingEffect::SetProgressRate( float progress )
{
  SetUniform( PROGRESS_RATE_PROPERTY_NAME, progress );
}

const std::string& BouncingEffect::GetProgressRatePropertyName() const
{
  return PROGRESS_RATE_PROPERTY_NAME;
}

} // namespace Toolkit

} // namespace Dali
