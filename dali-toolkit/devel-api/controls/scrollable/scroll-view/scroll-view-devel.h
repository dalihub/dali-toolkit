#ifndef DALI_TOOLKIT_SCROLL_VIEW_DEVEL_H
#define DALI_TOOLKIT_SCROLL_VIEW_DEVEL_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>

namespace Dali
{

namespace Toolkit
{

namespace DevelScrollView
{

namespace Property
{

enum
{
  // Event side properties

  WRAP_ENABLED = Dali::Toolkit::ScrollView::Property::WRAP_ENABLED,
  PANNING_ENABLED = Dali::Toolkit::ScrollView::Property::PANNING_ENABLED,
  AXIS_AUTO_LOCK_ENABLED = Dali::Toolkit::ScrollView::Property::AXIS_AUTO_LOCK_ENABLED,
  WHEEL_SCROLL_DISTANCE_STEP = Dali::Toolkit::ScrollView::Property::WHEEL_SCROLL_DISTANCE_STEP,

  /**
   * @brief The scroll mode
   * @details Name "scrollMode", type Property::MAP
   * The scroll mode map is a frontend for the Ruler helper class, containing the following keys:
   *
   * | %Property Name       | Type     | Required | Description                                                                                                                           |
   * |----------------------|----------|----------|---------------------------------------------------------------------------------------------------------------------------------------|
   * | xAxisScrollEnabled   | BOOLEAN  | No       | True if the content can be scrolled in X axis or false if not.                                                                        |
   * | xAxisSnapToInterval  | FLOAT    | No       | When set, causes scroll view to snap to multiples of the value of the interval in the X axis while flicking. (by default no snapping) |
   * | xAxisScrollBoundary  | FLOAT    | No       | When set, causes scroll view unable to scroll beyond the value of the boundary in the X axis (by default no boundary)                 |
   * | yAxisScrollEnabled   | BOOLEAN  | No       | True if the content can be scrolled in Y axis or false if not.                                                                        |
   * | yAxisSnapToInterval  | FLOAT    | No       | When set, causes scroll view to snap to multiples of the value of the interval in the Y axis while flicking. (by default no snapping) |
   * | yAxisScrollBoundary  | FLOAT    | No       | When set, causes scroll view unable to scroll beyond the value of the boundary in the Y axis (by default no boundary)                 |
   *
   * Alternatively, one can use the keys defined in the Dali::Toolkit::ScrollMode::Type enumeration.
   */
  SCROLL_MODE = WHEEL_SCROLL_DISTANCE_STEP + 1,
};

} // namespace Property

} // namespace DevelScrollView

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_SCROLL_VIEW_DEVEL_H
