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

// CLASS HEADER
#include <dali-toolkit/public-api/controls/scrollable/item-view/spiral-layout.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <dali/public-api/animation/animation.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace // unnamed namespace
{

const float DEFAULT_ITEMS_PER_SPIRAL_TURN = 9.5f;
const float DEFAULT_ITEM_SPACING_RADIANS = Math::PI*2.0f/DEFAULT_ITEMS_PER_SPIRAL_TURN;

const float DEFAULT_REVOLUTION_DISTANCE = 190.0f;
const float DEFAULT_ITEM_DESCENT = DEFAULT_REVOLUTION_DISTANCE / DEFAULT_ITEMS_PER_SPIRAL_TURN;

const float DEFAULT_TOP_ITEM_ALIGNMENT = -0.125f;

const float DEFAULT_SCROLL_SPEED_FACTOR = 0.01f;
const float DEFAULT_MAXIMUM_SWIPE_SPEED = 30.0f;
const float DEFAULT_ITEM_FLICK_ANIMATION_DURATION = 0.1f;

struct DefaultItemSizeFunction
{
  Vector3 operator()(const Vector3& layoutSize)
  {
    float width = layoutSize.width * 0.25f;

    // 4x3 aspect ratio
    return Vector3(width, (width/4)*3, (width/4)*3);
  }
};

struct DefaultSpiralRadiusFunction
{
  float operator()(const Vector3& layoutSize)
  {
    return layoutSize.width*0.4f;
  }
};

struct SpiralPositionConstraintUp
{
  SpiralPositionConstraintUp(SpiralLayout::SpiralRadiusFunction spiralRadius, float itemSpacingRadians, float itemDescent, float topItemAlignment)
  : mSpiralRadius(spiralRadius),
    mItemSpacingRadians(itemSpacingRadians),
    mItemDescent(itemDescent),
    mTopItemAlignment(topItemAlignment)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float spiralRadius = mSpiralRadius(layoutSize);

    float angle = -Math::PI*0.5f + mItemSpacingRadians * layoutPosition;

    return Vector3( -spiralRadius * cosf(angle),
                    (mItemDescent * layoutPosition) + layoutSize.height*mTopItemAlignment,
                    -spiralRadius * sinf(angle) );
  }

  SpiralLayout::SpiralRadiusFunction mSpiralRadius;
  float mItemSpacingRadians;
  float mItemDescent;
  float mTopItemAlignment;
};

struct SpiralPositionConstraintLeft
{
  SpiralPositionConstraintLeft(SpiralLayout::SpiralRadiusFunction spiralRadius, float itemSpacingRadians, float itemDescent, float topItemAlignment)
  : mSpiralRadius(spiralRadius),
    mItemSpacingRadians(itemSpacingRadians),
    mItemDescent(itemDescent),
    mTopItemAlignment(topItemAlignment)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float spiralRadius = mSpiralRadius(layoutSize);

    float angle = Math::PI*0.5f + mItemSpacingRadians * layoutPosition;

    return Vector3(  (mItemDescent * layoutPosition) + layoutSize.width*mTopItemAlignment,
                     -spiralRadius * cosf(angle),
                      spiralRadius * sinf(angle) );
  }

  SpiralLayout::SpiralRadiusFunction mSpiralRadius;
  float mItemSpacingRadians;
  float mItemDescent;
  float mTopItemAlignment;
};

struct SpiralPositionConstraintDown
{
  SpiralPositionConstraintDown(SpiralLayout::SpiralRadiusFunction spiralRadius, float itemSpacingRadians, float itemDescent, float topItemAlignment)
  : mSpiralRadius(spiralRadius),
    mItemSpacingRadians(itemSpacingRadians),
    mItemDescent(itemDescent),
    mTopItemAlignment(topItemAlignment)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float spiralRadius = mSpiralRadius(layoutSize);

    float angle = Math::PI*0.5f + mItemSpacingRadians * layoutPosition;

    return Vector3(  -spiralRadius * cosf(angle),
                    (-mItemDescent * layoutPosition) - layoutSize.height*mTopItemAlignment,
                      spiralRadius * sinf(angle) );
  }

  SpiralLayout::SpiralRadiusFunction mSpiralRadius;
  float mItemSpacingRadians;
  float mItemDescent;
  float mTopItemAlignment;
};

struct SpiralPositionConstraintRight
{
  SpiralPositionConstraintRight(SpiralLayout::SpiralRadiusFunction spiralRadius, float itemSpacingRadians, float itemDescent, float topItemAlignment)
  : mSpiralRadius(spiralRadius),
    mItemSpacingRadians(itemSpacingRadians),
    mItemDescent(itemDescent),
    mTopItemAlignment(topItemAlignment)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float spiralRadius = mSpiralRadius(layoutSize);

    float angle = -Math::PI*0.5f + mItemSpacingRadians * layoutPosition;

    return Vector3( (-mItemDescent * layoutPosition) - layoutSize.width*mTopItemAlignment,
                     -spiralRadius * cosf(angle),
                     -spiralRadius * sinf(angle) );
  }

  SpiralLayout::SpiralRadiusFunction mSpiralRadius;
  float mItemSpacingRadians;
  float mItemDescent;
  float mTopItemAlignment;
};

struct SpiralRotationConstraintUp
{
  SpiralRotationConstraintUp(float itemSpacingRadians)
  : mItemSpacingRadians(itemSpacingRadians)
  {
  }

  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float angle = -mItemSpacingRadians * layoutPosition;

    return Quaternion( Radian( angle ), Vector3::YAXIS);
  }

  float mItemSpacingRadians;
};

struct SpiralRotationConstraintLeft
{
  SpiralRotationConstraintLeft(float itemSpacingRadians)
  : mItemSpacingRadians(itemSpacingRadians)
  {
  }

  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float angle = -mItemSpacingRadians * layoutPosition;

    return Quaternion( Radian( -Math::PI * 0.5f ), Vector3::ZAXIS ) * Quaternion( Radian( angle ), Vector3::YAXIS );
  }

  float mItemSpacingRadians;
};

struct SpiralRotationConstraintDown
{
  SpiralRotationConstraintDown(float itemSpacingRadians)
  : mItemSpacingRadians(itemSpacingRadians)
  {
  }

  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float angle = -mItemSpacingRadians * layoutPosition;

    return Quaternion( Radian( -Math::PI ), Vector3::ZAXIS) * Quaternion( Radian( angle ), Vector3::YAXIS );
  }

  float mItemSpacingRadians;
};

struct SpiralRotationConstraintRight
{
  SpiralRotationConstraintRight(float itemSpacingRadians)
  : mItemSpacingRadians(itemSpacingRadians)
  {
  }

  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float angle = -mItemSpacingRadians * layoutPosition;

    return Quaternion( Radian( -Math::PI * 1.5f ), Vector3::ZAXIS) * Quaternion( Radian( angle ), Vector3::YAXIS );
  }

  float mItemSpacingRadians;
};

struct SpiralColorConstraint
{
  SpiralColorConstraint(float itemSpacingRadians)
  : mItemSpacingRadians(itemSpacingRadians)
  {
  }

  Vector4 operator()(const Vector4& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Radian angle( mItemSpacingRadians * fabsf(layoutPosition) / Dali::ANGLE_360 );

    float progress = angle - floorf( angle ); // take fractional bit only to get between 0.0 - 1.0
    progress = (progress > 0.5f) ? 2.0f*(1.0f - progress) : progress*2.0f;

    float darkness(1.0f);
    {
      const float startMarker = 0.10f; // The progress at which darkening starts
      const float endMarker   = 0.35f; // The progress at which darkening ends
      const float minDarkness = 0.15f; // The darkness at end marker

      if (progress > endMarker)
      {
        darkness = minDarkness;
      }
      else if (progress > startMarker)
      {
        darkness = 1.0f - ( (1.0f - minDarkness) * ((progress-startMarker) / (endMarker-startMarker)) );
      }
    }

    return Vector4( darkness, darkness, darkness, current.a );
  }

  float mItemSpacingRadians;
};

struct SpiralVisibilityConstraintPortrait
{
  SpiralVisibilityConstraintPortrait(float itemSpacingRadians, float itemDescent, float topItemAlignment)
  : mItemSpacingRadians(itemSpacingRadians),
    mItemDescent(itemDescent),
    mTopItemAlignment(topItemAlignment)
  {
  }

  bool operator()(const bool& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float itemsCachedBeforeTopItem = layoutSize.height*(mTopItemAlignment+0.5f) / mItemDescent;
    return (layoutPosition >= -itemsCachedBeforeTopItem - 1.0f && layoutPosition <= (layoutSize.height / mItemDescent) + 1.0f);
  }

  float mItemSpacingRadians;
  float mItemDescent;
  float mTopItemAlignment;
};

struct SpiralVisibilityConstraintLandscape
{
  SpiralVisibilityConstraintLandscape(float itemSpacingRadians, float itemDescent, float topItemAlignment)
  : mItemSpacingRadians(itemSpacingRadians),
    mItemDescent(itemDescent),
    mTopItemAlignment(topItemAlignment)
  {
  }

  bool operator()(const bool& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float itemsCachedBeforeTopItem = layoutSize.width*(mTopItemAlignment+0.5f) / mItemDescent;
    return (layoutPosition >= -itemsCachedBeforeTopItem - 1.0f && layoutPosition <= (layoutSize.width / mItemDescent) + 1.0f);
  }

  float mItemSpacingRadians;
  float mItemDescent;
  float mTopItemAlignment;
};

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

struct SpiralLayout::Impl
{
  Impl()
  : mItemSizeFunction(DefaultItemSizeFunction()),
    mSpiralRadiusFunction(DefaultSpiralRadiusFunction()),
    mItemSpacingRadians(DEFAULT_ITEM_SPACING_RADIANS),
    mRevolutionDistance(DEFAULT_REVOLUTION_DISTANCE),
    mItemDescent(DEFAULT_ITEM_DESCENT),
    mTopItemAlignment(DEFAULT_TOP_ITEM_ALIGNMENT),
    mScrollSpeedFactor(DEFAULT_SCROLL_SPEED_FACTOR),
    mMaximumSwipeSpeed(DEFAULT_MAXIMUM_SWIPE_SPEED),
    mItemFlickAnimationDuration(DEFAULT_ITEM_FLICK_ANIMATION_DURATION)
  {
  }

  ItemSizeFunction     mItemSizeFunction;
  SpiralRadiusFunction mSpiralRadiusFunction;

  float mItemSpacingRadians;
  float mRevolutionDistance;
  float mItemDescent;
  float mTopItemAlignment;
  float mScrollSpeedFactor;
  float mMaximumSwipeSpeed;
  float mItemFlickAnimationDuration;
};

SpiralLayoutPtr SpiralLayout::New()
{
  return SpiralLayoutPtr(new SpiralLayout());
}

SpiralLayout::~SpiralLayout()
{
  delete mImpl;
}

void SpiralLayout::SetItemSizeFunction(ItemSizeFunction function)
{
  mImpl->mItemSizeFunction = function;
}

SpiralLayout::ItemSizeFunction SpiralLayout::GetItemSizeFunction() const
{
  return mImpl->mItemSizeFunction;
}

void SpiralLayout::SetItemSpacing(Radian itemSpacing)
{
  mImpl->mItemSpacingRadians = itemSpacing;

  float itemsPerSpiral = std::max(1.0f, (2.0f*(float)Math::PI) / mImpl->mItemSpacingRadians);
  mImpl->mItemDescent = mImpl->mRevolutionDistance / itemsPerSpiral;
}

Radian SpiralLayout::GetItemSpacing() const
{
  return Radian( mImpl->mItemSpacingRadians );
}

void SpiralLayout::SetRevolutionDistance(float distance)
{
  mImpl->mRevolutionDistance = distance;

  float itemsPerSpiral = std::max(1.0f, (2.0f*(float)Math::PI) / mImpl->mItemSpacingRadians);
  mImpl->mItemDescent = mImpl->mRevolutionDistance / itemsPerSpiral;
}

float SpiralLayout::GetRevolutionDistance() const
{
  return mImpl->mRevolutionDistance;
}

void SpiralLayout::SetSpiralRadiusFunction(SpiralRadiusFunction function)
{
  mImpl->mSpiralRadiusFunction = function;
}

SpiralLayout::SpiralRadiusFunction SpiralLayout::GetSpiralRadiusFunction() const
{
  return mImpl->mSpiralRadiusFunction;
}

void SpiralLayout::SetTopItemAlignment(float alignment)
{
  mImpl->mTopItemAlignment = alignment;
}

float SpiralLayout::GetTopItemAlignment() const
{
  return mImpl->mTopItemAlignment;
}

void SpiralLayout::SetScrollSpeedFactor(float scrollSpeed)
{
  mImpl->mScrollSpeedFactor = scrollSpeed;
}

void SpiralLayout::SetMaximumSwipeSpeed(float speed)
{
  mImpl->mMaximumSwipeSpeed = speed;
}

void SpiralLayout::SetItemFlickAnimationDuration(float durationSeconds)
{
  mImpl->mItemFlickAnimationDuration = durationSeconds;
}

float SpiralLayout::GetScrollSpeedFactor() const
{
  return mImpl->mScrollSpeedFactor;
}

float SpiralLayout::GetMaximumSwipeSpeed() const
{
  return mImpl->mMaximumSwipeSpeed;
}

float SpiralLayout::GetItemFlickAnimationDuration() const
{
  return mImpl->mItemFlickAnimationDuration;
}

float SpiralLayout::GetMinimumLayoutPosition(unsigned int numberOfItems, Vector3 layoutSize) const
{
  return 1.0f - static_cast<float>(numberOfItems);
}

float SpiralLayout::GetClosestAnchorPosition(float layoutPosition) const
{
  return round(layoutPosition);
}

float SpiralLayout::GetItemScrollToPosition(unsigned int itemId) const
{
  return -(static_cast<float>(itemId));
}

ItemRange SpiralLayout::GetItemsWithinArea(float firstItemPosition, Vector3 layoutSize) const
{
  float layoutHeight = IsHorizontal(mOrientation) ? layoutSize.width : layoutSize.height;
  float itemsPerSpiral = layoutHeight / mImpl->mItemDescent;
  float itemsCachedBeforeTopItem = layoutHeight * (mImpl->mTopItemAlignment + 0.5f) / mImpl->mItemDescent;
  float itemsViewable = std::min(itemsPerSpiral, itemsPerSpiral - itemsCachedBeforeTopItem - firstItemPosition + 1.0f);

  unsigned int firstItem = static_cast<unsigned int>(std::max(0.0f, -firstItemPosition - itemsCachedBeforeTopItem - 1.0f));
  unsigned int lastItem  = static_cast<unsigned int>(std::max(0.0f, firstItem + itemsViewable));

  return ItemRange(firstItem, lastItem+1);
}

unsigned int SpiralLayout::GetReserveItemCount(Vector3 layoutSize) const
{
  float layoutHeight = IsHorizontal(mOrientation) ? layoutSize.width : layoutSize.height;
  return static_cast<unsigned int>(layoutHeight / mImpl->mItemDescent);
}

bool SpiralLayout::GetItemSize(unsigned int itemId, Vector3 layoutSize, Vector3& itemSize) const
{
  // Note: itemId is not checked, since every item has the same size

  itemSize = mImpl->mItemSizeFunction(layoutSize);
  return true;
}

void SpiralLayout::GetResizeAnimation(Animation& animation, Actor actor, Vector3 size, float durationSeconds) const
{
  if(animation)
  {
    animation.AnimateTo( Property( actor, Actor::Property::SIZE ), size );
  }
}

bool SpiralLayout::GetPositionConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const
{
  if (mOrientation == ControlOrientation::Up)
  {
    constraint = SpiralPositionConstraintUp(mImpl->mSpiralRadiusFunction, mImpl->mItemSpacingRadians, mImpl->mItemDescent, mImpl->mTopItemAlignment);
  }
  else if (mOrientation == ControlOrientation::Left)
  {
    constraint = SpiralPositionConstraintLeft(mImpl->mSpiralRadiusFunction, mImpl->mItemSpacingRadians, mImpl->mItemDescent, mImpl->mTopItemAlignment);
  }
  else if (mOrientation == ControlOrientation::Down)
  {
    constraint = SpiralPositionConstraintDown(mImpl->mSpiralRadiusFunction, mImpl->mItemSpacingRadians, mImpl->mItemDescent, mImpl->mTopItemAlignment);
  }
  else // mOrientation == ControlOrientation::Right
  {
    constraint = SpiralPositionConstraintRight(mImpl->mSpiralRadiusFunction, mImpl->mItemSpacingRadians, mImpl->mItemDescent, mImpl->mTopItemAlignment);
  }

  return true;
}

bool SpiralLayout::GetRotationConstraint(unsigned int itemId, ItemLayout::QuaternionFunction& constraint) const
{
  if (mOrientation == ControlOrientation::Up)
  {
    constraint = SpiralRotationConstraintUp(mImpl->mItemSpacingRadians);
  }
  else if (mOrientation == ControlOrientation::Left)
  {
    constraint = SpiralRotationConstraintLeft(mImpl->mItemSpacingRadians);
  }
  else if (mOrientation == ControlOrientation::Down)
  {
    constraint = SpiralRotationConstraintDown(mImpl->mItemSpacingRadians);
  }
  else // mOrientation == ControlOrientation::Right
  {
    constraint = SpiralRotationConstraintRight(mImpl->mItemSpacingRadians);
  }

  return true;
}

bool SpiralLayout::GetScaleConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const
{
  return false; // No scaling
}

bool SpiralLayout::GetColorConstraint(unsigned int itemId, ItemLayout::Vector4Function& constraint) const
{
  constraint = SpiralColorConstraint(mImpl->mItemSpacingRadians);
  return true;
}

bool SpiralLayout::GetVisibilityConstraint(unsigned int itemId, ItemLayout::BoolFunction& constraint) const
{
  if (IsVertical(mOrientation))
  {
    constraint = SpiralVisibilityConstraintPortrait(mImpl->mItemSpacingRadians, mImpl->mItemDescent, mImpl->mTopItemAlignment);
  }
  else // horizontal
  {
    constraint = SpiralVisibilityConstraintLandscape(mImpl->mItemSpacingRadians, mImpl->mItemDescent, mImpl->mTopItemAlignment);
  }

  return true;
}

Degree SpiralLayout::GetScrollDirection() const
{
  Degree scrollDirection(0);

  if (mOrientation == ControlOrientation::Up)
  {
    scrollDirection = Degree( -45.0f ); // Allow swiping horizontally & vertically
  }
  else if (mOrientation == ControlOrientation::Left)
  {
    scrollDirection = Degree( 45.0f );
  }
  else if (mOrientation == ControlOrientation::Down)
  {
    scrollDirection = Degree( 180.0f - 45.0f );
  }
  else // mOrientation == ControlOrientation::Right
  {
    scrollDirection = Degree( 270.0f - 45.0f );
  }

  return scrollDirection;
}

SpiralLayout::SpiralLayout()
: mImpl(NULL)
{
  mImpl = new Impl();
}

float SpiralLayout::GetClosestOnScreenLayoutPosition(int itemID, float currentLayoutPosition, const Vector3& layoutSize)
{
  return GetItemScrollToPosition(itemID);
}

} // namespace Toolkit

} // namespace Dali
