#ifndef DALI_INPUT_FILTER_PROPERTIES_H
#define DALI_INPUT_FILTER_PROPERTIES_H

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

namespace Dali
{
namespace Toolkit
{
/**
 * @addtogroup dali_toolkit_controls_text_controls
 * @{
 */

namespace InputFilter
{
/**
 * @brief InputFilter Property.
 * @SINCE_2_0.33
 */
namespace Property
{
/**
 * @brief Enumeration for the type of InputFilter.
 *
 * An enum that determines the input filter type of the InputFilter map.
 * Users can set the ACCEPTED or REJECTED character set, or both.
 * If both are used, REJECTED has higher priority.
 * The character set must follow the regular expression rules.
 * Behaviour can not be guaranteed for incorrect grammars.
 *
 * Useful Meta characters:
 *
 * | %Meta characters  | Description
 * |-------------------|------------------------------------------------------------------------------------------------------------|
 * | \\w               | Matches an alphanumeric character, including "_"; same as [A-Za-z0-9_].                                    |
 * | \\W               | Matches a non-alphanumeric character, excluding "_"; same as [^A-Za-z0-9_].                                |
 * | \\s               | Matches a whitespace character, which in ASCII are tab, line feed, form feed, carriage return, and space.  |
 * | \\S               | Matches anything but a whitespace.                                                                         |
 * | \\d               | Matches a digit; same as [0-9].                                                                            |
 * | \\D               | Matches a non-digit; same as [^0-9].                                                                       |
 *
 * Example Usage:
 * @code
 *   Property::Map filter;
 *   filter[InputFilter::Property::ACCEPTED] = "[\\d]"; // accept whole digits
 *   filter[InputFilter::Property::REJECTED] = "[0-5]"; // reject 0, 1, 2, 3, 4, 5
 *
 *   field.SetProperty(DevelTextField::Property::INPUT_FILTER, filter); // acceptable inputs are 6, 7, 8, 9
 * @endcode
 * @SINCE_2_0.33
 */
enum Type
{
  /**
   * @brief The set of characters to be accepted.
   * @details Name "accepted", type Property::STRING.
   * @SINCE_2_0.33
   * @note Available on regex string.
   */
  ACCEPTED,

  /**
   * @brief The set of characters to be rejected.
   * @details Name "rejected", type Property::STRING.
   * @SINCE_2_0.33
   * @note Available on regex string.
   */
  REJECTED
};

} // namespace Property

} // namespace InputFilter

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_INPUT_FILTER_PROPERTIES_H
