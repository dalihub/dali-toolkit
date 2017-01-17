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
#include <dali-toolkit/internal/controls/scrollable/item-view/grid-layout.h>

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

const unsigned int DEFAULT_NUMBER_OF_COLUMNS = 4;
const float DEFAULT_TOP_MARGIN     =  95.0f;
const float DEFAULT_BOTTOM_MARGIN  =  20.0f;
const float DEFAULT_SIDE_MARGIN    =  20.0f;
const float DEFAULT_COLUMN_SPACING =  20.0f;
const float DEFAULT_ROW_SPACING    =  20.0f;
const float DEFAULT_SCROLL_SPEED_FACTOR = 0.03f;
const float DEFAULT_MAXIMUM_SWIPE_SPEED = 100.0f;
const float DEFAULT_ITEM_FLICK_ANIMATION_DURATION = 0.015f;

struct GridPositionConstraint
{
  GridPositionConstraint(
      unsigned int itemId,
      const unsigned int columnIndex,
      const unsigned int numberOfColumns,
      const float rowSpacing,
      const float columnSpacing,
      const float topMargin,
      const float sideMargin,
      const Vector3& itemSize,
      const float gap )
  : mItemSize( itemSize ),
    mItemId( itemId ),
    mColumnIndex( columnIndex ),
    mNumberOfColumns( numberOfColumns ),
    mRowSpacing( rowSpacing ),
    mColumnSpacing( columnSpacing ),
    mTopMargin( topMargin ),
    mSideMargin( sideMargin ),
    mZGap( gap )
  {
  }

  inline void Orientation0( Vector3& current, float layoutPosition, const Vector3& layoutSize )
  {
    current.x = mSideMargin + ( mColumnIndex * ( mItemSize.x + mColumnSpacing ) ) + mItemSize.x * 0.5f - layoutSize.x * 0.5f;
    current.y = ( ( mItemSize.y + mRowSpacing ) * ( layoutPosition - mColumnIndex) ) / mNumberOfColumns - layoutSize.height * 0.5f + mItemSize.y * 0.5f + mTopMargin;
    current.z = mColumnIndex * mZGap;
  }

  inline void Orientation90( Vector3& current, float layoutPosition, const Vector3& layoutSize )
  {
    current.x = ( ( mItemSize.y + mRowSpacing ) * ( layoutPosition - mColumnIndex ) ) / mNumberOfColumns - layoutSize.width * 0.5f + mItemSize.y * 0.5f + mTopMargin;
    current.y = -( mSideMargin + ( mColumnIndex * ( mItemSize.x + mColumnSpacing ) ) + mItemSize.x * 0.5f - layoutSize.y * 0.5f );
    current.z = mColumnIndex * mZGap;
  }

  inline void Orientation180( Vector3& current, float layoutPosition, const Vector3& layoutSize )
  {
    current.x = -(mSideMargin + (mColumnIndex * (mItemSize.x + mColumnSpacing)) + mItemSize.x * 0.5f - layoutSize.x * 0.5f);
    current.y = -( ( ( mItemSize.y + mRowSpacing ) * ( layoutPosition - mColumnIndex ) ) / mNumberOfColumns - layoutSize.height * 0.5f + mItemSize.y * 0.5f + mTopMargin );
    current.z = mColumnIndex * mZGap;
  }

  inline void Orientation270( Vector3& current, float layoutPosition, const Vector3& layoutSize )
  {
    current.x = -( ( ( mItemSize.y + mRowSpacing ) * ( layoutPosition - mColumnIndex ) ) / mNumberOfColumns - layoutSize.width * 0.5f + mItemSize.y * 0.5f + mTopMargin );
    current.y = mSideMargin + ( mColumnIndex * ( mItemSize.x + mColumnSpacing ) ) + mItemSize.x * 0.5f - layoutSize.y * 0.5f;
    current.z = mColumnIndex * mZGap;
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

public:

  Vector3 mItemSize;
  unsigned int mItemId;
  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mRowSpacing;
  float mColumnSpacing;
  float mTopMargin;
  float mSideMargin;
  float mZGap;
};

void GridRotationConstraint0( Quaternion& current, const PropertyInputContainer& /* inputs */ )
{
  current = Quaternion( Radian( 0.0f ), Vector3::ZAXIS );
}

void GridRotationConstraint90( Quaternion& current, const PropertyInputContainer& /* inputs */ )
{
  current = Quaternion( Radian( 1.5f * Math::PI ), Vector3::ZAXIS );
}

void GridRotationConstraint180( Quaternion& current, const PropertyInputContainer& /* inputs */ )
{
  current = Quaternion( Radian( Math::PI ), Vector3::ZAXIS );
}

void GridRotationConstraint270( Quaternion& current, const PropertyInputContainer& /* inputs */ )
{
  current = Quaternion( Radian( 0.5f * Math::PI ), Vector3::ZAXIS );
}

void GridColorConstraint( Vector4& current, const PropertyInputContainer& /* inputs */ )
{
  current.r = current.g = current.b = 1.0f;
}

struct GridVisibilityConstraint
{
  GridVisibilityConstraint(
      unsigned int itemId,
      const unsigned int columnIndex,
      const unsigned int numberOfColumns,
      const float rowSpacing,
      const float columnSpacing,
      const float sideMargin,
      const Vector3& itemSize )
  : mItemSize( itemSize ),
    mItemId( itemId ),
    mColumnIndex( columnIndex ),
    mNumberOfColumns( numberOfColumns ),
    mRowSpacing( rowSpacing ),
    mColumnSpacing( columnSpacing ),
    mSideMargin( sideMargin )
  {
  }

  void Portrait( bool& current, const PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    const Vector3& layoutSize = inputs[1]->GetVector3();

    float row = ( layoutPosition - static_cast< float >( mColumnIndex ) ) / mNumberOfColumns;
    int rowsPerPage = ceil( layoutSize.height / ( mItemSize.y + mRowSpacing ) );

    current = ( row > -2.0f ) && ( row < rowsPerPage );
  }

  void Landscape( bool& current, const PropertyInputContainer& inputs )
  {
    float layoutPosition = inputs[0]->GetFloat() + static_cast< float >( mItemId );
    const Vector3& layoutSize = inputs[1]->GetVector3();

    float row = ( layoutPosition - static_cast< float >( mColumnIndex ) ) / mNumberOfColumns;
    int rowsPerPage = ceil( layoutSize.width / ( mItemSize.y + mRowSpacing ) );

    current = ( row > -2.0f ) && ( row < rowsPerPage );
  }

public:

  Vector3 mItemSize;
  unsigned int mItemId;
  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mRowSpacing;
  float mColumnSpacing;
  float mSideMargin;
};

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

struct GridLayout::Impl
{
  Impl()
  : mNumberOfColumns(DEFAULT_NUMBER_OF_COLUMNS),
    mRowSpacing(DEFAULT_ROW_SPACING),
    mColumnSpacing(DEFAULT_COLUMN_SPACING),
    mTopMargin(DEFAULT_TOP_MARGIN),
    mBottomMargin(DEFAULT_BOTTOM_MARGIN),
    mSideMargin(DEFAULT_SIDE_MARGIN),
    mZGap(0.f),
    mScrollSpeedFactor(DEFAULT_SCROLL_SPEED_FACTOR),
    mMaximumSwipeSpeed(DEFAULT_MAXIMUM_SWIPE_SPEED),
    mItemFlickAnimationDuration(DEFAULT_ITEM_FLICK_ANIMATION_DURATION)
  {
  }

  unsigned int mNumberOfColumns;
  float mRowSpacing;
  float mColumnSpacing;
  float mTopMargin;
  float mBottomMargin;
  float mSideMargin;
  float mZGap;

  float mScrollSpeedFactor;
  float mMaximumSwipeSpeed;
  float mItemFlickAnimationDuration;
};

GridLayoutPtr GridLayout::New()
{
  return GridLayoutPtr(new GridLayout());
}

GridLayout::~GridLayout()
{
  delete mImpl;
}

void GridLayout::SetNumberOfColumns(unsigned int columns)
{
  mImpl->mNumberOfColumns = columns;
}

unsigned int GridLayout::GetNumberOfColumns() const
{
  return mImpl->mNumberOfColumns;
}

void GridLayout::SetRowSpacing(float spacing)
{
  mImpl->mRowSpacing = spacing;
}

float GridLayout::GetRowSpacing() const
{
  return mImpl->mRowSpacing;
}

void GridLayout::SetColumnSpacing(float spacing)
{
  mImpl->mColumnSpacing = spacing;
}

float GridLayout::GetColumnSpacing() const
{
  return mImpl->mColumnSpacing;
}

void GridLayout::SetTopMargin(float margin)
{
  mImpl->mTopMargin = margin;
}

float GridLayout::GetTopMargin() const
{
  return mImpl->mTopMargin;
}

void GridLayout::SetBottomMargin(float margin)
{
  mImpl->mBottomMargin = margin;
}

float GridLayout::GetBottomMargin() const
{
  return mImpl->mBottomMargin;
}

void GridLayout::SetSideMargin(float margin)
{
  mImpl->mSideMargin = margin;
}

float GridLayout::GetSideMargin() const
{
  return mImpl->mSideMargin;
}

void GridLayout::SetZGap(float gap)
{
  mImpl->mZGap = gap;
}

float GridLayout::GetZGap() const
{
  return mImpl->mZGap;
}

void GridLayout::SetScrollSpeedFactor(float scrollSpeed)
{
  mImpl->mScrollSpeedFactor = scrollSpeed;
}

void GridLayout::SetMaximumSwipeSpeed(float speed)
{
  mImpl->mMaximumSwipeSpeed = speed;
}

void GridLayout::SetItemFlickAnimationDuration(float durationSeconds)
{
  mImpl->mItemFlickAnimationDuration = durationSeconds;
}

float GridLayout::GetScrollSpeedFactor() const
{
  return mImpl->mScrollSpeedFactor;
}

float GridLayout::GetMaximumSwipeSpeed() const
{
  return mImpl->mMaximumSwipeSpeed;
}

float GridLayout::GetItemFlickAnimationDuration() const
{
  return mImpl->mItemFlickAnimationDuration;
}

float GridLayout::GetMinimumLayoutPosition(unsigned int numberOfItems, Vector3 layoutSize) const
{
  float layoutHeight = IsHorizontal( GetOrientation() ) ? layoutSize.width : layoutSize.height;

  Vector3 itemSize;
  GetItemSize( 0, layoutSize, itemSize );

  unsigned int itemsLastRow = numberOfItems % mImpl->mNumberOfColumns;
  if (itemsLastRow == 0)
  {
    itemsLastRow = mImpl->mNumberOfColumns;
  }

  float rowsLastPage = (layoutHeight - mImpl->mBottomMargin - mImpl->mTopMargin + mImpl->mRowSpacing) / (itemSize.y + mImpl->mRowSpacing);
  float itemsLastPage = (rowsLastPage - 1.0f) * static_cast<float>(mImpl->mNumberOfColumns) + static_cast<float>(itemsLastRow);

  return itemsLastPage - static_cast<float>(numberOfItems);
}

float GridLayout::GetClosestAnchorPosition(float layoutPosition) const
{
  float rowIndex = static_cast<float>(round(layoutPosition / mImpl->mNumberOfColumns));
  return rowIndex * static_cast<float>(mImpl->mNumberOfColumns);
}

float GridLayout::GetItemScrollToPosition(unsigned int itemId) const
{
  float rowIndex = static_cast< float >( itemId ) / mImpl->mNumberOfColumns;
  return -rowIndex * static_cast<float>(mImpl->mNumberOfColumns);
}

ItemRange GridLayout::GetItemsWithinArea(float firstItemPosition, Vector3 layoutSize) const
{
  float layoutHeight = IsHorizontal( GetOrientation() ) ? layoutSize.width : layoutSize.height;

  Vector3 itemSize;
  GetItemSize( 0, layoutSize, itemSize );

  int itemsPerPage = mImpl->mNumberOfColumns * ceil(layoutHeight / (itemSize.y + mImpl->mRowSpacing));
  int firstVisibleItem = -(static_cast<int>(firstItemPosition / mImpl->mNumberOfColumns)) * mImpl->mNumberOfColumns;

  int firstItemIndex = std::max(0, firstVisibleItem - static_cast<int>(mImpl->mNumberOfColumns));
  int lastItemIndex  = std::max(0, firstVisibleItem + itemsPerPage);

  return ItemRange(firstItemIndex, lastItemIndex);
}

float GridLayout::GetClosestOnScreenLayoutPosition(int itemID, float currentLayoutPosition, const Vector3& layoutSize)
{
  Vector3 itemPosition = GetItemPosition( itemID, currentLayoutPosition, layoutSize );
  Vector3 itemSize;
  ControlOrientation::Type orientation = GetOrientation();

  GetItemSize(itemID, layoutSize, itemSize);
  Vector3 onScreenArea = ( layoutSize - ( IsVertical( orientation ) ? itemSize : Vector3( itemSize.y, itemSize.x, itemSize.z ) ) ) * 0.5f;
  if (itemPosition.x < -onScreenArea.x
      || itemPosition.x > onScreenArea.x
      || itemPosition.y < -onScreenArea.y
      || itemPosition.y > onScreenArea.y)
  {
    // item not within viewable area
    float rowHeight = itemSize.y + mImpl->mRowSpacing;
    Vector3 firstItemPosition = GetItemPosition( itemID, 0.0f, layoutSize );
    float offset = 0.0f;
    switch( orientation )
    {
      case ControlOrientation::Up:
      {
        if(itemPosition.y > onScreenArea.y)
        {
          offset = ((layoutSize.y - rowHeight) * 0.5f) - firstItemPosition.y;
        }
        else
        {
          offset = ((-layoutSize.y + rowHeight) * 0.5f) - firstItemPosition.y;
        }
        break;
      }
      case ControlOrientation::Down:
      {
        if(itemPosition.y < -onScreenArea.y)
        {
          offset = ((layoutSize.y - rowHeight) * 0.5f) - firstItemPosition.y;
        }
        else
        {
          offset = ((-layoutSize.y + rowHeight) * 0.5f) - firstItemPosition.y;
        }
        break;
      }
      case ControlOrientation::Left:
      {
        if(itemPosition.x > onScreenArea.x)
        {
          offset = ((layoutSize.x - rowHeight) * 0.5f) - firstItemPosition.x;
        }
        else
        {
          offset = ((-layoutSize.x + rowHeight) * 0.5f) - firstItemPosition.x;
        }
        break;
      }
      case ControlOrientation::Right:
      {
        if(itemPosition.x < -onScreenArea.x)
        {
          offset = ((layoutSize.x - rowHeight) * 0.5f) - firstItemPosition.x;
        }
        else
        {
          offset = ((-layoutSize.x + rowHeight) * 0.5f) - firstItemPosition.x;
        }
        break;
      }
    }
    // work out number of rows from first item position to an item aligned to bottom of screen
    float rowDiff = offset / rowHeight;
    float layoutPositionOffset = rowDiff * mImpl->mNumberOfColumns;
    float scrollTo = GetItemScrollToPosition(itemID) + layoutPositionOffset;
    return scrollTo;
  }
  return currentLayoutPosition;
}

unsigned int GridLayout::GetReserveItemCount(Vector3 layoutSize) const
{
  float layoutHeight = IsHorizontal( GetOrientation() ) ? layoutSize.width : layoutSize.height;

  Vector3 itemSize;
  GetItemSize( 0, layoutSize, itemSize );
  int itemsPerPage = mImpl->mNumberOfColumns * ceil(layoutHeight / (itemSize.y + mImpl->mRowSpacing));
  return itemsPerPage;
}

void GridLayout::GetDefaultItemSize( unsigned int itemId, const Vector3& layoutSize, Vector3& itemSize ) const
{
  float layoutWidth = IsHorizontal( GetOrientation() ) ? layoutSize.height : layoutSize.width;
  itemSize.width = ( layoutWidth - mImpl->mSideMargin * 2.0f - mImpl->mColumnSpacing * static_cast<float>( mImpl->mNumberOfColumns - 1 ) ) / static_cast<float>( mImpl->mNumberOfColumns );

  // 4x3 aspect ratio
  itemSize.height = itemSize.depth = itemSize.width * 0.75f;
}

Degree GridLayout::GetScrollDirection() const
{
  Degree scrollDirection(0.0f);
  ControlOrientation::Type orientation = GetOrientation();

  if ( orientation == ControlOrientation::Up )
  {
    scrollDirection = Degree( 0.0f );
  }
  else if ( orientation == ControlOrientation::Left )
  {
    scrollDirection = Degree( 90.0f );
  }
  else if ( orientation == ControlOrientation::Down )
  {
    scrollDirection = Degree( 180.0f );
  }
  else // orientation == ControlOrientation::Right
  {
    scrollDirection = Degree( 270.0f );
  }

  return scrollDirection;
}

void GridLayout::ApplyConstraints( Actor& actor, const int itemId, const Vector3& layoutSize, const Actor& itemViewActor )
{
  // This just implements the default behaviour of constraint application.
  // Custom layouts can override this function to apply their custom constraints.
  Dali::Toolkit::ItemView itemView = Dali::Toolkit::ItemView::DownCast( itemViewActor );
  if( itemView )
  {
    Vector3 itemSize;
    GetItemSize( itemId, layoutSize, itemSize );
    const unsigned int columnIndex = itemId % mImpl->mNumberOfColumns;
    const ControlOrientation::Type orientation = GetOrientation();

    // Position constraint
    GridPositionConstraint positionConstraint( itemId,
                                               columnIndex,
                                               mImpl->mNumberOfColumns,
                                               mImpl->mRowSpacing,
                                               mImpl->mColumnSpacing,
                                               mImpl->mTopMargin,
                                               mImpl->mSideMargin,
                                               itemSize,
                                               mImpl->mZGap );
    Constraint constraint;
    if ( orientation == ControlOrientation::Up )
    {
      constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, positionConstraint, &GridPositionConstraint::Orientation0 );
    }
    else if ( orientation == ControlOrientation::Left )
    {
      constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, positionConstraint, &GridPositionConstraint::Orientation90 );
    }
    else if ( orientation == ControlOrientation::Down )
    {
      constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, positionConstraint, &GridPositionConstraint::Orientation180 );
    }
    else // orientation == ControlOrientation::Right
    {
      constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, positionConstraint, &GridPositionConstraint::Orientation270 );
    }
    constraint.AddSource( ParentSource( Toolkit::ItemView::Property::LAYOUT_POSITION ) );
    constraint.AddSource( ParentSource( Actor::Property::SIZE ) );
    constraint.Apply();

    // Rotation constraint
    if ( orientation == ControlOrientation::Up )
    {
      constraint = Constraint::New< Quaternion >( actor, Actor::Property::ORIENTATION, &GridRotationConstraint0 );
    }
    else if ( orientation == ControlOrientation::Left )
    {
      constraint = Constraint::New< Quaternion >( actor, Actor::Property::ORIENTATION, &GridRotationConstraint90 );
    }
    else if ( orientation == ControlOrientation::Down )
    {
      constraint = Constraint::New< Quaternion >( actor, Actor::Property::ORIENTATION, &GridRotationConstraint180 );
    }
    else // orientation == ControlOrientation::Right
    {
      constraint = Constraint::New< Quaternion >( actor, Actor::Property::ORIENTATION, &GridRotationConstraint270 );
    }
    constraint.Apply();

    // Color constraint
    constraint = Constraint::New< Vector4 >( actor, Actor::Property::COLOR, &GridColorConstraint );
    constraint.SetRemoveAction( Dali::Constraint::Discard );
    constraint.Apply();

    // Visibility constraint
    GridVisibilityConstraint visibilityConstraint( itemId,
                                                   columnIndex,
                                                   mImpl->mNumberOfColumns,
                                                   mImpl->mRowSpacing,
                                                   mImpl->mColumnSpacing,
                                                   mImpl->mSideMargin,
                                                   itemSize );
    if ( IsVertical( orientation ) )
    {
      constraint = Constraint::New<bool>( actor, Actor::Property::VISIBLE, visibilityConstraint, &GridVisibilityConstraint::Portrait );
    }
    else // horizontal
    {
      constraint = Constraint::New<bool>( actor, Actor::Property::VISIBLE, visibilityConstraint, &GridVisibilityConstraint::Landscape );
    }
    constraint.AddSource( ParentSource( Toolkit::ItemView::Property::LAYOUT_POSITION ) );
    constraint.AddSource( ParentSource( Actor::Property::SIZE ) );
    constraint.SetRemoveAction( Dali::Constraint::Discard );
    constraint.Apply();
  }
}

void GridLayout::SetGridLayoutProperties(const Property::Map& properties)
{
  // Set any properties specified for gridLayout.
  for( unsigned int idx = 0, mapCount = properties.Count(); idx < mapCount; ++idx )
  {
    KeyValuePair propertyPair = properties.GetKeyValue( idx );
    switch(DefaultItemLayoutProperty::Property(propertyPair.first.indexKey))
    {
      case DefaultItemLayoutProperty::GRID_COLUMN_NUMBER:
      {
        SetNumberOfColumns(propertyPair.second.Get<int>());
        break;
      }
      case DefaultItemLayoutProperty::GRID_ROW_SPACING:
      {
        SetRowSpacing(propertyPair.second.Get<float>());
        break;
      }
      case DefaultItemLayoutProperty::GRID_COLUMN_SPACING:
      {
        SetColumnSpacing(propertyPair.second.Get<float>());
        break;
      }
      case DefaultItemLayoutProperty::GRID_TOP_MARGIN:
      {
        SetTopMargin(propertyPair.second.Get<float>());
        break;
      }
      case DefaultItemLayoutProperty::GRID_BOTTOM_MARGIN:
      {
        SetBottomMargin(propertyPair.second.Get<float>());
        break;
      }
      case DefaultItemLayoutProperty::GRID_SIDE_MARGIN:
      {
        SetSideMargin(propertyPair.second.Get<float>());
        break;
      }
      case DefaultItemLayoutProperty::GRID_SCROLL_SPEED_FACTOR:
      {
        SetScrollSpeedFactor(propertyPair.second.Get<float>());
        break;
      }
      case DefaultItemLayoutProperty::GRID_MAXIMUM_SWIPE_SPEED:
      {
        SetMaximumSwipeSpeed(propertyPair.second.Get<float>());
        break;
      }
      case DefaultItemLayoutProperty::GRID_ITEM_FLICK_ANIMATION_DURATION:
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
}

Vector3 GridLayout::GetItemPosition(int itemID, float currentLayoutPosition, const Vector3& layoutSize) const
{
  Vector3 itemPosition = Vector3::ZERO;
  const unsigned int columnIndex = itemID % mImpl->mNumberOfColumns;
  const ControlOrientation::Type orientation = GetOrientation();
  Vector3 itemSize;
  GetItemSize( itemID, layoutSize, itemSize );

  GridPositionConstraint positionConstraintStruct( itemID,
                                                   columnIndex,
                                                   mImpl->mNumberOfColumns,
                                                   mImpl->mRowSpacing,
                                                   mImpl->mColumnSpacing,
                                                   mImpl->mTopMargin,
                                                   mImpl->mSideMargin,
                                                   itemSize,
                                                   mImpl->mZGap );

  if ( orientation == ControlOrientation::Up )
  {
    positionConstraintStruct.Orientation0( itemPosition, currentLayoutPosition + itemID, layoutSize );
  }
  else if ( orientation == ControlOrientation::Left )
  {
    positionConstraintStruct.Orientation90( itemPosition, currentLayoutPosition + itemID, layoutSize );
  }
  else if ( orientation == ControlOrientation::Down )
  {
    positionConstraintStruct.Orientation180( itemPosition, currentLayoutPosition + itemID, layoutSize );
  }
  else // orientation == ControlOrientation::Right
  {
    positionConstraintStruct.Orientation270( itemPosition, currentLayoutPosition + itemID, layoutSize );
  }

  return itemPosition;
}

int GridLayout::GetNextFocusItemID(int itemID, int maxItems, Dali::Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled)
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
      itemID -= mImpl->mNumberOfColumns;
      if( itemID < 0 )
      {
        itemID = loopEnabled ? itemID + maxItems : itemID + mImpl->mNumberOfColumns;
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
      itemID += mImpl->mNumberOfColumns;
      if( itemID >= maxItems )
      {
        itemID = loopEnabled ? 0 : itemID - mImpl->mNumberOfColumns;
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

GridLayout::GridLayout()
: mImpl(NULL)
{
  mImpl = new Impl();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
