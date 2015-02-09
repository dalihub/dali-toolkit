#ifndef __DALI_TOOLKIT_SOLID_COLOR_ACTOR_H__
#define __DALI_TOOLKIT_SOLID_COLOR_ACTOR_H__

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
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/actors/image-actor.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Creates a Dali::ImageActor with a solid color, optionally it creates a border.
 *
 * If the \e border parameter is set to \e true, the Dali::ImageActor's style is set to Dali::ImageActor::STYLE_NINE_PATCH.
 *
 * @param[in] color The ImageActor's color.
 * @param[in] border If \e true, a border is created. By default, the value is set to \e false.
 * @param[in] borderColor The color for the ImageActor's border. By default, the value is set to Color::WHITE.
 * @param[in] borderSize The size for the ImageActor's border. By default, the value is set to 1 pixel. It supports under 10 pixel for clear result of gl blend
 * @return a handle to the new ImageActor
 */
DALI_IMPORT_API ImageActor CreateSolidColorActor( const Vector4& color, bool border = false, const Vector4& borderColor = Color::WHITE, const unsigned int borderSize = 1 );

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SOLID_COLOR_ACTOR_H__
