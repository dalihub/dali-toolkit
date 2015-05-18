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

//CLASS HEADER
#include <dali-toolkit/devel-api/shader-effects/quadratic-bezier.h>

//EXTERNAL HEADERS
#include <sstream>

namespace Dali
{

namespace Toolkit
{

namespace
{
const char* POINT_PROPERTY_NAME( "uPoint" );
const char* LINEWIDTH_PROPERTY_NAME( "uLineWidth" );
const char* COLOR_PROPERTY_NAME( "uColor" );
} // namespace

QuadraticBezier::QuadraticBezier()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
QuadraticBezier::QuadraticBezier(ShaderEffect handle)
:ShaderEffect(handle)
{
}

QuadraticBezier::~QuadraticBezier()
{
}

QuadraticBezier QuadraticBezier::New(unsigned int pointCount, bool filled )
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

  Dali::ShaderEffect shaderEffectCustom = Dali::ShaderEffect::NewWithPrefix( vertexShaderPrefix.str(),vertexShader,
                                                                             "#extension GL_OES_standard_derivatives:enable\n", fragmentShader,
                                                                             GEOMETRY_TYPE_UNTEXTURED_MESH );

  Dali::Toolkit::QuadraticBezier handle( shaderEffectCustom );

  //Set default uniform values
  handle.SetUniform( COLOR_PROPERTY_NAME, Vector4(1.0f,1.0f,1.0f,1.0f) );
  if( !filled )
  {
    //Set default line widht to 1 pixel
    handle.SetUniform( LINEWIDTH_PROPERTY_NAME, 1.0f );
  }

  return handle;
}

void QuadraticBezier::SetPoint(unsigned int index, const Vector3& position)
{
  SetUniform( GetPointPropertyName(index), position );
}

void QuadraticBezier::SetLineWidth( float width )
{
  SetUniform( LINEWIDTH_PROPERTY_NAME, width );
}

void QuadraticBezier::SetColor( const Vector4& color )
{
  SetUniform( COLOR_PROPERTY_NAME, color );
}

std::string QuadraticBezier::GetPointPropertyName(unsigned int index) const
{
  std::ostringstream propertyName;
  propertyName<<POINT_PROPERTY_NAME<<"["<<index<<"]";
  return propertyName.str();
}

std::string QuadraticBezier::GetLineWidthPropertyName( ) const
{
  return LINEWIDTH_PROPERTY_NAME;
}

std::string QuadraticBezier::GetColorPropertyName( ) const
{
  return COLOR_PROPERTY_NAME;
}

} // namespace Toolkit

} // namespace Dali

