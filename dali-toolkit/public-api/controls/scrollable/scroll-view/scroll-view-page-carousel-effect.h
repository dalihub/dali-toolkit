#ifndef __DALI_TOOLKIT_SCROLL_VIEW_PAGE_CAROUSEL_EFFECT_H__
#define __DALI_TOOLKIT_SCROLL_VIEW_PAGE_CAROUSEL_EFFECT_H__

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
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-effect.h>

namespace Dali
{

class Actor;

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class ScrollViewPageCarouselEffect;
}

/**
 * ScrollView Page Carousel Effect.
 *
 * This effect cause each page in a scroll-view to rotate round a 3D carousel.
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
class DALI_IMPORT_API ScrollViewPageCarouselEffect : public ScrollViewEffect
{

public:

  /**
   * Create an initialized ScrollViewPageCarouselEffect.
   * @return A handle to a newly allocated Dali resource.
   */
  static ScrollViewPageCarouselEffect New();

  /**
   * Create an uninitialized ScrollViewPageCarouselEffect; this can be initialized with ScrollViewPageCarouselEffect::New()
   * Calling member functions with an uninitialized Toolkit::ScrollViewPageCarouselEffect is not allowed.
   */
  ScrollViewPageCarouselEffect();

  /**
   * Downcast an Object handle to ScrollViewPageCarouselEffect. If handle points to a ScrollViewPageCarouselEffect the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ScrollViewPageCarouselEffect or an uninitialized handle
   */
  static ScrollViewPageCarouselEffect DownCast( BaseHandle handle );

  /**
   * Manually apply effect to a page in the scroll-view.
   * @param[in] page The page to be affected by this effect.
   * @param[in] positionToPageSizeRatio A ratio of the size of the page which determines the amount the page will move as it fades out.
   *                                    Default is moving it by the page size.
   */
  void ApplyToPage( Actor page, const Vector2& positionToPageSizeRatio = Vector2::ONE );

protected:

  /**
   * This constructor is used by Dali New() methods.
   * @param [in] impl A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ScrollViewPageCarouselEffect( Internal::ScrollViewPageCarouselEffect *impl );

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_VIEW_PAGE_CAROUSEL_EFFECT_H__
