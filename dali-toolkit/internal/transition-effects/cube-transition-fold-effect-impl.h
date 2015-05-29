#ifndef __DALI_TOOLKIT_INTERNAL_CUBE_TRANSITION_FOLD_EFFECT_H__
#define __DALI_TOOLKIT_INTERNAL_CUBE_TRANSITION_FOLD_EFFECT_H__

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
  static Toolkit::CubeTransitionFoldEffect New(unsigned int numRows, unsigned int numColumns, Size viewAreaSize);

protected:

   /**
    * @copydoc Toolkit::CubeTransitionEffect::OnInitialize
    */
   virtual void OnInitialize();

   /**
    * @copydoc Toolkit::CubeTransitionEffect::OnStartTransition
    */
   virtual void OnStartTransition( Vector2 panPosition, Vector2 panDisplacement );

   /**
    * @copydoc Toolkit::Internal::CubeTransitionEffect::OnStopTransition
    */
   virtual void OnStopTransition();
private:

   /**
    * Construct a new CubeTransitionFoldEffect object
    * @param[in] numRows How many rows of cubes
    * @param[in] numColumns How many columns of cubes
    * @param[in] viewAreaSize The size of view area for this transition effect
    */
   CubeTransitionFoldEffect( unsigned int numRows, unsigned int numColumns, Size viewAreaSize );

   /**
    * Set up animation to an Actor
    * @param[in] actorIndex The index of the cube in the cube array
    * @param[in] angle The angle of the rotation animation
    * @param[in] resetTranslation The translation used to reset the actor position before animation
    */

   void SetupAnimation( unsigned int actorIndex, float angle, Vector3 resetTranslation );

private:

   static const float  mDisplacementRatio;

}; //class CubeTransitionFoldEffect

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::CubeTransitionFoldEffect& GetImpl(Dali::Toolkit::CubeTransitionFoldEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::CubeTransitionFoldEffect&>(handle);
}

inline const Internal::CubeTransitionFoldEffect& GetImpl(const Dali::Toolkit::CubeTransitionFoldEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::CubeTransitionFoldEffect&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_CUBE_TRANSITION_FOLD_EFFECT_H_ */
