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
  float durationSeconds = timePeriod.durationSeconds;
  if(durationSeconds < 0.0f)
  {
    DALI_LOG_WARNING("duration should be greater than 0.0f.\n");
    durationSeconds = 0.0f;
  }

  TransitionPtr transition = new Transition(source, destination, timePeriod);

  // Second-phase construction
  transition->Initialize();

  return transition;
}

Transition::Transition(Dali::Toolkit::Control source, Dali::Toolkit::Control destination, TimePeriod timePeriod)
: TransitionBase(),
  mSourceView(source),
  mDestinationView(destination),
  mShowSourceAfterFinished(true)
{
  SetTargetView(destination);
  SetDuration(timePeriod.durationSeconds);
  SetDelay(timePeriod.delaySeconds);
}

Transition::~Transition()
{
}

void Transition::OnPlay()
{
  if(!mSourceView[Dali::Actor::Property::CONNECTED_TO_SCENE] ||
     !mDestinationView[Dali::Actor::Property::CONNECTED_TO_SCENE])
  {
    DALI_LOG_ERROR("The source or destination is not added on the window\n");
    return;
  }

  //Make startPropertyMap and finishPropertyMap to use for property animation.
  Matrix     sourceWorldTransform = mSourceView[Dali::Actor::Property::WORLD_MATRIX];
  Vector3    sourcePosition, sourceScale;
  Quaternion sourceOrientation;
  sourceWorldTransform.GetTransformComponents(sourcePosition, sourceOrientation, sourceScale);

  Matrix     destinationWorldTransform = GetWorldTransform(mDestinationView);
  Vector3    destinationPosition, destinationScale;
  Quaternion destinationOrientation;
  destinationWorldTransform.GetTransformComponents(destinationPosition, destinationOrientation, destinationScale);

  Vector3       targetSize  = mDestinationView[Dali::Actor::Property::SIZE];
  Vector4       targetColor = GetWorldColor(mDestinationView);
  Property::Map startPropertyMap;
  Property::Map finishPropertyMap;

  // Use world transform if this transition requires animation of transform.
  mDestinationView[Dali::Actor::Property::ANCHOR_POINT]               = AnchorPoint::CENTER;
  mDestinationView[Dali::Actor::Property::PARENT_ORIGIN]              = ParentOrigin::CENTER;
  mDestinationView[Dali::Actor::Property::POSITION_USES_ANCHOR_POINT] = true;
  mDestinationView[Dali::Actor::Property::INHERIT_POSITION]           = false;
  mDestinationView[Dali::Actor::Property::INHERIT_ORIENTATION]        = false;
  mDestinationView[Dali::Actor::Property::INHERIT_SCALE]              = false;
  mDestinationView[Dali::Actor::Property::COLOR_MODE]                 = Dali::ColorMode::USE_OWN_COLOR;

  // Set animation of Transform
  startPropertyMap.Insert(Dali::Actor::Property::POSITION, sourcePosition);
  finishPropertyMap.Insert(Dali::Actor::Property::POSITION, destinationPosition);

  startPropertyMap.Insert(Dali::Actor::Property::ORIENTATION, sourceOrientation);
  finishPropertyMap.Insert(Dali::Actor::Property::ORIENTATION, destinationOrientation);

  startPropertyMap.Insert(Dali::Actor::Property::SCALE, sourceScale);
  finishPropertyMap.Insert(Dali::Actor::Property::SCALE, destinationScale);

  Vector4 sourceColor = mSourceView.GetCurrentProperty<Vector4>(Dali::Actor::Property::WORLD_COLOR);
  startPropertyMap.Insert(Dali::Actor::Property::COLOR, sourceColor);
  finishPropertyMap.Insert(Dali::Actor::Property::COLOR, targetColor);

  // Set animation for other properties if source and destination is different.
  Vector3 sourceSize = mSourceView.GetCurrentProperty<Vector3>(Dali::Actor::Property::SIZE);
  if(sourceSize != targetSize)
  {
    startPropertyMap.Insert(Dali::Actor::Property::SIZE, sourceSize);
    finishPropertyMap.Insert(Dali::Actor::Property::SIZE, targetSize);
  }

  SetStartPropertyMap(startPropertyMap);
  SetFinishPropertyMap(finishPropertyMap);

  // source View becomes transparent during transition.
  GetImplementation(mSourceView).SetTransparent(true);

  Dali::Animation animation = GetAnimation();
  if(!animation)
  {
    DALI_LOG_ERROR("animation is still not initialized\n");
    return;
  }
  GetImplementation(mDestinationView).CreateTransitions(animation, mSourceView, GetAlphaFunction(), TimePeriod(GetDelay(), GetDuration()));
}

void Transition::OnFinished()
{
  if(mShowSourceAfterFinished)
  {
    GetImplementation(mSourceView).SetTransparent(false);
  }
}

void Transition::ShowSourceAfterFinished(bool showSourceAfterFinished)
{
  mShowSourceAfterFinished = showSourceAfterFinished;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
