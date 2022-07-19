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
#include <dali-scene3d/public-api/loader/bvh-loader.h>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

int UtcDaliLoadBvh(void)
{
  TestApplication application;

  AnimationDefinition animDef = LoadBvh(TEST_RESOURCE_DIR "/test.bvh", "testBvh");

  DALI_TEST_EQUAL(animDef.mName, "testBvh");
  DALI_TEST_EQUAL(animDef.mDuration, 0.3f);

  DALI_TEST_EQUAL(animDef.mProperties[0].mNodeName, "root");
  DALI_TEST_EQUAL(animDef.mProperties[0].mPropertyName, "position");
  DALI_TEST_EQUAL(animDef.mProperties[0].mKeyFrames.GetType(), Property::Type::VECTOR3);
  DALI_TEST_EQUAL(animDef.mProperties[0].mTimePeriod.durationSeconds, 0.3f);

  DALI_TEST_EQUAL(animDef.mProperties[1].mNodeName, "root");
  DALI_TEST_EQUAL(animDef.mProperties[1].mPropertyName, "orientation");
  DALI_TEST_EQUAL(animDef.mProperties[1].mKeyFrames.GetType(), Property::Type::ROTATION);
  DALI_TEST_EQUAL(animDef.mProperties[1].mTimePeriod.durationSeconds, 0.3f);

  DALI_TEST_EQUAL(animDef.mProperties[2].mNodeName, "first");
  DALI_TEST_EQUAL(animDef.mProperties[2].mPropertyName, "position");
  DALI_TEST_EQUAL(animDef.mProperties[2].mKeyFrames.GetType(), Property::Type::VECTOR3);
  DALI_TEST_EQUAL(animDef.mProperties[2].mTimePeriod.durationSeconds, 0.3f);

  DALI_TEST_EQUAL(animDef.mProperties[3].mNodeName, "first");
  DALI_TEST_EQUAL(animDef.mProperties[3].mPropertyName, "orientation");
  DALI_TEST_EQUAL(animDef.mProperties[3].mKeyFrames.GetType(), Property::Type::ROTATION);
  DALI_TEST_EQUAL(animDef.mProperties[3].mTimePeriod.durationSeconds, 0.3f);

  Actor root = Actor::New();
  root.SetProperty(Actor::Property::NAME, "root");

  Actor first = Actor::New();
  first.SetProperty(Actor::Property::NAME, "first");
  root.Add(first);

  auto getActor = [&root](const std::string& name) {
    return root.FindChildByName(name);
  };

  Animation animation = animDef.ReAnimate(getActor);
  DALI_TEST_EQUAL(animation.GetDuration(), animDef.mDuration);

  application.GetScene().Add(root);

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(Vector2(0, 0), root.GetProperty<Vector2>(Actor::Property::POSITION), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(0, 0), first.GetProperty<Vector2>(Actor::Property::POSITION), TEST_LOCATION);
  Vector3 rootWorldPositionBefore = root.GetProperty<Vector3>(Actor::Property::WORLD_POSITION);
  Vector3 firstWorldPositionBefore = first.GetProperty<Vector3>(Actor::Property::WORLD_POSITION);

  animation.Play();

  application.SendNotification();
  application.Render(1000);

  DALI_TEST_EQUALS(Vector2(0, 10), root.GetProperty<Vector2>(Actor::Property::POSITION), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(10, 0), first.GetProperty<Vector2>(Actor::Property::POSITION), TEST_LOCATION);

  Vector3 rootWorldPositionAfter = root.GetProperty<Vector3>(Actor::Property::WORLD_POSITION);
  Vector3 firstWorldPositionAfter = first.GetProperty<Vector3>(Actor::Property::WORLD_POSITION);

  DALI_TEST_EQUALS(Vector3(0, 10, 0), rootWorldPositionAfter - rootWorldPositionBefore, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector3(10, 10, 0), firstWorldPositionAfter - firstWorldPositionBefore, TEST_LOCATION);

  END_TEST;
}



int UtcDaliLoadBvhFailed(void)
{
  TestApplication application;

  AnimationDefinition animDef = LoadBvh("/nothing.bvh", "testBvh");
  DALI_TEST_EQUALS(0u, animDef.mProperties.size(), TEST_LOCATION);
  END_TEST;
}
