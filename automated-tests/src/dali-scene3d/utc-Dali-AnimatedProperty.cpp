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

#include "dali-scene3d/public-api/loader/animated-property.h"
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

int UtcDaliAnimatedPropertyGetPropertyType(void)
{
  TestApplication app;
  auto actor = Actor::New();
  actor.SetProperty(Actor::Property::NAME, "ChristopherPlummer");

  AnimatedProperty animProp {
   "ChristopherPlummer",
   "position",
   KeyFrames(),
   std::unique_ptr<AnimatedProperty::Value>{ new AnimatedProperty::Value{
     Property::Value{ Vector3::XAXIS * 100.f },
     true
   } },
  };

  Property expected(actor, Actor::Property::POSITION);
  Property result = animProp.GetProperty(actor);
  DALI_TEST_EQUAL(result.object, expected.object);
  DALI_TEST_EQUAL(result.propertyIndex, expected.propertyIndex);
  DALI_TEST_EQUAL(result.componentIndex, expected.componentIndex);
  DALI_TEST_EQUAL(animProp.GetPropertyType(actor), Property::VECTOR3);

  END_TEST;
}








