#ifndef DALI_TOOLKIT_SCROLL_VIEW_SCROLL_MODE_H
#define DALI_TOOLKIT_SCROLL_VIEW_SCROLL_MODE_H

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
 */

namespace Dali
{
namespace Toolkit
{
/**
 * @addtogroup dali_toolkit_controls_scroll_view
 * @{
 */

/**
 * @brief Properties to use in a Property::MAP when setting a ScrollView's scroll mode.
 * @SINCE_1_2.60
 */
namespace ScrollMode
{
/**
 * @brief Properties to use in a Property::MAP when setting a ScrollView's scroll mode.
 * @SINCE_1_2.60
 */
enum Type
{
  /**
   * @brief Whether the content can be scrolled along the X axis or not.
   * @details Name "xAxisScrollEnabled", type Property::BOOLEAN.
   * @SINCE_1_2.60
   */
  X_AXIS_SCROLL_ENABLED,

  /**
   * @brief When set, causes scroll view to snap to multiples of the
   * value of the interval while flicking along the X axis.
   * @details Name "xAxisSnapToInterval", type Property::FLOAT
   * @SINCE_1_2.60
   * @note By default, there is no snapping.
   */
  X_AXIS_SNAP_TO_INTERVAL,

  /**
   * @brief When set, the scroll view is unable to scroll beyond the
   * value of the boundary along the X axis.
   * @details Name "xAxisScrollBoundary", type Property::FLOAT.
   * @SINCE_1_2.60
   * @note By default, there is no boundary.
   */
  X_AXIS_SCROLL_BOUNDARY,

  /**
   * @brief Whether the content can be scrolled along the Y axis or not.
   * @details Name "yAxisScrollEnabled", type Property::BOOLEAN
   * @SINCE_1_2.60
   */
  Y_AXIS_SCROLL_ENABLED,

  /**
   * @brief When set, causes scroll view to snap to multiples of the
   * value of the interval while flicking along the Y axis.
   * @details Name "yAxisSnapToInterval", type Property::FLOAT.
   * @SINCE_1_2.60
   * @note By default, there is no snapping.
   */
  Y_AXIS_SNAP_TO_INTERVAL,

  /**
   * @brief When set, the scroll view is unable to scroll beyond the
   * value of the boundary along the Y axis.
   * @details Name "yAxisScrollBoundary", type Property::FLOAT.
   * @SINCE_1_2.60
   * @note By default, there is no boundary.
   */
  Y_AXIS_SCROLL_BOUNDARY
};

} // namespace ScrollMode

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_SCROLL_VIEW_SCROLL_MODE_H
