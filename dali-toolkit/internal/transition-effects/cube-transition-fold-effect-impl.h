#ifndef DALI_TOOLKIT_INTERNAL_CUBE_TRANSITION_FOLD_EFFECT_H
#define DALI_TOOLKIT_INTERNAL_CUBE_TRANSITION_FOLD_EFFECT_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/transition-effects/cube-transition-fold-effect.h>
#include <dali-toolkit/internal/transition-effects/cube-transition-effect-impl.h>

namespace Dali
{
namespace Toolkit
{
class CubeTransitionFoldEffect;

namespace Internal
{
class CubeTransitionEffect;

class CubeTransitionFoldEffect : public CubeTransitionEffect
{
public:
  /**
   * @copydoc Toolkit::CubeTransitionFoldEffect::New
   */
  static Toolkit::CubeTransitionFoldEffect New(unsigned int numRows, unsigned int numColumns);

protected:
  /**
   * @copydoc Toolkit::CubeTransitionEffect::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Toolkit::CubeTransitionEffect::OnStartTransition
   */
  void OnStartTransition(Vector2 panPosition, Vector2 panDisplacement) override;

private:
  /**
   * Construct a new CubeTransitionFoldEffect object
   * @param[in] numRows How many rows of cubes
   * @param[in] numColumns How many columns of cubes
   */
  CubeTransitionFoldEffect(unsigned int numRows, unsigned int numColumns);

  /**
   * Set up animation to an Actor
   * @param[in] actorIndex The index of the cube in the cube array
   * @param[in] angle The angle of the rotation animation
   */
  void SetupAnimation(unsigned int actorIndex, unsigned int x, float angle);

}; //class CubeTransitionFoldEffect

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::CubeTransitionFoldEffect& GetImpl(Dali::Toolkit::CubeTransitionFoldEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::RefObject& handle = obj.GetImplementation();

  return static_cast<Internal::CubeTransitionFoldEffect&>(handle);
}

inline const Internal::CubeTransitionFoldEffect& GetImpl(const Dali::Toolkit::CubeTransitionFoldEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::RefObject& handle = obj.GetImplementation();

  return static_cast<const Internal::CubeTransitionFoldEffect&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_CUBE_TRANSITION_FOLD_EFFECT_H
