#ifndef DALI_TOOLKIT_TEXT_ENUMERATIONS_DEVEL_H
#define DALI_TOOLKIT_TEXT_ENUMERATIONS_DEVEL_H

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
namespace DevelText
{
namespace TextDirection
{
enum Type
{
  LEFT_TO_RIGHT,
  RIGHT_TO_LEFT
};

} // namespace TextDirection

namespace VerticalLineAlignment
{
enum Type
{
  TOP,
  MIDDLE,
  BOTTOM
};

} // namespace VerticalLineAlignment
namespace LineWrap
{
enum Mode
{
  WORD,
  CHARACTER,
  HYPHENATION, // HYPHENATION mode will add hyphen and move part of the word to the next line.
  MIXED        // MIXEd mode will apply WORD mode, if failed try HYPHENATION mode, if failed try CHARACTER.
};

} // namespace LineWrap

/**
 * A enum for defining text layout directions.
 * It can also be inherited (from a parent) or deduced from the default language script of a locale.
 * And it can be the direction of the text ifself.
 */
enum class MatchLayoutDirection
{
  INHERIT, // The text layout direction is inherited. If you change the layout direction, it will be aligned with the changed layout direction.
  LOCALE,  // The text layout direction is determined by the locale of the system language.
  CONTENTS // The text layout direction is determined by the text itself.
};

/**
 * @brief Contains types which specify where the text is truncated and replaced by Ellipsis glyph.
 *
 * If the text cannot fit into layout size then truncate text and replace it by Ellipsis glyph.
 *
 * Ellipsis works after layouting text normally according to LineWrap mode.
 *
 * Ellipsis glyph is three dots "...".
 *
 * The Ellipsis property should be enabled.
 *
 *
 * Example: "Hello here is test goodbye."
 *
 * Assume LineWrap is CHARACTER.
 *
 * EllipsisPosition::END type in single line, truncate tail of line then add Ellipsis:
 * @code
 * +-----------+
 * |Hello he...|
 * +-----------+
 * @endcode
 *
 * EllipsisPosition::END type in multi-lines, truncate tail of the last appeared line then add Ellipsis:
 * @code
 * +-----------+
 * |Hello here |
 * |is test ...|
 * +-----------+
 * @endcode
 *
 * EllipsisPosition::START type in single line, truncate head of line then add Ellipsis:
 * @code
 * +-----------+
 * |...goodbye.|
 * +-----------+
 * @endcode
 *
 * EllipsisPosition::START type in multi-lines, truncate head of the first appeared line then add Ellipsis:
 * @code
 * +-----------+
 * |...test goo|
 * |dbye.      |
 * +-----------+
 * @endcode
 *
 * EllipsisPosition::MIDDLE type in single line, truncate middle of line then add Ellipsis:
 * @code
 * +-----------+
 * |Hell...bye.|
 * +-----------+
 * @endcode
 *
 * EllipsisPosition::MIDDLE type in multi-lines, truncate middle lines. In the end of the last appeared line before removed lines add Ellipsis:
 * @code
 * +-----------+
 * |Hello he...|
 * |dbye.      |
 * +-----------+
 * @endcode
 *
 */
namespace EllipsisPosition
{
/**
 * @brief Enumerations specifying where to position the ellipsis glyph.
 * @see EllipsisPosition
 */
enum Type
{
  END = 0, ///< END position will truncate tail of text that exceeds the layout size then replace it by Ellipsis glyph in the end of the last appeared line.
  START,   ///< START position will truncate head of text that exceeds the layout size then replace it by Ellipsis glyph in the start of the first appeared line. In multilines, the lines are removed from top until the text fit into layout height.
  MIDDLE   ///< MIDDLE position will truncate middle of text that exceeds the layout size then replace it by Ellipsis glyph in the middle of line. In multilines, the lines are removed from middle until the text fit into layout height then add ellipsis glyph in the end of the last line appeared before removed lines.
};

} // namespace EllipsisPosition

namespace Ellipsize
{
/**
 * @brief Enumerations specifying ellipsize mode.
 * @see EllipsisMode.
 */
enum Mode
{
  TRUNCATE = 0, ///< If the text exceeds the layout, it will be truncated with an ellipsis.
  AUTO_SCROLL   ///< If the text exceeds the layout, it will be auto scroll animated.
};

} // namespace EllipsisMode

} // namespace DevelText

} // namespace Toolkit

} // namespace Dali

#endif //DALI_TOOLKIT_TEXT_ENUMERATIONS_DEVEL_H
