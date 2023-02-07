/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include "dali-scene3d/public-api/loader/animation-definition.h"

namespace Dali
{
namespace Scene3D
{
namespace Loader
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

} // namespace Loader
} // namespace Scene3D
} // namespace Dali
