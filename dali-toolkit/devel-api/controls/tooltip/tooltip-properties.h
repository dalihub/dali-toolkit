#ifndef DALI_TOOLKIT_TOOLTIP_PROPERTIES_H
#define DALI_TOOLKIT_TOOLTIP_PROPERTIES_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/object/property-index-ranges.h>

namespace Dali
{

namespace Toolkit
{

namespace Tooltip
{

/**
 * @brief The properties used for a Tooltip.
 */
namespace Property
{

enum
{
  /**
   * @brief The content to display.
   * @details Name "content", type Property::STRING, Property::MAP or Property::ARRAY.
   *          If Property::STRING is used, then the string is shown as a text and the default font style for a Tooltip will be used.
   *          This can be overridden by passing in a Property::MAP of TextVisual properties. For text styling purposes, a Property::MAP excluding the TEXT property can be sent.
   *          If a different Visual is required, then a Property::MAP defining that visual can be set.
   *          If more than one visual is required, then a Property::ARRAY can be used. The contents are added to the layout as per their order in the array. Text has to be styled in the visual passed in (default toolkit style will not be used).
   * @note Mandatory.
   * @note If set using Property::STRING, then when retrieved, a Property::MAP is returned.
   *       If set using a Property::MAP or Property::ARRAY, then the appropriate type is returned.
   * @see Toolkit::TextVisual
   */
  CONTENT = CORE_PROPERTY_MAX_INDEX + 1,

  /**
   * @brief The layout of the content.
   * @details Name "layout", type Property::VECTOR2.
   *          The number of rows and columns expected.
   *          ( 1, 2 ) means 1 row, 2 columns so the content will have two items placed on one row.
   *          ( 2, 2 ) means 2 rows, 2 columns so the content will have 4 items with two items placed on each row.
   * @note Optional.
   * @note If not provided, the default is to put all items in the same row.
   */
  LAYOUT,

  /**
   * @brief Time to wait in seconds before a tooltip is shown while the is movement is within the allowed threshold.
   * @details Name "waitTime", type Property::FLOAT.
   * @note Optional.
   * @note If not provided, the default is 0.5 seconds.
   */
  WAIT_TIME,

  /**
   * @brief The background of the tooltip.
   * @details Name "background", type Property::STRING or Property::MAP.
   *          If Property::STRING, then the path to the image is required and it's assumed that there are no borders.
   * @note Optional.
   * @note If not provided, the default is taken from the stylesheet.
   * @note When retrieved, a Property::MAP is returned.
   * @see Tooltip::Background
   */
  BACKGROUND,

  /**
   * @brief The tail used by the tooltip.
   * @details Name "tail", type Property::BOOLEAN or Property::MAP.
   *          If Property::BOOLEAN and true, then the default visuals are used for the tail.
   *          A Property::MAP can be used to override the visuals for the tail.
   * @note Optional.
   * @note The default is false, i.e. to not show a tail.
   * @note When retrieved, a Property::MAP is returned.
   * @note If the popup has to be moved because it goes out of bounds, then the tail is not shown regardless of whether it is set or not.
   * @see Tooltip::Tail
   */
  TAIL,

  /**
   * @brief The position of the tooltip in relation to the control.
   * @details Name "position", type Tooltip::Position::Type (Property::INTEGER) or Property::STRING.
   * @note Optional.
   * @note If not provided, the default is Tooltip::Position::BELOW.
   * @note When retrieved, a Tooltip::Position::Type (Property::INTEGER) is returned.
   */
  POSITION,

  /**
   * @brief If Tooltip::Position::HOVER_POINT is used for the POSITION, then this is the offset the tooltip is displayed at from the hover point.
   * @details Name "hoverPointOffset", type Property::VECTOR2.
   * @note Optional.
   * @note If not provided, the default is Vector2( 10.0f, 10.0f ).
   */
  HOVER_POINT_OFFSET,

  /**
   * @brief The movement threshold allowed before showing (or hiding a popup).
   * @details Name "movementThreshold", type Property::INTEGER.
   *          This value is used as the threshold to hide the popup as well if DISAPPEAR_ON_MOVEMENT is set to true.
   * @note Optional.
   * @note If not provided, the default is 5.
   */
  MOVEMENT_THRESHOLD,

  /**
   * @brief If true, the tooltip will disappear after hover movement beyond a certain distance.
   * @details Name "disappearOnMovement", type Property::BOOLEAN.
   * @note Optional.
   * @note If not provided, the default is to disappear only when moving out of bounds of the control.
   */
  DISAPPEAR_ON_MOVEMENT,
};

} // namespace Property

namespace Background
{

namespace Property
{

enum
{
  /**
   * @brief The image to use as the background.
   * @details Name "visual", type Property::STRING.
   */
  VISUAL,

  /**
   * @brief The size of the borders in the order: left, right, bottom, top.
   * @details Name "border", type Property::RECTANGLE.
   *          This is different from the nPatch border as the it will place the content within the bounds specified.
   *          For example, there could be some round corners in the image used and we may not want the content going over the rounded corners.
   * @note Optional.
   * @note If not provided, then then it is assumed that the image does not have a border.
   */
  BORDER
};

} // namespace Property

} // namespace Background

/**
 * @brief The tail used by the tooltip.
 */
namespace Tail
{

/**
 * @brief The properties of the tail used by the tooltip.
 */
namespace Property
{

enum
{
  /**
   * @brief Whether to show the tail or not.
   * @details Name "visibility", type Property::BOOLEAN.
   * @note Optional.
   * @note If the popup has to be moved because it goes out of bounds, then the tail is not shown regardless of whether it is set or not.
   */
  VISIBILITY,

  /**
   * @brief The image used for the tail if it is above the tooltip.
   * @details Name "aboveVisual", type Property::STRING.
   */
  ABOVE_VISUAL,

  /**
   * @brief The image used for the tail if it is below the tooltip.
   * @details Name "belowVisual", type Property::STRING.
   */
  BELOW_VISUAL
};

} // namespace Property

} // namespace Tail

namespace Position
{

/**
 * @brief The position of the tooltip in relation to the control.
 */
enum Type
{
  ABOVE, ///< The tooltip will appear above the control.
  BELOW, ///< The tooltip will appear below the control.
  HOVER_POINT ///< The tooltip will appear near the hover point.
};

} // namespace Position

} // namespace Tooltip

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TOOLTIP_PROPERTIES_H
