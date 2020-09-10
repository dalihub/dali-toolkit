#ifndef DALI_TOOLKIT_TEXT_VISUAL_PROPERTIES_H
#define DALI_TOOLKIT_TEXT_VISUAL_PROPERTIES_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/toolkit-property-index-ranges.h>

namespace Dali
{
namespace Toolkit
{
/**
 * @addtogroup dali_toolkit_visuals
 * @{
 */

/**
 * @brief TextVisual is to render a text.
 * @SINCE_1_2.60
 */
namespace TextVisual
{
/**
 * @brief TextVisual Property.
 * @SINCE_1_2.60
 */
namespace Property
{
/**
 * @brief Enumeration for the instance of properties belonging to the TextVisual.
 * @SINCE_1_2.60
 */
enum
{
  /**
   * @brief The text to display in UTF-8 format.
   * @details name "text", type Property::STRING.
   * @SINCE_1_2.60
   */
  TEXT = VISUAL_PROPERTY_START_INDEX,

  /**
   * @brief The requested font family to use.
   * @details name "fontFamily", type Property::STRING.
   * @SINCE_1_2.60
   */
  FONT_FAMILY,

  /**
   * @brief The requested font style to use.
   * @details name "fontStyle", type Property::MAP.
   * @SINCE_1_2.60
   */
  FONT_STYLE,

  /**
   * @brief The size of font in points.
   * @details name "pointSize", type Property::FLOAT.
   * @SINCE_1_2.60
   */
  POINT_SIZE,

  /**
   * @brief The single-line or multi-line layout option.
   * @details name "multiLine", type Property::BOOLEAN, default false.
   * @SINCE_1_2.60
   */
  MULTI_LINE,

  /**
   * @brief The line horizontal alignment.
   * @details Name "horizontalAlignment", type HorizontalAlignment::Type (Property::INTEGER) or Property::STRING.
   * @note Optional. If not specified, the default is HorizontalAlignment::BEGIN
   * @note Return type is HorizontalAlignment::Type (Property::INTEGER)
   * @SINCE_1_2.60
   */
  HORIZONTAL_ALIGNMENT,

  /**
   * @brief The line vertical alignment.
   * @details name "verticalAlignment", VerticalAlignment::Type (Property::INTEGER) or  Property::STRING
   * @note Optional. If not specified, the default is VerticalAlignment::TOP
   * @note Return type is VerticalAlignment::Type (Property::INTEGER)`
   * @SINCE_1_2.60
   */
  VERTICAL_ALIGNMENT,

  /**
   * @brief The color of the text.
   * @details name "textColor", type Property::VECTOR4.
   * @SINCE_1_2.60
   */
  TEXT_COLOR,

  /**
   * @brief  Whether the mark-up processing is enabled.
   * @details name "enableMarkup", type Property::BOOLEAN.
   * @SINCE_1_2.60
   */
  ENABLE_MARKUP,

  /**
   * @brief The shadow parameters.
   * @details name "shadow", type Property::MAP.
   * @SINCE_1_2.60
   */
  SHADOW,

  /**
   * @brief The default underline parameters.
   * @details name "underline", type Property::MAP.
   * @SINCE_1_2.60
   */
  UNDERLINE,
};

} // namespace Property

} // namespace TextVisual

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_VISUAL_PROPERTIES_H
