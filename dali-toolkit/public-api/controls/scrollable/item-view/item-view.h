#ifndef __DALI_TOOLKIT_ITEM_VIEW_H__
#define __DALI_TOOLKIT_ITEM_VIEW_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scrollable/scrollable.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-view-declarations.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class ItemView;
}
/**
 * @addtogroup dali_toolkit_controls_item_view
 * @{
 */

class ItemFactory;
class ItemLayout;
struct ItemRange;

typedef IntrusivePtr<ItemLayout> ItemLayoutPtr;

/**
 * @brief ItemView is a scrollable layout container.
 *
 * Multiple ItemLayouts may be provided, to determine the logical position of each item a layout.
 * Actors are provided from an external ItemFactory, to display the currently visible items.
 *
 * Signals
 * | %Signal Name                    | Method                                     |
 * |---------------------------------|--------------------------------------------|
 * | layoutActivated                 | @ref LayoutActivatedSignal()               |
 * @SINCE_1_0.0
 *
 * Actions
 * | %Action Name  | Attributes              | Description                                     |
 * |---------------|-------------------------|-------------------------------------------------|
 * | stopScrolling | Doesn't have attributes | Stops the scroll animation. See @ref DoAction() |
 * @SINCE_1_1.33
 */

class DALI_IMPORT_API ItemView : public Scrollable
{
public:

  /**
   * @brief The start and end property ranges for this control.
   * @SINCE_1_1.18
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Toolkit::Scrollable::PROPERTY_END_INDEX + 1,                        ///< @SINCE_1_1.18
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000,                                        ///< Reserve property indices, @SINCE_1_1.18

    ANIMATABLE_PROPERTY_START_INDEX = Toolkit::Scrollable::ANIMATABLE_PROPERTY_END_INDEX + 1,
    ANIMATABLE_PROPERTY_END_INDEX   = ANIMATABLE_PROPERTY_START_INDEX + 1000                   ///< Reserve animatable property indices @SINCE_1_0.0
  };

  /**
   * @brief An enumeration of properties belonging to the ScrollView class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    enum
    {
      // Event side properties
      MINIMUM_SWIPE_SPEED = PROPERTY_START_INDEX,        ///< Property, name "minimumSwipeSpeed",        @see SetMinimumSwipeSpeed(),       type float,    @SINCE_1_1.18
      MINIMUM_SWIPE_DISTANCE,                            ///< Property, name "minimumSwipeDistance",     @see SetMinimumSwipeDistance(),    type float,    @SINCE_1_1.18
      WHEEL_SCROLL_DISTANCE_STEP,                        ///< Property, name "wheelScrollDistanceStep",  @see SetWheelScrollDistanceStep(), type float,    @SINCE_1_1.18
      SNAP_TO_ITEM_ENABLED,                              ///< Property, name "snapToItemEnabled",        @see SetAnchoring(),               type bool,     @SINCE_1_1.18
      REFRESH_INTERVAL,                                  ///< Property, name "refreshInterval",          @see SetRefreshInterval(),         type float,    @SINCE_1_1.18

      // Animatable properties
      LAYOUT_POSITION = ANIMATABLE_PROPERTY_START_INDEX, ///< Property, name "layoutPosition",           type float @SINCE_1_0.0
      SCROLL_SPEED,                                      ///< Property, name "scrollSpeed",              type float @SINCE_1_0.0
      OVERSHOOT,                                         ///< Property, name "overshoot",                type float @SINCE_1_0.0
      SCROLL_DIRECTION,                                  ///< Property, name "scrollDirection",          type Vector2 @SINCE_1_0.0
      LAYOUT_ORIENTATION,                                ///< Property, name "layoutOrientation",        type integer @SINCE_1_0.0
      SCROLL_CONTENT_SIZE                                ///< Property, name "scrollContentSize",        type float @SINCE_1_0.0
    };
  };

  // Signals

  typedef Signal< void () > LayoutActivatedSignalType;

public:

  /**
   * @brief Create an uninitialized ItemView; this can be initialized with ItemView::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   * @SINCE_1_0.0
   */
  ItemView();

  /**
   * @brief Copy constructor.
   * @SINCE_1_0.0
   * @param[in] itemView Handle to an object
   */
  ItemView( const ItemView& itemView );

  /**
   * @brief Assignment operator.
   * @SINCE_1_0.0
   * @param[in] itemView Handle to an object
   * @return A reference to this
   */
  ItemView& operator=( const ItemView& itemView );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~ItemView();

  /**
   * @brief Create an initialized ItemView.
   *
   * @SINCE_1_0.0
   * @param[in] factory The factory which provides ItemView with items.
   * @return A handle to a newly allocated Dali resource.
   */
  static ItemView New(ItemFactory& factory);

  /**
   * @brief Downcast a handle to ItemView handle.
   *
   * If handle points to a ItemView the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return A handle to a ItemView or an uninitialized handle
   */
  static ItemView DownCast( BaseHandle handle );

  /**
   * @brief Query the number of layouts.
   *
   * @SINCE_1_0.0
   * @return The number of layouts.
   */
  unsigned int GetLayoutCount() const;

  /**
   * @brief Add a layout.
   *
   * @SINCE_1_0.0
   * @param[in] layout The layout.
   */
  void AddLayout(ItemLayout& layout);

  /**
   * @brief Remove a layout.
   *
   * @SINCE_1_0.0
   * @param[in] layoutIndex The index of one of the ItemView layouts.
   * @pre layoutIndex is less than GetLayoutCount().
   */
  void RemoveLayout(unsigned int layoutIndex);

  /**
   * @brief Retrieve a layout.
   *
   * @SINCE_1_0.0
   * @param[in] layoutIndex The index of the layout to retrieve.
   * @return The layout
   * @pre layoutIndex is less than GetLayoutCount().
   */
  ItemLayoutPtr GetLayout(unsigned int layoutIndex) const;

  /**
   * @brief Retrieve the currently active layout, if any.
   *
   * @SINCE_1_0.0
   * @return The layout, or an uninitialized pointer if no layout is active.
   */
  ItemLayoutPtr GetActiveLayout() const;

  /**
   * @brief Retrieve the current layout-position of an item in the ItemView.
   *
   * @SINCE_1_0.0
   * @param[in] itemId The item identifier.
   * @return The current layout-position.
   */
  float GetCurrentLayoutPosition(ItemId itemId) const;

  /**
   * @brief Activate one of the layouts; this will resize the ItemView
   * & relayout actors within the ItemView.
   *
   * This is done by applying constraints from the new layout, and
   * removing constraints from the previous layout.
   *
   * @SINCE_1_0.0
   * @param[in] layoutIndex The index of one of the ItemView layouts.
   * @param[in] targetSize The target ItemView & layout size.
   * @param[in] durationSeconds The time taken to relayout in seconds (zero for immediate).
   * @pre layoutIndex is less than GetLayoutCount().
   * @pre durationSeconds is greater or equal to zero.
   */
  void ActivateLayout(unsigned int layoutIndex, Vector3 targetSize, float durationSeconds);

  /**
   * @brief Deactivate the current layout, if any.
   *
   * The constraints applied by the layout will be removed.
   * @SINCE_1_0.0
   */
  void DeactivateCurrentLayout();

  /**
   * @brief Set the minimum swipe speed in pixels per second; A pan
   * gesture must exceed this to trigger a swipe.
   *
   * @SINCE_1_0.0
   * @param[in] speed The minimum swipe speed
   */
  void SetMinimumSwipeSpeed(float speed);

  /**
   * @brief Get the minimum swipe speed in pixels per second.
   *
   * @SINCE_1_0.0
   * @return The minimum swipe speed
   */
  float GetMinimumSwipeSpeed() const;

  /**
   * @brief Set the minimum swipe distance in actor coordinates; A pan
   * gesture must exceed this to trigger a swipe.
   *
   * @SINCE_1_0.0
   * @param[in] distance The minimum swipe distance.
   */
  void SetMinimumSwipeDistance(float distance);

  /**
   * @brief Get the minimum swipe distance in actor coordinates.
   *
   * @SINCE_1_0.0
   * @return The minimum swipe distance
   */
  float GetMinimumSwipeDistance() const;

  /**
   * @brief Set the step of scroll distance in actor coordinates for each wheel event received.
   *
   * @SINCE_1_0.0
   * @param[in] step The step of scroll distance(pixel).
   */
  void SetWheelScrollDistanceStep(float step);

  /**
   * @brief Get the step of scroll distance in actor coordinates for each wheel event received.
   *
   * @SINCE_1_0.0
   * @return The step of scroll distance(pixel)
   */
  float GetWheelScrollDistanceStep() const;

  /**
   * @brief Set whether to enable the animation for the layout to
   * scroll to its anchor position after dragging or swiping.
   *
   * The anchor position is the position where all the items in the layout
   * are aligned to their closest rounded layout positions in integer.
   *
   * @SINCE_1_0.0
   * @param[in] enabled Whether the anchor animation is enabled or not.
   */
  void SetAnchoring(bool enabled);

  /**
   * @brief Get whether the anchor animation is enabled or not.
   *
   * @SINCE_1_0.0
   * @return Whether the anchor animation is enabled or not.
   */
  bool GetAnchoring() const;

  /**
   * @brief Set the duration of the anchor animation in seconds.
   *
   * This is the time taken to reach the nearest anchor position after
   * a drag or swipe gesture ends.
   *
   * @SINCE_1_0.0
   * @param[in] durationSeconds The duration of the anchor animation in seconds.
   * @pre durationSeconds must be greater than zero.
   */
  void SetAnchoringDuration(float durationSeconds);

  /**
   * @brief Get the duration of the anchor animation in seconds.
   *
   * @SINCE_1_0.0
   * @return The duration of the anchor animation
   */
  float GetAnchoringDuration() const;

  /**
   * @brief Scroll the current layout to a particular item.
   *
   * @SINCE_1_0.0
   * @param[in] itemId The ID of an item in the layout.
   * @param[in] durationSeconds How long the scrolling takes in seconds.
   * @pre durationSeconds must be zero or greater; zero means the layout should scroll to the particular item instantly.
   * If calling this with zero second of duration immediately after calling ActivateLayout, it might not work unless
   * the duration of relayout animation for ActivateLayout is also set to be zero.
   */
  void ScrollToItem(ItemId itemId, float durationSeconds);

  /**
   * @brief Set the interval between refreshes.
   *
   * When the layout-position of items is changed by this interval,
   * new items are requested from ItemFactory.
   *
   * @SINCE_1_0.0
   * @param[in] intervalLayoutPositions The refresh interval in layout position.
   */
  void SetRefreshInterval(float intervalLayoutPositions);

  /**
   * @brief Get the interval between refreshes in layout position.
   *
   * @SINCE_1_0.0
   * @return  The refresh interval
   */
  float GetRefreshInterval() const;

  /**
   * @brief Do a refresh of the item view.
   * @SINCE_1_0.0
   */
  void Refresh();

  /**
   * @brief Given the Item ID, this returns the accompanying actor.
   *
   * @SINCE_1_0.0
   * @param[in] itemId The Item ID of the actor required.
   * @return The Actor corresponding to the Item ID.
   */
  Actor GetItem(ItemId itemId) const;

  /**
   * @brief Returns the Item ID of the specified actor.
   *
   * @SINCE_1_0.0
   * @param[in] actor The actor whose Item ID is required.
   * @return The Item ID of the item.
   * @pre The actor should be an item of ItemView.
   */
  ItemId GetItemId(Actor actor) const;

  /**
   * @brief Insert an item.
   *
   * A relayout will occur for the existing actors; for example if InsertItem(Item(2, ActorZ), 0) is called,
   * the items with ID 2 or greater will be moved:
   *   Initial actors:     After insert:
   *     ID 1 - ActorA       ID 1 - ActorA
   *     ID 2 - ActorB       ID 2 - ActorZ !
   *     ID 3 - ActorC       ID 3 - ActorB
   *                         ID 4 - ActorC
   * @SINCE_1_0.0
   * @param[in] newItem The item to insert.
   * @param[in] durationSeconds How long the relayout takes in seconds.
   * @pre durationSeconds must be zero or greater; zero means the relayout occurs instantly.
   */
  void InsertItem(Item newItem, float durationSeconds);

  /**
   * @brief Insert a set of items; this is more efficient than calling InsertItem() repeatedly.
   *
   * @SINCE_1_0.0
   * @param[in] newItems The items to insert.
   * @param[in] durationSeconds How long the relayout takes in seconds.
   * @pre durationSeconds must be zero or greater; zero means the relayout occurs instantly.
   */
  void InsertItems(const ItemContainer& newItems, float durationSeconds);

  /**
   * @brief Removes an item with the given ID.
   *
   * A relayout will occur for the remaining actors; for example if RemoveItem(Item(2, ActorZ), 0) is called,
   * the items with ID 3 or greater will be moved:
   *   Initial actors:     After remove:
   *     ID 1 - ActorA       ID 1 - ActorA
   *     ID 2 - ActorB       ID 2 - ActorC (previously ID 3)
   *     ID 3 - ActorC       ID 3 - ActorB (previously ID 4)
   *     ID 4 - ActorD
   * @SINCE_1_0.0
   * @param[in] itemId The Item ID of the item to remove.
   * @param[in] durationSeconds How long the relayout takes in seconds.
   * @pre durationSeconds must be zero or greater; zero means the relayout occurs instantly.
   */
  void RemoveItem(ItemId itemId, float durationSeconds);

  /**
   * @brief Remove a set of items; this is more efficient than calling RemoveItem() repeatedly.
   *
   * @SINCE_1_0.0
   * @param[in] itemIds The IDs of the items to remove.
   * @param[in] durationSeconds How long the relayout takes in seconds.
   * @pre durationSeconds must be zero or greater; zero means the relayout occurs instantly.
   */
  void RemoveItems(const ItemIdContainer& itemIds, float durationSeconds);

  /**
   * @brief Replace an item.
   *
   * A relayout will occur for the replacement item only.
   * @SINCE_1_0.0
   * @param[in] replacementItem The replacement for an existing item.
   * @param[in] durationSeconds How long the relayout takes in seconds.
   * @pre durationSeconds must be zero or greater; zero means the relayout occurs instantly.
   */
  void ReplaceItem(Item replacementItem, float durationSeconds);

  /**
   * @brief Replace a set of items.
   *
   * A relayout will occur for the replacement items only.
   * @SINCE_1_0.0
   * @param[in] replacementItems The replacements for a set of existing items.
   * @param[in] durationSeconds How long the relayout takes in seconds.
   * @pre durationSeconds must be zero or greater; zero means the relayout occurs instantly.
   */
  void ReplaceItems(const ItemContainer& replacementItems, float durationSeconds);

  /**
   * @brief Set the parent origin of the items.
   *
   * A relayout will occur for all the items if the parent origin is different than the current one.
   * @SINCE_1_0.0
   * @param[in] parentOrigin New parent origin position vector
   */
  void SetItemsParentOrigin( const Vector3& parentOrigin );

  /**
   * @brief Get the parent origin of the items.
   *
   * @SINCE_1_0.0
   * @return The current parent origin of the items
   */
  Vector3 GetItemsParentOrigin() const;

  /**
   * @brief Set the anchor point of the items.
   *
   * A relayout will occur for all the items if the anchor point is different than the current one.
   * @SINCE_1_0.0
   * @param[in] anchorPoint New anchor point position vector
   */
  void SetItemsAnchorPoint( const Vector3& anchorPoint );

  /**
   * @brief Get the anchor point of the items.
   *
   * @SINCE_1_0.0
   * @return The current anchor point of the items
   */
  Vector3 GetItemsAnchorPoint() const;

  /**
   * @brief Get the range of items that are currently in ItemView.
   *
   * @SINCE_1_0.0
   * @param[out] range The range of items.
   */
  void GetItemsRange(ItemRange& range);

public: // Signals

  /**
   * @brief Signal emitted when layout activation is finished.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName();
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to.
   * @pre The Object has been initialized.
   */
  ItemView::LayoutActivatedSignalType& LayoutActivatedSignal();

public: // Not intended for application developers

  /**
   * @internal
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @SINCE_1_0.0
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL ItemView(Internal::ItemView& implementation);

  /**
   * @internal
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @SINCE_1_0.0
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL ItemView( Dali::Internal::CustomActor* internal );
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_ITEM_VIEW_H__
