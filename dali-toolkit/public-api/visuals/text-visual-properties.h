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
   * @brief The type of rendering e.g. bitmap-based
   * @details name "renderingBackend", type INTEGER, default RENDERING_SHARED_ATLAS
   * @SINCE_1_2.11
   */
  RENDERING_BACKEND = VISUAL_PROPERTY_START_INDEX,

  /**
   * @brief The text to display in UTF-8 format,
   * @details name "text", type STRING
   * @SINCE_1_2.11
   */
  TEXT,

  /**
   * @brief The requested font family to use,
   * @details name "fontFamily", type STRING
   * @SINCE_1_2.11
   */
  FONT_FAMILY,

  /**
   * @brief The requested font style to use,
   * @details name "fontStyle", type STRING or MAP
   * @SINCE_1_2.13
   */
  FONT_STYLE,

  /**
   * @brief The size of font in points
   * @details name "pointSize", type FLOAT
   * @SINCE_1_2.11
   */
  POINT_SIZE,

  /**
   * @brief The single-line or multi-line layout option
   * @details name "multiLine", type BOOLEAN, default false
   * @SINCE_1_2.11
   */
  MULTI_LINE,

  /**
   * @brief The line horizontal alignment
   * @details name "horizontalAlignment", type STRING,  values "BEGIN", "CENTER", "END", default BEGIN
   * @SINCE_1_2.11
   */
  HORIZONTAL_ALIGNMENT,

  /**
   * @brief The line vertical alignment
   * @details name "verticalAlignment", type STRING,  values "TOP",   "CENTER", "BOTTOM", default TOP
   * @SINCE_1_2.11
   */
  VERTICAL_ALIGNMENT,

  /**
   * @brief The color of the text
   * @details name "textColor", type VECTOR4
   * @SINCE_1_2.11
   */
  TEXT_COLOR,

  /**
   * @brief  Whether the mark-up processing is enabled
   * @details name "enableMarkup", type BOOLEAN
   * @SINCE_1_2.11
   */
  ENABLE_MARKUP,

  /**
   * @brief  Start or stop auto scrolling,
   * @details name "enableAutoScroll", type BOOLEAN, default is false
   * @SINCE_1_2.11
   */
  ENABLE_AUTO_SCROLL,

  /**
   * @brief Sets the speed of scrolling in pixels per second,
   * @details name "autoScrollSpeed", type INTEGER, default in style sheet
   * @SINCE_1_2.11
   */
  AUTO_SCROLL_SPEED,

  /**
   * @brief  Number of complete loops when scrolling enabled
   * @details name "autoScrollLoopCount", type INTEGER, default in style sheet
   * @SINCE_1_2.11
   */
  AUTO_SCROLL_LOOP_COUNT,

  /**
   * @brief  Gap before before scrolling wraps
   * @details name "autoScrollGap", type INTEGER, default in style sheet but can be overridden to prevent same text being show at start and end.
   * @SINCE_1_2.11
   */
  AUTO_SCROLL_GAP,

  /**
   * @brief The default extra space between lines in points.
   * @details name "lineSpacing", type FLOAT.
   * @SINCE_1_2.11
   */
  LINE_SPACING,

  /**
   * @brief The default underline parameters.
   * @details name "underline", type STRING or MAP.
   * @SINCE_1_2.13
   */
  UNDERLINE,

  /**
   * @brief The default shadow parameters.
   * @details name "shadow", type STRING or MAP.
   * @SINCE_1_2.13
   */
  SHADOW,

  /**
   * @brief The default emboss parameters.
   * @details name "emboss", type STRING or MAP.
   * @SINCE_1_2.13
   */
  EMBOSS,

  /**
   * @brief The default outline parameters.
   * @details name "outline", type STRING or MAP.
   * @SINCE_1_2.13
   */
  OUTLINE,

  /**
   * @brief This enables Text visuals to automatically be converted to Batch-Text visuals.
   * @details Name "batchingEnabled", type Property::BOOLEAN.
   * @SINCE_1_2.11
   * @note Optional. If not specified, the default is false.
   */
  BATCHING_ENABLED,
};

} // namespace Property

} // namespace TextVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_VISUAL_PROPERTIES_H
