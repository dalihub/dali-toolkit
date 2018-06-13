#ifndef DALI_TOOLKIT_LAYOUTING_FLEX_LAYOUT_H
#define DALI_TOOLKIT_LAYOUTING_FLEX_LAYOUT_H

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
 */

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-handle.h>
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali-toolkit/devel-api/layouting/layout-group.h>
#include <dali-toolkit/devel-api/layouting/layout-size.h>

namespace Dali
{
namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class FlexLayout;
}

/**
 * This class implements a flex layout.
 *
 * The flex layout implementation is based on open source Facebook Yoga layout engine.
 * For more information about the flex layout API and how to use it please refer to
 * https://yogalayout.com/docs/
 * We implement the subset of the API in the class below.
 */
class DALI_TOOLKIT_API FlexLayout : public LayoutGroup
{
public:

  enum PropertyRange
  {
    CHILD_PROPERTY_START_INDEX = FLEX_LAYOUT_CHILD_PROPERTY_START_INDEX,
    CHILD_PROPERTY_END_INDEX   = FLEX_LAYOUT_CHILD_PROPERTY_END_INDEX
  };

  /**
   * @brief Enumeration for the direction of the main axis in the flex container. This determines
   * the direction that flex items are laid out in the flex container.
   */
  enum class FlexDirection
  {
    COLUMN,                  ///< The flexible items are displayed vertically as a column
    COLUMN_REVERSE,          ///< The flexible items are displayed vertically as a column, but in reverse order
    ROW,                     ///< The flexible items are displayed horizontally as a row
    ROW_REVERSE              ///< The flexible items are displayed horizontally as a row, but in reverse order
  };

  /**
   * @brief Enumeration for the alignment of the flex items when the items do not use all available
   * space on the main-axis.
   */
  enum class Justification
  {
    FLEX_START,              ///< Items are positioned at the beginning of the container
    CENTER,                  ///< Items are positioned at the center of the container
    FLEX_END,                ///< Items are positioned at the end of the container
    SPACE_BETWEEN,           ///< Items are positioned with equal space between the lines
    SPACE_AROUND             ///< Items are positioned with equal space before, between, and after the lines
  };

  /**
   * @brief Enumeration for the alignment of the flex items or lines when the items or lines do not
   * use all the available space on the cross-axis.
   */
  struct Alignment
  {
    enum Type
    {
      AUTO,                  ///< Inherits the same alignment from the parent (only valid for "alignSelf" property)
      FLEX_START,            ///< At the beginning of the container
      CENTER,                ///< At the center of the container
      FLEX_END,              ///< At the end of the container
      STRETCH                ///< Stretch to fit the container
    };
  };

  /**
   * @brief Enumeration for the wrap type of the flex container when there is no enough room for
   * all the items on one flex line.
   */
  enum class WrapType
  {
    NO_WRAP,                 ///< Flex items laid out in single line (shrunk to fit the flex container along the main axis)
    WRAP                     ///< Flex items laid out in multiple lines if needed
  };

  struct ChildProperty
  {
    enum
    {
      FLEX = CHILD_PROPERTY_START_INDEX, ///< name "flex",      The proportion of the free space in the container the flex item will receive. If all items in the container set this property, their sizes will be proportional to the specified flex factor,  type FLOAT
      ALIGN_SELF                         ///< name "alignSelf", The alignment of the flex item along the cross axis, which, if set, overrides the default alignment for all items in the container, type INTEGER
    };
  };

  /**
   * @brief Creates an uninitialized FlexLayout handle.
   *
   * Initialize it using FlexLayout::New().
   * Calling member functions with an uninitialized handle is not allowed.
   */
  FlexLayout();

  /**
   * @brief Creates a FlexLayout object.
   */
  static FlexLayout New();

  /**
   * @brief Downcasts a handle to a FlexLayout handle.
   *
   * If handle points to a FlexLayout, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.

   * @param[in] handle to an object
   * @return Handle to a FlexLayout or an uninitialized handle
   */
  static FlexLayout DownCast( BaseHandle handle );

  /**
   * @brief Copy constructor
   */
  FlexLayout( const FlexLayout& other );

  /**
   * @brief Assigment operator
   */
  FlexLayout& operator=( const FlexLayout& other );

  /**
   * @brief Default destructor.
   *
   * This is non-virtual, since derived Handle types must not contain data or virtual methods
   */
  ~FlexLayout()=default;

  /**
    * @brief Set the flex direction in the layout.
    * The direction of the main-axis which determines the direction that flex items are laid out.
    *
    * @param[in] flexDirection The flex direction.
    */
  void SetFlexDirection( FlexDirection flexDirection );

  /**
    * @brief Get the flex direction in the layout.
    *
    * @return The flex direction.
    */
  FlexDirection GetFlexDirection() const;

  /**
    * @brief Set the justification in the layout.
    *
    * @param[in] flexJustification The flex justification.
    */
  void SetFlexJustification( Justification flexJustification );

  /**
    * @brief Get the flex justification in the layout.
    *
    * @return The flex justification.
    */
  Justification GetFlexJustification() const;

  /**
    * @brief Set the wrap in the layout.
    *
    * @param[in] flexWrap The flex wrap.
    */
  void SetFlexWrap( WrapType flexWrap );

  /**
    * @brief Get the flex wrap in the layout.
    *
    * @return The flex wrap.
    */
  WrapType GetFlexWrap() const;

  /**
    * @brief Set the alignment of the layout content.
    *
    * @param[in] flexAlignment The alignment of the content.
    */
  void SetFlexAlignment( Alignment::Type flexAlignment );

  /**
    * @brief Get the alignment of the layout content.
    *
    * @return The flex content alignment.
    */
  Alignment::Type GetFlexAlignment() const;

  /**
    * @brief Set the alignment of the layout items.
    *
    * @param[in] flexAlignment The alignment of the items.
    */
  void SetFlexItemsAlignment( Alignment::Type flexAlignment );

  /**
    * @brief Get the alignment of the layout items.
    *
    * @return The flex items alignment.
    */
  Alignment::Type GetFlexItemsAlignment() const;

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief This constructor is used by FlexLayout::New() methods.
   *
   * @param[in] actor A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL FlexLayout( Internal::FlexLayout* body );
  /// @endcond
};

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_LAYOUTING_FLEX_LAYOUT_H
