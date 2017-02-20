#ifndef DALI_TOOLKIT_INTERNAL_COLOR_CONVERSION_H
#define DALI_TOOLKIT_INTERNAL_COLOR_CONVERSION_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <string>
#include <dali/public-api/object/property.h>

namespace Dali
{

class Vector4;

namespace Toolkit
{
namespace Internal
{

/*
 * @brief Convert the string representation of a color into a Vector4.
 *
 * The supported string formats are:
 * 1) An HTML style 'color' hex string ("#FF0000" for bright red).
 * 2) An ID referring to the color palette of the current theme e.g. "B018"
 *
 * @param[in] colorString The color in string format.
 * @param[out] outColor The color if found.
 * @return True if the conversion was successful.
 */
bool ConvertStringToColor( const std::string& colorString, Vector4& outColor );

/*
 * @brief Convert a variety of different color representations into a Vector4.
 *
 * @param[in] colorValue The color in Vector4 or string format.
 * @param[out] outColor The color if found.
 * @return True if the conversion was successful.
 */
bool ConvertPropertyToColor( const Property::Value& colorValue, Vector4& outColor );

} // Internal
} // Toolkit
} // Dali


#endif // DALI_TOOLKIT_INTERNAL_COLOR_CONVERSION_H
