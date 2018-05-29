#ifndef __DALI_TOOLKIT_CUBE_TRANSITION_CROSS_EFFECT_H__
#define __DALI_TOOLKIT_CUBE_TRANSITION_CROSS_EFFECT_H__

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
// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/transition-effects/cube-transition-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
  /**
   * CubeTransitionCrossEffect implementation class
   */
  class CubeTransitionCrossEffect;

} // namespace Internal

/**
 * SubClass of CubeTransitionEffect
 * Rotate the neighboring cubes in perpendicular directions to transition from one image to another
 */
class DALI_TOOLKIT_API CubeTransitionCrossEffect : public CubeTransitionEffect
{

public:

  /**
   * Create an initialized CubeTransitionCrossEffect
   * @param[in] numRows How many rows of cubes
   * @param[in] numColumns How many columns of cubes
   * @return The initialized CubeTransitionCrossEffect object
   */
  static CubeTransitionCrossEffect New( unsigned int numRows, unsigned int numColumns );


public: // Not intended for developer use

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL CubeTransitionCrossEffect( Internal::CubeTransitionCrossEffect& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  DALI_INTERNAL CubeTransitionCrossEffect( Dali::Internal::CustomActor* internal );

}; // class CubeTransitionCrossEffect

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_CUBE_TRANSITION_CROSS_EFFECT_H__ */
