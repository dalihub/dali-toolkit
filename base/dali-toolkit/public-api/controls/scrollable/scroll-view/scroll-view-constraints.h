#ifndef __DALI_TOOLKIT_SCROLL_VIEW_CONSTRAINTS_H__
#define __DALI_TOOLKIT_SCROLL_VIEW_CONSTRAINTS_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

namespace Dali DALI_IMPORT_API
{

struct Vector2;
struct Vector3;
struct Vector4;
class PropertyInput;

namespace Toolkit
{

// Constraints ////////////////////////////////////////////////////////////////////////////////////

/**
 * Useful constraints to apply to a ScrollView.
 */

/**
 * Move Actor constraint.
 *
 * Moves an Actor in accordance to scroll position.
 */
Vector3 MoveActorConstraint(const Vector3&    current,
                            const PropertyInput& scrollPositionProperty);

/**
 * Move-Scaled Actor constraint.
 *
 * Moves an Actor in accordance to scroll position (and scroll scale).
 */
Vector3 MoveScaledActorConstraint(const Vector3&    current,
                                  const PropertyInput& scrollPositionProperty,
                                  const PropertyInput& scrollScaleProperty);

/**
 * Scale Actor constraint.
 *
 * Scales an Actor in accordance to scroll scale.
 */
Vector3 ScaleActorConstraint(const Vector3&    current,
                             const PropertyInput& scrollScaleProperty);

/**
 * Wrap Actor constraint.
 *
 * Wraps an Actors position in accordance to min/max bounds of domain.
 */
Vector3 WrapActorConstraint(const Vector3&    current,
                            const PropertyInput& actorScaleProperty,
                            const PropertyInput& actorAnchorPointProperty,
                            const PropertyInput& actorSizeProperty,
                            const PropertyInput& scrollPositionMin,
                            const PropertyInput& scrollPositionMax,
                            const PropertyInput& scrollWrap);

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_VIEW_CONSTRAINTS_H__
