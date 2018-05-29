#ifndef __DALI_TOOLKIT_SCROLL_VIEW_EFFECT_H__
#define __DALI_TOOLKIT_SCROLL_VIEW_EFFECT_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{

struct Vector2;
struct Vector3;
struct Vector4;
class PropertyInput;

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class ScrollViewEffect;
class ScrollViewWobbleEffect;
}
/**
 * @addtogroup dali_toolkit_controls_scroll_view
 * @{
 */

class ScrollView;
class ScrollViewEffect;

/**
 * @brief ScrollView Effect base class, used to apply custom effects to a
 * ScrollView instance.
 *
 * Such effects are purely logical (i.e. physics), and may produce
 * properties that can be used with visual effects, such as creating
 * constraints that are applied to ShaderEffects or Actors using these
 * properties as inputs.
 * @SINCE_1_0.0
 */
class DALI_TOOLKIT_API ScrollViewEffect : public Dali::BaseHandle
{

public:

  /**
   * @brief Creates an uninitialized ScrollViewEffect; this can only be initialized with derived classes.
   *
   * Calling member functions with an uninitialized Toolkit::BaseObject is not allowed.
   * @SINCE_1_0.0
   */
  ScrollViewEffect();

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @SINCE_1_0.0
   * @param[in] impl A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ScrollViewEffect(Internal::ScrollViewEffect *impl);
  /// @endcond

};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_VIEW_EFFECT_H__
