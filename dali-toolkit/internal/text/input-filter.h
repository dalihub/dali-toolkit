#ifndef DALI_INPUT_FILTER_H
#define DALI_INPUT_FILTER_H

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
#include <dali-toolkit/public-api/controls/text-controls/input-filter-properties.h>
#include <regex>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * Class to handle the input text filtering
 */
class InputFilter : public ConnectionTracker
{
public:
  /**
   * @brief Constructor
   */
  InputFilter();

public: // Intended for internal use
  /**
   * @brief Used to set options of input filter.
   * @param[in] map The property map describing the option.
   */
  void SetProperties(const Property::Map& map);

  /**
   * @brief Retrieve property map of input filter options.
   * @param[out] map The input filter option.
   */
  void GetProperties(Property::Map& map);

  /**
   * @brief Check if the source is contained in regex.
   * @param[in] type ACCEPTED or REJECTED
   * @param[in] source The original text.
   * @return @e true if the source is contained in regex, otherwise returns @e false.
   */
  bool Contains(Toolkit::InputFilter::Property::Type type, std::string source);

  /**
   * @brief Filtering the text using regex.
   * @param[in] type ACCEPTED or REJECTED
   * @param[in,out] text The string value to filter and write back.
   * @return @e true if the text is filtered by the regex, otherwise returns @e false.
   */
  bool Filter(Toolkit::InputFilter::Property::Type type, std::string& text);

private:
  std::string mAccepted;
  std::string mRejected;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_INPUT_FILTER_H
