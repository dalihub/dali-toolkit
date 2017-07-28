#ifndef DALI_TOOLKIT_TEXT_LABEL_DEVEL_H
#define DALI_TOOLKIT_TEXT_LABEL_DEVEL_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>

namespace Dali
{

namespace Toolkit
{

namespace DevelTextLabel
{

namespace Property
{
  enum Type
  {
      RENDERING_BACKEND = Dali::Toolkit::TextLabel::Property::RENDERING_BACKEND,
      TEXT = Dali::Toolkit::TextLabel::Property::TEXT,
      FONT_FAMILY = Dali::Toolkit::TextLabel::Property::FONT_FAMILY,
      FONT_STYLE = Dali::Toolkit::TextLabel::Property::FONT_STYLE,
      POINT_SIZE = Dali::Toolkit::TextLabel::Property::POINT_SIZE,
      MULTI_LINE = Dali::Toolkit::TextLabel::Property::MULTI_LINE,
      HORIZONTAL_ALIGNMENT = Dali::Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT,
      VERTICAL_ALIGNMENT = Dali::Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT,
      TEXT_COLOR = Dali::Toolkit::TextLabel::Property::TEXT_COLOR,
      SHADOW_OFFSET = Dali::Toolkit::TextLabel::Property::SHADOW_OFFSET,
      SHADOW_COLOR = Dali::Toolkit::TextLabel::Property::SHADOW_COLOR,
      UNDERLINE_ENABLED = Dali::Toolkit::TextLabel::Property::UNDERLINE_ENABLED,
      UNDERLINE_COLOR = Dali::Toolkit::TextLabel::Property::UNDERLINE_COLOR,
      UNDERLINE_HEIGHT = Dali::Toolkit::TextLabel::Property::UNDERLINE_HEIGHT,
      ENABLE_MARKUP = Dali::Toolkit::TextLabel::Property::ENABLE_MARKUP,
      ENABLE_AUTO_SCROLL = Dali::Toolkit::TextLabel::Property::ENABLE_AUTO_SCROLL,
      AUTO_SCROLL_SPEED = Dali::Toolkit::TextLabel::Property::AUTO_SCROLL_SPEED,
      AUTO_SCROLL_LOOP_COUNT = Dali::Toolkit::TextLabel::Property::AUTO_SCROLL_LOOP_COUNT,
      AUTO_SCROLL_GAP = Dali::Toolkit::TextLabel::Property::AUTO_SCROLL_GAP,
      LINE_SPACING = Dali::Toolkit::TextLabel::Property::LINE_SPACING,
      UNDERLINE = Dali::Toolkit::TextLabel::Property::UNDERLINE,
      SHADOW = Dali::Toolkit::TextLabel::Property::SHADOW,
      EMBOSS = Dali::Toolkit::TextLabel::Property::EMBOSS,
      OUTLINE = Dali::Toolkit::TextLabel::Property::OUTLINE,

      /**
       * @brief The size of font in pixels.
       *
       * Conversion from Point size to Pixel size :
       *  Pixel size = Point size * DPI / 72
       * @details name "pixelSize", type float
       */
      PIXEL_SIZE = OUTLINE + 1,

      /**
       * @brief Enable or disable the ellipsis.
       * @details name "ellipsis", type bool
       */
      ELLIPSIS = OUTLINE + 2,

      /**
       * @brief delay starting time of auto scrolling and further loops
       * @details name "autoScrollLoopDelay", type float.
       */
      AUTO_SCROLL_LOOP_DELAY = OUTLINE + 3,

      /**
       * @brief Auto scrolling stop behaviour.
       * @details name "autoScrollStopMode", type [Type](@ref Dali::Toolkit::DevelTextLabel::AutoScrollStopMode::Type) (Property::INTEGER) or Property::STRING.
       *          values FINISH_LOOP, IMMEDIATE, defualt FINISH_LOOP
       */
      AUTO_SCROLL_STOP_MODE = OUTLINE + 4,

      /*
       * @brief The line count of text.
       * @details name "lineCount", type int
       * @node this property is read-only.
       */
      LINE_COUNT = OUTLINE + 5,

      /**
       * @brief line wrap mode when the text lines over layout width.
       * @details name "lineWrapMode", type string.
       */
      LINE_WRAP_MODE = OUTLINE + 6
  };
} // namespace Property

/**
* @brief The type for TextLabel::Property::AUTO_SCROLL_STOP_MODE
*/
namespace AutoScrollStopMode
{
/**
* @ref Dali::Toolkit::DevelTextLabel::AutoScrollStopMode
*/
enum Type
{
  FINISH_LOOP = 0,  ///< stop animation after current loop finished.
  IMMEDIATE         ///< stop animation immediatly and reset position.
};
} // namespace AutoScrollStopMode

} // namespace DevelText

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_LABEL_DEVEL_H
