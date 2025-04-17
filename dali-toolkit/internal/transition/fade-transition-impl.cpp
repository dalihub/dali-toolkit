/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/transition/fade-transition-impl.h>

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
FadeTransitionPtr FadeTransition::New(Dali::Toolkit::Control control, float opacity, TimePeriod timePeriod)
{
  float delaySeconds = timePeriod.delaySeconds;
  if(delaySeconds < 0.0f)
  {
    DALI_LOG_DEBUG_INFO("delay should be greater than 0.0f.\n");
    delaySeconds = 0.0f;
  }

  float durationSeconds = timePeriod.durationSeconds;
  if(durationSeconds < 0.0f)
  {
    DALI_LOG_DEBUG_INFO("duration should be greater than 0.0f.\n");
    durationSeconds = 0.0f;
  }

  FadeTransitionPtr fadeTransition = new FadeTransition(control, Dali::Clamp(opacity, 0.0f, 1.0f), TimePeriod(delaySeconds, durationSeconds));

  // Second-phase construction
  fadeTransition->Initialize();

  return fadeTransition;
}

FadeTransition::FadeTransition(Dali::Toolkit::Control control, float opacity, TimePeriod timePeriod)
: TransitionBase(),
  mTargetControl(control),
  mOpacity(opacity)
{
  SetTarget(control);
  SetTimePeriod(timePeriod);
}

FadeTransition::~FadeTransition()
{
}

void FadeTransition::OnPlay()
{
  Dali::Toolkit::Control targetControl = mTargetControl.GetHandle();
  if(!targetControl || !targetControl[Dali::Actor::Property::CONNECTED_TO_SCENE])
  {
    DALI_LOG_ERROR("The Control is not added on the window\n");
    return;
  }

  Property::Map startPropertyMap;
  Property::Map finishPropertyMap;

  float targetOpacity = targetControl[Dali::Actor::Property::OPACITY];
  if(IsAppearingTransition())
  {
    startPropertyMap.Insert(Dali::Actor::Property::OPACITY, mOpacity * targetOpacity);
    finishPropertyMap.Insert(Dali::Actor::Property::OPACITY, targetOpacity);
  }
  else
  {
    startPropertyMap.Insert(Dali::Actor::Property::OPACITY, targetOpacity);
    finishPropertyMap.Insert(Dali::Actor::Property::OPACITY, mOpacity * targetOpacity);
  }

  SetStartPropertyMap(startPropertyMap);
  SetFinishPropertyMap(finishPropertyMap);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
