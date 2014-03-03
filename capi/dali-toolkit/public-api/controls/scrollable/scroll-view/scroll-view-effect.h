#ifndef __DALI_TOOLKIT_SCROLL_VIEW_EFFECT_H__
#define __DALI_TOOLKIT_SCROLL_VIEW_EFFECT_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
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

class ScrollView;
class ScrollViewEffect;

typedef std::vector<ScrollViewEffect> ScrollViewEffectContainer;
typedef ScrollViewEffectContainer::iterator ScrollViewEffectIter;
typedef ScrollViewEffectContainer::const_iterator ScrollViewEffectConstIter;

/**
 * ScrollView Effect base class, used to apply custom
 * effects to a ScrollView instance. Such effects are
 * purely logical (i.e. physics), and may produce
 * properties that can be used with visual effects.
 * Such as creating constraints that are applied to ShaderEffects
 * or Actors using these properties as inputs.
 */
class ScrollViewEffect : public Dali::BaseHandle
{

public:

  /**
   * Create an uninitialized ScrollViewEffect; this can only be initialized with derived classes
   * Calling member functions with an uninitialized Toolkit::BaseObject is not allowed.
   */
  ScrollViewEffect();

public: // Not intended for application developers

  /**
   * This constructor is used by Dali New() methods.
   * @param [in] impl A pointer to a newly allocated Dali resource
   */
  ScrollViewEffect(Internal::ScrollViewEffect *impl);

};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_SCROLL_VIEW_EFFECT_H__
