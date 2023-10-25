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

// CLASS HEADER
#include <dali-toolkit/internal/controls/scrollable/item-view/item-view-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/events/wheel-event.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <algorithm>
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/scroll-bar/scroll-bar.h>
#include <dali-toolkit/internal/controls/scrollable/bouncing-effect-actor.h>
#include <dali-toolkit/internal/controls/scrollable/item-view/depth-layout.h>
#include <dali-toolkit/internal/controls/scrollable/item-view/grid-layout.h>
#include <dali-toolkit/internal/controls/scrollable/item-view/spiral-layout.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/default-item-layout-property.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/default-item-layout.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-factory.h>

using std::string;
using namespace Dali;

namespace // Unnamed namespace
{
const float DEFAULT_MINIMUM_SWIPE_SPEED                   = 1.0f;
const float DEFAULT_MINIMUM_SWIPE_DISTANCE                = 3.0f;
const float DEFAULT_WHEEL_SCROLL_DISTANCE_STEP_PROPORTION = 0.1f;

const float DEFAULT_MINIMUM_SWIPE_DURATION = 0.45f;
const float DEFAULT_MAXIMUM_SWIPE_DURATION = 2.6f;

const float DEFAULT_REFRESH_INTERVAL_LAYOUT_POSITIONS = 20.0f; // 1 updates per 20 items
const int   WHEEL_EVENT_FINISHED_TIME_OUT             = 500;   // 0.5 second

const float DEFAULT_ANCHORING_DURATION = 1.0f; // 1 second

const float MILLISECONDS_PER_SECONDS = 1000.0f;

const float   OVERSHOOT_BOUNCE_ACTOR_RESIZE_THRESHOLD = 180.0f;
const Vector4 OVERSHOOT_OVERLAY_NINE_PATCH_BORDER(0.0f, 0.0f, 1.0f, 12.0f);
const float   DEFAULT_KEYBOARD_FOCUS_SCROLL_DURATION = 0.2f;

const unsigned int OVERSHOOT_SIZE_CONSTRAINT_TAG(42);

/**
 * Local helper to convert pan distance (in actor coordinates) to the layout-specific scrolling direction
 */
float CalculateScrollDistance(Vector2 panDistance, Toolkit::ItemLayout& layout)
{
  Radian scrollDirection(layout.GetScrollDirection());

  float cosTheta = cosf(scrollDirection);
  float sinTheta = sinf(scrollDirection);

  return panDistance.x * sinTheta + panDistance.y * cosTheta;
}

// Overshoot overlay constraints
struct OvershootOverlaySizeConstraint
{
  OvershootOverlaySizeConstraint(float height)
  : mOvershootHeight(height)
  {
  }

  void operator()(Vector3& current, const PropertyInputContainer& inputs)
  {
    const Vector2&                           parentScrollDirection = inputs[0]->GetVector2();
    const Toolkit::ControlOrientation::Type& layoutOrientation     = static_cast<Toolkit::ControlOrientation::Type>(inputs[1]->GetInteger());
    const Vector3&                           parentSize            = inputs[2]->GetVector3();

    if(Toolkit::IsVertical(layoutOrientation))
    {
      current.width = fabsf(parentScrollDirection.y) > Math::MACHINE_EPSILON_1 ? parentSize.x : parentSize.y;
    }
    else
    {
      current.width = fabsf(parentScrollDirection.x) > Math::MACHINE_EPSILON_1 ? parentSize.y : parentSize.x;
    }

    current.height = (current.width > OVERSHOOT_BOUNCE_ACTOR_RESIZE_THRESHOLD) ? mOvershootHeight : mOvershootHeight * 0.5f;
  }

  float mOvershootHeight;
};

void OvershootOverlayRotationConstraint(Quaternion& current, const PropertyInputContainer& inputs)
{
  const Vector2&                           parentScrollDirection = inputs[0]->GetVector2();
  const Toolkit::ControlOrientation::Type& layoutOrientation     = static_cast<Toolkit::ControlOrientation::Type>(inputs[1]->GetInteger());
  const float                              parentOvershoot       = inputs[2]->GetFloat();

  float multiplier = 0;
  if(Toolkit::IsVertical(layoutOrientation))
  {
    if(fabsf(parentScrollDirection.y) <= Math::MACHINE_EPSILON_1)
    {
      if((layoutOrientation == Toolkit::ControlOrientation::Up && parentOvershoot < Math::MACHINE_EPSILON_0) || (layoutOrientation == Toolkit::ControlOrientation::Down && parentOvershoot > Math::MACHINE_EPSILON_0))
      {
        multiplier = 0.5f;
      }
      else
      {
        multiplier = 1.5f;
      }
    }
    else if((parentOvershoot > Math::MACHINE_EPSILON_0 && parentScrollDirection.y > Math::MACHINE_EPSILON_0) || (parentOvershoot < Math::MACHINE_EPSILON_0 && parentScrollDirection.y < Math::MACHINE_EPSILON_0))
    {
      multiplier = 0.0f;
    }
    else
    {
      multiplier = 1.0f;
    }
  }
  else
  {
    if(fabsf(parentScrollDirection.x) <= Math::MACHINE_EPSILON_1)
    {
      if((layoutOrientation == Toolkit::ControlOrientation::Left && parentOvershoot > Math::MACHINE_EPSILON_0) || (layoutOrientation == Toolkit::ControlOrientation::Right && parentOvershoot < Math::MACHINE_EPSILON_0))
      {
        multiplier = 1.0f;
      }
      else
      {
        multiplier = 0.0f;
      }
    }
    else if((parentOvershoot > Math::MACHINE_EPSILON_0 && parentScrollDirection.x > Math::MACHINE_EPSILON_0) || (parentOvershoot < Math::MACHINE_EPSILON_0 && parentScrollDirection.x < Math::MACHINE_EPSILON_0))
    {
      multiplier = 1.5f;
    }
    else
    {
      multiplier = 0.5f;
    }
  }

  current = Quaternion(Radian(multiplier * Math::PI), Vector3::ZAXIS);
}

void OvershootOverlayPositionConstraint(Vector3& current, const PropertyInputContainer& inputs)
{
  const Vector3&                           parentSize            = inputs[0]->GetVector3();
  const Vector2&                           parentScrollDirection = inputs[1]->GetVector2();
  const Toolkit::ControlOrientation::Type& layoutOrientation     = static_cast<Toolkit::ControlOrientation::Type>(inputs[2]->GetInteger());
  const float                              parentOvershoot       = inputs[3]->GetFloat();

  Vector3 relativeOffset;

  if(Toolkit::IsVertical(layoutOrientation))
  {
    if(fabsf(parentScrollDirection.y) <= Math::MACHINE_EPSILON_1)
    {
      if((layoutOrientation == Toolkit::ControlOrientation::Up && parentOvershoot < Math::MACHINE_EPSILON_0) || (layoutOrientation == Toolkit::ControlOrientation::Down && parentOvershoot > Math::MACHINE_EPSILON_0))
      {
        relativeOffset = Vector3(1.0f, 0.0f, 0.0f);
      }
      else
      {
        relativeOffset = Vector3(0.0f, 1.0f, 0.0f);
      }
    }
    else if((parentOvershoot > Math::MACHINE_EPSILON_0 && parentScrollDirection.y > Math::MACHINE_EPSILON_0) || (parentOvershoot < Math::MACHINE_EPSILON_0 && parentScrollDirection.y < Math::MACHINE_EPSILON_0))
    {
      relativeOffset = Vector3(0.0f, 0.0f, 0.0f);
    }
    else
    {
      relativeOffset = Vector3(1.0f, 1.0f, 0.0f);
    }
  }
  else
  {
    if(fabsf(parentScrollDirection.x) <= Math::MACHINE_EPSILON_1)
    {
      if((layoutOrientation == Toolkit::ControlOrientation::Left && parentOvershoot < Math::MACHINE_EPSILON_0) || (layoutOrientation == Toolkit::ControlOrientation::Right && parentOvershoot > Math::MACHINE_EPSILON_0))
      {
        relativeOffset = Vector3(0.0f, 0.0f, 0.0f);
      }
      else
      {
        relativeOffset = Vector3(1.0f, 1.0f, 0.0f);
      }
    }
    else if((parentOvershoot > Math::MACHINE_EPSILON_0 && parentScrollDirection.x > Math::MACHINE_EPSILON_0) || (parentOvershoot < Math::MACHINE_EPSILON_0 && parentScrollDirection.x < Math::MACHINE_EPSILON_0))
    {
      relativeOffset = Vector3(0.0f, 1.0f, 0.0f);
    }
    else
    {
      relativeOffset = Vector3(1.0f, 0.0f, 0.0f);
    }
  }

  current = relativeOffset * parentSize;
}

void OvershootOverlayVisibilityConstraint(bool& current, const PropertyInputContainer& inputs)
{
  current = inputs[0]->GetBoolean();
}

} // unnamed namespace

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace // unnamed namespace
{
//Type registration
// clang-format off
DALI_TYPE_REGISTRATION_BEGIN(Toolkit::ItemView, Toolkit::Scrollable, nullptr)

DALI_PROPERTY_REGISTRATION(Toolkit, ItemView, "minimumSwipeSpeed",       FLOAT,   MINIMUM_SWIPE_SPEED       )
DALI_PROPERTY_REGISTRATION(Toolkit, ItemView, "minimumSwipeDistance",    FLOAT,   MINIMUM_SWIPE_DISTANCE    )
DALI_PROPERTY_REGISTRATION(Toolkit, ItemView, "wheelScrollDistanceStep", FLOAT,   WHEEL_SCROLL_DISTANCE_STEP)
DALI_PROPERTY_REGISTRATION(Toolkit, ItemView, "snapToItemEnabled",       BOOLEAN, SNAP_TO_ITEM_ENABLED      )
DALI_PROPERTY_REGISTRATION(Toolkit, ItemView, "refreshInterval",         FLOAT,   REFRESH_INTERVAL          )
DALI_PROPERTY_REGISTRATION(Toolkit, ItemView, "layout",                  ARRAY,   LAYOUT                    )


DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ItemView, "layoutPosition",    FLOAT,   LAYOUT_POSITION    )
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ItemView, "scrollSpeed",       FLOAT,   SCROLL_SPEED       )
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ItemView, "overshoot",         FLOAT,   OVERSHOOT          )
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ItemView, "scrollDirection",   VECTOR2, SCROLL_DIRECTION   )
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ItemView, "layoutOrientation", INTEGER, LAYOUT_ORIENTATION )
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ItemView, "scrollContentSize", FLOAT,   SCROLL_CONTENT_SIZE)

DALI_SIGNAL_REGISTRATION(Toolkit, ItemView, "layoutActivated", LAYOUT_ACTIVATED_SIGNAL)

DALI_ACTION_REGISTRATION(Toolkit, ItemView, "stopScrolling", ACTION_STOP_SCROLLING)

DALI_ACTION_REGISTRATION(Toolkit, ItemView, "enableRefresh",  ACTION_ENABLE_REFRESH )
DALI_ACTION_REGISTRATION(Toolkit, ItemView, "disableRefresh", ACTION_DISABLE_REFRESH)

DALI_TYPE_REGISTRATION_END()
// clang-format on

const ItemIter FindItemById(ItemContainer& items, ItemId id)
{
  for(ItemIter iter = items.begin(); items.end() != iter; ++iter)
  {
    if(iter->first == id)
    {
      return iter;
    }
  }

  return items.end();
}

void InsertToItemContainer(ItemContainer& items, Item item)
{
  if(items.end() == FindItemById(items, item.first))
  {
    ItemIter iterToInsert = std::lower_bound(items.begin(), items.end(), item);
    items.insert(iterToInsert, item);
  }
}

/**
  * Helper to apply size constraint to mOvershootOverlay
  * @param[in] overshootOverlay The overshootOverlay actor
  * @param[in] The required height
  */
void ApplyOvershootSizeConstraint(Actor overshootOverlay, float height)
{
  Constraint constraint = Constraint::New<Vector3>(overshootOverlay, Actor::Property::SIZE, OvershootOverlaySizeConstraint(height));
  constraint.AddSource(ParentSource(Dali::Toolkit::ItemView::Property::SCROLL_DIRECTION));
  constraint.AddSource(ParentSource(Dali::Toolkit::ItemView::Property::LAYOUT_ORIENTATION));
  constraint.AddSource(ParentSource(Dali::Actor::Property::SIZE));
  constraint.SetTag(OVERSHOOT_SIZE_CONSTRAINT_TAG);
  constraint.Apply();
}

} // unnamed namespace

Dali::Toolkit::ItemView ItemView::New(ItemFactory& factory)
{
  // Create the implementation
  ItemViewPtr itemView(new ItemView(factory));

  // Pass ownership to CustomActor via derived handle
  Dali::Toolkit::ItemView handle(*itemView);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  itemView->Initialize();

  return handle;
}

ItemView::ItemView(ItemFactory& factory)
: Scrollable(ControlBehaviour(DISABLE_SIZE_NEGOTIATION | DISABLE_STYLE_CHANGE_SIGNALS | REQUIRES_KEYBOARD_NAVIGATION_SUPPORT)),
  mItemFactory(factory),
  mItemsParentOrigin(ParentOrigin::CENTER),
  mItemsAnchorPoint(AnchorPoint::CENTER),
  mTotalPanDisplacement(Vector2::ZERO),
  mActiveLayout(NULL),
  mAnchoringDuration(DEFAULT_ANCHORING_DURATION),
  mRefreshIntervalLayoutPositions(0.0f),
  mMinimumSwipeSpeed(DEFAULT_MINIMUM_SWIPE_SPEED),
  mMinimumSwipeDistance(DEFAULT_MINIMUM_SWIPE_DISTANCE),
  mWheelScrollDistanceStep(0.0f),
  mScrollDistance(0.0f),
  mScrollSpeed(0.0f),
  mScrollOvershoot(0.0f),
  mGestureState(GestureState::CLEAR),
  mAnimatingOvershootOn(false),
  mAnimateOvershootOff(false),
  mAnchoringEnabled(false),
  mRefreshOrderHint(true /*Refresh item 0 first*/),
  mIsFlicking(false),
  mAddingItems(false),
  mRefreshEnabled(true),
  mRefreshNotificationEnabled(true),
  mInAnimation(false)
{
}

void ItemView::OnInitialize()
{
  Scrollable::OnInitialize();

  Actor self = Self();

  Vector2 stageSize        = Stage::GetCurrent().GetSize();
  mWheelScrollDistanceStep = stageSize.y * DEFAULT_WHEEL_SCROLL_DISTANCE_STEP_PROPORTION;

  self.TouchedSignal().Connect(this, &ItemView::OnTouch);
  EnableGestureDetection(GestureType::Value(GestureType::PAN));

  mWheelEventFinishedTimer = Timer::New(WHEEL_EVENT_FINISHED_TIME_OUT);
  mWheelEventFinishedTimer.TickSignal().Connect(this, &ItemView::OnWheelEventFinished);

  SetRefreshInterval(DEFAULT_REFRESH_INTERVAL_LAYOUT_POSITIONS);

  // Connect wheel event
  self.WheelEventSignal().Connect(this, &ItemView::OnWheelEvent);

  self.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::SCROLL_PANE);
}

DevelControl::ControlAccessible* ItemView::CreateAccessibleObject()
{
  return new ItemViewAccessible(Self());
}

ItemView::~ItemView()
{
}

unsigned int ItemView::GetLayoutCount() const
{
  return mLayouts.size();
}

void ItemView::AddLayout(ItemLayout& layout)
{
  mLayouts.push_back(ItemLayoutPtr(&layout));
}

void ItemView::RemoveLayout(unsigned int layoutIndex)
{
  DALI_ASSERT_ALWAYS(layoutIndex < mLayouts.size());

  if(mActiveLayout == mLayouts[layoutIndex].Get())
  {
    mActiveLayout = NULL;
  }

  mLayouts.erase(mLayouts.begin() + layoutIndex);
}

ItemLayoutPtr ItemView::GetLayout(unsigned int layoutIndex) const
{
  return mLayouts[layoutIndex];
}

ItemLayoutPtr ItemView::GetActiveLayout() const
{
  return ItemLayoutPtr(mActiveLayout);
}

float ItemView::GetCurrentLayoutPosition(unsigned int itemId) const
{
  return Self().GetCurrentProperty<float>(Toolkit::ItemView::Property::LAYOUT_POSITION) + static_cast<float>(itemId);
}

void ItemView::ActivateLayout(unsigned int layoutIndex, const Vector3& targetSize, float durationSeconds)
{
  DALI_ASSERT_ALWAYS(layoutIndex < mLayouts.size());

  mRefreshEnabled = false;

  Actor self = Self();

  // The ItemView size should match the active layout size
  self.SetProperty(Actor::Property::SIZE, targetSize);
  mActiveLayoutTargetSize = targetSize;

  // Switch to the new layout
  mActiveLayout = mLayouts[layoutIndex].Get();

  // Move the items to the new layout positions...

  for(ConstItemIter iter = mItemPool.begin(); iter != mItemPool.end(); ++iter)
  {
    unsigned int itemId = iter->first;
    Actor        actor  = iter->second;

    // Remove constraints from previous layout
    actor.RemoveConstraints();

    mActiveLayout->ApplyConstraints(actor, itemId, targetSize, Self());

    Vector3 size;
    mActiveLayout->GetItemSize(itemId, targetSize, size);
    actor.SetProperty(Actor::Property::SIZE, size.GetVectorXY());
  }

  // Refresh the new layout
  ItemRange range = GetItemRange(*mActiveLayout, targetSize, GetCurrentLayoutPosition(0), false /* don't reserve extra*/);
  AddActorsWithinRange(range, targetSize);

  // Scroll to an appropriate layout position

  bool  scrollAnimationNeeded(false);
  float firstItemScrollPosition(0.0f);

  float current = GetCurrentLayoutPosition(0);
  float minimum = ClampFirstItemPosition(current, targetSize, *mActiveLayout);

  if(current < minimum)
  {
    scrollAnimationNeeded   = true;
    firstItemScrollPosition = minimum;
  }
  else if(mAnchoringEnabled)
  {
    scrollAnimationNeeded   = true;
    firstItemScrollPosition = mActiveLayout->GetClosestAnchorPosition(current);
  }

  if(scrollAnimationNeeded)
  {
    RemoveAnimation(mScrollAnimation);
    mScrollAnimation = Animation::New(durationSeconds);
    mScrollAnimation.AnimateTo(Property(self, Toolkit::ItemView::Property::LAYOUT_POSITION), firstItemScrollPosition, AlphaFunction::EASE_OUT);
    mScrollAnimation.FinishedSignal().Connect(this, &ItemView::OnLayoutActivationScrollFinished);
    mScrollAnimation.Play();
  }
  else
  {
    // Emit the layout activated signal
    mLayoutActivatedSignal.Emit();
  }

  AnimateScrollOvershoot(0.0f);
  mScrollOvershoot = 0.0f;

  Radian scrollDirection(mActiveLayout->GetScrollDirection());
  self.SetProperty(Toolkit::ItemView::Property::SCROLL_DIRECTION, Vector2(sinf(scrollDirection), cosf(scrollDirection)));
  self.SetProperty(Toolkit::ItemView::Property::LAYOUT_ORIENTATION, static_cast<int>(mActiveLayout->GetOrientation()));
  self.SetProperty(Toolkit::ItemView::Property::SCROLL_SPEED, mScrollSpeed);

  CalculateDomainSize(targetSize);
}

void ItemView::DeactivateCurrentLayout()
{
  if(mActiveLayout)
  {
    for(ConstItemIter iter = mItemPool.begin(); iter != mItemPool.end(); ++iter)
    {
      Actor actor = iter->second;
      actor.RemoveConstraints();
    }

    mActiveLayout = NULL;
  }
}

void ItemView::OnRefreshNotification(PropertyNotification& source)
{
  if(mRefreshNotificationEnabled)
  {
    // Cancel scroll animation to prevent any fighting of setting the scroll position property by scroll bar during fast scroll.
    if(!mRefreshEnabled && mScrollAnimation)
    {
      RemoveAnimation(mScrollAnimation);
    }

    // Only cache extra items when it is not a fast scroll
    DoRefresh(GetCurrentLayoutPosition(0), mRefreshEnabled || mScrollAnimation);
  }
}

void ItemView::Refresh()
{
  for(ConstItemIter iter = mItemPool.begin(); iter != mItemPool.end(); ++iter)
  {
    ReleaseActor(iter->first, iter->second);
  }
  mItemPool.clear();

  DoRefresh(GetCurrentLayoutPosition(0), true);
}

void ItemView::DoRefresh(float currentLayoutPosition, bool cacheExtra)
{
  if(mActiveLayout)
  {
    ItemRange range = GetItemRange(*mActiveLayout, mActiveLayoutTargetSize, currentLayoutPosition, cacheExtra /*reserve extra*/);
    RemoveActorsOutsideRange(range);
    AddActorsWithinRange(range, Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE));

    mScrollUpdatedSignal.Emit(Vector2(0.0f, currentLayoutPosition));
  }
}

void ItemView::SetMinimumSwipeSpeed(float speed)
{
  mMinimumSwipeSpeed = speed;
}

float ItemView::GetMinimumSwipeSpeed() const
{
  return mMinimumSwipeSpeed;
}

void ItemView::SetMinimumSwipeDistance(float distance)
{
  mMinimumSwipeDistance = distance;
}

float ItemView::GetMinimumSwipeDistance() const
{
  return mMinimumSwipeDistance;
}

void ItemView::SetWheelScrollDistanceStep(float step)
{
  mWheelScrollDistanceStep = step;
}

float ItemView::GetWheelScrollDistanceStep() const
{
  return mWheelScrollDistanceStep;
}

void ItemView::SetAnchoring(bool enabled)
{
  mAnchoringEnabled = enabled;
}

bool ItemView::GetAnchoring() const
{
  return mAnchoringEnabled;
}

void ItemView::SetAnchoringDuration(float durationSeconds)
{
  mAnchoringDuration = durationSeconds;
}

float ItemView::GetAnchoringDuration() const
{
  return mAnchoringDuration;
}

void ItemView::SetRefreshInterval(float intervalLayoutPositions)
{
  if(!Equals(mRefreshIntervalLayoutPositions, intervalLayoutPositions))
  {
    mRefreshIntervalLayoutPositions = intervalLayoutPositions;

    Actor self = Self();
    if(mRefreshNotification)
    {
      self.RemovePropertyNotification(mRefreshNotification);
    }
    mRefreshNotification = self.AddPropertyNotification(Toolkit::ItemView::Property::LAYOUT_POSITION, StepCondition(mRefreshIntervalLayoutPositions, 0.0f));
    mRefreshNotification.NotifySignal().Connect(this, &ItemView::OnRefreshNotification);
  }
}

float ItemView::GetRefreshInterval() const
{
  return mRefreshIntervalLayoutPositions;
}

void ItemView::SetRefreshEnabled(bool enabled)
{
  mRefreshEnabled = enabled;
}

Actor ItemView::GetItem(unsigned int itemId) const
{
  Actor actor;

  for(ConstItemIter iter = mItemPool.begin(); iter != mItemPool.end(); ++iter)
  {
    if(iter->first == itemId)
    {
      actor = iter->second;
      break;
    }
  }

  return actor;
}

unsigned int ItemView::GetItemId(Actor actor) const
{
  unsigned int itemId(0);

  for(ConstItemIter iter = mItemPool.begin(); iter != mItemPool.end(); ++iter)
  {
    if(iter->second == actor)
    {
      itemId = iter->first;
      break;
    }
  }

  return itemId;
}

void ItemView::InsertItem(Item newItem, float durationSeconds)
{
  mAddingItems       = true;
  Vector3 layoutSize = Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE);

  Actor    displacedActor;
  ItemIter afterDisplacedIter = mItemPool.end();

  ItemIter foundIter = FindItemById(mItemPool, newItem.first);
  if(mItemPool.end() != foundIter)
  {
    SetupActor(newItem, layoutSize);
    Self().Add(newItem.second);

    displacedActor    = foundIter->second;
    foundIter->second = newItem.second;

    afterDisplacedIter = ++foundIter;
  }
  else
  {
    // Inserting before the existing item range?
    ItemIter iter = mItemPool.begin();
    if(iter != mItemPool.end() &&
       iter->first > newItem.first)
    {
      displacedActor = iter->second;
      iter           = mItemPool.erase(iter); // iter is still valid after the erase

      afterDisplacedIter = iter;
    }
  }

  if(displacedActor)
  {
    // Move the existing actors to make room
    for(ItemIter iter = afterDisplacedIter; mItemPool.end() != iter; ++iter)
    {
      Actor temp     = iter->second;
      iter->second   = displacedActor;
      displacedActor = temp;

      iter->second.RemoveConstraints();
      mActiveLayout->ApplyConstraints(iter->second, iter->first, layoutSize, Self());
    }

    // Create last item
    ItemContainer::reverse_iterator lastIter = mItemPool.rbegin();
    if(lastIter != mItemPool.rend())
    {
      ItemId lastId = lastIter->first;
      Item   lastItem(lastId + 1, displacedActor);
      InsertToItemContainer(mItemPool, lastItem);

      lastItem.second.RemoveConstraints();
      mActiveLayout->ApplyConstraints(lastItem.second, lastItem.first, layoutSize, Self());
    }
  }

  CalculateDomainSize(layoutSize);

  mAddingItems = false;
}

void ItemView::InsertItems(const ItemContainer& newItems, float durationSeconds)
{
  mAddingItems       = true;
  Vector3 layoutSize = Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE);

  // Insert from lowest id to highest
  ItemContainer sortedItems(newItems);
  std::sort(sortedItems.begin(), sortedItems.end());

  for(ItemIter iter = sortedItems.begin(); sortedItems.end() != iter; ++iter)
  {
    Self().Add(iter->second);

    ItemIter foundIter = FindItemById(mItemPool, iter->first);
    if(mItemPool.end() != foundIter)
    {
      Actor moveMe      = foundIter->second;
      foundIter->second = iter->second;

      // Move the existing actors to make room
      for(ItemIter iter = ++foundIter; mItemPool.end() != iter; ++iter)
      {
        Actor temp   = iter->second;
        iter->second = moveMe;
        moveMe       = temp;
      }

      // Create last item
      ItemId lastId = mItemPool.rbegin()->first;
      Item   lastItem(lastId + 1, moveMe);
      InsertToItemContainer(mItemPool, lastItem);
    }
    else
    {
      InsertToItemContainer(mItemPool, *iter);
    }
  }

  // Relayout everything
  for(ItemIter iter = mItemPool.begin(); iter != mItemPool.end(); ++iter)
  {
    // If newly inserted
    if(std::binary_search(sortedItems.begin(), sortedItems.end(), *iter))
    {
      SetupActor(*iter, layoutSize);
    }
    else
    {
      iter->second.RemoveConstraints();
      mActiveLayout->ApplyConstraints(iter->second, iter->first, layoutSize, Self());
    }
  }

  CalculateDomainSize(layoutSize);

  mAddingItems = false;
}

void ItemView::RemoveItem(unsigned int itemId, float durationSeconds)
{
  bool actorsReordered = RemoveActor(itemId);
  if(actorsReordered)
  {
    ReapplyAllConstraints();

    OnItemsRemoved();
  }
}

void ItemView::RemoveItems(const ItemIdContainer& itemIds, float durationSeconds)
{
  bool actorsReordered(false);

  // Remove from highest id to lowest
  ItemIdContainer sortedItems(itemIds);
  std::sort(sortedItems.begin(), sortedItems.end());

  for(ItemIdContainer::reverse_iterator iter = sortedItems.rbegin(); sortedItems.rend() != iter; ++iter)
  {
    if(RemoveActor(*iter))
    {
      actorsReordered = true;
    }
  }

  if(actorsReordered)
  {
    ReapplyAllConstraints();

    OnItemsRemoved();
  }
}

bool ItemView::RemoveActor(unsigned int itemId)
{
  bool reordered(false);

  ItemIter removeIter = FindItemById(mItemPool, itemId);
  if(removeIter != mItemPool.end())
  {
    ReleaseActor(itemId, removeIter->second);
  }
  else
  {
    // Removing before the existing item range?
    ItemIter iter = mItemPool.begin();
    if(iter != mItemPool.end() &&
       iter->first > itemId)
    {
      // In order to decrement the first visible item ID
      InsertToItemContainer(mItemPool, Item(iter->first - 1, Actor()));

      removeIter = mItemPool.begin();
    }
  }

  if(removeIter != mItemPool.end())
  {
    reordered = true;

    // Adjust the remaining item IDs, for example if item 2 is removed:
    //   Initial actors:     After insert:
    //     ID 1 - ActorA       ID 1 - ActorA
    //     ID 2 - ActorB       ID 2 - ActorC (previously ID 3)
    //     ID 3 - ActorC       ID 3 - ActorB (previously ID 4)
    //     ID 4 - ActorD
    for(ItemIter iter = removeIter; iter != mItemPool.end(); ++iter)
    {
      if(iter->first < mItemPool.rbegin()->first)
      {
        iter->second = (iter + 1)->second;
      }
      else
      {
        mItemPool.erase(iter);
        break;
      }
    }
  }

  return reordered;
}

void ItemView::ReplaceItem(Item replacementItem, float durationSeconds)
{
  mAddingItems       = true;
  Vector3 layoutSize = Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE);

  SetupActor(replacementItem, layoutSize);
  Self().Add(replacementItem.second);

  const ItemIter iter = FindItemById(mItemPool, replacementItem.first);
  if(mItemPool.end() != iter)
  {
    ReleaseActor(iter->first, iter->second);
    iter->second = replacementItem.second;
  }
  else
  {
    InsertToItemContainer(mItemPool, replacementItem);
  }

  CalculateDomainSize(layoutSize);

  mAddingItems = false;
}

void ItemView::ReplaceItems(const ItemContainer& replacementItems, float durationSeconds)
{
  for(ConstItemIter iter = replacementItems.begin(); replacementItems.end() != iter; ++iter)
  {
    ReplaceItem(*iter, durationSeconds);
  }
}

void ItemView::RemoveActorsOutsideRange(ItemRange range)
{
  // Remove unwanted actors from the ItemView & ItemPool
  for(ItemIter iter = mItemPool.begin(); iter != mItemPool.end();)
  {
    unsigned int current = iter->first;

    if(!range.Within(current))
    {
      ReleaseActor(iter->first, iter->second);

      iter = mItemPool.erase(iter); // iter is still valid after the erase
    }
    else
    {
      ++iter;
    }
  }
}

void ItemView::AddActorsWithinRange(ItemRange range, const Vector3& layoutSize)
{
  range.end = std::min(mItemFactory.GetNumberOfItems(), range.end);

  // The order of addition depends on the scroll direction.
  if(mRefreshOrderHint)
  {
    for(unsigned int itemId = range.begin; itemId < range.end; ++itemId)
    {
      AddNewActor(itemId, layoutSize);
    }
  }
  else
  {
    for(unsigned int itemId = range.end; itemId > range.begin; --itemId)
    {
      AddNewActor(itemId - 1, layoutSize);
    }
  }

  // Total number of items may change dynamically.
  // Always recalculate the domain size to reflect that.
  CalculateDomainSize(Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE));
}

void ItemView::AddNewActor(unsigned int itemId, const Vector3& layoutSize)
{
  mAddingItems = true;

  if(mItemPool.end() == FindItemById(mItemPool, itemId))
  {
    Actor actor = mItemFactory.NewItem(itemId);

    if(actor)
    {
      Item newItem(itemId, actor);

      InsertToItemContainer(mItemPool, newItem);

      SetupActor(newItem, layoutSize);
      Self().Add(actor);
    }
  }

  mAddingItems = false;
}

void ItemView::SetupActor(Item item, const Vector3& layoutSize)
{
  item.second.SetProperty(Actor::Property::PARENT_ORIGIN, mItemsParentOrigin);
  item.second.SetProperty(Actor::Property::ANCHOR_POINT, mItemsAnchorPoint);

  if(mActiveLayout)
  {
    Vector3 size;
    mActiveLayout->GetItemSize(item.first, mActiveLayoutTargetSize, size);
    item.second.SetProperty(Actor::Property::SIZE, size.GetVectorXY());

    mActiveLayout->ApplyConstraints(item.second, item.first, layoutSize, Self());
  }
}

void ItemView::ReleaseActor(ItemId item, Actor actor)
{
  Self().Remove(actor);
  mItemFactory.ItemReleased(item, actor);
}

ItemRange ItemView::GetItemRange(ItemLayout& layout, const Vector3& layoutSize, float layoutPosition, bool reserveExtra)
{
  unsigned int itemCount = mItemFactory.GetNumberOfItems();

  ItemRange available(0u, itemCount);

  ItemRange range = layout.GetItemsWithinArea(layoutPosition, layoutSize);

  if(reserveExtra)
  {
    // Add the reserve items for scrolling
    unsigned int extra = layout.GetReserveItemCount(layoutSize);
    range.begin        = (range.begin >= extra) ? (range.begin - extra) : 0u;
    range.end += extra;
  }

  return range.Intersection(available);
}

void ItemView::OnChildAdd(Actor& child)
{
  if(!mAddingItems)
  {
    // We don't want to do this downcast check for any item added by ItemView itself.
    Dali::Toolkit::ScrollBar scrollBar = Dali::Toolkit::ScrollBar::DownCast(child);
    if(scrollBar)
    {
      scrollBar.SetScrollPropertySource(Self(),
                                        Toolkit::ItemView::Property::LAYOUT_POSITION,
                                        Toolkit::Scrollable::Property::SCROLL_POSITION_MIN_Y,
                                        Toolkit::Scrollable::Property::SCROLL_POSITION_MAX_Y,
                                        Toolkit::ItemView::Property::SCROLL_CONTENT_SIZE);
    }
  }

  Scrollable::OnChildAdd(child);
}

bool ItemView::OnWheelEvent(Actor actor, const WheelEvent& event)
{
  // Respond the wheel event to scroll
  if(mActiveLayout)
  {
    Actor         self                    = Self();
    const Vector3 layoutSize              = Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE);
    float         layoutPositionDelta     = GetCurrentLayoutPosition(0) - (event.GetDelta() * mWheelScrollDistanceStep * mActiveLayout->GetScrollSpeedFactor());
    float         firstItemScrollPosition = ClampFirstItemPosition(layoutPositionDelta, layoutSize, *mActiveLayout);

    self.SetProperty(Toolkit::ItemView::Property::LAYOUT_POSITION, firstItemScrollPosition);

    mScrollStartedSignal.Emit(GetCurrentScrollPosition());
    mRefreshEnabled = true;
  }

  if(mWheelEventFinishedTimer.IsRunning())
  {
    mWheelEventFinishedTimer.Stop();
  }

  mWheelEventFinishedTimer.Start();

  return true;
}

bool ItemView::OnWheelEventFinished()
{
  if(mActiveLayout)
  {
    RemoveAnimation(mScrollAnimation);

    // No more wheel events coming. Do the anchoring if enabled.
    mScrollAnimation = DoAnchoring();
    if(mScrollAnimation)
    {
      mScrollAnimation.FinishedSignal().Connect(this, &ItemView::OnScrollFinished);
      mScrollAnimation.Play();
    }
    else
    {
      mScrollOvershoot = 0.0f;
      AnimateScrollOvershoot(0.0f);

      mScrollCompletedSignal.Emit(GetCurrentScrollPosition());
    }
  }

  return false;
}

void ItemView::ReapplyAllConstraints()
{
  Vector3 layoutSize = Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE);

  for(ConstItemIter iter = mItemPool.begin(); iter != mItemPool.end(); ++iter)
  {
    unsigned int id    = iter->first;
    Actor        actor = iter->second;

    actor.RemoveConstraints();
    mActiveLayout->ApplyConstraints(actor, id, layoutSize, Self());
  }
}

void ItemView::OnItemsRemoved()
{
  CalculateDomainSize(Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  // Adjust scroll-position after an item is removed
  if(mActiveLayout)
  {
    float firstItemScrollPosition = ClampFirstItemPosition(GetCurrentLayoutPosition(0), Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE), *mActiveLayout);
    Self().SetProperty(Toolkit::ItemView::Property::LAYOUT_POSITION, firstItemScrollPosition);
  }
}

float ItemView::ClampFirstItemPosition(float targetPosition, const Vector3& targetSize, ItemLayout& layout, bool updateOvershoot)
{
  Actor self              = Self();
  float minLayoutPosition = layout.GetMinimumLayoutPosition(mItemFactory.GetNumberOfItems(), targetSize);
  float clamppedPosition  = std::min(0.0f, std::max(minLayoutPosition, targetPosition));
  self.SetProperty(Toolkit::Scrollable::Property::SCROLL_POSITION_MAX, Vector2(0.0f, -minLayoutPosition));

  if(updateOvershoot)
  {
    mScrollOvershoot = targetPosition - clamppedPosition;
  }

  return clamppedPosition;
}

bool ItemView::OnTouch(Actor actor, const TouchEvent& touch)
{
  // Ignore events with multiple-touch points
  if(touch.GetPointCount() != 1)
  {
    return false;
  }

  if(touch.GetState(0) == PointState::DOWN)
  {
    // Cancel ongoing scrolling etc.
    mGestureState = GestureState::CLEAR;

    mScrollDistance = 0.0f;
    mScrollSpeed    = 0.0f;
    Self().SetProperty(Toolkit::ItemView::Property::SCROLL_SPEED, mScrollSpeed);

    mScrollOvershoot = 0.0f;
    AnimateScrollOvershoot(0.0f);

    if(mScrollAnimation)
    {
      mScrollCompletedSignal.Emit(GetCurrentScrollPosition());
    }

    RemoveAnimation(mScrollAnimation);
  }

  return false; // Do not consume as we're potentially scrolling (detecting pan gestures)
}

void ItemView::OnPan(const PanGesture& gesture)
{
  Actor         self       = Self();
  const Vector3 layoutSize = Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE);

  RemoveAnimation(mScrollAnimation);

  // Short-circuit if there is no active layout
  if(!mActiveLayout)
  {
    mGestureState = GestureState::CLEAR;
    return;
  }

  mGestureState = gesture.GetState();

  switch(mGestureState)
  {
    case GestureState::FINISHED:
    {
      // Swipe Detection
      if(fabsf(mScrollDistance) > mMinimumSwipeDistance &&
         mScrollSpeed > mMinimumSwipeSpeed)
      {
        float direction = (mScrollDistance < 0.0f) ? -1.0f : 1.0f;

        mRefreshOrderHint = true;

        float currentLayoutPosition   = GetCurrentLayoutPosition(0);
        float firstItemScrollPosition = ClampFirstItemPosition(currentLayoutPosition + mScrollSpeed * direction,
                                                               layoutSize,
                                                               *mActiveLayout);

        if(mAnchoringEnabled)
        {
          firstItemScrollPosition = mActiveLayout->GetClosestAnchorPosition(firstItemScrollPosition);
        }

        RemoveAnimation(mScrollAnimation);

        float flickAnimationDuration = Clamp(mActiveLayout->GetItemFlickAnimationDuration() * std::max(1.0f, fabsf(firstItemScrollPosition - GetCurrentLayoutPosition(0))), DEFAULT_MINIMUM_SWIPE_DURATION, DEFAULT_MAXIMUM_SWIPE_DURATION);

        mScrollAnimation = Animation::New(flickAnimationDuration);
        mScrollAnimation.AnimateTo(Property(self, Toolkit::ItemView::Property::LAYOUT_POSITION), firstItemScrollPosition, AlphaFunction::EASE_OUT);
        mScrollAnimation.AnimateTo(Property(self, Toolkit::ItemView::Property::SCROLL_SPEED), 0.0f, AlphaFunction::EASE_OUT);

        mIsFlicking = true;

        // Check whether it has already scrolled to the end
        if(fabs(currentLayoutPosition - firstItemScrollPosition) < Math::MACHINE_EPSILON_0)
        {
          AnimateScrollOvershoot(0.0f);
          RemoveAnimation(mScrollAnimation);
        }
      }

      // Anchoring may be triggered when there was no swipe
      if(!mScrollAnimation)
      {
        mScrollAnimation = DoAnchoring();
      }

      // Reset the overshoot if no scroll animation.
      if(!mScrollAnimation)
      {
        mScrollCompletedSignal.Emit(GetCurrentScrollPosition());

        AnimateScrollOvershoot(0.0f, false);
      }
    }
    break;

    case GestureState::STARTED: // Fall through
    {
      mTotalPanDisplacement = Vector2::ZERO;
      mScrollStartedSignal.Emit(GetCurrentScrollPosition());
      mRefreshEnabled = true;
    }

    case GestureState::CONTINUING:
    {
      const Vector2& displacement = gesture.GetDisplacement();
      mScrollDistance             = CalculateScrollDistance(displacement, *mActiveLayout);
      mScrollSpeed                = Clamp((gesture.GetSpeed() * gesture.GetSpeed() * mActiveLayout->GetFlickSpeedFactor() * MILLISECONDS_PER_SECONDS), 0.0f, mActiveLayout->GetMaximumSwipeSpeed());

      // Refresh order depends on the direction of the scroll; negative is towards the last item.
      mRefreshOrderHint = mScrollDistance < 0.0f;

      float layoutPositionDelta = GetCurrentLayoutPosition(0) + (mScrollDistance * mActiveLayout->GetScrollSpeedFactor());

      float firstItemScrollPosition = ClampFirstItemPosition(layoutPositionDelta, layoutSize, *mActiveLayout);

      float currentOvershoot = self.GetCurrentProperty<float>(Toolkit::ItemView::Property::OVERSHOOT);

      self.SetProperty(Toolkit::ItemView::Property::LAYOUT_POSITION, firstItemScrollPosition);

      if((firstItemScrollPosition >= 0.0f &&
          currentOvershoot < 1.0f) ||
         (firstItemScrollPosition <= mActiveLayout->GetMinimumLayoutPosition(mItemFactory.GetNumberOfItems(), layoutSize) &&
          currentOvershoot > -1.0f))
      {
        mTotalPanDisplacement += displacement;
      }

      mScrollOvershoot = CalculateScrollOvershoot();

      // If the view is moved in a direction against the overshoot indicator, then the indicator should be animated off.
      // First make sure we are not in an animation, otherwise a previously started
      // off-animation will be overwritten as the user continues scrolling.
      if(!mInAnimation)
      {
        // Check if the movement is against the current overshoot amount (if we are currently displaying the indicator).
        if(((mScrollOvershoot > Math::MACHINE_EPSILON_0) && (mScrollDistance < -Math::MACHINE_EPSILON_0)) ||
           ((mScrollOvershoot < Math::MACHINE_EPSILON_0) && (mScrollDistance > Math::MACHINE_EPSILON_0)))
        {
          // The user has moved against the indicator direction.
          // First, we reset the total displacement. This means the overshoot amount will become zero the next frame,
          // and if the user starts dragging in the overshoot direction again, the indicator will appear once more.
          mTotalPanDisplacement = Vector2::ZERO;
          // Animate the overshoot indicator off.
          AnimateScrollOvershoot(0.0f, false);
        }
        else
        {
          // Only set the property directly if we are not animating the overshoot away,
          // as otherwise this will overwrite the animation generated value.
          self.SetProperty(Toolkit::ItemView::Property::OVERSHOOT, mScrollOvershoot);
        }
      }
    }
    break;

    case GestureState::CANCELLED:
    {
      mScrollAnimation = DoAnchoring();
    }
    break;

    default:
      break;
  }

  if(mScrollAnimation)
  {
    mScrollAnimation.FinishedSignal().Connect(this, &ItemView::OnScrollFinished);
    mScrollAnimation.Play();
  }
}

bool ItemView::OnAccessibilityPan(PanGesture gesture)
{
  OnPan(gesture);
  return true;
}

Actor ItemView::GetNextKeyboardFocusableActor(Actor actor, Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled)
{
  Actor nextFocusActor;
  if(mActiveLayout)
  {
    int nextItemID = 0;
    if(!actor || actor == this->Self())
    {
      nextFocusActor = GetItem(nextItemID);
    }
    else if(actor && actor.GetParent() == this->Self())
    {
      int itemID     = GetItemId(actor);
      nextItemID     = mActiveLayout->GetNextFocusItemID(itemID, mItemFactory.GetNumberOfItems(), direction, loopEnabled);
      nextFocusActor = GetItem(nextItemID);
      if(nextFocusActor == actor)
      {
        // need to pass NULL actor back to focus manager
        nextFocusActor.Reset();
        return nextFocusActor;
      }
    }
    float   layoutPosition = mActiveLayout->GetClosestAnchorPosition(GetCurrentLayoutPosition(0));
    Vector3 layoutSize     = Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE);
    if(!nextFocusActor)
    {
      // likely the current item is not buffered, so not in our item pool, probably best to get first viewable item
      ItemRange viewableItems = mActiveLayout->GetItemsWithinArea(layoutPosition, layoutSize);
      nextItemID              = viewableItems.begin;
      nextFocusActor          = GetItem(nextItemID);
    }
  }
  return nextFocusActor;
}

void ItemView::OnKeyboardFocusChangeCommitted(Actor commitedFocusableActor)
{
  // only in this function if our chosen focus actor was actually used
  if(commitedFocusableActor)
  {
    int     nextItemID     = GetItemId(commitedFocusableActor);
    float   layoutPosition = GetCurrentLayoutPosition(0);
    Vector3 layoutSize     = Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE);

    float scrollTo = mActiveLayout->GetClosestOnScreenLayoutPosition(nextItemID, layoutPosition, layoutSize);
    ScrollTo(Vector2(0.0f, scrollTo), DEFAULT_KEYBOARD_FOCUS_SCROLL_DURATION);
  }
}

bool ItemView::ItemViewAccessible::ScrollToChild(Actor child)
{
  auto itemView = Dali::Toolkit::ItemView::DownCast(Self());
  Toolkit::GetImpl(itemView).OnKeyboardFocusChangeCommitted(child);
  return true;
}

Animation ItemView::DoAnchoring()
{
  Animation anchoringAnimation;
  Actor     self = Self();

  if(mActiveLayout && mAnchoringEnabled)
  {
    float anchorPosition = mActiveLayout->GetClosestAnchorPosition(GetCurrentLayoutPosition(0));

    anchoringAnimation = Animation::New(mAnchoringDuration);
    anchoringAnimation.AnimateTo(Property(self, Toolkit::ItemView::Property::LAYOUT_POSITION), anchorPosition, AlphaFunction::EASE_OUT);
    anchoringAnimation.AnimateTo(Property(self, Toolkit::ItemView::Property::SCROLL_SPEED), 0.0f, AlphaFunction::EASE_OUT);
    if(!mIsFlicking)
    {
      AnimateScrollOvershoot(0.0f);
    }
  }

  return anchoringAnimation;
}

void ItemView::OnScrollFinished(Animation& source)
{
  Actor self = Self();

  RemoveAnimation(mScrollAnimation); // mScrollAnimation is used to query whether we're scrolling

  mScrollCompletedSignal.Emit(GetCurrentScrollPosition());

  if(mIsFlicking && fabsf(mScrollOvershoot) > Math::MACHINE_EPSILON_1)
  {
    AnimateScrollOvershoot(mScrollOvershoot > 0.0f ? 1.0f : -1.0f, true);
  }
  else
  {
    // Reset the overshoot
    AnimateScrollOvershoot(0.0f);
  }
  mIsFlicking = false;

  mScrollOvershoot = 0.0f;
}

void ItemView::OnLayoutActivationScrollFinished(Animation& source)
{
  RemoveAnimation(mScrollAnimation);
  mRefreshEnabled = true;
  DoRefresh(GetCurrentLayoutPosition(0), true);

  // Emit the layout activated signal
  mLayoutActivatedSignal.Emit();
}

void ItemView::OnOvershootOnFinished(Animation& animation)
{
  mAnimatingOvershootOn = false;
  mScrollOvershootAnimation.FinishedSignal().Disconnect(this, &ItemView::OnOvershootOnFinished);
  RemoveAnimation(mScrollOvershootAnimation);
  if(mAnimateOvershootOff)
  {
    AnimateScrollOvershoot(0.0f);
  }
  mInAnimation = false;
}

void ItemView::ScrollToItem(unsigned int itemId, float durationSeconds)
{
  Actor         self                    = Self();
  const Vector3 layoutSize              = Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  float         firstItemScrollPosition = ClampFirstItemPosition(mActiveLayout->GetItemScrollToPosition(itemId), layoutSize, *mActiveLayout);

  if(durationSeconds > 0.0f)
  {
    RemoveAnimation(mScrollAnimation);
    mScrollAnimation = Animation::New(durationSeconds);
    mScrollAnimation.AnimateTo(Property(self, Toolkit::ItemView::Property::LAYOUT_POSITION), firstItemScrollPosition, mScrollToAlphaFunction);
    mScrollAnimation.FinishedSignal().Connect(this, &ItemView::OnScrollFinished);
    mScrollAnimation.Play();
  }
  else
  {
    self.SetProperty(Toolkit::ItemView::Property::LAYOUT_POSITION, firstItemScrollPosition);
    AnimateScrollOvershoot(0.0f);
  }

  mScrollStartedSignal.Emit(GetCurrentScrollPosition());
  mRefreshEnabled = true;
}

void ItemView::RemoveAnimation(Animation& animation)
{
  if(animation)
  {
    // Cease animating, and reset handle.
    animation.Clear();
    animation.Reset();
  }
}

void ItemView::CalculateDomainSize(const Vector3& layoutSize)
{
  Actor self = Self();

  Vector3 firstItemPosition(Vector3::ZERO);
  Vector3 lastItemPosition(Vector3::ZERO);

  if(mActiveLayout)
  {
    firstItemPosition = mActiveLayout->GetItemPosition(0, 0, layoutSize);

    float minLayoutPosition = mActiveLayout->GetMinimumLayoutPosition(mItemFactory.GetNumberOfItems(), layoutSize);
    lastItemPosition        = mActiveLayout->GetItemPosition(fabs(minLayoutPosition), fabs(minLayoutPosition), layoutSize);

    float domainSize;

    if(IsHorizontal(mActiveLayout->GetOrientation()))
    {
      domainSize = fabs(firstItemPosition.x - lastItemPosition.x);
    }
    else
    {
      domainSize = fabs(firstItemPosition.y - lastItemPosition.y);
    }

    self.SetProperty(Toolkit::Scrollable::Property::SCROLL_POSITION_MIN, Vector2::ZERO);
    self.SetProperty(Toolkit::Scrollable::Property::SCROLL_POSITION_MAX, Vector2(0.0f, -minLayoutPosition));

    self.SetProperty(Toolkit::ItemView::Property::SCROLL_CONTENT_SIZE, domainSize);

    bool isLayoutScrollable = IsLayoutScrollable(layoutSize);
    self.SetProperty(Toolkit::Scrollable::Property::CAN_SCROLL_VERTICAL, isLayoutScrollable);
    self.SetProperty(Toolkit::Scrollable::Property::CAN_SCROLL_HORIZONTAL, false);
  }
}

bool ItemView::IsLayoutScrollable(const Vector3& layoutSize)
{
  Actor self = Self();

  float currentLayoutPosition   = ClampFirstItemPosition(GetCurrentLayoutPosition(0), layoutSize, *mActiveLayout, false);
  float forwardClampedPosition  = ClampFirstItemPosition(currentLayoutPosition + 1.0, layoutSize, *mActiveLayout, false);
  float backwardClampedPosition = ClampFirstItemPosition(currentLayoutPosition - 1.0, layoutSize, *mActiveLayout, false);

  return (fabs(forwardClampedPosition - backwardClampedPosition) > Math::MACHINE_EPSILON_0);
}

float ItemView::GetScrollPosition(float layoutPosition, const Vector3& layoutSize) const
{
  Vector3 firstItemPosition(mActiveLayout->GetItemPosition(0, layoutPosition, layoutSize));
  return IsHorizontal(mActiveLayout->GetOrientation()) ? firstItemPosition.x : firstItemPosition.y;
}

Vector2 ItemView::GetCurrentScrollPosition() const
{
  return Vector2(0.0f, GetScrollPosition(GetCurrentLayoutPosition(0), Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE)));
}

void ItemView::AddOverlay(Actor actor)
{
  actor.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
  Self().Add(actor);
}

void ItemView::RemoveOverlay(Actor actor)
{
  Self().Remove(actor);
}

void ItemView::ScrollTo(const Vector2& position, float duration)
{
  Actor         self       = Self();
  const Vector3 layoutSize = Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE);

  float firstItemScrollPosition = ClampFirstItemPosition(position.y, layoutSize, *mActiveLayout);

  if(duration > 0.0f)
  {
    RemoveAnimation(mScrollAnimation);
    mScrollAnimation = Animation::New(duration);
    mScrollAnimation.AnimateTo(Property(self, Toolkit::ItemView::Property::LAYOUT_POSITION), firstItemScrollPosition, mScrollToAlphaFunction);
    mScrollAnimation.FinishedSignal().Connect(this, &ItemView::OnScrollFinished);
    mScrollAnimation.Play();
  }
  else
  {
    self.SetProperty(Toolkit::ItemView::Property::LAYOUT_POSITION, firstItemScrollPosition);
    AnimateScrollOvershoot(0.0f);
  }

  mScrollStartedSignal.Emit(GetCurrentScrollPosition());
  mRefreshEnabled = true;
}

void ItemView::SetOvershootSize(const Vector2& size)
{
  mOvershootSize = size;

  if(mOvershootOverlay)
  {
    // Remove old & add new size constraint
    mOvershootOverlay.RemoveConstraints(OVERSHOOT_SIZE_CONSTRAINT_TAG);
    ApplyOvershootSizeConstraint(mOvershootOverlay, mOvershootSize.height);
  }
}

void ItemView::SetOvershootEffectColor(const Vector4& color)
{
  mOvershootEffectColor = color;
  if(mOvershootOverlay)
  {
    mOvershootOverlay.SetProperty(Actor::Property::COLOR, color);
  }
}

void ItemView::EnableScrollOvershoot(bool enable)
{
  Actor self = Self();
  if(enable)
  {
    if(!mOvershootOverlay)
    {
      Property::Index effectOvershootPropertyIndex = Property::INVALID_INDEX;
      mOvershootOverlay                            = CreateBouncingEffectActor(effectOvershootPropertyIndex);
      mOvershootOverlay.SetProperty(Actor::Property::COLOR, mOvershootEffectColor);
      mOvershootOverlay.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
      mOvershootOverlay.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
      mOvershootOverlay.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
      self.Add(mOvershootOverlay);

      ApplyOvershootSizeConstraint(mOvershootOverlay, mOvershootSize.height);

      Constraint constraint = Constraint::New<Quaternion>(mOvershootOverlay, Actor::Property::ORIENTATION, OvershootOverlayRotationConstraint);
      constraint.AddSource(ParentSource(Toolkit::ItemView::Property::SCROLL_DIRECTION));
      constraint.AddSource(ParentSource(Toolkit::ItemView::Property::LAYOUT_ORIENTATION));
      constraint.AddSource(ParentSource(Toolkit::ItemView::Property::OVERSHOOT));
      constraint.Apply();

      constraint = Constraint::New<Vector3>(mOvershootOverlay, Actor::Property::POSITION, OvershootOverlayPositionConstraint);
      constraint.AddSource(ParentSource(Actor::Property::SIZE));
      constraint.AddSource(ParentSource(Toolkit::ItemView::Property::SCROLL_DIRECTION));
      constraint.AddSource(ParentSource(Toolkit::ItemView::Property::LAYOUT_ORIENTATION));
      constraint.AddSource(ParentSource(Toolkit::ItemView::Property::OVERSHOOT));
      constraint.Apply();

      constraint = Constraint::New<bool>(mOvershootOverlay, Actor::Property::VISIBLE, OvershootOverlayVisibilityConstraint);
      constraint.AddSource(ParentSource(Toolkit::Scrollable::Property::CAN_SCROLL_VERTICAL));
      constraint.Apply();

      constraint = Constraint::New<float>(mOvershootOverlay, effectOvershootPropertyIndex, EqualToConstraint());
      constraint.AddSource(ParentSource(Toolkit::ItemView::Property::OVERSHOOT));
      constraint.Apply();
    }
  }
  else
  {
    if(mOvershootOverlay)
    {
      self.Remove(mOvershootOverlay);
      mOvershootOverlay.Reset();
    }
  }
}

float ItemView::CalculateScrollOvershoot()
{
  float overshoot = 0.0f;

  if(mActiveLayout)
  {
    // The overshoot must be calculated from the accumulated pan gesture displacement
    // since the pan gesture starts.
    Actor self              = Self();
    float scrollDistance    = CalculateScrollDistance(mTotalPanDisplacement, *mActiveLayout) * mActiveLayout->GetScrollSpeedFactor();
    float positionDelta     = GetCurrentLayoutPosition(0) + scrollDistance;
    float minLayoutPosition = mActiveLayout->GetMinimumLayoutPosition(mItemFactory.GetNumberOfItems(), Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE));
    self.SetProperty(Toolkit::Scrollable::Property::SCROLL_POSITION_MAX, Vector2(0.0f, -minLayoutPosition));
    float clamppedPosition = std::min(0.0f, std::max(minLayoutPosition, positionDelta));
    overshoot              = positionDelta - clamppedPosition;
  }

  return overshoot > 0.0f ? std::min(overshoot, 1.0f) : std::max(overshoot, -1.0f);
}

void ItemView::AnimateScrollOvershoot(float overshootAmount, bool animateBack)
{
  bool animatingOn = fabsf(overshootAmount) > Math::MACHINE_EPSILON_1;

  // make sure we animate back if needed
  mAnimateOvershootOff = animateBack || (!animatingOn && mAnimatingOvershootOn);

  if(mAnimatingOvershootOn)
  {
    // animating on, do not allow animate off
    return;
  }

  Actor self = Self();

  if(mOvershootAnimationSpeed > Math::MACHINE_EPSILON_0)
  {
    float currentOvershoot = self.GetCurrentProperty<float>(Toolkit::ItemView::Property::OVERSHOOT);
    float duration         = 0.0f;

    if(mOvershootOverlay)
    {
      duration = mOvershootOverlay.GetCurrentProperty<Vector3>(Actor::Property::SIZE).height * (animatingOn ? (1.0f - fabsf(currentOvershoot)) : fabsf(currentOvershoot)) / mOvershootAnimationSpeed;
    }

    // Mark the animation as in progress to prevent manual property sets overwriting it.
    mInAnimation          = true;
    mAnimatingOvershootOn = animatingOn;
    RemoveAnimation(mScrollOvershootAnimation);
    mScrollOvershootAnimation = Animation::New(duration);
    mScrollOvershootAnimation.FinishedSignal().Connect(this, &ItemView::OnOvershootOnFinished);
    mScrollOvershootAnimation.AnimateTo(Property(self, Toolkit::ItemView::Property::OVERSHOOT), overshootAmount, TimePeriod(0.0f, duration));
    mScrollOvershootAnimation.Play();
  }
  else
  {
    self.SetProperty(Toolkit::ItemView::Property::OVERSHOOT, overshootAmount);
  }
}

void ItemView::SetItemsParentOrigin(const Vector3& parentOrigin)
{
  if(parentOrigin != mItemsParentOrigin)
  {
    mItemsParentOrigin = parentOrigin;
    for(ItemIter iter = mItemPool.begin(); iter != mItemPool.end(); ++iter)
    {
      iter->second.SetProperty(Actor::Property::PARENT_ORIGIN, parentOrigin);
    }
  }
}

Vector3 ItemView::GetItemsParentOrigin() const
{
  return mItemsParentOrigin;
}

void ItemView::SetItemsAnchorPoint(const Vector3& anchorPoint)
{
  if(anchorPoint != mItemsAnchorPoint)
  {
    mItemsAnchorPoint = anchorPoint;
    for(ItemIter iter = mItemPool.begin(); iter != mItemPool.end(); ++iter)
    {
      iter->second.SetProperty(Actor::Property::ANCHOR_POINT, anchorPoint);
    }
  }
}

Vector3 ItemView::GetItemsAnchorPoint() const
{
  return mItemsAnchorPoint;
}

void ItemView::GetItemsRange(ItemRange& range)
{
  if(!mItemPool.empty())
  {
    range.begin = mItemPool.begin()->first;
    range.end   = mItemPool.rbegin()->first + 1;
  }
  else
  {
    range.begin = 0;
    range.end   = 0;
  }
}

bool ItemView::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  Dali::BaseHandle handle(object);

  bool              connected(true);
  Toolkit::ItemView itemView = Toolkit::ItemView::DownCast(handle);

  if(0 == strcmp(signalName.c_str(), LAYOUT_ACTIVATED_SIGNAL))
  {
    itemView.LayoutActivatedSignal().Connect(tracker, functor);
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void ItemView::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  Toolkit::ItemView itemView = Toolkit::ItemView::DownCast(Dali::BaseHandle(object));

  if(itemView)
  {
    ItemView& itemViewImpl(GetImpl(itemView));
    switch(index)
    {
      case Toolkit::ItemView::Property::MINIMUM_SWIPE_SPEED:
      {
        itemViewImpl.SetMinimumSwipeSpeed(value.Get<float>());
        break;
      }

      case Toolkit::ItemView::Property::MINIMUM_SWIPE_DISTANCE:
      {
        itemViewImpl.SetMinimumSwipeDistance(value.Get<float>());
        break;
      }

      case Toolkit::ItemView::Property::WHEEL_SCROLL_DISTANCE_STEP:
      {
        itemViewImpl.SetWheelScrollDistanceStep(value.Get<float>());
        break;
      }

      case Toolkit::ItemView::Property::SNAP_TO_ITEM_ENABLED:
      {
        itemViewImpl.SetAnchoring(value.Get<bool>());
        break;
      }

      case Toolkit::ItemView::Property::REFRESH_INTERVAL:
      {
        itemViewImpl.SetRefreshInterval(value.Get<float>());
        break;
      }

      case Toolkit::ItemView::Property::LAYOUT:
      {
        // Get a Property::Array from the property if possible.
        Property::Array layoutArray;
        if(value.Get(layoutArray))
        {
          itemViewImpl.SetLayoutArray(layoutArray);
        }
        break;
      }
    }
  }
}

Property::Array ItemView::GetLayoutArray()
{
  return mlayoutArray;
}

void ItemView::SetLayoutArray(const Property::Array& layouts)
{
  mlayoutArray          = layouts;
  const int layoutCount = GetLayoutCount();
  if(layoutCount > 0)
  {
    for(int index = layoutCount - 1; index >= 0; --index)
    {
      RemoveLayout(index);
      if(index == 0) break;
    }
  }

  for(unsigned int arrayIdx = 0, arrayCount = layouts.Count(); arrayIdx < arrayCount; ++arrayIdx)
  {
    const Property::Value& element = layouts.GetElementAt(arrayIdx);

    const Property::Map* layout = element.GetMap();
    if(layout != NULL)
    {
      for(unsigned int mapIdx = 0, mapCount = (*layout).Count(); mapIdx < mapCount; ++mapIdx)
      {
        KeyValuePair propertyPair((*layout).GetKeyValue(mapIdx));

        if(propertyPair.first == DefaultItemLayoutProperty::TYPE)
        {
          int layoutType = propertyPair.second.Get<int>();
          if(layoutType <= DefaultItemLayout::SPIRAL && layoutType >= DefaultItemLayout::DEPTH)
          {
            //DEPTH, GRID, LIST, SPIRAL
            switch(DefaultItemLayout::Type(layoutType))
            {
              case DefaultItemLayout::DEPTH:
              {
                Internal::DepthLayoutPtr depthLayout = Internal::DepthLayout::New();
                (*depthLayout).SetLayoutProperties(*layout);
                (*depthLayout).SetDepthLayoutProperties(*layout);
                AddLayout(*depthLayout);
                break;
              }
              case DefaultItemLayout::GRID:
              {
                Internal::GridLayoutPtr gridLayout = Internal::GridLayout::New();
                (*gridLayout).SetLayoutProperties(*layout);
                (*gridLayout).SetGridLayoutProperties(*layout);
                AddLayout(*gridLayout);
                break;
              }
              case DefaultItemLayout::LIST:
              {
                Internal::GridLayoutPtr listLayout = Internal::GridLayout::New();
                listLayout->SetNumberOfColumns(1);
                (*listLayout).SetLayoutProperties(*layout);
                (*listLayout).SetGridLayoutProperties(*layout);
                AddLayout(*listLayout);
                break;
              }
              case DefaultItemLayout::SPIRAL:
              {
                Internal::SpiralLayoutPtr spiralLayout = Internal::SpiralLayout::New();
                (*spiralLayout).SetLayoutProperties(*layout);
                (*spiralLayout).SetSpiralLayoutProperties(*layout);
                AddLayout(*spiralLayout);
                break;
              }
            }
          }
        }
      }
    }
  }
}

Property::Value ItemView::GetProperty(BaseObject* object, Property::Index index)
{
  Property::Value value;

  Toolkit::ItemView itemView = Toolkit::ItemView::DownCast(Dali::BaseHandle(object));

  if(itemView)
  {
    ItemView& itemViewImpl(GetImpl(itemView));
    switch(index)
    {
      case Toolkit::ItemView::Property::MINIMUM_SWIPE_SPEED:
      {
        value = itemViewImpl.GetMinimumSwipeSpeed();
        break;
      }

      case Toolkit::ItemView::Property::MINIMUM_SWIPE_DISTANCE:
      {
        value = itemViewImpl.GetMinimumSwipeDistance();
        break;
      }

      case Toolkit::ItemView::Property::WHEEL_SCROLL_DISTANCE_STEP:
      {
        value = itemViewImpl.GetWheelScrollDistanceStep();
        break;
      }

      case Toolkit::ItemView::Property::SNAP_TO_ITEM_ENABLED:
      {
        value = itemViewImpl.GetAnchoring();
        break;
      }

      case Toolkit::ItemView::Property::REFRESH_INTERVAL:
      {
        value = itemViewImpl.GetRefreshInterval();
        break;
      }

      case Toolkit::ItemView::Property::LAYOUT:
      {
        value = itemViewImpl.GetLayoutArray();
        break;
      }
    }
  }

  return value;
}

bool ItemView::DoAction(BaseObject* object, const std::string& actionName, const Property::Map& attributes)
{
  Dali::BaseHandle handle(object);

  Toolkit::ItemView itemView = Toolkit::ItemView::DownCast(handle);

  DALI_ASSERT_ALWAYS(itemView);

  if(0 == strcmp(actionName.c_str(), ACTION_STOP_SCROLLING))
  {
    GetImpl(itemView).DoStopScrolling();
  }
  else if(0 == strcmp(actionName.c_str(), ACTION_ENABLE_REFRESH))
  {
    GetImpl(itemView).SetRefreshNotificationEnabled(true);
  }
  else if(0 == strcmp(actionName.c_str(), ACTION_DISABLE_REFRESH))
  {
    GetImpl(itemView).SetRefreshNotificationEnabled(false);
  }

  return true;
}

void ItemView::DoStopScrolling()
{
  if(mScrollAnimation)
  {
    mScrollAnimation.Stop();
    mScrollAnimation.Reset();
  }
}

void ItemView::SetRefreshNotificationEnabled(bool enabled)
{
  mRefreshNotificationEnabled = enabled;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
