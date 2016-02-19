#ifndef __DALI_TOOLKIT_DEFAULT_ITEM_LAYOUT_H__
#define __DALI_TOOLKIT_DEFAULT_ITEM_LAYOUT_H__

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
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-layout.h>

namespace Dali
{

namespace Toolkit
{
/**
 * @addtogroup dali_toolkit_controls_item_view
 * @{
 */

/**
 * @brief Default item layout mode.
 * @SINCE_1_0.0
 */
namespace DefaultItemLayout
{

enum Type
{
  DEPTH,     ///< Items arranged in a grid, scrolling along the Z-Axis. @SINCE_1_0.0
  GRID,      ///< Items arranged in a grid, scrolling along the Y-Axis. @SINCE_1_0.0
  LIST,      ///< One item per line, scrolling along the Y-Axis. @SINCE_1_0.0
  SPIRAL     ///< Items arranged in a spiral, centered around the Y-Axis. @SINCE_1_0.0
};

/**
 * @brief Creates a built-in default item-layout.
 *
 * @SINCE_1_0.0
 * @param[in]  type  The type of layout required.
 *
 * @return An ItemLayoutPtr to the newly created layout.
 */
DALI_IMPORT_API ItemLayoutPtr New( Type type );

} // namespace DefaultItemLayout

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_DEFAULT_ITEM_LAYOUT_H__
