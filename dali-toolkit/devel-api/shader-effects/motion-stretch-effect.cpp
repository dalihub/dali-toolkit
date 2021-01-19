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
 */

// CLASS HEADER
#include <dali-toolkit/devel-api/shader-effects/motion-stretch-effect.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>

namespace Dali
{
namespace Toolkit
{

void SetMotionStretchProperties( Actor& actor )
{
  actor.RegisterProperty("uGeometryStretchFactor", 0.5f);
  actor.RegisterProperty("uSpeedScalingFactor", 0.5f);
  actor.RegisterProperty("uObjectFadeStart", Vector2(0.25f, 0.25f));
  actor.RegisterProperty("uObjectFadeEnd", Vector2(0.5f, 0.5f));
  actor.RegisterProperty("uAlphaScale", 0.75f);
  Property::Index uModelProperty = actor.RegisterProperty("uModelLastFrame", Matrix::IDENTITY);

  Constraint constraint = Constraint::New<Matrix>(actor, uModelProperty, EqualToConstraint());
  constraint.AddSource(Source(actor, Actor::Property::WORLD_MATRIX));
  constraint.Apply();
}

Property::Map CreateMotionStretchEffect()
{
  Property::Map map;

  Property::Map customShader;
  customShader[Visual::Shader::Property::VERTEX_SHADER]   = SHADER_MOTION_STRETCH_EFFECT_VERT.data();
  customShader[Visual::Shader::Property::FRAGMENT_SHADER] = SHADER_MOTION_STRETCH_EFFECT_FRAG.data();

  customShader[Visual::Shader::Property::SUBDIVIDE_GRID_X] = 10;
  customShader[Visual::Shader::Property::SUBDIVIDE_GRID_Y] = 10;

  customShader[Visual::Shader::Property::HINTS] = Shader::Hint::OUTPUT_IS_TRANSPARENT;

  map[Toolkit::Visual::Property::SHADER] = customShader;
  return map;
}


} // Toolkit
} // Dali
