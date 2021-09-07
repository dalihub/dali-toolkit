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
#include <dali-toolkit/internal/transition/slide-transition-impl.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry.h>
#include <limits>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
SlideTransitionPtr SlideTransition::New(Dali::Toolkit::Control control, const Vector2& direction, TimePeriod timePeriod)
{
  float delaySeconds = timePeriod.delaySeconds;
  if(delaySeconds < 0.0f)
  {
    DALI_LOG_WARNING("delay should be greater than 0.0f.\n");
    delaySeconds = 0.0f;
  }

  float durationSeconds = timePeriod.durationSeconds;
  if(durationSeconds < 0.0f)
  {
    DALI_LOG_WARNING("duration should be greater than 0.0f.\n");
    durationSeconds = 0.0f;
  }

  SlideTransitionPtr slideTransition = new SlideTransition(control, direction, TimePeriod(delaySeconds, durationSeconds));

  // Second-phase construction
  slideTransition->Initialize();

  return slideTransition;
}

SlideTransition::SlideTransition(Dali::Toolkit::Control control, const Vector2& direction, TimePeriod timePeriod)
: TransitionBase(),
  mTargetControl(control),
  mDirection(direction)
{
  SetTarget(control);
  SetTimePeriod(timePeriod);
}

SlideTransition::~SlideTransition()
{
}

void SlideTransition::SetDirection(const Vector2& direction)
{
  mDirection = direction;
  mDirection.Normalize();
}

Vector2 SlideTransition::GetDirection() const
{
  return mDirection;
}

void SlideTransition::OnPlay()
{
  Dali::Toolkit::Control targetControl = mTargetControl.GetHandle();
  if(!targetControl || !targetControl[Dali::Actor::Property::CONNECTED_TO_SCENE])
  {
    DALI_LOG_ERROR("The Control is not added on the window\n");
    return;
  }

  Property::Map startPropertyMap;
  Property::Map finishPropertyMap;

  Vector3 currentPosition = targetControl[Dali::Actor::Property::POSITION];
  Vector3 currentScale    = targetControl[Dali::Actor::Property::SCALE];

  Vector3 size = targetControl[Dali::Actor::Property::SIZE];
  size *= currentScale;

  Vector2 windowSize = DevelWindow::Get(targetControl).GetSize();
  // This checkPosition go outside of window(by following the direction), this targetControl will be out of window.
  Vector2 checkPosition = windowSize / 2.0f +
                          Vector2(currentPosition.x + ((mDirection.x < 0.0f) ? size.x / 2.0f : -size.x / 2.0f),
                                  currentPosition.y + ((mDirection.y < 0.0f) ? size.y / 2.0f : -size.y / 2.0f));

  float xScale = (mDirection.x == 0.0f) ? std::numeric_limits<float>::max()
                                        : ((mDirection.x < 0.0f) ? checkPosition.x : windowSize.x - checkPosition.x) / std::abs(mDirection.x);
  float yScale = (mDirection.y == 0.0f) ? std::numeric_limits<float>::max()
                                        : ((mDirection.y < 0.0f) ? checkPosition.y : windowSize.y - checkPosition.y) / std::abs(mDirection.y);

  Vector2 displacement = mDirection * std::min(xScale, yScale);

  Vector3 startPosition;
  Vector3 finishPosition;
  if(IsAppearingTransition())
  {
    startPosition  = currentPosition + Vector3(displacement);
    finishPosition = currentPosition;
  }
  else
  {
    startPosition  = currentPosition;
    finishPosition = currentPosition + Vector3(displacement);
  }

  startPropertyMap.Insert(Dali::Actor::Property::POSITION, startPosition);
  finishPropertyMap.Insert(Dali::Actor::Property::POSITION, finishPosition);

  SetStartPropertyMap(startPropertyMap);
  SetFinishPropertyMap(finishPropertyMap);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
