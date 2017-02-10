#ifndef DALI_TOOLKIT_INTERNAL_PROPERTY_HELPER_H
#define DALI_TOOLKIT_INTERNAL_PROPERTY_HELPER_H

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
 *
 */

// EXTERNAL INCLUDES
#include <string>
#include <dali/public-api/object/property.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * @brief Parses a Property::Value to retrieve the string.
 *
 * If value is a Property::STRING, then it simply extracts the required string.
 * If value is a Property::ARRAY, then it combines all the strings it contains into one adding a newline character to each line.
 * The second option allows users to write long strings over several lines in a JSON file.
 *
 * @return True if a string was extracted successfully.
 */
bool GetStringFromProperty( const Property::Value& value, std::string& output );

} // namespace Internal

} // namespace Toolkit

} // namespace Dali


#endif // DALI_TOOLKIT_INTERNAL_PROPERTY_HELPER_H
