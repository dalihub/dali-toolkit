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
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-layout.h>

// EXTERNAL INCLUDES
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/time-period.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-view.h>

namespace Dali
{

namespace Toolkit
{

struct ItemLayout::Impl
{
  Vector3 mItemSize;                              ///< The size of an item in the layout
  ControlOrientation::Type mOrientation;          ///< the orientation of the layout.
};

ItemLayout::ItemLayout()
: mImpl( new Impl )
{
  mImpl->mOrientation = ControlOrientation::Up;
}

ItemLayout::~ItemLayout()
{
  delete mImpl;
}

void ItemLayout::SetOrientation(ControlOrientation::Type orientation)
{
  mImpl->mOrientation = orientation;
}

ControlOrientation::Type ItemLayout::GetOrientation() const
{
  return mImpl->mOrientation;
}

void ItemLayout::GetItemSize( unsigned int itemId, const Vector3& layoutSize, Vector3& itemSize ) const
{
  // If item-size has not been set then get the default size
  if ( mImpl->mItemSize == Vector3::ZERO )
  {
    GetDefaultItemSize( itemId, layoutSize, itemSize );
  }
  else
  {
    itemSize = mImpl->mItemSize;
  }
}

void ItemLayout::SetItemSize( const Vector3& itemSize )
{
  mImpl->mItemSize = itemSize;
}

float ItemLayout::GetClosestOnScreenLayoutPosition(int itemID, float currentLayoutPosition, const Vector3& layoutSize)
{
  Vector3 itemPosition = GetItemPosition( itemID, currentLayoutPosition, layoutSize );
  Vector3 itemSize;
  GetItemSize(itemID, layoutSize, itemSize);
  Vector3 onScreenArea = (layoutSize - itemSize) * 0.5f;
  if (itemPosition.x < -onScreenArea.x
      || itemPosition.x > onScreenArea.x
      || itemPosition.y < -onScreenArea.y
      || itemPosition.y > onScreenArea.y)
  {
    // item not within viewable area
    // safest thing to do here since we have no idea how the implementation will work is to return the scroll to position
    return GetItemScrollToPosition(itemID);
  }
  return currentLayoutPosition;
}

int ItemLayout::GetNextFocusItemID(int itemID, int maxItems, Dali::Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled)
{
  switch( direction )
  {
    case Control::KeyboardFocus::LEFT:
    case Control::KeyboardFocus::UP:
    {
      itemID--;
      if( itemID < 0 )
      {
        itemID = loopEnabled ? maxItems - 1 : 0;
      }
      break;
    }
    case Control::KeyboardFocus::RIGHT:
    case Control::KeyboardFocus::DOWN:
    {
      itemID++;
      if( itemID >= maxItems )
      {
        itemID = loopEnabled ? 0 : maxItems - 1;
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

float ItemLayout::GetFlickSpeedFactor() const
{
  // By default, the speed factor while dragging and swiping is the same.
  return GetScrollSpeedFactor();
}

} // namespace Toolkit

} // namespace Dali
