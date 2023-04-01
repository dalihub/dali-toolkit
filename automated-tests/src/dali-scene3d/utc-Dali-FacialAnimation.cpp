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

#include <dali-test-suite-utils.h>
#include <dali-scene3d/public-api/loader/animation-definition.h>
#include <dali-scene3d/public-api/loader/facial-animation-loader.h>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

int UtcDaliLoadFacialAnimation(void)
{
  TestApplication app;

  AnimationDefinition animDef = LoadFacialAnimation(TEST_RESOURCE_DIR "/facial-blendshape-animation.json");

  std::string name = animDef.GetName();
  DALI_TEST_EQUAL(name, "Facial_Blendshape_Animation");
  DALI_TEST_EQUAL(animDef.GetDuration(), 14.966001f);
  DALI_TEST_EQUAL(animDef.GetEndAction(), Animation::BAKE);
  DALI_TEST_EQUAL(animDef.GetSpeedFactor(), 1.0f);
  DALI_TEST_EQUAL(animDef.GetLoopCount(), 1);
  DALI_TEST_EQUAL(animDef.GetPropertyCount(), 122);

  DALI_TEST_EQUAL(animDef.GetPropertyAt(0).mNodeName, "GEO_1");
  DALI_TEST_EQUAL(animDef.GetPropertyAt(0).mPropertyName, "uBlendShapeWeight[0]");
  DALI_TEST_EQUAL(animDef.GetPropertyAt(0).mKeyFrames.GetType(), Property::Type::FLOAT);
  DALI_TEST_EQUAL(animDef.GetPropertyAt(0).mTimePeriod.delaySeconds, 0.0f);
  DALI_TEST_EQUAL(animDef.GetPropertyAt(0).mTimePeriod.durationSeconds, 14.966001f);

  DALI_TEST_EQUAL(animDef.GetPropertyAt(69).mNodeName, "GEO_2");
  DALI_TEST_EQUAL(animDef.GetPropertyAt(69).mPropertyName, "uBlendShapeWeight[1]");
  DALI_TEST_EQUAL(animDef.GetPropertyAt(69).mKeyFrames.GetType(), Property::Type::FLOAT);
  DALI_TEST_EQUAL(animDef.GetPropertyAt(69).mTimePeriod.delaySeconds, 0.0f);
  DALI_TEST_EQUAL(animDef.GetPropertyAt(69).mTimePeriod.durationSeconds, 14.966001f);

  DALI_TEST_EQUAL(animDef.GetPropertyAt(86).mNodeName, "GEO_3");
  DALI_TEST_EQUAL(animDef.GetPropertyAt(86).mPropertyName, "uBlendShapeWeight[2]");
  DALI_TEST_EQUAL(animDef.GetPropertyAt(86).mKeyFrames.GetType(), Property::Type::FLOAT);
  DALI_TEST_EQUAL(animDef.GetPropertyAt(86).mTimePeriod.delaySeconds, 0.0f);
  DALI_TEST_EQUAL(animDef.GetPropertyAt(86).mTimePeriod.durationSeconds, 14.966001f);

  DALI_TEST_EQUAL(animDef.GetPropertyAt(100).mNodeName, "GEO_4");
  DALI_TEST_EQUAL(animDef.GetPropertyAt(100).mPropertyName, "uBlendShapeWeight[7]");
  DALI_TEST_EQUAL(animDef.GetPropertyAt(100).mKeyFrames.GetType(), Property::Type::FLOAT);
  DALI_TEST_EQUAL(animDef.GetPropertyAt(100).mTimePeriod.delaySeconds, 0.0f);
  DALI_TEST_EQUAL(animDef.GetPropertyAt(100).mTimePeriod.durationSeconds, 14.966001f);

  DALI_TEST_EQUAL(animDef.GetPropertyAt(121).mNodeName, "GEO_5");
  DALI_TEST_EQUAL(animDef.GetPropertyAt(121).mPropertyName, "uBlendShapeWeight[19]");
  DALI_TEST_EQUAL(animDef.GetPropertyAt(121).mKeyFrames.GetType(), Property::Type::FLOAT);
  DALI_TEST_EQUAL(animDef.GetPropertyAt(121).mTimePeriod.delaySeconds, 0.0f);
  DALI_TEST_EQUAL(animDef.GetPropertyAt(121).mTimePeriod.durationSeconds, 14.966001f);

  auto actor = Actor::New();
  actor.SetProperty(Actor::Property::NAME, "GEO_1");

  char        weightNameBuffer[32];
  char* const pWeightName = weightNameBuffer + snprintf(weightNameBuffer, sizeof(weightNameBuffer), "%s", "uBlendShapeWeight");
  for(int i = 0; i < 122; i++)
  {
    snprintf(pWeightName, sizeof(weightNameBuffer) - (pWeightName - weightNameBuffer), "[%d]", i);
    std::string weightName{weightNameBuffer};
    actor.RegisterProperty(weightName, 0.0f);
  }

  auto getActor = [&actor](const Dali::Scene3D::Loader::AnimatedProperty& property) {
    return actor.FindChildByName(property.mNodeName);
  };

  auto anim = animDef.ReAnimate(getActor);
  DALI_TEST_EQUAL(anim.GetDuration(), animDef.GetDuration());
  DALI_TEST_EQUAL(anim.GetEndAction(), animDef.GetEndAction());
  DALI_TEST_EQUAL(anim.GetSpeedFactor(), animDef.GetSpeedFactor());
  DALI_TEST_EQUAL(anim.GetLoopCount(), animDef.GetLoopCount());

  END_TEST;
}
