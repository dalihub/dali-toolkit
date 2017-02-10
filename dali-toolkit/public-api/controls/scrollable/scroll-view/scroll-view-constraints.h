#ifndef __DALI_TOOLKIT_SCROLL_VIEW_CONSTRAINTS_H__
#define __DALI_TOOLKIT_SCROLL_VIEW_CONSTRAINTS_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/animation/constraint.h>

namespace Dali
{

struct Vector2;
struct Vector3;
struct Vector4;
class PropertyInput;

namespace Toolkit
{
/**
 * @addtogroup dali_toolkit_controls_scroll_view
 * @{
 */

// Constraints ////////////////////////////////////////////////////////////////////////////////////

/**
 * Useful constraints to apply to a ScrollView.
 */

/**
 * @brief Moves an Actor in accordance with the scroll position.
 *
 * @SINCE_1_0.0
 * @param[in] current Current position
 * @param[in] inputs The position input that Actor will move
 */
DALI_IMPORT_API void MoveActorConstraint( Vector3& current, const PropertyInputContainer& inputs );

/**
 * @brief Wraps an Actor's position in accordance with the min/max bounds of domain.
 *
 * @SINCE_1_0.0
 * @param[in] position Position to be wrapped
 * @param[in] inputs The input that Actor's position will be wrapped
 */
DALI_IMPORT_API void WrapActorConstraint( Vector3& position, const PropertyInputContainer& inputs );

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_VIEW_CONSTRAINTS_H__
