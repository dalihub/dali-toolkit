#ifndef DALI_TOOLKIT_BASE64_ENCODING_H
#define DALI_TOOLKIT_BASE64_ENCODING_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/property.h>
#include <string_view>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
namespace Toolkit
{
/**
 * @brief Parses a Property::STRING or Property::ARRAY of STRINGS to
 * retrieve a block of uint32_t data.
 *
 * Data can be encoded using the base64 encoding scheme to allow it to be used
 * in JSON (The property system maps to JSON types).
 *
 * @param[in] value The property value to decode
 * @param[out] outputData The output data block
 * @return True if a data block was decoded successfully.
 */
DALI_TOOLKIT_API bool DecodeBase64PropertyData(const Property::Value& value, std::vector<uint32_t>& outputData);

/**
 * @brief Parses a Property::STRING or Property::ARRAY of STRINGS to
 * retrieve an array of uint8_t data.
 *
 * Data can be encoded using the base64 encoding scheme to allow it to be used
 * in JSON (The property system maps to JSON types).
 *
 * @param[in] value The property value to decode
 * @param[out] outputData The output data block
 * @return True if a data block was decoded successfully.
 */
DALI_TOOLKIT_API bool DecodeBase64PropertyData(const Property::Value& value, std::vector<uint8_t>& outputData);

/**
 * @brief Parses a std::string_view to retrieve an array of uint8_t data.
 *
 * Data can be encoded using the base64 encoding scheme to allow it to be used
 * in JSON (The property system maps to JSON types).
 *
 * @param[in] encodedString The input string to decode
 * @param[out] outputData The output data block
 * @return True if a data block was decoded successfully.
 */
DALI_TOOLKIT_API bool DecodeBase64FromString(const std::string_view& encodedString, std::vector<uint8_t>& outputData);

/**
 * @brief Convert a block of uint32_t data into a Property::STRING or ARRAY of STRINGs
 * encoded using base64. This allows the data to be mapped to JSON easily.
 *
 * @param[out] value The value to write data into (to avoid copying).
 * @param[in] inputData The input
 */
DALI_TOOLKIT_API void EncodeBase64PropertyData(Property::Value& value, const std::vector<uint32_t>& inputData);

/**
 * @brief Convert a block of uint8_t data into a Property::STRING or ARRAY of STRINGs
 * encoded using base64. This allows the data to be mapped to JSON easily.
 *
 * @param[out] value The value to write data into (to avoid copying).
 * @param[in] inputData The input
 */
DALI_TOOLKIT_API void EncodeBase64PropertyData(Property::Value& value, const std::vector<uint8_t>& inputData);

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_BASE64_ENCODING_H
