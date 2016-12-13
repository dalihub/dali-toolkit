#ifndef DALI_TOOLKIT_TEXT_VISUAL_PROPERTIES_H
#define DALI_TOOLKIT_TEXT_VISUAL_PROPERTIES_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

namespace TextVisual
{

namespace Property
{

enum
{
  /**
   * @brief The text to display in UTF-8 format,
   * @details name "text", type STRING
   */
  TEXT = VISUAL_PROPERTY_START_INDEX,

  /**
   * @brief The requested font family to use,
   * @details name "fontFamily", type STRING
   */
  FONT_FAMILY,

  /**
   * @brief The requested font style to use,
   * @details name "fontStyle", type MAP
   */
  FONT_STYLE,

  /**
   * @brief The size of font in points
   * @details name "pointSize", type FLOAT
   */
  POINT_SIZE,

  /**
   * @brief The single-line or multi-line layout option
   * @details name "multiLine", type BOOLEAN, default false
   */
  MULTI_LINE,

  /**
   * @brief The line horizontal alignment
   * @details name "horizontalAlignment", type STRING,  values "BEGIN", "CENTER", "END", default BEGIN
   */
  HORIZONTAL_ALIGNMENT,

  /**
   * @brief The line vertical alignment
   * @details name "verticalAlignment", type STRING,  values "TOP",   "CENTER", "BOTTOM", default TOP
   */
  VERTICAL_ALIGNMENT,

  /**
   * @brief The color of the text
   * @details name "textColor", type VECTOR4
   */
  TEXT_COLOR,

  /**
   * @brief  Whether the mark-up processing is enabled
   * @details name "enableMarkup", type BOOLEAN
   */
  ENABLE_MARKUP,
};

} // namespace Property

} // namespace TextVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_VISUAL_PROPERTIES_H
