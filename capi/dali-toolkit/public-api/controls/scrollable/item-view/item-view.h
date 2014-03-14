#ifndef __DALI_TOOLKIT_ITEM_VIEW_H__
#define __DALI_TOOLKIT_ITEM_VIEW_H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// EXTERNAL INCLUDES
#include <dali/dali.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scrollable/scrollable.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-view-declarations.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class ItemView;
}

class ScrollConnector;
class ItemFactory;
class ItemLayout;

typedef IntrusivePtr<ItemLayout> ItemLayoutPtr;

/**
 * ItemView is a scrollable layout container.
 * Multiple ItemLayouts may be provided, to determine the logical position of each item a layout.
 * Actors are provided from an external ItemFactory, to display the currently visible items.
 */
class ItemView : public Scrollable
{
public:

  /**
   * Create an uninitialized ItemView; this can be initialized with ItemView::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  ItemView();

  /**
   * Copy constructor.
   */
  ItemView( const ItemView& itemView );

  /**
   * Assignment operator.
   */
  ItemView& operator=( const ItemView& itemView );

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~ItemView();

  /**
   * Create an initialized ItemView.
   * @param[in] factory The factory which provides ItemView with items.
   * @return A handle to a newly allocated Dali resource.
   */
  static ItemView New(ItemFactory& factory);

  /**
   * Downcast an Object handle to ItemView. If handle points to a ItemView the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ItemView or an uninitialized handle
   */
  static ItemView DownCast( BaseHandle handle );

  /**
   * Retrieve a scroll-connector; this can be used to connect scroll components e.g. scroll bars.
   * @return The connector.
   */
  ScrollConnector GetScrollConnector() const;

  /**
   * Query the number of layouts.
   * @return The number of layouts.
   */
  unsigned int GetLayoutCount() const;

  /**
   * Add a layout.
   * @param[in] layout The layout.
   */
  void AddLayout(ItemLayout& layout);

  /**
   * Remove a layout.
   * @pre layoutIndex is less than GetLayoutCount().
   * @param[in] layoutIndex The index of one of the ItemView layouts.
   */
  void RemoveLayout(unsigned int layoutIndex);

  /**
   * Retrieve a layout.
   * @pre layoutIndex is less than GetLayoutCount().
   * @return The layout
   */
  ItemLayoutPtr GetLayout(unsigned int layoutIndex) const;

  /**
   * Retrieve the currently active layout, if any.
   * @return The layout, or an uninitialized pointer if no layout is active.
   */
  ItemLayoutPtr GetActiveLayout() const;

  /**
   * Retrieve the current layout-position of an item in the ItemView.
   * @param[in] itemId The item identifier.
   * @return The current layout-position.
   */
  float GetCurrentLayoutPosition(ItemId itemId) const;

  /**
   * Activate one of the layouts; this will resize the ItemView & relayout actors within the ItemView.
   * This is done by applying constraints from the new layout, and removing constraints from the previous layout.
   * @pre layoutIndex is less than GetLayoutCount().
   * @pre durationSeconds is greater or equal to zero.
   * @param[in] layoutIndex The index of one of the ItemView layouts.
   * @param[in] targetSize The target ItemView & layout size.
   * @param[in] durationSeconds The time taken to relayout in seconds (zero for immediate).
   */
  void ActivateLayout(unsigned int layoutIndex, Vector3 targetSize, float durationSeconds);

  /**
   * Deactivate the current layout, if any.
   * The constraints applied by the layout will be removed.
   */
  void DeactivateCurrentLayout();

  /**
   * Set the minimum swipe speed in pixels per second; A pan gesture must exceed this to trigger a swipe.
   * @param[in] speed The minimum swipe speed
   */
  void SetMinimumSwipeSpeed(float speed);

  /**
   * Get the minimum swipe speed in pixels per second.
   * @return The minimum swipe speed
   */
  float GetMinimumSwipeSpeed() const;

  /**
   * Set the minimum swipe distance in actor coordinates; A pan gesture must exceed this to trigger a swipe.
   * @param[in] distance The minimum swipe distance.
   */
  void SetMinimumSwipeDistance(float distance);

  /**
   * Get the minimum swipe distance in actor coordinates.
   * @return The minimum swipe distance
   */
  float GetMinimumSwipeDistance() const;

  /**
   * Set the step of scroll distance in actor coordinates for each mouse wheel event received.
   * @param[in] step The step of scroll distance(pixel).
   */
  void SetMouseWheelScrollDistanceStep(float step);

  /**
   * Get the step of scroll distance in actor coordinates for each mouse wheel event received.
   * @return The step of scroll distance(pixel)
   */
  float GetMouseWheelScrollDistanceStep() const;

  /**
   * Set whether to enable the animation for the layout to scroll to its anchor position after
   * dragging or swiping. The anchor position is the position where all the items in the layout
   * are aligned to their closest rounded layout positions in integer.
   * @param[in] enabled Whether the anchor animation is enabled or not.
   */
  void SetAnchoring(bool enabled);

  /**
   * Get whether the anchor animation is enabled or not
   * @return Whether the anchor animation is enabled or not.
   */
  bool GetAnchoring() const;

  /**
   * Set the duration of the anchor animation in seconds. This is the time taken to reach the nearest
   * anchor position after a drag or swipe gesture ends.
   * @pre durationSeconds must be greater than zero.
   * @param[in] durationSeconds The duration of the anchor animation in seconds.
   */
  void SetAnchoringDuration(float durationSeconds);

  /**
   * Get the duration of the anchor animation in seconds
   * @return The duration of the anchor animation
   */
  float GetAnchoringDuration() const;

  /**
   * Scroll the current layout to a particular item.
   * @pre durationSeconds must be zero or greater; zero means the layout should scroll to the particular item instantly.
   * If calling this with zero second of duration immediately after calling ActivateLayout, it might not work unless
   * the duration of relayout animation for ActivateLayout is also set to be zero.
   * @param[in] itemId The ID of an item in the layout.
   * @param[in] durationSeconds How long the scrolling takes in seconds.
   */
  void ScrollToItem(ItemId itemId, float durationSeconds);

  /**
   * Set the interval between refreshes, during which new items are requested from ItemFactory.
   * @param[in] intervalMilliseconds The refresh interval in milliseconds.
   */
  void SetRefreshInterval(unsigned int intervalMilliseconds);

  /**
   * Get the interval between refreshes in milliseconds.
   * @return  The refresh interval
   */
  unsigned int GetRefreshInterval() const;

  /**
   * Given the Item ID, this returns the accompanying actor.
   * @param[in] itemId The Item ID of the actor required.
   * @return The Actor corresponding to the Item ID.
   */
  Actor GetItem(ItemId itemId) const;

  /**
   * Returns the Item ID of the specified actor.
   * @param[in] actor The actor whose Item ID is required.
   * @return The Item ID of the item.
   * @pre The actor should be an item of ItemView.
   */
  ItemId GetItemId(Actor actor) const;

  /**
   * Insert an item.
   * A relayout will occur for the existing actors; for example if InsertItem(Item(2, ActorZ), 0) is called,
   * the items with ID 2 or greater will be moved:
   *   Initial actors:     After insert:
   *     ID 1 - ActorA       ID 1 - ActorA
   *     ID 2 - ActorB       ID 2 - ActorZ !
   *     ID 3 - ActorC       ID 3 - ActorB
   *                         ID 4 - ActorC
   * @pre durationSeconds must be zero or greater; zero means the relayout occurs instantly.
   * @param[in] newItem The item to insert.
   * @param[in] durationSeconds How long the relayout takes in seconds.
   */
  void InsertItem(Item newItem, float durationSeconds);

  /**
   * Insert a set of items; this is more efficient than calling InsertItem() repeatedly.
   * @pre durationSeconds must be zero or greater; zero means the relayout occurs instantly.
   * @param[in] newItems The items to insert.
   * @param[in] durationSeconds How long the relayout takes in seconds.
   */
  void InsertItems(const ItemContainer& newItems, float durationSeconds);

  /**
   * Removes an item with the given ID.
   * A relayout will occur for the remaining actors; for example if RemoveItem(Item(2, ActorZ), 0) is called,
   * the items with ID 3 or greater will be moved:
   *   Initial actors:     After remove:
   *     ID 1 - ActorA       ID 1 - ActorA
   *     ID 2 - ActorB       ID 2 - ActorC (previously ID 3)
   *     ID 3 - ActorC       ID 3 - ActorB (previously ID 4)
   *     ID 4 - ActorD
   * @pre durationSeconds must be zero or greater; zero means the relayout occurs instantly.
   * @param[in] itemId The Item ID of the item to remove.
   * @param[in] durationSeconds How long the relayout takes in seconds.
   */
  void RemoveItem(ItemId itemId, float durationSeconds);

  /**
   * Remove a set of items; this is more efficient than calling RemoveItem() repeatedly.
   * @pre durationSeconds must be zero or greater; zero means the relayout occurs instantly.
   * @param[in] itemIds The IDs of the items to remove.
   * @param[in] durationSeconds How long the relayout takes in seconds.
   */
  void RemoveItems(const ItemIdContainer& itemIds, float durationSeconds);

  /**
   * Replace an item.
   * A relayout will occur for the replacement item only.
   * @pre durationSeconds must be zero or greater; zero means the relayout occurs instantly.
   * @param[in] replacementItem The replacement for an existing item.
   * @param[in] durationSeconds How long the relayout takes in seconds.
   */
  void ReplaceItem(Item replacementItem, float durationSeconds);

  /**
   * Replace a set of items.
   * A relayout will occur for the replacement items only.
   * @pre durationSeconds must be zero or greater; zero means the relayout occurs instantly.
   * @param[in] replacementItems The replacements for a set of existing items.
   * @param[in] durationSeconds How long the relayout takes in seconds.
   */
  void ReplaceItems(const ItemContainer& replacementItems, float durationSeconds);

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  ItemView(Internal::ItemView& implementation);

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  ItemView( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_ITEM_VIEW_H__
