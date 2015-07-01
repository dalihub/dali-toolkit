#ifndef __DALI_TOOLKIT_ALPHA_DISCARD_EFFECT_H__
#define __DALI_TOOLKIT_ALPHA_DISCARD_EFFECT_H__

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
#include <dali/public-api/shader-effects/shader-effect.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Creates a new Alpha discard effect
 *
 * Alpha discard effect is used to discard fragments when the alpha colour value is below a threshold.
 * This is useful for stenciling.
 *
 * Usage example:
 *
 *   ImageActor actor = ImageActor::New( Image( EXAMPLE_IMAGE_PATH ) );
 *   ShaderEffect alphaDiscardEffect = CreateAlphaDiscardEffect();
 *   actor.SetShaderEffect( alphaDiscardEffect );
 *
 * @return A handle to a newly allocated ShaderEffect.
 */
inline ShaderEffect CreateAlphaDiscardEffect()
{
  const char* ALPHA_DISCARD_FRAGMENT_SHADER_SOURCE =
      "void main()                                                    \n"
      "{                                                              \n"
      "  mediump vec4 color = texture2D( sTexture, vTexCoord );       \n"
      "  if(color.a <= 0.0001)                                        \n"
      "  {                                                            \n"
      "    discard;                                                   \n"
      "  }                                                            \n"
      "  gl_FragColor = color * uColor;                               \n"
      "}                                                              \n";

  return ShaderEffect::New( "", // Use default
                            ALPHA_DISCARD_FRAGMENT_SHADER_SOURCE );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_ALPHA_DISCARD_EFFECT_H__
