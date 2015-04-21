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

// CLASS HEADER
#include <dali-toolkit/public-api/shader-effects/nine-patch-mask-effect.h>

// EXTERNAL INCLUDES
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/object/property-input.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/public-api/images/resource-image.h>

namespace Dali
{

namespace Toolkit
{

namespace NinePatchMaskEffect
{

namespace
{

void NinePatchMaskEffectSizeConstraint( Vector2& current, const PropertyInputContainer& inputs )
{
  const Vector3& actorSize = inputs[0]->GetVector3();
  current.x = actorSize.x;
  current.y = actorSize.y;
}

} // unnamed namespace

static void DoApply( ImageActor actor, const std::string& maskImage, const Vector2& maskSize, Vector4 maskBorder )
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

void Apply( ImageActor actor, const std::string& maskImage )
{
  const Uint16Pair maskSize = ResourceImage::GetImageSize( maskImage );

  const float leftRight = (maskSize.GetWidth()  - 1.0f) * 0.5f;
  const float topBottom = (maskSize.GetHeight() - 1.0f) * 0.5f;

  DoApply( actor, maskImage, Vector2( maskSize.GetWidth(), maskSize.GetHeight() ), Vector4( leftRight, topBottom, leftRight, topBottom ) );
}

void Apply( ImageActor actor, const std::string& maskImage, const Vector4& maskBorder )
{
  const Uint16Pair maskSize = ResourceImage::GetImageSize( maskImage );

  DoApply( actor, maskImage, Vector2( maskSize.GetWidth(), maskSize.GetHeight() ), maskBorder );
}

} // namespace NinePatchMaskEffect

} // namespace Toolkit

} // namespace Dali
