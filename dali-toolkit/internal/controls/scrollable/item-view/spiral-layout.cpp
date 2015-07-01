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
#include <dali-toolkit/internal/controls/scrollable/item-view/spiral-layout.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/constraint.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-view.h>

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

float GetDefaultSpiralRadiusFunction(const Vector3& layoutSize)
{
  return layoutSize.width*0.4f;
}

struct SpiralPositionConstraint
{
  SpiralPositionConstraint( unsigned int itemId, float spiralRadius, float itemSpacingRadians, float itemDescent, float topItemAlignment )
  : mItemId( itemId ),
    mSpiralRadius( spiralRadius ),
    mItemSpacingRadians( itemSpacingRadians ),
    mItemDescent( itemDescent ),
    mTopItemAlignment( topItemAlignment )
  {
  }

  inline void OrientationUp( Vector3& current, float layoutPosition, const Vector3& layoutSize )
  {
    float angle = -Math::PI * 0.5f + mItemSpacingRadians * layoutPosition;

    current.x = -mSpiralRadius * cosf( angle );
    current.y = ( mItemDescent * layoutPosition ) + layoutSize.height * mTopItemAlignment;
    current.z = -mSpiralRadius * sinf( angle );
  }

  inline void OrientationLeft( Vector3& current, float layoutPosition, const Vector3& layoutSize )
  {
    float angle = Math::PI * 0.5f + mItemSpacingRadians * layoutPosition;

    current.x = ( mItemDescent * layoutPosition ) + layoutSize.width * mTopItemAlignment;
    current.y = -mSpiralRadius * cosf( angle );
    current.z = mSpiralRadius * sinf( angle );
  }

  inline void OrientationDown( Vector3& current, float layoutPosition, const Vector3& layoutSize )
  {
    float angle = Math::PI * 0.5f + mItemSpacingRadians * layoutPosition;

    current.x = -mSpiralRadius * cosf( angle );
    current.y = ( -mItemDescent * layoutPosition ) - layoutSize.height * mTopItemAlignment;
    current.z = mSpiralRadius * sinf(angle);
  }

  inline void OrientationRight( Vector3& current, float layoutPosition, const Vector3& layoutSize )
  {
    float angle = -Math::PI*0.5f + mItemSpacingRadians * layoutPosition;

    current.x = (-mItemDescent * layoutPosition) - layoutSize.width * mTopItemAlignment;
    current.y = -mSpiralRadius * cosf( angle );
    current.z = -mSpiralRadius * sinf( angle );
  }

  void OrientationUp( Vector3& current, const PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    const Vector3& layoutSize = inputs[1]->GetVector3();
    OrientationUp( current, layoutPosition, layoutSize );
  }

  void OrientationLeft( Vector3& current, const PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    const Vector3& layoutSize = inputs[1]->GetVector3();
    OrientationLeft( current, layoutPosition, layoutSize );
  }

  void OrientationDown( Vector3& current, const PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    const Vector3& layoutSize = inputs[1]->GetVector3();
    OrientationDown( current, layoutPosition, layoutSize );
  }

  void OrientationRight( Vector3& current, const PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    const Vector3& layoutSize = inputs[1]->GetVector3();
    OrientationRight( current, layoutPosition, layoutSize );
  }

  unsigned int mItemId;
  float mSpiralRadius;
  float mItemSpacingRadians;
  float mItemDescent;
  float mTopItemAlignment;
};

struct SpiralRotationConstraint
{
  SpiralRotationConstraint( unsigned int itemId, float itemSpacingRadians )
  : mItemId( itemId ),
    mItemSpacingRadians( itemSpacingRadians )
  {
  }

  void OrientationUp( Quaternion& current, const PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    float angle = -mItemSpacingRadians * layoutPosition;

    current = Quaternion( Radian( angle ), Vector3::YAXIS);
  }

  void OrientationLeft( Quaternion& current, const PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    float angle = -mItemSpacingRadians * layoutPosition;

    current = Quaternion( Radian( -Math::PI * 0.5f ), Vector3::ZAXIS ) * Quaternion( Radian( angle ), Vector3::YAXIS );
  }

  void OrientationDown( Quaternion& current, const PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    float angle = -mItemSpacingRadians * layoutPosition;

    current = Quaternion( Radian( -Math::PI ), Vector3::ZAXIS) * Quaternion( Radian( angle ), Vector3::YAXIS );
  }

  void OrientationRight( Quaternion& current, const PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    float angle = -mItemSpacingRadians * layoutPosition;

    current = Quaternion( Radian( -Math::PI * 1.5f ), Vector3::ZAXIS) * Quaternion( Radian( angle ), Vector3::YAXIS );
  }

  unsigned int mItemId;
  float mItemSpacingRadians;
};

struct SpiralColorConstraint
{
  SpiralColorConstraint( unsigned int itemId, float itemSpacingRadians )
  : mItemId( itemId ),
    mItemSpacingRadians( itemSpacingRadians )
  {
  }

  void operator()( Vector4& current, const PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    Radian angle( mItemSpacingRadians * fabsf( layoutPosition ) / Dali::ANGLE_360 );

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

    current.r = current.g = current.b = darkness;
  }

  unsigned int mItemId;
  float mItemSpacingRadians;
};

struct SpiralVisibilityConstraint
{
  SpiralVisibilityConstraint( unsigned int itemId, float itemSpacingRadians, float itemDescent, float topItemAlignment )
  : mItemId( itemId ),
    mItemSpacingRadians( itemSpacingRadians ),
    mItemDescent( itemDescent ),
    mTopItemAlignment( topItemAlignment )
  {
  }

  void Portrait( bool& current, const PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    const Vector3& layoutSize = inputs[1]->GetVector3();
    float itemsCachedBeforeTopItem = layoutSize.height*(mTopItemAlignment+0.5f) / mItemDescent;
    current = ( layoutPosition >= -itemsCachedBeforeTopItem - 1.0f && layoutPosition <= ( layoutSize.height / mItemDescent ) + 1.0f );
  }

  void Landscape( bool& current, const PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    const Vector3& layoutSize = inputs[1]->GetVector3();
    float itemsCachedBeforeTopItem = layoutSize.width*(mTopItemAlignment+0.5f) / mItemDescent;
    current = ( layoutPosition >= -itemsCachedBeforeTopItem - 1.0f && layoutPosition <= ( layoutSize.width / mItemDescent ) + 1.0f );
  }

  unsigned int mItemId;
  float mItemSpacingRadians;
  float mItemDescent;
  float mTopItemAlignment;
};

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

struct SpiralLayout::Impl
{
  Impl()
  : mItemSpacingRadians(DEFAULT_ITEM_SPACING_RADIANS),
    mRevolutionDistance(DEFAULT_REVOLUTION_DISTANCE),
    mItemDescent(DEFAULT_ITEM_DESCENT),
    mTopItemAlignment(DEFAULT_TOP_ITEM_ALIGNMENT),
    mScrollSpeedFactor(DEFAULT_SCROLL_SPEED_FACTOR),
    mMaximumSwipeSpeed(DEFAULT_MAXIMUM_SWIPE_SPEED),
    mItemFlickAnimationDuration(DEFAULT_ITEM_FLICK_ANIMATION_DURATION)
  {
  }

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
  float layoutHeight = IsHorizontal( GetOrientation() ) ? layoutSize.width : layoutSize.height;
  float itemsPerSpiral = layoutHeight / mImpl->mItemDescent;
  float itemsCachedBeforeTopItem = layoutHeight * (mImpl->mTopItemAlignment + 0.5f) / mImpl->mItemDescent;
  float itemsViewable = std::min(itemsPerSpiral, itemsPerSpiral - itemsCachedBeforeTopItem - firstItemPosition + 1.0f);

  unsigned int firstItem = static_cast<unsigned int>(std::max(0.0f, -firstItemPosition - itemsCachedBeforeTopItem - 1.0f));
  unsigned int lastItem  = static_cast<unsigned int>(std::max(0.0f, firstItem + itemsViewable));

  return ItemRange(firstItem, lastItem+1);
}

unsigned int SpiralLayout::GetReserveItemCount(Vector3 layoutSize) const
{
  float layoutHeight = IsHorizontal( GetOrientation() ) ? layoutSize.width : layoutSize.height;
  return static_cast<unsigned int>(layoutHeight / mImpl->mItemDescent);
}

void SpiralLayout::GetDefaultItemSize( unsigned int itemId, const Vector3& layoutSize, Vector3& itemSize ) const
{
  itemSize.width = layoutSize.width * 0.25f;

  // 4x3 aspect ratio
  itemSize.height = itemSize.depth = ( itemSize.width / 4.0f ) * 3.0f;
}

Degree SpiralLayout::GetScrollDirection() const
{
  Degree scrollDirection(0);
  const ControlOrientation::Type orientation = GetOrientation();

  if ( orientation == ControlOrientation::Up )
  {
    scrollDirection = Degree( -45.0f ); // Allow swiping horizontally & vertically
  }
  else if ( orientation == ControlOrientation::Left )
  {
    scrollDirection = Degree( 45.0f );
  }
  else if ( orientation == ControlOrientation::Down )
  {
    scrollDirection = Degree( 180.0f - 45.0f );
  }
  else // orientation == ControlOrientation::Right
  {
    scrollDirection = Degree( 270.0f - 45.0f );
  }

  return scrollDirection;
}

void SpiralLayout::ApplyConstraints( Actor& actor, const int itemId, const Vector3& layoutSize, const Actor& itemViewActor )
{
  // This just implements the default behaviour of constraint application.
  // Custom layouts can override this function to apply their custom constraints.
  Dali::Toolkit::ItemView itemView = Dali::Toolkit::ItemView::DownCast( itemViewActor );
  if( itemView )
  {
    const ControlOrientation::Type orientation = GetOrientation();

    // Position constraint
    SpiralPositionConstraint positionConstraint( itemId, GetDefaultSpiralRadiusFunction( layoutSize ), mImpl->mItemSpacingRadians, mImpl->mItemDescent, mImpl->mTopItemAlignment );
    Constraint constraint;
    if ( orientation == ControlOrientation::Up )
    {
      constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, positionConstraint, &SpiralPositionConstraint::OrientationUp );
    }
    else if ( orientation == ControlOrientation::Left )
    {
      constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, positionConstraint, &SpiralPositionConstraint::OrientationLeft );
    }
    else if ( orientation == ControlOrientation::Down )
    {
      constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, positionConstraint, &SpiralPositionConstraint::OrientationDown );
    }
    else // orientation == ControlOrientation::Right
    {
      constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, positionConstraint, &SpiralPositionConstraint::OrientationRight );
    }
    constraint.AddSource( ParentSource( Toolkit::ItemView::Property::LAYOUT_POSITION ) );
    constraint.AddSource( ParentSource( Actor::Property::SIZE ) );
    constraint.Apply();

    // Rotation constraint
    SpiralRotationConstraint rotationConstraint( itemId, mImpl->mItemSpacingRadians );
    if ( orientation == ControlOrientation::Up )
    {
      constraint = Constraint::New< Quaternion >( actor, Actor::Property::ORIENTATION, rotationConstraint, &SpiralRotationConstraint::OrientationUp );
    }
    else if ( orientation == ControlOrientation::Left )
    {
      constraint = Constraint::New< Quaternion >( actor, Actor::Property::ORIENTATION, rotationConstraint, &SpiralRotationConstraint::OrientationLeft );
    }
    else if ( orientation == ControlOrientation::Down )
    {
      constraint = Constraint::New< Quaternion >( actor, Actor::Property::ORIENTATION, rotationConstraint, &SpiralRotationConstraint::OrientationDown );
    }
    else // orientation == ControlOrientation::Right
    {
      constraint = Constraint::New< Quaternion >( actor, Actor::Property::ORIENTATION, rotationConstraint, &SpiralRotationConstraint::OrientationRight );
    }
    constraint.AddSource( ParentSource( Toolkit::ItemView::Property::LAYOUT_POSITION ) );
    constraint.Apply();

    // Color constraint
    constraint = Constraint::New< Vector4 >( actor, Actor::Property::COLOR, SpiralColorConstraint( itemId, mImpl->mItemSpacingRadians ) );
    constraint.AddSource( ParentSource( Toolkit::ItemView::Property::LAYOUT_POSITION ) );
    constraint.SetRemoveAction(Dali::Constraint::Discard);
    constraint.Apply();

    // Visibility constraint
    SpiralVisibilityConstraint visibilityConstraint( itemId, mImpl->mItemSpacingRadians, mImpl->mItemDescent, mImpl->mTopItemAlignment );
    if (IsVertical( orientation ) )
    {
      constraint = Constraint::New< bool >( actor, Actor::Property::VISIBLE, visibilityConstraint, &SpiralVisibilityConstraint::Portrait );
    }
    else // horizontal
    {
      constraint = Constraint::New< bool >( actor, Actor::Property::VISIBLE, visibilityConstraint, &SpiralVisibilityConstraint::Landscape );
    }
    constraint.AddSource( ParentSource( Toolkit::ItemView::Property::LAYOUT_POSITION ) );
    constraint.AddSource( ParentSource( Actor::Property::SIZE ) );
    constraint.SetRemoveAction(Dali::Constraint::Discard);
    constraint.Apply();
  }
}

Vector3 SpiralLayout::GetItemPosition(int itemID, float currentLayoutPosition, const Vector3& layoutSize) const
{
  Vector3 itemPosition = Vector3::ZERO;
  const ControlOrientation::Type orientation = GetOrientation();

  SpiralPositionConstraint positionConstraint( itemID, GetDefaultSpiralRadiusFunction( layoutSize ), mImpl->mItemSpacingRadians, mImpl->mItemDescent, mImpl->mTopItemAlignment );

  if ( orientation == ControlOrientation::Up )
  {
    positionConstraint.OrientationUp( itemPosition, currentLayoutPosition + itemID, layoutSize );
  }
  else if ( orientation == ControlOrientation::Left )
  {
    positionConstraint.OrientationLeft( itemPosition, currentLayoutPosition + itemID, layoutSize );
  }
  else if ( orientation == ControlOrientation::Down )
  {
    positionConstraint.OrientationDown( itemPosition, currentLayoutPosition + itemID, layoutSize );
  }
  else // orientation == ControlOrientation::Right
  {
    positionConstraint.OrientationRight( itemPosition, currentLayoutPosition + itemID, layoutSize );
  }

  return itemPosition;
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

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
