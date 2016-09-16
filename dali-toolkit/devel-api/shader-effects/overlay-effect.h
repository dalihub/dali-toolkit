#ifndef __DALI_TOOLKIT_OVERLAY_EFFECT_H__
#define __DALI_TOOLKIT_OVERLAY_EFFECT_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/shader-effects/shader-effect.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Creates a new OverlayEffect
 *
 * OverlayEffect is used to apply an overlay image to the actor.
 * Typically overlay images should be the same size as the main image being viewed, but this isn't essential.
 *
 * Usage example:
 *
 *   ImageActor actor = ImageActor::New( Image( EXAMPLE_IMAGE_PATH ) );
 *   ShaderEffect overlayEffect = CreateOverlayEffect( Image::New( OVERLAY_IMAGE_PATH ) );
 *   actor.SetShaderEffect( overlayEffect );
 *
 *   @param[in] overlayImage The image to overlay on top of the actor
 *   @return A handle to a newly allocated ShaderEffect
 */
inline ShaderEffect CreateOverlayEffect(Image overlayImage)
{
  // (Target > 0.5) * (1 - (1-2*(Target-0.5)) * (1-Blend)) + (Target <= 0.5) * ((2*Target) * Blend)
  const char* OVERLAY_FRAGMENT_SHADER_SOURCE =
      "void main()\n"
      "{\n"
      "  lowp vec4 target = texture2D(sTexture, vTexCoord);\n"
      "  lowp vec4 overlay = texture2D(sEffect, vTexCoord);\n"
      "  if ( length( target.rgb ) > 0.5 )\n"
      "  {\n"
      "    gl_FragColor = vec4( mix( target.rgb, 1.0 - ( 1.0 - 2.0 * ( target.rgb - 0.5 ) )  * ( 1.0 - overlay.rgb ), overlay.a ), min( 1.0, target.a + overlay.a ) );\n"
      "  }\n"
      "  else\n"
      "  {\n"
      "    gl_FragColor = vec4( mix( target.rgb, 2.0 * target.rgb * overlay.rgb, overlay.a ), target.a + overlay.a );\n"
      "  }\n"
      "}\n";

  ShaderEffect shaderEffect = ShaderEffect::New(
      "", // Use default
      OVERLAY_FRAGMENT_SHADER_SOURCE,
      ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING ) );

  shaderEffect.SetEffectImage( overlayImage );

  return shaderEffect;
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_OVERLAY_EFFECT_H__
