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

// CLASS HEADER
#include <dali-toolkit/internal/controls/scrollable/item-view/item-view-impl.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/public-api/events/mouse-wheel-event.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-factory.h>

using namespace std;
using namespace Dali;

namespace // unnamed namespace
{

//Type registration
TypeRegistration mType( typeid(Toolkit::ItemView), typeid(Toolkit::Scrollable), NULL );

const float DEFAULT_MINIMUM_SWIPE_SPEED = 1.0f;
const float DEFAULT_MINIMUM_SWIPE_DISTANCE = 3.0f;
const float DEFAULT_MOUSE_WHEEL_SCROLL_DISTANCE_STEP_PROPORTION = 0.1f;

const int DEFAULT_REFRESH_INTERVAL_MILLISECONDS = 50; // 20 updates per second
const int MOUSE_WHEEL_EVENT_FINISHED_TIME_OUT = 500;  // 0.5 second

const float DEFAULT_ANCHORING_DURATION = 1.0f;  // 1 second
const float DEFAULT_COLOR_VISIBILITY_REMOVE_TIME = 0.5f; // 0.5 second

const float MILLISECONDS_PER_SECONDS = 1000.0f;

const char* OVERSHOOT_OVERLAY_RIPPLE_IMAGE_PATH = DALI_IMAGE_DIR "overshoot_ripple.png";
const Rect<int> OVERSHOOT_BOUNCE_IMAGE_1_PIXEL_AREA( 0, 0, 720, 58 );
const Vector4 OVERSHOOT_OVERLAY_NINE_PATCH_BORDER(0.0f, 0.0f, 1.0f, 12.0f);
const float MAXIMUM_OVERSHOOT_HEIGHT = 36.0f;  // 36 pixels
const float DEFAULT_OVERSHOOT_ANIMATION_DURATION = 0.5f;  // 0.5 second
const float DEFAULT_KEYBOARD_FOCUS_SCROLL_DURATION = 0.2f;

const string LAYOUT_POSITION_PROPERTY_NAME( "item-view-layout-position" );
const string POSITION_PROPERTY_NAME( "item-view-position" );
const string MINIMUM_LAYOUT_POSITION_PROPERTY_NAME( "item-view-minimum-layout-position" );
const string SCROLL_SPEED_PROPERTY_NAME( "item-view-scroll-speed" );
const string SCROLL_DIRECTION_PROPERTY_NAME( "item-view-scroll-direction" );
const string OVERSHOOT_PROPERTY_NAME( "item-view-overshoot" );

// Functors which wrap constraint functions with stored item IDs

struct WrappedVector3Constraint
{
  WrappedVector3Constraint(Toolkit::ItemLayout::Vector3Function wrapMe, unsigned int itemId)
  : mWrapMe(wrapMe),
    mItemId(itemId)
  {
  }

  Vector3 operator()(const Vector3& current, const PropertyInput& layoutPosition, const PropertyInput& scrollSpeed, const PropertyInput& layoutSize)
  {
    float offsetLayoutPosition = layoutPosition.GetFloat() + static_cast<float>(mItemId);

    return mWrapMe(current, offsetLayoutPosition, scrollSpeed.GetFloat(), layoutSize.GetVector3());
  }

  Toolkit::ItemLayout::Vector3Function mWrapMe;
  unsigned int mItemId;
};

struct WrappedQuaternionConstraint
{
  WrappedQuaternionConstraint(Toolkit::ItemLayout::QuaternionFunction wrapMe, unsigned int itemId)
  : mWrapMe(wrapMe),
    mItemId(itemId)
  {
  }

  Quaternion operator()(const Quaternion& current, const PropertyInput& layoutPosition, const PropertyInput& scrollSpeed, const PropertyInput& layoutSize)
  {
    float offsetLayoutPosition = layoutPosition.GetFloat() + static_cast<float>(mItemId);

    return mWrapMe(current, offsetLayoutPosition, scrollSpeed.GetFloat(), layoutSize.GetVector3());
  }

  Toolkit::ItemLayout::QuaternionFunction mWrapMe;
  unsigned int mItemId;
};

struct WrappedVector4Constraint
{
  WrappedVector4Constraint(Toolkit::ItemLayout::Vector4Function wrapMe, unsigned int itemId)
  : mWrapMe(wrapMe),
    mItemId(itemId)
  {
  }

  Vector4 operator()(const Vector4& current, const PropertyInput& layoutPosition, const PropertyInput& scrollSpeed, const PropertyInput& layoutSize)
  {
    float offsetLayoutPosition = layoutPosition.GetFloat() + static_cast<float>(mItemId);

    return mWrapMe(current, offsetLayoutPosition, scrollSpeed.GetFloat(), layoutSize.GetVector3());
  }

  Toolkit::ItemLayout::Vector4Function mWrapMe;
  unsigned int mItemId;
};

struct WrappedBoolConstraint
{
  WrappedBoolConstraint(Toolkit::ItemLayout::BoolFunction wrapMe, unsigned int itemId)
  : mWrapMe(wrapMe),
    mItemId(itemId)
  {
  }

  bool operator()(const bool& current, const PropertyInput& layoutPosition, const PropertyInput& scrollSpeed, const PropertyInput& layoutSize)
  {
    float offsetLayoutPosition = layoutPosition.GetFloat() + static_cast<float>(mItemId);

    return mWrapMe(current, offsetLayoutPosition, scrollSpeed.GetFloat(), layoutSize.GetVector3());
  }

  Toolkit::ItemLayout::BoolFunction mWrapMe;
  unsigned int mItemId;
};

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
  float operator()(const float& current,
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

    return overlayWidth;
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

///////////////////////////////////////////////////////////////////////////////////////////////////
// ItemPool
///////////////////////////////////////////////////////////////////////////////////////////////////

void ItemPool::AddItems(bool scrollingTowardsLast, ItemRange range, const Vector3& targetSize)
{
  // Add new actors to the ItemPool.
  // The order of addition depends on the scroll direction.
  if (scrollingTowardsLast)
  {
    for (unsigned int itemId = range.begin; itemId < range.end; ++itemId)
    {
      AddItem(itemId, targetSize);
    }
  }
  else
  {
    for (unsigned int itemId = range.end; itemId > range.begin; --itemId)
    {
      AddItem(itemId-1, targetSize);
    }
  }
}

void ItemPool::RemoveItems(ItemRange range)
{
  // Remove unwanted actors from the ItemView & ItemPool
  for (IDKeyIter iter = mIdKeyContainer.begin(); iter != mIdKeyContainer.end(); )
  {
    unsigned int current = iter->first;

    if (!range.Within(current))
    {
      mItemView.ActorRemovedFromItemPool(iter->second, iter->first);

      mActorKeyContainer.erase(iter->second);
      mIdKeyContainer.erase(iter++); // erase invalidates the current iter; the post-increment is important here
    }
    else
    {
      ++iter;
    }
  }
}

void ItemPool::AddItem(unsigned int itemId, const Vector3& targetSize)
{
  if (mIdKeyContainer.find(itemId) == mIdKeyContainer.end())
  {
    Actor actor = mItemView.CreateActor(itemId);

    if (actor)
    {
      mIdKeyContainer.insert(IDKeyPair(itemId, actor));
      mActorKeyContainer.insert(ActorKeyPair(actor, itemId));

      mItemView.ActorAddedToItemPool(actor, itemId, targetSize);
    }
  }
}

bool ItemPool::RemoveItem(unsigned int itemId)
{
  bool found = false;

  IDKeyIter iter = mIdKeyContainer.find(itemId);
  if (iter != mIdKeyContainer.end())
  {
    mItemView.ActorRemovedFromItemPool(iter->second, iter->first);

    mActorKeyContainer.erase(iter->second);
    for (ActorKeyIter iterActorKey = mActorKeyContainer.begin(); iterActorKey != mActorKeyContainer.end(); ++iterActorKey)
    {
      if(iterActorKey->second > itemId)
      {
        iterActorKey->second--;
      }
    }

    for (IDKeyIter iterIDKey = iter; iterIDKey != mIdKeyContainer.end(); ++iterIDKey)
    {
      if(iterIDKey->first < mIdKeyContainer.rbegin()->first)
      {
        iterIDKey->second = mIdKeyContainer[iterIDKey->first + 1];
      }
      else
      {
        mIdKeyContainer.erase(iterIDKey);
        break;
      }
    }

    found = true;
  }

  return found;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ItemView
///////////////////////////////////////////////////////////////////////////////////////////////////

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
  mItemPool(*this),
  mActiveLayout(NULL),
  mAnimatingOvershootOn(false),
  mAnimateOvershootOff(false),
  mAnchoringEnabled(true),
  mAnchoringDuration(DEFAULT_ANCHORING_DURATION),
  mRefreshIntervalMilliseconds(DEFAULT_REFRESH_INTERVAL_MILLISECONDS),
  mRefreshOrderHint(true/*Refresh item 0 first*/),
  mMinimumSwipeSpeed(DEFAULT_MINIMUM_SWIPE_SPEED),
  mMinimumSwipeDistance(DEFAULT_MINIMUM_SWIPE_DISTANCE),
  mScrollDistance(0.0f),
  mScrollSpeed(0.0f),
  mTotalPanDisplacement(Vector2::ZERO),
  mScrollOvershoot(0.0f),
  mIsFlicking(false),
  mGestureState(Gesture::Clear)
{
  SetRequiresMouseWheelEvents(true);
  SetKeyboardNavigationSupport(true);
}

void ItemView::OnInitialize()
{
  SetSizePolicy( Toolkit::Control::Fixed, Toolkit::Control::Fixed );

  RegisterCommonProperties();

  Actor self = Self();

  mOvershootEffect = OvershootRippleEffect::New();
  Image overshootImage = Image::New( OVERSHOOT_OVERLAY_RIPPLE_IMAGE_PATH );
  mOvershootOverlay = ImageActor::New( overshootImage );
  mOvershootOverlay.SetParentOrigin(ParentOrigin::TOP_LEFT);
  mOvershootOverlay.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  mOvershootOverlay.SetDrawMode(DrawMode::OVERLAY);
  mOvershootOverlay.SetShaderEffect(mOvershootEffect);
  mOvershootOverlay.SetPixelArea(OVERSHOOT_BOUNCE_IMAGE_1_PIXEL_AREA);
  self.Add(mOvershootOverlay);

  mPropertyLayoutPosition = self.RegisterProperty(LAYOUT_POSITION_PROPERTY_NAME, 0.0f);
  mPropertyMinimumLayoutPosition = self.RegisterProperty(MINIMUM_LAYOUT_POSITION_PROPERTY_NAME, 0.0f);
  mPropertyPosition = self.RegisterProperty(POSITION_PROPERTY_NAME, 0.0f);
  mPropertyScrollSpeed = self.RegisterProperty(SCROLL_SPEED_PROPERTY_NAME, 0.0f);
  mPropertyOvershoot = self.RegisterProperty(OVERSHOOT_PROPERTY_NAME, 0.0f);

  ApplyOvershootOverlayConstraints();

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
  return Self().GetProperty<float>(mPropertyLayoutPosition) + static_cast<float>(itemId);
}

void ItemView::ActivateLayout(unsigned int layoutIndex, const Vector3& targetSize, float durationSeconds)
{
  DALI_ASSERT_ALWAYS(layoutIndex < mLayouts.size());

  Actor self = Self();

  // The ItemView size should match the active layout size
  self.SetSize(targetSize);

  // Switch to the new layout
  ItemLayout* previousLayout = mActiveLayout;
  mActiveLayout = mLayouts[layoutIndex].Get();

  // Calculate which items are within either layout
  ItemRange oldRange = previousLayout ? GetItemRange(*previousLayout, targetSize, false/*don't reserve extra*/) : ItemRange(0u, 0u);
  ItemRange newRange = GetItemRange(*mActiveLayout, targetSize, false/*don't reserve extra*/);

  // Move the items to the new layout positions...

  bool resizeAnimationNeeded(false);

  const ItemPool::IDKeyContainer& itemPool = mItemPool.GetIDKeyContainer();
  for (ItemPool::IDKeyConstIter iter = itemPool.begin(); iter != itemPool.end(); ++iter)
  {
    unsigned int itemId = iter->first;
    Actor actor = iter->second;

    // Immediately relayout items that aren't within either layout
    bool immediate = !oldRange.Within(itemId) &&
                     !newRange.Within(itemId);

    // Remove constraints from previous layout
    actor.RemoveConstraints();

    Vector3 size;
    if(mActiveLayout->GetItemSize(itemId, targetSize, size))
    {
      if (!immediate &&
          durationSeconds > 0.0f)
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

    ApplyConstraints(actor, *mActiveLayout, itemId, immediate ? 0.0f : durationSeconds);
  }

  if (resizeAnimationNeeded)
  {
    mResizeAnimation.Play();
  }

  // Refresh the new layout
  ItemRange range = GetItemRange(*mActiveLayout, targetSize, true/*reserve extra*/);
  AddItems(*mActiveLayout, targetSize, range);

  // Scroll to an appropriate layout position

  bool scrollAnimationNeeded(false);
  float firstItemScrollPosition(0.0f);

  float current = self.GetProperty<float>(mPropertyLayoutPosition);
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
    mScrollAnimation = Animation::New(mAnchoringDuration);
    mScrollAnimation.AnimateTo( Property(self, mPropertyLayoutPosition), firstItemScrollPosition, AlphaFunctions::EaseOut );
    mScrollAnimation.AnimateTo( Property(self, mPropertyPosition), GetScrollPosition(firstItemScrollPosition, targetSize), AlphaFunctions::EaseOut );
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
    const ItemPool::IDKeyContainer& itemPool = mItemPool.GetIDKeyContainer();
    for (ItemPool::IDKeyConstIter iter = itemPool.begin(); iter != itemPool.end(); ++iter)
    {
      Actor actor = iter->second;
      actor.RemoveConstraints();
    }

    mActiveLayout = NULL;
  }

  CancelRefreshTimer();
}

bool ItemView::OnRefreshTick()
{
  // Short-circuit if there is no active layout
  if (!mActiveLayout)
  {
    return false;
  }

  const Vector3 layoutSize = Self().GetCurrentSize();

  ItemRange range = GetItemRange(*mActiveLayout, layoutSize, true/*reserve extra*/);

  RemoveItems(range);
  AddItems(*mActiveLayout, layoutSize, range);

  // Keep refreshing whilst the layout is moving
  return mScrollAnimation || (mGestureState == Gesture::Started || mGestureState == Gesture::Continuing);
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

void ItemView::SetRefreshInterval(unsigned int intervalMilliseconds)
{
  mRefreshIntervalMilliseconds = intervalMilliseconds;
}

unsigned int ItemView::GetRefreshInterval() const
{
  return mRefreshIntervalMilliseconds;
}

Actor ItemView::GetItem(unsigned int itemId) const
{
  Actor actor;

  ItemPool::IDKeyConstIter found = mItemPool.GetIDKeyContainer().find(itemId);
  if (found != mItemPool.GetIDKeyContainer().end())
  {
    actor = found->second;
  }

  return actor;
}

unsigned int ItemView::GetItemId(Actor actor) const
{
  unsigned int itemId(0);

  ItemPool::ActorKeyConstIter found = mItemPool.GetActorKeyContainer().find(actor);
  if (found != mItemPool.GetActorKeyContainer().end())
  {
    itemId = found->second;
  }

  return itemId;
}

void ItemView::RemoveItem(unsigned int itemId, float durationSeconds)
{
  if (mItemPool.RemoveItem(itemId))
  {
    const ItemPool::IDKeyContainer& itemPool = mItemPool.GetIDKeyContainer();
    for (ItemPool::IDKeyConstIter iter = itemPool.begin(); iter != itemPool.end(); ++iter)
    {
      unsigned int id = iter->first;
      Actor actor = iter->second;

      // Reposition the items if necessary
      actor.RemoveConstraints();
      ApplyConstraints(actor, *mActiveLayout, id, durationSeconds);
    }

    CalculateDomainSize(Self().GetCurrentSize());
  }
}

Actor ItemView::CreateActor(unsigned int itemId)
{
  return mItemFactory.NewItem(itemId);
}

void ItemView::ActorAddedToItemPool(Actor actor, unsigned int itemId, const Vector3& targetSize)
{
  Actor self = Self();

  actor.SetParentOrigin(ParentOrigin::CENTER);
  actor.SetAnchorPoint(AnchorPoint::CENTER);

  if (mActiveLayout)
  {
    Vector3 size;
    if(mActiveLayout->GetItemSize(itemId, targetSize, size))
    {
      actor.SetSize(size);
    }

    ApplyConstraints(actor, *mActiveLayout, itemId, 0.0f/*immediate*/);
  }

  self.Add(actor);
}

void ItemView::ActorRemovedFromItemPool(Actor actor, unsigned int itemId)
{
  Self().Remove(actor);
}

void ItemView::RemoveItems(ItemRange range)
{
  mItemPool.RemoveItems(range);
}

void ItemView::AddItems(ItemLayout& layout, const Vector3& layoutSize, ItemRange range)
{
  range.end = min(mItemFactory.GetNumberOfItems(), range.end);

  mItemPool.AddItems(mRefreshOrderHint, range, layoutSize);
}

ItemRange ItemView::GetItemRange(ItemLayout& layout, const Vector3& layoutSize, bool reserveExtra)
{
  unsigned int itemCount = mItemFactory.GetNumberOfItems();

  ItemRange available(0u, itemCount);

  ItemRange range = layout.GetItemsWithinArea(Self().GetProperty<float>(mPropertyLayoutPosition), layoutSize);

  if (reserveExtra)
  {
    // Add the reserve items for scrolling
    unsigned int extra = layout.GetReserveItemCount(layoutSize);
    range.begin = (range.begin >= extra) ? (range.begin - extra) : 0u;
    range.end += extra;
  }

  return range.Intersection(available);
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

    mScrollCompletedSignalV2.Emit(GetCurrentScrollPosition());

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
    float layoutPositionDelta = self.GetProperty<float>(mPropertyLayoutPosition) + (event.z * mMouseWheelScrollDistanceStep * mActiveLayout->GetScrollSpeedFactor());
    float firstItemScrollPosition = ClampFirstItemPosition(layoutPositionDelta, layoutSize, *mActiveLayout);
    self.SetProperty(mPropertyLayoutPosition, firstItemScrollPosition);
    self.SetProperty(mPropertyPosition, GetScrollPosition(firstItemScrollPosition, layoutSize));
    mScrollStartedSignalV2.Emit(GetCurrentScrollPosition());
    StartRefreshTimer();
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
      StartRefreshTimer();

      mScrollAnimation.FinishedSignal().Connect(this, &ItemView::OnScrollFinished);
      mScrollAnimation.Play();
    }
    else
    {
      mScrollOvershoot = 0.0f;
      AnimateScrollOvershoot(0.0f);

      mScrollCompletedSignalV2.Emit(GetCurrentScrollPosition());
    }
  }

  return false;
}

void ItemView::ApplyConstraints(Actor& actor, ItemLayout& layout, unsigned int itemId, float duration)
{
  ItemLayout::Vector3Function positionConstraint;
  if (layout.GetPositionConstraint(itemId, positionConstraint))
  {
    WrappedVector3Constraint wrapped(positionConstraint, itemId);

    Constraint constraint = Constraint::New<Vector3>( Actor::POSITION,
                                                      ParentSource( mPropertyLayoutPosition ),
                                                      ParentSource( mPropertyScrollSpeed ),
                                                      ParentSource( Actor::SIZE ),
                                                      wrapped );
    constraint.SetApplyTime(duration);

    actor.ApplyConstraint(constraint);
  }

  ItemLayout::QuaternionFunction rotationConstraint;
  if (layout.GetRotationConstraint(itemId, rotationConstraint))
  {
    WrappedQuaternionConstraint wrapped(rotationConstraint, itemId);

    Constraint constraint = Constraint::New<Quaternion>( Actor::ROTATION,
                                                         ParentSource( mPropertyLayoutPosition ),
                                                         ParentSource( mPropertyScrollSpeed ),
                                                         ParentSource( Actor::SIZE ),
                                                         wrapped );
    constraint.SetApplyTime(duration);

    actor.ApplyConstraint(constraint);
  }

  ItemLayout::Vector3Function scaleConstraint;
  if (layout.GetScaleConstraint(itemId, scaleConstraint))
  {
    WrappedVector3Constraint wrapped(scaleConstraint, itemId);

    Constraint constraint = Constraint::New<Vector3>( Actor::SCALE,
                                                      ParentSource( mPropertyLayoutPosition ),
                                                      ParentSource( mPropertyScrollSpeed ),
                                                      ParentSource( Actor::SIZE ),
                                                      wrapped );
    constraint.SetApplyTime(duration);

    actor.ApplyConstraint(constraint);
  }

  ItemLayout::Vector4Function colorConstraint;
  if (layout.GetColorConstraint(itemId, colorConstraint))
  {
    WrappedVector4Constraint wrapped(colorConstraint, itemId);

    Constraint constraint = Constraint::New<Vector4>( Actor::COLOR,
                                                      ParentSource( mPropertyLayoutPosition ),
                                                      ParentSource( mPropertyScrollSpeed ),
                                                      ParentSource( Actor::SIZE ),
                                                      wrapped );
    constraint.SetApplyTime(duration);

    // Release color constraints slowly; this allows ItemView to co-exist with ImageActor fade-in
    constraint.SetRemoveTime(DEFAULT_COLOR_VISIBILITY_REMOVE_TIME);
    constraint.SetRemoveAction(Dali::Constraint::Discard);

    actor.ApplyConstraint(constraint);
  }

  ItemLayout::BoolFunction visibilityConstraint;
  if (layout.GetVisibilityConstraint(itemId, visibilityConstraint))
  {
    WrappedBoolConstraint wrapped(visibilityConstraint, itemId);

    Constraint constraint = Constraint::New<bool>( Actor::VISIBLE,
                                                   ParentSource( mPropertyLayoutPosition ),
                                                   ParentSource( mPropertyScrollSpeed ),
                                                   ParentSource( Actor::SIZE ),
                                                   wrapped );
    constraint.SetApplyTime(duration);

    // Release visibility constraints the same time as the color constraint
    constraint.SetRemoveTime(DEFAULT_COLOR_VISIBILITY_REMOVE_TIME);
    constraint.SetRemoveAction(Dali::Constraint::Discard);

    actor.ApplyConstraint(constraint);
  }
}

float ItemView::ClampFirstItemPosition(float targetPosition, const Vector3& targetSize, ItemLayout& layout)
{
  Actor self = Self();
  float minLayoutPosition = layout.GetMinimumLayoutPosition(mItemFactory.GetNumberOfItems(), targetSize);
  float clamppedPosition = min(0.0f, max(minLayoutPosition, targetPosition));
  mScrollOvershoot = targetPosition - clamppedPosition;
  self.SetProperty(mPropertyMinimumLayoutPosition, minLayoutPosition);

  return clamppedPosition;
}

void ItemView::OnPan(PanGesture gesture)
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

        float currentLayoutPosition = self.GetProperty<float>(mPropertyLayoutPosition);
        float firstItemScrollPosition = ClampFirstItemPosition(currentLayoutPosition + mScrollSpeed * direction,
                                                               layoutSize,
                                                               *mActiveLayout);

        if (mAnchoringEnabled)
        {
          firstItemScrollPosition = mActiveLayout->GetClosestAnchorPosition(firstItemScrollPosition);
        }

        RemoveAnimation(mScrollAnimation);

        float flickAnimationDuration = mActiveLayout->GetItemFlickAnimationDuration() * max(1.0f, fabsf(firstItemScrollPosition - GetCurrentLayoutPosition(0)));
        mScrollAnimation = Animation::New(flickAnimationDuration);
        mScrollAnimation.AnimateTo( Property(self, mPropertyLayoutPosition), firstItemScrollPosition, AlphaFunctions::EaseOut );
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
        mScrollCompletedSignalV2.Emit(GetCurrentScrollPosition());

        AnimateScrollOvershoot(0.0f, false);
      }
    }
    break;

    case Gesture::Started: // Fall through
    {
      mTotalPanDisplacement = Vector2::ZERO;
    }

    case Gesture::Continuing:
    {
      mScrollDistance = CalculateScrollDistance(gesture.displacement, *mActiveLayout);
      mScrollSpeed = Clamp((gesture.GetSpeed() * mActiveLayout->GetScrollSpeedFactor() * MILLISECONDS_PER_SECONDS), 0.0f, mActiveLayout->GetMaximumSwipeSpeed());

      // Refresh order depends on the direction of the scroll; negative is towards the last item.
      mRefreshOrderHint = mScrollDistance < 0.0f;

      RemoveAnimation(mScrollSpeedAnimation);
      mScrollSpeedAnimation = Animation::New(0.3f);
      mScrollSpeedAnimation.AnimateTo( Property(self, mPropertyScrollSpeed), mScrollSpeed, AlphaFunctions::Linear );
      mScrollSpeedAnimation.Play();

      float layoutPositionDelta = self.GetProperty<float>(mPropertyLayoutPosition) + (mScrollDistance * mActiveLayout->GetScrollSpeedFactor());

      float firstItemScrollPosition = ClampFirstItemPosition(layoutPositionDelta, layoutSize, *mActiveLayout);

      self.SetProperty(mPropertyLayoutPosition, firstItemScrollPosition);
      self.SetProperty(mPropertyPosition, GetScrollPosition(firstItemScrollPosition, layoutSize));
      mScrollStartedSignalV2.Emit(GetCurrentScrollPosition());

      mTotalPanDisplacement += gesture.displacement;
      mScrollOvershoot = layoutPositionDelta - firstItemScrollPosition;
      if( mScrollOvershoot > Math::MACHINE_EPSILON_1 )
      {
        AnimateScrollOvershoot(1.0f);
      }
      else if( mScrollOvershoot < -Math::MACHINE_EPSILON_1 )
      {
        AnimateScrollOvershoot(-1.0f);
      }
      else
      {
        AnimateScrollOvershoot(0.0f);
      }

      StartRefreshTimer();
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
    StartRefreshTimer();

    mScrollAnimation.FinishedSignal().Connect(this, &ItemView::OnScrollFinished);
    mScrollAnimation.Play();
  }
}

bool ItemView::OnAccessibilityPan(PanGesture gesture)
{
  OnPan(gesture);
  return true;
}

Actor ItemView::GetNextKeyboardFocusableActor(Actor actor, Control::KeyboardFocusNavigationDirection direction, bool loopEnabled)
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
    float layoutPosition = mActiveLayout->GetClosestAnchorPosition(Self().GetProperty<float>(mPropertyLayoutPosition));
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
    float layoutPosition = Self().GetProperty<float>(mPropertyLayoutPosition);
    Vector3 layoutSize = Self().GetCurrentSize();
    Vector3 focusItemPosition = Vector3::ZERO;
    ItemLayout::Vector3Function itemPositionConstraint;
    if (mActiveLayout->GetPositionConstraint(nextItemID, itemPositionConstraint))
    {
      focusItemPosition = itemPositionConstraint(Vector3::ZERO, layoutPosition + nextItemID, 0.0f, layoutSize);
    }

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
    float anchorPosition = mActiveLayout->GetClosestAnchorPosition(Self().GetProperty<float>(mPropertyLayoutPosition));

    anchoringAnimation = Animation::New(mAnchoringDuration);
    anchoringAnimation.AnimateTo( Property(self, mPropertyLayoutPosition), anchorPosition, AlphaFunctions::EaseOut );
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

  mScrollCompletedSignalV2.Emit(GetCurrentScrollPosition());

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

void ItemView::StartRefreshTimer()
{
  if (!mRefreshTimer)
  {
    mRefreshTimer = Timer::New( mRefreshIntervalMilliseconds );
    mRefreshTimer.TickSignal().Connect( this, &ItemView::OnRefreshTick );
  }

  if (!mRefreshTimer.IsRunning())
  {
    mRefreshTimer.Start();
  }
}

void ItemView::CancelRefreshTimer()
{
  if (mRefreshTimer)
  {
    mRefreshTimer.Stop();
  }
}

void ItemView::ScrollToItem(unsigned int itemId, float durationSeconds)
{
  Actor self = Self();
  const Vector3 layoutSize = Self().GetCurrentSize();
  float firstItemScrollPosition = ClampFirstItemPosition(mActiveLayout->GetItemScrollToPosition(itemId), layoutSize, *mActiveLayout);

  StartRefreshTimer();

  if(durationSeconds > 0.0f)
  {
    RemoveAnimation(mScrollAnimation);
    mScrollAnimation = Animation::New(durationSeconds);
    mScrollAnimation.AnimateTo( Property(self, mPropertyLayoutPosition), firstItemScrollPosition, AlphaFunctions::EaseOut );
    mScrollAnimation.AnimateTo( Property(self, mPropertyPosition), GetScrollPosition(firstItemScrollPosition, layoutSize), AlphaFunctions::EaseOut );
    mScrollAnimation.FinishedSignal().Connect(this, &ItemView::OnScrollFinished);
    mScrollAnimation.Play();
  }
  else
  {
    self.SetProperty(mPropertyLayoutPosition, firstItemScrollPosition);
    AnimateScrollOvershoot(0.0f);
  }

  mScrollStartedSignalV2.Emit(GetCurrentScrollPosition());
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
    ItemLayout::Vector3Function firstItemPositionConstraint;
    if (mActiveLayout->GetPositionConstraint(0, firstItemPositionConstraint))
    {
      firstItemPosition = firstItemPositionConstraint(Vector3::ZERO, 0, 0.0f, layoutSize);
    }

    float minLayoutPosition = mActiveLayout->GetMinimumLayoutPosition(mItemFactory.GetNumberOfItems(), layoutSize);
    ItemLayout::Vector3Function lastItemPositionConstraint;
    if (mActiveLayout->GetPositionConstraint(fabs(minLayoutPosition), lastItemPositionConstraint))
    {
      lastItemPosition = lastItemPositionConstraint(Vector3::ZERO, fabs(minLayoutPosition), 0.0f, layoutSize);
    }

    if(IsHorizontal(mActiveLayout->GetOrientation()))
    {
      self.SetProperty(mPropertyPositionMin, Vector3(0.0f, firstItemPosition.x, 0.0f));
      self.SetProperty(mPropertyPositionMax, Vector3(0.0f, lastItemPosition.x, 0.0f));
    }
    else
    {
      self.SetProperty(mPropertyPositionMin, Vector3(0.0f, firstItemPosition.y, 0.0f));
      self.SetProperty(mPropertyPositionMax, Vector3(0.0f, lastItemPosition.y, 0.0f));
    }

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

  float currentLayoutPosition = ClampFirstItemPosition(self.GetProperty<float>(mPropertyLayoutPosition), layoutSize, *mActiveLayout);
  float forwardClampedPosition = ClampFirstItemPosition(currentLayoutPosition + 1.0, layoutSize, *mActiveLayout);
  float backwardClampedPosition = ClampFirstItemPosition(currentLayoutPosition - 1.0, layoutSize, *mActiveLayout);

  return (fabs(forwardClampedPosition - backwardClampedPosition) > Math::MACHINE_EPSILON_0);
}

float ItemView::GetScrollPosition(float layoutPosition, const Vector3& layoutSize) const
{
  Vector3 firstItemPosition(Vector3::ZERO);
  ItemLayout::Vector3Function firstItemPositionConstraint;
  if (mActiveLayout->GetPositionConstraint(0, firstItemPositionConstraint))
  {
    firstItemPosition = firstItemPositionConstraint(Vector3::ZERO, layoutPosition, 0.0f, layoutSize);
  }

  return IsHorizontal(mActiveLayout->GetOrientation()) ? firstItemPosition.x: firstItemPosition.y;
}

Vector3 ItemView::GetCurrentScrollPosition() const
{
  float currentLayoutPosition = Self().GetProperty<float>(mPropertyLayoutPosition);
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

  StartRefreshTimer();

  if(duration > 0.0f)
  {
    RemoveAnimation(mScrollAnimation);
    mScrollAnimation = Animation::New(duration);
    mScrollAnimation.AnimateTo( Property(self, mPropertyLayoutPosition), firstItemScrollPosition, AlphaFunctions::EaseOut );
    mScrollAnimation.AnimateTo( Property(self, mPropertyPosition), GetScrollPosition(firstItemScrollPosition, layoutSize), AlphaFunctions::EaseOut );
    mScrollAnimation.FinishedSignal().Connect(this, &ItemView::OnScrollFinished);
    mScrollAnimation.Play();
  }
  else
  {
    self.SetProperty(mPropertyLayoutPosition, firstItemScrollPosition);
    AnimateScrollOvershoot(0.0f);
  }

  mScrollStartedSignalV2.Emit(GetCurrentScrollPosition());
}

void ItemView::ApplyOvershootOverlayConstraints()
{
  Constraint constraint = Constraint::New<float>( Actor::SIZE_WIDTH,
                                                    ParentSource( mPropertyScrollDirection ),
                                                    ParentSource( mPropertyOvershoot ),
                                                    ParentSource( Actor::SIZE ),
                                                    OvershootOverlaySizeConstraint() );
  mOvershootOverlay.ApplyConstraint(constraint);
  mOvershootOverlay.SetSize(OVERSHOOT_BOUNCE_IMAGE_1_PIXEL_AREA.width, OVERSHOOT_BOUNCE_IMAGE_1_PIXEL_AREA.height);

  constraint = Constraint::New<Quaternion>( Actor::ROTATION,
                                            ParentSource( mPropertyScrollDirection ),
                                            ParentSource( mPropertyOvershoot ),
                                            OvershootOverlayRotationConstraint() );
  mOvershootOverlay.ApplyConstraint(constraint);

  constraint = Constraint::New<Vector3>( Actor::POSITION,
                                         ParentSource( Actor::SIZE ),
                                         ParentSource( mPropertyScrollDirection ),
                                         ParentSource( mPropertyOvershoot ),
                                         OvershootOverlayPositionConstraint() );
  mOvershootOverlay.ApplyConstraint(constraint);

  constraint = Constraint::New<bool>( Actor::VISIBLE,
                                      ParentSource( mPropertyCanScrollVertical ),
                                      OvershootOverlayVisibilityConstraint() );
  mOvershootOverlay.ApplyConstraint(constraint);

  int effectOvershootPropertyIndex = mOvershootEffect.GetPropertyIndex(mOvershootEffect.GetOvershootPropertyName());
  Actor self = Self();
  constraint = Constraint::New<float>( effectOvershootPropertyIndex,
                                       Source( self, mPropertyOvershoot ),
                                       EqualToConstraint() );
  mOvershootEffect.ApplyConstraint(constraint);
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
    float positionDelta = self.GetProperty<float>(mPropertyLayoutPosition) + scrollDistance;
    float minLayoutPosition = mActiveLayout->GetMinimumLayoutPosition(mItemFactory.GetNumberOfItems(), Self().GetCurrentSize());
    self.SetProperty(mPropertyMinimumLayoutPosition, minLayoutPosition);
    float clamppedPosition = min(0.0f, max(minLayoutPosition, positionDelta));
    overshoot = positionDelta - clamppedPosition;
  }

  return overshoot;
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

  Actor self = Self();
  float currentOvershoot = self.GetProperty<float>(mPropertyOvershoot);
  float duration = DEFAULT_OVERSHOOT_ANIMATION_DURATION * (animatingOn ? (1.0f - fabsf(currentOvershoot)) : fabsf(currentOvershoot));

  RemoveAnimation(mScrollOvershootAnimation);
  mScrollOvershootAnimation = Animation::New(duration);
  mScrollOvershootAnimation.FinishedSignal().Connect(this, &ItemView::OnOvershootOnFinished);
  mScrollOvershootAnimation.AnimateTo( Property(self, mPropertyOvershoot), overshootAmount, TimePeriod(0.0f, duration) );
  mScrollOvershootAnimation.Play();

  mAnimatingOvershootOn = animatingOn;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
