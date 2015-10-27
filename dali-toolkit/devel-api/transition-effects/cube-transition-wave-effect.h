#ifndef __DALI_TOOLKIT_CUBE_TRANSITION_WAVE_EFFECT_H__
#define __DALI_TOOLKIT_CUBE_TRANSITION_WAVE_EFFECT_H__

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
#include <dali-toolkit/devel-api/transition-effects/cube-transition-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
  /**
   * CubeTransitionWaveEffect implementation class
   */
  class CubeTransitionWaveEffect;

} // namespace Internal

/**
 * SubClass of CubeTransitionEffect
 * Rotate the cubes successively according to the finger movement to achieve wave-like transition effect
 */
class DALI_IMPORT_API CubeTransitionWaveEffect : public CubeTransitionEffect
{

public:

  /**
   * Create an initialized CubeTransitionWaveEffect
   * @param[in] numRows How many rows of cubes
   * @param[in] numColumns How many columns of cubes
   * @return The initialized CubeTransitionWaveEffect object
   */
  static CubeTransitionWaveEffect New( unsigned int numRows, unsigned int numColumns );


public: // Not intended for developer use

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL CubeTransitionWaveEffect( Internal::CubeTransitionWaveEffect& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  DALI_INTERNAL CubeTransitionWaveEffect( Dali::Internal::CustomActor* internal );

}; // class CubeTransitionWaveEffect

} // namespace Toolkit

} // namespace Dali
#endif /* __DALI_TOOLKIT_CUBE_TRANSITION_WAVE_EFFECT_H__ */
