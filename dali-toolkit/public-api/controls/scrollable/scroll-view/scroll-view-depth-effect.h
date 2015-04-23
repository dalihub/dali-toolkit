#ifndef __DALI_TOOLKIT_SCROLL_VIEW_DEPTH_EFFECT_H__
#define __DALI_TOOLKIT_SCROLL_VIEW_DEPTH_EFFECT_H__

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

class ScrollViewEffect;

namespace Internal DALI_INTERNAL
{
class ScrollViewDepthEffect;
}

/**
 * ScrollView Depth-Effect.
 *
 * This effect causes Actors to appear to scroll off the page
 * at different speeds.
 *
 * It should be used on the following Actor hierarchy:
 *
 * ScrollView
 * |
 * Page (1..n)
 * |
 * Child (1..m)
 *
 * You should ensure ScrollView's default constraints have been removed,
 * by calling ScrollView::RemoveConstraintsFromChildren() before applying
 * this effect to ScrollView.
 *
 * Manual operation:
 * upon adding children to pages, the ApplyToActor(...) method should be called.
 *
 * Automatic operation:
 * not implemented.
 */
class DALI_IMPORT_API ScrollViewDepthEffect : public ScrollViewEffect
{

public:

  /**
   * Create an initialized ScrollViewDepthEffect.
   * @return A handle to a newly allocated Dali resource.
   */
  static ScrollViewDepthEffect New();

  /**
   * Create an uninitialized ScrollViewDepthEffect; this can be initialized with ScrollViewDepthEffect::New()
   * Calling member functions with an uninitialized Toolkit::ScrollViewDepthEffect is not allowed.
   */
  ScrollViewDepthEffect();

  /**
   * Downcast an Object handle to ScrollViewDepthEffect. If handle points to a ScrollViewDepthEffect the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ScrollViewCubeEffect or an uninitialized handle
   */
  static ScrollViewDepthEffect DownCast( BaseHandle handle );

  /**
   * Manually apply effect to an Actor.
   * @param[in] child The child Actor to be affected by this effect.
   * @param[in] positionExtent Controls how much Actor's X and Y
   * position affects their alpha function's exponent value
   * @param[in] offsetExtent exponent offset for X and Y scrolling
   * axes.
   * @param[in] positionScale Changes the amount the page as a whole
   * moves by.
   * @param[in] scaleExtent Scale factor to reach when page is one whole
   * page away from screen.
   */
  void ApplyToActor(Actor child,
                    const Vector2& positionExtent,
                    const Vector2& offsetExtent,
                    float positionScale,
                    float scaleExtent);

protected:

  /**
   * This constructor is used by Dali New() methods.
   * @param [in] impl A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ScrollViewDepthEffect(Internal::ScrollViewDepthEffect *impl);

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_VIEW_DEPTH_EFFECT_H__
