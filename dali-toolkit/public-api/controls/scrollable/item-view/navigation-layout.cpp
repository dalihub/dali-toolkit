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

#include <algorithm>

#include <dali-toolkit/public-api/controls/scrollable/item-view/navigation-layout.h>
using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;

namespace // unnamed namespace
{
const unsigned int DEFAULT_NUMBER_OF_COLUMNS = 3;
const float DEFAULT_TOP_MARGIN     =  0.3f;
const float DEFAULT_BOTTOM_MARGIN  =  0.3f;
const float DEFAULT_SIDE_MARGIN    =  0.2f;
const float DEFAULT_COLUMN_SPACING =  20.0f;
const float DEFAULT_ROW_SPACING    =  20.0f;
const float DEFAULT_SCROLL_SPEED_FACTOR = 0.01f;
const float DEFAULT_MAXIMUM_SWIPE_SPEED = 3.0f;
const float DEFAULT_ITEM_FLICK_ANIMATION_DURATION = 0.05f;
const float DEFAULT_SIZE_EXTEND = 1.4f;
const float DEFAULT_HEIGHT_FACTOR = 0.6f;

// 4 orientations are supported
struct NavigationPositionConstraintUp
{
  NavigationPositionConstraintUp(const unsigned int columnIndex,
                                         const unsigned int numberOfColumns,
                                         const float columnSpacing,
                                         const float sizeExtend,
                                         const float bottomMargin,
                                         const float topMargin)
  : mColumnIndex(columnIndex),
    mNumberOfColumns(numberOfColumns),
    mColumnSpacing(columnSpacing),
    mSizeExtend(sizeExtend),
    mBottomMargin(bottomMargin),
    mTopMargin(topMargin)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float itemWidth = (layoutSize.width * mSizeExtend- mColumnSpacing * (mNumberOfColumns - 1))/( mNumberOfColumns );

    Vector3 itemPosition;

    float z = (sinf((layoutPosition + 1.0f) * Math::PI *0.5f) - 1.0f) * itemWidth * 2.0f;

    itemPosition = Vector3( (layoutPosition + 1.0f) * (itemWidth + mColumnSpacing) + itemWidth * 0.5f - layoutSize.width * mSizeExtend * 0.5f,
                             (- mBottomMargin + mTopMargin) * layoutSize.width * 0.5f ,
                             z);
    return itemPosition;
  }

public:
  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mColumnSpacing;
  float mSizeExtend;
  float mBottomMargin;
  float mTopMargin;
};

struct NavigationPositionConstraintLeft
{
  NavigationPositionConstraintLeft(const unsigned int columnIndex,
                                          const unsigned int numberOfColumns,
                                          const float columnSpacing,
                                          const float sizeExtend,
                                          const float bottomMargin,
                                          const float topMargin)
  : mColumnIndex(columnIndex),
    mNumberOfColumns(numberOfColumns),
    mColumnSpacing(columnSpacing),
    mSizeExtend(sizeExtend),
    mBottomMargin(bottomMargin),
    mTopMargin(topMargin)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float itemWidth = (DEFAULT_HEIGHT_FACTOR * layoutSize.height * mSizeExtend- mColumnSpacing * (mNumberOfColumns - 1))/( mNumberOfColumns );
    Vector3 itemPosition;
    float z = (sinf((layoutPosition + 1.0f) * Math::PI *0.5f) - 1.0f) * itemWidth * 1.5f;
    itemPosition = Vector3( (- mBottomMargin + mTopMargin) * 0.5f * layoutSize.width ,
                            -((layoutPosition+ 1.0) * (itemWidth + mColumnSpacing) + itemWidth * 0.5f - DEFAULT_HEIGHT_FACTOR * layoutSize.height * mSizeExtend * 0.5f ),
                            z);
    return itemPosition;
  }

public:
  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mColumnSpacing;
  float mSizeExtend;
  float mBottomMargin;
  float mTopMargin;
};

struct NavigationPositionConstraintDown
{
  NavigationPositionConstraintDown(const unsigned int columnIndex,
                                            const unsigned int numberOfColumns,
                                            const float columnSpacing,
                                            const float sizeExtend,
                                            const float bottomMargin,
                                            const float topMargin)
  : mColumnIndex(columnIndex),
    mNumberOfColumns(numberOfColumns),
    mColumnSpacing(columnSpacing),
    mSizeExtend(sizeExtend),
    mBottomMargin(bottomMargin),
    mTopMargin(topMargin)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float itemWidth = (layoutSize.width * mSizeExtend- mColumnSpacing * (mNumberOfColumns - 1))/( mNumberOfColumns );
    Vector3 itemPosition;

    float z = (sinf((layoutPosition + 1.0f ) * Math::PI *0.5f) - 1.0f) * itemWidth * 2.0f;
    itemPosition = Vector3(  -((layoutPosition + 1.0f)  * (itemWidth + mColumnSpacing) + itemWidth * 0.5f - layoutSize.width * mSizeExtend * 0.5f),
                             (- mBottomMargin + mTopMargin)* layoutSize.width * 0.5f,
                             z);
    return itemPosition;
  }

  public:
    unsigned int mColumnIndex;
    unsigned int mNumberOfColumns;
    float mColumnSpacing;
    float mSizeExtend;
    float mBottomMargin;
    float mTopMargin;
};

struct NavigationPositionConstraintRight
{
  NavigationPositionConstraintRight(const unsigned int columnIndex,
                                            const unsigned int numberOfColumns,
                                            const float columnSpacing,
                                            const float sizeExtend,
                                            const float bottomMargin,
                                            const float topMargin)
  : mColumnIndex(columnIndex),
    mNumberOfColumns(numberOfColumns),
    mColumnSpacing(columnSpacing),
    mSizeExtend(sizeExtend),
    mBottomMargin(bottomMargin),
    mTopMargin(topMargin)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float itemWidth = (DEFAULT_HEIGHT_FACTOR * layoutSize.height * mSizeExtend- mColumnSpacing * (mNumberOfColumns - 1))/( mNumberOfColumns );
    Vector3 itemPosition;
    float z = (sinf((layoutPosition + 1.0f) * Math::PI *0.5f) - 1.0f) * itemWidth * 1.5f;
    itemPosition = Vector3(  (- mBottomMargin + mTopMargin) * layoutSize.width * 0.5f,
                             ((layoutPosition + 1.0f) * (itemWidth + mColumnSpacing) + itemWidth * 0.5f - DEFAULT_HEIGHT_FACTOR * layoutSize.height * mSizeExtend *0.5f ),
                             z);
  return itemPosition;
  }

public:
  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mColumnSpacing;
  float mSizeExtend;
  float mBottomMargin;
  float mTopMargin;
};

struct NavigationRotationConstraintUp
{
  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float angle = 0.0f;
    float _layoutPosition = layoutPosition + 1.0f;
    if(_layoutPosition >= 1.0f)
    {
      angle = - sinf(Math::PI * _layoutPosition) * Math::PI * 0.2f;
    }
    else
    {
      angle =  sinf(Math::PI * _layoutPosition) * Math::PI * 0.2f;
    }
    return Quaternion(angle, Vector3::YAXIS);
  }

};

struct NavigationRotationConstraintLeft
{
  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float angle = 0.0f;
    float _layoutPosition = layoutPosition + 1.0f;
    if(_layoutPosition >= 1.0f)
    {
      angle = - sinf(Math::PI * _layoutPosition) * Math::PI * 0.2f;
    }
    else
    {
      angle =  sinf(Math::PI * _layoutPosition) * Math::PI * 0.2f;
    }
    return Quaternion(Math::PI * 0.5f, Vector3::ZAXIS) * Quaternion(angle, Vector3::YAXIS);
  }
};

struct NavigationRotationConstraintDown
{
  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float angle = 0.0f;
    float _layoutPosition = layoutPosition + 1.0f;
    if(_layoutPosition >= 1.0f)//right side
    {
      //rotation angle by z axis
      angle = - sinf(Math::PI * _layoutPosition) * Math::PI * 0.2f;
    }
    else // left side
    {
      angle =  sinf(Math::PI * _layoutPosition) * Math::PI * 0.2f;
    }
    return Quaternion(Math::PI, Vector3::ZAXIS) * Quaternion(angle, Vector3::YAXIS);
  }
};

struct NavigationRotationConstraintRight
{
  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float angle = 0.0f;
    float _layoutPosition = layoutPosition + 1.0f;
    if(_layoutPosition >= 1.0f)
    {
      angle = - sinf(Math::PI * _layoutPosition) * Math::PI * 0.2f;
    }
    else
    {
      angle =  sinf(Math::PI * _layoutPosition) * Math::PI * 0.2f;
    }
    return Quaternion(Math::PI * 1.5f, Vector3::ZAXIS) * Quaternion(angle, Vector3::YAXIS);
  }
};

struct NavigationColorConstraint
{
  NavigationColorConstraint(unsigned int numberOfColumns)
  : mNumberOfColumns(numberOfColumns)
  {

  }
  Vector4 operator()(const Vector4& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float darkness = 1.0f;
    float alpha = 1.0f;

    float pos = ( layoutPosition + 1.0f);
    darkness = (-0.25f) * (pos + 1.0f) * (pos + 1.0f) + 1.0f * (pos + 1.0f) + 0.2f;

    darkness = fabs(darkness);
    darkness /= 1.2f;

    return Vector4(darkness, darkness, darkness, current.a * alpha);
  }
  unsigned int mNumberOfColumns;

};

struct NavigationVisibilityConstraint
{
  NavigationVisibilityConstraint(const unsigned int columnIndex,
                                      const unsigned int numberOfColumns,
                                      const float columnSpacing )
  : mColumnIndex(columnIndex),
    mNumberOfColumns(numberOfColumns),
    mColumnSpacing(columnSpacing)
  {
  }

  bool operator()(const bool& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float index = layoutPosition + 1.0f;
    return (index >= -1.0f) && (index <= mNumberOfColumns );
  }

public:
  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mColumnSpacing;
};
}//end namespace

namespace Dali
{
namespace Toolkit
{
struct NavigationLayout::Impl
{
  Impl()
  : mNumberOfColumns(DEFAULT_NUMBER_OF_COLUMNS),
    mColumnSpacing(DEFAULT_COLUMN_SPACING),
    mTopMargin(DEFAULT_TOP_MARGIN),
    mBottomMargin(DEFAULT_BOTTOM_MARGIN),
    mSideMargin(DEFAULT_SIDE_MARGIN),
    mScrollSpeedFactor(DEFAULT_SCROLL_SPEED_FACTOR),
    mMaximumSwipeSpeed(DEFAULT_MAXIMUM_SWIPE_SPEED),
    mItemFlickAnimationDuration(DEFAULT_ITEM_FLICK_ANIMATION_DURATION),
    mSizeExtend(DEFAULT_SIZE_EXTEND)
    {
      mColorConstraint = NavigationColorConstraint(mNumberOfColumns);
      mRotationConstraint[0] = NavigationRotationConstraintUp();
      mRotationConstraint[1] = NavigationRotationConstraintLeft();
      mRotationConstraint[2] = NavigationRotationConstraintDown();
      mRotationConstraint[3] = NavigationRotationConstraintRight();
    }

  unsigned int mNumberOfColumns;
  float mColumnSpacing;
  float mTopMargin;
  float mBottomMargin;
  float mSideMargin;
  float mScrollSpeedFactor;
  float mMaximumSwipeSpeed;
  float mItemFlickAnimationDuration;
  float mSizeExtend;

  ItemLayout::QuaternionFunction mRotationConstraint[4];

  ItemLayout::Vector4Function mColorConstraint;

  NavigationLayout::NavigationSignalV2 mSignalPanV2;/*the signal to notify the application the selected item*/
};

NavigationLayoutPtr NavigationLayout::New()
{
  return NavigationLayoutPtr(new NavigationLayout());
}

NavigationLayout::~NavigationLayout()
{
  delete mImpl;
}

void NavigationLayout::SetNumberOfColumns(unsigned int columns)
{
  mImpl->mNumberOfColumns = columns;
}

unsigned int NavigationLayout::GetNumberOfColumns() const
{
  return mImpl->mNumberOfColumns;
}

void NavigationLayout::SetColumnSpacing(float spacing)
{
  mImpl->mColumnSpacing = spacing;
}

float NavigationLayout::GetColumnSpacing() const
{
  return mImpl->mColumnSpacing;
}

void NavigationLayout::SetTopMargin(float margin)
{
  mImpl->mTopMargin = margin;
}

float NavigationLayout::GetTopMargin() const
{
  return mImpl->mTopMargin;
}

void NavigationLayout::SetBottomMargin(float margin)
{
  mImpl->mBottomMargin = margin;
}

float NavigationLayout::GetBottomMargin() const
{
  return mImpl->mBottomMargin;
}

void NavigationLayout::SetSideMargin(float margin)
{
  mImpl->mSideMargin = margin;
  mImpl->mSizeExtend = (1.0f - margin) * 3.0f;
}

void NavigationLayout::SetScrollSpeedFactor(float scrollSpeed)
{
  mImpl->mScrollSpeedFactor = scrollSpeed;
}

void NavigationLayout::SetMaximumSwipeSpeed(float speed)
{
  mImpl->mMaximumSwipeSpeed = speed;
}

void NavigationLayout::SetItemFlickAnimationDuration(float durationSeconds)
{
  mImpl->mItemFlickAnimationDuration = durationSeconds;
}

float NavigationLayout::GetScrollSpeedFactor() const
{
  return mImpl->mScrollSpeedFactor;
}

float NavigationLayout::GetMaximumSwipeSpeed() const
{
  return mImpl->mMaximumSwipeSpeed;
}

float NavigationLayout::GetItemFlickAnimationDuration() const
{
  return mImpl->mItemFlickAnimationDuration;
}

float NavigationLayout::GetMinimumLayoutPosition(unsigned int numberOfItems, Vector3 layoutSize) const
{
  unsigned int itemsLastRow = numberOfItems % mImpl->mNumberOfColumns;
  if (itemsLastRow == 0)
  {
    itemsLastRow = mImpl->mNumberOfColumns;
  }

  float itemsLastPage =  static_cast<float>(itemsLastRow);
  return itemsLastPage - static_cast<float>(numberOfItems) - 2.0f;

}

float NavigationLayout::GetClosestAnchorPosition(float layoutPosition) const
{
  return round(layoutPosition);
}

float NavigationLayout::GetItemScrollToPosition(unsigned int itemId) const
{
  return - static_cast<float>(itemId);
}

ItemRange NavigationLayout::GetItemsWithinArea(float firstItemPosition, Vector3 layoutSize) const
{
  int itemsPerPage = mImpl->mNumberOfColumns;

  int firstItemIndex = std::max(0.0f, -firstItemPosition -1.0f  );
  int lastItemIndex = std::max(0.0f, -(firstItemPosition) + itemsPerPage );

  mImpl->mSignalPanV2.Emit(lastItemIndex - mImpl->mNumberOfColumns);
  return ItemRange(firstItemIndex , lastItemIndex );
}

unsigned int NavigationLayout::GetReserveItemCount(Vector3 layoutSize) const
{
  float layoutWidth = IsHorizontal(mOrientation) ? layoutSize.height : layoutSize.width;
  float itemWidth = (layoutWidth * mImpl->mSizeExtend - mImpl->mColumnSpacing * (mImpl->mNumberOfColumns - 1))/( mImpl->mNumberOfColumns );
  return static_cast<unsigned int>(layoutWidth / itemWidth);
}

bool NavigationLayout::GetItemSize(unsigned int itemId, Vector3 layoutSize, Vector3& itemSize) const
{
  float layoutWidth = IsHorizontal(mOrientation) ? (DEFAULT_HEIGHT_FACTOR * layoutSize.height) : layoutSize.width;
  layoutWidth = layoutWidth * mImpl->mSizeExtend;

  float itemWidth = (layoutWidth -  mImpl->mColumnSpacing*(mImpl->mNumberOfColumns-1)) / mImpl->mNumberOfColumns;
  float itemHeight = layoutWidth * (1.0f - mImpl->mBottomMargin - mImpl->mTopMargin);
  itemSize = Vector3(itemWidth, itemHeight, (itemWidth/4)*3);

  return true;
}

void NavigationLayout::GetResizeAnimation(Animation& animation, Actor actor, Vector3 size, float durationSeconds) const
{
}

bool NavigationLayout::GetPositionConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const
{
  unsigned int columnIndex = itemId % mImpl->mNumberOfColumns;
  if (mOrientation == ControlOrientation::Left)
  {
    constraint = NavigationPositionConstraintLeft(columnIndex, mImpl->mNumberOfColumns, mImpl->mColumnSpacing, mImpl->mSizeExtend, mImpl->mBottomMargin, mImpl->mTopMargin);
  }
  else if (mOrientation == ControlOrientation::Up)
  {
    constraint = NavigationPositionConstraintUp(columnIndex, mImpl->mNumberOfColumns, mImpl->mColumnSpacing, mImpl->mSizeExtend, mImpl->mBottomMargin, mImpl->mTopMargin);
  }
  else if (mOrientation == ControlOrientation::Down)
  {
    constraint = NavigationPositionConstraintDown(columnIndex, mImpl->mNumberOfColumns, mImpl->mColumnSpacing, mImpl->mSizeExtend, mImpl->mBottomMargin, mImpl->mTopMargin);
  }
  else if (mOrientation == ControlOrientation::Right)
  {
    constraint = NavigationPositionConstraintRight(columnIndex, mImpl->mNumberOfColumns, mImpl->mColumnSpacing, mImpl->mSizeExtend, mImpl->mBottomMargin, mImpl->mTopMargin);
  }

  return true;
}

bool NavigationLayout::GetScaleConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const
{
  return false; // No scaling
}

bool NavigationLayout::GetRotationConstraint(unsigned int itemId, ItemLayout::QuaternionFunction& constraint) const
{
  constraint = mImpl->mRotationConstraint[mOrientation];
  return true;
}

bool NavigationLayout::GetColorConstraint(unsigned int itemId, ItemLayout::Vector4Function& constraint) const
{
  constraint = mImpl->mColorConstraint;
  return true;
}

bool NavigationLayout::GetVisibilityConstraint(unsigned int itemId, ItemLayout::BoolFunction& constraint) const
{
  unsigned int columnIndex = itemId % mImpl->mNumberOfColumns;
  constraint = NavigationVisibilityConstraint(columnIndex, mImpl->mNumberOfColumns, mImpl->mColumnSpacing);
  return true;
}

NavigationLayout::NavigationSignalV2& NavigationLayout::PanSignal()
{
  return mImpl->mSignalPanV2;
}

Degree NavigationLayout::GetScrollDirection() const
{
  Degree scrollDirection(0);
  if (mOrientation == ControlOrientation::Down)
  {
    scrollDirection = 0.0f - 45.0f;
  }
  else if (mOrientation == ControlOrientation::Right)
  {
    scrollDirection = 90.0f - 45.0f;
  }
  else if (mOrientation == ControlOrientation::Up)
  {
    scrollDirection = 180.0f - 45.0f;
  }
  else // mOrientation == ControlOrientation::Left
  {
    scrollDirection = 270.0f - 45.0f;
  }

  return scrollDirection;
}

NavigationLayout::NavigationLayout()
  :mImpl(NULL)
{
  mImpl = new Impl();
}

}
}
