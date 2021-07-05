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
#include <dali-toolkit/internal/transition/fade-impl.h>

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
namespace
{
const Dali::AlphaFunction DEFAULT_ALPHA_FUNCTION(Dali::AlphaFunction::DEFAULT);

} // anonymous namespace

FadePtr Fade::New(Dali::Toolkit::Control control, float opacity, TimePeriod timePeriod)
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

  FadePtr fade = new Fade(control, Dali::Clamp(opacity, 0.0f, 1.0f), TimePeriod(delaySeconds, durationSeconds));

  // Second-phase construction
  fade->Initialize();

  return fade;
}

Fade::Fade(Dali::Toolkit::Control control, float opacity, TimePeriod timePeriod)
: TransitionBase(),
  mTargetControl(control),
  mOpacity(opacity)
{
  SetTarget(control);
  SetTimePeriod(timePeriod);
}

Fade::~Fade()
{
}

void Fade::OnPlay()
{
  Dali::Toolkit::Control targetControl = mTargetControl.GetHandle();
  if(!targetControl || !targetControl[Dali::Actor::Property::CONNECTED_TO_SCENE])
  {
    DALI_LOG_ERROR("The Control is not added on the window\n");
    return;
  }

  Property::Map initialPropertyMap;
  Property::Map startPropertyMap;
  Property::Map finishPropertyMap;

  float targetOpacity                              = GetWorldColor(targetControl).a;
  targetControl[Dali::Actor::Property::COLOR_MODE] = Dali::ColorMode::USE_OWN_COLOR;

  if(IsAppearingTransition())
  {
    initialPropertyMap.Insert(Dali::Actor::Property::OPACITY, 0.0f);
    startPropertyMap.Insert(Dali::Actor::Property::OPACITY, mOpacity * targetOpacity);
    finishPropertyMap.Insert(Dali::Actor::Property::OPACITY, targetOpacity);
  }
  else
  {
    initialPropertyMap.Insert(Dali::Actor::Property::OPACITY, targetOpacity);
    startPropertyMap.Insert(Dali::Actor::Property::OPACITY, targetOpacity);
    finishPropertyMap.Insert(Dali::Actor::Property::OPACITY, mOpacity * targetOpacity);
  }

  SetInitialPropertyMap(initialPropertyMap);
  SetStartPropertyMap(startPropertyMap);
  SetFinishPropertyMap(finishPropertyMap);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
