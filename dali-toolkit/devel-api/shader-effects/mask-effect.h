#ifndef __DALI_TOOLKIT_MASK_EFFECT_H__
#define __DALI_TOOLKIT_MASK_EFFECT_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/images/image.h>
#include <dali/public-api/shader-effects/shader-effect.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Creates a new MaskEffect
 *
 * MaskEffect is used to control which parts of an image are visible, using the alpha channel of a separate mask image.
 *
 * Typically mask images should be the same size as the main image being viewed, but this isn't essential.
 *
 * Usage example:
 *
 *   ImageActor actor = ImageActor::New( Image( EXAMPLE_IMAGE_PATH ) );
 *   ShaderEffect maskEffect = CreateMaskEffect( Image::New( MASK_IMAGE_PATH ) );
 *   actor.SetShaderEffect( maskEffect );
 *
 * @param[in] maskImage The image to use as a mask
 * @return A handle to a newly allocated ShaderEffect
 */
inline ShaderEffect CreateMaskEffect(Image maskImage)
{
  const char* ALPHA_MASK_FRAGMENT_SHADER_SOURCE =
      "void main()                                                                    \n"
      "{                                                                              \n"
      "  highp vec4 mask = texture2D(sEffect, vTexCoord);                             \n"
      "  gl_FragColor = texture2D(sTexture, vTexCoord) * uColor * vec4(1,1,1,mask.a); \n"
      "}                                                                              \n";

  ShaderEffect shaderEffect = ShaderEffect::New(
      "", // Use default
      ALPHA_MASK_FRAGMENT_SHADER_SOURCE,
      ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING ) );

  shaderEffect.SetEffectImage( maskImage );

  return shaderEffect;
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_MASK_EFFECT_H__
