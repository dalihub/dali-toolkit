#ifndef __DALI_TOOLKIT_CUBE_TRANSITION_FOLD_EFFECT_H__
#define __DALI_TOOLKIT_CUBE_TRANSITION_FOLD_EFFECT_H__

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
   * CubeTransitionFoldEffectimplementation class
   */
  class CubeTransitionFoldEffect;

} // namespace Internal

/**
 * SubClass of CubeTransitionEffect
 * Rotate the neighboring cubes in opposite directions to transition from one image to another
 */
class DALI_IMPORT_API CubeTransitionFoldEffect : public CubeTransitionEffect
{

public:

  /**
   * Create an initialized CubeTransitionFoldEffect
   * @param[in] numRows How many rows of cubes
   * @param[in] numColumns How many columns of cubes
   * @return The initialized CubeTransitionFoldEffect object
   */
  static CubeTransitionFoldEffect New( unsigned int numRows, unsigned int numColumns );


public: // Not intended for developer use

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL CubeTransitionFoldEffect( Internal::CubeTransitionFoldEffect& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  DALI_INTERNAL CubeTransitionFoldEffect( Dali::Internal::CustomActor* internal );

}; // class CubeTransitionFoldEffect

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_CUBE_TRANSITION_FOLD_EFFECT_H__ */
