#ifndef __DALI_TOOLKIT_SCROLL_VIEW_PAGE_PATH_EFFECT_H__
#define __DALI_TOOLKIT_SCROLL_VIEW_PAGE_PATH_EFFECT_H__

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
class Path;

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class ScrollViewPagePathEffect;
}
/**
 * @addtogroup dali-toolkit-controls-scroll-view
 * @{
 */

/**
 * ScrollView Page Path Effect.
 * This effect causes Actors to follow a given path. The opacity of the actor will be 0.0 at
 * the beginning of the path and will go to 1.0 as it is approximating to half of the path to return
 * to 0.0 at the end of the path
 *
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
class DALI_IMPORT_API ScrollViewPagePathEffect : public ScrollViewEffect
{

public:

  /**
   * Create an initialized ScrollViewPagePathEffect.
   * @param[in] path The path that will be used by the scroll effect
   * @param[in] forward Vector in page object space which will be aligned with the tangent of the path
   * @param[in] inputPropertyIndex Index of a property of the scroll-view which will be used as the input for the path.
   * @param[in] pageSize Size of a page in the scrollview
   * @param[in] pageCount Total number of pages in the scrollview
   * @return A handle to a newly allocated Dali resource.
   */
  static ScrollViewPagePathEffect New(Path path, const Vector3& forward, Dali::Property::Index inputPropertyIndex, const Vector3& pageSize, unsigned int pageCount);

  /**
   * Create an uninitialized ScrollViewPagePathEffect; this can be initialized with ScrollViewPagePathEffect::New()
   * Calling member functions with an uninitialized Toolkit::ScrollViewPagePathEffect is not allowed.
   */
  ScrollViewPagePathEffect();

  /**
   * Downcast an Object handle to ScrollViewPagePathEffect. If handle points to a ScrollViewPagePathEffect the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ScrollViewPagePathEffect or an uninitialized handle
   */
  static ScrollViewPagePathEffect DownCast( BaseHandle handle );

  /**
   * Manually apply effect to a page in the scroll-view.
   * @param[in] page The page to be affected by this effect.
   * @param[in] pageOrder The order of the page in the scroll-view
   */
  void ApplyToPage( Actor page, unsigned int pageOrder );

protected:

  /**
   * This constructor is used by Dali New() methods.
   * @param [in] impl A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ScrollViewPagePathEffect( Internal::ScrollViewPagePathEffect *impl );

};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_VIEW_PAGE_PATH_EFFECT_H__
