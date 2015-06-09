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

#include <dali-toolkit/internal/controls/page-turn-view/page-turn-effect.h>
#include <dali/public-api/math/matrix.h>

using namespace Dali;
using namespace Dali::Toolkit;

void CommonParametersConstraint( Dali::Matrix& current, const PropertyInputContainer& inputs )
{
  const Vector2& originalCenter = inputs[0]->GetVector2();
  Vector2 currentCenter = inputs[1]->GetVector2();
  const Vector2& pageSize = inputs[2]->GetVector2();

  // calculate the curve direction and the vanishing point
  // here, the vanishing point is the intersection of spine with the line passing through original center and vertical to curve direction
  Vector2 curveDirection( currentCenter - originalCenter );
  curveDirection.Normalize();
  if( fabs(curveDirection.y) < 0.01f) // eliminate the possibility of division by zero in the next step
  {
    curveDirection.y = 0.01f;
  }
  float vanishingPointY = originalCenter.y + curveDirection.x * originalCenter.x / curveDirection.y;

  float curveEndY, cosTheta ,sinTheta ,translateX, translateY;
  // when the vanishing point is very far away, make it infinitely, in this case, the page bent horizontally
  const float THRESHOLD(20.0);
  if( fabs(vanishingPointY-pageSize.y*0.5f) >= pageSize.y*THRESHOLD )
  {
    curveDirection = Vector2(-1.f,0.f);
    currentCenter.y = originalCenter.y;

    curveEndY = originalCenter.y;
    cosTheta = 1.f;
    sinTheta = 0.f;
    translateX = currentCenter.x - originalCenter.x;
    translateY = vanishingPointY;
  }
  else
  {
    curveEndY = currentCenter.y - curveDirection.y * (currentCenter.x/curveDirection.x) ;
    Vector2 v1( currentCenter.x, currentCenter.y - vanishingPointY );
    v1.Normalize();
    Vector2 v2( originalCenter.x, originalCenter.y - vanishingPointY );
    v2.Normalize();
    cosTheta = v1.x*v2.x + v1.y*v2.y;
    sinTheta = ( vanishingPointY > pageSize.y*0.5f ) ? sqrt(1.0-cosTheta*cosTheta) : -sqrt(1.0-cosTheta*cosTheta);
    translateX = currentCenter.x - cosTheta*originalCenter.x - sinTheta*( originalCenter.y-vanishingPointY );
    translateY = currentCenter.y + sinTheta*originalCenter.x - cosTheta*( originalCenter.y-vanishingPointY );
  }

  float originalLength = fabs(originalCenter.x/curveDirection.x);
  float currentLength = fabs(currentCenter.x/curveDirection.x);
  float curveHeight = 0.45f*sqrt(originalLength*originalLength - currentLength*currentLength);

  float* parameterArray = current.AsFloat();
  parameterArray[0] = cosTheta;
  parameterArray[1] = -sinTheta;
  parameterArray[2] = originalCenter.x;
  parameterArray[3] = originalCenter.y;
  parameterArray[4] = sinTheta;
  parameterArray[5] = cosTheta;
  parameterArray[6] = currentCenter.x;
  parameterArray[7] = currentCenter.y;
  parameterArray[8] = translateX;
  parameterArray[9] = translateY;
  parameterArray[10] = vanishingPointY;
  parameterArray[11] = curveEndY;
  parameterArray[12] = curveDirection.x;
  parameterArray[13] = curveDirection.y;
  parameterArray[14] = curveHeight;
  parameterArray[15] = currentLength;
}

void Dali::Toolkit::Internal::PageTurnApplyInternalConstraint( ShaderEffect& shaderEffect)
{
  Constraint constraint = Constraint::New<Dali::Matrix>( shaderEffect, shaderEffect.GetPropertyIndex( "uCommonParameters" ), CommonParametersConstraint );
  constraint.AddSource( LocalSource( shaderEffect.GetPropertyIndex( "uOriginalCenter" ) ) );
  constraint.AddSource( LocalSource( shaderEffect.GetPropertyIndex( "uCurrentCenter" ) ) );
  constraint.AddSource( LocalSource( shaderEffect.GetPropertyIndex( "uPageSize" ) ) );
  constraint.Apply();
}

ShaderEffect Dali::Toolkit::Internal::CreatePageTurnEffect(bool enableBlending)
{
  std::string vertexShader = DALI_COMPOSE_SHADER(
      /*
       * The common parameters for all the vertices, calculate in CPU then pass into the shader as uniforms
       *
       *  first part of the page, (outside the the line passing through original center and vertical to curve direction)
       * no Z change, only 2D rotation and translation
       * ([0][0],[0][1],[1][0],[1][1]) mat2 rotateMatrix
       * ([2][0],[2][1]) vec2 translationVector
       *
       * ([0][2],[0][3]) vec2 originalCenter: Typically the press down position of the Pan Gesture
       * ([1][2],[1][3]) vec2 currentCenter: Typically the current position of the Pan Gesture
       * ([3][0],[3][1]) vec2 curveDirection: The normalized vector pointing from original center to current center
       * ([2][2]) float vanishingPointY: The Y coordinate of the intersection of the spine
       *                                 and the line which goes through the original center and is vertical to the curveDirection
       * ([2][3]) float curveEndY: The Y coordinate of intersection of the spine and the line through both original and current center
       * ([3][2]) float curveHeight: The height of the interpolated hermite curve.
       * ([3][3]) float currentLength: The length from the current center to the curveEnd.
       */
      precision mediump float;\n
      uniform mat4 uCommonParameters;\n
      \n
      uniform vec2 uPageSize;\n
      uniform float uIsTurningBack;\n
      uniform float uShadowWidth;\n
      varying vec3 vNormal;\n
      varying vec4 vPosition;\n
      varying float vEdgeShadow;\n
      \n
      void main()\n
      {\n
        vec4 position = vec4( aPosition.xy, 0.0, 1.0);\n
        vec2 currentCenter = vec2( uCommonParameters[1][2], uCommonParameters[1][3]);\n
        vec2 originalCenter = vec2( uCommonParameters[0][2], uCommonParameters[0][3]);\n
        vec3 normal = vec3(0.0,0.0,1.0);\n
        \n
        if(currentCenter.x < originalCenter.x)\n
        {\n
          // change the coordinate origin from the center of the page to its top-left
          position.xy += uPageSize * 0.5;\n
          vec2 curveDirection = vec2( uCommonParameters[3]);\n
          vec3 vanishingPoint = vec3(0.0, uCommonParameters[2][2], 0.0);\n
          // first part of the page, (outside the the line passing through original center and vertical to curve direction)
          //no Z change, only 2D rotation and translation
          if( dot(curveDirection, position.xy - originalCenter) < 0.0 )
          {\n
            position.y -= vanishingPoint.y;\n
            position.xy = mat2(uCommonParameters)*position.xy + vec2( uCommonParameters[2]);\n
          }\n
          // second part of the page, bent as a ruled surface
          else\n
          {\n
            // calculate on the flat plane, between
            // the first line passing through current vertex and vanishing point
            // the second line passing through original center and current center
            vec2 curveEnd = vec2( 0.0, uCommonParameters[2][3] );\n
            vec2 curFlatDirection = vec2(0.0,1.0);\n
            float lengthFromCurve = position.y - originalCenter.y;\n
            float lengthOnCurve = position.x;\n
            if(currentCenter.y != originalCenter.y)\n
            {\n
              curFlatDirection = normalize(position.xy - vanishingPoint.xy);\n
              lengthFromCurve = (curveEnd.x*curveDirection.y-curveEnd.y*curveDirection.x-position.x*curveDirection.y+position.y*curveDirection.x)
              / (curFlatDirection.x*curveDirection.y-curFlatDirection.y*curveDirection.x);\n
              lengthOnCurve = length(position.xy+lengthFromCurve*curFlatDirection-curveEnd);\n
            }\n
            \n
            // define the control points of hermite curve, composed with two segments
            // calulation is carried out on the 2D plane which is passing through both current and original center and vertical to the image plane
            float currentLength = uCommonParameters[3][3];\n
            float originalLength =  abs(originalCenter.x/curveDirection.x);\n
            float height = uCommonParameters[3][2];\n
            float percentage = currentLength/originalLength;\n
            //vec2 SegmentOneControlPoint0 = vec2(0.0, 0.0);
            vec2 SegmentOneControlPoint1 = vec2((0.65*percentage - 0.15)*originalLength, (0.8 + 0.2 * percentage)*height); \n
            vec2 SegmentTwoControlPoint0 = SegmentOneControlPoint1;\n
            vec2 SegmentTwoControlPoint1 = vec2(currentLength, 0.0); \n
            vec2 SegmentOneTangentVector0 = SegmentOneControlPoint1;\n
            vec2 SegmentOneTangentVector1 = vec2(0.5*originalLength,0.0);\n
            vec2 SegmentTwoTangentVector0 = SegmentOneTangentVector1;\n
            vec2 SegmentTwoTangentVector1 = SegmentOneTangentVector1;\n
            \n
            // calulate the corresponding curve point position and its tangent vector
            // it is a linear mapping onto nonlinear curves, might cause some unwanted deformation
            // but as there are no analytical method to calculate the curve length on arbitrary segment
            // no efficient way to solve this nonlinear mapping, Numerical approximation would cost too much computation in shader
            vec2 curvePoint2D;\n
            vec2 tangent;\n
            float t0 = lengthOnCurve / originalLength;\n
            if(t0<=0.5)\n
            {\n
              float t = 2.0*t0;\n
              float t_2 = t*t;\n
              float t_3 = t*t_2;\n
              curvePoint2D = (-2.0*t_3+3.0*t_2)*SegmentOneControlPoint1
              + (t_3-2.0*t_2+t)*SegmentOneTangentVector0 + (t_3-t_2)*SegmentOneTangentVector1;\n
              tangent = (-6.0*t_2+6.0*t)*SegmentOneControlPoint1
              + (3.0*t_2-4.0*t+1.0)*SegmentOneTangentVector0 + (3.0*t_2-2.0*t)*SegmentOneTangentVector1;\n
            }\n
            else\n
            {\n
              float t = 2.0*t0-1.0;\n
              float t_2 = t*t;\n
              float t_3 = t*t_2;\n
              curvePoint2D = (2.0*t_3-3.0*t_2+1.0)*SegmentTwoControlPoint0 + (-2.0*t_3+3.0*t_2)*SegmentTwoControlPoint1
              + (t_3-2.0*t_2+t)*SegmentTwoTangentVector0 + (t_3-t_2)*SegmentTwoTangentVector1;\n
              tangent = (6.0*t_2-6.0*t)*SegmentTwoControlPoint0 + (-6.0*t_2+6.0*t)*SegmentTwoControlPoint1
              + (3.0*t_2-4.0*t+1.0)*SegmentTwoTangentVector0 + (3.0*t_2-2.0*t)*SegmentTwoTangentVector1;\n
              // a trick to eliminate some optical illusion caused by the gradient matter of normal in per-fragment shading
              // which is caused by linear interpolation of normal vs. nonlinear lighting
              // will notice some artifact in the areas with dramatically normal changes, so compress the normal differences here
              tangent.y *=  min(1.0, length(position.xyz - vanishingPoint) / uPageSize.y ); \n
            }\n
            vec3 curvePoint = vec3(curveEnd - curvePoint2D.x*curveDirection,max(0.0,curvePoint2D.y));\n
            vec3 tangentVector = vec3(-tangent.x*curveDirection,tangent.y);\n
            \n
            // locate the new vertex position on the line passing through both vanishing point and the calculated curve point position
            vec3 curLiftDirection = vec3(0.0,-1.0,0.0);\n
            if(currentCenter.y != originalCenter.y)\n
            {\n
              curLiftDirection = normalize(curvePoint - vanishingPoint);\n
              tangentVector *= (curveDirection.y > 0.0) ? -1.0 : 1.0;\n
              // an heuristic adjustment here, to compensate the linear parameter mapping onto the nonlinear curve
              float Y0 = position.y - curveDirection.y * (position.x/curveDirection.x); \n
              float proportion;
              float refLength;\n
              if(abs(Y0-vanishingPoint.y) > abs(curveEnd.y-vanishingPoint.y)) \n
              {\n
                proportion = abs(curveEnd.y - Y0) / (abs(curveEnd.y-Y0)+abs(curveEnd.y - vanishingPoint.y)); \n
                refLength = proportion*length(originalCenter-vanishingPoint.xy) / (proportion-1.0); \n
              }\n
              else\n
              {\n
                proportion = abs(curveEnd.y - Y0) / abs(curveEnd.y - vanishingPoint.y);\n
                refLength = proportion*length(originalCenter-vanishingPoint.xy); \n
              }\n
              float Y1 = currentCenter.y - (normalize(currentCenter-vanishingPoint.xy)).y * refLength; \n
              position.y = mix(Y0, Y1, t0); \n
            }\n
            position.xz = curvePoint.xz - lengthFromCurve*curLiftDirection.xz;\n
            // calculate the normal vector, will be used for lighting
            normal = cross(curLiftDirection, normalize(tangentVector));\n
            // the signature of Z is decided by the page turning direction:
            // from left to right(negative); from right to left (positive)
            position.z *= -uIsTurningBack;\n
            normal.xy *= -uIsTurningBack;\n
          }\n
          // change the coordinate origin from the top-left of the page to its center
          position.xy -= uPageSize * 0.5; \n
        }\n
        position.z += aPosition.z;\n
        gl_Position = uMvpMatrix * position;\n
        // varying parameters for fragment shader
        vTexCoord = aTexCoord;
        vNormal = uNormalMatrix*normal;\n
        vPosition = uModelView * position;\n
  );

  std::string vertexShaderWithFakedShadow = DALI_COMPOSE_SHADER(
      // display shadow, the fake shadow value is calculated according to the height and the distance from page edge
      vTexCoord.x = (aTexCoord.x-sTextureRect.s) /( 1.0 - uShadowWidth ) + sTextureRect.s;\n
      vTexCoord.y = ( aTexCoord.y-sTextureRect.t-0.5*uShadowWidth*(sTextureRect.q-sTextureRect.t) )/( 1.0 - uShadowWidth ) + sTextureRect.t;\n
      float heightCoef = (1.0 + position.z*uIsTurningBack*3.0 / uPageSize.x) * 0.6;
      vEdgeShadow = clamp(0.9 - heightCoef, 0.0, 0.9 ); \n
      if( vTexCoord.y >= sTextureRect.q || vTexCoord.y <= sTextureRect.t || vTexCoord.x >= sTextureRect.p  )\n
      {\n
        float inversedShadowWidth = (1.0-uShadowWidth) / uShadowWidth ;\n
        float alpha1 = (vTexCoord.x-sTextureRect.p) * inversedShadowWidth / (sTextureRect.p - sTextureRect.s);\n
        inversedShadowWidth = 2.0 * inversedShadowWidth  / (sTextureRect.q - sTextureRect.t); \n
        float alpha2 = (vTexCoord.y-sTextureRect.q) * inversedShadowWidth;\n
        float alpha3 = (sTextureRect.t-vTexCoord.y) * inversedShadowWidth;\n
        float alpha;\n
        if(alpha1 > 0.0 && alpha2 > 0.0) alpha = sqrt(alpha2*alpha2+alpha1*alpha1)/sqrt(1.0 + max(alpha1,alpha2)*max(alpha1,alpha2));\n //bottom-right corner
        else if(alpha1 > 0.0 && alpha3 > 0.0) alpha = sqrt(alpha3*alpha3+alpha1*alpha1)/sqrt(1.0+max(alpha1,alpha3)*max(alpha1,alpha3));\n //top-right corner
        else alpha = max(alpha1,max(alpha2,alpha3)); \n
        alpha = 0.9 - alpha*0.9;\n
        vEdgeShadow = clamp(alpha - heightCoef, 0.0, 0.9 ); \n
      }\n
  );

  std::string vertexShaderEnd("}");

  std::string fragmentShaderPartOne = DALI_COMPOSE_SHADER(
      precision mediump float;\n
      uniform vec2 uPageSize;\n
      uniform vec2 uSpineShadowParameter;\n
      varying vec3 vNormal;\n
      varying vec4 vPosition;\n
      varying float vEdgeShadow;\n
      \n
      void main()\n
      {\n
        // need to re-normalize the interpolated normal
        vec3 normal = normalize(vNormal);\n
        vec4 texel;\n
        float spineShadowCoef = 1.0; \n
  );

  std::string fragmentShaderWithFakedShadow = DALI_COMPOSE_SHADER(
      if( vTexCoord.y > sTextureRect.q || vTexCoord.y < sTextureRect.t || vTexCoord.x > sTextureRect.p  )\n
      texel = vec4(0.0,0.0,0.0,vEdgeShadow);
      else \n
  );

  std::string fragmentShaderPartTwo = DALI_COMPOSE_SHADER(
      { \n
    // display page content
    // display back image of the page, flip the texture
    if(  dot(vPosition.xyz, normal) > 0.0 ) texel = texture2D( sTexture, vec2( sTextureRect.p+sTextureRect.s-vTexCoord.x, vTexCoord.y ) );\n
    // display front image of the page
    else texel = texture2D( sTexture, vTexCoord );\n
    // display book spine, a stripe of shadowed texture
    float pixelPos = (vTexCoord.x-sTextureRect.s)*uPageSize.x; \n
    if(pixelPos < uSpineShadowParameter.x) \n
    {\n
      float x = pixelPos - uSpineShadowParameter.x;\n
      float y = sqrt( uSpineShadowParameter.x*uSpineShadowParameter.x - x*x);\n
      spineShadowCoef = normalize( vec2( uSpineShadowParameter.y*x/uSpineShadowParameter.x, y ) ).y;\n
    }\n
      }\n
      // calculate the lighting
      // set the ambient color as vec3(0.4);
      float lightColor = abs( normal.z ) * 0.6 + 0.4;\n
      gl_FragColor = vec4( ( spineShadowCoef* lightColor)* texel.rgb , texel.a ) * uColor;\n
      }
  );

  // Create the implementation, temporarily owned on stack,
  Dali::ShaderEffect shaderEffectCustom;
  std::ostringstream vertexShaderStringStream;
  std::ostringstream fragmentShaderStringStream;
  if( enableBlending )
  {
    vertexShaderStringStream<< vertexShader << vertexShaderWithFakedShadow << vertexShaderEnd;
    fragmentShaderStringStream<< fragmentShaderPartOne << fragmentShaderWithFakedShadow << fragmentShaderPartTwo;
    shaderEffectCustom = Dali::ShaderEffect::New( vertexShaderStringStream.str(), fragmentShaderStringStream.str(),
                                                  ShaderEffect::GeometryHints( ShaderEffect::HINT_GRID | ShaderEffect::HINT_DEPTH_BUFFER | ShaderEffect::HINT_BLENDING) );
  }
  else
  {
    vertexShaderStringStream<< vertexShader << vertexShaderEnd;
    fragmentShaderStringStream<< fragmentShaderPartOne << fragmentShaderPartTwo;
    shaderEffectCustom = Dali::ShaderEffect::New( vertexShaderStringStream.str(), fragmentShaderStringStream.str(),
                                                  ShaderEffect::GeometryHints( ShaderEffect::HINT_GRID | ShaderEffect::HINT_DEPTH_BUFFER ) );
  }

  static const float DEFAULT_SHADOW_WIDTH(0.15f);
  static const Vector2 DEFAULT_SPINE_SHADOW_PARAMETER(50.0f, 20.0f);

  Vector2 defaultPageSize = Dali::Stage::GetCurrent().GetSize();
  Dali::Matrix zeroMatrix(true);
  shaderEffectCustom.SetUniform( "uCommonParameters", zeroMatrix );
  shaderEffectCustom.SetUniform( "uPageSize", defaultPageSize/(1.f-DEFAULT_SHADOW_WIDTH) );
  shaderEffectCustom.SetUniform( "uShadowWidth", DEFAULT_SHADOW_WIDTH );
  shaderEffectCustom.SetUniform( "uSpineShadowParameter", DEFAULT_SPINE_SHADOW_PARAMETER );

  shaderEffectCustom.RegisterProperty( "uOriginalCenter", Vector2( defaultPageSize[0], defaultPageSize[1]*0.5f ) );
  shaderEffectCustom.RegisterProperty( "uCurrentCenter", Vector2( defaultPageSize[0], defaultPageSize[1]*0.5f ) );

  PageTurnApplyInternalConstraint(shaderEffectCustom);

  // setting isTurningBack to -1.0f here means turning page forward
  shaderEffectCustom.SetUniform( "uIsTurningBack", -1.0f );

  return shaderEffectCustom;
}

