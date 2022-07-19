#ifndef DALI_SCENE3D_LOADER_ANIMATION_DEFINITION_H
#define DALI_SCENE3D_LOADER_ANIMATION_DEFINITION_H
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

#include "dali-scene3d/public-api/api.h"
#include "dali-scene3d/public-api/loader/animated-property.h"
#include "dali/public-api/common/vector-wrapper.h"

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
/**
 * @brief Animation handle + name + definition of properties.
 */
class DALI_SCENE3D_API AnimationDefinition
{
public: // STATIC
  // For Animations created in the SDK.
  static const float DEFAULT_DURATION_SECONDS;

  // For parsing Animations from dli, when duration was not defined.
  static const float MIN_DURATION_SECONDS;

  /**
   * @brief Saves the original end action of @a anim, sets the end action to
   *  Discard, then stops the animation and returns the end action.
   */
  static Animation::EndAction StopForModification(Animation& anim);

public: // METHODS
  AnimationDefinition();

  AnimationDefinition(AnimationDefinition&& other);

  /**
   * @brief Registers the properties against the given @a animation. @a getActor
   *  will be used to obtain the Actors for each AnimatedProperty.
   */
  void Animate(Animation& animation, AnimatedProperty::GetActor getActor);

  /**
   * @brief Creates a new Animation and Animates() its properties. @a getActor
   *  will be used to obtain the Actors for each AnimatedProperty.
   */
  Animation ReAnimate(AnimatedProperty::GetActor getActor);

  AnimationDefinition& operator=(AnimationDefinition&& other);

public: // DATA
  std::string mName;

  float                mDuration         = DEFAULT_DURATION_SECONDS;
  int                  mLoopCount        = 1;
  Animation::EndAction mDisconnectAction = Animation::BAKE_FINAL;
  Animation::EndAction mEndAction        = Animation::BAKE;
  float                mSpeedFactor      = 1.f;
  Vector2              mPlayRange        = Vector2{0.f, 1.f};

  std::vector<AnimatedProperty> mProperties;
};

struct AnimationGroupDefinition
{
  std::string              mName;
  std::vector<std::string> mAnimations;
};

} // namespace Loader
} // namespace Scene3D
} // namespace Dali

#endif //DALI_SCENE3D_LOADER_ANIMATION_DEFINITION_H
