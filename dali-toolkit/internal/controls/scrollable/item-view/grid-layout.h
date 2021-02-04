#ifndef DALI_TOOLKIT_GRID_LAYOUT_H
#define DALI_TOOLKIT_GRID_LAYOUT_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class GridLayout;

typedef IntrusivePtr<GridLayout> GridLayoutPtr; ///< Pointer to a Dali::Toolkit::GridLayout object

/**
 * @brief An ItemView layout which arranges items in a grid.
 */
class GridLayout : public ItemLayout
{
public:
  /**
   * @brief Create a new grid layout.
   */
  static GridLayoutPtr New();

  /**
   * @brief Virtual destructor.
   */
  virtual ~GridLayout();

  /**
   * Apply grid layout Properties.
   * @param[in] properties The properties of the layout.
   */
  void SetGridLayoutProperties(const Property::Map& properties);

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
  float GetScrollSpeedFactor() const override;

  /**
   * @copydoc ItemLayout::GetMaximumSwipeSpeed()
   */
  float GetMaximumSwipeSpeed() const override;

  /**
   * @copydoc ItemLayout::GetItemFlickAnimationDuration()
   */
  float GetItemFlickAnimationDuration() const override;

  /**
   * @copydoc ItemLayout::GetClosestOnScreenLayoutPosition()
   */
  float GetClosestOnScreenLayoutPosition(int itemID, float currentLayoutPosition, const Vector3& layoutSize) override;

  /**
   * @copydoc ItemLayout::GetNextFocusItemID()
   */
  int GetNextFocusItemID(int itemID, int maxItems, Dali::Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled) override;

private:
  /**
   * @copydoc ItemLayout::GetMinimumLayoutPosition()
   */
  float GetMinimumLayoutPosition(unsigned int numberOfItems, Vector3 layoutSize) const override;

  /**
   * @copydoc ItemLayout::GetClosestAnchorPosition()
   */
  float GetClosestAnchorPosition(float layoutPosition) const override;

  /**
   * @copydoc ItemLayout::GetItemScrollToPosition()
   */
  float GetItemScrollToPosition(unsigned int itemId) const override;

  /**
   * @copydoc ItemLayout::GetItemsWithinArea()
   */
  ItemRange GetItemsWithinArea(float firstItemPosition, Vector3 layoutSize) const override;

  /**
   * @copydoc ItemLayout::GetReserveItemCount()
   */
  unsigned int GetReserveItemCount(Vector3 layoutSize) const override;

  /**
   * @copydoc ItemLayout::GetDefaultItemSize()
   */
  void GetDefaultItemSize(unsigned int itemId, const Vector3& layoutSize, Vector3& itemSize) const override;

  /**
   * @copydoc ItemLayout::GetScrollDirection()
   */
  Degree GetScrollDirection() const override;

  /**
   * @copydoc ItemLayout::ApplyConstraints()
   */
  void ApplyConstraints(Actor& actor, const int itemId, const Vector3& layoutSize, const Actor& itemViewActor) override;

  /**
   * @copydoc ItemLayout::GetItemPosition()
   */
  Vector3 GetItemPosition(int itemID, float currentLayoutPosition, const Vector3& layoutSize) const override;

protected:
  /**
   * @brief Protected constructor; see also GridLayout::New().
   */
  GridLayout();

private:
  // Undefined
  GridLayout(const GridLayout& itemLayout);

  // Undefined
  GridLayout& operator=(const GridLayout& rhs);

private:
  struct Impl;
  Impl* mImpl;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_GRID_LAYOUT_H
