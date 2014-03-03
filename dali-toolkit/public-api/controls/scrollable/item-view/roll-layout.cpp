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

#include <dali-toolkit/public-api/controls/scrollable/item-view/roll-layout.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;

namespace // unnamed namespace
{

const float DEFAULT_ROW_SPACING = 20.0f;
const float DEFAULT_SCROLL_SPEED_FACTOR = 0.0015f;
const float DEFAULT_MAXIMUM_SWIPE_SPEED = 8.0f;
const float DEFAULT_ITEM_FLICK_ANIMATION_DURATION = 0.4f;

// 4 orientations are supported
static const unsigned int ORIENTATION_COUNT = 4;

static Vector3 GetItemSizeDefaultFunction(float layoutWidth, float layoutHeight, float rowSpacing)
{
  float height = (layoutHeight - rowSpacing) * 0.5f;
  return Vector3(layoutWidth, height, height);
}

struct RollPositionConstraint0
{
  RollPositionConstraint0(const float rowSpacing, RollLayout::ItemSizeFunction itemSizeFunction)
  : mRowSpacing(rowSpacing),
    mItemSizeFunction(itemSizeFunction)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(layoutSize.width, layoutSize.height, mRowSpacing);

    float adjustedLayoutPosition = layoutPosition;
    float scrollSpeedFactor = scrollSpeed * scrollSpeed * scrollSpeed * scrollSpeed;
    float y = 0.0f;

    float adjustedRowSpacing = mRowSpacing + scrollSpeedFactor;

    if(adjustedLayoutPosition > Math::MACHINE_EPSILON_0 && adjustedLayoutPosition -2.0f < Math::MACHINE_EPSILON_0)
    {
      float adjustment = 1.0f - Dali::AlphaFunctions::EaseInOutSine60((2.0f - adjustedLayoutPosition) * 0.5f);
      adjustedLayoutPosition = adjustment * 2.0f;
      y = ((itemSize.y + adjustedRowSpacing ) * adjustedLayoutPosition) - layoutSize.height * 0.5f + itemSize.y * 0.5f;
    }
    else
    {
      float yStep = max(50.0f, min(itemSize.y, scrollSpeedFactor));
      y = adjustedLayoutPosition < Math::MACHINE_EPSILON_0 ? adjustedLayoutPosition * yStep : (layoutSize.height * 0.5f + adjustedRowSpacing) + (adjustedLayoutPosition - 1.0f) * yStep;
      y += itemSize.y * 0.5f - layoutSize.height * 0.5f;
    }

    float z = adjustedLayoutPosition * (10.0f + scrollSpeedFactor);
    z -= min(3000.0f, scrollSpeedFactor * 2.0f);

    return Vector3(itemSize.x * 0.5f - layoutSize.x * 0.5f, y, z);
  }

public:

  float mRowSpacing;
  RollLayout::ItemSizeFunction mItemSizeFunction;
};

struct RollPositionConstraint90
{
  RollPositionConstraint90(const float rowSpacing, RollLayout::ItemSizeFunction itemSizeFunction)
  : mRowSpacing(rowSpacing),
    mItemSizeFunction(itemSizeFunction)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(layoutSize.height, layoutSize.width, mRowSpacing);

    float adjustedLayoutPosition = layoutPosition;
    float scrollSpeedFactor = scrollSpeed * scrollSpeed * scrollSpeed;
    float x = 0.0f;

    float adjustedRowSpacing = mRowSpacing + scrollSpeedFactor;

    if(adjustedLayoutPosition > Math::MACHINE_EPSILON_0 && adjustedLayoutPosition -2.0f < Math::MACHINE_EPSILON_0)
    {
      float adjustment = 1.0f - Dali::AlphaFunctions::EaseInOutSine60((2.0f - adjustedLayoutPosition) * 0.5f);
      adjustedLayoutPosition = adjustment * 2.0f;
      x = ((itemSize.y + adjustedRowSpacing ) * adjustedLayoutPosition) - layoutSize.width * 0.5f + itemSize.y * 0.5f;
    }
    else
    {
      float xStep = max(50.0f, min(itemSize.y, scrollSpeedFactor));
      x = adjustedLayoutPosition < Math::MACHINE_EPSILON_0 ? adjustedLayoutPosition * xStep : (layoutSize.width * 0.5f + adjustedRowSpacing) + (adjustedLayoutPosition - 1.0f) * xStep;
      x += itemSize.y * 0.5f - layoutSize.width * 0.5f;
    }

    float z = adjustedLayoutPosition * (10.0f + scrollSpeedFactor);
    z -= min(3000.0f, scrollSpeedFactor * 2.0f);

    return Vector3(x, itemSize.x * 0.5f - layoutSize.y * 0.5f, z);
  }

public:

  float mRowSpacing;
  RollLayout::ItemSizeFunction mItemSizeFunction;
};

struct RollPositionConstraint180
{
  RollPositionConstraint180(const float rowSpacing, RollLayout::ItemSizeFunction itemSizeFunction)
  : mRowSpacing(rowSpacing),
    mItemSizeFunction(itemSizeFunction)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(layoutSize.width, layoutSize.height, mRowSpacing);

    float adjustedLayoutPosition = layoutPosition;
    float scrollSpeedFactor = scrollSpeed * scrollSpeed * scrollSpeed;
    float y = 0.0f;

    float adjustedRowSpacing = mRowSpacing + scrollSpeedFactor;

    if(adjustedLayoutPosition > Math::MACHINE_EPSILON_0 && adjustedLayoutPosition -2.0f < Math::MACHINE_EPSILON_0)
    {
      float adjustment = 1.0f - Dali::AlphaFunctions::EaseInOutSine60((2.0f - adjustedLayoutPosition) * 0.5f);
      adjustedLayoutPosition = adjustment * 2.0f;
      y = ((itemSize.y + adjustedRowSpacing ) * adjustedLayoutPosition) - layoutSize.height * 0.5f + itemSize.y * 0.5f;
    }
    else
    {
      float yStep = max(50.0f, min(itemSize.y, scrollSpeedFactor));
      y = adjustedLayoutPosition < Math::MACHINE_EPSILON_0 ? adjustedLayoutPosition * yStep : (layoutSize.height * 0.5f + adjustedRowSpacing) + (adjustedLayoutPosition - 1.0f) * yStep;
      y += itemSize.y * 0.5f - layoutSize.height * 0.5f;
    }

    float z = adjustedLayoutPosition * (10.0f + scrollSpeedFactor);
    z -= min(3000.0f, scrollSpeedFactor * 2.0f);


    return Vector3(-(itemSize.x * 0.5f - layoutSize.x * 0.5f),
                   -y,
                   z);
  }

public:

  float mRowSpacing;
  RollLayout::ItemSizeFunction mItemSizeFunction;
};

struct RollPositionConstraint270
{
  RollPositionConstraint270(const float rowSpacing, RollLayout::ItemSizeFunction itemSizeFunction)
  : mRowSpacing(rowSpacing),
    mItemSizeFunction(itemSizeFunction)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(layoutSize.height, layoutSize.width, mRowSpacing);

    float adjustedLayoutPosition = layoutPosition;
    float scrollSpeedFactor = scrollSpeed * scrollSpeed * scrollSpeed;
    float x = 0.0f;

    float adjustedRowSpacing = mRowSpacing + scrollSpeedFactor;

    if(adjustedLayoutPosition > Math::MACHINE_EPSILON_0 && adjustedLayoutPosition -2.0f < Math::MACHINE_EPSILON_0)
    {
      float adjustment = 1.0f - Dali::AlphaFunctions::EaseInOutSine60((2.0f - adjustedLayoutPosition) * 0.5f);
      adjustedLayoutPosition = adjustment * 2.0f;
      x = ((itemSize.y + adjustedRowSpacing ) * adjustedLayoutPosition) - layoutSize.width * 0.5f + itemSize.y * 0.5f;
    }
    else
    {
      float xStep = max(50.0f, min(itemSize.y, scrollSpeedFactor));
      x = adjustedLayoutPosition < Math::MACHINE_EPSILON_0 ? adjustedLayoutPosition * xStep : (layoutSize.width * 0.5f + adjustedRowSpacing) + (adjustedLayoutPosition - 1.0f) * xStep;
      x += itemSize.y * 0.5f - layoutSize.width * 0.5f;
    }

    float z = adjustedLayoutPosition * (10.0f + scrollSpeedFactor);
    z -= min(3000.0f, scrollSpeedFactor * 2.0f);

    return Vector3(-x,
                   itemSize.x * 0.5f - layoutSize.y * 0.5f,
                   z);
  }

public:

  float mRowSpacing;
  RollLayout::ItemSizeFunction mItemSizeFunction;
};

struct RollRotationConstraint0
{
  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    return Quaternion(0.0f, Vector3::ZAXIS);
  }
};

struct RollRotationConstraint90
{
  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    return Quaternion(1.5f * Math::PI, Vector3::ZAXIS);
  }
};

struct RollRotationConstraint180
{
  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    return Quaternion(Math::PI, Vector3::ZAXIS);
  }
};

struct RollRotationConstraint270
{
  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    return Quaternion(0.5f * Math::PI, Vector3::ZAXIS);
  }
};

struct RollScaleConstraint
{
  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float adjustedLayoutPosition = layoutPosition;
    float factor = 0.0f;
    if(adjustedLayoutPosition < Math::MACHINE_EPSILON_0)
    {
      factor = fabsf(adjustedLayoutPosition);
    }
    if(adjustedLayoutPosition - 1.0f > Math::MACHINE_EPSILON_0)
    {
      factor = adjustedLayoutPosition - 1.0f;
    }

    float scale = min(1.0f, max(0.1f, 1.0f - 0.1f * factor));
    if(scrollSpeed > 0.0f)
    {
      scale *= min(1.0f, max(0.1f, 1.0f / (scrollSpeed * 0.05f)));
    }

    return Vector3(scale, scale, scale);
  }
};

struct RollColorConstraint
{
  Vector4 operator()(const Vector4& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float adjustedLayoutPosition = layoutPosition;

    float factor = 0.0f;
    if(adjustedLayoutPosition < Math::MACHINE_EPSILON_0)
    {
      factor = fabsf(adjustedLayoutPosition);
    }
    if(adjustedLayoutPosition - 1.0f > Math::MACHINE_EPSILON_0)
    {
      factor = adjustedLayoutPosition - 1.0f;
    }

    float darkness = min(1.0f, max(0.5f, 1.0f - 0.5f * factor));
    float alpha = min(1.0f, max(0.0f, 1.0f - 0.9f * factor));
    return Vector4(darkness, darkness, darkness, alpha);
  }
};

struct RollVisibilityConstraintPortrait
{
  RollVisibilityConstraintPortrait(const float rowSpacing, RollLayout::ItemSizeFunction itemSizeFunction)
  : mRowSpacing(rowSpacing),
    mItemSizeFunction(itemSizeFunction)
  {
  }

  bool operator()(const bool& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(layoutSize.width, layoutSize.height, mRowSpacing);
    int rowsPerPage = ceil(layoutSize.height / (itemSize.y + mRowSpacing));
    return (layoutPosition > -rowsPerPage) && (layoutPosition < rowsPerPage);
  }

public:

  float mRowSpacing;
  RollLayout::ItemSizeFunction mItemSizeFunction;
};

struct RollVisibilityConstraintLandscape
{
  RollVisibilityConstraintLandscape(const float rowSpacing, RollLayout::ItemSizeFunction itemSizeFunction)
  : mRowSpacing(rowSpacing),
    mItemSizeFunction(itemSizeFunction)
  {
  }

  bool operator()(const bool& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(layoutSize.height, layoutSize.width, mRowSpacing);
    int rowsPerPage = ceil(layoutSize.width / (itemSize.y + mRowSpacing));
    return (layoutPosition + 2.0f > Math::MACHINE_EPSILON_0) && (layoutPosition < rowsPerPage);
  }

public:

  float mRowSpacing;
  RollLayout::ItemSizeFunction mItemSizeFunction;
};

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

struct RollLayout::Impl
{
  Impl()
  : mRowSpacing(DEFAULT_ROW_SPACING),
    mScrollSpeedFactor(DEFAULT_SCROLL_SPEED_FACTOR),
    mMaximumSwipeSpeed(DEFAULT_MAXIMUM_SWIPE_SPEED),
    mItemFlickAnimationDuration(DEFAULT_ITEM_FLICK_ANIMATION_DURATION),
    mItemSizeFunction(GetItemSizeDefaultFunction)
  {
    mScaleConstraint = RollScaleConstraint();
    mColorConstraint = RollColorConstraint();

    mRotationConstraint[0] = RollRotationConstraint0();
    mRotationConstraint[1] = RollRotationConstraint90();
    mRotationConstraint[2] = RollRotationConstraint180();
    mRotationConstraint[3] = RollRotationConstraint270();
  }

  float mRowSpacing;

  float mScrollSpeedFactor;
  float mMaximumSwipeSpeed;
  float mItemFlickAnimationDuration;

  ItemLayout::QuaternionFunction mRotationConstraint[ORIENTATION_COUNT];
  ItemLayout::Vector3Function mScaleConstraint;
  ItemLayout::Vector4Function mColorConstraint;

  ItemSizeFunction mItemSizeFunction;
};

RollLayoutPtr RollLayout::New()
{
  return RollLayoutPtr(new RollLayout());
}

RollLayout::~RollLayout()
{
  delete mImpl;
}

void RollLayout::SetRowSpacing(float spacing)
{
  mImpl->mRowSpacing = spacing;
}

float RollLayout::GetRowSpacing() const
{
  return mImpl->mRowSpacing;
}

void RollLayout::SetItemSizeFunction(ItemSizeFunction function)
{
  mImpl->mItemSizeFunction = function;
}

RollLayout::ItemSizeFunction RollLayout::GetItemSizeFunction() const
{
  return mImpl->mItemSizeFunction;
}

void RollLayout::SetScrollSpeedFactor(float scrollSpeed)
{
  mImpl->mScrollSpeedFactor = scrollSpeed;
}

void RollLayout::SetMaximumSwipeSpeed(float speed)
{
  mImpl->mMaximumSwipeSpeed = speed;
}

void RollLayout::SetItemFlickAnimationDuration(float durationSeconds)
{
  mImpl->mItemFlickAnimationDuration = durationSeconds;
}

float RollLayout::GetScrollSpeedFactor() const
{
  return mImpl->mScrollSpeedFactor;
}

float RollLayout::GetMaximumSwipeSpeed() const
{
  return mImpl->mMaximumSwipeSpeed;
}

float RollLayout::GetItemFlickAnimationDuration() const
{
  return mImpl->mItemFlickAnimationDuration;
}

float RollLayout::GetMinimumLayoutPosition(unsigned int numberOfItems, Vector3 layoutSize) const
{
  return 2.0f - static_cast<float>(numberOfItems);
}

float RollLayout::GetClosestAnchorPosition(float layoutPosition) const
{
  return static_cast<float>(round(layoutPosition));
}

float RollLayout::GetItemScrollToPosition(unsigned int itemId) const
{
  return 0.0f - static_cast<float>(itemId);
}

ItemRange RollLayout::GetItemsWithinArea(float firstItemPosition, Vector3 layoutSize) const
{
  float layoutWidth = IsHorizontal(mOrientation) ? layoutSize.height : layoutSize.width;
  float layoutHeight = IsHorizontal(mOrientation) ? layoutSize.width : layoutSize.height;

  Vector3 itemSize = mImpl->mItemSizeFunction( layoutWidth,layoutHeight, mImpl->mRowSpacing);

  float itemsPerPage = (layoutHeight / (itemSize.y + mImpl->mRowSpacing));
  if(firstItemPosition + 0.001f >= Math::MACHINE_EPSILON_0)
  {
    itemsPerPage = std::max(0.0f, itemsPerPage - 1.0f);
  }
  int firstVisibleItem = -(static_cast<int>(firstItemPosition));

  int firstItemIndex = std::max(0, firstVisibleItem);
  int lastItemIndex  = std::max(0, static_cast<int>(ceil(firstVisibleItem + itemsPerPage - 1)));
  return ItemRange(firstItemIndex, lastItemIndex);
}

unsigned int RollLayout::GetReserveItemCount(Vector3 layoutSize) const
{
  float layoutWidth = IsHorizontal(mOrientation) ? layoutSize.height : layoutSize.width;
  float layoutHeight = IsHorizontal(mOrientation) ? layoutSize.width : layoutSize.height;

  Vector3 itemSize = mImpl->mItemSizeFunction(layoutWidth, layoutHeight, mImpl->mRowSpacing);
  int itemsPerPage = ceil(layoutHeight / (itemSize.y + mImpl->mRowSpacing));
  return itemsPerPage * 5;
}

bool RollLayout::GetItemSize(unsigned int itemId, Vector3 layoutSize, Vector3& itemSize) const
{
  // Note: itemId is not checked, since every item has the same size
  float layoutWidth = IsHorizontal(mOrientation) ? layoutSize.height : layoutSize.width;
  float layoutHeight = IsHorizontal(mOrientation) ? layoutSize.width : layoutSize.height;

  itemSize = mImpl->mItemSizeFunction(layoutWidth, layoutHeight, mImpl->mRowSpacing);

  return true;
}

void RollLayout::GetResizeAnimation(Animation& animation, Actor actor, Vector3 size, float durationSeconds) const
{
  if(animation)
  {
    animation.Resize(actor, size);
  }
}

bool RollLayout::GetPositionConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const
{
  if (mOrientation == ControlOrientation::Up)
  {
    constraint = RollPositionConstraint0(mImpl->mRowSpacing, mImpl->mItemSizeFunction);
  }
  else if (mOrientation == ControlOrientation::Left)
  {
    constraint = RollPositionConstraint90(mImpl->mRowSpacing, mImpl->mItemSizeFunction);
  }
  else if (mOrientation == ControlOrientation::Down)
  {
    constraint = RollPositionConstraint180(mImpl->mRowSpacing, mImpl->mItemSizeFunction);
  }
  else // mOrientation == ControlOrientation::Right
  {
    constraint = RollPositionConstraint270(mImpl->mRowSpacing, mImpl->mItemSizeFunction);
  }

  return true;
}

bool RollLayout::GetRotationConstraint(unsigned int itemId, ItemLayout::QuaternionFunction& constraint) const
{
  constraint = mImpl->mRotationConstraint[mOrientation];
  return true;
}

bool RollLayout::GetScaleConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const
{
  constraint = mImpl->mScaleConstraint;
  return true;
}

bool RollLayout::GetColorConstraint(unsigned int itemId, ItemLayout::Vector4Function& constraint) const
{
  constraint = mImpl->mColorConstraint;
  return true;
}

bool RollLayout::GetVisibilityConstraint(unsigned int itemId, ItemLayout::BoolFunction& constraint) const
{
  if (IsVertical(mOrientation))
  {
    constraint = RollVisibilityConstraintPortrait(mImpl->mRowSpacing, mImpl->mItemSizeFunction);
  }
  else // horizontal
  {
    constraint = RollVisibilityConstraintLandscape(mImpl->mRowSpacing, mImpl->mItemSizeFunction);
  }

  return true;
}

Degree RollLayout::GetScrollDirection() const
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

RollLayout::RollLayout()
: mImpl(NULL)
{
  mImpl = new Impl();
}

} // namespace Toolkit

} // namespace Dali
