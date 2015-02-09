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

// CLASS HEADER
#include <dali-toolkit/internal/controls/scrollable/item-view/item-view-impl.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/set-wrapper.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/events/mouse-wheel-event.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-factory.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-connector-impl.h>
#include <dali-toolkit/internal/controls/scrollable/bouncing-effect-actor.h>

using std::string;
using std::set;
using namespace Dali;

namespace // unnamed namespace
{

//Type registration
TypeRegistration mType( typeid(Toolkit::ItemView), typeid(Toolkit::Scrollable), NULL );

const float DEFAULT_MINIMUM_SWIPE_SPEED = 1.0f;
const float DEFAULT_MINIMUM_SWIPE_DISTANCE = 3.0f;
const float DEFAULT_MOUSE_WHEEL_SCROLL_DISTANCE_STEP_PROPORTION = 0.1f;

const float DEFAULT_MINIMUM_SWIPE_DURATION = 0.45f;
const float DEFAULT_MAXIMUM_SWIPE_DURATION = 2.6f;

const float DEFAULT_REFRESH_INTERVAL_LAYOUT_POSITIONS = 20.0f; // 1 updates per 20 items
const int MOUSE_WHEEL_EVENT_FINISHED_TIME_OUT = 500;  // 0.5 second

const float DEFAULT_ANCHORING_DURATION = 1.0f;  // 1 second

const float MILLISECONDS_PER_SECONDS = 1000.0f;

const Vector2 OVERSHOOT_BOUNCE_ACTOR_DEFAULT_SIZE( 720.0f, 42.0f );
const float OVERSHOOT_BOUNCE_ACTOR_RESIZE_THRESHOLD = 180.0f;
const Vector4 OVERSHOOT_OVERLAY_NINE_PATCH_BORDER(0.0f, 0.0f, 1.0f, 12.0f);
const float DEFAULT_KEYBOARD_FOCUS_SCROLL_DURATION = 0.2f;

const string LAYOUT_POSITION_PROPERTY_NAME( "item-view-layout-position" );
const string POSITION_PROPERTY_NAME( "item-view-position" );
const string MINIMUM_LAYOUT_POSITION_PROPERTY_NAME( "item-view-minimum-layout-position" );
const string SCROLL_SPEED_PROPERTY_NAME( "item-view-scroll-speed" );
const string SCROLL_DIRECTION_PROPERTY_NAME( "item-view-scroll-direction" );
const string OVERSHOOT_PROPERTY_NAME( "item-view-overshoot" );

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
  Vector3 operator()(const Vector3& current,
                     const PropertyInput& parentScrollDirectionProperty,
                     const PropertyInput& parentOvershootProperty,
                     const PropertyInput& parentSizeProperty)
  {
    const Vector3 parentScrollDirection = parentScrollDirectionProperty.GetVector3();
    const Vector3 parentSize = parentSizeProperty.GetVector3();
    const Toolkit::ControlOrientation::Type& parentOrientation = static_cast<Toolkit::ControlOrientation::Type>(parentScrollDirection.z);

    float overlayWidth;

    if(Toolkit::IsVertical(parentOrientation))
    {
      overlayWidth = fabsf(parentScrollDirection.y) > Math::MACHINE_EPSILON_1 ? parentSize.x : parentSize.y;
    }
    else
    {
      overlayWidth = fabsf(parentScrollDirection.x) > Math::MACHINE_EPSILON_1 ? parentSize.y : parentSize.x;
    }

    float overlayHeight = (overlayWidth > OVERSHOOT_BOUNCE_ACTOR_RESIZE_THRESHOLD) ? OVERSHOOT_BOUNCE_ACTOR_DEFAULT_SIZE.height : OVERSHOOT_BOUNCE_ACTOR_DEFAULT_SIZE.height*0.5f;

    return Vector3( overlayWidth, overlayHeight, current.depth );
  }
};

struct OvershootOverlayRotationConstraint
{
  Quaternion operator()(const Quaternion& current,
                        const PropertyInput& parentScrollDirectionProperty,
                        const PropertyInput& parentOvershootProperty)
  {
    const Vector3 parentScrollDirection = parentScrollDirectionProperty.GetVector3();
    const float parentOvershoot = parentOvershootProperty.GetFloat();
    const Toolkit::ControlOrientation::Type& parentOrientation = static_cast<Toolkit::ControlOrientation::Type>(parentScrollDirection.z);

    Quaternion rotation;

    if(Toolkit::IsVertical(parentOrientation))
    {
      if(fabsf(parentScrollDirection.y) <= Math::MACHINE_EPSILON_1)
      {
        if( (parentOrientation == Toolkit::ControlOrientation::Up && parentOvershoot < Math::MACHINE_EPSILON_0)
            || (parentOrientation == Toolkit::ControlOrientation::Down && parentOvershoot > Math::MACHINE_EPSILON_0) )
        {
          rotation = Quaternion(0.5f * Math::PI, Vector3::ZAXIS);
        }
        else
        {
          rotation = Quaternion(1.5f * Math::PI, Vector3::ZAXIS);
        }
      }
      else if( (parentOvershoot > Math::MACHINE_EPSILON_0 && parentScrollDirection.y > Math::MACHINE_EPSILON_0)
            || (parentOvershoot < Math::MACHINE_EPSILON_0 && parentScrollDirection.y < Math::MACHINE_EPSILON_0) )
      {
        rotation = Quaternion(0.0f, Vector3::ZAXIS);
      }
      else
      {
        rotation = Quaternion(Math::PI, Vector3::ZAXIS);
      }
    }
    else
    {
      if(fabsf(parentScrollDirection.x) <= Math::MACHINE_EPSILON_1)
      {
        if( (parentOrientation == Toolkit::ControlOrientation::Left && parentOvershoot > Math::MACHINE_EPSILON_0)
            ||(parentOrientation == Toolkit::ControlOrientation::Right && parentOvershoot < Math::MACHINE_EPSILON_0) )
        {
          rotation = Quaternion(Math::PI, Vector3::ZAXIS);
        }
        else
        {
          rotation = Quaternion(0.0f, Vector3::ZAXIS);
        }
      }
      else if( (parentOvershoot > Math::MACHINE_EPSILON_0 && parentScrollDirection.x > Math::MACHINE_EPSILON_0)
            || (parentOvershoot < Math::MACHINE_EPSILON_0 && parentScrollDirection.x < Math::MACHINE_EPSILON_0) )
      {
        rotation = Quaternion(1.5f * Math::PI, Vector3::ZAXIS);
      }
      else
      {
        rotation = Quaternion(0.5f * Math::PI, Vector3::ZAXIS);
      }
    }

    return rotation;
  }
};

struct OvershootOverlayPositionConstraint
{
  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& parentSizeProperty,
                     const PropertyInput& parentScrollDirectionProperty,
                     const PropertyInput& parentOvershootProperty)
  {
    const Vector3 parentScrollDirection = parentScrollDirectionProperty.GetVector3();
    const float parentOvershoot = parentOvershootProperty.GetFloat();
    const Vector3 parentSize = parentSizeProperty.GetVector3();
    const Toolkit::ControlOrientation::Type& parentOrientation = static_cast<Toolkit::ControlOrientation::Type>(parentScrollDirection.z);

    Vector3 relativeOffset;

    if(Toolkit::IsVertical(parentOrientation))
    {
      if(fabsf(parentScrollDirection.y) <= Math::MACHINE_EPSILON_1)
      {
        if( (parentOrientation == Toolkit::ControlOrientation::Up && parentOvershoot < Math::MACHINE_EPSILON_0)
            || (parentOrientation == Toolkit::ControlOrientation::Down && parentOvershoot > Math::MACHINE_EPSILON_0) )
        {
          relativeOffset = Vector3(1.0f, 0.0f, 0.0f);
        }
        else
        {
          relativeOffset =Vector3(0.0f, 1.0f, 0.0f);
        }
      }
      else if( (parentOvershoot > Math::MACHINE_EPSILON_0 && parentScrollDirection.y > Math::MACHINE_EPSILON_0)
            || (parentOvershoot < Math::MACHINE_EPSILON_0 && parentScrollDirection.y < Math::MACHINE_EPSILON_0) )
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
        if( (parentOrientation == Toolkit::ControlOrientation::Left && parentOvershoot < Math::MACHINE_EPSILON_0)
            || (parentOrientation == Toolkit::ControlOrientation::Right && parentOvershoot > Math::MACHINE_EPSILON_0) )
        {
          relativeOffset = Vector3(0.0f, 0.0f, 0.0f);
        }
        else
        {
          relativeOffset = Vector3(1.0f, 1.0f, 0.0f);
        }
      }
      else if( (parentOvershoot > Math::MACHINE_EPSILON_0 && parentScrollDirection.x > Math::MACHINE_EPSILON_0)
            || (parentOvershoot < Math::MACHINE_EPSILON_0 && parentScrollDirection.x < Math::MACHINE_EPSILON_0) )
      {
        relativeOffset = Vector3(0.0f, 1.0f, 0.0f);
      }
      else
      {
        relativeOffset = Vector3(1.0f, 0.0f, 0.0f);
      }
    }

    return relativeOffset * parentSize;

  }
};

struct OvershootOverlayVisibilityConstraint
{
  bool operator()(const bool& current,
                  const PropertyInput& parentLayoutScrollableProperty)
  {
    const bool parentLayoutScrollable = parentLayoutScrollableProperty.GetBoolean();

    return parentLayoutScrollable;
  }
};

/**
 * Relative position Constraint
 * Generates the relative position value of the item view based on the layout position,
 * and it's relation to the layout domain. This is a value from 0.0f to 1.0f in each axis.
 */
Vector3 RelativePositionConstraint(const Vector3& current,
                                   const PropertyInput& scrollPositionProperty,
                                   const PropertyInput& scrollMinProperty,
                                   const PropertyInput& scrollMaxProperty,
                                   const PropertyInput& layoutSizeProperty)
{
  const Vector3& position = Vector3(0.0f, scrollPositionProperty.GetFloat(), 0.0f);
  const Vector3& min = scrollMinProperty.GetVector3();
  const Vector3& max = scrollMaxProperty.GetVector3();

  Vector3 relativePosition;
  Vector3 domainSize = max - min;

  relativePosition.x = fabsf(domainSize.x) > Math::MACHINE_EPSILON_1 ? ((min.x - position.x) / fabsf(domainSize.x)) : 0.0f;
  relativePosition.y = fabsf(domainSize.y) > Math::MACHINE_EPSILON_1 ? ((min.y - position.y) / fabsf(domainSize.y)) : 0.0f;

  return relativePosition;
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

bool FindById( const ItemContainer& items, ItemId id )
{
  for( ConstItemIter iter = items.begin(); items.end() != iter; ++iter )
  {
    if( iter->first == id )
    {
      return true;
    }
  }

  return false;
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
: Scrollable(),
  mItemFactory(factory),
  mActiveLayout(NULL),
  mAnimatingOvershootOn(false),
  mAnimateOvershootOff(false),
  mAnchoringEnabled(true),
  mAnchoringDuration(DEFAULT_ANCHORING_DURATION),
  mRefreshIntervalLayoutPositions(0.0f),
  mRefreshOrderHint(true/*Refresh item 0 first*/),
  mMinimumSwipeSpeed(DEFAULT_MINIMUM_SWIPE_SPEED),
  mMinimumSwipeDistance(DEFAULT_MINIMUM_SWIPE_DISTANCE),
  mMouseWheelScrollDistanceStep(0.0f),
  mScrollDistance(0.0f),
  mScrollSpeed(0.0f),
  mTotalPanDisplacement(Vector2::ZERO),
  mScrollOvershoot(0.0f),
  mIsFlicking(false),
  mGestureState(Gesture::Clear),
  mAddingItems(false),
  mPropertyPosition(Property::INVALID_INDEX),
  mPropertyMinimumLayoutPosition(Property::INVALID_INDEX),
  mPropertyScrollSpeed(Property::INVALID_INDEX),
  mRefreshEnabled(true),
  mItemsParentOrigin( ParentOrigin::CENTER),
  mItemsAnchorPoint( AnchorPoint::CENTER)
{
  SetRequiresMouseWheelEvents(true);
  SetKeyboardNavigationSupport(true);
}

void ItemView::OnInitialize()
{
  SetSizePolicy( Toolkit::Control::Fixed, Toolkit::Control::Fixed );

  RegisterCommonProperties();

  Actor self = Self();

  mScrollConnector = Dali::Toolkit::ScrollConnector::New();
  mScrollPositionObject = mScrollConnector.GetScrollPositionObject();
  mScrollConnector.ScrollPositionChangedSignal().Connect( this, &ItemView::OnScrollPositionChanged );

  mPropertyMinimumLayoutPosition = self.RegisterProperty(MINIMUM_LAYOUT_POSITION_PROPERTY_NAME, 0.0f);
  mPropertyPosition = self.RegisterProperty(POSITION_PROPERTY_NAME, 0.0f);
  mPropertyScrollSpeed = self.RegisterProperty(SCROLL_SPEED_PROPERTY_NAME, 0.0f);

  EnableScrollComponent(Toolkit::Scrollable::OvershootIndicator);

  Constraint constraint = Constraint::New<Vector3>(mPropertyRelativePosition,
                                                   LocalSource(mPropertyPosition),
                                                   LocalSource(mPropertyPositionMin),
                                                   LocalSource(mPropertyPositionMax),
                                                   LocalSource(Actor::SIZE),
                                                   RelativePositionConstraint);
  self.ApplyConstraint(constraint);

  Vector2 stageSize = Stage::GetCurrent().GetSize();
  mMouseWheelScrollDistanceStep = stageSize.y * DEFAULT_MOUSE_WHEEL_SCROLL_DISTANCE_STEP_PROPORTION;

  EnableGestureDetection(Gesture::Type(Gesture::Pan));

  mMouseWheelEventFinishedTimer = Timer::New( MOUSE_WHEEL_EVENT_FINISHED_TIME_OUT );
  mMouseWheelEventFinishedTimer.TickSignal().Connect( this, &ItemView::OnMouseWheelEventFinished );

  SetRefreshInterval(DEFAULT_REFRESH_INTERVAL_LAYOUT_POSITIONS);
}

ItemView::~ItemView()
{
}

Dali::Toolkit::ScrollConnector ItemView::GetScrollConnector() const
{
  return mScrollConnector;
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

  if (mActiveLayout == mLayouts[layoutIndex].Get())
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
  return mScrollConnector.GetScrollPosition() + static_cast<float>( itemId );
}

void ItemView::ActivateLayout(unsigned int layoutIndex, const Vector3& targetSize, float durationSeconds)
{
  DALI_ASSERT_ALWAYS(layoutIndex < mLayouts.size());

  mRefreshEnabled = false;

  Actor self = Self();

  // The ItemView size should match the active layout size
  self.SetSize(targetSize);
  mActiveLayoutTargetSize = targetSize;

  // Switch to the new layout
  mActiveLayout = mLayouts[layoutIndex].Get();

  // Move the items to the new layout positions...

  bool resizeAnimationNeeded(false);
  for (ConstItemPoolIter iter = mItemPool.begin(); iter != mItemPool.end(); ++iter)
  {
    unsigned int itemId = iter->first;
    Actor actor = iter->second;

    // Remove constraints from previous layout
    actor.RemoveConstraints();

    Vector3 size;
    if(mActiveLayout->GetItemSize(itemId, targetSize, size))
    {
      if( durationSeconds > 0.0f )
      {
        // Use a size animation
        if (!resizeAnimationNeeded)
        {
          resizeAnimationNeeded = true;
          RemoveAnimation(mResizeAnimation);
          mResizeAnimation = Animation::New(durationSeconds);
        }

        // The layout provides its own resize animation
        mActiveLayout->GetResizeAnimation(mResizeAnimation, actor, size, durationSeconds);
      }
      else
      {
        // resize immediately
        actor.SetSize(size);
      }
    }

    mActiveLayout->ApplyConstraints(actor, itemId, durationSeconds, mScrollPositionObject, Self() );
  }

  if (resizeAnimationNeeded)
  {
    mResizeAnimation.Play();
  }

  // Refresh the new layout
  ItemRange range = GetItemRange(*mActiveLayout, targetSize, GetCurrentLayoutPosition(0), false/* don't reserve extra*/);
  AddActorsWithinRange( range, durationSeconds );

  // Scroll to an appropriate layout position

  bool scrollAnimationNeeded(false);
  float firstItemScrollPosition(0.0f);

  float current = GetCurrentLayoutPosition(0);
  float minimum = ClampFirstItemPosition(current, targetSize, *mActiveLayout);
  self.SetProperty(mPropertyPosition, GetScrollPosition(current, targetSize));

  if (current < minimum)
  {
    scrollAnimationNeeded = true;
    firstItemScrollPosition = minimum;
  }
  else if (mAnchoringEnabled)
  {
    scrollAnimationNeeded = true;
    firstItemScrollPosition = mActiveLayout->GetClosestAnchorPosition(current);
  }

  if (scrollAnimationNeeded)
  {
    RemoveAnimation(mScrollAnimation);
    mScrollAnimation = Animation::New(durationSeconds);
    mScrollAnimation.AnimateTo( Property( mScrollPositionObject, ScrollConnector::SCROLL_POSITION ), firstItemScrollPosition, AlphaFunctions::EaseOut );
    mScrollAnimation.AnimateTo( Property(self, mPropertyPosition), GetScrollPosition(firstItemScrollPosition, targetSize), AlphaFunctions::EaseOut );
    mScrollAnimation.FinishedSignal().Connect(this, &ItemView::OnLayoutActivationScrollFinished);
    mScrollAnimation.Play();
  }

  self.SetProperty(mPropertyMinimumLayoutPosition, mActiveLayout->GetMinimumLayoutPosition(mItemFactory.GetNumberOfItems(), targetSize));
  AnimateScrollOvershoot(0.0f);
  mScrollOvershoot = 0.0f;

  Radian scrollDirection(mActiveLayout->GetScrollDirection());
  float orientation = static_cast<float>(mActiveLayout->GetOrientation());
  self.SetProperty(mPropertyScrollDirection, Vector3(sinf(scrollDirection), cosf(scrollDirection), orientation));

  self.SetProperty(mPropertyScrollSpeed, mScrollSpeed);

  CalculateDomainSize(targetSize);
}

void ItemView::DeactivateCurrentLayout()
{
  if (mActiveLayout)
  {
    for (ConstItemPoolIter iter = mItemPool.begin(); iter != mItemPool.end(); ++iter)
    {
      Actor actor = iter->second;
      actor.RemoveConstraints();
    }

    mActiveLayout = NULL;
  }
}

void ItemView::OnRefreshNotification(PropertyNotification& source)
{
  if(mRefreshEnabled || mScrollAnimation)
  {
    // Only refresh the cache during normal scrolling
    DoRefresh(GetCurrentLayoutPosition(0), true);
  }
}

void ItemView::DoRefresh(float currentLayoutPosition, bool cacheExtra)
{
  if (mActiveLayout)
  {
    ItemRange range = GetItemRange(*mActiveLayout, mActiveLayoutTargetSize, currentLayoutPosition, cacheExtra/*reserve extra*/);
    RemoveActorsOutsideRange( range );
    AddActorsWithinRange( range, 0.0f/*immediate*/ );

    mScrollUpdatedSignal.Emit( Vector3(0.0f, currentLayoutPosition, 0.0f) );
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

void ItemView::SetMouseWheelScrollDistanceStep(float step)
{
  mMouseWheelScrollDistanceStep = step;
}

float ItemView::GetMouseWheelScrollDistanceStep() const
{
  return mMouseWheelScrollDistanceStep;
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
  if( !Equals(mRefreshIntervalLayoutPositions, intervalLayoutPositions) )
  {
    mRefreshIntervalLayoutPositions = intervalLayoutPositions;

    if(mRefreshNotification)
    {
      mScrollPositionObject.RemovePropertyNotification(mRefreshNotification);
    }
    mRefreshNotification = mScrollPositionObject.AddPropertyNotification( ScrollConnector::SCROLL_POSITION, StepCondition(mRefreshIntervalLayoutPositions, 0.0f) );
    mRefreshNotification.NotifySignal().Connect( this, &ItemView::OnRefreshNotification );
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

  ConstItemPoolIter iter = mItemPool.find( itemId );
  if( iter != mItemPool.end() )
  {
    actor = iter->second;
  }

  return actor;
}

unsigned int ItemView::GetItemId( Actor actor ) const
{
  unsigned int itemId( 0 );

  for ( ConstItemPoolIter iter = mItemPool.begin(); iter != mItemPool.end(); ++iter )
  {
    if( iter->second == actor )
    {
      itemId = iter->first;
      break;
    }
  }

  return itemId;
}

void ItemView::InsertItem( Item newItem, float durationSeconds )
{
  mAddingItems = true;

  Actor displacedActor;
  ItemPoolIter afterDisplacedIter = mItemPool.end();

  ItemPoolIter foundIter = mItemPool.find( newItem.first );
  if( mItemPool.end() != foundIter )
  {
    SetupActor( newItem, durationSeconds );
    Self().Add( newItem.second );

    displacedActor = foundIter->second;
    foundIter->second = newItem.second;

    afterDisplacedIter = ++foundIter;
  }
  else
  {
    // Inserting before the existing item range?
    ItemPoolIter iter = mItemPool.begin();
    if( iter != mItemPool.end() &&
        iter->first > newItem.first )
    {
      displacedActor = iter->second;
      mItemPool.erase( iter++ ); // iter is still valid after the erase

      afterDisplacedIter = iter;
    }
  }

  if( displacedActor )
  {
    // Move the existing actors to make room
    for( ItemPoolIter iter = afterDisplacedIter; mItemPool.end() != iter; ++iter )
    {
      Actor temp = iter->second;
      iter->second = displacedActor;
      displacedActor = temp;

      iter->second.RemoveConstraints();
      mActiveLayout->ApplyConstraints( iter->second, iter->first, durationSeconds, mScrollPositionObject, Self() );
    }

    // Create last item
    ItemPool::reverse_iterator lastIter = mItemPool.rbegin();
    if ( lastIter != mItemPool.rend() )
    {
      ItemId lastId = lastIter->first;
      Item lastItem( lastId + 1, displacedActor );
      mItemPool.insert( lastItem );

      lastItem.second.RemoveConstraints();
      mActiveLayout->ApplyConstraints( lastItem.second, lastItem.first, durationSeconds, mScrollPositionObject, Self() );
    }
  }

  CalculateDomainSize(Self().GetCurrentSize());

  mAddingItems = false;
}

void ItemView::InsertItems( const ItemContainer& newItems, float durationSeconds )
{
  mAddingItems = true;

  // Insert from lowest id to highest
  std::set<Item> sortedItems;
  for( ConstItemIter iter = newItems.begin(); newItems.end() != iter; ++iter )
  {
    sortedItems.insert( *iter );
  }

  for( std::set<Item>::iterator iter = sortedItems.begin(); sortedItems.end() != iter; ++iter )
  {
    Self().Add( iter->second );

    ItemPoolIter foundIter = mItemPool.find( iter->first );
    if( mItemPool.end() != foundIter )
    {
      Actor moveMe = foundIter->second;
      foundIter->second = iter->second;

      // Move the existing actors to make room
      for( ItemPoolIter iter = ++foundIter; mItemPool.end() != iter; ++iter )
      {
        Actor temp = iter->second;
        iter->second = moveMe;
        moveMe = temp;
      }

      // Create last item
      ItemId lastId = mItemPool.rbegin()->first;
      Item lastItem( lastId + 1, moveMe );
      mItemPool.insert( lastItem );
    }
    else
    {
      mItemPool.insert( *iter );
    }
  }

  // Relayout everything
  for (ItemPoolIter iter = mItemPool.begin(); iter != mItemPool.end(); ++iter)
  {
    // If newly inserted
    if( FindById( newItems, iter->first ) )
    {
      SetupActor( *iter, durationSeconds );
    }
    else
    {
      iter->second.RemoveConstraints();
      mActiveLayout->ApplyConstraints( iter->second, iter->first, durationSeconds, mScrollPositionObject, Self() );
    }
  }

  CalculateDomainSize(Self().GetCurrentSize());

  mAddingItems = false;
}

void ItemView::RemoveItem( unsigned int itemId, float durationSeconds )
{
  bool actorsReordered = RemoveActor( itemId );
  if( actorsReordered )
  {
    ReapplyAllConstraints( durationSeconds );

    OnItemsRemoved();
  }
}

void ItemView::RemoveItems( const ItemIdContainer& itemIds, float durationSeconds )
{
  bool actorsReordered( false );

  // Remove from highest id to lowest
  set<ItemId> sortedItems;
  for( ConstItemIdIter iter = itemIds.begin(); itemIds.end() != iter; ++iter )
  {
    sortedItems.insert( *iter );
  }

  for( set<ItemId>::reverse_iterator iter = sortedItems.rbegin(); sortedItems.rend() != iter; ++iter )
  {
    if( RemoveActor( *iter ) )
    {
      actorsReordered = true;
    }
  }

  if( actorsReordered )
  {
    ReapplyAllConstraints( durationSeconds );

    OnItemsRemoved();
  }
}

bool ItemView::RemoveActor(unsigned int itemId)
{
  bool reordered( false );

  ItemPoolIter removeIter = mItemPool.find( itemId );
  if( removeIter != mItemPool.end() )
  {
    ReleaseActor(itemId, removeIter->second);
  }
  else
  {
    // Removing before the existing item range?
    ItemPoolIter iter = mItemPool.begin();
    if( iter != mItemPool.end() &&
        iter->first > itemId )
    {
      // In order to decrement the first visible item ID
      mItemPool.insert( Item(iter->first - 1, Actor()) );

      removeIter = mItemPool.begin();
    }
  }

  if( removeIter != mItemPool.end() )
  {
    reordered = true;

    // Adjust the remaining item IDs, for example if item 2 is removed:
    //   Initial actors:     After insert:
    //     ID 1 - ActorA       ID 1 - ActorA
    //     ID 2 - ActorB       ID 2 - ActorC (previously ID 3)
    //     ID 3 - ActorC       ID 3 - ActorB (previously ID 4)
    //     ID 4 - ActorD
    for (ItemPoolIter iter = removeIter; iter != mItemPool.end(); ++iter)
    {
      if( iter->first < mItemPool.rbegin()->first )
      {
        iter->second = mItemPool[ iter->first + 1 ];
      }
      else
      {
        mItemPool.erase( iter );
        break;
      }
    }
  }

  return reordered;
}

void ItemView::ReplaceItem( Item replacementItem, float durationSeconds )
{
  mAddingItems = true;

  SetupActor( replacementItem, durationSeconds );
  Self().Add( replacementItem.second );

  const ItemPoolIter iter = mItemPool.find( replacementItem.first );
  if( mItemPool.end() != iter )
  {
    ReleaseActor(iter->first, iter->second);
    iter->second = replacementItem.second;
  }
  else
  {
    mItemPool.insert( replacementItem );
  }

  CalculateDomainSize(Self().GetCurrentSize());

  mAddingItems = false;
}

void ItemView::ReplaceItems( const ItemContainer& replacementItems, float durationSeconds )
{
  for( ConstItemIter iter = replacementItems.begin(); replacementItems.end() != iter; ++iter )
  {
    ReplaceItem( *iter, durationSeconds );
  }
}

void ItemView::RemoveActorsOutsideRange( ItemRange range )
{
  // Remove unwanted actors from the ItemView & ItemPool
  for (ItemPoolIter iter = mItemPool.begin(); iter != mItemPool.end(); )
  {
    unsigned int current = iter->first;

    if( ! range.Within( current ) )
    {
      ReleaseActor(iter->first, iter->second);

      mItemPool.erase( iter++ ); // erase invalidates the return value of post-increment; iter remains valid
    }
    else
    {
      ++iter;
    }
  }
}

void ItemView::AddActorsWithinRange( ItemRange range, float durationSeconds )
{
  range.end = std::min(mItemFactory.GetNumberOfItems(), range.end);

  // The order of addition depends on the scroll direction.
  if (mRefreshOrderHint)
  {
    for (unsigned int itemId = range.begin; itemId < range.end; ++itemId)
    {
      AddNewActor( itemId, durationSeconds );
    }
  }
  else
  {
    for (unsigned int itemId = range.end; itemId > range.begin; --itemId)
    {
      AddNewActor( itemId-1, durationSeconds );
    }
  }

  // Total number of items may change dynamically.
  // Always recalculate the domain size to reflect that.
  CalculateDomainSize(Self().GetCurrentSize());
}

void ItemView::AddNewActor( unsigned int itemId, float durationSeconds )
{
  mAddingItems = true;

  if( mItemPool.end() == mItemPool.find( itemId ) )
  {
    Actor actor = mItemFactory.NewItem( itemId );

    if( actor )
    {
      Item newItem( itemId, actor );

      mItemPool.insert( newItem );

      SetupActor( newItem, durationSeconds );
      Self().Add( actor );
    }
  }

  mAddingItems = false;
}

void ItemView::SetupActor( Item item, float durationSeconds )
{
  item.second.SetParentOrigin( mItemsParentOrigin );
  item.second.SetAnchorPoint( mItemsAnchorPoint );

  if( mActiveLayout )
  {
    Vector3 size;
    if( mActiveLayout->GetItemSize( item.first, mActiveLayoutTargetSize, size ) )
    {
      item.second.SetSize( size );
    }

    mActiveLayout->ApplyConstraints( item.second, item.first, durationSeconds, mScrollPositionObject, Self() );
  }
}

void ItemView::ReleaseActor( ItemId item, Actor actor )
{
  Self().Remove( actor );
  mItemFactory.ItemReleased(item, actor);
}

ItemRange ItemView::GetItemRange(ItemLayout& layout, const Vector3& layoutSize, float layoutPosition, bool reserveExtra)
{
  unsigned int itemCount = mItemFactory.GetNumberOfItems();

  ItemRange available(0u, itemCount);

  ItemRange range = layout.GetItemsWithinArea( layoutPosition, layoutSize );

  if (reserveExtra)
  {
    // Add the reserve items for scrolling
    unsigned int extra = layout.GetReserveItemCount(layoutSize);
    range.begin = (range.begin >= extra) ? (range.begin - extra) : 0u;
    range.end += extra;
  }

  return range.Intersection(available);
}

void ItemView::OnChildAdd(Actor& child)
{
  if(!mAddingItems)
  {
    // We don't want to do this downcast check for any item added by ItemView itself.
    Dali::Toolkit::ScrollComponent scrollComponent = Dali::Toolkit::ScrollComponent::DownCast(child);
    if(scrollComponent)
    {
      // Set the scroll connector when scroll bar is being added
      scrollComponent.SetScrollConnector(mScrollConnector);
    }
  }
}

bool ItemView::OnTouchEvent(const TouchEvent& event)
{
  // Ignore events with multiple-touch points
  if (event.GetPointCount() != 1)
  {
    return false;
  }

  if (event.GetPoint(0).state == TouchPoint::Down)
  {
    // Cancel ongoing scrolling etc.
    mGestureState = Gesture::Clear;

    mScrollDistance = 0.0f;
    mScrollSpeed = 0.0f;
    Self().SetProperty(mPropertyScrollSpeed, mScrollSpeed);

    mScrollOvershoot = 0.0f;
    AnimateScrollOvershoot(0.0f);

    if(mScrollAnimation)
    {
      mScrollCompletedSignal.Emit(GetCurrentScrollPosition());
    }

    RemoveAnimation(mScrollAnimation);
  }

  return true; // consume since we're potentially scrolling
}

bool ItemView::OnMouseWheelEvent(const MouseWheelEvent& event)
{
  // Respond the mouse wheel event to scroll
  if (mActiveLayout)
  {
    Actor self = Self();
    const Vector3 layoutSize = Self().GetCurrentSize();
    float layoutPositionDelta = GetCurrentLayoutPosition(0) - (event.z * mMouseWheelScrollDistanceStep * mActiveLayout->GetScrollSpeedFactor());
    float firstItemScrollPosition = ClampFirstItemPosition(layoutPositionDelta, layoutSize, *mActiveLayout);

    mScrollPositionObject.SetProperty( ScrollConnector::SCROLL_POSITION, firstItemScrollPosition );
    self.SetProperty(mPropertyPosition, GetScrollPosition(firstItemScrollPosition, layoutSize));
    mScrollStartedSignal.Emit(GetCurrentScrollPosition());
    mRefreshEnabled = true;
  }

  if (mMouseWheelEventFinishedTimer.IsRunning())
  {
    mMouseWheelEventFinishedTimer.Stop();
  }

  mMouseWheelEventFinishedTimer.Start();

  return true;
}

bool ItemView::OnMouseWheelEventFinished()
{
  if (mActiveLayout)
  {
    RemoveAnimation(mScrollAnimation);

    // No more mouse wheel events coming. Do the anchoring if enabled.
    mScrollAnimation = DoAnchoring();
    if (mScrollAnimation)
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

void ItemView::ReapplyAllConstraints( float durationSeconds )
{
  for (ConstItemPoolIter iter = mItemPool.begin(); iter != mItemPool.end(); ++iter)
  {
    unsigned int id = iter->first;
    Actor actor = iter->second;

    actor.RemoveConstraints();
    mActiveLayout->ApplyConstraints(actor, id, durationSeconds, mScrollPositionObject, Self());
  }
}

void ItemView::OnItemsRemoved()
{
  CalculateDomainSize(Self().GetCurrentSize());

  // Adjust scroll-position after an item is removed
  if( mActiveLayout )
  {
    float firstItemScrollPosition = ClampFirstItemPosition(GetCurrentLayoutPosition(0), Self().GetCurrentSize(), *mActiveLayout);

    mScrollPositionObject.SetProperty( ScrollConnector::SCROLL_POSITION, firstItemScrollPosition );
  }
}

float ItemView::ClampFirstItemPosition(float targetPosition, const Vector3& targetSize, ItemLayout& layout)
{
  Actor self = Self();
  float minLayoutPosition = layout.GetMinimumLayoutPosition(mItemFactory.GetNumberOfItems(), targetSize);
  float clamppedPosition = std::min(0.0f, std::max(minLayoutPosition, targetPosition));
  mScrollOvershoot = targetPosition - clamppedPosition;
  self.SetProperty(mPropertyMinimumLayoutPosition, minLayoutPosition);

  return clamppedPosition;
}

void ItemView::OnPan( const PanGesture& gesture )
{
  Actor self = Self();
  const Vector3 layoutSize = Self().GetCurrentSize();

  RemoveAnimation(mScrollAnimation);

  // Short-circuit if there is no active layout
  if (!mActiveLayout)
  {
    mGestureState = Gesture::Clear;
    return;
  }

  mGestureState = gesture.state;

  switch (mGestureState)
  {
    case Gesture::Finished:
    {
      // Swipe Detection
      if (fabsf(mScrollDistance) > mMinimumSwipeDistance &&
          mScrollSpeed > mMinimumSwipeSpeed)
      {
        float direction = (mScrollDistance < 0.0f) ? -1.0f : 1.0f;

        mRefreshOrderHint = true;

        float currentLayoutPosition = GetCurrentLayoutPosition(0);
        float firstItemScrollPosition = ClampFirstItemPosition(currentLayoutPosition + mScrollSpeed * direction,
                                                               layoutSize,
                                                               *mActiveLayout);

        if (mAnchoringEnabled)
        {
          firstItemScrollPosition = mActiveLayout->GetClosestAnchorPosition(firstItemScrollPosition);
        }

        RemoveAnimation(mScrollAnimation);

        float flickAnimationDuration = Clamp( mActiveLayout->GetItemFlickAnimationDuration() * std::max(1.0f, fabsf(firstItemScrollPosition - GetCurrentLayoutPosition(0)))
                                       , DEFAULT_MINIMUM_SWIPE_DURATION, DEFAULT_MAXIMUM_SWIPE_DURATION);

        mScrollAnimation = Animation::New(flickAnimationDuration);
        mScrollAnimation.AnimateTo( Property( mScrollPositionObject, ScrollConnector::SCROLL_POSITION ), firstItemScrollPosition, AlphaFunctions::EaseOut );
        mScrollAnimation.AnimateTo( Property(self, mPropertyPosition), GetScrollPosition(firstItemScrollPosition, layoutSize), AlphaFunctions::EaseOut );
        mScrollAnimation.AnimateTo( Property(self, mPropertyScrollSpeed), 0.0f, AlphaFunctions::EaseOut );

        mIsFlicking = true;
        // Check whether it has already scrolled to the end
        if(fabs(currentLayoutPosition - firstItemScrollPosition) > Math::MACHINE_EPSILON_0)
        {
          AnimateScrollOvershoot(0.0f);
        }
      }

      // Anchoring may be triggered when there was no swipe
      if (!mScrollAnimation)
      {
        mScrollAnimation = DoAnchoring();
      }

      // Reset the overshoot if no scroll animation.
      if (!mScrollAnimation)
      {
        mScrollCompletedSignal.Emit(GetCurrentScrollPosition());

        AnimateScrollOvershoot(0.0f, false);
      }
    }
    break;

    case Gesture::Started: // Fall through
    {
      mTotalPanDisplacement = Vector2::ZERO;
      mScrollStartedSignal.Emit(GetCurrentScrollPosition());
      mRefreshEnabled = true;
    }

    case Gesture::Continuing:
    {
      mScrollDistance = CalculateScrollDistance(gesture.displacement, *mActiveLayout);
      mScrollSpeed = Clamp((gesture.GetSpeed() * gesture.GetSpeed() * mActiveLayout->GetFlickSpeedFactor() * MILLISECONDS_PER_SECONDS), 0.0f, mActiveLayout->GetMaximumSwipeSpeed());

      // Refresh order depends on the direction of the scroll; negative is towards the last item.
      mRefreshOrderHint = mScrollDistance < 0.0f;

      float layoutPositionDelta = GetCurrentLayoutPosition(0) + (mScrollDistance * mActiveLayout->GetScrollSpeedFactor());

      float firstItemScrollPosition = ClampFirstItemPosition(layoutPositionDelta, layoutSize, *mActiveLayout);

      float currentOvershoot = mScrollPositionObject.GetProperty<float>(ScrollConnector::OVERSHOOT);

      mScrollPositionObject.SetProperty( ScrollConnector::SCROLL_POSITION, firstItemScrollPosition );
      self.SetProperty(mPropertyPosition, GetScrollPosition(firstItemScrollPosition, layoutSize));

      if( (firstItemScrollPosition >= 0.0f && currentOvershoot < 1.0f) || (firstItemScrollPosition <= mActiveLayout->GetMinimumLayoutPosition(mItemFactory.GetNumberOfItems(), layoutSize) && currentOvershoot > -1.0f) )
      {
        mTotalPanDisplacement += gesture.displacement;
      }

      mScrollOvershoot = CalculateScrollOvershoot();
      mScrollPositionObject.SetProperty( ScrollConnector::OVERSHOOT, mScrollOvershoot );
    }
    break;

    case Gesture::Cancelled:
    {
      mScrollAnimation = DoAnchoring();
    }
    break;

    default:
      break;
  }

  if (mScrollAnimation)
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

Actor ItemView::GetNextKeyboardFocusableActor(Actor actor, Toolkit::Control::KeyboardFocusNavigationDirection direction, bool loopEnabled)
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
      int itemID = GetItemId(actor);
      nextItemID = mActiveLayout->GetNextFocusItemID(itemID, mItemFactory.GetNumberOfItems(), direction, loopEnabled);
      nextFocusActor = GetItem(nextItemID);
      if(nextFocusActor == actor)
      {
        // need to pass NULL actor back to focus manager
        nextFocusActor.Reset();
        return nextFocusActor;
      }
    }
    float layoutPosition = mActiveLayout->GetClosestAnchorPosition( GetCurrentLayoutPosition(0) );
    Vector3 layoutSize = Self().GetCurrentSize();
    if(!nextFocusActor)
    {
      // likely the current item is not buffered, so not in our item pool, probably best to get first viewable item
      ItemRange viewableItems = mActiveLayout->GetItemsWithinArea(layoutPosition, layoutSize);
      nextItemID = viewableItems.begin;
      nextFocusActor = GetItem(nextItemID);
    }
  }
  return nextFocusActor;
}

void ItemView::OnKeyboardFocusChangeCommitted(Actor commitedFocusableActor)
{
  // only in this function if our chosen focus actor was actually used
  if(commitedFocusableActor)
  {
    int nextItemID = GetItemId(commitedFocusableActor);
    float layoutPosition = GetCurrentLayoutPosition(0);
    Vector3 layoutSize = Self().GetCurrentSize();

    float scrollTo = mActiveLayout->GetClosestOnScreenLayoutPosition(nextItemID, layoutPosition, layoutSize);
    ScrollTo(Vector3(0.0f, scrollTo, 0.0f), DEFAULT_KEYBOARD_FOCUS_SCROLL_DURATION);
  }
}

Animation ItemView::DoAnchoring()
{
  Animation anchoringAnimation;
  Actor self = Self();

  if (mActiveLayout && mAnchoringEnabled)
  {
    float anchorPosition = mActiveLayout->GetClosestAnchorPosition( GetCurrentLayoutPosition(0) );

    anchoringAnimation = Animation::New(mAnchoringDuration);
    anchoringAnimation.AnimateTo( Property( mScrollPositionObject, ScrollConnector::SCROLL_POSITION ), anchorPosition, AlphaFunctions::EaseOut );
    anchoringAnimation.AnimateTo( Property(self, mPropertyPosition), GetScrollPosition(anchorPosition, self.GetCurrentSize()), AlphaFunctions::EaseOut );
    anchoringAnimation.AnimateTo( Property(self, mPropertyScrollSpeed), 0.0f, AlphaFunctions::EaseOut );
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
    AnimateScrollOvershoot( mScrollOvershoot > 0.0f ? 1.0f : -1.0f, true);
  }
  else
  {
    // Reset the overshoot
    AnimateScrollOvershoot( 0.0f );
  }
  mIsFlicking = false;

  mScrollOvershoot = 0.0f;
}

void ItemView::OnLayoutActivationScrollFinished(Animation& source)
{
  RemoveAnimation(mScrollAnimation);
  mRefreshEnabled = true;
  DoRefresh(GetCurrentLayoutPosition(0), true);
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
}

void ItemView::ScrollToItem(unsigned int itemId, float durationSeconds)
{
  Actor self = Self();
  const Vector3 layoutSize = Self().GetCurrentSize();
  float firstItemScrollPosition = ClampFirstItemPosition(mActiveLayout->GetItemScrollToPosition(itemId), layoutSize, *mActiveLayout);

  if(durationSeconds > 0.0f)
  {
    RemoveAnimation(mScrollAnimation);
    mScrollAnimation = Animation::New(durationSeconds);
    mScrollAnimation.AnimateTo( Property( mScrollPositionObject, ScrollConnector::SCROLL_POSITION ), firstItemScrollPosition, AlphaFunctions::EaseOut );
    mScrollAnimation.AnimateTo( Property(self, mPropertyPosition), GetScrollPosition(firstItemScrollPosition, layoutSize), AlphaFunctions::EaseOut );
    mScrollAnimation.FinishedSignal().Connect(this, &ItemView::OnScrollFinished);
    mScrollAnimation.Play();
  }
  else
  {
    mScrollPositionObject.SetProperty( ScrollConnector::SCROLL_POSITION, firstItemScrollPosition );
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
    firstItemPosition = mActiveLayout->GetItemPosition( 0,0,layoutSize );

    float minLayoutPosition = mActiveLayout->GetMinimumLayoutPosition(mItemFactory.GetNumberOfItems(), layoutSize);
    self.SetProperty(mPropertyMinimumLayoutPosition, minLayoutPosition);
    lastItemPosition = mActiveLayout->GetItemPosition( fabs(minLayoutPosition),fabs(minLayoutPosition),layoutSize );

    float domainSize;

    if(IsHorizontal(mActiveLayout->GetOrientation()))
    {
      self.SetProperty(mPropertyPositionMin, Vector3(0.0f, firstItemPosition.x, 0.0f));
      self.SetProperty(mPropertyPositionMax, Vector3(0.0f, lastItemPosition.x, 0.0f));
      domainSize = fabs(firstItemPosition.x - lastItemPosition.x);
    }
    else
    {
      self.SetProperty(mPropertyPositionMin, Vector3(0.0f, firstItemPosition.y, 0.0f));
      self.SetProperty(mPropertyPositionMax, Vector3(0.0f, lastItemPosition.y, 0.0f));
      domainSize = fabs(firstItemPosition.y - lastItemPosition.y);
    }

    mScrollConnector.SetScrollDomain(minLayoutPosition, 0.0f, domainSize);

    bool isLayoutScrollable = IsLayoutScrollable(layoutSize);
    self.SetProperty(mPropertyCanScrollVertical, isLayoutScrollable);
    self.SetProperty(mPropertyCanScrollHorizontal, false);
  }
}

Vector3 ItemView::GetDomainSize() const
{
  Actor self = Self();

  float minScrollPosition = self.GetProperty<float>(mPropertyPositionMin);
  float maxScrollPosition = self.GetProperty<float>(mPropertyPositionMax);

  return Vector3(0.0f, fabs(maxScrollPosition - minScrollPosition), 0.0f);
}

bool ItemView::IsLayoutScrollable(const Vector3& layoutSize)
{
  Actor self = Self();

  float currentLayoutPosition = ClampFirstItemPosition( GetCurrentLayoutPosition(0), layoutSize, *mActiveLayout );
  float forwardClampedPosition = ClampFirstItemPosition(currentLayoutPosition + 1.0, layoutSize, *mActiveLayout);
  float backwardClampedPosition = ClampFirstItemPosition(currentLayoutPosition - 1.0, layoutSize, *mActiveLayout);

  return (fabs(forwardClampedPosition - backwardClampedPosition) > Math::MACHINE_EPSILON_0);
}

float ItemView::GetScrollPosition(float layoutPosition, const Vector3& layoutSize) const
{
  Vector3 firstItemPosition( mActiveLayout->GetItemPosition(0, layoutPosition, layoutSize ) );
  return IsHorizontal(mActiveLayout->GetOrientation()) ? firstItemPosition.x: firstItemPosition.y;
}

Vector3 ItemView::GetCurrentScrollPosition() const
{
  float currentLayoutPosition = GetCurrentLayoutPosition(0);
  return Vector3(0.0f, GetScrollPosition(currentLayoutPosition, Self().GetCurrentSize()), 0.0f);
}

void ItemView::AddOverlay(Actor actor)
{
  Self().Add(actor);
}

void ItemView::RemoveOverlay(Actor actor)
{
  Self().Remove(actor);
}

void ItemView::ScrollTo(const Vector3& position, float duration)
{
  Actor self = Self();
  const Vector3 layoutSize = Self().GetCurrentSize();

  float firstItemScrollPosition = ClampFirstItemPosition(position.y, layoutSize, *mActiveLayout);

  if(duration > 0.0f)
  {
    RemoveAnimation(mScrollAnimation);
    mScrollAnimation = Animation::New(duration);
    mScrollAnimation.AnimateTo( Property( mScrollPositionObject, ScrollConnector::SCROLL_POSITION ), firstItemScrollPosition, AlphaFunctions::EaseOut );
    mScrollAnimation.AnimateTo( Property(self, mPropertyPosition), GetScrollPosition(firstItemScrollPosition, layoutSize), AlphaFunctions::EaseOut );
    mScrollAnimation.FinishedSignal().Connect(this, &ItemView::OnScrollFinished);
    mScrollAnimation.Play();
  }
  else
  {
    mScrollPositionObject.SetProperty( ScrollConnector::SCROLL_POSITION, firstItemScrollPosition );
    AnimateScrollOvershoot(0.0f);
  }

  mScrollStartedSignal.Emit(GetCurrentScrollPosition());
  mRefreshEnabled = true;
}

void ItemView::SetOvershootEffectColor( const Vector4& color )
{
  mOvershootEffectColor = color;
  if( mOvershootOverlay )
  {
    mOvershootOverlay.SetColor( color );
  }
}

void ItemView::SetOvershootEnabled( bool enable )
{
  Actor self = Self();
  if( enable )
  {
    Property::Index effectOvershootPropertyIndex = Property::INVALID_INDEX;
    mOvershootOverlay = CreateBouncingEffectActor( effectOvershootPropertyIndex );
    mOvershootOverlay.SetColor(mOvershootEffectColor);
    mOvershootOverlay.SetParentOrigin(ParentOrigin::TOP_LEFT);
    mOvershootOverlay.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    mOvershootOverlay.SetDrawMode(DrawMode::OVERLAY);
    self.Add(mOvershootOverlay);

    Constraint constraint = Constraint::New<Vector3>( Actor::SIZE,
                                                      ParentSource( mPropertyScrollDirection ),
                                                      Source( mScrollPositionObject, ScrollConnector::OVERSHOOT ),
                                                      ParentSource( Actor::SIZE ),
                                                      OvershootOverlaySizeConstraint() );
    mOvershootOverlay.ApplyConstraint(constraint);
    mOvershootOverlay.SetSize(OVERSHOOT_BOUNCE_ACTOR_DEFAULT_SIZE.width, OVERSHOOT_BOUNCE_ACTOR_DEFAULT_SIZE.height);

    constraint = Constraint::New<Quaternion>( Actor::ROTATION,
                                              ParentSource( mPropertyScrollDirection ),
                                              Source( mScrollPositionObject, ScrollConnector::OVERSHOOT ),
                                              OvershootOverlayRotationConstraint() );
    mOvershootOverlay.ApplyConstraint(constraint);

    constraint = Constraint::New<Vector3>( Actor::POSITION,
                                           ParentSource( Actor::SIZE ),
                                           ParentSource( mPropertyScrollDirection ),
                                           Source( mScrollPositionObject, ScrollConnector::OVERSHOOT ),
                                           OvershootOverlayPositionConstraint() );
    mOvershootOverlay.ApplyConstraint(constraint);

    constraint = Constraint::New<bool>( Actor::VISIBLE,
                                        ParentSource( mPropertyCanScrollVertical ),
                                        OvershootOverlayVisibilityConstraint() );
    mOvershootOverlay.ApplyConstraint(constraint);

    constraint = Constraint::New<float>( effectOvershootPropertyIndex,
                                         Source( mScrollPositionObject, ScrollConnector::OVERSHOOT ),
                                         EqualToConstraint() );
    mOvershootOverlay.ApplyConstraint(constraint);
  }
  else
  {
    if( mOvershootOverlay )
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
    Actor self = Self();
    float scrollDistance = CalculateScrollDistance(mTotalPanDisplacement, *mActiveLayout) * mActiveLayout->GetScrollSpeedFactor();
    float positionDelta = GetCurrentLayoutPosition(0) + scrollDistance;
    float minLayoutPosition = mActiveLayout->GetMinimumLayoutPosition(mItemFactory.GetNumberOfItems(), Self().GetCurrentSize());
    self.SetProperty(mPropertyMinimumLayoutPosition, minLayoutPosition);
    float clamppedPosition = std::min(0.0f, std::max(minLayoutPosition, positionDelta));
    overshoot = positionDelta - clamppedPosition;
  }

  return overshoot > 0.0f ? std::min(overshoot, 1.0f) : std::max(overshoot, -1.0f);
}

void ItemView::AnimateScrollOvershoot(float overshootAmount, bool animateBack)
{
  bool animatingOn = fabsf(overshootAmount) > Math::MACHINE_EPSILON_1;

  // make sure we animate back if needed
  mAnimateOvershootOff = animateBack || (!animatingOn && mAnimatingOvershootOn);

  if( mAnimatingOvershootOn )
  {
    // animating on, do not allow animate off
    return;
  }

  if(mOvershootAnimationSpeed > Math::MACHINE_EPSILON_0)
  {
    float currentOvershoot = mScrollPositionObject.GetProperty<float>(ScrollConnector::OVERSHOOT);
    float duration = mOvershootOverlay.GetCurrentSize().height * (animatingOn ? (1.0f - fabsf(currentOvershoot)) : fabsf(currentOvershoot)) / mOvershootAnimationSpeed;

    RemoveAnimation(mScrollOvershootAnimation);
    mScrollOvershootAnimation = Animation::New(duration);
    mScrollOvershootAnimation.FinishedSignal().Connect(this, &ItemView::OnOvershootOnFinished);
    mScrollOvershootAnimation.AnimateTo( Property(mScrollPositionObject, ScrollConnector::OVERSHOOT), overshootAmount, TimePeriod(0.0f, duration) );
    mScrollOvershootAnimation.Play();

    mAnimatingOvershootOn = animatingOn;
  }
  else
  {
    mScrollPositionObject.SetProperty( ScrollConnector::OVERSHOOT, overshootAmount );
  }
}

void ItemView::SetItemsParentOrigin( const Vector3& parentOrigin )
{
  if( parentOrigin != mItemsParentOrigin )
  {
    mItemsParentOrigin = parentOrigin;
    for (ItemPoolIter iter = mItemPool.begin(); iter != mItemPool.end(); ++iter)
    {
      iter->second.SetParentOrigin(parentOrigin);
    }
  }
}

Vector3 ItemView::GetItemsParentOrigin() const
{
  return mItemsParentOrigin;
}

void ItemView::SetItemsAnchorPoint( const Vector3& anchorPoint )
{
  if( anchorPoint != mItemsAnchorPoint )
  {
    mItemsAnchorPoint = anchorPoint;
    for (ItemPoolIter iter = mItemPool.begin(); iter != mItemPool.end(); ++iter)
    {
      iter->second.SetAnchorPoint(anchorPoint);
    }
  }
}

Vector3 ItemView::GetItemsAnchorPoint() const
{
  return mItemsAnchorPoint;
}

void ItemView::GetItemsRange(ItemRange& range)
{
  if( !mItemPool.empty() )
  {
    range.begin = mItemPool.begin()->first;
    range.end = mItemPool.rbegin()->first + 1;
  }
  else
  {
    range.begin = 0;
    range.end = 0;
  }
}

void ItemView::OnScrollPositionChanged( float position )
{
  // Cancel scroll animation to prevent any fighting of setting the scroll position property.
  RemoveAnimation(mScrollAnimation);

  // Refresh the cache immediately when the scroll position is changed.
  DoRefresh(position, false); // No need to cache extra items.
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
