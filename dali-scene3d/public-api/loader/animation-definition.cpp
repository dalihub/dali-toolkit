/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali-scene3d/public-api/loader/animation-definition.h>

namespace Dali::Scene3D::Loader
{
const float AnimationDefinition::DEFAULT_DURATION_SECONDS = 1.f;
const float AnimationDefinition::MIN_DURATION_SECONDS     = 1e-2f;

Animation::EndAction AnimationDefinition::StopForModification(Animation& anim)
{
  const auto endAction = anim.GetEndAction();
  anim.SetEndAction(Animation::DISCARD);
  anim.Stop();
  return endAction;
}

AnimationDefinition::AnimationDefinition() = default;

AnimationDefinition::AnimationDefinition(AnimationDefinition&& other)
: mName(std::move(other.mName)),
  mDuration(other.mDuration),
  mLoopCount(other.mLoopCount),
  mDisconnectAction(other.mDisconnectAction),
  mEndAction(other.mEndAction),
  mSpeedFactor(other.mSpeedFactor),
  mPlayRange(other.mPlayRange),
  mProperties(std::move(other.mProperties))
{
}

AnimationDefinition& AnimationDefinition::operator=(AnimationDefinition&& other)
{
  AnimationDefinition tmp(std::move(other));
  mName             = std::move(tmp.mName);
  mDuration         = tmp.mDuration;
  mLoopCount        = tmp.mLoopCount;
  mDisconnectAction = tmp.mDisconnectAction;
  mEndAction        = tmp.mEndAction;
  mSpeedFactor      = tmp.mSpeedFactor;
  mPlayRange        = tmp.mPlayRange;
  mProperties.swap(tmp.mProperties);
  return *this;
}

void AnimationDefinition::Animate(Animation& animation, AnimatedProperty::GetActor getActor)
{
  DALI_ASSERT_ALWAYS(animation);
  for(auto& property : mProperties)
  {
    property.Animate(animation, getActor);
  }
}

Animation AnimationDefinition::ReAnimate(AnimatedProperty::GetActor getActor)
{
  // Create and configure new animation.
  Animation animation = Animation::New(mDuration);
  animation.SetLoopCount(mLoopCount);
  animation.SetDisconnectAction(mDisconnectAction);
  animation.SetEndAction(mEndAction);

  animation.SetSpeedFactor(mSpeedFactor);
  animation.SetPlayRange(mPlayRange);

  Animate(animation, getActor);
  return animation;
}

void AnimationDefinition::SetName(const std::string& name)
{
  mName = name;
}

const std::string& AnimationDefinition::GetName() const
{
  return mName;
}

void AnimationDefinition::SetDuration(float duration)
{
  mDuration = duration;
}

float AnimationDefinition::GetDuration() const
{
  return mDuration;
}

void AnimationDefinition::SetLoopCount(int32_t loopCount)
{
  mLoopCount = loopCount;
}

int AnimationDefinition::GetLoopCount() const
{
  return mLoopCount;
}

void AnimationDefinition::SetDisconnectAction(Animation::EndAction disconnectAction)
{
  mDisconnectAction = disconnectAction;
}

Animation::EndAction AnimationDefinition::GetDisconnectAction() const
{
  return mDisconnectAction;
}

void AnimationDefinition::SetEndAction(Animation::EndAction endAction)
{
  mEndAction = endAction;
}

Animation::EndAction AnimationDefinition::GetEndAction() const
{
  return mEndAction;
}

void AnimationDefinition::SetSpeedFactor(float speedFactor)
{
  mSpeedFactor = speedFactor;
}

float AnimationDefinition::GetSpeedFactor() const
{
  return mSpeedFactor;
}

void AnimationDefinition::SetPlayRange(const Vector2& playRange)
{
  mPlayRange = playRange;
}

Vector2 AnimationDefinition::GetPlayRange() const
{
  return mPlayRange;
}

void AnimationDefinition::ReserveSize(uint32_t size)
{
  mProperties.reserve(size);
}

uint32_t AnimationDefinition::GetPropertyCount()
{
  return mProperties.size();
}

void AnimationDefinition::SetProperty(uint32_t index, AnimatedProperty&& property)
{
  if(mProperties.size() <= index)
  {
    mProperties.resize(index + 1);
  }
  mProperties[index] = std::move(property);
}

const AnimatedProperty& AnimationDefinition::GetPropertyAt(uint32_t index)
{
  return mProperties[index];
}

} // namespace Dali::Scene3D::Loader
