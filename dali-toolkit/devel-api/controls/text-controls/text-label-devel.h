#ifndef DALI_TOOLKIT_TEXT_LABEL_DEVEL_H
#define DALI_TOOLKIT_TEXT_LABEL_DEVEL_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
    UNUSED_PROPERTY_TEXT_COLOR = Dali::Toolkit::TextLabel::Property::UNUSED_PROPERTY_TEXT_COLOR,
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
    PIXEL_SIZE = Dali::Toolkit::TextLabel::Property::PIXEL_SIZE,
    ELLIPSIS = Dali::Toolkit::TextLabel::Property::ELLIPSIS,
    AUTO_SCROLL_LOOP_DELAY = Dali::Toolkit::TextLabel::Property::AUTO_SCROLL_LOOP_DELAY,
    AUTO_SCROLL_STOP_MODE = Dali::Toolkit::TextLabel::Property::AUTO_SCROLL_STOP_MODE,
    LINE_COUNT = Dali::Toolkit::TextLabel::Property::LINE_COUNT,
    LINE_WRAP_MODE = Dali::Toolkit::TextLabel::Property::LINE_WRAP_MODE,

    /**
     * @brief The direction of the layout.
     * @details Name "textDirection", type [Type](@ref Dali::Toolkit::DevelText::TextDirection::Type) (Property::INTEGER), Read/Write
     * @note The text direction can be changed only by replacing the text itself.
     * @see TextDirection::Type for supported values.
     */
    TEXT_DIRECTION,

    /**
     * @brief Alignment of text within area of single line
     * @details Name "verticalLineAlignment", type [Type](@ref Dali::Toolkit::DevelText::VerticalLineAlignment::Type) (Property::INTEGER), Read/Write
     * @note The default value is TOP
     * @see VerticalLineAlignment::Type for supported values
     */
    VERTICAL_LINE_ALIGNMENT,

    /**
     * @brief The default text background parameters.
     * @details Name "textBackground", type Property::MAP.
     * @note Use "textBackground" as property name to avoid conflict with Control's "background" property
     *
     * The background map contains the following keys:
     *
     * | %Property Name       | Type     | Required | Description                                                                                                        |
     * |----------------------|----------|----------|--------------------------------------------------------------------------------------------------------------------|
     * | enable               | BOOLEAN  | No       | True to enable the background or false to disable (the default value is false)                                     |
     * | color                | VECTOR4  | No       | The color of the background (the default value is Color::CYAN)                                                     |
     */
    BACKGROUND,
  };

} // namespace Property

} // namespace DevelTextLabel

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_LABEL_DEVEL_H
