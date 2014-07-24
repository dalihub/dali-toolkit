#ifndef __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_HELPER_FUNCTIONS_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_HELPER_FUNCTIONS_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/dali.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/// Some helper methods with common functionality used in scroll view constraints
namespace ScrollViewHelperFunctions
{

/**
 * Checks whether the we're looking straight at the page and if we are, it returns true.
 *
 * @param[in]  position  The position of the page.
 */
bool IsStraightOnView( const Vector3& position );

/**
 * Modifies the position to wrap within the given domain.
 *
 * @param[in/out]  position  The position of the page, this is modified accordingly.
 * @param[in]      pageSize  The size of each page.
 * @param[in]      min       The minimum position of the scroll-view.
 * @param[in]      max       The maximum position of the scroll-view.
 */
void WrapPositionWithinDomain( Vector3& position, const Vector3& pageSize, const Vector3& min, const Vector3& max );

/**
 * Checks whether the page is positioned outside of our view and returns true if it is.
 *
 * @param[in]  position  The position of the page.
 * @param[in]  pageSize  The size of each page.
 */
bool IsOutsideView( const Vector3& position, const Vector3& pageSize );

} // namespace ScrollViewHelperFunctions

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_HELPER_FUNCTIONS_H__
