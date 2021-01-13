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
const Dali::AlphaFunction DEFAULT_ALPHA_FUNCTION(Dali::AlphaFunction::DEFAULT);

} // anonymous namespace

TransitionBasePtr TransitionBase::New()
{
  TransitionBasePtr transition = new TransitionBase();

  // Second-phase construction
  transition->Initialize();

  return transition;
}

TransitionBase::TransitionBase()
: mAlphaFunction(DEFAULT_ALPHA_FUNCTION),
  mDurationSeconds(0.0f),
  mDelaySeconds(0.0f),
  mTransitionWithChild(false)
{
}

void TransitionBase::Initialize()
{
  RegisterObject();
}

void TransitionBase::SetDuration(float seconds)
{
  if(seconds < 0.0f)
  {
    DALI_LOG_WARNING("duration should be greater than 0.0f.\n");
    return;
  }

  mDurationSeconds = seconds;
}

float TransitionBase::GetDuration() const
{
  // This is not animatable; the cached value is up-to-date.
  return mDurationSeconds;
}

void TransitionBase::SetDelay(float seconds)
{
  if(seconds < 0.0f)
  {
    DALI_LOG_WARNING("delay should be greater than 0.0f.\n");
    return;
  }

  mDelaySeconds = seconds;
}

float TransitionBase::GetDelay() const
{
  // This is not animatable; the cached value is up-to-date.
  return mDelaySeconds;
}

void TransitionBase::SetTimePeriod(const Dali::TimePeriod& timePeriod)
{
  if(timePeriod.durationSeconds <= 0.0f || timePeriod.delaySeconds <= 0.0f)
  {
    DALI_LOG_WARNING("duration and delay should be greater than 0.0f.\n");
    return;
  }

  SetDuration(timePeriod.durationSeconds);
  SetDelay(timePeriod.delaySeconds);
}

void TransitionBase::TransitionWithChild(bool transitionWithChild)
{
  mTransitionWithChild = transitionWithChild;
}

void TransitionBase::SetPlay(Dali::Toolkit::TransitionSet transitionSet)
{
  mTransitionSet = transitionSet;
}

void TransitionBase::PreProcess(Dali::Animation animation)
{
  mAnimation            = animation;
  mOriginialPropertyMap = GetProperties(mTargetView);
  mMoveTargetChildren   = false;
  if(!mTransitionWithChild && mTargetView.GetChildCount() > 0)
  {
    mMoveTargetChildren = true;
    CopyTarget();
  }
  GetImplementation(mTargetView).SetTransparent(false);
}

void TransitionBase::Play()
{
  if(!mTargetView[Dali::Actor::Property::CONNECTED_TO_SCENE])
  {
    DALI_LOG_ERROR("The target view is not added on the window\n");
    return;
  }

  OnPlay();

  SetAnimation();
}

Property::Map TransitionBase::GetProperties(Dali::Toolkit::Control control)
{
  Property::Map propertyMap;
  propertyMap.Insert(Dali::Actor::Property::ANCHOR_POINT, control[Dali::Actor::Property::ANCHOR_POINT]);
  propertyMap.Insert(Dali::Actor::Property::PARENT_ORIGIN, control[Dali::Actor::Property::PARENT_ORIGIN]);
  propertyMap.Insert(Dali::Actor::Property::POSITION_USES_ANCHOR_POINT, control[Dali::Actor::Property::POSITION_USES_ANCHOR_POINT]);
  propertyMap.Insert(Dali::Actor::Property::INHERIT_POSITION, control[Dali::Actor::Property::INHERIT_POSITION]);
  propertyMap.Insert(Dali::Actor::Property::INHERIT_ORIENTATION, control[Dali::Actor::Property::INHERIT_ORIENTATION]);
  propertyMap.Insert(Dali::Actor::Property::INHERIT_SCALE, control[Dali::Actor::Property::INHERIT_SCALE]);
  propertyMap.Insert(Dali::Actor::Property::COLOR_MODE, control[Dali::Actor::Property::COLOR_MODE]);
  propertyMap.Insert(Dali::Actor::Property::POSITION, control[Dali::Actor::Property::POSITION]);
  propertyMap.Insert(Dali::Actor::Property::ORIENTATION, control[Dali::Actor::Property::ORIENTATION]);
  propertyMap.Insert(Dali::Actor::Property::SCALE, control[Dali::Actor::Property::SCALE]);
  propertyMap.Insert(Dali::Actor::Property::COLOR, control[Dali::Actor::Property::COLOR]);

  return propertyMap;
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
      AnimateBetween(mTargetView, mStartPropertyMap.GetKeyAt(i).indexKey, mStartPropertyMap.GetValue(i), *finishValue);
    }
  }
}

void TransitionBase::AnimateBetween(Dali::Toolkit::Control target, Property::Index index, Property::Value sourceValue, Property::Value destinationValue)
{
  if(mAnimation)
  {
    Dali::KeyFrames keyframes = Dali::KeyFrames::New();
    keyframes.Add(0.0f, sourceValue);
    keyframes.Add(1.0f, destinationValue);
    mAnimation.AnimateBetween(Property(target, index), keyframes, mAlphaFunction, TimePeriod(mDelaySeconds, mDurationSeconds));
  }
}

void TransitionBase::CopyTarget()
{
  mCopiedActor = Dali::Actor::New();
  mTargetView.GetParent().Add(mCopiedActor);
  mCopiedActor[Dali::DevelActor::Property::SIBLING_ORDER] = static_cast<int32_t>(mTargetView[Dali::DevelActor::Property::SIBLING_ORDER]) + 1;
  for(uint32_t i = 0; i < mTargetView.GetChildCount(); ++i)
  {
    Dali::Actor child = mTargetView.GetChildAt(i);
    Dali::DevelActor::SwitchParent(child, mCopiedActor);
  }

  // Animation to set property instantly without SetProperty.
  mCopiedActor[Dali::Actor::Property::SIZE] = mTargetView.GetProperty<Vector3>(Dali::Actor::Property::SIZE);
  mCopiedActor.SetProperties(mOriginialPropertyMap);
}

void TransitionBase::TransitionFinished()
{
  OnFinished();

  mTargetView.SetProperties(mOriginialPropertyMap);
  if(mMoveTargetChildren)
  {
    for(uint32_t i = 0; i < mCopiedActor.GetChildCount(); ++i)
    {
      Dali::Actor child = mCopiedActor.GetChildAt(i);
      Dali::DevelActor::SwitchParent(child, mTargetView);
    }
    mCopiedActor.Unparent();
    mCopiedActor.Reset();
  }
  mTransitionSet.Reset();
  mAnimation.Reset();
}

Matrix TransitionBase::GetWorldTransform(Dali::Actor actor)
{
  enum InheritanceMode
  {
    DONT_INHERIT_TRANSFORM = 0,
    INHERIT_POSITION       = 1,
    INHERIT_SCALE          = 2,
    INHERIT_ORIENTATION    = 4,
    INHERIT_ALL            = INHERIT_POSITION | INHERIT_SCALE | INHERIT_ORIENTATION,
  };

  std::vector<Dali::Actor>     descentList;
  std::vector<InheritanceMode> inheritanceModeList;
  Dali::Actor                  currentActor = actor;
  int                          inheritance  = 0;
  do
  {
    inheritance = (static_cast<int>(currentActor.GetProperty<bool>(Dali::Actor::Property::INHERIT_ORIENTATION)) << 2) +
                  (static_cast<int>(currentActor.GetProperty<bool>(Dali::Actor::Property::INHERIT_SCALE)) << 1) +
                  static_cast<int>(currentActor.GetProperty<bool>(Dali::Actor::Property::INHERIT_POSITION));
    inheritanceModeList.push_back(static_cast<InheritanceMode>(inheritance));
    descentList.push_back(currentActor);
    currentActor = currentActor.GetParent();
  } while(inheritance != DONT_INHERIT_TRANSFORM && currentActor);

  Matrix  worldMatrix;
  Vector3 localPosition;
  for(unsigned int i(descentList.size() - 1); i < descentList.size(); --i)
  {
    Vector3    anchorPoint             = descentList[i].GetProperty<Vector3>(Dali::Actor::Property::ANCHOR_POINT);
    Vector3    parentOrigin            = descentList[i].GetProperty<Vector3>(Dali::Actor::Property::PARENT_ORIGIN);
    bool       positionUsesAnchorPoint = descentList[i].GetProperty<bool>(Dali::Actor::Property::POSITION_USES_ANCHOR_POINT);
    Vector3    size                    = descentList[i].GetProperty<Vector3>(Dali::Actor::Property::SIZE);
    Vector3    actorPosition           = descentList[i].GetProperty<Vector3>(Dali::Actor::Property::POSITION);
    Quaternion localOrientation        = descentList[i].GetProperty<Quaternion>(Dali::Actor::Property::ORIENTATION);
    Vector3    localScale              = descentList[i].GetProperty<Vector3>(Dali::Actor::Property::SCALE);

    Vector3 centerPosition = CalculateCenterPosition(anchorPoint, positionUsesAnchorPoint, size, localScale, localOrientation);
    if(inheritanceModeList[i] != DONT_INHERIT_TRANSFORM && descentList[i].GetParent())
    {
      Matrix  localMatrix;
      Vector3 parentSize = descentList[i + 1].GetProperty<Vector3>(Dali::Actor::Property::SIZE);
      if(inheritanceModeList[i] == INHERIT_ALL)
      {
        localPosition = actorPosition + centerPosition + (parentOrigin - Vector3(0.5f, 0.5f, 0.5f)) * parentSize;
        localMatrix.SetTransformComponents(localScale, localOrientation, localPosition);

        //Update the world matrix
        Matrix tempMatrix;
        Matrix::Multiply(tempMatrix, localMatrix, worldMatrix);
        worldMatrix = tempMatrix;
      }
      else
      {
        Vector3    parentPosition, parentScale;
        Quaternion parentOrientation;
        worldMatrix.GetTransformComponents(parentPosition, parentOrientation, parentScale);

        if((inheritanceModeList[i] & INHERIT_SCALE) == 0)
        {
          //Don't inherit scale
          localScale /= parentScale;
        }

        if((inheritanceModeList[i] & INHERIT_ORIENTATION) == 0)
        {
          //Don't inherit orientation
          parentOrientation.Invert();
          localOrientation = parentOrientation * localOrientation;
        }

        if((inheritanceModeList[i] & INHERIT_POSITION) == 0)
        {
          localMatrix.SetTransformComponents(localScale, localOrientation, Vector3::ZERO);
          Matrix tempMatrix;
          Matrix::Multiply(tempMatrix, localMatrix, worldMatrix);
          worldMatrix = tempMatrix;
          worldMatrix.SetTranslation(actorPosition + centerPosition);
        }
        else
        {
          localPosition = actorPosition + centerPosition + (parentOrigin - Vector3(0.5f, 0.5f, 0.5f)) * parentSize;
          localMatrix.SetTransformComponents(localScale, localOrientation, localPosition);
          Matrix tempMatrix;
          Matrix::Multiply(tempMatrix, localMatrix, worldMatrix);
          worldMatrix = tempMatrix;
        }
      }
    }
    else
    {
      localPosition = actorPosition + centerPosition;
      worldMatrix.SetTransformComponents(localScale, localOrientation, localPosition);
    }
  }

  return worldMatrix;
}

Vector4 TransitionBase::GetWorldColor(Dali::Actor actor)
{
  std::vector<Dali::Actor>     descentList;
  std::vector<Dali::ColorMode> inheritanceModeList;
  Dali::Actor                  currentActor = actor;
  Dali::ColorMode              inheritance  = Dali::ColorMode::USE_OWN_MULTIPLY_PARENT_ALPHA;
  do
  {
    inheritance = currentActor.GetProperty<Dali::ColorMode>(Dali::Actor::Property::COLOR_MODE);
    inheritanceModeList.push_back(inheritance);
    descentList.push_back(currentActor);
    currentActor = currentActor.GetParent();
  } while(inheritance != Dali::ColorMode::USE_OWN_COLOR && currentActor);

  Vector4 worldColor;
  for(unsigned int i(descentList.size() - 1); i < descentList.size(); --i)
  {
    if(inheritanceModeList[i] == USE_OWN_COLOR || i == descentList.size() - 1)
    {
      worldColor = descentList[i].GetProperty<Vector4>(Dali::Actor::Property::COLOR);
    }
    else if(inheritanceModeList[i] == USE_OWN_MULTIPLY_PARENT_ALPHA)
    {
      Vector4 ownColor = descentList[i].GetProperty<Vector4>(Dali::Actor::Property::COLOR);
      worldColor       = Vector4(ownColor.r, ownColor.g, ownColor.b, ownColor.a * worldColor.a);
    }
    else if(inheritanceModeList[i] == USE_OWN_MULTIPLY_PARENT_COLOR)
    {
      Vector4 ownColor = descentList[i].GetProperty<Vector4>(Dali::Actor::Property::COLOR);
      worldColor *= ownColor;
    }
  }

  return worldColor;
}

Vector3 TransitionBase::CalculateCenterPosition(
  const Vector3&    anchorPoint,
  const bool        positionUsesAnchorPoint,
  const Vector3&    size,
  const Vector3&    scale,
  const Quaternion& orientation)
{
  Vector3       centerPosition;
  const Vector3 half(0.5f, 0.5f, 0.5f);
  const Vector3 topLeft(0.0f, 0.0f, 0.5f);
  // Calculate the center-point by applying the scale and rotation on the anchor point.
  centerPosition = (half - anchorPoint) * size * scale;
  centerPosition *= orientation;

  // If the position is ignoring the anchor-point, then remove the anchor-point shift from the position.
  if(!positionUsesAnchorPoint)
  {
    centerPosition -= (topLeft - anchorPoint) * size;
  }
  return centerPosition;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
