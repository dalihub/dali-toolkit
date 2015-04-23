#ifndef __DALI_TOOLKIT_SCROLL_VIEW_CAROUSEL_EFFECT_H__
#define __DALI_TOOLKIT_SCROLL_VIEW_CAROUSEL_EFFECT_H__

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
class ScrollViewCarouselEffect;
}

/**
 * ScrollView Carousel-Effect.
 *
 * This effect causes Actors to appear to move around around a carousel
 * It should be used on the following Actor hierarchy:
 *
 * ScrollView
 * |
 * Container
 * |
 * Child (1..n)
 *
 * You should ensure ScrollView's default constraints have been removed,
 * by calling ScrollView::RemoveConstraintsFromChildren() before applying
 * this effect to ScrollView.
 *
 * Manual operation:
 * upon adding children to container, the ApplyToActor(...) method should be called.
 *
 * Automatic operation:
 * not implemented.
 *
 * Notes:
 * * Assumes Actor's AnchorPoint = AnchorPoint::CENTER
 */
class DALI_IMPORT_API ScrollViewCarouselEffect : public ScrollViewEffect
{

public:

  static const std::string EFFECT_ACTIVATE;     ///< Activation property (a value between: 0.0 normal/no effect, 1.0 - full effect)

public:

  /**
   * Create an initialized ScrollViewCarouselEffect.
   * @return A handle to a newly allocated Dali resource.
   */
  static ScrollViewCarouselEffect New();

  /**
   * Create an uninitialized ScrollViewCarouselEffect; this can be initialized with ScrollViewCarouselEffect::New()
   * Calling member functions with an uninitialized Toolkit::ScrollViewCarouselEffect is not allowed.
   */
  ScrollViewCarouselEffect();

  /**
   * Downcast an Object handle to ScrollViewCarouselEffect. If handle points to a ScrollViewCarouselEffect the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ScrollViewCarouselEffect or an uninitialized handle
   */
  static ScrollViewCarouselEffect DownCast( BaseHandle handle );

  /**
   * Manually apply effect to an Actor.
   * @param[in] child The child Actor to be affected by this effect.
   * @param[in] angleSwing The maximum amount the child actor should
   * rotate in radians for each axis (X and Y) as the page is scrolled.
   */
  void ApplyToActor(Actor child, const Vector2& angleSwing);

protected:

  /**
   * This constructor is used by Dali New() methods.
   * @param [in] impl A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ScrollViewCarouselEffect(Internal::ScrollViewCarouselEffect *impl);

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_VIEW_CAROUSEL_EFFECT_H__
