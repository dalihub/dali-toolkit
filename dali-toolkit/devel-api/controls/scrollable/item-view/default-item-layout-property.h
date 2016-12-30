#ifndef DALI_TOOLKIT_DEFAULT_ITEM_LAYOUT_PROPERTY_H
#define DALI_TOOLKIT_DEFAULT_ITEM_LAYOUT_PROPERTY_H

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

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Default item layout property.
 */
namespace DefaultItemLayoutProperty
{

/**
 * @brief The properties of each type of item layout.
 */
enum Property
{
  /**
  * @brief The type of the Layout.
  * @details Name "type", type(Dali::Toolkit::DefaultItemLayout::Type),Property::INTEGER
  * @note Mandatory.
  */
  TYPE = 0,

  /**
  * @brief The size of each item in the Layout.
  * @details Name "itemSize",Property::VECTOR3
  * @note Optional.
  * @note If not supplied, see ItemLayout::GetDefaultItemSize().
  */
  ITEM_SIZE,

  /**
  * @brief The internal orientation of the Layout.
  * @details Name "orientation",type(@ref Dali::Toolkit::ControlOrientation::Type),Property::INTEGER
  * @note Optional.
  * @note If not supplied, the default is ControlOrientation::Up, The contents of control are in a vertical layout, from top to bottom.
  */
  ORIENTATION,

  /**
  * @brief The number of columns in the GridLayout.
  * @details Name "gridColumnNumber",Property::INTEGER.
  * @note Optional.
  * @note If not supplied, the default is 4.
  */
  GRID_COLUMN_NUMBER,

  /**
  * @brief The spacing between rows in the GridLayout.
  * @details Name "gridRowSpacing",Property::FLOAT
  * @note Optional.
  * @note If not supplied, the default is 20.0f.
  */
  GRID_ROW_SPACING,

  /**
  * @brief The spacing between columns in the GridLayout.
  * @details Name "gridColumnSpacing",Property::FLOAT
  * @note Optional.
  * @note If not supplied, the default is 20.0f.
  */
  GRID_COLUMN_SPACING,

  /**
  * @brief The margin in the top of the GridLayout.
  * @details Name "gridTopMargin",Property::FLOAT
  * @note Optional.
  * @note If not supplied, the default is 95.0f.
  */
  GRID_TOP_MARGIN,

  /**
  * @brief The margin in the bottom of the GridLayout.
  * @details Name "gridBottomMargin",Property::FLOAT
  * @note Optional.
  * @note If not supplied, the default is 20.0f.
  */
  GRID_BOTTOM_MARGIN,

  /**
  * @brief The margin in the left and right of the GridLayout.
  * @details Name "gridSideMargin",Property::FLOAT
  * @note Optional.
  * @note If not supplied, the default is 20.0f.
  */
  GRID_SIDE_MARGIN,

  /**
  * @brief The factor used to customise the scroll speed while dragging and swiping the GridLayout.
  * @details Name "gridScrollSpeedFactor",Property::FLOAT
  * @note Optional.
  * @note If not supplied, the default is 0.03f.
  */
  GRID_SCROLL_SPEED_FACTOR,

  /**
  * @brief The maximum swipe speed in pixels per second of GridLayout.
  * @details Name "gridMaximumSwipSpeed",Property::FLOAT
  * @note Optional.
  * @note If not supplied, the default is 100.0f.
  */
  GRID_MAXIMUM_SWIPE_SPEED,

  /**
  * @brief The duration of the flick animation in seconds of GridLayout.
  * @details Name "gridItemFlickAnimationDuration",Property::FLOAT
  * @note Optional.
  * @note This is the time taken to animate each item to its next layout position (e.g. from 1.0 to 2.0) when a flick animation is triggered by a swipe gesture.
  * @note Must be greater than zero;If not supplied, the default is 0.015f.
  */
  GRID_ITEM_FLICK_ANIMATION_DURATION,

  /**
  * @brief The number of columns in the DepthLayout.
  * @details Name "depthColumnNumber",Property::INTEGER
  * @note Optional.
  * @note If not supplied, the default is 3.
  */
  DEPTH_COLUMN_NUMBER,

  /**
  * @brief The number of rows in the DepthLayout.
  * @details Name "depthRowNumber",Property::INTEGER
  * @note Optional.
  * @note If not supplied, the default is 26.
  */
  DEPTH_ROW_NUMBER,

  /**
  * @brief The spacing between rows in the DepthLayout.
  * @details Name "depthRowSpacing",Property::FLOAT
  * @note Optional.
  * @note If not supplied, the default is 55.0f.
  */
  DEPTH_ROW_SPACING,

  /**
  * @brief The factor used to customise the scroll speed while dragging and swiping the  DepthLayout.
  * @details Name "depthScrollSpeedFactor",Property::FLOAT
  * @note Optional.
  * @note If not supplied, the default is 0.02f.
  */
  DEPTH_SCROLL_SPEED_FACTOR,

  /**
  * @brief The maximumSwipSpeed of the DepthLayout.
  * @details Name "depthMaximumSwipSpeed",Property::FLOAT
  * @note Optional.
  * @note If not supplied, the default is 50.0f.
  */
  DEPTH_MAXIMUM_SWIPE_SPEED,

  /**
  * @brief The duration of the flick animation in seconds of DepthLayout.
  * @details Name "depthItemFlickAnimationDuration",Property::FLOAT
  * @note Optional.
  * @note This is the time taken to animate each item to its next layout position (e.g. from 1.0 to 2.0) when a flick animation is triggered by a swipe gesture.
  * @note Must be greater than zero; If not supplied, the default is 0.03f.
  */
  DEPTH_ITEM_FLICK_ANIMATION_DURATION,

  /**
  * @brief The tilt angle of DepthLayout.
  * @details Name "depthTiltAngle",Property::FLOAT
  * @note Optional.
  * @note If not supplied, the default is (Math::PI)*0.15f.
  * @note This is clamped between -45 & 45 degrees.
  */
  DEPTH_TILT_ANGLE,

  /**
  * @brief The tilt angle of the individual items in the DepthLayout.
  * @details Name "depthItemTiltAngle",Property::FLOAT
  * @note Optional.
  * @note If not supplied, the default is -(Math::PI)*0.025f.
  */
  DEPTH_ITEM_TILT_ANGLE,

  /**
  * @brief The spacing angle between items in the SpiralLayout.
  * @details Name "spiralItemSpacing",Property::FLOAT
  * @note Optional.
  * @note If not supplied, the default is 9.5f.
  */
  SPIRAL_ITEM_SPACING,

  /**
  * @brief The factor used to customise the scroll speed while dragging and swiping the SpiralLayout.
  * @details Name "spiralScrollSpeedFactor",Property::FLOAT
  * @note Optional.
  * @note If not supplied, the default is 0.01f.
  */
  SPIRAL_SCROLL_SPEED_FACTOR,

  /**
  * @brief The maximum swipe speed in pixels per second of the SpiralLayout.
  * @details Name "spiralMaximumSwipSpeed",Property::FLOAT
  * @note Optional.
  * @note If not supplied, the default is 30.0f.
  */
  SPIRAL_MAXIMUM_SWIPE_SPEED,

  /**
  * @brief The duration of the flick animation in seconds of the SpiralLayout.
  * @details Name "spiralItemFlickAnimationDuration",Property::FLOAT
  * @note Optional.
  * @note This is the time taken to animate each item to its next layout position (e.g. from 1.0 to 2.0) when a flick animation is triggered by a swipe gesture.
  * @note Must be greater than zero; If not supplied, the default is 0.1f.
  */
  SPIRAL_ITEM_FLICK_ANIMATION_DURATION,

  /**
  * @brief The vertical distance for one revolution of the SpiralLayout.
  * @details Name "spiralRevolutionDistance",Property::FLOAT
  * @note Optional.
  * @note If not supplied, the default is 190.0f.
  */
  SPIRAL_REVOLUTION_DISTANCE,

  /**
  * @brief The alignment of the top-item, when at the beginning of the SpiralLayout.
  * @details Name "spiralTopItemAlignment",Property::FLOAT
  * @note Optional.
  * @note When at the beginning of the spiral (with a first-item layout-position of zero).A value of 0 indicates that the top-item is centered in the middle of the layout.
  * A value of -0.5 or 0.5 indicates that the top-item is centred at the top or bottom of the layout respectively.
  * @note If not supplied, the default is (-0.125f).
  */
  SPIRAL_TOP_ITEM_ALIGNMENT,
};

} // namespace Toolkit

} // namespace Dali

}

#endif // DALI_TOOLKIT_DEFAULT_ITEM_LAYOUT_PROPERTY_H

