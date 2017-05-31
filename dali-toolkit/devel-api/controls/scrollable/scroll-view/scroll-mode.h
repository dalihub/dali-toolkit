#ifndef DALI_TOOLKIT_DEVEL_API_SCROLL_VIEW_SCROLL_MODE_H
#define DALI_TOOLKIT_DEVEL_API_SCROLL_VIEW_SCROLL_MODE_H
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
 */
namespace Dali
{
namespace Toolkit
{
namespace ScrollMode
{

enum Type
{
  /**
   * @brief True if the content can be scrolled in X axis or false if
   * not.
   *
   * @details Name "xAxisScrollEnabled", type Property::BOOLEAN
   */
  X_AXIS_SCROLL_ENABLED,

  /**
   * @brief When set, causes scroll view to snap to multiples of the
   * value of the interval in the X axis while flicking.
   *
   * By default, there is no snapping.
   * @details Name "xAxisSnapToInterval", type Property::FLOAT
   */
  X_AXIS_SNAP_TO_INTERVAL,

  /**
   * @brief When set, causes scroll view unable to scroll beyond the
   * value of the boundary in the X axis.
   *
   * By default, there is no boundary.
   * @details Name "xAxisScrollBoundary", type Property::FLOAT
   */
  X_AXIS_SCROLL_BOUNDARY,

  /**
   * @brief True if the content can be scrolled in Y axis or false if
   * not.
   *
   * @details Name "yAxisScrollEnabled", type Property::BOOLEAN
   */
  Y_AXIS_SCROLL_ENABLED,

  /**
   * @brief When set, causes scroll view to snap to multiples of the
   * value of the interval in the Y axis while flicking.
   *
   * By default, there is no snapping.
   * @details Name "yAxisSnapToInterval", type Property::FLOAT
   */
  Y_AXIS_SNAP_TO_INTERVAL,

  /**
   * @brief When set, causes scroll view unable to scroll beyond the
   * value of the boundary in the Y axis.
   *
   * By default, there is no boundary.
   * @details Name "yAxisScrollBoundary", type Property::FLOAT
   */
  Y_AXIS_SCROLL_BOUNDARY
};

} // ScrollMode
} // toolkit
} // Dali

#endif //DALI_TOOLKIT_DEVEL_API_SCROLL_VIEW_SCROLL_MODE_H
