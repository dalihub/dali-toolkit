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

#include <dali-scene3d/public-api/loader/bvh-loader.h>
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

int UtcDaliLoadBvh(void)
{
  TestApplication application;

  for(uint32_t tc = 0; tc < 2; ++tc)
  {
    AnimationDefinition animDef;
    tet_printf("UtcDaliLoadBvh testcase %u\n", tc);
    switch(tc)
    {
      case 0: // Load bvh from url
      {
        animDef = LoadBvh(TEST_RESOURCE_DIR "/test.bvh", "testBvh");
        break;
      }
      case 1: // Load bvh from buffer stream.
      {
        std::string rawString = ReadBufferFromFile(TEST_RESOURCE_DIR "/test.bvh");
        animDef               = LoadBvhFromBuffer(reinterpret_cast<uint8_t*>(rawString.data()), static_cast<int>(rawString.length()), "testBvh");
        break;
      }
    }

    DALI_TEST_EQUAL(animDef.GetName(), "testBvh");
    DALI_TEST_EQUAL(animDef.GetDuration(), 0.3f);

    DALI_TEST_EQUAL(animDef.GetPropertyAt(0).mNodeName, "root");
    DALI_TEST_EQUAL(animDef.GetPropertyAt(0).mPropertyName, "position");
    DALI_TEST_EQUAL(animDef.GetPropertyAt(0).mKeyFrames.GetType(), Property::Type::VECTOR3);
    DALI_TEST_EQUAL(animDef.GetPropertyAt(0).mTimePeriod.durationSeconds, 0.3f);

    DALI_TEST_EQUAL(animDef.GetPropertyAt(1).mNodeName, "root");
    DALI_TEST_EQUAL(animDef.GetPropertyAt(1).mPropertyName, "orientation");
    DALI_TEST_EQUAL(animDef.GetPropertyAt(1).mKeyFrames.GetType(), Property::Type::ROTATION);
    DALI_TEST_EQUAL(animDef.GetPropertyAt(1).mTimePeriod.durationSeconds, 0.3f);

    DALI_TEST_EQUAL(animDef.GetPropertyAt(2).mNodeName, "first");
    DALI_TEST_EQUAL(animDef.GetPropertyAt(2).mPropertyName, "position");
    DALI_TEST_EQUAL(animDef.GetPropertyAt(2).mKeyFrames.GetType(), Property::Type::VECTOR3);
    DALI_TEST_EQUAL(animDef.GetPropertyAt(2).mTimePeriod.durationSeconds, 0.3f);

    DALI_TEST_EQUAL(animDef.GetPropertyAt(3).mNodeName, "first");
    DALI_TEST_EQUAL(animDef.GetPropertyAt(3).mPropertyName, "orientation");
    DALI_TEST_EQUAL(animDef.GetPropertyAt(3).mKeyFrames.GetType(), Property::Type::ROTATION);
    DALI_TEST_EQUAL(animDef.GetPropertyAt(3).mTimePeriod.durationSeconds, 0.3f);

    Actor root = Actor::New();
    root.SetProperty(Actor::Property::NAME, "root");

    Actor first = Actor::New();
    first.SetProperty(Actor::Property::NAME, "first");
    root.Add(first);

    auto getActor = [&root](const Dali::Scene3D::Loader::AnimatedProperty& property) {
      return root.FindChildByName(property.mNodeName);
    };

    Animation animation = animDef.ReAnimate(getActor);
    DALI_TEST_EQUAL(animation.GetDuration(), animDef.GetDuration());

    application.GetScene().Add(root);

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS(Vector2(0, 0), root.GetProperty<Vector2>(Actor::Property::POSITION), TEST_LOCATION);
    DALI_TEST_EQUALS(Vector2(0, 0), first.GetProperty<Vector2>(Actor::Property::POSITION), TEST_LOCATION);
    Vector3 rootWorldPositionBefore  = root.GetProperty<Vector3>(Actor::Property::WORLD_POSITION);
    Vector3 firstWorldPositionBefore = first.GetProperty<Vector3>(Actor::Property::WORLD_POSITION);

    animation.Play();

    application.SendNotification();
    application.Render(1000);

    DALI_TEST_EQUALS(Vector2(0, 10), root.GetProperty<Vector2>(Actor::Property::POSITION), TEST_LOCATION);
    DALI_TEST_EQUALS(Vector2(10, 0), first.GetProperty<Vector2>(Actor::Property::POSITION), TEST_LOCATION);

    Vector3 rootWorldPositionAfter  = root.GetProperty<Vector3>(Actor::Property::WORLD_POSITION);
    Vector3 firstWorldPositionAfter = first.GetProperty<Vector3>(Actor::Property::WORLD_POSITION);

    DALI_TEST_EQUALS(Vector3(0, 10, 0), rootWorldPositionAfter - rootWorldPositionBefore, TEST_LOCATION);
    DALI_TEST_EQUALS(Vector3(10, 10, 0), firstWorldPositionAfter - firstWorldPositionBefore, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliLoadBvhFailed01(void)
{
  TestApplication application;

  AnimationDefinition animDef = LoadBvh("/nothing.bvh", "testBvh");
  DALI_TEST_EQUALS(0u, animDef.GetPropertyCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliLoadBvhFailed02(void)
{
  TestApplication application;

  AnimationDefinition animDef = LoadBvhFromBuffer(nullptr, 0, "testBvh");
  DALI_TEST_EQUALS(0u, animDef.GetPropertyCount(), TEST_LOCATION);
  END_TEST;
}