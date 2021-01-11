#ifndef DALI_SCENE_LOADER_ANIMATED_PROPERTY_H
#define DALI_SCENE_LOADER_ANIMATED_PROPERTY_H
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include "dali-scene-loader/public-api/api.h"

// EXTERNAL INCLUDES
#include "dali/public-api/actors/actor.h"
#include "dali/public-api/animation/animation.h"
#include "dali/public-api/object/property.h"
#include <memory>
#include <functional>

namespace Dali
{
namespace SceneLoader
{

/**
 * @brief Intermediate representation for a property that's given to
 *  a Animation to animate. Since there is no getting an
 *  animated property back from the Animation (i.e. past AnimateBetween/By/To()),
 *  changing properties (e.g. from the SDK) requires the whole Animation
 *  object to be recreated with all of its properties (incl. modifications).
 */
struct DALI_SCENE_LOADER_API AnimatedProperty
{
public: // METHODS
  /**
   * @brief Function to obtain an Actor based on its name. Its processing will
   *  ignore empty handles returned by it.
   */
  using GetActor = std::function<Actor(const std::string&)>;

  /**
   * @return The Property object (of the given @a actor) whose value is being animated.
   */
  Property GetProperty(Actor& actor)
  {
    auto idx = actor.GetPropertyIndex(mPropertyName);
    return Property(actor, idx);
  }

  /**
   * @brief The type of the Property (of the given @a actor) that is being animated.
   */
  Property::Type GetPropertyType(Actor& actor)
  {
    auto idx = actor.GetPropertyIndex(mPropertyName);
    return actor.GetPropertyType(idx);
  }

  /**
   * @brief Registers the animation of this property against the given @a anim.
   *  @a getActor will be used to obtain the Actor named by this property.
   *  Failing to find the actor purely means that this property will not be
   *  animated.
   */
  void Animate(Animation& anim, GetActor getActor);

public: // DATA
  struct Value
  {
    Property::Value mValue;
    bool mIsRelative;
  };

  std::string mNodeName;
  std::string mPropertyName;

  KeyFrames mKeyFrames;
  std::unique_ptr<Value> mValue;

  AlphaFunction mAlphaFunction = AlphaFunction::DEFAULT;
  TimePeriod mTimePeriod = TimePeriod(0.f);
};

}
}

#endif //DALI_SCENE_LOADER_ANIMATED_PROPERTY_H
