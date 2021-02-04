#ifndef DALI_TOOLKIT_DEPTH_LAYOUT_H
#define DALI_TOOLKIT_DEPTH_LAYOUT_H

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

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class DepthLayout;

typedef IntrusivePtr<DepthLayout> DepthLayoutPtr;

/**
 * This layout arranges items in a grid, which scrolls along the Z-Axis.
 */
class DepthLayout : public ItemLayout
{
public:
  /**
   * Create a new spiral layout
   */
  static DepthLayoutPtr New();

  /**
   * Virtual destructor.
   */
  virtual ~DepthLayout();

  /**
   * Apply depth layout Properties.
   * @param[in] properties The properties of the layout.
   */
  void SetDepthLayoutProperties(const Property::Map& properties);

  /**
   * Set the number of columns in the layout.
   * @param[in] columns The number of columns.
   */
  void SetNumberOfColumns(unsigned int columns);

  /**
   * Get the number of columns in the layout.
   * @return The number of columns.
   */
  unsigned int GetNumberOfColumns() const;

  /**
   * Set the number of rows in the layout.
   * The default is 20, with 10 behind the viewable area.
   * @param[in] rows The number-of-rows.
   */
  void SetNumberOfRows(unsigned int rows);

  /**
   * Get the number of rows in the layout.
   * @return The number of rows.
   */
  unsigned int GetNumberOfRows() const;

  /**
   * Set the spacing between rows.
   * @param[in] spacing The row spacing.
   */
  void SetRowSpacing(float spacing);

  /**
   * Get the spacing between rows.
   * @return The row spacing.
   */
  float GetRowSpacing() const;

  /**
   * Set the tilt angle of the layout; this is clamped between -45 & 45 degrees.
   * @param[in] angle The tilt angle in degrees.
   */
  void SetTiltAngle(Degree angle);

  /**
   * Get the tilt angle of the layout.
   * @return The tilt angle in degrees.
   */
  Degree GetTiltAngle() const;

  /**
   * Set the tilt angle of the individual items in the layout.
   * @param[in] angle The item tilt angle in degrees.
   */
  void SetItemTiltAngle(Degree angle);

  /**
   * Get the tilt angle of the individual items in the layout.
   * @return The item tilt angle in degrees.
   */
  Degree GetItemTiltAngle() const;

  /**
   * Set the factor used to customise the scroll speed while dragging and swiping the layout.
   * @param[in] scrollSpeed The scroll speed factor.
   */
  void SetScrollSpeedFactor(float scrollSpeed);

  /**
   * Set the maximum swipe speed in pixels per second.
   * @param[in] speed The maximum swipe speed.
   */
  void SetMaximumSwipeSpeed(float speed);

  /**
   * Set the duration of the flick animation in second. This is the time taken to animate each
   * item to its next layout position (e.g. from 1.0 to 2.0) when a flick animation is triggered
   * by a swipe gesture.
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
   * Protected constructor; see also DepthLayout::New()
   */
  DepthLayout();

private:
  // Undefined
  DepthLayout(const DepthLayout& depthLayout);

  // Undefined
  DepthLayout& operator=(const DepthLayout& depthLayout);

private:
  struct Impl;
  Impl* mImpl;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEPTH_LAYOUT_H
