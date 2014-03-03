#ifndef __DALI_TOOLKIT_ENUMS_H__
#define __DALI_TOOLKIT_ENUMS_H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace ControlOrientation
{

/**
 * The internal orientation a control.
 */
enum Type
{
  Up,   ///< The contents of control are in a vertical layout, from top to bottom
  Left, ///< The contents of control are in a horizontal layout, from left to right
  Down, ///< The contents of control are in a vertical layout, from bottom to top
  Right ///< The contents of control are in a horizontal layout, from right to left
};

} // namespace ControlOrientation

/**
 * Query whether an orientation is vertical.
 * @param[in] orientation The orientation.
 * @return True if the orientation is vertical.
 */
bool IsVertical(ControlOrientation::Type orientation);

/**
 * Query whether an orientation is horizontal.
 * @param[in] orientation The orientation.
 * @return True if the orientation is horizontal.
 */
bool IsHorizontal(ControlOrientation::Type orientation);

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_ENUMS_H__
