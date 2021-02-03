/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/object/property-map.h>
#include <string.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/page-turn-view/page-turn-effect.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* const PROPERTY_COMMON_PARAMETERS("uCommonParameters");
const char* const PROPERTY_ORIGINAL_CENTER("originalCenter");
const char* const PROPERTY_CURRENT_CENTER("currentCenter");
} // namespace

/**
 * This constraint updates the common parameter values used by every vertex.
 * By using constraint, they are calculate once in CPU then pass into the vertex shader as uniforms
 */
struct CommonParametersConstraint
{
  CommonParametersConstraint(float pageHeight)
  : mPageHeight(pageHeight)
  {
  }

  void operator()(Dali::Matrix& current, const PropertyInputContainer& inputs)
  {
    const Vector2& originalCenter = inputs[0]->GetVector2();
    Vector2        currentCenter  = inputs[1]->GetVector2();

    // calculate the curve direction and the vanishing point
    // here, the vanishing point is the intersection of spine with the line passing through original center and vertical to curve direction
    Vector2 curveDirection(currentCenter - originalCenter);
    curveDirection.Normalize();
    if(fabs(curveDirection.y) < 0.01f) // eliminate the possibility of division by zero in the next step
    {
      curveDirection.y = 0.01f;
    }
    float vanishingPointY = originalCenter.y + curveDirection.x * originalCenter.x / curveDirection.y;

    float curveEndY, cosTheta, sinTheta, translateX, translateY;
    // when the vanishing point is very far away, make it infinitely, in this case, the page bent horizontally
    const float THRESHOLD(20.0);
    if(fabs(vanishingPointY - mPageHeight * 0.5f) >= mPageHeight * THRESHOLD)
    {
      curveDirection  = Vector2(-1.f, 0.f);
      currentCenter.y = originalCenter.y;

      curveEndY  = originalCenter.y;
      cosTheta   = 1.f;
      sinTheta   = 0.f;
      translateX = currentCenter.x - originalCenter.x;
      translateY = vanishingPointY;
    }
    else
    {
      curveEndY = currentCenter.y - curveDirection.y * (currentCenter.x / curveDirection.x);
      Vector2 v1(currentCenter.x, currentCenter.y - vanishingPointY);
      v1.Normalize();
      Vector2 v2(originalCenter.x, originalCenter.y - vanishingPointY);
      v2.Normalize();
      cosTheta   = v1.x * v2.x + v1.y * v2.y;
      sinTheta   = (vanishingPointY > mPageHeight * 0.5f) ? sqrt(1.0 - cosTheta * cosTheta) : -sqrt(1.0 - cosTheta * cosTheta);
      translateX = currentCenter.x - cosTheta * originalCenter.x - sinTheta * (originalCenter.y - vanishingPointY);
      translateY = currentCenter.y + sinTheta * originalCenter.x - cosTheta * (originalCenter.y - vanishingPointY);
    }

    float originalLength = fabs(originalCenter.x / curveDirection.x);
    float currentLength  = fabs(currentCenter.x / curveDirection.x);
    float curveHeight    = 0.45f * sqrt(originalLength * originalLength - currentLength * currentLength);

    float* parameterArray = current.AsFloat();
    parameterArray[0]     = cosTheta;
    parameterArray[1]     = -sinTheta;
    parameterArray[2]     = originalCenter.x;
    parameterArray[3]     = originalCenter.y;
    parameterArray[4]     = sinTheta;
    parameterArray[5]     = cosTheta;
    parameterArray[6]     = currentCenter.x;
    parameterArray[7]     = currentCenter.y;
    parameterArray[8]     = translateX;
    parameterArray[9]     = translateY;
    parameterArray[10]    = vanishingPointY;
    parameterArray[11]    = curveEndY;
    parameterArray[12]    = curveDirection.x;
    parameterArray[13]    = curveDirection.y;
    parameterArray[14]    = curveHeight;
    parameterArray[15]    = currentLength;
  }

  float mPageHeight;
};

void Dali::Toolkit::Internal::PageTurnApplyInternalConstraint(Actor& actor, float pageHeight)
{
  Constraint constraint = Constraint::New<Dali::Matrix>(actor, actor.GetPropertyIndex(PROPERTY_COMMON_PARAMETERS), CommonParametersConstraint(pageHeight));
  constraint.AddSource(LocalSource(actor.GetPropertyIndex(PROPERTY_ORIGINAL_CENTER)));
  constraint.AddSource(LocalSource(actor.GetPropertyIndex(PROPERTY_CURRENT_CENTER)));
  constraint.Apply();
}

Property::Map Dali::Toolkit::Internal::CreatePageTurnEffect()
{
  Property::Map map;

  Property::Map customShader;

  customShader[Toolkit::Visual::Shader::Property::VERTEX_SHADER]    = SHADER_PAGE_TURN_EFFECT_VERT.data();
  customShader[Toolkit::Visual::Shader::Property::FRAGMENT_SHADER]  = SHADER_PAGE_TURN_EFFECT_FRAG.data();
  customShader[Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_X] = 20;
  customShader[Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_Y] = 20;

  map[Toolkit::Visual::Property::SHADER] = customShader;
  return map;
}
