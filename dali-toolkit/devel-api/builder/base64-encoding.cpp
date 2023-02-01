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
#include <iterator>
#include <sstream>

#include <dali-toolkit/devel-api/builder/base64-encoding.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/property-value.h>
#include <dali-toolkit/third-party/base-n/basen.hpp>

namespace Dali
{
namespace Toolkit
{
namespace
{
const int MAX_PROPERTY_STRING_LENGTH(64); // Cuts larger strings into blocks of this size.

bool GetStringFromProperty(const Property::Value& value, std::string& output)
{
  bool extracted = false;
  if(value.Get(output))
  {
    extracted = true;
  }
  else
  {
    const Property::Array* array = value.GetArray();
    if(array)
    {
      const unsigned int arraySize = array->Size();
      for(unsigned int i = 0; i < arraySize; ++i)
      {
        std::string element;
        if(array->GetElementAt(i).Get(element))
        {
          extracted = true;
          output += element;
        }
        else
        {
          // If property in array is anything other than a string, then it is invalid so break and clear output.
          output.clear();
          extracted = false;
          break;
        }
      }
    }
  }
  return extracted;
}

} //anonymous namespace

bool DecodeBase64PropertyData(const Property::Value& value, std::vector<uint32_t>& outputData)
{
  bool        decoded = false;
  std::string encodedString;

  if(GetStringFromProperty(value, encodedString))
  {
    std::vector<unsigned char> outputTmpData;
    outputTmpData.reserve(ceil(encodedString.size() * 0.75f));
    bn::decode_b64(encodedString.begin(), encodedString.end(), std::back_inserter(outputTmpData));

    outputData.clear();
    uint32_t outputSize = outputTmpData.size() / sizeof(uint32_t) + static_cast<uint32_t>(!!(outputTmpData.size() % sizeof(uint32_t)));
    outputData.resize(outputSize);
    // Treat as a block of data
    memcpy(&outputData[0], &outputTmpData[0], outputTmpData.size());

    decoded = true;
  }
  return decoded;
}

bool DecodeBase64PropertyData(const Property::Value& value, std::vector<uint8_t>& outputData)
{
  bool        decoded = false;
  std::string encodedString;

  if(GetStringFromProperty(value, encodedString))
  {
    outputData.reserve(ceil(encodedString.size() * 0.75f));
    bn::decode_b64(encodedString.begin(), encodedString.end(), std::back_inserter(outputData));

    decoded = true;
  }
  return decoded;
}

void EncodeBase64PropertyData(Property::Value& value, const std::vector<uint32_t>& inputData)
{
  std::ostringstream oss;

  bn::encode_b64(reinterpret_cast<const char*>(&inputData[0]),
                 reinterpret_cast<const char*>(&inputData[0] + inputData.size()),
                 std::ostream_iterator<unsigned char>(oss, ""));

  std::string encodedString = oss.str();
  if(encodedString.length() > MAX_PROPERTY_STRING_LENGTH)
  {
    // cut string up into blocks of MAX_PROPERTY_STRING_LENGTH and store to an array
    auto numStrings = encodedString.length() / MAX_PROPERTY_STRING_LENGTH +
                      ((encodedString.length() % MAX_PROPERTY_STRING_LENGTH) != 0);

    Property::Array array;
    for(auto i = 0u; i < numStrings; ++i)
    {
      array.PushBack(encodedString.substr(i * MAX_PROPERTY_STRING_LENGTH, MAX_PROPERTY_STRING_LENGTH));
    }
    value = array;
  }
  else
  {
    value = encodedString;
  }
}

} // namespace Toolkit

} // namespace Dali
