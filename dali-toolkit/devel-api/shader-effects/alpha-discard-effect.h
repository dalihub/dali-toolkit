#ifndef DALI_TOOLKIT_ALPHA_DISCARD_EFFECT_H
#define DALI_TOOLKIT_ALPHA_DISCARD_EFFECT_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

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
 *   ImageView actor = ImageView::New( EXAMPLE_IMAGE_PATH );
 *   Property::Map alphaDiscardEffect = CreateAlphaDiscardEffect();
 *   actor.SetProperty( ImageView::Property::IMAGE, alphaDiscardEffect );
 *
 * @return A property map of the required shaders.
 */
DALI_TOOLKIT_API Property::Map CreateAlphaDiscardEffect();

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_ALPHA_DISCARD_EFFECT_H
