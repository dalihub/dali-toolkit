#ifndef __DALI_TOOLKIT_SCROLL_VIEW_PAGE_SPIRAL_EFFECT_H__
#define __DALI_TOOLKIT_SCROLL_VIEW_PAGE_SPIRAL_EFFECT_H__

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
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-effect.h>

namespace Dali
{

class Actor;

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class ScrollViewPageSpiralEffect;
}

/**
 * @brief This effect cause each page in a scroll-view to move along a spiral.
 *
 * It should be used on the following Actor hierarchy:
 *
 * ScrollView
 * |
 * Page (1..n)
 *
 * You should ensure ScrollView's default constraints have been removed,
 * by calling ScrollView::RemoveConstraintsFromChildren() before applying
 * this effect to ScrollView.
 *
 * Manual operation:
 * ApplyToPage(...) method should be called on every page.
 *
 * Automatic operation:
 * not implemented.
 */
class DALI_IMPORT_API ScrollViewPageSpiralEffect : public ScrollViewEffect
{

public:

  /**
   * @brief Create an initialized ScrollViewPageSpiralEffect.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static ScrollViewPageSpiralEffect New();

  /**
   * @brief Create an uninitialized ScrollViewPageSpiralEffect; this can be initialized with ScrollViewPageSpiralEffect::New().
   *
   * Calling member functions with an uninitialized Toolkit::ScrollViewPageSpiralEffect is not allowed.
   */
  ScrollViewPageSpiralEffect();

  /**
   * @brief Downcast an Object handle to ScrollViewPageSpiralEffect.
   *
   * If handle points to a ScrollViewPageSpiralEffect the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ScrollViewPageSpiralEffect or an uninitialized handle
   */
  static ScrollViewPageSpiralEffect DownCast( BaseHandle handle );

  /**
   * @brief Manually apply effect to a page in the scroll-view.
   *
   * @param[in] page The page to be affected by this effect.
   * @param[in] spiralAngle The spirald angle (in radians).
   *
   * @note If the wrap mode of the scroll view is changed, then this needs to be called for every
   * page again after removing the previous constraints.
   */
  void ApplyToPage( Actor page, const Vector2& spiralAngle );

protected:

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] impl A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ScrollViewPageSpiralEffect( Internal::ScrollViewPageSpiralEffect *impl );

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_VIEW_PAGE_SPIRAL_EFFECT_H__
