#ifndef __DALI_TOOLKIT_MASK_EFFECT_H__
#define __DALI_TOOLKIT_MASK_EFFECT_H__

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

/**
 * @addtogroup CAPI_DALI_TOOLKIT_SHADER_EFFECTS_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

/**
 * @brief MaskEffect is used to control which parts of an image are visible, using the alpha channel of a separate mask image.
 *
 * Typically mask images should be the same size as the main image being viewed, but this isn't essential.
 *
 * Usage example:
 *
 *   ImageActor actor = ImageActor::New( Image( EXAMPLE_IMAGE_PATH ) );
 *   MaskEffect maskEffect = MaskEffect::New( Image::New( MASK_IMAGE_PATH ) );
 *   actor.SetShaderEffect( maskEffect );
 */
class DALI_IMPORT_API MaskEffect : public ShaderEffect
{
public:

  /**
   * @brief Create an empty MaskEffect handle.
   */
  MaskEffect();

  /**
   * @brief Virtual destructor.
   */
  virtual ~MaskEffect();

  /**
   * @brief Create a MaskEffect.
   *
   * @param[in] maskImage The image to use as a mask
   * @return A handle to a newly allocated MaskEffect.
   */
  static MaskEffect New( Image maskImage );

private: // Not intended for application developers

  DALI_INTERNAL MaskEffect( ShaderEffect handle );
};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_MASK_EFFECT_H__
