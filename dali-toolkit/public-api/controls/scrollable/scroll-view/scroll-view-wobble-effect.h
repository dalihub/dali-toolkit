#ifndef __DALI_TOOLKIT_SCROLL_VIEW_WOBBLE_EFFECT_H__
#define __DALI_TOOLKIT_SCROLL_VIEW_WOBBLE_EFFECT_H__

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

namespace Toolkit
{

class ScrollViewEffect;

namespace Internal DALI_INTERNAL
{
class ScrollViewWobbleEffect;
}

/**
 * ScrollView Wobble-Effect.
 *
 * Using this effect, a %wobble% property is produced which swings
 * towards the origin in accordance to how the user pans the ScrollView.
 */
class DALI_IMPORT_API ScrollViewWobbleEffect : public ScrollViewEffect
{
public:

  static const std::string EFFECT_OVERSHOOT;
  static const std::string EFFECT_TIME;

public:

  /**
   * Create an initialized ScrollViewWobbleEffect.
   * @return A handle to a newly allocated Dali resource.
   */
  static ScrollViewWobbleEffect New();

  /**
   * Create an uninitialized ScrollViewWobbleEffect; this can be initialized with ScrollViewWobbleEffect::New()
   * Calling member functions with an uninitialized Toolkit::ScrollViewWobbleEffect is not allowed.
   */
  ScrollViewWobbleEffect();

protected:

  /**
   * This constructor is used by Dali New() methods.
   * @param [in] impl A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ScrollViewWobbleEffect(Internal::ScrollViewWobbleEffect *impl);

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_VIEW_WOBBLE_EFFECT_H__
