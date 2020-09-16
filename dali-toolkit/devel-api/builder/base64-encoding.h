#ifndef DALI_TOOLKIT_BASE64_ENCODING_H
#define DALI_TOOLKIT_BASE64_ENCODING_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
bool DecodeBase64PropertyData(const Property::Value& value, std::vector<uint32_t>& outputData);

/**
 * @brief Convert a block of uint32_t data into a Property::STRING or ARRAY of STRINGs
 * encoded using base64. This allows the data to be mapped to JSON easily.
 *
 * @param[out] value The value to write data into (to avoid copying).
 * @param[in] inputData The input
 */
void EncodeBase64PropertyData(Property::Value& value, const std::vector<uint32_t>& inputData);

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_BASE64_ENCODING_H
