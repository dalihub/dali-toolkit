#ifndef DALI_TOOLKIT_SPIRAL_LAYOUT_H
#define DALI_TOOLKIT_SPIRAL_LAYOUT_H

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
class SpiralLayout;

typedef IntrusivePtr<SpiralLayout> SpiralLayoutPtr;

/**
 * An ItemView layout which arranges items in a spiral.
 */
class SpiralLayout : public ItemLayout
{
public:
  /**
   * Create a new spiral layout
   */
  static SpiralLayoutPtr New();

  /**
   * Virtual destructor.
   */
  virtual ~SpiralLayout();

  /**
   * Apply spiral layout Properties.
   * @param[in] properties The properties of the layout.
   */
  void SetSpiralLayoutProperties(const Property::Map& properties);

  /**
   * Set spacing angle between items.
   * @param[in] itemSpacing The angle in radians.
   */
  void SetItemSpacing(Radian itemSpacing);

  /**
   * Get spacing angle between items.
   * @return The angle in radians.
   */
  Radian GetItemSpacing() const;

  /**
   * Set the vertical distance for one revolution of the spiral.
   * @param[in] distance The revolution distance.
   */
  void SetRevolutionDistance(float distance);

  /**
   * Get the vertical distance for one revolution of the spiral.
   * @return The revolution distance.
   */
  float GetRevolutionDistance() const;

  /**
   * Set the alignment of the top-item, when at the beginning of the spiral (with a first-item layout-position of zero).
   * A value of 0 indicates that the top-item is centered in the middle of the layout. A value of -0.5 or 0.5 indicates
   * that the top-item is centred at the top or bottom of the layout respectively.
   * @param[in] alignment The top-item alignment.
   */
  void SetTopItemAlignment(float alignment);

  /**
   * Get the alignment of the top-item, when at the beginning of the spiral
   * @return The top-item alignment.
   */
  float GetTopItemAlignment() const;

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
   * Protected constructor; see also SpiralLayout::New()
   */
  SpiralLayout();

private:
  // Undefined
  SpiralLayout(const SpiralLayout& spiralLayout);

  // Undefined
  SpiralLayout& operator=(const SpiralLayout& spiralLayout);

private:
  struct Impl;
  Impl* mImpl;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_SPIRAL_LAYOUT_H
