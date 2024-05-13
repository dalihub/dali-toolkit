#ifndef DALI_TOOLKIT_DEVEL_CONTROL_DEPTH_INDEX_RANGES_H
#define DALI_TOOLKIT_DEVEL_CONTROL_DEPTH_INDEX_RANGES_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
// The lowest possible value is MINIMUM_DEPTH_INDEX, and highest possible
// value is MAXIMUM_DEPTH_INDEX. The divisor of 10 ensures the range fits
// within the sibling order range, and has enough gaps to allow Control
// Authors to use other intermediate depths.
//
// Note : AUTO_INDEX is special value. It means that the depth index will be
// automatically assigned when we register visual into the control.
// Other values will be clamped to the range.

enum Ranges
{
  MINIMUM_DEPTH_INDEX = -5 * DevelLayer::SIBLING_ORDER_MULTIPLIER / 10 + 1,
  BACKGROUND_EFFECT   = -2 * DevelLayer::SIBLING_ORDER_MULTIPLIER / 10,
  BACKGROUND          = -1 * DevelLayer::SIBLING_ORDER_MULTIPLIER / 10,
  CONTENT             = 0,
  DECORATION          = 1 * DevelLayer::SIBLING_ORDER_MULTIPLIER / 10,
  FOREGROUND_EFFECT   = 2 * DevelLayer::SIBLING_ORDER_MULTIPLIER / 10,
  MAXIMUM_DEPTH_INDEX = 5 * DevelLayer::SIBLING_ORDER_MULTIPLIER / 10,

  AUTO_INDEX = MINIMUM_DEPTH_INDEX - 1, ///< Special value to indicate that the depth index should be automatically calculated.
                                        ///  If visual replaced by another visual, the depth index of the new visual will be set to previous visual.
                                        ///  Otherwise, depth index will be set as the maximum depth index + 1
                                        ///  what given control already has, or CONTENT if no visuals.
};

static_assert((unsigned int)DevelLayer::ACTOR_DEPTH_MULTIPLIER > (unsigned int)DevelLayer::SIBLING_ORDER_MULTIPLIER);
static_assert(MINIMUM_DEPTH_INDEX < BACKGROUND_EFFECT);
static_assert(BACKGROUND_EFFECT < BACKGROUND);
static_assert(BACKGROUND < CONTENT);
static_assert(CONTENT < DECORATION);
static_assert(DECORATION < FOREGROUND_EFFECT);
static_assert(FOREGROUND_EFFECT < MAXIMUM_DEPTH_INDEX);

// AUTO_INDEX should not be inside of the valid range.
static_assert(!(MINIMUM_DEPTH_INDEX <= AUTO_INDEX && AUTO_INDEX <= MAXIMUM_DEPTH_INDEX));

} // namespace DepthIndex

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_CONTROL_DEPTH_INDEX_RANGES_H
