#ifndef DALI_SCENE3D_LOADER_ANIMATION_DEFINITION_H
#define DALI_SCENE3D_LOADER_ANIMATION_DEFINITION_H
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

// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/loader/animated-property.h>

namespace Dali::Scene3D::Loader
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

  AnimationDefinition& operator=(AnimationDefinition&& other);

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

  /**
   * @brief Set the name of the animation.
   *
   * @param[in] name The name of the animation.
   */
  void SetName(const std::string& name);

  /**
   * @brief Get the name of the animation.
   *
   * @return The name of the animation.
   */
  const std::string& GetName() const;

  /**
   * @brief Set the duration of the animation in seconds.
   *
   * @param[in] duration The duration of the animation in seconds.
   */
  void SetDuration(float duration);

  /**
   * @brief Get the duration of the animation in seconds.
   *
   * @return The duration of the animation in seconds.
   */
  float GetDuration() const;

  /**
   * @brief Set the number of times to loop the animation.
   *
   * @param[in] loopCount The number of times to loop the animation. Use -1 for infinite looping.
   */
  void SetLoopCount(int32_t loopCount);

  /**
   * @brief Get the number of times to loop the animation.
   *
   * @return The number of times to loop the animation. Use -1 for infinite looping.
   */
  int GetLoopCount() const;

  /**
   * @brief Set what should happen when an animation is disconnected from an object.
   *
   * @param[in] disconnectAction What should happen when an animation is disconnected from an object.
   */
  void SetDisconnectAction(Animation::EndAction disconnectAction);

  /**
   * @brief Get what should happen when an animation is disconnected from an object.
   *
   * @return What should happen when an animation is disconnected from an object.
   */
  Animation::EndAction GetDisconnectAction() const;

  /**
   * @brief Set what should happen when an animation reaches its end.
   *
   * @param[in] endAction What should happen when an animation reaches its end.
   */
  void SetEndAction(Animation::EndAction endAction);

  /**
   * @brief Get what should happen when an animation reaches its end.
   *
   * @return What should happen when an animation reaches its end.
   */
  Animation::EndAction GetEndAction() const;

  /**
   * @brief Set a speed factor for this animation. This can be used to speed up or slow down playback of this animation relative to other animations in a scene.
   *
   * @param[in] speedFactor The speed factor for this animation. 1.0 is normal speed, 2.0 is double speed, 0.5 is half speed, etc.
   */
  void SetSpeedFactor(float speedFactor);

  /**
   * @brief Get a speed factor for this animation. This can be used to speed up or slow down playback of this animation relative to other animations in a scene.
   *
   * @return The speed factor for this animation. 1.0 is normal speed, 2.0 is double speed, 0.5 is half speed, etc.
   */
  float GetSpeedFactor() const;

  /**
   * @brief Set a range within which to play this animation. This can be used to play only part of an animation or to play it backwards by setting playRange.y < playRange.x
   *
   * @param[in] playRange A range within which to play this animation. x = start time in seconds, y = end time in seconds
   */
  void SetPlayRange(const Vector2& playRange);

  /**
   * @brief Get a range within which to play this animation. This can be used to play only part of an animation or to play it backwards by setting playRange.y < playRange.x
   *
   * @return A range within which to play this animation. x = start time in seconds, y = end time in seconds
   */
  Vector2 GetPlayRange() const;

  /**
   * @brief Reserves Animated property vector's size
   *
   * @param[in] size The size to reserve property
   */
  void ReserveSize(uint32_t size);

  /**
   * @brief Retrieves the number of animated properties' count
   *
   * @return The count of animated properties.
   */
  uint32_t GetPropertyCount();

  /**
   * @brief Add a property that will be animated by this AnimationDefinition
   *
   * @param[in] index The index the property will be stored.
   * @param[in] property The property that will be animated by this AnimationDefinition
   */
  void SetProperty(uint32_t index, AnimatedProperty&& property);

  /**
   * @brief Retrieves animated property at the index
   *
   * @param[in] index The index of property to be retrieved.
   */
  AnimatedProperty& GetPropertyAt(uint32_t index);

private: // DATA
  std::string mName;

  float                mDuration         = DEFAULT_DURATION_SECONDS;
  int32_t              mLoopCount        = 1;
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

} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_ANIMATION_DEFINITION_H
