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
#include <dali-toolkit/internal/transition/transition-impl.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>

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

TransitionPtr Transition::New(Dali::Toolkit::Control source, Dali::Toolkit::Control destination, TimePeriod timePeriod)
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

  TransitionPtr transition = new Transition(source, destination, TimePeriod(delaySeconds, durationSeconds));

  // Second-phase construction
  transition->Initialize();

  return transition;
}

Transition::Transition(Dali::Toolkit::Control source, Dali::Toolkit::Control destination, TimePeriod timePeriod)
: TransitionBase(),
  mSourceControl(source),
  mDestinationControl(destination)
{
  SetTarget(destination);
  SetTimePeriod(timePeriod);
}

Transition::~Transition()
{
}

void Transition::OnPlay()
{
  if(!mSourceControl[Dali::Actor::Property::CONNECTED_TO_SCENE] ||
     !mDestinationControl[Dali::Actor::Property::CONNECTED_TO_SCENE])
  {
    DALI_LOG_ERROR("The source or destination is not added on the window\n");
    return;
  }

  //Make startPropertyMap and finishPropertyMap to use for property animation.
  Matrix     sourceWorldTransform = mSourceControl[Dali::Actor::Property::WORLD_MATRIX];
  Vector3    sourcePosition, sourceScale;
  Quaternion sourceOrientation;
  sourceWorldTransform.GetTransformComponents(sourcePosition, sourceOrientation, sourceScale);

  Matrix     destinationWorldTransform = GetWorldTransform(mDestinationControl);
  Vector3    destinationPosition, destinationScale;
  Quaternion destinationOrientation;
  destinationWorldTransform.GetTransformComponents(destinationPosition, destinationOrientation, destinationScale);

  Vector3       targetSize  = mDestinationControl[Dali::Actor::Property::SIZE];
  Vector4       targetColor = GetWorldColor(mDestinationControl);
  Property::Map startPropertyMap;
  Property::Map finishPropertyMap;

  // Use world transform if this transition requires animation of transform.
  mDestinationControl[Dali::Actor::Property::ANCHOR_POINT]               = AnchorPoint::CENTER;
  mDestinationControl[Dali::Actor::Property::PARENT_ORIGIN]              = ParentOrigin::CENTER;
  mDestinationControl[Dali::Actor::Property::POSITION_USES_ANCHOR_POINT] = true;
  mDestinationControl[Dali::Actor::Property::INHERIT_POSITION]           = false;
  mDestinationControl[Dali::Actor::Property::INHERIT_ORIENTATION]        = false;
  mDestinationControl[Dali::Actor::Property::INHERIT_SCALE]              = false;
  mDestinationControl[Dali::Actor::Property::COLOR_MODE]                 = Dali::ColorMode::USE_OWN_COLOR;

  // Set animation of Transform
  startPropertyMap.Insert(Dali::Actor::Property::POSITION, sourcePosition);
  finishPropertyMap.Insert(Dali::Actor::Property::POSITION, destinationPosition);

  startPropertyMap.Insert(Dali::Actor::Property::ORIENTATION, sourceOrientation);
  finishPropertyMap.Insert(Dali::Actor::Property::ORIENTATION, destinationOrientation);

  startPropertyMap.Insert(Dali::Actor::Property::SCALE, sourceScale);
  finishPropertyMap.Insert(Dali::Actor::Property::SCALE, destinationScale);

  Vector4 sourceColor = mSourceControl.GetCurrentProperty<Vector4>(Dali::Actor::Property::WORLD_COLOR);
  startPropertyMap.Insert(Dali::Actor::Property::COLOR, sourceColor);
  finishPropertyMap.Insert(Dali::Actor::Property::COLOR, targetColor);

  // Set animation for other properties if source and destination is different.
  Vector3 sourceSize = mSourceControl.GetCurrentProperty<Vector3>(Dali::Actor::Property::SIZE);
  if(sourceSize != targetSize)
  {
    startPropertyMap.Insert(Dali::Actor::Property::SIZE, sourceSize);
    finishPropertyMap.Insert(Dali::Actor::Property::SIZE, targetSize);
  }

  SetStartPropertyMap(startPropertyMap);
  SetFinishPropertyMap(finishPropertyMap);

  // source View becomes transparent during transition.
  if(IsTransitionWithChild())
  {
    mSourceControl[Dali::Actor::Property::VISIBLE] = false;
  }
  else
  {
    GetImplementation(mSourceControl).SetTransparent(true);
  }

  Dali::Animation animation = GetAnimation();
  if(!animation)
  {
    DALI_LOG_ERROR("animation is still not initialized\n");
    return;
  }
  Dali::Toolkit::DevelControl::CreateTransitions(mDestinationControl, animation, mSourceControl, GetAlphaFunction(), GetTimePeriod());
}

void Transition::OnFinished()
{
  if(IsTransitionWithChild())
  {
    mSourceControl[Dali::Actor::Property::VISIBLE] = true;
  }
  else
  {
    GetImplementation(mSourceControl).SetTransparent(false);
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
