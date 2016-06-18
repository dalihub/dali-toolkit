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

namespace Dali
{

namespace Toolkit
{

namespace Internal
{


/**
 * @brief Re-applies PageTurnEffect internal constraints
 * The internal constraint uses the OriginalCenter property and the CURRENT_CENTER Property
 * to update the variety of common parameters which are with the same value for all the vertices.
 * Note: For each actor, the constraints are applied in the same order as the calls to Actor::ApplyConstraint().
 * So if there are other contraints applied to the ORIGINAL_CENTER or CURRENT_CENTER while when using this effect,
 * call this method to get the internal constraints and re-apply it afterwards.
 *
 * @param[in] actor The page turn actor to which internal constraints should be re-applied
 * @param[in] pageHeight The page height.
 */
void PageTurnApplyInternalConstraint( Actor& actor, float pageHeight );

/**
 * @brief Create a new PageTurnEffect
 * PageTurnEffect is a custom shader to achieve page turn effect for image views.
 *
 * Usage example:-
 *
 * // create shader used for doing page-turn effect\n
 * Property::Map pageTurnEffect = CreatePageTurnEffect();
 *
 * // set image view custom shader to the page-turn one\n
 * // for portrait view, the image is rendered as the front side of  page\n
 * // for landscape view, the back side becomes visible when the page is turned over. \n
 * //     in this case, the left and right half of the image are renderer as the front and back side separately. \n
 * ImageView page = ImageView::New(....); \n
 * page.SetProperty ( ImageView::Property::IMAGE, pageTurnEffect ); \n
 *
 * //set initial values
 * page.SetProperty( page.GetPropertyIndex("uIsTurningBack",) bool );\n
 * page.SetProperty( page.GetPropertyIndex("uSpineShadowParameter",) Vector2 );\n
 * page.SetProperty( page.GetPropertyIndex("ORIGINAL_CENTER"), Vector2 );\n
 * page.SetProperty( page.GetPropertyIndex("CURRENT_CENTER"), Vector2 );\n
 * page.SetProperty( page.GetPropertyIndex("uCommonParameters"), Matrix );\n
 * page.SetProperty( page.GetPropertyIndex("uTextureWidth"), float ); // Set to 1.0 for single-sided or 2.0 for double-sided \n
 * PageTurnApplyInternalConstraint( page );\n
 *
 * //Animate it with the current center property\n
 * Animation animation = Animation::New( ... );\n
 * animation.AnimateTo(Property( page, "CURRENT_CENTER" ),
 *                               currentCenter,
 *                               AlphaFunction::...);\n
 * animation.Play(); \n
 *
 */
Property::Map CreatePageTurnEffect();

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_SC_CURVE_EFFECT_H_ */
