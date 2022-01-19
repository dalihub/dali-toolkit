#ifndef DALI_TOOLKIT_TEXT_ENUMERATIONS_H
#define DALI_TOOLKIT_TEXT_ENUMERATIONS_H

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

namespace Text
{
/**
 * @brief The available Horizontal alignments for text.
 *
 * @SINCE_1_2.60
 */
namespace HorizontalAlignment
{
/**
 * @brief Enumerations for Horizontal alignment.
 *
 * @SINCE_1_2.60
 */
enum Type
{
  BEGIN,  ///< @SINCE_1_2.60
  CENTER, ///< @SINCE_1_2.60
  END     ///< @SINCE_1_2.60
};

} // namespace HorizontalAlignment

/**
 * @brief The available Vertical alignments for text.
 *
 * @SINCE_1_2.60
 */
namespace VerticalAlignment
{
/**
 * @brief Enumerations for Vertical alignment.
 *
 * @SINCE_1_2.60
 */
enum Type
{
  TOP,    ///< @SINCE_1_2.60
  CENTER, ///< @SINCE_1_2.60
  BOTTOM  ///< @SINCE_1_2.60
};

} // namespace VerticalAlignment

/**
 * @brief Contains modes which specify how lines are wrapped.
 *
 * If the layout width is too short to show the full text, then a wrapping mode can be specified.
 *
 * LineWrap::WORD mode will move an entire word to the next line:
 * @code
 * +---------+
 * |HELLO    |
 * |WORLD    |
 * +---------+
 * @endcode
 *
 * LineWrap::CHARACTER mode will move character by character to the next line:
 * @code
 * +---------+
 * |HELLO WOR|
 * |LD       |
 * +---------+
 * @endcode
 *
 * @SINCE_1_2.60
 */
namespace LineWrap
{
/**
 * @brief Enumerations specifying how a line is wrapped.
 * @SINCE_1_2.60
 * @see LineWrap
 */
enum Mode
{
  WORD,     ///< @SINCE_1_2.60
  CHARACTER ///< @SINCE_1_2.60
};

} // namespace LineWrap

/**
 * @brief The available underline types for text.
 * @SINCE_1_2.60
 */
namespace Underline
{
/**
 * @brief Enumerations specifying the underline type.
 */
enum Type
{
  SOLID,
  DASHED,
  DOUBLE
};

} // namespace UnderlineType

} // namespace Text

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif //DALI_TOOLKIT_TEXT_ENUMERATIONS_H
