#ifndef DALI_TOOLKIT_PLACEHOLDER_PROPERTIES_H
#define DALI_TOOLKIT_PLACEHOLDER_PROPERTIES_H

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
 * @brief Placeholder text used by Text controls to show text before any text inputed.
 * @SINCE_1_2.62
 *
 */
namespace PlaceHolder
{
/**
 * @brief Placeholder text Properties used by Text controls to show placeholder
 * @SINCE_1_2.62
 *
 */
namespace Property
{
/**
 * @brief The configurable settings for the Placeholder text.
 * @SINCE_1_2.62
 *
 */
enum Setting
{
  /**
   * @brief The text to display as a placeholder.
   * @details Name "text", type Property::STRING.
   * @note Optional. If not provided then no placeholder text will be shown whilst control not focused.
   * @SINCE_1_2.62
   */
  TEXT,

  /**
   * @brief The text to display as placeholder when focused.
   * @details Name "textFocused", type Property::STRING.
   * @note Optional. If not provided then no placeholder text will be shown when focused.
   * @SINCE_1_2.62
   */
  TEXT_FOCUSED,

  /**
   * @brief The colour of the placeholder text.
   * @details Name "color", type Property::VECTOR4.
   * @note If color not provided then 80% white will be used.
   * @SINCE_1_2.62
   */
  COLOR,

  /**
   * @brief The font family to be used for placeholder text.
   * @details Name "fontFamily", type Property::STRING.
   * @note Optional. Default font family used if not provided.
   * @SINCE_1_2.62
   */
  FONT_FAMILY,

  /**
   * @brief The font style to be used for placeholder text.
   * @details Name "fontStyle", type Property::MAP.
   *
   * Example usage:
   * @code
   *   Property::Map fontStylePropertyMap;
   *   fontStylePropertyMap.Insert( "weight", "bold" );
   *   fontStylePropertyMap.Insert( "width", "condensed" );
   *   fontStylePropertyMap.Insert( "slant", "italic" );
   *   ...
   *   placeholderPropertyMap[ Text::PlaceHolder::Property::FONT_STYLE] = fontStylePropertyMap;
   * @endcode
   *
   * @note Optional. Default font style used if not provided.
   * @SINCE_1_2.62
   */
  FONT_STYLE,

  /**
   * @brief The font point size to be used.
   * @details Name "pointSize", type Property::FLOAT.
   * @note Optional. Not required if PIXEL_SIZE provided.  If neither provided then the text control point size is used.
   * @SINCE_1_2.62
   */
  POINT_SIZE,

  /**
   * @brief The font size in pixels to be used
   * @details Name "pixelSize", type Property::FLOAT.
   * @note Optional. Not required if POINT_SIZE provided. If neither provided then the text control point size is used.
   * @SINCE_1_2.62
   */
  PIXEL_SIZE,

  /**
   * @brief If ellipsis should be used when placeholder is too long.
   * @details Name "ellipsis", type Property::BOOLEAN
   * @note Optional. Default is false.
   * @SINCE_1_2.62
   */
  ELLIPSIS
};

} // namespace Property

} // namespace PlaceHolder

} // namespace Text

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif //DALI_TOOLKIT_PLACEHOLDER_PROPERTIES_H
