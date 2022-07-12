#ifndef DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_ATTRIBUTE_HELPER_FUNCTIONS_H
#define DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_ATTRIBUTE_HELPER_FUNCTIONS_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/text-definitions.h>

// EXTERNAL INCLUDES
#include <functional>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
struct Attribute;

/**
 * @brief Copy the value from attribute buffer to value.
 *
 * @param[in] attribute the value of attribute.
 * @param[in] maxLengthAttributeValue the maximum length of any of the possible value for attribute
 * @param[out] value the value container.
 *
 */
void CopyAttributeValueFromBuffer(const Attribute& attribute, const Length maxLengthAttributeValue, char* value);

/**
 * @brief Process the float attribute value from buffer.
 *
 * @param[in] attribute the float attribute.
 *
 * @return The float value.
 */
float ProcessFloatAttribute(const Attribute& attribute);

/**
 * @brief Process the Enumeration attribute value from buffer.
 *
 * @param[in] attribute the Enumeration attribute.
 * @param[in] maxLengthAttributeValue the maximum length of any of the possible value for attribute
 * @param[in] funcStringToEnum the function converts string value to enum value
 * @param[out] enumValue the enum value
 *
 * @return      True if the enum value was processed successfully
 *
 */
template<typename T>
bool ProcessEnumerationAttribute(const Attribute&                    attribute,
                                 const Length                        maxLengthAttributeValue,
                                 std::function<T(const char* const)> funcStringToEnum,
                                 T&                                  enumValue)
{
  char* value = new char[maxLengthAttributeValue + 1u];

  CopyAttributeValueFromBuffer(attribute, maxLengthAttributeValue, value);

  enumValue = funcStringToEnum(value); // @TODO: the functions that process Enum value should be refactored to return bool from Scripting::GetEnumeration

  delete[] value;

  return true;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_ATTRIBUTE_HELPER_FUNCTIONS_H
