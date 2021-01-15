#ifndef DALI_TOOLKIT_SHADER_EFFECT_DISTANCEFIELD_H
#define DALI_TOOLKIT_SHADER_EFFECT_DISTANCEFIELD_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/property-map.h>

namespace Dali
{
namespace Toolkit
{
/**
 * Creates a new DistanceFieldEffect
 *
 * DistanceFieldEffect is a custom shader effect to achieve distance field on Image actors
 *
 * Animatable/Constrainable uniforms - These will need to be registered to the actor as a custom property to take into effect:
 *
 *  "uDoGlow"       - The glow state. If true, glow is enabled
 *  "uGlowBoundary" - The glow boundary factor
 *  "uGlowColor"    - The glow color multiplier

 *  "uDoShadow"     - The shadow state. If true, shadows is enabled. Cannot be used with glow/and or outline
 *  "uShadowColor"  - The shadow color multiplier
 *  "uShadowOffset" - The shadow offset

 *  "uDoOutline"    - The outline state. If true, outline is enabled
 *  "uOutlineColor" - The outline color multiplier
 *  "uOutlineParams"- Thickness of outline. The outline thickness is determined by two values.
 *                    First value [0-1] Specifies the distance field value for the center of the outline.
 *                    Second value [0-1] Specifies the softness/width/anti-aliasing of the outlines inner edge.
 *
 *  @return The newly created Property::Map with the distance field effect
 */
DALI_TOOLKIT_API Dali::Property::Map CreateDistanceFieldEffect();

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_SHADER_EFFECT_DISTANCEFIELD_H
