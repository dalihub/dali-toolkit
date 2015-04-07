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
#include <dali-toolkit/public-api/controls/scrollable/item-view/grid-layout.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <dali/public-api/animation/animation.h>

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

// 4 orientations are supported
static const unsigned int ORIENTATION_COUNT = 4;

static Vector3 GetItemSizeDefaultFunction(unsigned int numberOfColumns, float layoutWidth, float sideMargin, float columnSpacing)
{
  float width = (layoutWidth - sideMargin * 2.0f - columnSpacing * static_cast<float>(numberOfColumns - 1)) / static_cast<float>(numberOfColumns);

  // 4x3 aspect ratio
  return Vector3(width, width * 0.75f, width * 0.75f);
}

struct GridPositionConstraint0
{
  GridPositionConstraint0(const unsigned int columnIndex, const unsigned int numberOfColumns, const float rowSpacing, const float columnSpacing, const float topMargin, const float sideMargin, GridLayout::ItemSizeFunction itemSizeFunction, const float gap)
  : mColumnIndex(columnIndex),
    mNumberOfColumns(numberOfColumns),
    mRowSpacing(rowSpacing),
    mColumnSpacing(columnSpacing),
    mTopMargin(topMargin),
    mSideMargin(sideMargin),
    mItemSizeFunction(itemSizeFunction),
    mZGap(gap)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(mNumberOfColumns, layoutSize.width, mSideMargin, mColumnSpacing);

    return Vector3(mSideMargin + (mColumnIndex * (itemSize.x + mColumnSpacing)) + itemSize.x * 0.5f - layoutSize.x * 0.5f,
                   ((itemSize.y + mRowSpacing) * (layoutPosition - mColumnIndex)) / mNumberOfColumns - layoutSize.height * 0.5f + itemSize.y * 0.5f + mTopMargin,
                   mColumnIndex * mZGap);
  }

public:

  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mRowSpacing;
  float mColumnSpacing;
  float mTopMargin;
  float mSideMargin;
  GridLayout::ItemSizeFunction mItemSizeFunction;
  float mZGap;
};

struct GridPositionConstraint90
{
  GridPositionConstraint90(const unsigned int columnIndex, const unsigned int numberOfColumns, const float rowSpacing, const float columnSpacing, const float topMargin, const float sideMargin, GridLayout::ItemSizeFunction itemSizeFunction, const float gap)
  : mColumnIndex(columnIndex),
    mNumberOfColumns(numberOfColumns),
    mRowSpacing(rowSpacing),
    mColumnSpacing(columnSpacing),
    mTopMargin(topMargin),
    mSideMargin(sideMargin),
    mItemSizeFunction(itemSizeFunction),
    mZGap(gap)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(mNumberOfColumns, layoutSize.height, mSideMargin, mColumnSpacing);

    return Vector3(((itemSize.y + mRowSpacing) * (layoutPosition - mColumnIndex)) / mNumberOfColumns - layoutSize.width * 0.5f + itemSize.y * 0.5f + mTopMargin,
                   -(mSideMargin + (mColumnIndex * (itemSize.x + mColumnSpacing)) + itemSize.x * 0.5f - layoutSize.y*0.5f),
                   mColumnIndex * mZGap);
  }

public:

  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mRowSpacing;
  float mColumnSpacing;
  float mTopMargin;
  float mSideMargin;
  GridLayout::ItemSizeFunction mItemSizeFunction;
  float mZGap;
};

struct GridPositionConstraint180
{
  GridPositionConstraint180(const unsigned int columnIndex, const unsigned int numberOfColumns, const float rowSpacing, const float columnSpacing, const float topMargin, const float sideMargin, GridLayout::ItemSizeFunction itemSizeFunction, const float gap)
  : mColumnIndex(columnIndex),
    mNumberOfColumns(numberOfColumns),
    mRowSpacing(rowSpacing),
    mColumnSpacing(columnSpacing),
    mTopMargin(topMargin),
    mSideMargin(sideMargin),
    mItemSizeFunction(itemSizeFunction),
    mZGap(gap)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(mNumberOfColumns, layoutSize.width, mSideMargin, mColumnSpacing);

    return Vector3(-(mSideMargin + (mColumnIndex * (itemSize.x + mColumnSpacing)) + itemSize.x * 0.5f - layoutSize.x * 0.5f),
                   -(((itemSize.y + mRowSpacing) * (layoutPosition - mColumnIndex)) / mNumberOfColumns - layoutSize.height * 0.5f + itemSize.y * 0.5f + mTopMargin),
                   mColumnIndex * mZGap);
  }

public:

  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mRowSpacing;
  float mColumnSpacing;
  float mTopMargin;
  float mSideMargin;
  GridLayout::ItemSizeFunction mItemSizeFunction;
  float mZGap;
};

struct GridPositionConstraint270
{
  GridPositionConstraint270(const unsigned int columnIndex, const unsigned int numberOfColumns, const float rowSpacing, const float columnSpacing, const float topMargin, const float sideMargin, GridLayout::ItemSizeFunction itemSizeFunction, const float gap)
  : mColumnIndex(columnIndex),
    mNumberOfColumns(numberOfColumns),
    mRowSpacing(rowSpacing),
    mColumnSpacing(columnSpacing),
    mTopMargin(topMargin),
    mSideMargin(sideMargin),
    mItemSizeFunction(itemSizeFunction),
    mZGap(gap)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(mNumberOfColumns, layoutSize.height, mSideMargin, mColumnSpacing);

    return Vector3(-(((itemSize.y + mRowSpacing) * (layoutPosition - mColumnIndex)) / mNumberOfColumns - layoutSize.width * 0.5f + itemSize.y * 0.5f + mTopMargin),
                   mSideMargin + (mColumnIndex * (itemSize.x + mColumnSpacing)) + itemSize.x * 0.5f - layoutSize.y * 0.5f,
                   mColumnIndex * mZGap);
  }

public:

  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mRowSpacing;
  float mColumnSpacing;
  float mTopMargin;
  float mSideMargin;
  GridLayout::ItemSizeFunction mItemSizeFunction;
  float mZGap;
};

struct GridRotationConstraint0
{
  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    return Quaternion(0.0f, Vector3::ZAXIS);
  }
};

struct GridRotationConstraint90
{
  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    return Quaternion(1.5f * Math::PI, Vector3::ZAXIS);
  }
};

struct GridRotationConstraint180
{
  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    return Quaternion(Math::PI, Vector3::ZAXIS);
  }
};

struct GridRotationConstraint270
{
  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    return Quaternion(0.5f * Math::PI, Vector3::ZAXIS);
  }
};

struct GridColorConstraint
{
  Vector4 operator()(const Vector4& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    return Vector4( 1.0f, 1.0f, 1.0f, current.a );
  }
};

struct GridVisibilityConstraintPortrait
{
  GridVisibilityConstraintPortrait(const unsigned int columnIndex, const unsigned int numberOfColumns, const float rowSpacing, const float columnSpacing, const float sideMargin, GridLayout::ItemSizeFunction itemSizeFunction)
  : mColumnIndex(columnIndex),
    mNumberOfColumns(numberOfColumns),
    mRowSpacing(rowSpacing),
    mColumnSpacing(columnSpacing),
    mSideMargin(sideMargin),
    mItemSizeFunction(itemSizeFunction)
  {
  }

  bool operator()(const bool& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(mNumberOfColumns, layoutSize.width, mSideMargin, mColumnSpacing);

    float row = (layoutPosition - static_cast<float>(mColumnIndex)) / mNumberOfColumns;
    int rowsPerPage = ceil(layoutSize.height / (itemSize.y + mRowSpacing));

    return (row > -2.0f) && (row < rowsPerPage);
  }

public:

  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mRowSpacing;
  float mColumnSpacing;
  float mSideMargin;
  GridLayout::ItemSizeFunction mItemSizeFunction;
};

struct GridVisibilityConstraintLandscape
{
  GridVisibilityConstraintLandscape(const unsigned int columnIndex, const unsigned int numberOfColumns, const float rowSpacing, const float columnSpacing, const float sideMargin, GridLayout::ItemSizeFunction itemSizeFunction)
  : mColumnIndex(columnIndex),
    mNumberOfColumns(numberOfColumns),
    mRowSpacing(rowSpacing),
    mColumnSpacing(columnSpacing),
    mSideMargin(sideMargin),
    mItemSizeFunction(itemSizeFunction)
  {
  }

  bool operator()(const bool& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(mNumberOfColumns, layoutSize.height, mSideMargin, mColumnSpacing);

    float row = (layoutPosition - static_cast<float>(mColumnIndex)) / mNumberOfColumns;
    int rowsPerPage = ceil(layoutSize.width / (itemSize.y + mRowSpacing));

    return (row > -2.0f) && (row < rowsPerPage);
  }

public:

  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mRowSpacing;
  float mColumnSpacing;
  float mSideMargin;
  GridLayout::ItemSizeFunction mItemSizeFunction;
};

} // unnamed namespace

namespace Dali
{

namespace Toolkit
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
    mItemFlickAnimationDuration(DEFAULT_ITEM_FLICK_ANIMATION_DURATION),
    mItemSizeFunction(GetItemSizeDefaultFunction)
  {
    mColorConstraint = GridColorConstraint();

    mRotationConstraint[0] = GridRotationConstraint0();
    mRotationConstraint[1] = GridRotationConstraint90();
    mRotationConstraint[2] = GridRotationConstraint180();
    mRotationConstraint[3] = GridRotationConstraint270();
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

  ItemLayout::QuaternionFunction mRotationConstraint[ORIENTATION_COUNT];
  ItemLayout::Vector4Function mColorConstraint;

  ItemSizeFunction mItemSizeFunction;
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

void GridLayout::SetItemSizeFunction(ItemSizeFunction function)
{
  mImpl->mItemSizeFunction = function;
}

GridLayout::ItemSizeFunction GridLayout::GetItemSizeFunction() const
{
  return mImpl->mItemSizeFunction;
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
  float layoutWidth = IsHorizontal(mOrientation) ? layoutSize.height : layoutSize.width;
  float layoutHeight = IsHorizontal(mOrientation) ? layoutSize.width : layoutSize.height;

  Vector3 itemSize = mImpl->mItemSizeFunction( mImpl->mNumberOfColumns, layoutWidth, mImpl->mSideMargin, mImpl->mColumnSpacing);

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
  float rowIndex = static_cast<float>(itemId / mImpl->mNumberOfColumns);
  return -rowIndex * static_cast<float>(mImpl->mNumberOfColumns);
}

ItemRange GridLayout::GetItemsWithinArea(float firstItemPosition, Vector3 layoutSize) const
{
  float layoutWidth = IsHorizontal(mOrientation) ? layoutSize.height : layoutSize.width;
  float layoutHeight = IsHorizontal(mOrientation) ? layoutSize.width : layoutSize.height;

  Vector3 itemSize = mImpl->mItemSizeFunction( mImpl->mNumberOfColumns, layoutWidth, mImpl->mSideMargin, mImpl->mColumnSpacing);

  int itemsPerPage = mImpl->mNumberOfColumns * ceil(layoutHeight / (itemSize.y + mImpl->mRowSpacing));
  int firstVisibleItem = -(static_cast<int>(firstItemPosition / mImpl->mNumberOfColumns)) * mImpl->mNumberOfColumns;

  int firstItemIndex = std::max(0, firstVisibleItem - static_cast<int>(mImpl->mNumberOfColumns));
  int lastItemIndex  = std::max(0, firstVisibleItem + itemsPerPage);

  return ItemRange(firstItemIndex, lastItemIndex);
}

float GridLayout::GetClosestOnScreenLayoutPosition(int itemID, float currentLayoutPosition, const Vector3& layoutSize)
{
  ItemLayout::Vector3Function positionConstraint;
  Vector3 itemPosition = Vector3::ZERO;
  if (GetPositionConstraint(itemID, positionConstraint))
  {
    itemPosition = positionConstraint(Vector3::ZERO, currentLayoutPosition + itemID, 0.0f, layoutSize);
  }
  Vector3 itemSize;
  GetItemSize(itemID, layoutSize, itemSize);
  Vector3 onScreenArea = (layoutSize - (IsVertical(mOrientation) ? itemSize : Vector3(itemSize.y, itemSize.x, itemSize.z))) * 0.5f;
  if (itemPosition.x < -onScreenArea.x
      || itemPosition.x > onScreenArea.x
      || itemPosition.y < -onScreenArea.y
      || itemPosition.y > onScreenArea.y)
  {
    // item not within viewable area
    float rowHeight = itemSize.y + mImpl->mRowSpacing;
    ItemLayout::Vector3Function firstItemPositionConstraint;
    Vector3 firstItemPosition = Vector3::ZERO;
    float offset = 0.0f;
    if (GetPositionConstraint(0, firstItemPositionConstraint))
    {
      firstItemPosition = firstItemPositionConstraint(Vector3::ZERO, 0.0f, 0.0f, layoutSize);
    }
    switch( mOrientation )
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
  float layoutWidth = IsHorizontal(mOrientation) ? layoutSize.height : layoutSize.width;
  float layoutHeight = IsHorizontal(mOrientation) ? layoutSize.width : layoutSize.height;

  Vector3 itemSize = mImpl->mItemSizeFunction( mImpl->mNumberOfColumns, layoutWidth, mImpl->mSideMargin, mImpl->mColumnSpacing);
  int itemsPerPage = mImpl->mNumberOfColumns * ceil(layoutHeight / (itemSize.y + mImpl->mRowSpacing));
  return itemsPerPage;
}

bool GridLayout::GetItemSize(unsigned int itemId, Vector3 layoutSize, Vector3& itemSize) const
{
  // Note: itemId is not checked, since every item has the same size

  itemSize = mImpl->mItemSizeFunction( mImpl->mNumberOfColumns, (IsHorizontal(mOrientation) ? layoutSize.height : layoutSize.width), mImpl->mSideMargin, mImpl->mColumnSpacing);
  return true;
}

void GridLayout::GetResizeAnimation(Animation& animation, Actor actor, Vector3 size, float durationSeconds) const
{
  if(animation)
  {
    Vector3 currentSize( actor.GetCurrentSize() );
    Vector3 shrink( currentSize );

    shrink.width = std::min(size.width, currentSize.width);
    shrink.height = std::min(size.height, currentSize.height);

    // Do a nonlinear size animation to shrink the actor first when the actor size changes,
    // so that we can avoid the actors overlapping during orientation change.
    animation.AnimateTo( Property( actor, Actor::Property::SIZE ), shrink, AlphaFunctions::EaseOut, TimePeriod( 0.0f, durationSeconds * 0.5f ) );
    animation.AnimateTo( Property( actor, Actor::Property::SIZE ), size, AlphaFunctions::EaseIn, TimePeriod( 0.0f, durationSeconds ) );
  }
}

bool GridLayout::GetPositionConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const
{
  unsigned int columnIndex = itemId % mImpl->mNumberOfColumns;

  if (mOrientation == ControlOrientation::Up)
  {
    constraint = GridPositionConstraint0(columnIndex, mImpl->mNumberOfColumns, mImpl->mRowSpacing, mImpl->mColumnSpacing, mImpl->mTopMargin, mImpl->mSideMargin, mImpl->mItemSizeFunction, mImpl->mZGap);
  }
  else if (mOrientation == ControlOrientation::Left)
  {
    constraint = GridPositionConstraint90(columnIndex, mImpl->mNumberOfColumns, mImpl->mRowSpacing, mImpl->mColumnSpacing, mImpl->mTopMargin, mImpl->mSideMargin, mImpl->mItemSizeFunction, mImpl->mZGap);
  }
  else if (mOrientation == ControlOrientation::Down)
  {
    constraint = GridPositionConstraint180(columnIndex, mImpl->mNumberOfColumns, mImpl->mRowSpacing, mImpl->mColumnSpacing, mImpl->mTopMargin, mImpl->mSideMargin, mImpl->mItemSizeFunction, mImpl->mZGap);
  }
  else // mOrientation == ControlOrientation::Right
  {
    constraint = GridPositionConstraint270(columnIndex, mImpl->mNumberOfColumns, mImpl->mRowSpacing, mImpl->mColumnSpacing, mImpl->mTopMargin, mImpl->mSideMargin, mImpl->mItemSizeFunction, mImpl->mZGap);
  }

  return true;
}

bool GridLayout::GetRotationConstraint(unsigned int itemId, ItemLayout::QuaternionFunction& constraint) const
{
  constraint = mImpl->mRotationConstraint[mOrientation];
  return true;
}

bool GridLayout::GetScaleConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const
{
  return false; // No scaling
}

bool GridLayout::GetColorConstraint(unsigned int itemId, ItemLayout::Vector4Function& constraint) const
{
  constraint = mImpl->mColorConstraint;
  return true;
}

bool GridLayout::GetVisibilityConstraint(unsigned int itemId, ItemLayout::BoolFunction& constraint) const
{
  unsigned int columnIndex = itemId % mImpl->mNumberOfColumns;

  if (IsVertical(mOrientation))
  {
    constraint = GridVisibilityConstraintPortrait(columnIndex, mImpl->mNumberOfColumns, mImpl->mRowSpacing, mImpl->mColumnSpacing, mImpl->mSideMargin, mImpl->mItemSizeFunction);
  }
  else // horizontal
  {
    constraint = GridVisibilityConstraintLandscape(columnIndex, mImpl->mNumberOfColumns, mImpl->mRowSpacing, mImpl->mColumnSpacing, mImpl->mSideMargin, mImpl->mItemSizeFunction);
  }

  return true;
}

Degree GridLayout::GetScrollDirection() const
{
  Degree scrollDirection(0.0f);

  if (mOrientation == ControlOrientation::Up)
  {
    scrollDirection = 0.0f;
  }
  else if (mOrientation == ControlOrientation::Left)
  {
    scrollDirection = 90.0f;
  }
  else if (mOrientation == ControlOrientation::Down)
  {
    scrollDirection = 180.0f;
  }
  else // mOrientation == ControlOrientation::Right
  {
    scrollDirection = 270.0f;
  }

  return scrollDirection;
}

int GridLayout::GetNextFocusItemID(int itemID, int maxItems, Dali::Toolkit::Control::KeyboardFocusNavigationDirection direction, bool loopEnabled)
{
  switch( direction )
  {
    case Control::Left:
    {
      itemID--;
      if( itemID < 0 )
      {
        itemID = loopEnabled ? maxItems - 1 : 0;
      }
      break;
    }
    case Control::Up:
    {
      itemID -= mImpl->mNumberOfColumns;
      if( itemID < 0 )
      {
        itemID = loopEnabled ? itemID + maxItems : itemID + mImpl->mNumberOfColumns;
      }
      break;
    }
    case Control::Right:
    {
      itemID++;
      if( itemID >= maxItems )
      {
        itemID = loopEnabled ? 0 : maxItems - 1;
      }
      break;
    }
    case Control::Down:
    {
      itemID += mImpl->mNumberOfColumns;
      if( itemID >= maxItems )
      {
        itemID = loopEnabled ? 0 : itemID - mImpl->mNumberOfColumns;
      }
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

} // namespace Toolkit

} // namespace Dali
