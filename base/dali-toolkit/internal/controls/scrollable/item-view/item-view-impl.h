#ifndef __DALI_TOOLKIT_INTERNAL_ITEM_VIEW_H__
#define __DALI_TOOLKIT_INTERNAL_ITEM_VIEW_H__

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
#include <dali/dali.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-view.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-layout.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-connector.h>
#include <dali-toolkit/internal/controls/scrollable/scrollable-impl.h>
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class ItemView;

typedef IntrusivePtr<ItemView> ItemViewPtr;

/**
 * ItemView is a scrollable layout container.
 * Multiple ItemLayouts may be provided, to determine the logical position of each item a layout.
 * Actor-ID pairs are provided from a shared ItemFactory, to display the currently visible items.
 */
class ItemView : public Scrollable
{
public:

  /**
   * Create a new ItemView.
   * @param[in] factory The factory which provides ItemView with items.
   * @return A public handle to the newly allocated ItemView.
   */
  static Dali::Toolkit::ItemView New(ItemFactory& factory);

  /**
   * @copydoc Toolkit::ItemView::GetScrollConnector
   */
  Dali::Toolkit::ScrollConnector GetScrollConnector() const;

  /**
   * @copydoc Toolkit::ItemView::GetLayoutCount
   */
  unsigned int GetLayoutCount() const;

  /**
   * @copydoc Toolkit::ItemView::AddLayout
   */
  void AddLayout(ItemLayout& layout);

  /**
   * @copydoc Toolkit::ItemView::RemoveLayout
   */
  void RemoveLayout(unsigned int layoutIndex);

  /**
   * @copydoc Toolkit::ItemView::GetLayout
   */
  ItemLayoutPtr GetLayout(unsigned int layoutIndex) const;

  /**
   * @copydoc Toolkit::ItemView::GetActiveLayout
   */
  ItemLayoutPtr GetActiveLayout() const;

  /**
   * @copydoc Toolkit::ItemView::GetCurrentLayoutPosition
   */
  float GetCurrentLayoutPosition(unsigned int itemId) const;

  /**
   * @copydoc Toolkit::ItemView::ActivateLayout
   */
  void ActivateLayout(unsigned int layoutIndex, const Vector3& targetSize, float durationSeconds);

  /**
   * @copydoc Toolkit::ItemView::SetDefaultAlphaFunction
   */
  void SetDefaultAlphaFunction(AlphaFunction func);

  /**
   * @copydoc Toolkit::ItemView::GetDefaultAlphaFunction
   */
  AlphaFunction GetDefaultAlphaFunction() const;

  /**
   * @copydoc Toolkit::ItemView::DeactivateCurrentLayout
   */
  void DeactivateCurrentLayout();

  /**
   * @copydoc Toolkit::ItemView::SetMinimumSwipeSpeed
   */
  void SetMinimumSwipeSpeed(float speed);

  /**
   * @copydoc Toolkit::ItemView::GetMinimumSwipeSpeed
   */
  float GetMinimumSwipeSpeed() const;

  /**
   * @copydoc Toolkit::ItemView::SetMinimumSwipeDistance
   */
  void SetMinimumSwipeDistance(float distance);

  /**
   * @copydoc Toolkit::ItemView::GetMinimumSwipeDistance
   */
  float GetMinimumSwipeDistance() const;

  /**
   * @copydoc Toolkit::ItemView::SetMouseWheelScrollDistanceStep
   */
  void SetMouseWheelScrollDistanceStep(float step);

  /**
   * @copydoc Toolkit::ItemView::GetMouseWheelScrollDistanceStep
   */
  float GetMouseWheelScrollDistanceStep() const;

  /**
   * @copydoc Toolkit::ItemView::SetAnchoring
   */
  void SetAnchoring(bool enabled);

  /**
   * @copydoc Toolkit::ItemView::GetAnchoring
   */
  bool GetAnchoring() const;

  /**
   * @copydoc Toolkit::ItemView::SetAnchoringDuration
   */
  void SetAnchoringDuration(float durationSeconds);

  /**
   * @copydoc Toolkit::ItemView::GetAnchoringDuration
   */
  float GetAnchoringDuration() const;

  /**
   * @copydoc Toolkit::ItemView::ScrollToItem
   */
  void ScrollToItem(unsigned int itemId, float durationSeconds);

  /**
   * @copydoc Toolkit::ItemView::SetRefreshInterval
   */
  void SetRefreshInterval(float intervalLayoutPositions);

  /**
   * @copydoc Toolkit::ItemView::GetRefreshInterval
   */
  float GetRefreshInterval() const;

  /**
   * @copydoc Toolkit::ItemView::GetItem
   */
  Actor GetItem(unsigned int itemId) const;

  /**
   * @copydoc Toolkit::ItemView::GetItemId
   */
  unsigned int GetItemId(Actor actor) const;

  /**
   * @copydoc Toolkit::ItemView::InsertItem
   */
  void InsertItem(Item newItem, float durationSeconds);

  /**
   * @copydoc Toolkit::ItemView::InsertItem
   */
  void InsertItems(const ItemContainer& newItems, float durationSeconds);

  /**
   * @copydoc Toolkit::ItemView::RemoveItem
   */
  void RemoveItem(ItemId itemId, float durationSeconds);

  /**
   * @copydoc Toolkit::ItemView::InsertItem
   */
  void RemoveItems(const ItemIdContainer& itemIds, float durationSeconds);

  /**
   * @copydoc Toolkit::ItemView::InsertItem
   */
  void ReplaceItem(Item replacementItem, float durationSeconds);

  /**
   * @copydoc Toolkit::ItemView::InsertItem
   */
  void ReplaceItems(const ItemContainer& replacementItems, float durationSeconds);

  /**
   * @copydoc Toolkit::Scrollable::GetDomainSize
   */
  Vector3 GetDomainSize() const;

  /**
   * @copydoc Toolkit::Scrollable::GetCurrentScrollPosition
   */
  Vector3 GetCurrentScrollPosition() const;

  /**
   * @copydoc Toolkit::Scrollable::AddOverlay()
   */
  void AddOverlay(Actor actor);

  /**
   * @copydoc Toolkit::Scrollable::RemoveOverlay()
   */
  void RemoveOverlay(Actor actor);

  /**
   * @copydoc Toolkit::Scrollable::ScrollTo(const Vector3& position, float duration)
   */
  void ScrollTo(const Vector3& position, float duration);

  /**
   * @copydoc Toolkit::Internal::Scrollable::SetOvershootEffectColor
   */
  void SetOvershootEffectColor( const Vector4& color );

  /**
   * @brief Set whether to enable automatic refresh or not. When refresh is disabled,
   * ItemView will not automatically refresh the cache in the given interval when the
   * layout position is changed. This is useful in some cases, for example, automatic
   * refresh is not needed during fast scrolling, otherwise it will cache unneeded
   * items when the layout position changes quickly.
   *
   * @param[in] enabled True to enable automatic refresh or false to disable it.
   */
  void SetRefreshEnabled(bool enabled);

  /**
   * @brief Helper to perform the refresh.
   *
   * @param[in] currentLayoutPosition The current layout position.
   * @param[in] cacheExtra Whether to cache extra items during refresh.
   */
  void DoRefresh(float currentLayoutPosition, bool cacheExtra);


  /**
   * @copydoc Toolkit::ItemView::SetItemsParentOrigin
   */
  void SetItemsParentOrigin( const Vector3& parentOrigin );

  /**
   * @copydoc Toolkit::ItemView::GetItemsParentOrigin
   */
  Vector3 GetItemsParentOrigin() const;

  /**
   * @copydoc Toolkit::ItemView::SetItemsAnchorPoint
   */
  void SetItemsAnchorPoint( const Vector3& anchorPoint );

  /**
   * @copydoc Toolkit::ItemView::GetItemsAnchorPoint
   */
  Vector3 GetItemsAnchorPoint() const;

  /**
   * @copydoc Toolkit::ItemView::GetItemsRange
   */
  void GetItemsRange(ItemRange& range);

private:

  /**
   * Remove an Actor if found in the ItemPool.
   * @param[in] itemId The item to remove.
   * @return True if an actor was removed.
   */
  bool RemoveActor( unsigned int itemId );

  /**
   * Remove any Actors outside a given range.
   * @param[in] @param[in] range The range of required items.
   */
  void RemoveActorsOutsideRange( ItemRange range );

  /**
   * Add a range of Actors, if they are not already in the ItemPool.
   * @param[in] range The range of Item IDs to associate with the new actors.
   * @param[in] durationSeconds The time taken to fully constrain the newly added actor.
   */
  void AddActorsWithinRange( ItemRange range, float durationSeconds );

  /**
   * Add a new Actor, if not already in the ItemPool.
   * @param[in] item The ID for the new item.
   * @param[in] durationSeconds The time taken to fully constrain the new actor.
   */
  void AddNewActor( ItemId item, float durationSeconds );

  /**
   * Apply the constraints etc. that are required for ItemView children.
   * @param[in] item The item to setup.
   * @param[in] durationSeconds The time taken to fully constrain the actor.
   */
  void SetupActor( Item item, float durationSeconds );

  /**
   * Remove the Actor from the ItemPool and notify the ItemFactory the actor has been released by ItemView.
   * @param[in] item The ID for the item to be released.
   * @param[in] actor The actor to be removed from ItemView.
   */
  void ReleaseActor( ItemId item, Actor actor );

private: // From CustomActorImpl

  /**
   * From CustomActorImpl; called after a child has been added to the owning actor.
   * @param[in] child The child which has been added.
   */
  virtual void OnChildAdd(Actor& child);

  /**
   * From CustomActorImpl; called after a touch-signal is received by the owning actor.
   * @param[in] event The touch event.
   * @return True if the event should be consumed.
   */
  virtual bool OnTouchEvent(const TouchEvent& event);

  /**
   * From CustomActorImpl; called after a mouse-wheel-event is received by the owning actor.
   * @param[in] event The mouse wheel event.
   * @return True if the event should be consumed.
   */
  virtual bool OnMouseWheelEvent(const MouseWheelEvent& event);

private: // From Control

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::Control::OnAccessibilityPan()
   */
  virtual bool OnAccessibilityPan(PanGesture gesture);

  /**
   * @copydoc Toolkit::Control::GetNextKeyboardFocusableActor()
   */
  virtual Actor GetNextKeyboardFocusableActor(Actor actor, Toolkit::Control::KeyboardFocusNavigationDirection direction, bool loopEnabled);

  /**
   * @copydoc Toolkit::Control::OnKeyboardFocusChangeCommitted()
   */
  virtual void OnKeyboardFocusChangeCommitted(Actor commitedFocusableActor);

protected:

  /**
   * Construct a new ItemView.
   * @param[in] factory The factory which provides ItemView with items.
   */
  ItemView(ItemFactory& factory);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ItemView();

private:

  // Undefined
  ItemView(const ItemView&);

  // Undefined
  ItemView& operator=(const ItemView& rhs);

  /**
   * Helper to apply constraints to an actor.
   * @param[in] actor The actor to constrain.
   * @param[in] layout The active layout.
   * @param[in] itemId The ID of the item represented by the actor.
   * @param[in] durationSeconds The time taken to fully constrain the actors.
   */
  void ApplyConstraints(Actor& actor, ItemLayout& layout, unsigned int itemId, float durationSeconds);

  /**
   * Helper to re-apply all the constraints after items have been inserted, removed etc.
   * @param[in] durationSeconds The time taken to fully constrain the actors.
   */
  void ReapplyAllConstraints( float durationSeconds );

  /**
   * Helper to remove items outside a given range.
   * @param[in] range The range of required items.
   */
  void RemoveItems(ItemRange range);

  /**
   * Helper to add a range of items, if not already in the ItemPool.
   * @param[in] layout The layout used to position the new items.
   * @param[in] layoutSize The current size of the layout.
   * @param[in] range The range of required items.
   */
  void AddItems(ItemLayout& layout, const Vector3& layoutSize, ItemRange range);

  /**
   * Helper to find the range of items to populate with.
   * @param[in] layout The current layout.
   * @param[in] range The range of items.
   * @param[in] reserveExtra True if reserve items should be included.
   */
  ItemRange GetItemRange(ItemLayout& layout, const Vector3& layoutSize, float layoutPosition, bool reserveExtra);

  // Input Handling

  /**
   * Helper to handle pressed (Down) events.
   * @param[in] x The X coordinate of the touch event.
   * @param[in] y The Y coordinate of the touch event.
   * @param[in] timeMs The time-stamp of the touch event.
   */
  void OnPressed(float x, float y, unsigned long timeMs);

  /**
   * Helper to clamp the first-item position when dragging/swiping.
   * @param[in] targetPosition The target position of the drag etc.
   * @param[in] targetSize The target ItemView & layout size.
   * @param[in] layout The current layout.
   * @return The clamped first-item position.
   */
  float ClampFirstItemPosition(float targetPosition, const Vector3& targetSize, ItemLayout& layout);

  /**
   * Called upon pan gesture event.
   *
   * @param[in] gesture The gesture event.
   */
  void OnPan(PanGesture pan);

  /**
   * Helper to handle anchoring animations.
   * @return The animation, or an uninitialized handle if not necessary.
   */
  Animation DoAnchoring();

  /**
   * Callback from scroll animations
   * @param[in] animation The scroll-animation which has finished.
   */
  void OnScrollFinished(Animation& animation);

  /**
   * Callback from layout activation scroll animations
   * @param[in] animation The scroll-animation which has finished.
   */
  void OnLayoutActivationScrollFinished(Animation& animation);

  /**
   * Called by animation system when overshoot has finished animating to maximum (either -1.0f or 1.0f)
   *
   * @param[in] animation the animation that has finished
   */
  void OnOvershootOnFinished(Animation& animation);

  /**
   * This is called after a timeout when no new mouse wheel event is received for a certain period of time.
   * @return will return false; one-shot timer.
   */
  bool OnMouseWheelEventFinished();

  /**
   * Stops and removes animation if exists.
   * @param[in,out] animation The animation handle to be removed.
   */
  void RemoveAnimation(Animation& animation);

  /**
   * @copydoc Toolkit::Internal::Scrollable::SetOvershootEnabled
   */
  virtual void SetOvershootEnabled( bool enable );

  /**
   * Helper to calculate the scroll overshoot according to the pan gesture displacement.
   * @return The scroll overshoot.
   */
  float CalculateScrollOvershoot();

  /**
   * Helper to calculate the scroll overshoot according to the pan gesture displacement.
   *
   * @param[in] overshootAmount amount to animate to
   * @param[in] animateBack whether to animate back to zero immediately after
   * @return The scroll overshoot.
   */
  void AnimateScrollOvershoot(float overshootAmount, bool animateBack = false);

  /**
   * Gets the scroll position in pixels according to the logical layout position.
   * @param[in] layoutSize The current size of the layout.
   */
  float GetScrollPosition(float layoutPosition, const Vector3& layoutSize) const;

  /**
   * Calculates the minimum and maximum positions for each axis to scroll to.
   * @param[in] layoutSize The current size of the layout.
   */
  void CalculateDomainSize(const Vector3& layoutSize);

  /**
   * Calculates whether we want to allow current item view to scroll.
   * @param[in] layoutSize The current size of the layout.
   * @return    true if itemview is scrollable
   */
  bool IsLayoutScrollable(const Vector3& layoutSize);

  /**
   * Callback when the current layout position of ItemView changes in both positive
   * and negative directions by the specified amount. Refresh the ItemView to create
   * newly visible items.
   * @param[in] source the property notification that triggered this callback
   */
  void OnRefreshNotification(PropertyNotification& source);

private:

  ItemFactory& mItemFactory;

  typedef std::map<unsigned int, Actor> ItemPool;
  typedef ItemPool::iterator            ItemPoolIter;
  typedef ItemPool::const_iterator      ConstItemPoolIter;

  ItemPool mItemPool;

  ItemLayoutContainer mLayouts;
  ItemLayout* mActiveLayout;
  Vector3 mActiveLayoutTargetSize;

  AlphaFunction mDefaultAlphaFunction;

  Animation mResizeAnimation;
  Animation mScrollAnimation;
  Animation mScrollOvershootAnimation;
  bool      mAnimatingOvershootOn;          ///< whether we are currently animating overshoot to 1.0f/-1.0f (on) or to 0.0f (off)
  bool      mAnimateOvershootOff;         ///< whether we are currently animating overshoot to 1.0f/-1.0f (on) or to 0.0f (off)

  bool mAnchoringEnabled;
  float mAnchoringDuration;

  float mRefreshIntervalLayoutPositions;  ///< Refresh item view when the layout position changes by this interval in both positive and negative directions.
  PropertyNotification mRefreshNotification; // stores the property notification used for item view refresh
  bool mRefreshOrderHint; ///< True if scrolling towards the last item

  // Input handling

  float mMinimumSwipeSpeed;
  float mMinimumSwipeDistance;
  float mMouseWheelScrollDistanceStep; ///< The step of scroll distance in actor coordinates for each mouse wheel event received.

  float mScrollDistance;
  float mScrollSpeed;
  Vector2 mTotalPanDisplacement;

  float mScrollOvershoot;
  bool mIsFlicking;

  Timer mMouseWheelEventFinishedTimer; ///< The timer to determine whether there is no mouse wheel event received for a certain period of time.

  Dali::Gesture::State mGestureState;

  Actor mOvershootOverlay;           ///< The overlay actor for overshoot effect

  Dali::Toolkit::ScrollConnector mScrollConnector; ///< Connects ItemView with scrollable components e.g. scroll bars
  Constrainable   mScrollPositionObject;     ///< From mScrollConnector

  bool mAddingItems;

  Property::Index mPropertyPosition; ///< The physical position of the first item within the layout
  Property::Index mPropertyMinimumLayoutPosition; ///< The minimum valid layout position in the layout.
  Property::Index mPropertyScrollSpeed; ///< The current scroll speed of item view

  bool mRefreshEnabled; ///< Whether to refresh the cache automatically

  Vector3 mItemsParentOrigin;
  Vector3 mItemsAnchorPoint;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::ItemView& GetImpl(Toolkit::ItemView& itemView)
{
  DALI_ASSERT_ALWAYS(itemView);

  Dali::RefObject& handle = itemView.GetImplementation();

  return static_cast<Toolkit::Internal::ItemView&>(handle);
}

inline const Toolkit::Internal::ItemView& GetImpl(const Toolkit::ItemView& itemView)
{
  DALI_ASSERT_ALWAYS(itemView);

  const Dali::RefObject& handle = itemView.GetImplementation();

  return static_cast<const Toolkit::Internal::ItemView&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_ITEM_VIEW_H__
