#ifndef __DALI_TOOLKIT_ALBUM_LAYOUT_H__
#define __DALI_TOOLKIT_ALBUM_LAYOUT_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/*
Note:This layout is customized for music player application, so there are some limitations:
1.This layout can only be used with 1280x720 mode (not 720x1280);
2.Need promram in application layer to support it(EX.SetNum).
*/

// EXTERNAL INCLUDES
#include <boost/function.hpp>

// INTERNAL INCLUDES
#include <dali/dali.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-layout.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

class AlbumLayout;

typedef IntrusivePtr<AlbumLayout> AlbumLayoutPtr;

/**
 * An ItemView layout which arranges items in a album.
 */
class AlbumLayout : public ItemLayout
{
public:

  typedef boost::function<Vector3 (const Vector3& layoutSize)> ItemSizeFunction;
  typedef boost::function<float   (const Vector3& layoutSize)> AlbumRadiusFunction;

  /**
   * Create a new album layout
   */
  static AlbumLayoutPtr New();

  /**
   * Virtual destructor.
   */
  virtual ~AlbumLayout();

  /**
   * Set the function used to calculate the item-size, for a given layout-size.
   * @param[in] function The item-size function.
   */
  void SetItemSizeFunction(ItemSizeFunction function);

  /**
   * Get the function used to calculate the item-size
   * @return The item-size function.
   */
  ItemSizeFunction GetItemSizeFunction() const;

  /**
   * Set the function used to calculate the album radius, for a given layout-size.
   * @param[in] function The spiral-radius function.
   */
  void SetAlbumRadiusFunction(AlbumRadiusFunction function);

  /**
   * Get the function used to calculate the album radius.
   * @return The album-radius function.
   */
  AlbumRadiusFunction GetAlbumRadiusFunction() const;

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

  /**
   * Set the num of items.
   * @param[in] num The number of items.
   */
  void SetNumOfItems(int num);

  /**
   * Get the num of items.
   */
  int GetNumOfItems() const;

  /**
   * Set the items num of stack.
   * @param[in] num The number of items on the stack.
   */
  void SetStackNum(int num);

  /**
   * Get the items num of stack.
   */
  int GetStackNum() const;

  /**
   * Set the position of each item.
   * @param[in] positionList The vector which contains the position for each item.
   */
  void SetPosition(std::vector<Vector3> positionList);

  /**
   * Get the position of each item.
   */
  std::vector<Vector3> GetPosition() const;

  /**
   * Set the rotation of each item.
   * @param[in] rotation around X, the vector which contains the rotation for each item.
   */
  void SetRotationX(float rotation);

  /**
   * Set the rotation of each item.
   */
  float GetRotationX() const;

  /**
   * Set the rotation of each item.
   * @param[in] rotationList around Z, the vector which contains the rotation for each item.
   */
  void SetRotationZ(std::vector<float> rotationList);

  /**
   * Get the rotation of each item.
   */
  std::vector<float> GetRotationZ() const;

  /**
   * Set the scale of each item.
   * @param[in] scaleList The vector which contains the scale for each item.
   */
  void SetScale(std::vector<float> scaleList);

  /**
   * Get the scale of each item.
   */
  std::vector<float> GetScale() const;

  /**
   * Set the color of each item.
   * @param[in] colorList The vector which contains the color for each item.
   */
  void SetColor(std::vector<Vector2> colorList);

  /**
   * Get the color of each item.
   */
  std::vector<Vector2> GetColor() const;

  /**
   * Set the position of center(selected) item.
   * @param[in] pos The positon to set.
   */
  void SetCenterPosition(Vector3 pos);


  /**
   * Get the position of center(selected) item.
   */
  Vector3 GetCenterPosition() const;

  /**
   * Set the scale of center(selected) item.
   * @param[in] scale The scale to set.
   */
  void SetCenterScale(float scale);

  /**
   * Get the scale of center(selected) item.
   */
  float GetCenterScale() const;

  /**
    * Set the color of center(selected) item.
    * @param[in] color The color to set.
   */
  void SetCenterColor(Vector2 color);

  /**
   * Get the color of center(selected) item.
   */
  Vector2 GetCenterColor() const;

  /**
    * Set the postion of stack item(right stack and left stack).
    * @param[in] rightPos The position of right stack.
    * @param[in] leftPos The position of left stack,.
   */
  void SetStackPosition(Vector3 rightPos, Vector3 leftPos);

  /**
   * Get the postion of right stack .
   */
  Vector3 GetRightStackPosition() const;

  /**
   * Get the postion of left stack .
   */
  Vector3 GetLeftStackPosition() const;

  /**
    * Set the scale of stack item(right stack and left stack).
    * @param[in] rightScale The scale of right stack.
    * @param[in] leftScale The scale of left stack,.
   */
  void SetStackScale(float rightScale, float leftScale);

  /**
   * Get the scale of right stack.
   */
  float GetRightStackScale() const;

  /**
   * Get the scale of left stack.
   */
  float GetLeftStackScale() const;

  /**
    * Set the color of stack item(right stack and left stack).
    * @param[in] rightColor The color of right stack.
    * @param[in] leftColor The color of left stack.
   */
  void SetStackColor(Vector2 rightColor, Vector2 leftColor);

  /**
    * Get the color of right stack.
    */
  Vector2 GetRightStackColor() const;

  /**
   * Get the color of left stack.
   */
  Vector2 GetLeftStackColor() const;

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
   * Protected constructor; see also AlbumLayout::New()
   */
  AlbumLayout();

private:

  struct Impl;
  Impl* mImpl;
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_ALBUM_LAYOUT_H__
