#ifndef __DALI_TOOLKIT_ITEM_LAYOUT_H__
#define __DALI_TOOLKIT_ITEM_LAYOUT_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/common/vector-wrapper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/enums.h>
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

class ItemLayout;

typedef IntrusivePtr<ItemLayout> ItemLayoutPtr; ///< Pointer to a Dali::Toolkit::ItemLayout object

typedef std::vector<ItemLayoutPtr>          ItemLayoutContainer; ///< Container of Dali::Toolkit::ItemLayout objects
typedef ItemLayoutContainer::iterator       ItemLayoutIter;      ///< Iterator for Dali::Toolkit::ItemLayoutContainer
typedef ItemLayoutContainer::const_iterator ItemLayoutConstIter; ///< Const Iterator for Dali::Toolkit::ItemLayoutContainer


/**
 * @brief A support class for managing ranges of items.
 */
struct ItemRange
{
  /**
   * @brief Create a range of item identifiers.
   *
   * @param[in] beginItem The first item within the range.
   * @param[in] endItem The past-the-end item.
   */
  ItemRange(unsigned int beginItem, unsigned int endItem)
  : begin(beginItem),
    end(endItem)
  {
  }

  /**
   * @brief Copy Constructor.
   *
   * @param[in] copy ItemRange we should copy from.
   */
  ItemRange(const ItemRange& copy)
  : begin(copy.begin),
    end(copy.end)
  {
  }

  /**
   * @brief Assignment operator.
   *
   * @param[in] range The Range to assign from.
   * @return The updated range.
   */
  ItemRange& operator=(const ItemRange& range)
  {
    begin = range.begin;
    end = range.end;
    return *this;
  }

  /**
   * @brief Test whether an item is within the range.
   *
   * @param[in] itemId The item identifier.
   * @return True if the item is within the range.
   */
  bool Within(unsigned int itemId)
  {
    return itemId >= begin &&
           itemId < end;
  }

  /**
   * @brief Create the intersection of two ranges.
   *
   * @param[in] second The second range.
   * @return The intersection.
   */
  ItemRange Intersection(const ItemRange& second)
  {
    ItemRange intersection(0u, 0u);

    // If the ranges intersect
    if ( (begin < second.end && end > second.begin) ||
         (second.begin < end && second.end > begin) )
    {
      intersection.begin = std::max(begin, second.begin);
      intersection.end   = std::min(end, second.end);
    }

    return intersection;
  }

  unsigned int begin; ///< The start of the range
  unsigned int end;   ///< The end of the range
};

/**
 * @brief An ItemLayout describes the constraints which are imposed on items in the layout.
 *
 *   - Potentially visible items are represented by Actors, created for ItemView by the ItemFactory.
 *   - Constraints are applied after ItemView activates a layout.
 *
 * An ItemLayout also describes the direction of input gestures, used to scroll through the layout.
 * Whilst scrolling, the layout provides a range of items that are within a layout-area (3D bounding volume).
 */
class ItemLayout : public RefObject
{
public:

  /// @brief Function signature of a boolean constraint
  typedef boost::function<bool       (const bool&       current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)> BoolFunction;

  /// @brief Function signature of a Vector3 constraint
  typedef boost::function<Vector3    (const Vector3&    current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)> Vector3Function;

  /// @brief Function signature of a Vector4 constraint
  typedef boost::function<Vector4    (const Vector4&    current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)> Vector4Function;

  /// @brief Function signature of a Quaternion constraint
  typedef boost::function<Quaternion (const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)> QuaternionFunction;

  /**
   * @brief Virtual destructor.
   */
  DALI_IMPORT_API virtual ~ItemLayout();

  /**
   * @brief Set the orientation of the layout.
   *
   * @param[in] orientation The orientation of the layout.
   */
  DALI_IMPORT_API void SetOrientation(ControlOrientation::Type orientation);

  /**
   * @brief Query the orientation of the layout.
   *
   * @return the orientation of the layout.
   */
  DALI_IMPORT_API ControlOrientation::Type GetOrientation() const;

  /**
   * @brief Query the minimum valid layout position; this is a negative value.
   *
   * When scrolling, the first item will move within the range 0 to GetMinimumLayoutPosition().
   * @param[in] numberOfItems The current number of items in the layout.
   * @param[in] layoutSize The size of the layout area.
   * @return The minimum layout position.
   */
  virtual float GetMinimumLayoutPosition(unsigned int numberOfItems, Vector3 layoutSize) const = 0;

  /**
   * @brief Query the closest anchor position for the given layout position.
   *
   * This anchor position is the position where all the items in the layout are aligned to
   * their rounded layout positions in integer.
   * @param[in] layoutPosition The layout position.
   * @return The closest anchor position for the given layout position.
   */
  virtual float GetClosestAnchorPosition(float layoutPosition) const = 0;

  /**
   * @brief Query the layout position for the first item in the layout to move to when the layout
   * needs to scroll to a particular item.
   *
   * @param[in] itemId The ID of an item in the layout.
   * @return The layout position for the first item in the layout to move to.
   */
  virtual float GetItemScrollToPosition(unsigned int itemId) const = 0;

  /**
   * @brief Query the items within a given layout-area.
   *
   * @param[in] firstItemPosition The layout-position of the first item in the layout.
   * @param[in] layoutSize The size of the layout area.
   * @return The ID of the first & last visible item.
   */
  virtual ItemRange GetItemsWithinArea(float firstItemPosition, Vector3 layoutSize) const = 0;

  /**
   * @brief Get the closest layout position to bring an item onto the screen.
   *
   * If the item is already fully on the screen this function will
   * return the current layout position.
   *
   * This function is used by systems such as KeyboardFocusManager to
   * bring the next focusable item into view and all layout
   * implementations should provide their own version of this function
   * to ensure proper functionality of internal toolkit systems.
   *
   * @param[in] itemID id of the item to bring within the viewable screen area
   * @param[in] currentLayoutPosition the current layout position of the item view instance
   * @param[in] layoutSize the current size of the item view instance
   * @return The layout position
   */
  DALI_IMPORT_API virtual float GetClosestOnScreenLayoutPosition(int itemID, float currentLayoutPosition, const Vector3& layoutSize);

  /**
   * @brief Query the number of items that should be reserved, for scrolling purposes.
   *
   * @param[in] layoutSize The size of the layout area.
   * @return The number of extra items. ItemView will populate itself with actors within the layout-area
   * (see GetItemsWithinArea), plus this number of additional items on either-side.
   */
  virtual unsigned int GetReserveItemCount(Vector3 layoutSize) const = 0;

  /**
   * @brief Retrieve the target size of an item in the layout.
   *
   * @note layout-position is not provided as a parameter, since applying size constraints is not recommended.
   * Animating to target-sizes is preferable, since this allows controls to perform layouting without constraints.
   * @param[in] itemId The ID of an item in the layout.
   * @param[in] layoutSize The layout size
   * @param[out] itemSize The target size of an item, or an uninitialized value.
   * @return Whether the item size is available or not
   */
  virtual bool GetItemSize(unsigned int itemId, Vector3 layoutSize, Vector3& itemSize) const = 0;

  /**
   * @brief Retrieve the resize animation in the layout.
   *
   * @note This allows the layout to provide its own resize animation.
   * @param[in] animation The resize animation, not owned by the layout
   * @param[in] actor The actor to animate
   * @param [in] size The target size.
   * @param [in] durationSeconds The duration of the resizing.
   */
  virtual void GetResizeAnimation(Animation& animation, Actor actor, Vector3 size, float durationSeconds) const = 0;

  /**
   * @brief Retrieve the position constraint function of an item in the layout.
   *
   * The constraint will be applied when the item is created or the layout is activated.
   * @param[in] itemId The ID of an item in the layout.
   * @param[out] constraint The position constraint function of an item, or an uninitialized function pointer.
   * @return Whether the position constraint function of an item is available or not
   */
  virtual bool GetPositionConstraint(unsigned int itemId, Vector3Function& constraint) const = 0;

  /**
   * @brief Retrieve the rotation constraint function of an item in the layout.
   *
   * The constraint will be applied when the item is created or the layout is activated.
   * @param[in] itemId The ID of an item in the layout.
   * @param[out] constraint The rotation constraint function of an item, or an uninitialized function pointer.
   * @return Whether the rotation constraint function of an item is available or not
   */
  virtual bool GetRotationConstraint(unsigned int itemId, QuaternionFunction& constraint) const = 0;

  /**
   * @brief Retrieve the scale constraint function of an item in the layout.
   *
   * The constraint will be applied when the item is created or the layout is activated.
   * @param[in] itemId The ID of an item in the layout.
   * @param[out] constraint The scale constraint function of an item, or an uninitialized function pointer.
   * @return Whether the scale constraint function of an item is available or not
   */
  virtual bool GetScaleConstraint(unsigned int itemId, Vector3Function& constraint) const = 0;

  /**
   * @brief Retrieve the color constraint function of an item in the layout.
   *
   * The constraint will be applied when the item is created or the layout is activated.
   * @param[in] itemId The ID of an item in the layout.
   * @param[out] constraint The color constraint function of an item, or an uninitialized function pointer.
   * @return Whether the color constraint function of an item is available or not
   */
  virtual bool GetColorConstraint(unsigned int itemId, Vector4Function& constraint) const = 0;

  /**
   * @brief Retrieve the visibility constraint function of an item in the layout.
   *
   * The constraint will be applied when the item is created or the layout is activated.
   * @param[in] itemId The ID of an item in the layout.
   * @param[out] constraint The visibility constraint function of an item, or an uninitialized function pointer.
   * @return Whether the visibility constraint function of an item is available or not
   */
  virtual bool GetVisibilityConstraint(unsigned int itemId, BoolFunction& constraint) const = 0;

  /**
   * @brief Query the scroll direction of the layout.
   *
   * When an input gesture follows this direction, the layout-position of items will be increased.
   * If the input gesture points in the opposite direction, then the layout-positions will decrease.
   * @return The scroll direction in degrees.
   */
  virtual Degree GetScrollDirection() const = 0;

  /**
   * @brief Query the scroll speed factor of the layout while dragging.
   *
   * This factor is used by the layout to customise its scroll speed while dragging.
   * The factor will be multiplied with the scroll distance of how many pixels in actor coordinate,
   * and the layout position of the actors in ItemView will be moved by this result.
   * For example, when the speed factor is 0.01, if the scroll distance is 100 pixels, the layout
   * position of actors will be moved by 1.
   * Therefore, the bigger the factor is, the faster the scroll speed will be.
   *
   * @return The scroll speed factor of the layout.
   */
  virtual float GetScrollSpeedFactor() const = 0;

  /**
   * @brief Query the maximum swipe speed in pixels per second.
   *
   * Swipe gestures will be clamped when exceeding this speed limit.
   * @return speed The maximum swipe speed.
   */
  virtual float GetMaximumSwipeSpeed() const = 0;

  /**
   * @brief Get the duration of the flick animation in second.
   *
   * This is the time taken to animate each
   * item to its next layout position (e.g. from 1.0 to 2.0) when a flick animation is triggered
   * by a swipe gesture.
   * @return The duration of the flick animation.
   */
  virtual float GetItemFlickAnimationDuration() const = 0;

  /**
   * @brief Gets the id of the next item for KeyboardFocusManager to focus on depending on the inputted item ID.
   *
   * @param[in] itemID The current focused item
   * @param[in] maxItems The maximum number of items in the list
   * @param[in] direction The directional key pressed on the keyboard
   * @param[in] loopEnabled Whether the KeyboardFocusManager is set to wrap around between first and last item
   * @return The next item ID.
   */
  DALI_IMPORT_API virtual int GetNextFocusItemID(int itemID, int maxItems, Dali::Toolkit::Control::KeyboardFocusNavigationDirection direction, bool loopEnabled);

  /**
   * @brief Query the flick speed factor of the layout while swipping.
   *
   * This factor is used by the layout to customise its scroll speed while swiping.
   * The factor will be multiplied with the scroll distance of how many pixels in actor coordinate,
   * and the layout position of the actors in ItemView will be moved by this result.
   * For example, when the speed factor is 0.01, if the scroll distance is 100 pixels, the layout
   * position of actors will be moved by 1.
   * Therefore, the bigger the factor is, the faster the flick speed will be.
   *
   * @return The scroll speed factor of the layout.
   */
  DALI_IMPORT_API virtual float GetFlickSpeedFactor() const;

  /*
   * @brief Applies constraints defined by the layout to an actor.
   *
   * @param[in] actor The actor to constrain.
   * @param[in] itemId The ID of the item represented by the actor.
   * @param[in] durationSeconds The time taken to fully constrain the actors.
   * @param[in] scrollPositionObject The object which provides the layout position property.
   * @param[in] itemViewActor The item view instance which requests the application of constraints.
   */
  DALI_IMPORT_API virtual void ApplyConstraints( Actor& actor, const int itemId, const float durationSeconds, Handle scrollPositionObject, const Actor& itemViewActor );

  /**
   * @brief Gets the position of a given item
   *
   * @param[in] itemID id of the item we want to get its position
   * @param[in] currentLayoutPosition the current layout position of the item view instance
   * @param[in] layoutSize the current size of the item view instance
   * @return The item position (x,y,z)
   */
  DALI_IMPORT_API virtual Vector3 GetItemPosition(int itemID, float currentLayoutPosition, const Vector3& layoutSize) const;

  /**
   * @brief Set the alpha function used when applying constraints
   *
   * @param[in] func The alpha function to use.
   */
  DALI_IMPORT_API void SetAlphaFunction(AlphaFunction func);

  /**
   * @brief Retrieve the alpha function used when applying constraints
   *
   * @return The alpha function.
   */
  DALI_IMPORT_API AlphaFunction GetAlphaFunction() const;

protected:

  /**
   * @brief Create a new ItemLayout; Only derived versions are instantiatable.
   */
  DALI_IMPORT_API ItemLayout();

protected:

  ControlOrientation::Type mOrientation;   ///< the orientation of the layout.
  AlphaFunction            mAlphaFunction; ///<Alpha function to be applied when removing/adding constraints
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_ITEM_LAYOUT_H__
