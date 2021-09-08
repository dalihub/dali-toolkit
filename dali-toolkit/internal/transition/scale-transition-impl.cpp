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
#include <dali-toolkit/internal/transition/scale-transition-impl.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

ScaleTransitionPtr ScaleTransition::New(Dali::Toolkit::Control control, const Vector2& scaleFactor, TimePeriod timePeriod)
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

  ScaleTransitionPtr scaleTransition = new ScaleTransition(control, scaleFactor, TimePeriod(delaySeconds, durationSeconds));

  // Second-phase construction
  scaleTransition->Initialize();

  return scaleTransition;
}

ScaleTransition::ScaleTransition(Dali::Toolkit::Control control, const Vector2& scaleFactor, TimePeriod timePeriod)
: TransitionBase(),
  mTargetControl(control),
  mScaleFactor(scaleFactor)
{
  SetTarget(control);
  SetTimePeriod(timePeriod);
}

ScaleTransition::~ScaleTransition()
{
}

void ScaleTransition::SetScaleFactor(const Vector2& scaleFactor)
{
  mScaleFactor = scaleFactor;
}

Vector2 ScaleTransition::GetScaleFactor() const
{
  return mScaleFactor;
}

void ScaleTransition::OnPlay()
{
  Dali::Toolkit::Control targetControl = mTargetControl.GetHandle();
  if(!targetControl || !targetControl[Dali::Actor::Property::CONNECTED_TO_SCENE])
  {
    DALI_LOG_ERROR("The Control is not added on the window\n");
    return;
  }

  Property::Map startPropertyMap;
  Property::Map finishPropertyMap;

  Vector3 targetScale = targetControl[Dali::Actor::Property::SCALE];
  Vector3 scaleFactor = Vector3(mScaleFactor.x, mScaleFactor.y, 1.0f);
  if(IsAppearingTransition())
  {
    startPropertyMap.Insert(Dali::Actor::Property::SCALE, scaleFactor * targetScale);
    finishPropertyMap.Insert(Dali::Actor::Property::SCALE, targetScale);
  }
  else
  {
    startPropertyMap.Insert(Dali::Actor::Property::SCALE, targetScale);
    finishPropertyMap.Insert(Dali::Actor::Property::SCALE, scaleFactor * targetScale);
  }

  SetStartPropertyMap(startPropertyMap);
  SetFinishPropertyMap(finishPropertyMap);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
