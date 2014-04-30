#ifndef __DALI_TOOLKIT_ALPHA_DISCARD_EFFECT_H__
#define __DALI_TOOLKIT_ALPHA_DISCARD_EFFECT_H__

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
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

/**
 * Alpha discard effect is used to discard fragments when the alpha colour value is below a threshold.
 * This is useful for stenciling.
 *
 * Usage example:
 *
 *   ImageActor actor = ImageActor::New( Image( EXAMPLE_IMAGE_PATH ) );
 *   AlphaDiscardEffect alphaDiscardEffect = AlphaDiscardEffect::New();
 *   actor.SetShaderEffect( alphaDiscardEffect );
 */
class DALI_IMPORT_API AlphaDiscardEffect : public ShaderEffect
{
public:

  /**
   * Create an empty AlphaDiscardEffect handle.
   */
  AlphaDiscardEffect();

  /**
   * Virtual destructor.
   */
  virtual ~AlphaDiscardEffect();

  /**
   * Create a AlphaDiscardEffect.
   * @return A handle to a newly allocated AlphaDiscardEffect.
   */
  static AlphaDiscardEffect New();

private: // Not intended for application developers

  DALI_INTERNAL AlphaDiscardEffect( ShaderEffect handle );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_ALPHA_DISCARD_EFFECT_H__
