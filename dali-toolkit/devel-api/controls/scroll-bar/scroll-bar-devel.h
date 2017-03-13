#ifndef DALI_TOOLKIT_SCROLL_BAR_DEVEL_H
#define DALI_TOOLKIT_SCROLL_BAR_DEVEL_H

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
#include <dali-toolkit/public-api/controls/scroll-bar/scroll-bar.h>

namespace Dali
{

namespace Toolkit
{

namespace DevelScrollBar
{

namespace Property
{

/**
 * @brief ScrollBar is a UI component that can be linked to the scrollable objects
 * indicating the current scroll position of the scrollable object.
 *
 * Actions
 * | %Action Name           | Attributes              | Description                               |
 * |------------------------|-------------------------|-------------------------------------------|
 * | ShowIndicator          | Doesn't have attributes | Shows the scroll-indicator                |
 * | HideIndicator          | Doesn't have attributes | Hides the scroll-indicator                |
 * | ShowTransientIndicator | Doesn't have attributes | Shows the scroll-indicator for short time |
 */

/**
 * @brief Enumeration for the instance of properties belonging to the ScrollBar class.
 */
enum
{
  SCROLL_DIRECTION          = Dali::Toolkit::ScrollBar::Property::SCROLL_DIRECTION,
  INDICATOR_HEIGHT_POLICY   = Dali::Toolkit::ScrollBar::Property::INDICATOR_HEIGHT_POLICY,
  INDICATOR_FIXED_HEIGHT    = Dali::Toolkit::ScrollBar::Property::INDICATOR_FIXED_HEIGHT,
  INDICATOR_SHOW_DURATION   = Dali::Toolkit::ScrollBar::Property::INDICATOR_SHOW_DURATION,
  INDICATOR_HIDE_DURATION   = Dali::Toolkit::ScrollBar::Property::INDICATOR_HIDE_DURATION,
  SCROLL_POSITION_INTERVALS = Dali::Toolkit::ScrollBar::Property::SCROLL_POSITION_INTERVALS,
  INDICATOR_MINIMUM_HEIGHT  = Dali::Toolkit::ScrollBar::Property::INDICATOR_MINIMUM_HEIGHT,
  INDICATOR_START_PADDING   = Dali::Toolkit::ScrollBar::Property::INDICATOR_START_PADDING,
  INDICATOR_END_PADDING     = Dali::Toolkit::ScrollBar::Property::INDICATOR_END_PADDING,

  /**
   * @brief The duration that transient indicators will remain fully visible.
   * @details name "indicatorTransientDuration", type float.
   */
  INDICATOR_TRANSIENT_DURATION = INDICATOR_END_PADDING + 1
};

} // namespace Property

} // namespace DevelScrollBar

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_SCROLL_BAR_DEVEL_H
