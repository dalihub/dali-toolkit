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
#include <dali-toolkit/public-api/controls/scrollable/item-view/depth-layout.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <dali/public-api/animation/animation.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace // unnamed namespace
{

const unsigned int DEFAULT_NUMBER_OF_COLUMNS    = 3;
const float        DEFAULT_NUMBER_OF_ROWS       = 20.0f;
const float        DEFAULT_ROW_SPACING          = 55.0f;
const float        DEFAULT_BOTTOM_MARGIN_FACTOR = 0.1f;
const Radian       DEFAULT_TILT_ANGLE           ( Math::PI*0.12f );
const Radian       DEFAULT_ITEM_TILT_ANGLE      ( -Math::PI*0.025f );
const float        DEFAULT_SCROLL_SPEED_FACTOR  = 0.02f;
const float        DEFAULT_MAXIMUM_SWIPE_SPEED  = 50.0f;
const float        DEFAULT_ITEM_FLICK_ANIMATION_DURATION = 0.03f;

static Vector3 GetItemSizeDefaultFunction(unsigned int numberOfColumns, float layoutWidth)
{
  float width = layoutWidth / static_cast<float>(numberOfColumns + 1);

  // 1x1 aspect ratio
  return Vector3(width, width, width);
}

static float GetBottomMarginDefaultFunction(float layoutHeight)
{
  return layoutHeight * DEFAULT_BOTTOM_MARGIN_FACTOR;
}

struct GetColumnPositionDefaultFunction
{
  float operator()(unsigned int numberOfColumns,
                   unsigned int columnNumber,
                   const Vector3& itemSize,
                   float layoutWidth)
  {
    // Share the available space between margins & column spacings
    float availableSpace = std::max(0.0f, (layoutWidth - itemSize.width*numberOfColumns));

    float leftMargin = availableSpace/numberOfColumns * 0.5f;

    float columnPosition = leftMargin + itemSize.width*0.5f + columnNumber*(itemSize.width + availableSpace/numberOfColumns);

    return columnPosition - layoutWidth*0.5f;
  }
};

struct DepthPositionConstraint0
{
  DepthPositionConstraint0(unsigned int numberOfColumns,
                           unsigned int columnNumber,
                           DepthLayout::ItemSizeFunction itemSizeFunction,
                           DepthLayout::BottomMarginFunction bottomMarginFunction,
                           DepthLayout::ColumnPositionFunction columnPositionFunction,
                           float heightScale,
                           float depthScale)
  : mNumberOfColumns(numberOfColumns),
    mColumnNumber(columnNumber),
    mItemSizeFunction(itemSizeFunction),
    mBottomMarginFunction(bottomMarginFunction),
    mColumnPositionFunction(columnPositionFunction),
    mHeightScale(heightScale),
    mDepthScale(depthScale)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(mNumberOfColumns, layoutSize.width);

    float rowLayoutPositon = layoutPosition - static_cast<float>(mColumnNumber);

    return Vector3(  mColumnPositionFunction(mNumberOfColumns, mColumnNumber, itemSize, layoutSize.width),
                     rowLayoutPositon*mHeightScale + layoutSize.height*0.5f - mBottomMarginFunction(layoutSize.height) - itemSize.height * 0.5f,
                    -rowLayoutPositon*mDepthScale );
  }

  unsigned int mNumberOfColumns;
  unsigned int mColumnNumber;

  DepthLayout::ItemSizeFunction       mItemSizeFunction;
  DepthLayout::BottomMarginFunction   mBottomMarginFunction;
  DepthLayout::ColumnPositionFunction mColumnPositionFunction;

  float mHeightScale;
  float mDepthScale;
};

struct DepthPositionConstraint90
{
  DepthPositionConstraint90(unsigned int numberOfColumns,
                            unsigned int columnNumber,
                            DepthLayout::ItemSizeFunction itemSizeFunction,
                            DepthLayout::BottomMarginFunction bottomMarginFunction,
                            DepthLayout::ColumnPositionFunction columnPositionFunction,
                            float heightScale,
                            float depthScale)
  : mNumberOfColumns(numberOfColumns),
    mColumnNumber(columnNumber),
    mItemSizeFunction(itemSizeFunction),
    mBottomMarginFunction(bottomMarginFunction),
    mColumnPositionFunction(columnPositionFunction),
    mHeightScale(heightScale),
    mDepthScale(depthScale)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(mNumberOfColumns, layoutSize.height);

    float rowLayoutPositon = layoutPosition - static_cast<float>(mColumnNumber) + mNumberOfColumns*0.5f;

    return Vector3(  rowLayoutPositon*mHeightScale + layoutSize.width*0.5f - mBottomMarginFunction(layoutSize.width) - itemSize.height * 0.5f,
                     -mColumnPositionFunction(mNumberOfColumns, mColumnNumber, itemSize, layoutSize.height),
                    -rowLayoutPositon*mDepthScale );
  }

  unsigned int mNumberOfColumns;
  unsigned int mColumnNumber;

  DepthLayout::ItemSizeFunction       mItemSizeFunction;
  DepthLayout::BottomMarginFunction   mBottomMarginFunction;
  DepthLayout::ColumnPositionFunction mColumnPositionFunction;

  float mHeightScale;
  float mDepthScale;
};

struct DepthPositionConstraint180
{
  DepthPositionConstraint180(unsigned int numberOfColumns,
                             unsigned int columnNumber,
                             DepthLayout::ItemSizeFunction itemSizeFunction,
                             DepthLayout::BottomMarginFunction bottomMarginFunction,
                             DepthLayout::ColumnPositionFunction columnPositionFunction,
                             float heightScale,
                             float depthScale)
  : mNumberOfColumns(numberOfColumns),
    mColumnNumber(columnNumber),
    mItemSizeFunction(itemSizeFunction),
    mBottomMarginFunction(bottomMarginFunction),
    mColumnPositionFunction(columnPositionFunction),
    mHeightScale(heightScale),
    mDepthScale(depthScale)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(mNumberOfColumns, layoutSize.width);

    float rowLayoutPositon = layoutPosition - static_cast<float>(mColumnNumber);

    return Vector3( -mColumnPositionFunction(mNumberOfColumns, mColumnNumber, itemSize, layoutSize.width),
                    -(rowLayoutPositon*mHeightScale + layoutSize.height*0.5f - mBottomMarginFunction(layoutSize.height) - itemSize.height * 0.5f),
                    -rowLayoutPositon*mDepthScale );
  }

  unsigned int mNumberOfColumns;
  unsigned int mColumnNumber;

  DepthLayout::ItemSizeFunction       mItemSizeFunction;
  DepthLayout::BottomMarginFunction   mBottomMarginFunction;
  DepthLayout::ColumnPositionFunction mColumnPositionFunction;

  float mHeightScale;
  float mDepthScale;
};

struct DepthPositionConstraint270
{
  DepthPositionConstraint270(unsigned int numberOfColumns,
                             unsigned int columnNumber,
                             DepthLayout::ItemSizeFunction itemSizeFunction,
                             DepthLayout::BottomMarginFunction bottomMarginFunction,
                             DepthLayout::ColumnPositionFunction columnPositionFunction,
                             float heightScale,
                             float depthScale)
  : mNumberOfColumns(numberOfColumns),
    mColumnNumber(columnNumber),
    mItemSizeFunction(itemSizeFunction),
    mBottomMarginFunction(bottomMarginFunction),
    mColumnPositionFunction(columnPositionFunction),
    mHeightScale(heightScale),
    mDepthScale(depthScale)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(mNumberOfColumns, layoutSize.height);

    float rowLayoutPositon = layoutPosition - static_cast<float>(mColumnNumber) + mNumberOfColumns*0.5f;

    return Vector3( -(rowLayoutPositon*mHeightScale + layoutSize.width*0.5f - mBottomMarginFunction(layoutSize.width) - itemSize.height * 0.5f),
                    mColumnPositionFunction(mNumberOfColumns, mColumnNumber, itemSize, layoutSize.height),
                    -rowLayoutPositon*mDepthScale );
  }

  unsigned int mNumberOfColumns;
  unsigned int mColumnNumber;

  DepthLayout::ItemSizeFunction       mItemSizeFunction;
  DepthLayout::BottomMarginFunction   mBottomMarginFunction;
  DepthLayout::ColumnPositionFunction mColumnPositionFunction;

  float mHeightScale;
  float mDepthScale;
};

struct DepthRotationConstraint0
{
  DepthRotationConstraint0(Radian angleRadians)
  : mTiltAngle(angleRadians)
  {
  }

  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    return Quaternion( Radian( 0.0f ), Vector3::ZAXIS ) * Quaternion( mTiltAngle, Vector3::XAXIS );
  }

  Radian mTiltAngle;
};

struct DepthRotationConstraint90
{
  DepthRotationConstraint90(Radian angleRadians)
  : mTiltAngle(angleRadians)
  {
  }

  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    return Quaternion( Radian( 1.5f * Math::PI ), Vector3::ZAXIS) * Quaternion( mTiltAngle, Vector3::XAXIS );
  }

  Radian mTiltAngle;
};

struct DepthRotationConstraint180
{
  DepthRotationConstraint180(Radian angleRadians)
  : mTiltAngle(angleRadians)
  {
  }

  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    return Quaternion( Radian( -Math::PI ), Vector3::ZAXIS) * Quaternion( mTiltAngle, Vector3::XAXIS );
  }

  Radian mTiltAngle;
};

struct DepthRotationConstraint270
{
  DepthRotationConstraint270(Radian angleRadians)
  : mTiltAngle(angleRadians)
  {
  }

  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    return Quaternion( Radian( 0.5f * Math::PI ), Vector3::ZAXIS) * Quaternion( mTiltAngle, Vector3::XAXIS );
  }

  Radian mTiltAngle;
};

struct DepthColorConstraint
{
  DepthColorConstraint(unsigned int numberOfColumns, float numberOfRows, unsigned int columnNumber)
  : mNumberOfColumns(numberOfColumns),
    mNumberOfRows(numberOfRows),
    mColumnNumber(columnNumber)
  {
  }

  Vector4 operator()(const Vector4& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float row = (layoutPosition - static_cast<float>(mColumnNumber)) / mNumberOfColumns;

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

    return Vector4( darkness, darkness, darkness, current.a * alpha );
  }

  unsigned int mNumberOfColumns;
  float mNumberOfRows;
  unsigned int mColumnNumber;
};

struct DepthVisibilityConstraint
{
  DepthVisibilityConstraint(unsigned int numberOfColumns, float numberOfRows, unsigned int columnNumber)
  : mNumberOfColumns(numberOfColumns),
    mNumberOfRows(numberOfRows),
    mColumnNumber(columnNumber)
  {
  }

  bool operator()(const bool& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float row = (layoutPosition - static_cast<float>(mColumnNumber)) / mNumberOfColumns;
    return (row > -1.0f) && (row < mNumberOfRows);
  }

  unsigned int mNumberOfColumns;
  float mNumberOfRows;
  unsigned int mColumnNumber;
};

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

struct PositionConstraintSet
{
  ItemLayout::Vector3Function mOrientation0;
  ItemLayout::Vector3Function mOrientation90;
  ItemLayout::Vector3Function mOrientation180;
  ItemLayout::Vector3Function mOrientation270;
};

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
    mItemFlickAnimationDuration(DEFAULT_ITEM_FLICK_ANIMATION_DURATION),
    mItemSizeFunction(GetItemSizeDefaultFunction),
    mBottomMarginFunction(GetBottomMarginDefaultFunction),
    mColumnPositionFunction(GetColumnPositionDefaultFunction())
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

  ItemSizeFunction        mItemSizeFunction;
  BottomMarginFunction    mBottomMarginFunction;
  ColumnPositionFunction  mColumnPositionFunction;
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

void DepthLayout::SetItemSizeFunction(ItemSizeFunction function)
{
  mImpl->mItemSizeFunction = function;
}

DepthLayout::ItemSizeFunction DepthLayout::GetItemSizeFunction() const
{
  return mImpl->mItemSizeFunction;
}

void DepthLayout::SetBottomMarginFunction(BottomMarginFunction function)
{
  mImpl->mBottomMarginFunction = function;
}

DepthLayout::BottomMarginFunction DepthLayout::GetBottomMarginFunction() const
{
  return mImpl->mBottomMarginFunction;
}

void DepthLayout::SetItemTiltAngle(Degree angle)
{
  mImpl->mItemTiltAngle = angle;
}

Degree DepthLayout::GetItemTiltAngle() const
{
  return Degree( mImpl->mItemTiltAngle );
}

void DepthLayout::SetColumnPositionFunction(ColumnPositionFunction function)
{
  mImpl->mColumnPositionFunction = function;
}

DepthLayout::ColumnPositionFunction DepthLayout::GetColumnPositionFunction() const
{
  return mImpl->mColumnPositionFunction;
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
  float rowIndex = static_cast<float>(itemId / mImpl->mNumberOfColumns);
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

bool DepthLayout::GetItemSize(unsigned int itemId, Vector3 layoutSize, Vector3& itemSize) const
{
  // Note: itemId is not checked, since every item has the same size

  itemSize = mImpl->mItemSizeFunction( mImpl->mNumberOfColumns, (IsVertical(mOrientation) ? layoutSize.width : layoutSize.height) );
  return true;
}

void DepthLayout::GetResizeAnimation(Animation& animation, Actor actor, Vector3 size, float durationSeconds) const
{
  if(animation)
  {
    animation.AnimateTo( Property( actor, Actor::Property::SIZE ), size );
  }
}

bool DepthLayout::GetPositionConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const
{
  float heightScale = -sinf(mImpl->mTiltAngle) * mImpl->mRowSpacing;
  float depthScale  =  cosf(mImpl->mTiltAngle) * mImpl->mRowSpacing;

  if (mOrientation == ControlOrientation::Up)
  {
    constraint = DepthPositionConstraint0( mImpl->mNumberOfColumns,
                                           itemId % mImpl->mNumberOfColumns,
                                           mImpl->mItemSizeFunction,
                                           mImpl->mBottomMarginFunction,
                                           mImpl->mColumnPositionFunction,
                                           heightScale,
                                           depthScale );
  }
  else if (mOrientation == ControlOrientation::Left)
  {
    constraint = DepthPositionConstraint90( mImpl->mNumberOfColumns,
                                            itemId % mImpl->mNumberOfColumns,
                                            mImpl->mItemSizeFunction,
                                            mImpl->mBottomMarginFunction,
                                            mImpl->mColumnPositionFunction,
                                            heightScale,
                                            depthScale );
  }
  else if (mOrientation == ControlOrientation::Down)
  {
    constraint = DepthPositionConstraint180( mImpl->mNumberOfColumns,
                                             itemId % mImpl->mNumberOfColumns,
                                             mImpl->mItemSizeFunction,
                                             mImpl->mBottomMarginFunction,
                                             mImpl->mColumnPositionFunction,
                                             heightScale,
                                             depthScale );
  }
  else // mOrientation == ControlOrientation::Right
  {
    constraint = DepthPositionConstraint270( mImpl->mNumberOfColumns,
                                             itemId % mImpl->mNumberOfColumns,
                                             mImpl->mItemSizeFunction,
                                             mImpl->mBottomMarginFunction,
                                             mImpl->mColumnPositionFunction,
                                             heightScale,
                                             depthScale );
  }

  return true;
}

bool DepthLayout::GetRotationConstraint(unsigned int itemId, ItemLayout::QuaternionFunction& constraint) const
{
  if (mOrientation == ControlOrientation::Up)
  {
    constraint = DepthRotationConstraint0(mImpl->mItemTiltAngle);
  }
  else if (mOrientation == ControlOrientation::Left)
  {
    constraint = DepthRotationConstraint90(mImpl->mItemTiltAngle);
  }
  else if (mOrientation == ControlOrientation::Down)
  {
    constraint = DepthRotationConstraint180(mImpl->mItemTiltAngle);
  }
  else // mOrientation == ControlOrientation::Right
  {
    constraint = DepthRotationConstraint270(mImpl->mItemTiltAngle);
  }

  return true;
}

bool DepthLayout::GetScaleConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const
{
  return false; // No scaling
}

bool DepthLayout::GetColorConstraint(unsigned int itemId, ItemLayout::Vector4Function& constraint) const
{
  constraint = DepthColorConstraint(mImpl->mNumberOfColumns, mImpl->mNumberOfRows*0.5f, itemId % mImpl->mNumberOfColumns);
  return true;
}

bool DepthLayout::GetVisibilityConstraint(unsigned int itemId, ItemLayout::BoolFunction& constraint) const
{
  constraint = DepthVisibilityConstraint(mImpl->mNumberOfColumns, mImpl->mNumberOfRows*0.5f, itemId % mImpl->mNumberOfColumns);
  return true;
}

Degree DepthLayout::GetScrollDirection() const
{
  Degree scrollDirection(0.0f);

  if (mOrientation == ControlOrientation::Up)
  {
    scrollDirection = Degree( 180.0f );
  }
  else if (mOrientation == ControlOrientation::Left)
  {
    scrollDirection = Degree( 270.0f );
  }
  else if (mOrientation == ControlOrientation::Down)
  {
    scrollDirection = Degree( 0.0f );
  }
  else // mOrientation == ControlOrientation::Right
  {
    scrollDirection = Degree( 90.0f );
  }

  return scrollDirection;
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

int DepthLayout::GetNextFocusItemID(int itemID, int maxItems, Dali::Toolkit::Control::KeyboardFocusNavigationDirection direction, bool loopEnabled)
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
      itemID += mImpl->mNumberOfColumns;
      if( itemID >= maxItems )
      {
        itemID = loopEnabled ? 0 : itemID - mImpl->mNumberOfColumns;
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
      itemID -= mImpl->mNumberOfColumns;
      if( itemID < 0 )
      {
        itemID = loopEnabled ? itemID + maxItems : itemID + mImpl->mNumberOfColumns;
      }
      break;
    }
  }
  return itemID;
}

} // namespace Toolkit

} // namespace Dali
