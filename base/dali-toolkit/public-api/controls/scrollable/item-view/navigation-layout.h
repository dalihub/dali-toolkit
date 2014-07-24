#ifndef __DALI_TOOLKIT_NAVIGATION_LAYOUT_H__
#define __DALI_TOOLKIT_NAVIGATION_LAYOUT_H__

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
#include <dali/dali.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-layout.h>

namespace Dali DALI_IMPORT_API
{
namespace Toolkit
{
class NavigationLayout;

typedef IntrusivePtr<NavigationLayout> NavigationLayoutPtr;

/**
 * An ItemView layout which arranges items in navigation mode.
 */
class NavigationLayout: public ItemLayout
{
public:

  typedef boost::function<Vector3 (const Vector3& layoutSize,
                                   const unsigned int orientation,
                                   const unsigned int numberOfColumns,
                                   const float columnSpacing,
                                   const float sideMargin)>  ResizeFunction;

  /**
   * @deprecated This should not have been added to an ItemLayout
   */
  typedef SignalV2< void (int) > NavigationSignalV2;

  /**
   * Create a new navigation layout
   */
  static NavigationLayoutPtr New();

  /**
   * Virtual destructor.
   */
  virtual ~NavigationLayout();

  /**
   * Get the pan signal
   */
  NavigationSignalV2& PanSignal();

  /**
   * Set the number of columns in the layout.
   * @param[in] columns  The number of columns.
   */
  void SetNumberOfColumns(unsigned int columns);

  /**
   * Get the number of columns in the layout.
   * @return The number of columns.
   */
  unsigned int GetNumberOfColumns() const;

  /**
   * Set the spacing between columns.
   * @param[in] spacing The spacing.
   */
  void SetColumnSpacing(float spacing);

  /**
   * Get the spacing between columns.
   * @return The spacing.
   */
  float GetColumnSpacing() const;

  /**
   * Set the margin in the top of the layout
   * @param[in] margin The layout top margin.
   */
  void SetTopMargin(float margin);

  /**
   * Get the margin in the top of the layout
   * @return The layout top margin.
   */
  float GetTopMargin() const;

  /**
   * Set the margin in the bottom of the layout
   * @param[in] margin The layout bottom margin.
   */
  void SetBottomMargin(float margin);

  /**
   * Get the margin in the bottom of the layout
   * @return The layout bottom margin.
   */
  float GetBottomMargin() const;

  /**
   * Set the margin in the left and right of the layout
   * @param[in] margin The layout side margin.
   */
  void SetSideMargin(float margin);

  /**
   * Get the margin in the left and right of the layout
   * @return The layout side margin.
   */
  float GetSideMargin() const;

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
  virtual float GetScrollSpeedFactor() const;

  /**
   * @copydoc ItemLayout::GetMaximumSwipeSpeed()
   */
  virtual float GetMaximumSwipeSpeed() const;

  /**
   * @copydoc ItemLayout::GetItemFlickAnimationDuration()
   */
  virtual float GetItemFlickAnimationDuration() const;

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
  virtual void GetResizeAnimation(Animation& animation,
                                        Actor actor,
                                        Vector3 size,
                                        float durationSeconds) const;

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
   * Protected constructor; see also GridLayout::New()
   */
  NavigationLayout();

private:

  struct Impl;
  Impl* mImpl;
};

}
}
#endif //__DALI_TOOLKIT_NAVIGATION_LAYOUT_H__
