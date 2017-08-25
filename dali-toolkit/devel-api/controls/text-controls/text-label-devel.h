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

/**
 * @brief Enumeration for the start and end property ranges for this control.
 * @SINCE_1_0.0
 */
enum PropertyRange
{
  ANIMATABLE_PROPERTY_START_INDEX = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,
  ANIMATABLE_PROPERTY_END_INDEX =   ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1000 ///< Reserve animatable property indices
};

namespace Property
{
  enum Type
  {
    ///////////////////////////////////////////////////////////////////////////////
    // Event side (non-animatable) properties
    ///////////////////////////////////////////////////////////////////////////////

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
     * @details Name "pixelSize", type Property::FLOAT.
     *          Conversion from Point size to Pixel size:
     *            Pixel size = Point size * DPI / 72
     */
    PIXEL_SIZE = OUTLINE + 1,

    /**
     * @brief Enable or disable the ellipsis.
     * @details Name "ellipsis", type Property::BOOLEAN.
     */
    ELLIPSIS = OUTLINE + 2,

    /**
     * @brief delay starting time of auto scrolling and further loops
     * @details Name "autoScrollLoopDelay", type Property::FLOAT.
     */
    AUTO_SCROLL_LOOP_DELAY = OUTLINE + 3,

    /**
     * @brief Auto scrolling stop behaviour.
     * @details Name "autoScrollStopMode", type [Type](@ref Dali::Toolkit::DevelTextLabel::AutoScrollStopMode::Type) (Property::INTEGER) or Property::STRING.
     *          Values FINISH_LOOP, IMMEDIATE, default FINISH_LOOP
     */
    AUTO_SCROLL_STOP_MODE = OUTLINE + 4,

    /*
     * @brief The line count of text.
     * @details name "lineCount", type int
     * @node This property is read-only.
     */
    LINE_COUNT = OUTLINE + 5,

    /**
     * @brief line wrap mode when the text lines over layout width.
     * @details Name "lineWrapMode", type Property::STRING.
     */
    LINE_WRAP_MODE = OUTLINE + 6,

    ///////////////////////////////////////////////////////////////////////////////
    // Animatable Properties
    ///////////////////////////////////////////////////////////////////////////////

    /*
     * @brief Animatable text color.
     * @details Name "textColorAnimatable", type Property::VECTOR4.
     *          Text color property to use if required to animate the text color.
     */
    TEXT_COLOR_ANIMATABLE = ANIMATABLE_PROPERTY_START_INDEX,

    /**
     * @brief The red component of the text color.
     * @details Name "textColorRed", type Property::FLOAT.
     * @see TEXT_COLOR_ANIMATABLE
     */
    TEXT_COLOR_RED = ANIMATABLE_PROPERTY_START_INDEX + 1,

    /**
     * @brief The green component of the text color.
     * @details Name "textColorGreen", type Property::FLOAT.
     * @see TEXT_COLOR_ANIMATABLE
     */
    TEXT_COLOR_GREEN = ANIMATABLE_PROPERTY_START_INDEX + 2,

    /**
     * @brief The blue component of the text color.
     * @details Name "textColorBlue", type Property::FLOAT.
     * @see TEXT_COLOR_ANIMATABLE
     */
    TEXT_COLOR_BLUE = ANIMATABLE_PROPERTY_START_INDEX + 3,

    /**
     * @brief The alpha component of the text color.
     * @details Name "textColorAlpha", type Property::FLOAT.
     * @see TEXT_COLOR_ANIMATABLE
     */
    TEXT_COLOR_ALPHA = ANIMATABLE_PROPERTY_START_INDEX + 4,
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
