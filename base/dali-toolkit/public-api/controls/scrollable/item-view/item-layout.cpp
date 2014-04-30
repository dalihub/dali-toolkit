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

#include <dali-toolkit/public-api/controls/scrollable/item-view/item-layout.h>

namespace Dali
{

namespace Toolkit
{

ItemLayout::ItemLayout()
: mOrientation(ControlOrientation::Up)
{
}

ItemLayout::~ItemLayout()
{
}

void ItemLayout::SetOrientation(ControlOrientation::Type orientation)
{
  mOrientation = orientation;
}

ControlOrientation::Type ItemLayout::GetOrientation() const
{
  return mOrientation;
}

float ItemLayout::GetClosestOnScreenLayoutPosition(int itemID, float currentLayoutPosition, const Vector3& layoutSize)
{
  ItemLayout::Vector3Function positionConstraint;
  Vector3 itemPosition = Vector3::ZERO;
  if (GetPositionConstraint(itemID, positionConstraint))
  {
    itemPosition = positionConstraint(Vector3::ZERO, currentLayoutPosition + itemID, 0.0f, layoutSize);
  }
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

void ItemLayout::GetXAxisScrollHint(Vector2& scrollHint) const
{
  scrollHint = Vector2::ZERO;
  Radian scrollAngle(GetScrollDirection());
  Vector2 scrollDirection(sinf(scrollAngle), cosf(scrollAngle));
  switch(mOrientation)
  {
    case ControlOrientation::Up:
    {
      if(fabsf(scrollDirection.y) < Math::MACHINE_EPSILON_1)
      {
        // we probably want x scrolling
        if(scrollDirection.x > 0.0f)
        {
          // normal positive scrolling
          scrollHint = Vector2::XAXIS;
        }
        else
        {
          scrollHint = -Vector2::XAXIS;
        }
      }
      break;
    }
    case ControlOrientation::Down:
    {
      if(fabsf(scrollDirection.y) < Math::MACHINE_EPSILON_1)
      {
        // we probably want x scrolling
        if(scrollDirection.x > 0.0f)
        {
          // normal positive scrolling
          scrollHint = -Vector2::XAXIS;
        }
        else
        {
          scrollHint = Vector2::XAXIS;
        }
      }
      break;
    }
    case ControlOrientation::Left:
    {
      // we probably want x scrolling
      if(scrollDirection.x > 0.0f)
      {
        // normal positive scrolling
        scrollHint = Vector2::XAXIS;
      }
      else
      {
        scrollHint = -Vector2::XAXIS;
      }
      break;
    }
    case ControlOrientation::Right:
    {
      // we probably want x scrolling
      if(scrollDirection.x > 0.0f)
      {
        // normal positive scrolling
        scrollHint = -Vector2::XAXIS;
      }
      else
      {
        scrollHint = Vector2::XAXIS;
      }
      break;
    }
  }
}

void ItemLayout::GetYAxisScrollHint(Vector2& scrollHint) const
{
  scrollHint = Vector2::ZERO;
  Radian scrollAngle(GetScrollDirection());
  Vector2 scrollDirection(sinf(scrollAngle), cosf(scrollAngle));
  switch(mOrientation)
  {
    case ControlOrientation::Up:
    {
      // we probably want x scrolling
      if(scrollDirection.y > 0.0f)
      {
        // normal positive scrolling
        scrollHint = Vector2::YAXIS;
      }
      else
      {
        scrollHint = -Vector2::YAXIS;
      }
      break;
    }
    case ControlOrientation::Down:
    {
      // we probably want x scrolling
      if(scrollDirection.y > 0.0f)
      {
        // normal positive scrolling
        scrollHint = -Vector2::YAXIS;
      }
      else
      {
        scrollHint = Vector2::YAXIS;
      }
      break;
    }
    case ControlOrientation::Left:
    {
      if(fabsf(scrollDirection.x) < Math::MACHINE_EPSILON_1)
      {
        // we probably want x scrolling
        if(scrollDirection.y > 0.0f)
        {
          // normal positive scrolling
          scrollHint = -Vector2::YAXIS;
        }
        else
        {
          scrollHint = Vector2::YAXIS;
        }
      }
      break;
    }
    case ControlOrientation::Right:
    {
      if(fabsf(scrollDirection.x) < Math::MACHINE_EPSILON_1)
      {
        // we probably want x scrolling
        if(scrollDirection.y > 0.0f)
        {
          // normal positive scrolling
          scrollHint = Vector2::YAXIS;
        }
        else
        {
          scrollHint = -Vector2::YAXIS;
        }
      }
      break;
    }
  }
}

int ItemLayout::GetNextFocusItemID(int itemID, int maxItems, Dali::Toolkit::Control::KeyboardFocusNavigationDirection direction, bool loopEnabled)
{
  switch( direction )
  {
    case Control::Left:
    case Control::Up:
    {
      itemID--;
      if( itemID < 0 )
      {
        itemID = loopEnabled ? maxItems - 1 : 0;
      }
      break;
    }
    case Control::Right:
    case Control::Down:
    {
      itemID++;
      if( itemID >= maxItems )
      {
        itemID = loopEnabled ? 0 : maxItems - 1;
      }
      break;
    }
  }
  return itemID;
}

} // namespace Toolkit

} // namespace Dali
