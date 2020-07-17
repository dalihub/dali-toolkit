#ifndef DALI_TOOLKIT_LAYOUTING_FLEX_NODE_H
#define DALI_TOOLKIT_LAYOUTING_FLEX_NODE_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <memory>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
namespace Toolkit
{

namespace Flex
{

class Node;

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
  SPACE_BETWEEN,           ///< Items are positioned with equal space between the items
  SPACE_AROUND             ///< Items are positioned with equal space before, between, and after the items
};

/**
 * @brief Enumeration for the wrap type of the flex container when there is not enough room for
 * all the items on one flex line.
 */
enum class WrapType
{
  NO_WRAP,                 ///< Flex items laid out in single line (shrunk to fit the flex container along the main axis)
  WRAP                     ///< Flex items laid out in multiple lines if needed
};

/**
 * @brief Enumeration for the alignment of the flex items or lines when the items or lines do not
 * use all the available space on the cross-axis.
 */
enum class Alignment
{
    AUTO,                  ///< Currently unsupported, placeholder for inheritance of parent alignment.

    FLEX_START,            ///< At the beginning of the container
    CENTER,                ///< At the center of the container
    FLEX_END,              ///< At the end of the container
    STRETCH                ///< Stretch to fit the container
};

/**
 * @brief Enumeration for the position type of the flex item how it is positioned within its parent.
 */
enum class PositionType
{
    RELATIVE,              ///< Flex items laid out relatively
    ABSOLUTE               ///< Flex items laid out absolutely
};

/**
 * Struct used for MeasureCallback
 */
struct SizeTuple
{
  SizeTuple( float x, float y ) : width( x ), height( y ){}

  float width;
  float height;
};

/**
 * @brief Callback signature for child Actor measure callback.
 * @note Actor, child Actor to measure
 * @note float, available width for child
 * @note int, width measure specifcation mode
 * @note float, available height for child
 * @note int, height measure specification mode
 */
using MeasureCallback = SizeTuple (*)( Dali::Actor, float , int , float , int );

/**
 * This class provides the API for calling into the Flex layout implementation.
 */
class DALI_TOOLKIT_API Node
{
public:
  /**
   * @brief Constructor.
   */
  Node();

  /**
   * @brief Destructor.
   */
  ~Node();

  Node& operator=(Node&&) = default;
  Node(Node&&) = default;
  Node(const Node&) = delete;
  Node& operator=(const Node&) = delete;

  /**
   * @brief Insert child into the FlexLayout at the given index.
   * @param[in] child Actor to insert.
   * @param[in] margin of child Actor.
   * @param[in] measureFunction for the child.
   * @param[in] index to insert at.
   * @return child node pointer
   */
  Node* AddChild( Actor child, Extents margin, MeasureCallback measureFunction, int index );

  /**
   * @brief Remove child from the FlexLayout at the given index.
   * @param[in] child child to be removed.
   */
  void RemoveChild( Actor child );

  /**
   * @brief Return the dimensions of the node.
   * @param[in] width width specification
   * @param[in] widthMode width specification mode
   * @param[in] height height specification
   * @param[in] heightMode height specification mode
   * @return Size tuple representing the width and height of the node
   */
  SizeTuple MeasureNode( float width, int widthMode, float height, int heightMode );

  /**
   * @brief Perform the layout measure calculations.
   * @param[in] availableWidth Amount of space available for layout, width.
   * @param[in] availableHeight Amount of space available for layout, height.
   * @param[in] isRTL Is the direction of the layout right to left.
   */
  void CalculateLayout( float availableWidth, float availableHeight, bool isRTL );

  /**
   * @brief Get the calculated width of the given node.
   * @return the width of the node
   */
  float GetFlexWidth() const;

  /**
   * @brief Get the calculated height of the given node.
   * @return the height of the node
   */
  float GetFlexHeight() const;

  /**
   * @brief Get frame coordinates of the node at the given index.
   * @param[in] index of the child
   * @return Frame structure left x, top y, right z, bottom w
   */
  Vector4 GetNodeFrame(int index ) const;

  /**
   * @brief Set the flex direction in the layout.
   * The direction of the main-axis which determines the direction that flex items are laid out.
   * @param[in] flexDirection The flex direction.
   */
  void SetFlexDirection( FlexDirection flexDirection );

  /**
   * @brief Get the flex direction in the layout.
   * @return The flex direction.
   */
  FlexDirection GetFlexDirection() const;

  /**
   * @brief Set the justification in the layout.
   * @param[in] flexJustification The flex justification.
   */
  void SetFlexJustification( Justification flexJustification );

  /**
   * @brief Get the flex justification in the layout.
   * @return The flex justification.
   */
  Justification GetFlexJustification() const;

  /**
   * @brief Set the wrap in the layout.
   * @param[in] flexWrap The flex wrap.
   */
  void SetFlexWrap(WrapType flexWrap );

  /**
   * @brief Get the flex wrap in the layout.
   * @return The flex wrap.
   */
  WrapType GetFlexWrap() const;

  /**
   * @brief Set the alignment of the layout content.
   * @param[in] flexAlignment The alignment of the content.
   */
  void SetFlexAlignment( Alignment flexAlignment );

  /**
   * @brief Get the alignment of the layout content.
   * @return The flex content alignment.
   */
  Alignment GetFlexAlignment() const;

  /**
   * @brief Set the alignment of the layout items.
   * @param[in] flexAlignment The alignment of the items.
   */
  void SetFlexItemsAlignment( Alignment flexAlignment );

  /**
   * @brief Get the alignment of the layout items.
   * @return The flex items alignment.
   */
  Alignment GetFlexItemsAlignment() const;

  /**
   * @brief Set the alignment self of the layout items.
   * @param[in] flexAlignmentSelf The alignment self of the items.
   */
  void SetFlexAlignmentSelf( Alignment flexAlignmentSelf );

  /**
   * @brief Get the alignment self of the layout items.
   * @return The flex items alignment self.
   */
  Alignment GetFlexAlignmentSelf() const;

  /**
   * @brief Set the position type of the layout items.
   * @param[in] flexPositionType The position type of the items.
   */
  void SetFlexPositionType( PositionType flexPositionType );

  /**
   * @brief Get the position type of the layout items.
   * @return The flex position type.
   */
  PositionType GetFlexPositionType() const;

  /**
   * @brief Set the aspect ratio of the layout items.
   * @param[in] flexAspectRatio The aspect ratio of the items.
   */
  void SetFlexAspectRatio( float flexAspectRatio );

  /**
   * @brief Get the aspect ratio of the layout items.
   * @return The flex aspect ratio.
   */
  float GetFlexAspectRatio() const;

  /**
   * @brief Set the basis of the layout items.
   * @param[in] flexBasis The basis of the items.
   */
  void SetFlexBasis( float flexBasis );

  /**
   * @brief Get the basis of the layout items.
   * @return The flex basis.
   */
  float GetFlexBasis() const;

  /**
   * @brief Set the shrink of the layout items.
   * @param[in] flexShrink The shrink of the items.
   */
  void SetFlexShrink( float flexShrink );

  /**
   * @brief Get the shrink of the layout items.
   * @return The flex shrink.
   */
  float GetFlexShrink() const;

  /**
   * @brief Set the grow of the layout items.
   * @param[in] flexGrow The grow of the items.
   */
  void SetFlexGrow( float flexGrow );

  /**
   * @brief Get the grow of the layout items.
   * @return The flex grow.
   */
  float GetFlexGrow() const;

  /**
   * @brief Set the margin.
   * @param[in] margin The margin value.
   */
  void SetMargin( Extents margin );

  /**
   * @brief Set the padding.
   * @param[in] padding The padding value.
   */
  void SetPadding( Extents padding );

private:
  struct Impl;
  std::unique_ptr< Impl > mImpl;

}; // Node


} // namespace Flex
} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_LAYOUTING_FLEX_NODE_H
