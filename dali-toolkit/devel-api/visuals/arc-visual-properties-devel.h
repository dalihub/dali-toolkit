#ifndef DALI_TOOLKIT_ARC_VISUAL_PROPERTIES_DEVEL_H
#define DALI_TOOLKIT_ARC_VISUAL_PROPERTIES_DEVEL_H

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

namespace DevelArcVisual
{

/**
 * @brief ArcVisual Properties.
 */
namespace Property
{

/**
 * @brief Enumeration for the instance of properties belonging to the ArcVisual.
 */
enum
{
  /**
   * @brief The thickness of the arc.
   * @details Name "thickness", type Property::FLOAT.
   * @note Mandatory.
   */
  THICKNESS = VISUAL_PROPERTY_START_INDEX,

  /**
   * @brief The start angle where the arc begins in degrees.
   * @details Name "startAngle", type Property::FLOAT.
   * @note Optional. If not specified, the default is 0.
   */
  START_ANGLE,

  /**
   * @brief The sweep angle of the arc in degrees.
   * @details Name "sweepAngle", type Property::FLOAT.
   * The arc starts at a specified start angle and sweeps clockwise, drawing slices of pie.
   * @note Optional. If not specified, the default is 360.
   */
  SWEEP_ANGLE,

  /**
   * @brief The cap of the arc.
   * @details Name "cap", Type Cap::Type (Property::INTEGER)
   * It specifies the shape of the endpoints.
   * @note Optional. If not specified, the default is Cap::BUTT.
   */
  CAP,
};

} // namespace Property

/**
 * @brief Enumeration for the cap style.
 */
namespace Cap
{

enum Type
{
  BUTT,      ///< The arc does not extend beyond its two endpoints.
  ROUND      ///< The arc will be extended by a half circle with the center at the end.
};

} // namespace Cap

} // namespace DevelArcVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_ARC_VISUAL_PROPERTIES_DEVEL_H
