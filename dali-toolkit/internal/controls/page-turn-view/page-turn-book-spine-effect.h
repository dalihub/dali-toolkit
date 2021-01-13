#ifndef DALI_PAGE_TURN_BOOK_SPINE_EFFECT_H
#define DALI_PAGE_TURN_BOOK_SPINE_EFFECT_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * @brief Creates a new PageTurnBookSpineEffect
 * This is an assisting effect of PageTurnEffect to display a book spine on _static_ pages, and also to flip the image horizontally when needed.
 *
 * When the page is turned over in landscape, call
 * SetIsBackImageVisible(true), this effect can display the back image
 * correctly after the page been rotated 180 degrees.  To
 * display the pages visually consistent with its turning state,
 * please set the uniforms with the same values as the PageTurnEffect.
 *
 * Animatable/Constrainable uniforms:
 *  "uSpineShadowParameter" - The two parameters are the major&minor radius (in pixels) to form an ellipse shape. The top-left
 *                            quarter of this ellipse is used to calculate spine normal for simulating shadow *
 *  "uTextureWidth" - 1.0 for single sided page,
 *                    2.0 for double sided image which has left half part as page front side and right half part as page back side.
 *
 * @return The newly created Property::Map with the page turn book spine effect
 **/
inline Property::Map CreatePageTurnBookSpineEffect()
{
  Property::Map map;

  Property::Map customShader;

  customShader[ Toolkit::Visual::Shader::Property::VERTEX_SHADER ] = SHADER_PAGE_TURN_BOOK_SPINE_EFFECT_VERT.data();
  customShader[ Toolkit::Visual::Shader::Property::FRAGMENT_SHADER ] = SHADER_PAGE_TURN_BOOK_SPINE_EFFECT_FRAG.data();

  map[ Toolkit::Visual::Property::SHADER ] = customShader;
  return map;
}

} //namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_PAGE_TURN_BOOK_SPINE_EFFECT_H
