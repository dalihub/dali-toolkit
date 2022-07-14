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

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include "dali-scene3d/public-api/loader/animation-definition.h"
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

int UtcDaliAnimationDefinitionStopForModification(void)
{
  TestApplication app;
  auto anim = Animation::New(15.f);
  anim.Play();

  auto oldEndAction = AnimationDefinition::StopForModification(anim);
  DALI_TEST_EQUAL(anim.GetState(), Animation::STOPPED);
  DALI_TEST_EQUAL(oldEndAction, Animation::BAKE);
  DALI_TEST_EQUAL(anim.GetEndAction(), Animation::DISCARD);

  END_TEST;
}

int UtcDaliAnimationDefinitionReAnimate(void)
{
  TestApplication app;
  auto actor = Actor::New();
  actor.SetProperty(Actor::Property::NAME, "ChristopherPlummer");

  auto getActor = [&actor](const std::string& name) {
    return actor.FindChildByName(name);
  };

  for (bool b: { false, true })
  {
    AnimationDefinition animDef;
    animDef.mName = "WalkRight";
    animDef.mDuration = 10.f;
    animDef.mLoopCount = 2;
    animDef.mEndAction = Animation::BAKE_FINAL;
    animDef.mSpeedFactor = .7f;
    animDef.mProperties.push_back(AnimatedProperty{
     "ChristopherPlummer",
     "position",
     KeyFrames(),
     std::unique_ptr<AnimatedProperty::Value>{ new AnimatedProperty::Value{
       Property::Value{ Vector3::XAXIS * 100.f },
       b
     } },
     AlphaFunction::EASE_OUT,
     TimePeriod(animDef.mDuration)
    });

    auto anim = animDef.ReAnimate(getActor);
    DALI_TEST_EQUAL(anim.GetDuration(), animDef.mDuration);
    DALI_TEST_EQUAL(anim.GetEndAction(), animDef.mEndAction);
    DALI_TEST_EQUAL(anim.GetSpeedFactor(), animDef.mSpeedFactor);
    DALI_TEST_EQUAL(anim.GetLoopCount(), animDef.mLoopCount);
  }

  END_TEST;
}

int UtcDaliAnimationDefinitionReAnimateKeyFrames(void)
{
  TestApplication app;
  auto actor = Actor::New();
  actor.SetProperty(Actor::Property::NAME, "ChristopherPlummer");

  auto getActor = [&actor](const std::string& name) {
    return actor.FindChildByName(name);
  };

  KeyFrames kf = KeyFrames::New();
  kf.Add(0.f, Vector3::ZERO);
  kf.Add(1.f, Vector3::XAXIS * 100.f);

  AnimationDefinition animDef;
  animDef.mName = "WalkRight";
  animDef.mDuration = 10.f;
  animDef.mLoopCount = 2;
  animDef.mEndAction = Animation::BAKE_FINAL;
  animDef.mSpeedFactor = .7f;
  animDef.mProperties.push_back(AnimatedProperty{
   "ChristopherPlummer",
   "position",
   kf,
   nullptr,
   AlphaFunction::EASE_OUT,
   TimePeriod(animDef.mDuration)
  });

  auto anim = animDef.ReAnimate(getActor);
  DALI_TEST_EQUAL(anim.GetDuration(), animDef.mDuration);
  DALI_TEST_EQUAL(anim.GetEndAction(), animDef.mEndAction);
  DALI_TEST_EQUAL(anim.GetSpeedFactor(), animDef.mSpeedFactor);
  DALI_TEST_EQUAL(anim.GetLoopCount(), animDef.mLoopCount);

  END_TEST;
}
