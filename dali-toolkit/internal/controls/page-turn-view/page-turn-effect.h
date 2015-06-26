#ifndef __DALI_PAGE_TURN_EFFECT_H_
#define __DALI_PAGE_TURN_EFFECT_H_

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

// EXTERNAL INCLUDES
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/property-input.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <sstream>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{


/**
 * @brief Re-applies PageTurnEffect internal constraints
 * The internal constraint uses the OriginalCenter property and the CurrentCenter Property
 * to update the variety of common parameters which are with the same value for all the vertices.
 * Note: For each actor, the constraints are applied in the same order as the calls to Actor::ApplyConstraint().
 * So if there are other contraints applied to the OriginalCenter or CurrentCenter while when using this effect,
 * call this method to get the internal constraints and re-apply it afterwards.
 *
 * @param[in] shaderEffect The page turn effect to which internal constraints should be re-applied
 */
void PageTurnApplyInternalConstraint( ShaderEffect& shaderEffect);

/**
 * @brief Create a new PageTurnEffect
 * PageTurnEffect is a custom shader effect to achieve page turn effect for Image actors.
 *
 * Usage example:-
 *
 * // create shader used for doing page-turn effect\n
 * ShaderEffect pageTurnEffect = CreatePageTurnEffect();
 *
 * // set image actor shader to the page-turn one\n
 * // for portrait view, one image actor for each page\n
 * // for landscape view, the page turned over is still visible, so back image is needed \n
 * //     in this case, create another image Actor using the back image and added to the page actor \n
 * ImageActor pageActor = ImageActor::New(....); \n
 * ImageActor backImageActor = ImageActor::New(....); \n
 * pageActor.Add(backPageActor);\n
 * pageActor.SetShaderEffect ( pageTurnEffect ); \n
 *
 * //set initial values
 * pageTurnEffect.SetUniform("uPageSize", Vector2);\n
 * pageTurnEffect.SetUniform("uOriginalCenter", Vector2);\n
 * pageTurnEffect.SetUniform("uIsTurningBack", bool) SetIsTurningBack();\n
 * pageTurnEffect.SetUniform("uCurrentCenter",Vector2);\n
 *
 * //Animate it with the current center property\n
 * Animation animation[mAnimationIndex] = Animation::New( ... );\n
 * animation.AnimateTo(Property( pageTurnEffect, "uCurrentCenter" ),
 *                            currentCenter,
 *                            AlphaFunction::...);\n
 * animation[mAnimationIndex].Play(); \n
 *
 * Animatable/Constrainable uniforms:
 *
 * @param[in] enableBlending If true, apply HINT_BLENDING when creating the shader object; If false, disable the HINT_BLENDING
 * @return A handle to a newly allocated ShaderEffect
 */
ShaderEffect CreatePageTurnEffect(bool enableBlending = true);

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_SC_CURVE_EFFECT_H_ */
