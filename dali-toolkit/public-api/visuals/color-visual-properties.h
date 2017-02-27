#ifndef DALI_TOOLKIT_COLOR_VISUAL_PROPERTIES_H
#define DALI_TOOLKIT_COLOR_VISUAL_PROPERTIES_H

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

/**
 * @brief ColorVisual is to render a solid color to the control's quad.
 * @SINCE_1_1.45
 */
namespace ColorVisual
{

/**
 * @brief ColorVisual Property
 * @SINCE_1_1.45
 */
namespace Property
{

/**
 * @brief ColorVisual Property
 * @SINCE_1_1.45
 */
enum
{
  /**
   * @brief The solid color required.
   * @details Name "mixColor", type Property::VECTOR4.
   * @SINCE_1_1.45
   * @note Mandatory.
   */
  MIX_COLOR = VISUAL_PROPERTY_START_INDEX,
};

} // namespace Property

} // namespace ColorVisual

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_COLOR_VISUAL_PROPERTIES_H
