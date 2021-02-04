#ifndef DALI_TOOLKIT_SHADER_EFFECT_DISSOLVE_H
#define DALI_TOOLKIT_SHADER_EFFECT_DISSOLVE_H

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
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/property-map.h>
#include <string.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Toolkit
{
/**
 * @brief Set the dissolve central line.
 *
 * Use one point (position) and one direction ( displacement ) vector to define this line
 * As we use the texture coordinate as pixel position to calculate random offset,
 * the line should passing through rectangle {(0,0),(0,1),(1,0),(1,1)},
 * so make the position parameter with two component values between 0.0 to 1.0
 * @param[in] actor The actor that registers the uniform properties
 * @param[in] position The point ( locates within rectangle {(0,0),(0,1),(1,0),(1,1)} ) passed through by the central line
 * @param[in] displacement The direction of the central line
 * @param[in] initialProgress The normalised initial progress of the shader
 */
DALI_TOOLKIT_API void DissolveEffectSetCentralLine(Actor& actor, const Vector2& position, const Vector2& displacement, float initialProgress);

/**
 * @brief Create a new Dissolve effect
 *
 *  DissolveEffect is a custom shader effect to achieve Dissolve effects in image views.
 *
 *  Animatable/Constrainable uniforms:
 *    "uPercentage" - This value is proportional to the distortion applied; a value of zero means no distortion.
 *
 *  @param[in] useHighPrecision True if using high precision in fragment shader for fully random noise, false otherwise
 *  @return The newly created Property::Map with the dissolve effect
 */

DALI_TOOLKIT_API Property::Map CreateDissolveEffect(bool useHighPrecision = true);

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_SHADER_EFFECT_DISSOLVE_H
