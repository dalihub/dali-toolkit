#ifndef DALI_HIDDEN_INPUT_PROPERTIES_H
#define DALI_HIDDEN_INPUT_PROPERTIES_H

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

namespace Dali
{
namespace Toolkit
{
/**
 * @addtogroup dali_toolkit_controls_text_controls
 * @{
 */

namespace HiddenInput
{
/**
 * @brief HiddenInput Property.
 * @SINCE_1_2.60
 */
namespace Property
{
/**
 * @brief HiddenInput Property.
 * @SINCE_1_2.60
 */
enum
{
  /**
   * @brief The mode for input text display.
   * @details Name "mode", type HiddenInput::Mode (Property::INTEGER).
   * @SINCE_1_2.60
   * @note Optional.
   * @see HiddenInput::Mode
   */
  MODE,

  /**
   * @brief All input characters are substituted by this character.
   * @details Name "substituteCharacter", type Property::INTEGER.
   * @SINCE_1_2.60
   * @note Optional.
   */
  SUBSTITUTE_CHARACTER,

  /**
   * @brief Length of text to show or hide, available when HIDE_COUNT/SHOW_COUNT mode is used.
   * @details Name "substituteCount", type Property::INTEGER.
   * @SINCE_1_2.60
   * @note Optional.
   */
  SUBSTITUTE_COUNT,

  /**
   * @brief Hide last character after this duration, available when SHOW_LAST_CHARACTER mode.
   * @details Name "showDuration", type Property::INTEGER.
   * @SINCE_1_2.60
   * @note Optional.
   */
  SHOW_LAST_CHARACTER_DURATION
};

} // namespace Property

/**
 * @brief The type for HiddenInput::Property::MODE.
 * @SINCE_1_2.60
 */
namespace Mode
{
/**
 * @brief The type for HiddenInput::Property::MODE.
 * @SINCE_1_2.60
 */
enum Type
{
  HIDE_NONE,          ///< Do not hide text. @SINCE_1_2.60
  HIDE_ALL,           ///< Hide all the input text. @SINCE_1_2.60
  HIDE_COUNT,         ///< Hide n characters from start. @SINCE_1_2.60
  SHOW_COUNT,         ///< Show n characters from start. @SINCE_1_2.60
  SHOW_LAST_CHARACTER ///< Show last character for the duration (use Property::SHOW_LAST_CHARACTER_DURATION to modify duration). @SINCE_1_2.60
};

} // namespace Mode

} // namespace HiddenInput

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_HIDDEN_INPUT_PROPERTIES_H
