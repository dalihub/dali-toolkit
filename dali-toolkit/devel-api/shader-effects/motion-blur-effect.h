#ifndef DALI_TOOLKIT_SHADER_EFFECT_MOTION_BLUR_H
#define DALI_TOOLKIT_SHADER_EFFECT_MOTION_BLUR_H

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
 * @brief Set the properties for the motion blur
 *
 * @param[in] actor The actor that registers the uniform properties
 * @param[in] numBlurSamples Number of samples used by the shader
 */
DALI_TOOLKIT_API void SetMotionBlurProperties(Actor& actor, unsigned int numBlurSamples = 8);

/**
 * @brief Create a new MotionBlurEffect
 *
 * Motion blur shader works on a per object basis. Objects will
 * blur when they move, or if the camera moves.
 *
 * Animatable/Constrainable uniforms:
 *  "uBlurTexCoordScale"      - This scales the offset for texture samples along the motion velocity vector.
 *                              A smaller value means the samples will be spaced closer, larger value further
 *                              apart. User should use this to get the blur to look contiguous, i.e. the blur
 *                              texels should not be too widely spread, with gaps in between. Default 0.125.
 *  "uGeometryStretchFactor"  - This scales the amount the geometry stretches backwards along the motion velocity
 *                              vector. A smaller value means the geometry stretches less, larger it stretches more.
 *                              User should use this to get the blur to 'bleed' into areas outside the physical
 *                              bounds of the actor. We need this as the blur is only applied inside the bounds of
 *                              the actor, but you would expect motion blur trails where the actor was previously
 *                              but is there no longer. Default 0.05.
 *  "uSpeedScalingFactor"     - This takes the magnitude of the motion velocity vector and scales it to produce a
 *                              value which is used to fade the blur in / out with the speed that the actor is moving.
 *                              As the blur fades in, more of the blur is visible and less of the original actor, and
 *                              viceversa. This value is also used to control how much to fade the actor near the
 *                              edges, based on the speed the actor is moving. When the actor is at rest this is not applied.
 *                              Default 0.5.
 *  "uObjectFadeStart"        - The displacement from the centre of the actor that the actor will start to fade towards its
 *                              edges. This is used to prevent an unsightly hard edge between the blurred actor and the scene.
 *                              Depends on the values of the vertices in the vertex stream. When the actor is at rest this is
 *                              not applied. Default 0.25, which is half way towards the edge for an ImageVisual::QUAD.
 *  "uObjectFadeEnd"          - The displacement from the centre of the actor that the actor will finish fading towards its
 *                              edges. This is used to prevent an unsightly hard edge between the blurred actor and the scene.
 *                              Depends on the values of the vertices in the vertex stream. When the actor is at rest this is
 *                              not applied.Default 0.5, which is all the way towards the edge for an ImageVisual::QUAD.
 *  "uAlphaScale"             - Global scaler applied to the alpha of the actor. Used to make the blurred actor a bit more subtle
 *                              (helps to hide discontinuities due to limited number of texture samples) and reveal a bit of the
 *                              background behind it as it moves. When the actor is at rest this is not applied. Default 0.75.
 *  "uNumSamples"             - The number of texture samples to be taken. Increasing the number of samples provides better quality
 *                              at the cost of performance.
 *  "uModelLastFrame"         - The model to world space transformation matrix of the actor in the previous frame.
 *
 * @return The newly created Property::Map with the motion blur effect
 */
DALI_TOOLKIT_API Property::Map CreateMotionBlurEffect();

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_SHADER_EFFECT_MOTION_BLUR_H
