#ifndef DALI_TOOLKIT_COLOR_VISUAL_PROPERTIES_DEVEL_H
#define DALI_TOOLKIT_COLOR_VISUAL_PROPERTIES_DEVEL_H

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
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>

namespace Dali
{

namespace Toolkit
{


namespace DevelColorVisual
{

/**
 * @brief Additional ColorVisual Properties.
 */
namespace Property
{

/**
 * @brief Enumeration for the instance of additional properties belonging to the ColorVisual.
 */
enum
{
  MIX_COLOR = Toolkit::ColorVisual::Property::MIX_COLOR,

  /**
   * @brief Whether to render if the MIX_COLOR is transparent.
   * @details Name "renderIfTransparent", type Property::BOOLEAN.
   * @note Optional.
   * @note By default it's false, i.e. ColorVisual will not render if the MIX_COLOR is transparent.
   */
  RENDER_IF_TRANSPARENT = MIX_COLOR + 1,
};

} // namespace Property

} // namespace ColorVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_COLOR_VISUAL_PROPERTIES_DEVEL_H
