#ifndef DALI_TOOLKIT_SHADER_EFFECT_MOTION_STRETCH_H
#define DALI_TOOLKIT_SHADER_EFFECT_MOTION_STRETCH_H

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
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
namespace Toolkit
{
/**
 * @brief Set the properties for the motion stretch
 */
DALI_TOOLKIT_API void SetMotionStretchProperties(Actor& actor);

/**
 * @brief Creates a new MotionStretchEffect
 *
 * Motion stretch shader works on a per object basis. Objects will stretch in the direction of motion when they move, or if the camera moves.
 *
 * Animatable/Constrainable uniforms:
 *  "uGeometryStretchFactor"  - This scales the amount the geometry stretches along the motion velocity vector.
 *                              A smaller value means the geometry stretches less, larger it stretches more. Default 0.5.
 *  "uSpeedScalingFactor"     - This value is used to control how much to fade the actor near the edges, based on the
 *                              speed the actor is moving. When the actor is at rest this is not applied. Default 0.5.
 *  "uObjectFadeStart"        - The displacement from the centre of the actor that the actor will start to fade towards
 *                              its edges. This is used to prevent an unsightly hard edge between the stretched actor and
 *                              the scene. Depends on the values of the vertices in the vertex stream. When the actor is at
 *                              rest this is not applied. Default Vector2(0.25, 0.25), which is half way towards the edge for
 *                              an ImageVisual::QUAD.
 *  "uObjectFadeEnd"          - The displacement from the centre of the actor that the actor will finish fading towards its edges.
 *                              This is used to prevent an unsightly hard edge between the stretched actor and the scene. Depends
 *                              on the values of the vertices in the vertex stream. When the actor is at rest this is not applied.
 *                              Default 0.5, which is all the way towards the edge for an ImageVisual::QUAD.
 *  "uAlphaScale"             - Global scaler applied to the alpha of the actor. Used to make the stretched actor a bit more subtle
 *                              and reveal a bit of the background behind it as it moves. When the actor is at rest this is not
 *                              applied. Default 0.75.
 *  "uModelLastFrame"         - The model to world space transformation matrix of the actor in the previous frame.
 *
 * @return The newly created Property::Map with the motion stretch effect
 */
DALI_TOOLKIT_API Property::Map CreateMotionStretchEffect();

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_SHADER_EFFECT_MOTION_STRETCH_H
