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

  auto getActor = [&actor](const Dali::Scene3D::Loader::AnimatedProperty& property) {
    return actor.FindChildByName(property.mNodeName);
  };

  uint32_t animatedPropertyIndex = 0;
  for (bool b: { false, true })
  {
    AnimationDefinition animDef;
    animDef.SetName("WalkRight");
    animDef.SetDuration(10.f);
    animDef.SetLoopCount(2);
    animDef.SetEndAction(Animation::BAKE_FINAL);
    animDef.SetSpeedFactor(.7f);
    animDef.SetProperty(animatedPropertyIndex++,std::move(AnimatedProperty{
     INVALID_INDEX,
     "ChristopherPlummer",
     "position",
     KeyFrames(),
     std::unique_ptr<AnimatedProperty::Value>{ new AnimatedProperty::Value{
       Property::Value{ Vector3::XAXIS * 100.f },
       b
     } },
     AlphaFunction::EASE_OUT,
     TimePeriod(animDef.GetDuration())
    }));

    auto anim = animDef.ReAnimate(getActor);
    DALI_TEST_EQUAL(anim.GetDuration(), animDef.GetDuration());
    DALI_TEST_EQUAL(anim.GetEndAction(), animDef.GetEndAction());
    DALI_TEST_EQUAL(anim.GetSpeedFactor(), animDef.GetSpeedFactor());
    DALI_TEST_EQUAL(anim.GetLoopCount(), animDef.GetLoopCount());
  }

  END_TEST;
}

int UtcDaliAnimationDefinitionReAnimateKeyFrames(void)
{
  TestApplication app;
  auto actor = Actor::New();
  actor.SetProperty(Actor::Property::NAME, "ChristopherPlummer");

  auto getActor = [&actor](const Dali::Scene3D::Loader::AnimatedProperty& property) {
    return actor.FindChildByName(property.mNodeName);
  };

  KeyFrames kf = KeyFrames::New();
  kf.Add(0.f, Vector3::ZERO);
  kf.Add(1.f, Vector3::XAXIS * 100.f);

  uint32_t animatedPropertyIndex = 0;
  AnimationDefinition animDef;
  animDef.SetName("WalkRight");
  animDef.SetDuration(10.f);
  animDef.SetLoopCount(2);
  animDef.SetEndAction(Animation::BAKE_FINAL);
  animDef.SetSpeedFactor(.7f);
  animDef.SetProperty(animatedPropertyIndex++, std::move(AnimatedProperty{
   INVALID_INDEX,
   "ChristopherPlummer",
   "position",
   kf,
   nullptr,
   AlphaFunction::EASE_OUT,
   TimePeriod(animDef.GetDuration())
  }));

  auto anim = animDef.ReAnimate(getActor);
  DALI_TEST_EQUAL(anim.GetDuration(), animDef.GetDuration());
  DALI_TEST_EQUAL(anim.GetEndAction(), animDef.GetEndAction());
  DALI_TEST_EQUAL(anim.GetSpeedFactor(), animDef.GetSpeedFactor());
  DALI_TEST_EQUAL(anim.GetLoopCount(), animDef.GetLoopCount());

  END_TEST;
}
