#ifndef __DALI_TOOLKIT_NINE_PATCH_MASK_EFFECT_H__
#define __DALI_TOOLKIT_NINE_PATCH_MASK_EFFECT_H__

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
#include <dali/public-api/actors/image-actor.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief NinePatchMaskEffect is used to control which parts of an image are visible, using the alpha channel of a separate mask image.
 *
 * The mask image is expected to be smaller than the main image being viewed.
 * Conceptually the mask image is divided into a 3x3 grid (9 patches). The middle patch is stretched whilst the outer border is not.
 *
 * Usage example:
 *
 * @code
 *   ImageActor actor = ImageActor::New( Image( EXAMPLE_IMAGE_PATH ) );
 *   NinePatchMaskEffect::Apply( actor, MASK_IMAGE_PATH );
 * @endcode
 *
 * NinePatchMaskEffect is mutually exclusive with ImageActor::STYLE_NINE_PATCH i.e. the actor's main image should not be a nine-patch.
 */
namespace NinePatchMaskEffect
{

static void NinePatchMaskEffectSizeConstraint( Vector2& current, const PropertyInputContainer& inputs )
{
  const Vector3& actorSize = inputs[0]->GetVector3();
  current.x = actorSize.x;
  current.y = actorSize.y;
}

inline void DoApply( ImageActor actor, const std::string& maskImage, const Vector2& maskSize, Vector4 maskBorder )
{
  const char* ALPHA_MASK_VERTEX_SHADER_SOURCE =
  "precision mediump float;\n"
  "uniform vec2 uImageSize;                                                       \n"
  "uniform vec2 uMaskSize;                                                        \n"
  "varying vec2 vMaskTexCoord;                                                    \n"
  "                                                                               \n"
  "void main()                                                                    \n"
  "{                                                                              \n"
  "  gl_Position = uMvpMatrix * vec4(aPosition, 1.0);                             \n"
  "                                                                               \n"
  "  // Ignore mask UVs for image                                                 \n"
  "                                                                               \n"
  "  highp vec2 halfImageSize = uImageSize * 0.5;                                 \n"
  "  vTexCoord = (aPosition.xy + halfImageSize) / uImageSize;                     \n"
  "                                                                               \n"
  "  // UVs were calculated for image size, so convert for mask size              \n"
  "                                                                               \n"
  "  highp vec2 halfMaskSize  = uMaskSize * 0.5;                                  \n"
  "  highp vec2 halfSizeDelta = halfImageSize - halfMaskSize;                     \n"
  "                                                                               \n"
  "  highp vec2 maskPosition = aPosition.xy;                                      \n"
  "  maskPosition.x -= halfSizeDelta.x * sign(aPosition.x);                       \n"
  "  maskPosition.y -= halfSizeDelta.y * sign(aPosition.y);                       \n"
  "                                                                               \n"
  "  vMaskTexCoord = (maskPosition + halfMaskSize) / uMaskSize;                   \n"
  "}                                                                              \n";

  const char* ALPHA_MASK_FRAGMENT_SHADER_SOURCE =
  "varying mediump vec2 vMaskTexCoord;                                            \n"
  "                                                                               \n"
  "void main()                                                                    \n"
  "{                                                                              \n"
  "  highp vec4 mask = texture2D(sEffect, vMaskTexCoord);                         \n"
  "  gl_FragColor = texture2D(sTexture, vTexCoord) * uColor * vec4(1,1,1,mask.a); \n"
  "}                                                                              \n";

  ShaderEffect maskEffect = ShaderEffect::New(
    ALPHA_MASK_VERTEX_SHADER_SOURCE,
    ALPHA_MASK_FRAGMENT_SHADER_SOURCE,
    GeometryType( GEOMETRY_TYPE_IMAGE ),
    ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING ) );

  maskEffect.SetEffectImage( ResourceImage::New( maskImage ) );

  maskEffect.SetUniform( "uImageSize", Vector2(0,0) /*Constrained to actor size*/ );

  Constraint constraint = Constraint::New<Vector2>( maskEffect, maskEffect.GetPropertyIndex("uImageSize"), NinePatchMaskEffectSizeConstraint );
  constraint.AddSource( Source(actor, Actor::Property::SIZE) );
  constraint.Apply();

  maskEffect.SetUniform( "uMaskSize", maskSize );

  // Actor must provide nine-patch style geometry for this effect to work
  actor.SetStyle( ImageActor::STYLE_NINE_PATCH );
  actor.SetNinePatchBorder( maskBorder );

  actor.SetShaderEffect( maskEffect );
}

/**
 * @brief Apply the mask effect to an ImageActor.
 *
 * NinePatchMaskEffect is mutually exclusive with ImageActor::STYLE_NINE_PATCH i.e. the actor's main image should not be a nine-patch.
 * @param [in] actor The actor which needs the effect. To remove the effect call actor.RemoveShaderEffect().
 * @param [in] maskImage The path to a file containing the mask. The center pixels of the mask will be stretched.
 */
inline void Apply( ImageActor actor, const std::string& maskImage )
{
  const Uint16Pair maskSize = ResourceImage::GetImageSize( maskImage );

  const float leftRight = (maskSize.GetWidth()  - 1.0f) * 0.5f;
  const float topBottom = (maskSize.GetHeight() - 1.0f) * 0.5f;

  DoApply( actor, maskImage, Vector2( maskSize.GetWidth(), maskSize.GetHeight() ), Vector4( leftRight, topBottom, leftRight, topBottom ) );
}

/**
 * @brief Apply the mask effect to an ImageActor.
 *
 * NinePatchMaskEffect is mutually exclusive with ImageActor::STYLE_NINE_PATCH i.e. the actor's main image should not be a nine-patch.
 * @param [in] actor The actor which needs the effect. To remove the effect call actor.RemoveShaderEffect().
 * @param [in] maskImage The path to a file containing the mask.
 * @param [in] maskBorder Specifies the part of the mask image that will be stretched (left, top, right, bottom).
 */
inline void Apply( ImageActor actor, const std::string& maskImage, const Vector4& maskBorder )
{
  const Uint16Pair maskSize = ResourceImage::GetImageSize( maskImage );

  DoApply( actor, maskImage, Vector2( maskSize.GetWidth(), maskSize.GetHeight() ), maskBorder );
}

} // namespace NinePatchMaskEffect

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_NINE_PATCH_MASK_EFFECT_H__
