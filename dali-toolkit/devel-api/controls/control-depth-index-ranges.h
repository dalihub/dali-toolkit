#ifndef DALI_TOOLKIT_DEVEL_CONTROL_DEPTH_INDEX_RANGES_H
#define DALI_TOOLKIT_DEVEL_CONTROL_DEPTH_INDEX_RANGES_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/actors/layer-devel.h>

namespace Dali
{

namespace Toolkit
{

namespace DepthIndex
{

// The negative value for background effect and background has been
// chosen so that newer controls can have content without setting
// depth index, and go in front of native controls with a background.
// This backround negative value means that the highest possible value
// is SIBLING_ORDER_MULTIPLIER-BACKGROUND_EFFECT-1.  The divisor of
// 100 ensures the range fits within the sibling order range, and has
// enough gaps to allow Control Authors to use other intermediate depths.

enum Ranges
{
  BACKGROUND_EFFECT = -2 * DevelLayer::SIBLING_ORDER_MULTIPLIER/100,
  BACKGROUND    =     -1 * DevelLayer::SIBLING_ORDER_MULTIPLIER/100,
  CONTENT       =      0,
  DECORATION    =      1 * DevelLayer::SIBLING_ORDER_MULTIPLIER/100,
  FOREGROUND_EFFECT =  2 * DevelLayer::SIBLING_ORDER_MULTIPLIER/100
};

static_assert( (unsigned int)DevelLayer::ACTOR_DEPTH_MULTIPLIER > (unsigned int)DevelLayer::SIBLING_ORDER_MULTIPLIER );
static_assert( BACKGROUND_EFFECT < BACKGROUND );
static_assert( BACKGROUND < CONTENT );
static_assert( CONTENT < DECORATION );
static_assert( DECORATION < FOREGROUND_EFFECT );

} // namespace DepthIndex

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_CONTROL_DEPTH_INDEX_RANGES_H
