#ifndef __DALI_TOOLKIT_ENUMS_H__
#define __DALI_TOOLKIT_ENUMS_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
/**
 * @brief DALi Toolkit namespace.
 * @SINCE_1_0.0
 */
namespace Toolkit
{
/**
 * @addtogroup dali_toolkit_controls
 * @{
 */

/**
 * @brief Control Orientation namespace.
 * @SINCE_1_0.0
 */
namespace ControlOrientation
{

/**
 * @brief The internal orientation of a control.
 * @SINCE_1_0.0
 */
enum Type
{
  Up,   ///< The contents of control are in a vertical layout, from top to bottom @SINCE_1_0.0
  Left, ///< The contents of control are in a horizontal layout, from left to right @SINCE_1_0.0
  Down, ///< The contents of control are in a vertical layout, from bottom to top @SINCE_1_0.0
  Right ///< The contents of control are in a horizontal layout, from right to left @SINCE_1_0.0
};

} // namespace ControlOrientation

/**
 * @brief Query whether an orientation is vertical.
 *
 * @param[in] orientation The orientation.
 * @return True if the orientation is vertical.
 */
DALI_IMPORT_API bool IsVertical(ControlOrientation::Type orientation);

/**
 * @brief Query whether an orientation is horizontal.
 *
 * @SINCE_1_0.0
 * @param[in] orientation The orientation.
 * @return True if the orientation is horizontal.
 */
DALI_IMPORT_API bool IsHorizontal(ControlOrientation::Type orientation);

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_ENUMS_H__
