/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-impl-property-handler.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-impl.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-mode.h>

namespace Dali::Toolkit::Internal
{

void ScrollViewPropertyHandler::Set(BaseObject* object, Property::Index index, const Property::Value& value)
{
  Toolkit::ScrollView scrollView = Toolkit::ScrollView::DownCast(Dali::BaseHandle(object));

  if(scrollView)
  {
    ScrollView& scrollViewImpl(GetImpl(scrollView));
    switch(index)
    {
      case Toolkit::ScrollView::Property::WRAP_ENABLED:
      {
        scrollViewImpl.SetWrapMode(value.Get<bool>());
        break;
      }
      case Toolkit::ScrollView::Property::PANNING_ENABLED:
      {
        scrollViewImpl.SetScrollSensitive(value.Get<bool>());
        break;
      }
      case Toolkit::ScrollView::Property::AXIS_AUTO_LOCK_ENABLED:
      {
        scrollViewImpl.SetAxisAutoLock(value.Get<bool>());
        break;
      }
      case Toolkit::ScrollView::Property::WHEEL_SCROLL_DISTANCE_STEP:
      {
        scrollViewImpl.SetWheelScrollDistanceStep(value.Get<Vector2>());
        break;
      }
      case Toolkit::ScrollView::Property::SCROLL_MODE:
      {
        const Property::Map* map = value.GetMap();
        if(map)
        {
          SetScrollMode(scrollViewImpl, *map);
        }
      }
    }
  }
}

Property::Value ScrollViewPropertyHandler::Get(BaseObject* object, Property::Index index)
{
  Property::Value value;

  Toolkit::ScrollView scrollView = Toolkit::ScrollView::DownCast(Dali::BaseHandle(object));

  if(scrollView)
  {
    ScrollView& scrollViewImpl(GetImpl(scrollView));
    switch(index)
    {
      case Toolkit::ScrollView::Property::WRAP_ENABLED:
      {
        value = scrollViewImpl.GetWrapMode();
        break;
      }
      case Toolkit::ScrollView::Property::PANNING_ENABLED:
      {
        value = scrollViewImpl.GetScrollSensitive();
        break;
      }
      case Toolkit::ScrollView::Property::AXIS_AUTO_LOCK_ENABLED:
      {
        value = scrollViewImpl.GetAxisAutoLock();
        break;
      }
      case Toolkit::ScrollView::Property::WHEEL_SCROLL_DISTANCE_STEP:
      {
        value = scrollViewImpl.GetWheelScrollDistanceStep();
        break;
      }
    }
  }

  return value;
}

void ScrollViewPropertyHandler::SetScrollMode(ScrollView& scrollView, const Property::Map& scrollModeMap)
{
  Toolkit::RulerPtr rulerX, rulerY;

  // Check the scroll mode in the X axis
  bool             xAxisScrollEnabled = true;
  Property::Value* valuePtr           = scrollModeMap.Find(Toolkit::ScrollMode::X_AXIS_SCROLL_ENABLED, "xAxisScrollEnabled");
  if(valuePtr && valuePtr->GetType() == Property::BOOLEAN)
  {
    valuePtr->Get(xAxisScrollEnabled);
  }

  if(!xAxisScrollEnabled)
  {
    // Default ruler and disabled
    rulerX = new Toolkit::DefaultRuler();
    rulerX->Disable();
  }
  else
  {
    valuePtr                  = scrollModeMap.Find(Toolkit::ScrollMode::X_AXIS_SNAP_TO_INTERVAL, "xAxisSnapToInterval");
    float xAxisSnapToInterval = 0.0f;
    if(valuePtr && valuePtr->Get(xAxisSnapToInterval))
    {
      // Fixed ruler and enabled
      rulerX = new Toolkit::FixedRuler(xAxisSnapToInterval);
    }
    else
    {
      // Default ruler and enabled
      rulerX = new Toolkit::DefaultRuler();
    }

    valuePtr                  = scrollModeMap.Find(Toolkit::ScrollMode::X_AXIS_SCROLL_BOUNDARY, "xAxisScrollBoundary");
    float xAxisScrollBoundary = 0.0f;
    if(valuePtr && valuePtr->Get(xAxisScrollBoundary))
    {
      // By default ruler domain is disabled unless set
      rulerX->SetDomain(Toolkit::RulerDomain(0, xAxisScrollBoundary, true));
    }
  }

  // Check the scroll mode in the Y axis
  bool yAxisScrollEnabled = true;
  valuePtr                = scrollModeMap.Find(Toolkit::ScrollMode::Y_AXIS_SCROLL_ENABLED, "yAxisScrollEnabled");
  if(valuePtr && valuePtr->GetType() == Property::BOOLEAN)
  {
    valuePtr->Get(yAxisScrollEnabled);
  }

  if(!yAxisScrollEnabled)
  {
    // Default ruler and disabled
    rulerY = new Toolkit::DefaultRuler();
    rulerY->Disable();
  }
  else
  {
    valuePtr                  = scrollModeMap.Find(Toolkit::ScrollMode::Y_AXIS_SNAP_TO_INTERVAL, "yAxisSnapToInterval");
    float yAxisSnapToInterval = 0.0f;
    if(valuePtr && valuePtr->Get(yAxisSnapToInterval))
    {
      // Fixed ruler and enabled
      rulerY = new Toolkit::FixedRuler(yAxisSnapToInterval);
    }
    else
    {
      // Default ruler and enabled
      rulerY = new Toolkit::DefaultRuler();
    }

    valuePtr                  = scrollModeMap.Find(Toolkit::ScrollMode::Y_AXIS_SCROLL_BOUNDARY, "yAxisScrollBoundary");
    float yAxisScrollBoundary = 0.0f;
    if(valuePtr && valuePtr->Get(yAxisScrollBoundary))
    {
      // By default ruler domain is disabled unless set
      rulerY->SetDomain(Toolkit::RulerDomain(0, yAxisScrollBoundary, true));
    }
  }

  scrollView.SetRulerX(rulerX);
  scrollView.SetRulerY(rulerY);
}

void ScrollViewPropertyHandler::UpdatePropertyDomain(ScrollView& scrollView)
{
  Actor   self                  = scrollView.Self();
  Vector3 size                  = self.GetTargetSize();
  Vector2 min                   = scrollView.mMinScroll;
  Vector2 max                   = scrollView.mMaxScroll;
  bool    scrollPositionChanged = false;
  bool    domainChanged         = false;

  bool canScrollVertical   = false;
  bool canScrollHorizontal = false;
  scrollView.UpdateLocalScrollProperties();
  if(scrollView.mRulerX->IsEnabled())
  {
    const Toolkit::RulerDomain& rulerDomain = scrollView.mRulerX->GetDomain();
    if(fabsf(min.x - rulerDomain.min) > Math::MACHINE_EPSILON_100 || fabsf(max.x - rulerDomain.max) > Math::MACHINE_EPSILON_100)
    {
      domainChanged = true;
      min.x         = rulerDomain.min;
      max.x         = rulerDomain.max;

      // make sure new scroll value is within new domain
      if(scrollView.mScrollPrePosition.x < min.x || scrollView.mScrollPrePosition.x > max.x)
      {
        scrollPositionChanged = true;
        scrollView.mScrollPrePosition.x  = Clamp(scrollView.mScrollPrePosition.x, -(max.x - size.x), -min.x);
      }
    }
    if((fabsf(rulerDomain.max - rulerDomain.min) - size.x) > Math::MACHINE_EPSILON_100)
    {
      canScrollHorizontal = true;
    }
  }
  else if(fabs(min.x) > Math::MACHINE_EPSILON_100 || fabs(max.x) > Math::MACHINE_EPSILON_100)
  {
    // need to reset to 0
    domainChanged       = true;
    min.x               = 0.0f;
    max.x               = 0.0f;
    canScrollHorizontal = false;
  }

  if(scrollView.mRulerY->IsEnabled())
  {
    const Toolkit::RulerDomain& rulerDomain = scrollView.mRulerY->GetDomain();
    if(fabsf(min.y - rulerDomain.min) > Math::MACHINE_EPSILON_100 || fabsf(max.y - rulerDomain.max) > Math::MACHINE_EPSILON_100)
    {
      domainChanged = true;
      min.y         = rulerDomain.min;
      max.y         = rulerDomain.max;

      // make sure new scroll value is within new domain
      if(scrollView.mScrollPrePosition.y < min.y || scrollView.mScrollPrePosition.y > max.y)
      {
        scrollPositionChanged = true;
        scrollView.mScrollPrePosition.y  = Clamp(scrollView.mScrollPrePosition.y, -(max.y - size.y), -min.y);
      }
    }
    if((fabsf(rulerDomain.max - rulerDomain.min) - size.y) > Math::MACHINE_EPSILON_100)
    {
      canScrollVertical = true;
    }
  }
  else if(fabs(min.y) > Math::MACHINE_EPSILON_100 || fabs(max.y) > Math::MACHINE_EPSILON_100)
  {
    // need to reset to 0
    domainChanged     = true;
    min.y             = 0.0f;
    max.y             = 0.0f;
    canScrollVertical = false;
  }

  // avoid setting properties if possible, otherwise this will cause an entire update as well as triggering constraints using each property we update
  if(scrollView.mCanScrollVertical != canScrollVertical)
  {
    scrollView.mCanScrollVertical = canScrollVertical;
    self.SetProperty(Toolkit::Scrollable::Property::CAN_SCROLL_VERTICAL, canScrollVertical);
  }
  if(scrollView.mCanScrollHorizontal != canScrollHorizontal)
  {
    scrollView.mCanScrollHorizontal = canScrollHorizontal;
    self.SetProperty(Toolkit::Scrollable::Property::CAN_SCROLL_HORIZONTAL, canScrollHorizontal);
  }
  if(scrollPositionChanged)
  {
    self.SetProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION, scrollView.mScrollPrePosition);
  }
  if(domainChanged)
  {
    scrollView.mMinScroll = min;
    scrollView.mMaxScroll = max;
    self.SetProperty(Toolkit::Scrollable::Property::SCROLL_POSITION_MIN, scrollView.mMinScroll);
    self.SetProperty(Toolkit::Scrollable::Property::SCROLL_POSITION_MAX, scrollView.mMaxScroll);
  }
}

} // namespace Dali::Toolkit::Internal
