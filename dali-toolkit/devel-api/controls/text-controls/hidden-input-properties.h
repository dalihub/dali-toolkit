#ifndef __DALI_HIDDEN_TEXT_PROPERTIES_H__
#define __DALI_HIDDEN_TEXT_PROPERTIES_H__

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

// INTERNAL INCLUDES

namespace Dali
{

namespace Toolkit
{

namespace HiddenInput
{

/**
 * @brief HiddenInput Property
 * @SINCE_1_2.33
 */
namespace Property
{

/**
 * @brief HiddenInput Property
 * @SINCE_1_2.33
 */
enum
{
  /**
   * @brief The mode for input text display.
   * @details Name "mode", type HiddenInput::Mode or INTEGER.
   * @SINCE_1_2.33
   * @note Optional.
   * @see HiddenInput::Mode
   */
  MODE,

  /**
   * @brief All input characters are substituted by this character.
   * @details Name "substituteCharacter", type INTEGER.
   * @SINCE_1_2.33
   * @note Optional.
   */
  SUBSTITUTE_CHARACTER,

  /**
   * @brief Length of text to show or hide, available when HIDE_COUNT/SHOW_COUNT mode is used.
   * @details Name "substituteCount", type INTEGER.
   * @SINCE_1_2.33
   * @note Optional.
   */
  SUBSTITUTE_COUNT,

  /**
   * @brief Hide last character after this duration, available when SHOW_LAST_CHARACTER mode.
   * @details Name "showDuration", type INTEGER.
   * @SINCE_1_2.33
   * @note Optional.
   */
  SHOW_DURATION
};

} // namespace Property

/**
 * @brief The type for HiddenInput::Property::MODE.
 * @SINCE_1_2.33
 */
namespace Mode
{

/**
 * @brief The type for HiddenInput::Property::MODE.
 * @SINCE_1_2.33
 */
enum Type
{
  HIDE_NONE,  ///< Don't hide text
  HIDE_ALL,   ///< Hide all the input text
  HIDE_COUNT,  ///< Hide n characters from start
  SHOW_COUNT,  ///< Show n characters from start
  SHOW_LAST_CHARACTER  ///< Show last character for the duration(use SHOW_DURATION property to modify duration)
};

} // namespace Mode

} // namespace HiddenInput

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_HIDDEN_TEXT_PROPERTIES_H__
