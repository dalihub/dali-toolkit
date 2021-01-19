#ifndef DALI_TOOLKIT_IMAGE_REGION_EFFECT_H
#define DALI_TOOLKIT_IMAGE_REGION_EFFECT_H

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
 * @brief Creates a new ImageRegionEffect
 *
 * ImageRegionEffect is a custom shader effect to show only a region of an Image actor.
 *
 * Animatable/Constrainable uniforms:
 *  "uTopLeft"      - The top-left corner of the image region. The coordinates are in percentage,
 *                    (0,0) being the top-left and (1,1) the bottom right of the original image
 *  "uBottomRight"  - The bottom-right corner of the image region. The coordinates are in percentage,
 *                    (0,0) being the top-left and (1,1) the bottom right of the original image
 *
 * @return A property map of the required shader
 */
DALI_TOOLKIT_API Property::Map CreateImageRegionEffect();

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_IMAGE_REGION_EFFECT_H
