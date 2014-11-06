#ifndef __DALI_TOOLKIT_GRID_LAYOUT_H__
#define __DALI_TOOLKIT_GRID_LAYOUT_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/public-api/controls/scrollable/item-view/item-layout.h>

namespace Dali
{

namespace Toolkit
{

class GridLayout;

typedef IntrusivePtr<GridLayout> GridLayoutPtr; ///< Pointer to a Dali::Toolkit::GridLayout object

/**
 * @brief An ItemView layout which arranges items in a grid.
 */
class DALI_IMPORT_API GridLayout : public ItemLayout
{
public:

  /**
   * @brief Function signature for a method to calculate the item size.
   *
   * @see SetItemSizeFunction()
   */
  typedef boost::function<Vector3 (unsigned int numberOfColumns, float layoutWidth, float sideMargin, float columnSpacing)> ItemSizeFunction;

  /**
   * @brief Create a new grid layout.
   */
  static GridLayoutPtr New();

  /**
   * @brief Virtual destructor.
   */
  virtual ~GridLayout();

  /**
   * @brief Set the number of columns in the layout.
   *
   * @param[in] columns The number of columns.
   */
  void SetNumberOfColumns(unsigned int columns);

  /**
   * @brief Get the number of columns in the layout.
   *
   * @return The number of columns.
   */
  unsigned int GetNumberOfColumns() const;

  /**
   * @brief Set the spacing between rows.
   *
   * @param[in] spacing The row spacing.
   */
  void SetRowSpacing(float spacing);

  /**
   * @brief Get the spacing between rows.
   *
   * @return The row spacing.
   */
  float GetRowSpacing() const;

  /**
   * @brief Set the spacing between columns.
   *
   * @param[in] spacing The row spacing.
   */
  void SetColumnSpacing(float spacing);

  /**
   * @brief Get the spacing between columns.
   *
   * @return The row spacing.
   */
  float GetColumnSpacing() const;

  /**
   * @brief Set the margin in the top of the layout.
   *
   * @param[in] margin The layout top margin.
   */
  void SetTopMargin(float margin);

  /**
   * @brief Get the margin in the top of the layout.
   *
   * @return The layout top margin.
   */
  float GetTopMargin() const;

  /**
   * @brief Set the margin in the bottom of the layout.
   *
   * @param[in] margin The layout bottom margin.
   */
  void SetBottomMargin(float margin);

  /**
   * @brief Get the margin in the bottom of the layout.
   *
   * @return The layout bottom margin.
   */
  float GetBottomMargin() const;

  /**
   * @brief Set the margin in the left and right of the layout.
   *
   * @param[in] margin The layout side margin.
   */
  void SetSideMargin(float margin);

  /**
   * @brief Get the margin in the left and right of the layout.
   *
   * @return The layout side margin.
   */
  float GetSideMargin() const;

  /**
   * @brief Set the gap of items in the Z axis in different columns.
   *
   * @param[in] gap The gap of items.
   */
  void SetZGap(float gap);

  /**
   * @brief Get the gap of items in the Z axis in different columns.
   *
   * @return The gap of items.
   */
  float GetZGap() const;

  /**
   * @brief Set the function used to calculate the item-size, for a given layout-size.
   *
   * @param[in] function The item-size function.
   */
  void SetItemSizeFunction(ItemSizeFunction function);

  /**
   * @brief Get the function used to calculate the item-size.
   *
   * @return The item-size function.
   */
  ItemSizeFunction GetItemSizeFunction() const;

  /**
   * @brief Set the factor used to customise the scroll speed while dragging and swiping the layout.
   *
   * @param[in] scrollSpeed The scroll speed factor.
   */
  void SetScrollSpeedFactor(float scrollSpeed);

  /**
   * @brief Set the maximum swipe speed in pixels per second.
   *
   * @param[in] speed The maximum swipe speed.
   */
  void SetMaximumSwipeSpeed(float speed);

  /**
   * @brief Set the duration of the flick animation in seconds.
   *
   * This is the time taken to animate each item to its next layout
   * position (e.g. from 1.0 to 2.0) when a flick animation is
   * triggered by a swipe gesture.
   *
   * @pre durationSeconds must be greater than zero.
   * @param[in] durationSeconds The duration of flick animation in seconds.
   */
  void SetItemFlickAnimationDuration(float durationSeconds);

  /**
   * @copydoc ItemLayout::GetScrollSpeedFactor()
   */
  virtual float GetScrollSpeedFactor() const;

  /**
   * @copydoc ItemLayout::GetMaximumSwipeSpeed()
   */
  virtual float GetMaximumSwipeSpeed() const;

  /**
   * @copydoc ItemLayout::GetItemFlickAnimationDuration()
   */
  virtual float GetItemFlickAnimationDuration() const;

  /**
   * @copydoc ItemLayout::GetClosestOnScreenLayoutPosition()
   */
  virtual float GetClosestOnScreenLayoutPosition(int itemID, float currentLayoutPosition, const Vector3& layoutSize);

  /**
   * @copydoc ItemLayout::GetNextFocusItemID()
   */
  virtual int GetNextFocusItemID(int itemID, int maxItems, Dali::Toolkit::Control::KeyboardFocusNavigationDirection direction, bool loopEnabled);

private:

  /**
   * @copydoc ItemLayout::GetMinimumLayoutPosition()
   */
  virtual float GetMinimumLayoutPosition(unsigned int numberOfItems, Vector3 layoutSize) const;

  /**
   * @copydoc ItemLayout::GetClosestAnchorPosition()
   */
  virtual float GetClosestAnchorPosition(float layoutPosition) const;

  /**
   * @copydoc ItemLayout::GetItemScrollToPosition()
   */
  virtual float GetItemScrollToPosition(unsigned int itemId) const;

  /**
   * @copydoc ItemLayout::GetItemsWithinArea()
   */
  virtual ItemRange GetItemsWithinArea(float firstItemPosition, Vector3 layoutSize) const;

  /**
   * @copydoc ItemLayout::GetReserveItemCount()
   */
  virtual unsigned int GetReserveItemCount(Vector3 layoutSize) const;

  /**
   * @copydoc ItemLayout::GetItemSize()
   */
  virtual bool GetItemSize(unsigned int itemId, Vector3 layoutSize, Vector3& itemSize) const;

  /**
   * @copydoc ItemLayout::GetResizeAnimation()
   */
  virtual void GetResizeAnimation(Animation& animation, Actor actor, Vector3 size, float durationSeconds) const;

  /**
   * @copydoc ItemLayout::GetPositionConstraint()
   */
  virtual bool GetPositionConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const;

  /**
   * @copydoc ItemLayout::GetRotationConstraint()
   */
  virtual bool GetRotationConstraint(unsigned int itemId, ItemLayout::QuaternionFunction& constraint) const;

  /**
   * @copydoc ItemLayout::GetScaleConstraint()
   */
  virtual bool GetScaleConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const;

  /**
   * @copydoc ItemLayout::GetColorConstraint()
   */
  virtual bool GetColorConstraint(unsigned int itemId, ItemLayout::Vector4Function& constraint) const;

  /**
   * @copydoc ItemLayout::GetVisibilityConstraint()
   */
  virtual bool GetVisibilityConstraint(unsigned int itemId, ItemLayout::BoolFunction& constraint) const;

  /**
   * @copydoc ItemLayout::GetScrollDirection()
   */
  virtual Degree GetScrollDirection() const;

protected:

  /**
   * @brief Protected constructor; see also GridLayout::New().
   */
  GridLayout();

private:

  struct Impl;
  Impl* mImpl;
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_GRID_LAYOUT_H__
