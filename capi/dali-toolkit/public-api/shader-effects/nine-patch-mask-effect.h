#ifndef __DALI_TOOLKIT_NINE_PATCH_MASK_EFFECT_H__
#define __DALI_TOOLKIT_NINE_PATCH_MASK_EFFECT_H__

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

/**
 * @brief Apply the mask effect to an ImageActor.
 *
 * NinePatchMaskEffect is mutually exclusive with ImageActor::STYLE_NINE_PATCH i.e. the actor's main image should not be a nine-patch.
 * @param [in] actor The actor which needs the effect. To remove the effect call actor.RemoveShaderEffect().
 * @param [in] maskImage The path to a file containing the mask. The center pixels of the mask will be stretched.
 */
void Apply( ImageActor actor, const std::string& maskImage );

/**
 * @brief Apply the mask effect to an ImageActor.
 *
 * NinePatchMaskEffect is mutually exclusive with ImageActor::STYLE_NINE_PATCH i.e. the actor's main image should not be a nine-patch.
 * @param [in] actor The actor which needs the effect. To remove the effect call actor.RemoveShaderEffect().
 * @param [in] maskImage The path to a file containing the mask.
 * @param [in] maskBorder Specifies the part of the mask image that will be stretched (left, top, right, bottom).
 */
void Apply( ImageActor actor, const std::string& maskImage, const Vector4& maskBorder );

} // namespace NinePatchMaskEffect

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_NINE_PATCH_MASK_EFFECT_H__
