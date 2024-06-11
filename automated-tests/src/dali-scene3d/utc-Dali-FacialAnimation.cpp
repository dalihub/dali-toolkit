/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

#include <dali-scene3d/public-api/loader/animation-definition.h>
#include <dali-scene3d/public-api/loader/facial-animation-loader.h>
#include <dali-test-suite-utils.h>

#include <fstream>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

namespace
{
std::string ReadBufferFromFile(const std::string& url)
{
  std::string  rawString;
  std::fstream fileStream;

  fileStream.open(url, std::ios::in | std::ios::binary);
  if(!fileStream.is_open())
  {
    DALI_LOG_WARNING("stream open failed for: \"%s\", in mode: \"%d\".\n", url.c_str(), static_cast<int>(std::ios::in | std::ios::binary));
  }

  // get length of file:
  fileStream.seekg(0, std::ios::end);
  auto length = fileStream.tellg();
  fileStream.seekg(0, std::ios::beg);

  rawString.resize(length);
  fileStream.read(rawString.data(), length);

  fileStream.close();

  return rawString;
}
} // namespace

int UtcDaliLoadFacialAnimation(void)
{
  TestApplication app;

  for(uint32_t tc = 0; tc < 2; ++tc)
  {
    tet_printf("UtcDaliLoadFacialAnimation testcase %u\n", tc);
    AnimationDefinition animDef;
    switch(tc)
    {
      case 0: // Load bvh from url
      {
        animDef = LoadFacialAnimation(TEST_RESOURCE_DIR "/facial-blendshape-animation.json");
        break;
      }
      case 1: // Load bvh from buffer stream.
      {
        std::string rawString = ReadBufferFromFile(TEST_RESOURCE_DIR "/facial-blendshape-animation.json");
        animDef               = LoadFacialAnimationFromBuffer(reinterpret_cast<uint8_t*>(rawString.data()), static_cast<int>(rawString.length()));
        break;
      }
    }

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
  }

  END_TEST;
}

int UtcDaliLoadFacialAnimationFailed01(void)
{
  TestApplication application;

  AnimationDefinition animDef = LoadFacialAnimation("/nothing.json");
  DALI_TEST_EQUALS(0u, animDef.GetPropertyCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliLoadFacialAnimationFailed02(void)
{
  TestApplication application;

  AnimationDefinition animDef = LoadFacialAnimationFromBuffer(nullptr, 0);
  DALI_TEST_EQUALS(0u, animDef.GetPropertyCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliLoadFacialAnimationFailed03(void)
{
  TestApplication application;

  tet_infoline("Parse error for invalid json");
  uint32_t caseCount = 6;
  for(uint32_t tc = 0; tc < caseCount; ++tc)
  {
    tet_printf("Parse error for json %u\n", tc);
    std::ostringstream oss;
    oss << TEST_RESOURCE_DIR << "/test-invalid-facial-animation" << tc << ".json";
    AnimationDefinition animDef = LoadFacialAnimation(oss.str());
    DALI_TEST_EQUALS(0u, animDef.GetPropertyCount(), TEST_LOCATION);
  }
  END_TEST;
}