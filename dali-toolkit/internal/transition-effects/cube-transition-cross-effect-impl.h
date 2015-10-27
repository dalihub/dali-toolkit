#ifndef __DALI_TOOLKIT_INTERNAL_CUBE_TRANSITION_CROSS_EFFECT_H__
#define __DALI_TOOLKIT_INTERNAL_CUBE_TRANSITION_CROSS_EFFECT_H__

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
#include <dali-toolkit/devel-api/transition-effects/cube-transition-cross-effect.h>
#include <dali-toolkit/internal/transition-effects/cube-transition-effect-impl.h>

namespace Dali
{

namespace Toolkit
{

class CubeTransitionCrossEffect;

namespace Internal
{

class CubeTransitionEffect;

class CubeTransitionCrossEffect : public CubeTransitionEffect
{

public:

  /**
   * @copydoc Toolkit::CubeTransitionCrossEffect::New
   */
  static Toolkit::CubeTransitionCrossEffect New( unsigned int numRows, unsigned int numColumns );

protected:

   /**
    * @copydoc Toolkit::Internal::CubeTransitionEffect::OnInitialize
    */
   virtual void OnInitialize();

   /**
    * @copydoc Toolkit::Internal::CubeTransitionEffect::OnStartTransition
    */
   virtual void OnStartTransition( Vector2 panPosition, Vector2 panDisplacement );

private:

   /**
    * @brief Construct a new CubeTransitionCrossEffect object
    *
    * @param[in] numRows How many rows of cubes
    * @param[in] numColumns How many columns of cubes
    */
   CubeTransitionCrossEffect( unsigned int numRows, unsigned int numColumns );

   /**
    * @brief Set up animation to an Actor
    * This will also displace the cubes in the z direction by mCubeDisplacement and spread them apart on the xy plane
    * given by the closeness of displacementCentre to the XY plane at 0.
    *
    * @param[in] actorIndex The index of the cube in the cube array
    * @param[in] angle The angle of the rotation animation
    * @param[in] the centre to "explode" the tiles outwards from
    */
   void SetupAnimation( unsigned int actorIndex, unsigned int x, unsigned int y, float angle, const Vector3 axis, const Vector3& displacementCentre );

private:

   /**
    * The factor that determines how spread apart from each other the cubes will go
    * when they are displaced during the transition animation.
    * The larger the value the more the spread apart the cubes will be.
    * it should be in the range (0.0, +infinity)
    */
   float  mDisplacementSpreadFactor;

}; //class CubeTransitionCrossEffect

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::CubeTransitionCrossEffect& GetImpl( Dali::Toolkit::CubeTransitionCrossEffect& obj )
{
  DALI_ASSERT_ALWAYS( obj );

  Dali::RefObject& handle = obj.GetImplementation();

  return static_cast< Internal::CubeTransitionCrossEffect& >( handle );
}

inline const Internal::CubeTransitionCrossEffect& GetImpl( const Dali::Toolkit::CubeTransitionCrossEffect& obj )
{
  DALI_ASSERT_ALWAYS( obj );

  const Dali::RefObject& handle = obj.GetImplementation();

  return static_cast< const Internal::CubeTransitionCrossEffect& >( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_CUBE_TRANSITION_CROSS_EFFECT_H_ */
