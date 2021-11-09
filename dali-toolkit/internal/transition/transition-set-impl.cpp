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
#include <dali-toolkit/internal/transition/transition-set-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/transition/transition-lifecycle-controller.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
// Signals
static constexpr std::string_view SIGNAL_FINISHED = "finished";

static constexpr float OPACITY_TRANSPARENT = 0.0f;

float CustomAlphaFunction(float progress)
{
  return (progress >= 1.0f) ? 1.0f : 0.0f;
}

BaseHandle Create()
{
  return Dali::Toolkit::TransitionSet::New();
}

TypeRegistration mType(typeid(Dali::Toolkit::TransitionSet), typeid(Dali::BaseHandle), Create);

SignalConnectorType signalConnector1(mType, std::string(SIGNAL_FINISHED), &TransitionSet::DoConnectSignal);

} // anonymous namespace

TransitionSetPtr TransitionSet::New()
{
  TransitionSetPtr transitionSet = new TransitionSet();

  return transitionSet;
}

TransitionSet::TransitionSet()
{
}

TransitionSet::~TransitionSet()
{
  mTransitions.clear();
}

void TransitionSet::AddTransition(TransitionBasePtr transition)
{
  mTransitions.push_back(transition);
}

TransitionBase* TransitionSet::GetTransitionAt(uint32_t index) const
{
  TransitionBase* result(nullptr);
  if(index < GetTransitionCount())
  {
    result = mTransitions[index].Get();
  }
  else
  {
    DALI_LOG_ERROR("Error: Invalid index to TransitionSet::GetTransitionAt\n");
  }

  return result;
}

uint32_t TransitionSet::GetTransitionCount() const
{
  return mTransitions.size();
}

void TransitionSet::Play()
{
  Adaptor::Get().RegisterProcessor(*this, true);
  Adaptor::Get().RegisterProcessor(*this, false);
  TransitionLifecycleController::GetInstance().AddTransitions(Dali::Toolkit::TransitionSet(this));
}

void TransitionSet::TransitionPreProcess()
{
  float lastDuration = 0.0f;
  for(auto&& transition : mTransitions)
  {
    TimePeriod timePeriod = transition->GetTimePeriod();
    if(lastDuration <= timePeriod.durationSeconds + timePeriod.delaySeconds)
    {
      lastDuration = timePeriod.durationSeconds + timePeriod.delaySeconds;
    }
  }
  mAnimation = Dali::Animation::New(lastDuration);

  for(auto&& transition : mTransitions)
  {
    transition->PreProcess(mAnimation);
  }
}

void TransitionSet::TransitionStart()
{
  std::vector<std::pair<Dali::Actor, float>> minimumDelays;
  for(auto&& transition : mTransitions)
  {
    transition->Play();

    // If target Control has appearing transition, the target will not be rendered during delay.
    // And if the Control has multiple transitions, the target will not be rendered during minimum delay of the transitions.
    // Here we can find minimum delay of each target.
    if(!transition->IsPairTransition() && transition->IsAppearingTransition())
    {
      bool found = false;
      for(uint32_t index = 0; index < minimumDelays.size(); ++index)
      {
        if(minimumDelays[index].first == transition->GetTarget())
        {
          minimumDelays[index].second = std::min(minimumDelays[index].second, transition->GetTimePeriod().delaySeconds);
          found = true;
          break;
        }
      }
      if(!found)
      {
        minimumDelays.push_back(std::pair<Dali::Actor, float>(transition->GetTarget(), transition->GetTimePeriod().delaySeconds));
      }
    }
  }

  // If the target has delay that is larger than 0, hide the target during minimum delay.
  // The custom alpha function make the target hide just during delay.
  for(auto&& delay : minimumDelays)
  {
    if(delay.second > Dali::Math::MACHINE_EPSILON_10)
    {
      Dali::KeyFrames initialKeyframes = Dali::KeyFrames::New();
      initialKeyframes.Add(0.0f, OPACITY_TRANSPARENT);
      initialKeyframes.Add(1.0f, delay.first.GetProperty<float>(Dali::Actor::Property::OPACITY));

      AlphaFunction alpha(&CustomAlphaFunction);
      mAnimation.AnimateBetween(Property(delay.first, Dali::Actor::Property::OPACITY), initialKeyframes, alpha, TimePeriod(delay.second));
    }
  }

  mAnimation.FinishedSignal().Connect(this, &TransitionSet::TransitionFinished);
  mAnimation.Play();
}

void TransitionSet::TransitionFinished(Dali::Animation& source)
{
  // Call TransitionFinished() in reverse order.
  // This let the first copied original properties will be return again at the final.
  std::vector<TransitionBasePtr>::reverse_iterator riter;
  for(riter = mTransitions.rbegin(); riter != mTransitions.rend(); riter++)
  {
    (*riter)->TransitionFinished();
  }

  EmitFinishedSignal();
}

Dali::Toolkit::TransitionSet::TransitionSetSignalType& TransitionSet::FinishedSignal()
{
  return mFinishedSignal;
}

void TransitionSet::EmitFinishedSignal()
{
  if(!mFinishedSignal.Empty())
  {
    Dali::Toolkit::TransitionSet handle(this);
    mFinishedSignal.Emit(handle);
  }
}

bool TransitionSet::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  bool           connected(false);
  TransitionSet* transitionSet = static_cast<TransitionSet*>(object); // TypeRegistry guarantees that this is the correct type.

  if(SIGNAL_FINISHED == signalName)
  {
    transitionSet->FinishedSignal().Connect(tracker, functor);
    connected = true;
  }

  return connected;
}

void TransitionSet::Process(bool postProcessor)
{
  if(!postProcessor)
  {
    TransitionPreProcess();
  }
  else
  {
    TransitionStart();
  }
  Adaptor::Get().UnregisterProcessor(*this, postProcessor);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
