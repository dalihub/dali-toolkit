/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/scrollable/item-view/depth-layout.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/constraint.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-view.h>
#include <dali-toolkit/devel-api/controls/scrollable/item-view/default-item-layout-property.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace // unnamed namespace
{

const unsigned int DEFAULT_NUMBER_OF_COLUMNS    = 3;
const float        DEFAULT_NUMBER_OF_ROWS       = 26.0f;
const float        DEFAULT_ROW_SPACING          = 55.0f;
const float        DEFAULT_BOTTOM_MARGIN_FACTOR = 0.2f;
const Radian       DEFAULT_TILT_ANGLE           ( Math::PI*0.15f );
const Radian       DEFAULT_ITEM_TILT_ANGLE      ( -Math::PI*0.025f );
const float        DEFAULT_SCROLL_SPEED_FACTOR  = 0.02f;
const float        DEFAULT_MAXIMUM_SWIPE_SPEED  = 50.0f;
const float        DEFAULT_ITEM_FLICK_ANIMATION_DURATION = 0.03f;

inline float GetColumnPosition( unsigned int numberOfColumns, unsigned int columnNumber, const Vector3& itemSize, float layoutWidth )
{
  // Share the available space between margins & column spacings
  float availableSpace = std::max( 0.0f, ( layoutWidth - itemSize.width * numberOfColumns ) );

  float leftMargin = availableSpace / numberOfColumns * 0.5f;

  float columnPosition = leftMargin + itemSize.width * 0.5f + columnNumber * ( itemSize.width + availableSpace / numberOfColumns );

  return columnPosition - layoutWidth * 0.5f;
}

struct DepthPositionConstraint
{
  DepthPositionConstraint( unsigned int itemId,
                           unsigned int numberOfColumns,
                           unsigned int columnNumber,
                           const Vector3& itemSize,
                           float heightScale,
                           float depthScale )
  : mItemSize( itemSize ),
    mItemId( itemId ),
    mNumberOfColumns( numberOfColumns ),
    mColumnNumber( columnNumber ),
    mHeightScale( heightScale ),
    mDepthScale( depthScale )
  {
  }

  inline void Orientation0( Vector3& current, float layoutPosition, const Vector3& layoutSize )
  {
    float rowLayoutPositon = layoutPosition - static_cast< float >( mColumnNumber );

    current.x = GetColumnPosition( mNumberOfColumns, mColumnNumber, mItemSize, layoutSize.width );
    current.y = rowLayoutPositon * mHeightScale + layoutSize.height * 0.5f - DEFAULT_BOTTOM_MARGIN_FACTOR * layoutSize.height - mItemSize.height * 0.5f;
    current.z = -rowLayoutPositon * mDepthScale;
  }

  inline void Orientation90( Vector3& current, float layoutPosition, const Vector3& layoutSize )
  {
    float rowLayoutPositon = layoutPosition - static_cast< float >( mColumnNumber ) + mNumberOfColumns * 0.5f;

    current.x = rowLayoutPositon * mHeightScale + layoutSize.width * 0.5f - DEFAULT_BOTTOM_MARGIN_FACTOR * layoutSize.width - mItemSize.height * 0.5f;
    current.y = -GetColumnPosition( mNumberOfColumns, mColumnNumber, mItemSize, layoutSize.height );
    current.z = -rowLayoutPositon * mDepthScale;
  }

  inline void Orientation180( Vector3& current, float layoutPosition, const Vector3& layoutSize )
  {
    float rowLayoutPositon = layoutPosition - static_cast< float >( mColumnNumber );

    current.x = -GetColumnPosition( mNumberOfColumns, mColumnNumber, mItemSize, layoutSize.width );
    current.y = -( rowLayoutPositon * mHeightScale + layoutSize.height * 0.5f - DEFAULT_BOTTOM_MARGIN_FACTOR * layoutSize.height - mItemSize.height * 0.5f );
    current.z = -rowLayoutPositon * mDepthScale;
  }

  inline void Orientation270( Vector3& current, float layoutPosition, const Vector3& layoutSize )
  {
    float rowLayoutPositon = layoutPosition - static_cast< float >( mColumnNumber ) + mNumberOfColumns * 0.5f;

    current.x = -( rowLayoutPositon * mHeightScale + layoutSize.width * 0.5f - DEFAULT_BOTTOM_MARGIN_FACTOR * layoutSize.width - mItemSize.height * 0.5f );
    current.y = GetColumnPosition( mNumberOfColumns, mColumnNumber, mItemSize, layoutSize.height );
    current.z = -rowLayoutPositon * mDepthScale;
  }

  void Orientation0( Vector3& current, const PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    const Vector3& layoutSize = inputs[1]->GetVector3();
    Orientation0( current, layoutPosition, layoutSize );
  }

  void Orientation90( Vector3& current, const PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    const Vector3& layoutSize = inputs[1]->GetVector3();
    Orientation90( current, layoutPosition, layoutSize );
  }

  void Orientation180( Vector3& current, const PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    const Vector3& layoutSize = inputs[1]->GetVector3();
    Orientation180( current, layoutPosition, layoutSize );
  }

  void Orientation270( Vector3& current, const PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    const Vector3& layoutSize = inputs[1]->GetVector3();
    Orientation270( current, layoutPosition, layoutSize );
  }

  Vector3 mItemSize;
  unsigned int mItemId;
  unsigned int mNumberOfColumns;
  unsigned int mColumnNumber;
  float mHeightScale;
  float mDepthScale;
};

struct DepthRotationConstraint
{
  DepthRotationConstraint( Radian angleRadians, ControlOrientation::Type orientation )
  : mTiltAngle( angleRadians ),
    mMultiplier( 0.0f )
  {
    if ( orientation == ControlOrientation::Up )
    {
      mMultiplier = 0.0f;
    }
    else if ( orientation == ControlOrientation::Left )
    {
      mMultiplier = 1.5f;
    }
    else if ( orientation == ControlOrientation::Down )
    {
      mMultiplier = -1.0f;
    }
    else // orientation == ControlOrientation::Right
    {
      mMultiplier = 0.5f;
    }
  }

  void operator()( Quaternion& current, const PropertyInputContainer& /* inputs */ )
  {
    current = Quaternion( Radian( mMultiplier * Math::PI ), Vector3::ZAXIS ) * Quaternion( mTiltAngle, Vector3::XAXIS );
  }

  Radian mTiltAngle;
  float mMultiplier;
};

struct DepthColorConstraint
{
  DepthColorConstraint( unsigned int itemId, unsigned int numberOfColumns, float numberOfRows, unsigned int columnNumber )
  : mItemId( itemId ),
    mNumberOfColumns( numberOfColumns ),
    mNumberOfRows( numberOfRows ),
    mColumnNumber( columnNumber )
  {
  }

  void operator()( Vector4& current, const Dali::PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    float row = ( layoutPosition - static_cast<float>( mColumnNumber ) ) / mNumberOfColumns;

    float darkness(1.0f);
    float alpha(1.0f);

    if (row < 0.0f)
    {
      darkness = alpha = std::max(0.0f, 1.0f + row);
    }
    else
    {
      if (row > mNumberOfRows)
      {
        darkness = 0.0f;
      }
      else
      {
        darkness = 1.0f - ( 1.0f * (row / mNumberOfRows) );
      }

      if (row > (mNumberOfRows-1.0f))
      {
        alpha = std::max(0.0f, 1.0f - (row-(mNumberOfRows-1.0f)));
      }
    }

    current.r = current.g = current.b = darkness;
    current.a *= alpha;
  }

  unsigned int mItemId;
  unsigned int mNumberOfColumns;
  float mNumberOfRows;
  unsigned int mColumnNumber;
};

struct DepthVisibilityConstraint
{
  DepthVisibilityConstraint( unsigned int itemId, unsigned int numberOfColumns, float numberOfRows, unsigned int columnNumber )
  : mItemId( itemId ),
    mNumberOfColumns(numberOfColumns),
    mNumberOfRows(numberOfRows),
    mColumnNumber(columnNumber)
  {
  }

  void operator()( bool& current, const Dali::PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    float row = ( layoutPosition - static_cast< float >( mColumnNumber ) ) / mNumberOfColumns;

    current = ( row > -1.0f ) && ( row < mNumberOfRows );
  }

  unsigned int mItemId;
  unsigned int mNumberOfColumns;
  float mNumberOfRows;
  unsigned int mColumnNumber;
};

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

struct DepthLayout::Impl
{
  Impl()
  : mNumberOfColumns(DEFAULT_NUMBER_OF_COLUMNS),
    mNumberOfRows(DEFAULT_NUMBER_OF_ROWS),
    mRowSpacing(DEFAULT_ROW_SPACING),
    mTiltAngle(DEFAULT_TILT_ANGLE),
    mItemTiltAngle(DEFAULT_ITEM_TILT_ANGLE),
    mScrollSpeedFactor(DEFAULT_SCROLL_SPEED_FACTOR),
    mMaximumSwipeSpeed(DEFAULT_MAXIMUM_SWIPE_SPEED),
    mItemFlickAnimationDuration(DEFAULT_ITEM_FLICK_ANIMATION_DURATION)
  {
  }

  unsigned int mNumberOfColumns;
  unsigned int mNumberOfRows;

  float mRowSpacing;

  Radian mTiltAngle;
  Radian mItemTiltAngle;

  float mScrollSpeedFactor;
  float mMaximumSwipeSpeed;
  float mItemFlickAnimationDuration;
};

DepthLayoutPtr DepthLayout::New()
{
  return DepthLayoutPtr(new DepthLayout());
}

DepthLayout::~DepthLayout()
{
  delete mImpl;
}

void DepthLayout::SetNumberOfColumns(unsigned int columns)
{
  mImpl->mNumberOfColumns = columns;
}

unsigned int DepthLayout::GetNumberOfColumns() const
{
  return mImpl->mNumberOfColumns;
}

void DepthLayout::SetNumberOfRows(unsigned int rows)
{
  mImpl->mNumberOfRows = rows;
}

unsigned int DepthLayout::GetNumberOfRows() const
{
  return mImpl->mNumberOfRows;
}

void DepthLayout::SetRowSpacing(float spacing)
{
  mImpl->mRowSpacing = spacing;
}

float DepthLayout::GetRowSpacing() const
{
  return mImpl->mRowSpacing;
}

void DepthLayout::SetTiltAngle(Degree angle)
{
  mImpl->mTiltAngle = Degree( Clamp( angle, -45.0f, 45.0f ) );
}

Degree DepthLayout::GetTiltAngle() const
{
  return Degree( mImpl->mTiltAngle );
}

void DepthLayout::SetItemTiltAngle(Degree angle)
{
  mImpl->mItemTiltAngle = angle;
}

Degree DepthLayout::GetItemTiltAngle() const
{
  return Degree( mImpl->mItemTiltAngle );
}

void DepthLayout::SetScrollSpeedFactor(float scrollSpeed)
{
  mImpl->mScrollSpeedFactor = scrollSpeed;
}

void DepthLayout::SetMaximumSwipeSpeed(float speed)
{
  mImpl->mMaximumSwipeSpeed = speed;
}

void DepthLayout::SetItemFlickAnimationDuration(float durationSeconds)
{
  mImpl->mItemFlickAnimationDuration = durationSeconds;
}

float DepthLayout::GetScrollSpeedFactor() const
{
  return mImpl->mScrollSpeedFactor;
}

float DepthLayout::GetMaximumSwipeSpeed() const
{
  return mImpl->mMaximumSwipeSpeed;
}

float DepthLayout::GetItemFlickAnimationDuration() const
{
  return mImpl->mItemFlickAnimationDuration;
}

float DepthLayout::GetMinimumLayoutPosition(unsigned int numberOfItems, Vector3 layoutSize) const
{
  return static_cast<float>(mImpl->mNumberOfColumns) - static_cast<float>(numberOfItems);
}

float DepthLayout::GetClosestAnchorPosition(float layoutPosition) const
{
  float rowIndex = static_cast<float>(round(layoutPosition / mImpl->mNumberOfColumns));
  return rowIndex * static_cast<float>(mImpl->mNumberOfColumns);
}

float DepthLayout::GetItemScrollToPosition(unsigned int itemId) const
{
  float rowIndex = static_cast< float >( itemId ) / mImpl->mNumberOfColumns;
  return -rowIndex * static_cast<float>(mImpl->mNumberOfColumns);
}

ItemRange DepthLayout::GetItemsWithinArea(float firstItemPosition, Vector3 layoutSize) const
{
  float firstRow = -(firstItemPosition/mImpl->mNumberOfColumns);
  float lastRow = firstRow + mImpl->mNumberOfRows * 0.5f;

  unsigned int firstItem = static_cast<unsigned int>(std::max(0.0f, firstRow * mImpl->mNumberOfColumns));
  unsigned int lastItem  = static_cast<unsigned int>(std::max(0.0f, lastRow  * mImpl->mNumberOfColumns));

  return ItemRange(firstItem, lastItem+1);
}

unsigned int DepthLayout::GetReserveItemCount(Vector3 layoutSize) const
{
  float itemsWithinLayout = (layoutSize.depth * mImpl->mNumberOfColumns) / (cosf(mImpl->mTiltAngle) * mImpl->mRowSpacing);

  return static_cast<unsigned int>(itemsWithinLayout);
}

void DepthLayout::GetDefaultItemSize( unsigned int itemId, const Vector3& layoutSize, Vector3& itemSize ) const
{
  // 1x1 aspect ratio
  itemSize.width = itemSize.height = itemSize.depth = ( IsVertical( GetOrientation() ) ? layoutSize.width : layoutSize.height ) / static_cast<float>( mImpl->mNumberOfColumns + 1 );
}

Degree DepthLayout::GetScrollDirection() const
{
  Degree scrollDirection(0.0f);
  ControlOrientation::Type orientation = GetOrientation();

  if ( orientation == ControlOrientation::Up )
  {
    scrollDirection = Degree( 180.0f );
  }
  else if ( orientation == ControlOrientation::Left )
  {
    scrollDirection = Degree( 270.0f );
  }
  else if ( orientation == ControlOrientation::Down )
  {
    scrollDirection = Degree( 0.0f );
  }
  else // orientation == ControlOrientation::Right
  {
    scrollDirection = Degree( 90.0f );
  }

  return scrollDirection;
}

void DepthLayout::ApplyConstraints( Actor& actor, const int itemId, const Vector3& layoutSize, const Actor& itemViewActor )
{

  if(HasLayoutChanged())
  {
    SetDepthLayoutProperties(GetLayoutProperties());
  }
  Dali::Toolkit::ItemView itemView = Dali::Toolkit::ItemView::DownCast( itemViewActor );
  if( itemView )
  {
    Vector3 itemSize;
    GetItemSize( itemId, layoutSize, itemSize );

    ControlOrientation::Type orientation = GetOrientation();

    // Position constraint
    Constraint constraint;
    DepthPositionConstraint depthPositionStruct( itemId,
                                                 mImpl->mNumberOfColumns,
                                                 itemId % mImpl->mNumberOfColumns,
                                                 itemSize,
                                                 -sinf( mImpl->mTiltAngle ) * mImpl->mRowSpacing,
                                                 cosf( mImpl->mTiltAngle ) * mImpl->mRowSpacing );
    if ( orientation == ControlOrientation::Up )
    {
      constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, depthPositionStruct, &DepthPositionConstraint::Orientation0 );
    }
    else if ( orientation == ControlOrientation::Left )
    {
      constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, depthPositionStruct, &DepthPositionConstraint::Orientation90 );
    }
    else if ( orientation == ControlOrientation::Down )
    {
      constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, depthPositionStruct, &DepthPositionConstraint::Orientation180 );
    }
    else // orientation == ControlOrientation::Right
    {
      constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, depthPositionStruct, &DepthPositionConstraint::Orientation270 );
    }
    constraint.AddSource( ParentSource( Toolkit::ItemView::Property::LAYOUT_POSITION ) );
    constraint.AddSource( ParentSource( Actor::Property::SIZE ) );
    constraint.Apply();

    // Rotation constraint
    constraint = Constraint::New< Quaternion >( actor, Actor::Property::ORIENTATION, DepthRotationConstraint( mImpl->mItemTiltAngle, orientation ) );
    constraint.Apply();

    // Color constraint
    constraint = Constraint::New< Vector4 >( actor, Actor::Property::COLOR, DepthColorConstraint( itemId, mImpl->mNumberOfColumns, mImpl->mNumberOfRows*0.5f, itemId % mImpl->mNumberOfColumns ) );
    constraint.AddSource( ParentSource( Toolkit::ItemView::Property::LAYOUT_POSITION ) );
    constraint.SetRemoveAction( Dali::Constraint::Discard );
    constraint.Apply();

    // Visibility constraint
    constraint = Constraint::New< bool >( actor, Actor::Property::VISIBLE, DepthVisibilityConstraint( itemId, mImpl->mNumberOfColumns, mImpl->mNumberOfRows*0.5f, itemId % mImpl->mNumberOfColumns ) );
    constraint.AddSource( ParentSource( Toolkit::ItemView::Property::LAYOUT_POSITION ) );
    constraint.SetRemoveAction( Dali::Constraint::Discard );
    constraint.Apply();
  }
}

void DepthLayout::SetDepthLayoutProperties(const Property::Map& properties)
{
  // Set any properties specified for DepthLayout.
  for( unsigned int idx = 0, mapCount = properties.Count(); idx < mapCount; ++idx )
  {
    KeyValuePair propertyPair = properties.GetKeyValue( idx );
    switch(DefaultItemLayoutProperty::Property(propertyPair.first.indexKey))
    {
      case DefaultItemLayoutProperty::DEPTH_COLUMN_NUMBER:
      {
        SetNumberOfColumns(propertyPair.second.Get<int>());
        break;
      }
      case DefaultItemLayoutProperty::DEPTH_ROW_NUMBER:
      {
        SetNumberOfRows(propertyPair.second.Get<int>());
        break;
      }
      case DefaultItemLayoutProperty::DEPTH_ROW_SPACING:
      {
        SetRowSpacing(propertyPair.second.Get<float>());
        break;
      }
      case DefaultItemLayoutProperty::DEPTH_MAXIMUM_SWIPE_SPEED:
      {
        SetMaximumSwipeSpeed(propertyPair.second.Get<float>());
        break;
      }
      case DefaultItemLayoutProperty::DEPTH_SCROLL_SPEED_FACTOR:
      {
        SetScrollSpeedFactor(propertyPair.second.Get<float>());
        break;
      }
      case DefaultItemLayoutProperty::DEPTH_TILT_ANGLE:
      {
        SetTiltAngle(Degree(Radian(propertyPair.second.Get<float>())));
        break;
      }
      case DefaultItemLayoutProperty::DEPTH_ITEM_TILT_ANGLE:
      {
        SetItemTiltAngle(Degree(Radian(propertyPair.second.Get<float>())));
        break;
      }
      case DefaultItemLayoutProperty::DEPTH_ITEM_FLICK_ANIMATION_DURATION:
      {
        SetItemFlickAnimationDuration(propertyPair.second.Get<float>());
        break;
      }
      default:
      {
        break;
      }
    }
  }
  ResetLayoutChangedFlag();
}

Vector3 DepthLayout::GetItemPosition( int itemID, float currentLayoutPosition, const Vector3& layoutSize ) const
{
  Vector3 itemPosition = Vector3::ZERO;

  const float heightScale = -sinf( mImpl->mTiltAngle ) * mImpl->mRowSpacing;
  const float depthScale  =  cosf( mImpl->mTiltAngle ) * mImpl->mRowSpacing;

  Vector3 itemSize;
  GetItemSize( itemID, layoutSize, itemSize );
  DepthPositionConstraint positionFunctor = DepthPositionConstraint( itemID,
                                                                     mImpl->mNumberOfColumns,
                                                                     itemID % mImpl->mNumberOfColumns,
                                                                     itemSize,
                                                                     heightScale,
                                                                     depthScale );
  ControlOrientation::Type orientation = GetOrientation();
  if ( orientation == ControlOrientation::Up )
  {
    positionFunctor.Orientation0( itemPosition, currentLayoutPosition + itemID, layoutSize );
  }
  else if ( orientation == ControlOrientation::Left )
  {
    positionFunctor.Orientation90( itemPosition, currentLayoutPosition + itemID, layoutSize );
  }
  else if ( orientation == ControlOrientation::Down )
  {
    positionFunctor.Orientation180( itemPosition, currentLayoutPosition + itemID, layoutSize );
  }
  else // orientation == ControlOrientation::Right
  {
    positionFunctor.Orientation270( itemPosition, currentLayoutPosition + itemID, layoutSize );
  }

  return itemPosition;
}

DepthLayout::DepthLayout()
: mImpl(NULL)
{
  mImpl = new Impl();
}

float DepthLayout::GetClosestOnScreenLayoutPosition(int itemID, float currentLayoutPosition, const Vector3& layoutSize)
{
  float scrollTo = currentLayoutPosition;
  float row = (currentLayoutPosition + itemID - static_cast<float>(itemID % mImpl->mNumberOfColumns)) / mImpl->mNumberOfColumns;

  // Check whether item is not within viewable area
  if(row <= -1.0f)
  {
    scrollTo = GetItemScrollToPosition(itemID);
  }
  else if(row > mImpl->mNumberOfRows * 0.5f - 1.0f)
  {
    scrollTo = GetItemScrollToPosition(itemID) + (mImpl->mNumberOfRows - 1.0f) * 0.5f * mImpl->mNumberOfColumns;
  }

  return scrollTo;
}

int DepthLayout::GetNextFocusItemID(int itemID, int maxItems, Dali::Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled)
{
  switch( direction )
  {
    case Toolkit::Control::KeyboardFocus::LEFT:
    {
      itemID--;
      if( itemID < 0 )
      {
        itemID = loopEnabled ? maxItems - 1 : 0;
      }
      break;
    }
    case Toolkit::Control::KeyboardFocus::UP:
    {
      itemID += mImpl->mNumberOfColumns;
      if( itemID >= maxItems )
      {
        itemID = loopEnabled ? 0 : itemID - mImpl->mNumberOfColumns;
      }
      break;
    }
    case Toolkit::Control::KeyboardFocus::RIGHT:
    {
      itemID++;
      if( itemID >= maxItems )
      {
        itemID = loopEnabled ? 0 : maxItems - 1;
      }
      break;
    }
    case Toolkit::Control::KeyboardFocus::DOWN:
    {
      itemID -= mImpl->mNumberOfColumns;
      if( itemID < 0 )
      {
        itemID = loopEnabled ? itemID + maxItems : itemID + mImpl->mNumberOfColumns;
      }
      break;
    }
    default:
    {
      break;
    }
  }
  return itemID;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
