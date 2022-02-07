#ifndef DALI_TOOLKIT_DEVEL_API_VISUALS_TEXT_VISUAL_PROPERTIES_DEVEL_H
#define DALI_TOOLKIT_DEVEL_API_VISUALS_TEXT_VISUAL_PROPERTIES_DEVEL_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/visuals/text-visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelTextVisual
{
namespace Property
{
/**
 * @brief All the visual types.
 */
enum
{
  TEXT                 = Dali::Toolkit::TextVisual::Property::TEXT,
  FONT_FAMILY          = Dali::Toolkit::TextVisual::Property::FONT_FAMILY,
  FONT_STYLE           = Dali::Toolkit::TextVisual::Property::FONT_STYLE,
  POINT_SIZE           = Dali::Toolkit::TextVisual::Property::POINT_SIZE,
  MULTI_LINE           = Dali::Toolkit::TextVisual::Property::MULTI_LINE,
  HORIZONTAL_ALIGNMENT = Dali::Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT,
  VERTICAL_ALIGNMENT   = Dali::Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT,
  TEXT_COLOR           = Dali::Toolkit::TextVisual::Property::TEXT_COLOR,
  ENABLE_MARKUP        = Dali::Toolkit::TextVisual::Property::ENABLE_MARKUP,
  SHADOW               = Dali::Toolkit::TextVisual::Property::SHADOW,
  UNDERLINE            = Dali::Toolkit::TextVisual::Property::UNDERLINE,

  /**
   * @brief The default outline parameters.
   * @details name "outline", type Property::MAP.
   */
  OUTLINE = UNDERLINE + 1,

  /**
   * @copydoc Dali::Toolkit::DevelTextLabel::Property::BACKGROUND
   */
  BACKGROUND = UNDERLINE + 2,

  /**
   * @copydoc Dali::Toolkit::DevelTextLabel::Property::STRIKETHROUGH
   */
  STRIKETHROUGH = UNDERLINE + 3,

  /**
   * @copydoc Dali::Toolkit::DevelTextLabel::Property::CHARACTER_SPACING
   */
  CHARACTER_SPACING = UNDERLINE + 4,
};

} // namespace Property

} // namespace DevelTextVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_VISUALS_TEXT_VISUAL_PROPERTIES_DEVEL_H
