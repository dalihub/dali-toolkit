#ifndef __DALI_TOOLKIT_CUBE_TRANSITION_CROSS_EFFECT_H__
#define __DALI_TOOLKIT_CUBE_TRANSITION_CROSS_EFFECT_H__

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
// INTERNAL INCLUDES
#include <dali-toolkit/public-api/transition-effects/cube-transition-effect.h>

namespace Dali DALI_IMPORT_API
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
 * Rotate the neighboring cubes in perpendicular directions to transite from one image to another
 */
class CubeTransitionCrossEffect : public CubeTransitionEffect
{

public:

  /**
   * Create an initialized CubeTransitionCrossEffect
   * @param[in] numRows How many rows of cubes
   * @param[in] numColumns How many columns of cubes
   * @param[in] viewAreaSize The size of view area for this transition effect
   * @return The initialized CubeTransitionCrossEffect object
   */
  static CubeTransitionCrossEffect New( unsigned int numRows, unsigned int numColumns, Size viewAreaSize );


public: // Not intended for developer use

  /**
   * This constructor is used by Dali New() methods.
   * @param [in] impl A pointer to a newly allocated Dali resource
   */
  CubeTransitionCrossEffect( Internal::CubeTransitionCrossEffect* impl );

}; // class CubeTransitionCrossEffect

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_CUBE_TRANSITION_CROSS_EFFECT_H__ */
