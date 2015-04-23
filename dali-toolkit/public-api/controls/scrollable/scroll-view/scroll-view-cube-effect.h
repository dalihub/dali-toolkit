#ifndef __DALI_TOOLKIT_SCROLL_VIEW_CUBE_EFFECT_H__
#define __DALI_TOOLKIT_SCROLL_VIEW_CUBE_EFFECT_H__

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
class ScrollViewCubeEffect;
}

/**
 * @brief This effect causes Actors to appear to rotate around a 3D cube.
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
class DALI_IMPORT_API ScrollViewCubeEffect : public ScrollViewEffect
{

public:

  /**
   * @brief Create an initialized ScrollViewCubeEffect.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static ScrollViewCubeEffect New();

  /**
   * @brief Create an uninitialized ScrollViewCubeEffect; this can be initialized with ScrollViewCubeEffect::New().
   *
   * Calling member functions with an uninitialized Toolkit::ScrollViewCubeEffect is not allowed.
   */
  ScrollViewCubeEffect();

  /**
   * @brief Downcast an Object handle to ScrollViewCubeEffect.
   *
   * If handle points to a ScrollViewCubeEffect the downcast produces
   * valid handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return handle to a ScrollViewCubeEffect or an uninitialized handle
   */
  static ScrollViewCubeEffect DownCast( BaseHandle handle );

  /**
   * @brief Manually apply effect to an Actor.
   *
   * @param[in] child The child Actor to be affected by this effect.
   * @param[in] anchor The anchor point that the child actor should
   * rotate around when scrolling
   * @param[in] angleSwing The maximum amount the child actor should
   * rotate in radians for each axis (X and Y) as the page is scrolled.
   * @param[in] positionSwing The maximum amount the child actor should
   * move for each axis (X and Y) as the page is scrolled.
   */
  void ApplyToActor(Actor child,
                    const Vector3& anchor,
                    const Vector2& angleSwing,
                    const Vector2& positionSwing);

  /**
   * @brief Manually apply effect to an Actor.
   *
   * @param[in] child The child Actor to be affected by this effect.
   * @param[in] parentPage The parent page Actor to be used by this effect.
   * @param[in] anchor The anchor point that the child actor should
   * rotate around when scrolling
   * @param[in] angleSwing The maximum amount the child actor should
   * rotate in radians for each axis (X and Y) as the page is scrolled.
   * @param[in] positionSwing The maximum amount the child actor should
   * move for each axis (X and Y) as the page is scrolled.
   */
  void ApplyToActor(Actor child,
                    Actor parentPage,
                    const Vector3& anchor,
                    const Vector2& angleSwing,
                    const Vector2& positionSwing);

protected:

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] impl A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ScrollViewCubeEffect(Internal::ScrollViewCubeEffect *impl);

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_VIEW_CUBE_EFFECT_H__
