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
#include <dali-toolkit/internal/transition/transition-base-impl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/animation/key-frames.h>
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
Property::Map GetOriginalProperties(Dali::Toolkit::Control control)
{
  Property::Map propertyMap;
  propertyMap.Insert(Dali::Actor::Property::ANCHOR_POINT, control[Dali::Actor::Property::ANCHOR_POINT]);
  propertyMap.Insert(Dali::Actor::Property::PARENT_ORIGIN, control[Dali::Actor::Property::PARENT_ORIGIN]);
  propertyMap.Insert(Dali::Actor::Property::POSITION_USES_ANCHOR_POINT, control[Dali::Actor::Property::POSITION_USES_ANCHOR_POINT]);
  propertyMap.Insert(Dali::Actor::Property::INHERIT_POSITION, control[Dali::Actor::Property::INHERIT_POSITION]);
  propertyMap.Insert(Dali::Actor::Property::INHERIT_ORIENTATION, control[Dali::Actor::Property::INHERIT_ORIENTATION]);
  propertyMap.Insert(Dali::Actor::Property::INHERIT_SCALE, control[Dali::Actor::Property::INHERIT_SCALE]);
  propertyMap.Insert(Dali::Actor::Property::COLOR_MODE, control[Dali::Actor::Property::COLOR_MODE]);
  propertyMap.Insert(Dali::Actor::Property::HEIGHT_RESIZE_POLICY, control[Dali::Actor::Property::HEIGHT_RESIZE_POLICY]);
  propertyMap.Insert(Dali::Actor::Property::WIDTH_RESIZE_POLICY, control[Dali::Actor::Property::WIDTH_RESIZE_POLICY]);
  propertyMap.Insert(Dali::Actor::Property::POSITION, control[Dali::Actor::Property::POSITION]);
  propertyMap.Insert(Dali::Actor::Property::ORIENTATION, control[Dali::Actor::Property::ORIENTATION]);
  propertyMap.Insert(Dali::Actor::Property::SCALE, control[Dali::Actor::Property::SCALE]);
  propertyMap.Insert(Dali::Actor::Property::COLOR, control[Dali::Actor::Property::COLOR]);

  return propertyMap;
}
} // anonymous namespace

TransitionBasePtr TransitionBase::New()
{
  TransitionBasePtr transition = new TransitionBase();

  // Second-phase construction
  transition->Initialize();

  return transition;
}

TransitionBase::TransitionBase()
: mAlphaFunction(Dali::AlphaFunction::DEFAULT),
  mTimePeriod(TimePeriod(0.0f)),
  mTransitionWithChild(false),
  mMoveTargetChildren(false),
  mIsAppearingTransition(true),
  mIsPairTransition(false)
{
}

void TransitionBase::Initialize()
{
  RegisterObject();
}

void TransitionBase::SetTimePeriod(const Dali::TimePeriod& timePeriod)
{
  if(timePeriod.durationSeconds < 0.0f)
  {
    DALI_LOG_DEBUG_INFO("Duration should be greater than 0.0f.\n");
  }
  else
  {
    mTimePeriod.durationSeconds = timePeriod.durationSeconds;
  }

  if(timePeriod.delaySeconds < 0.0f)
  {
    DALI_LOG_DEBUG_INFO("Delay should be greater than 0.0f.\n");
    return;
  }
  else
  {
    mTimePeriod.delaySeconds = timePeriod.delaySeconds;
  }
}

Dali::TimePeriod TransitionBase::GetTimePeriod() const
{
  return mTimePeriod;
}

void TransitionBase::TransitionWithChild(bool transitionWithChild)
{
  mTransitionWithChild = transitionWithChild;
}

void TransitionBase::PreProcess(Dali::Animation animation)
{
  mAnimation = animation;
  // Retrieve original property map of mTarget to backup and to reset after transition is finished.
  mOriginalPropertyMap = GetOriginalProperties(mTarget);
  mMoveTargetChildren  = false;
  if(!mTransitionWithChild && mTarget.GetChildCount() > 0)
  {
    mMoveTargetChildren = true;
    CopyTarget();
  }
  GetImplementation(mTarget).SetTransparent(false);
}

void TransitionBase::Play()
{
  if(!mTarget[Dali::Actor::Property::CONNECTED_TO_SCENE])
  {
    DALI_LOG_ERROR("The target is not added on the window\n");
    return;
  }

  // Set world transform and color to the target control to make it independent of the parent control and its transition.
  // The properties will be returned at the TransitionFinished() method.
  Matrix     targetWorldTransform = DevelActor::GetWorldTransform(mTarget);
  Vector3    targetPosition, targetScale;
  Quaternion targetOrientation;
  targetWorldTransform.GetTransformComponents(targetPosition, targetOrientation, targetScale);
  Vector4 targetColor = DevelActor::GetWorldColor(mTarget);

  mTarget[Dali::Actor::Property::ANCHOR_POINT]               = AnchorPoint::CENTER;
  mTarget[Dali::Actor::Property::PARENT_ORIGIN]              = ParentOrigin::CENTER;
  mTarget[Dali::Actor::Property::POSITION_USES_ANCHOR_POINT] = true;
  mTarget[Dali::Actor::Property::INHERIT_POSITION]           = false;
  mTarget[Dali::Actor::Property::INHERIT_ORIENTATION]        = false;
  mTarget[Dali::Actor::Property::INHERIT_SCALE]              = false;
  mTarget[Dali::Actor::Property::COLOR_MODE]                 = Dali::ColorMode::USE_OWN_COLOR;

  mTarget[Dali::Actor::Property::POSITION]    = targetPosition;
  mTarget[Dali::Actor::Property::SCALE]       = targetScale;
  mTarget[Dali::Actor::Property::ORIENTATION] = targetOrientation;
  mTarget[Dali::Actor::Property::COLOR]       = targetColor;

  OnPlay();

  SetAnimation();
}

void TransitionBase::SetAnimation()
{
  if(!mAnimation)
  {
    DALI_LOG_ERROR("animation is not initialized\n");
    return;
  }

  for(uint32_t i = 0; i < mStartPropertyMap.Count(); ++i)
  {
    Property::Value* finishValue = mFinishPropertyMap.Find(mStartPropertyMap.GetKeyAt(i).indexKey);
    if(finishValue)
    {
      // If this transition is appearing transition, this property keeps start value during delay.
      // If multiple transitions are applied to this Control and others run before this transition,
      // this property should keep start value until this transition starts.
      if(!IsPairTransition() && IsAppearingTransition() && mTimePeriod.delaySeconds > Dali::Math::MACHINE_EPSILON_10)
      {
        mTarget.SetProperty(mStartPropertyMap.GetKeyAt(i).indexKey, mStartPropertyMap.GetValue(i));
      }
      AnimateBetween(mTarget, mStartPropertyMap.GetKeyAt(i).indexKey, mStartPropertyMap.GetValue(i), *finishValue);
    }
  }
}

void TransitionBase::AnimateBetween(Dali::Toolkit::Control target, Property::Index index, Property::Value sourceValue, Property::Value destinationValue)
{
  if(mAnimation)
  {
    // To make each property keep start value during delay time.
    // When this transition is not Pair transition, it is not required.
    // (For appearing transition, the mTarget control will not be shown during delay time,
    //  For disapplearing transition, the property of mTarget control keeps current value during delay time)
    if(IsPairTransition() && mTimePeriod.delaySeconds > Dali::Math::MACHINE_EPSILON_10)
    {
      Dali::KeyFrames initialKeyframes = Dali::KeyFrames::New();
      initialKeyframes.Add(0.0f, sourceValue);
      initialKeyframes.Add(1.0f, sourceValue);
      mAnimation.AnimateBetween(Property(target, index), initialKeyframes, TimePeriod(mTimePeriod.delaySeconds));
    }
    Dali::KeyFrames keyframes = Dali::KeyFrames::New();
    keyframes.Add(0.0f, sourceValue);
    keyframes.Add(1.0f, destinationValue);
    mAnimation.AnimateBetween(Property(target, index), keyframes, mAlphaFunction, mTimePeriod);
  }
}

void TransitionBase::CopyTarget()
{
  mCopiedActor = Dali::Actor::New();
  mTarget.GetParent().Add(mCopiedActor);

  mCopiedActor[Dali::DevelActor::Property::SIBLING_ORDER] = static_cast<int32_t>(mTarget[Dali::DevelActor::Property::SIBLING_ORDER]) + 1;
  while(mTarget.GetChildCount() > 0)
  {
    Dali::Actor child = mTarget.GetChildAt(0);
    Dali::DevelActor::SwitchParent(child, mCopiedActor);
  }

  // Copy Size property to mCopiedActor because Size is not included mOriginalPropertyMap.
  mCopiedActor[Dali::Actor::Property::SIZE] = mTarget.GetProperty<Vector3>(Dali::Actor::Property::SIZE);
  mCopiedActor.SetProperties(mOriginalPropertyMap);
}

void TransitionBase::TransitionFinished()
{
  OnFinished();

  mTarget.SetProperties(mOriginalPropertyMap);
  if(mMoveTargetChildren)
  {
    while(mCopiedActor.GetChildCount() > 0)
    {
      Dali::Actor child = mCopiedActor.GetChildAt(0);
      Dali::DevelActor::SwitchParent(child, mTarget);
    }
    mCopiedActor.Unparent();
    mCopiedActor.Reset();
  }
  mAnimation.Reset();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
