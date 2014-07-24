#ifndef __DALI_TOOLKIT_OVERLAY_EFFECT_H__
#define __DALI_TOOLKIT_OVERLAY_EFFECT_H__

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

// INTERNAL INCLUDES
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

/**
 * OverlayEffect is used to apply an overlay image to the actor.
 * Typically overlay images should be the same size as the main image being viewed, but this isn't essential.
 *
 * Usage example:
 *
 *   ImageActor actor = ImageActor::New( Image( EXAMPLE_IMAGE_PATH ) );
 *   OverlayEffect overlayEffect = OverlayEffect::New( Image::New( OVERLAY_IMAGE_PATH ) );
 *   actor.SetShaderEffect( overlayEffect );
 */
class DALI_IMPORT_API OverlayEffect : public ShaderEffect
{
public:

  /**
   * Create an empty OverlayEffect handle.
   */
  OverlayEffect();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~OverlayEffect();

  /**
   * Create a OverlayEffect.
   * @return A handle to a newly allocated OverlayEffect.
   */
  static OverlayEffect New( Image overlayImage );

private: // Not intended for application developers

  DALI_INTERNAL OverlayEffect( ShaderEffect handle );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_OVERLAY_EFFECT_H__
