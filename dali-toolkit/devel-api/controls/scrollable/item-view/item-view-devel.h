#ifndef DALI_TOOLKIT_ITEM_VIEW_DEVEL_H
#define DALI_TOOLKIT_ITEM_VIEW_DEVEL_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-view.h>

namespace Dali
{

namespace Toolkit
{

namespace DevelItemView
{

namespace Property
{

enum
{
  // Event side properties
  MINIMUM_SWIPE_SPEED          = Dali::Toolkit::ItemView::Property::MINIMUM_SWIPE_SPEED,
  MINIMUM_SWIPE_DISTANCE       = Dali::Toolkit::ItemView::Property::MINIMUM_SWIPE_DISTANCE,
  WHEEL_SCROLL_DISTANCE_STEP   = Dali::Toolkit::ItemView::Property::WHEEL_SCROLL_DISTANCE_STEP,
  SNAP_TO_ITEM_ENABLED         = Dali::Toolkit::ItemView::Property::SNAP_TO_ITEM_ENABLED,
  REFRESH_INTERVAL             = Dali::Toolkit::ItemView::Property::REFRESH_INTERVAL,

  /**
   * @brief The layout used.
   * @details Name "layout", type Property::ARRAY
   * @see SetLayout()
   */
  LAYOUT                       = REFRESH_INTERVAL + 1,

  // Animatable properties
  LAYOUT_POSITION              = Dali::Toolkit::ItemView::Property::LAYOUT_POSITION,
  SCROLL_SPEED                 = Dali::Toolkit::ItemView::Property::SCROLL_SPEED,
  OVERSHOOT                    = Dali::Toolkit::ItemView::Property::OVERSHOOT,
  SCROLL_DIRECTION             = Dali::Toolkit::ItemView::Property::SCROLL_DIRECTION,
  LAYOUT_ORIENTATION           = Dali::Toolkit::ItemView::Property::LAYOUT_ORIENTATION,
  SCROLL_CONTENT_SIZE          = Dali::Toolkit::ItemView::Property::SCROLL_CONTENT_SIZE
};

} // namespace Property

} // namespace DevelItemView

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_ITEM_VIEW_DEVEL_H
