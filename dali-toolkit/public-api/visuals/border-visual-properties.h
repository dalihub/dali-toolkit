#ifndef DALI_TOOLKIT_BORDER_VISUAL_PROPERTIES_H
#define DALI_TOOLKIT_BORDER_VISUAL_PROPERTIES_H

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

/**
 * @addtogroup dali_toolkit_visuals
 * @{
 */

namespace BorderVisual
{

namespace Property
{

enum
{
  /**
   * @brief The color of the border.
   * @details Name "borderColor", type Property::VECTOR4.
   * @SINCE_1_1.45
   * @note Mandatory.
   */
  COLOR = VISUAL_PROPERTY_START_INDEX,

  /**
   * @brief The width of the border (in pixels).
   * @details Name "borderSize", type Property::FLOAT.
   * @SINCE_1_1.45
   * @note Mandatory.
   */
  SIZE,

  /**
   * @brief Whether anti-aliasing of the border is required.
   * @details Name "antiAliasing", type Property::BOOLEAN.
   * @SINCE_1_1.45
   * @note Optional. If not supplied, default is false.
   */
  ANTI_ALIASING,
};

} // namespace Property

} // namespace BorderVisual

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_BORDER_VISUAL_PROPERTIES_H
