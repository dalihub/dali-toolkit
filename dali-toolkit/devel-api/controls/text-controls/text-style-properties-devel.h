#ifndef DALI_TOOLKIT_TEXT_STYLE_PROPERTIES_DEVEL_H
#define DALI_TOOLKIT_TEXT_STYLE_PROPERTIES_DEVEL_H

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
 * @addtogroup dali_toolkit_controls
 * @{
 */

namespace DevelText
{
namespace Shadow
{
/**
 * @brief Used by Text controls to show different styles of text.
 *
 */
namespace Property
{
enum
{
  /**
   * @brief The color of the shadow.
   * @details Name "color", type Property::STRING or Property::VECTOR4.
   * @note Optional. If not provided the default color (BLACK) is used.
   */
  COLOR,

  /**
   * @brief The offset in pixels of the shadow.
   * @details Name "offset", type Property::STRING or Property::VECTOR2. i.e "3.0 3.0" or Vector2( 3.f, 3.f )
   * @note Optional. If not provided then the shadow is not enabled.
   */
  OFFSET,

  /**
   * @brief The radius of the Gaussian blur for the soft shadow.
   * @details Name "blurRadius", type Property::STRING or Property::FLOAT. i.e "5.0" or 5.f
   * @note Optional. If not provided then the soft shadow is not enabled.
   */
  BLUR_RADIUS
};

} // namespace Property

} // namespace Shadow

namespace Underline
{
namespace Property
{
enum
{
  /**
   * @brief Whether the underline is enabled.
   * @details Name "enable", type Property::STRING or Property::BOOLEAN. i.e. "true", "false", true or false
   * @note Optional. By default is disabled.
   */
  ENABLE,

  /**
   * @brief The color of the underline.
   * @details Name "color", type Property::STRING or Property::VECTOR4
   * @note Optional. If not provided then the color of the text is used.
   */
  COLOR,

  /**
   * @brief The height in pixels of the underline.
   * @details Name "height", type Property::STRING or Property::FLOAT. i.e. "1.0" or 1.f
   * @note Optional. If not provided then the default height is used (1 pixel).
   */
  HEIGHT,

  /**
   * @brief The type of the underline.
   * @details Name "type", type Property::STRING or type Text::Underline::Type. i.e "dashed" or Text::Underline::DASHED
   *          Values "SOLID", "DASHED", "DOUBLE" , default SOLID.
   * @note Optional. If not provided then the default type is used (solid underline).
   */
  TYPE,

  /**
   * @brief The width in pixels of the dashes of the dashed underline. Only valid when "DASHED" underline type is used.
   * @details Name "dashWidth", type Property::STRING or Property::FLOAT. e.g. "1.0" or 1.f
   * @note Optional. If not provided then the default width is used (2 pixel).
   */
  DASH_WIDTH,

  /**
   * @brief The gap in pixels between the dashes of the dashed underline. Only valid when "DASHED" underline type is used.
   * @details Name "dashGap", type Property::STRING or Property::FLOAT. e.g. "1.0" or 1.f
   * @note Optional. If not provided then the default gap is used (1 pixel).
   */
  DASH_GAP
};

} // namespace Property

} // namespace Underline

namespace Outline
{
namespace Property
{
enum
{
  /**
   * @brief The color of the outline.
   * @details Name "color", type Property::STRING or Property::VECTOR4
   * @note Optional. If not provided the default color (WHITE) is used.
   */
  COLOR,

  /**
   * @brief The width in pixels of the outline.
   * @details Name "width", type Property::STRING or Property::FLOAT i.e. "1.0" or 1.f
   * @note Optional. If not provided then the outline is not enabled.
   */
  WIDTH
};

} // namespace Property

} // namespace Outline

namespace Background
{
namespace Property
{
enum
{
  /**
   * @brief Whether to paint the text's background.
   * @details Name "enable", type Property::STRING or Property::BOOLEAN i.e. "true", "false", true or false
   * @note Optional. By default is disabled.
   */
  ENABLE,

  /**
   * @brief The color of the background.
   * @details Name "color", type Property::STRING or Property::VECTOR4
   * @note Optional. If not provided the default color (CYAN) is used.
   */
  COLOR
};

} // namespace Property

} // namespace Background

namespace Strikethrough
{
namespace Property
{
enum
{
  /**
   * @brief Whether the strikethrough is enabled.
   * @details Name "enable", type Property::STRING or Property::BOOLEAN. i.e. "true", "false", true or false
   * @note Optional. By default is disabled.
   */
  ENABLE,

  /**
   * @brief The color of the strikethrough.
   * @details Name "color", type Property::STRING or Property::VECTOR4
   * @note Optional. If not provided then the default color is black.
   */
  COLOR,

  /**
   * @brief The height in pixels of the strikethrough.
   * @details Name "height", type Property::STRING or Property::FLOAT. i.e. "1.0" or 1.f
   * @note Optional. If not provided then the default height is used (1 pixel).
   */
  HEIGHT
};

} // namespace Property

} // namespace Strikethrough

} // namespace DevelText

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_STYLE_PROPERTIES_DEVEL_H
