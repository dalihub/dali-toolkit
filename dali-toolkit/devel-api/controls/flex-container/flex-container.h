#ifndef __DALI_TOOLKIT_FLEX_CONTAINER_H__
#define __DALI_TOOLKIT_FLEX_CONTAINER_H__

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
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class FlexContainer;
}

/**
 * @addtogroup dali_toolkit_controls_flex_container
 * @{
 */

/**
 * @brief FlexContainer implements a subset of the flexbox spec (defined by W3C):
 *
 * https://www.w3.org/TR/css3-flexbox/
 *
 * It aims at providing a more efficient way to lay out, align and distribute space among
 * items in the container, even when their size is unknown or dynamic.
 *
 * FlexContainer has the ability to alter the width and height of its children (i.e. flex
 * items) to fill the available space in the best possible way on different screen sizes.
 * FlexContainer can expand items to fill available free space, or shrink them to prevent
 * overflow.
 *
 * Below is an illustration of the various directions and terms as applied to a flex
 * container with the "flex direction" defined as "row".
 *
 * @code
 *     flex container
 *    --------------------------------------------------------------- cross start
 *    | ------------------ --------|--------------------------- |
 *    | |                | |       |                          | |
 *    | |                | |       |                          | |
 *    | |  flex item 1   | |       |    flex item 2           | | main axis
 *    |-|----------------|-|-------|--------------------------|-|------------>
 *    | |                | |       |                          | |
 *    | |                | |       |                          | |
 *    | |                | |       |                          | |
 *    | ------------------ --------|--------------------------- |
 *    -----------------------------|--------------------------------- cross end
 *    |                            |                            |
 *    | main start                 | cross axis                 | main end
 *    |                            |                            |
 *                                 v
 * @endcode
 *
 * @nosubgrouping
 * <h3>Per-child Custom properties for script supporting:</h3>
 *
 * The following custom properties of the actor are checked to decide how to lay out the
 * actor inside the flex container.
 *
 * These properties are registered dynamically to the child and are non-animatable.
 *
 * | %Property Name          | Type        |
 * |-------------------------|-------------|
 * | flex                    | float       |
 * | alignSelf               | integer     |
 * | flexMargin              | Vector4     |
 *
 * The available values for alignSelf are: ALIGN_AUTO, ALIGN_FLEX_START, ALIGN_CENTER, ALIGN_FLEX_END, ALIGN_STRETCH
 *
 * @code
 * "name":"icon",
 * "type":"ImageView",
 * "image":"image.png",
 *   "customProperties": {
 *     "flex":1,                        // property to make the item to receive the specified proportion of the free space in the container. If all items in the container use this pattern, their sizes will be proportional to the specified flex factor.
 *     "alignSelf":"flexStart",         // property to specify how the item will align along the cross axis, if set, this overides the default alignment for all items in the container
 *     "flexMargin":[10, 10, 10, 10]    // property to specify the space around the item, if not set, default value is [0, 0, 0, 0]
 *   }
 * @endcode
 */

class DALI_IMPORT_API FlexContainer : public Control
{
public:

  /**
   * @brief The direction of the main axis in the flex container. This determines
   * the direction that flex items are laid out in the flex container.
   */
  enum FlexDirection
  {
    COLUMN,                  ///< The flexible items are displayed vertically as a column
    COLUMN_REVERSE,          ///< The flexible items are displayed vertically as a column, but in reverse order
    ROW,                     ///< The flexible items are displayed horizontally as a row
    ROW_REVERSE              ///< The flexible items are displayed horizontally as a row, but in reverse order
  };

  /**
   * @brief The primary direction in which content is ordered in the flex container
   * and on which sides the “start” and “end” are.
   */
  enum ContentDirection
  {
    INHERIT,                 ///< Inherits the same direction from the parent
    LTR,                     ///< From left to right
    RTL                      ///< From right to left
  };

  /**
   * @brief Alignment of the flex items when the items do not use all available
   * space on the main-axis.
   */
  enum Justification
  {
    JUSTIFY_FLEX_START,      ///< Items are positioned at the beginning of the container
    JUSTIFY_CENTER,          ///< Items are positioned at the center of the container
    JUSTIFY_FLEX_END,        ///< Items are positioned at the end of the container
    JUSTIFY_SPACE_BETWEEN,   ///< Items are positioned with equal space between the lines
    JUSTIFY_SPACE_AROUND     ///< Items are positioned with equal space before, between, and after the lines
  };

  /**
   * @brief Alignment of the flex items or lines when the items or lines do not
   * use all available space on the cross-axis.
   */
  enum Alignment
  {
    ALIGN_AUTO,              ///< Inherits the same alignment from the parent (only valid for "alignSelf" property)
    ALIGN_FLEX_START,        ///< At the beginning of the container
    ALIGN_CENTER,            ///< At the center of the container
    ALIGN_FLEX_END,          ///< At the end of the container
    ALIGN_STRETCH            ///< Stretch to fit the container
  };

  /**
   * @brief The wrap type of the flex container when there is no enough room for
   * all the items on one flex line.
   */
  enum WrapType
  {
    NO_WRAP,                 ///< Flex items laid out in single line (shrunk to fit the flex container along the main axis)
    WRAP                     ///< Flex items laid out in multiple lines if needed
  };

public:

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the FlexContainer class.
   */
  struct Property
  {
    enum
    {
      CONTENT_DIRECTION = PROPERTY_START_INDEX, ///< name "contentDirection",   The primary direction in which content is ordered,                                                 @see FlexContainer::ContentDirection,  type INTEGER
      FLEX_DIRECTION,                           ///< name "flexDirection",      The direction of the main-axis which determines the direction that flex items are laid out,        @see FlexContainer::FlexDirection,     type INTEGER
      FLEX_WRAP,                                ///< name "flexWrap",           Whether the flex items should wrap or not if there is no enough room for them on one flex line,    @see FlexContainer::WrapType,          type INTEGER
      JUSTIFY_CONTENT,                          ///< name "justifyContent",     The alignment of flex items when the items do not use all available space on the main-axis,        @see FlexContainer::Justification,     type INTEGER
      ALIGN_ITEMS,                              ///< name "alignItems",         The alignment of flex items when the items do not use all available space on the cross-axis,       @see FlexContainer::Alignment,         type INTEGER
      ALIGN_CONTENT                             ///< name "alignContent",       Similar to "alignItems", but it aligns flex lines, so only works when there are multiple lines,    @see FlexContainer::Alignment,         type INTEGER
    };
  };

  /**
   * Create a FlexContainer handle; this can be initialised with FlexContainer::New()
   * Calling member functions with an uninitialised handle is not allowed.
   */
  FlexContainer();

  /**
   * Copy constructor. Creates another handle that points to the same real object
   * @param handle to copy from
   */
  FlexContainer( const FlexContainer& handle );

  /**
   * Assignment operator. Changes this handle to point to another real object
   */
  FlexContainer& operator=( const FlexContainer& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~FlexContainer();

  /**
   * Create the FlexContainer control.
   * @return A handle to the FlexContainer control.
   */
  static FlexContainer New();

  /**
   * Downcast an Object handle to FlexContainer. If handle points to a FlexContainer the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a FlexContainer or an uninitialized handle
   */
  static FlexContainer DownCast( BaseHandle handle );


public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The Control implementation.
   */
  DALI_INTERNAL FlexContainer( Internal::FlexContainer& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL FlexContainer( Dali::Internal::CustomActor* internal );
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_FLEX_CONTAINER_H__
