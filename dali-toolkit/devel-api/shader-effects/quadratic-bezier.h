#ifndef __DALI_TOOLKIT_QUADRATIC_BEZIER_H__
#define __DALI_TOOLKIT_QUADRATIC_BEZIER_H__

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
 * @brief Creates a new QuadraticBezier shader effect
 *
 * QuadraticBezier is a custom shader to render quadratic bezier curves and bounded regions.
 *
 * Implementation based on the paper "Resolution Independent Curve Rendering using Programmable Graphics Hardware"
 * by Charles Loop and Jim Blinn.
 *
 * The coordinates of the control points is passed to the shader via uniforms.
 * aNormal attribute is used to pass the coefficients of each control point (xy coordinates) as well as the vertex ID (z coordinate).
 * A quadratic curve should have as normal for the first control point (0.0,0.0), (0.5,0.0) for the second and (1.0,1.0) for the third.
 * Triangles that do not contain curves should have coordinates (0.0,1.0) for each control point in order to be filled properly.
 *
 * Animatable/Constrainable uniforms:
 *  "uPoint"      - Position coordinates for the points in the curve
 *  "uColor"      - The color of the curve or bounded region
 *  "uLineWidth"  - The width of the path. Only for not filled curves
 *
 * @param[in] pointCount The maximum number of vertices
 * @param[in] filled Specify whether the the bounded region should be filled or not
 * @return A handle to a newly allocated ShaderEffect
 */
inline ShaderEffect CreateQuadraticBezier(unsigned int pointCount, bool filled)
{
  std::string vertexShader = DALI_COMPOSE_SHADER
      (
          uniform mediump vec3 uPoint[MAX_POINT_COUNT];\n
          varying highp vec2 vCoefficient;
          void main()\n
              {\n
            int vertexId = int(aNormal.z);\n
            gl_Position = uMvpMatrix * vec4(uPoint[vertexId], 1.0);\n
            vCoefficient = aNormal.xy;\n
              }\n
      );

  std::string fragmentShader;

  if( filled )
  {
    fragmentShader = DALI_COMPOSE_SHADER
        (
            varying highp vec2 vCoefficient;\n

            void main()\n
            {\n
              highp float C = (vCoefficient.x*vCoefficient.x-vCoefficient.y);\n
              highp float Cdx = dFdx(C);\n
              highp float Cdy = dFdy(C);\n

              highp float distance = float(C / sqrt(Cdx*Cdx + Cdy*Cdy));\n

              gl_FragColor = uColor;\n
              highp float alpha = 0.5 - distance;\n
              if( alpha < 0.0 )\n
              discard;\n

              gl_FragColor.w = alpha;\n
            }\n
        );
  }
  else
  {
    fragmentShader = DALI_COMPOSE_SHADER
        (
            varying highp vec2 vCoefficient;\n
            uniform lowp float uLineWidth;\n

            void main()\n
            {\n
              highp float C = (vCoefficient.x*vCoefficient.x-vCoefficient.y);\n
              highp float Cdx = dFdx(C);\n
              highp float Cdy = dFdy(C);\n
              highp float distance = abs(float(C / sqrt(Cdx*Cdx + Cdy*Cdy)));\n
              gl_FragColor = uColor*(uLineWidth-distance);\n
            }\n
        );
  }

  std::ostringstream vertexShaderPrefix;
  vertexShaderPrefix << "#define MAX_POINT_COUNT "<< pointCount << "\n";

  Dali::ShaderEffect shaderEffect = Dali::ShaderEffect::NewWithPrefix(
      vertexShaderPrefix.str(),vertexShader,
      "#extension GL_OES_standard_derivatives:enable\n", fragmentShader );

  //Set default uniform values
  shaderEffect.SetUniform( "uColor", Vector4(1.0f,1.0f,1.0f,1.0f) );
  if( !filled )
  {
    //Set default line widht to 1 pixel
    shaderEffect.SetUniform( "uLineWidth", 1.0f );
  }

  return shaderEffect;
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_IMAGE_REGION_EFFECT_H__
