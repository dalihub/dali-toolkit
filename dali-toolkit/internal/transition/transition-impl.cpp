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
#include <dali-toolkit/internal/controls/control/control-data-impl.h>

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

TransitionPtr Transition::New(Dali::Toolkit::Control source, Dali::Toolkit::Control destination, bool useDestinationTarget, TimePeriod timePeriod)
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

  TransitionPtr transition = new Transition(source, destination, useDestinationTarget, TimePeriod(delaySeconds, durationSeconds));

  // Second-phase construction
  transition->Initialize();

  return transition;
}

Transition::Transition(Dali::Toolkit::Control source, Dali::Toolkit::Control destination, bool useDestinationTarget, TimePeriod timePeriod)
: TransitionBase(),
  mUseDestinationTarget(useDestinationTarget),
  mOriginalSize(),
  mSourceControl(source),
  mDestinationControl(destination)
{
  SetTarget(mUseDestinationTarget ? destination : source);
  SetTimePeriod(timePeriod);
  SetPairTransition(true);
}

Transition::~Transition()
{
}

void Transition::OnPlay()
{
  Dali::Toolkit::Control sourceControl = mSourceControl.GetHandle();
  Dali::Toolkit::Control destinationControl = mDestinationControl.GetHandle();
  if(!sourceControl || !sourceControl[Dali::Actor::Property::CONNECTED_TO_SCENE] ||
     !destinationControl || !destinationControl[Dali::Actor::Property::CONNECTED_TO_SCENE])
  {
    DALI_LOG_ERROR("The source or destination is not added on the window\n");
    return;
  }

  //Make startPropertyMap and finishPropertyMap to use for property animation.
  Matrix     sourceWorldTransform = GetWorldTransform(sourceControl);
  Vector3    sourcePosition, sourceScale;
  Quaternion sourceOrientation;
  sourceWorldTransform.GetTransformComponents(sourcePosition, sourceOrientation, sourceScale);

  Matrix     destinationWorldTransform = GetWorldTransform(destinationControl);
  Vector3    destinationPosition, destinationScale;
  Quaternion destinationOrientation;
  destinationWorldTransform.GetTransformComponents(destinationPosition, destinationOrientation, destinationScale);

  Property::Map startPropertyMap;
  Property::Map finishPropertyMap;

  // Set animation of Transform
  startPropertyMap.Insert(Dali::Actor::Property::POSITION, sourcePosition);
  finishPropertyMap.Insert(Dali::Actor::Property::POSITION, destinationPosition);

  startPropertyMap.Insert(Dali::Actor::Property::ORIENTATION, sourceOrientation);
  finishPropertyMap.Insert(Dali::Actor::Property::ORIENTATION, destinationOrientation);

  startPropertyMap.Insert(Dali::Actor::Property::SCALE, sourceScale);
  finishPropertyMap.Insert(Dali::Actor::Property::SCALE, destinationScale);

  Vector4 sourceColor      = GetWorldColor(sourceControl);
  Vector4 destinationColor = GetWorldColor(destinationControl);
  startPropertyMap.Insert(Dali::Actor::Property::COLOR, sourceColor);
  finishPropertyMap.Insert(Dali::Actor::Property::COLOR, destinationColor);

  // Set animation for other properties if source and destination is different.
  Vector3 sourceSize      = sourceControl[Dali::Actor::Property::SIZE];
  Vector3 destinationSize = destinationControl[Dali::Actor::Property::SIZE];
  if(sourceSize != destinationSize)
  {
    startPropertyMap.Insert(Dali::Actor::Property::SIZE, sourceSize);
    finishPropertyMap.Insert(Dali::Actor::Property::SIZE, destinationSize);
    if(!mUseDestinationTarget)
    {
      mOriginalSize = GetTargetControl().GetProperty<Vector3>(Dali::Actor::Property::SIZE);
    }
  }

  SetStartPropertyMap(startPropertyMap);
  SetFinishPropertyMap(finishPropertyMap);

  // source View becomes transparent during transition.
  Dali::Toolkit::Control waitingControl = GetWaitingControl();
  if(IsTransitionWithChild())
  {
    waitingControl[Dali::Actor::Property::VISIBLE] = false;
  }
  else
  {
    GetImplementation(waitingControl).SetTransparent(true);
  }

  Dali::Animation animation = GetAnimation();
  if(!animation)
  {
    DALI_LOG_ERROR("animation is still not initialized\n");
    return;
  }

  mOriginalVisualProperties.clear();
  std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>> destinationVisualProperties;
  Dali::Toolkit::Control                                             targetControl   = GetTargetControl();
  Internal::Control&                                                 internalControl = Toolkit::Internal::GetImplementation(targetControl);
  Internal::Control::Impl&                                           controlDataImpl = Toolkit::Internal::Control::Impl::Get(internalControl);
  controlDataImpl.CreateTransitions(mOriginalVisualProperties, destinationVisualProperties, sourceControl, destinationControl);

  for(uint32_t index = 0; index < mOriginalVisualProperties.size(); ++index)
  {
    Dali::Property::Map source      = mOriginalVisualProperties[index].second;
    Dali::Property::Map destination = destinationVisualProperties[index].second;
    for(size_t i = 0; i < source.Count(); ++i)
    {
      Dali::Property property = DevelControl::GetVisualProperty(targetControl, mOriginalVisualProperties[index].first, source.GetKeyAt(i));
      if(GetTimePeriod().delaySeconds > 0.0f)
      {
        Dali::KeyFrames initialKeyframes = Dali::KeyFrames::New();
        initialKeyframes.Add(0.0f, source.GetValue(i));
        animation.AnimateBetween(property, initialKeyframes, TimePeriod(GetTimePeriod().delaySeconds));
      }
      Dali::KeyFrames keyframes = Dali::KeyFrames::New();
      keyframes.Add(0.0f, source.GetValue(i));
      keyframes.Add(1.0f, destination.GetValue(i));
      animation.AnimateBetween(property, keyframes, GetAlphaFunction(), GetTimePeriod());
    }
  }
}

void Transition::OnFinished()
{
  Dali::Toolkit::Control waitingControl = GetWaitingControl();
  if(!waitingControl)
  {
    return;
  }

  if(!mUseDestinationTarget)
  {
    Dali::Toolkit::Control target         = GetTargetControl();
    Dali::Animation        resetAnimation = Dali::Animation::New(0.0f);
    if(mOriginalSize != target.GetProperty<Vector3>(Dali::Actor::Property::SIZE))
    {
      // Use Animation not to notify size change and not to change width and height resize policy.
      resetAnimation.AnimateTo(Dali::Property(target, Dali::Actor::Property::SIZE), mOriginalSize);
    }
    resetAnimation.Play();

    Dali::Toolkit::Control   targetControl   = GetTargetControl();
    Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(targetControl);
    Internal::Control::Impl& controlDataImpl = Toolkit::Internal::Control::Impl::Get(internalControl);
    controlDataImpl.UpdateVisualProperties(mOriginalVisualProperties);
  }

  if(IsTransitionWithChild())
  {
    waitingControl[Dali::Actor::Property::VISIBLE] = true;
  }
  else
  {
    GetImplementation(waitingControl).SetTransparent(false);
  }
}

Dali::Toolkit::Control Transition::GetTargetControl()
{
  Dali::Toolkit::Control target = mUseDestinationTarget ? mDestinationControl.GetHandle() : mSourceControl.GetHandle();
  return target;
}

Dali::Toolkit::Control Transition::GetWaitingControl()
{
  Dali::Toolkit::Control waitingControl = mUseDestinationTarget ? mSourceControl.GetHandle() : mDestinationControl.GetHandle();
  return waitingControl;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
