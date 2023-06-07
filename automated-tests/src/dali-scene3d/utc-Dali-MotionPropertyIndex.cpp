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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <stdlib.h>
#include <iostream>

#include <dali-scene3d/public-api/model-motion/motion-index/motion-index.h>
#include <dali-scene3d/public-api/model-motion/motion-index/motion-property-index.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Scene3D;

void model_motion_motion_property_index_startup(void)
{
  test_return_value = TET_UNDEF;
}

void model_motion_motion_property_index_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
} // namespace

// Positive test case for a method
int UtcDaliMotionPropertyIndexNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliMotionPropertyIndexNew");

  MotionPropertyIndex motionPropertyIndex = MotionPropertyIndex::New();
  DALI_TEST_CHECK(motionPropertyIndex);
  DALI_TEST_EQUALS(motionPropertyIndex.GetModelNodeId().stringKey, "", TEST_LOCATION);
  DALI_TEST_EQUALS(motionPropertyIndex.GetPropertyId().indexKey, Property::INVALID_INDEX, TEST_LOCATION);

  motionPropertyIndex = MotionPropertyIndex::New("dummy", 3u);
  DALI_TEST_CHECK(motionPropertyIndex);
  DALI_TEST_EQUALS(motionPropertyIndex.GetModelNodeId().stringKey, "dummy", TEST_LOCATION);
  DALI_TEST_EQUALS(motionPropertyIndex.GetPropertyId().indexKey, 3u, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliMotionPropertyIndexDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliMotionPropertyIndexDownCast");

  MotionPropertyIndex motionPropertyIndex = MotionPropertyIndex::New();
  BaseHandle          handle(motionPropertyIndex);

  MotionPropertyIndex motionPropertyIndex2 = MotionPropertyIndex::DownCast(handle);
  DALI_TEST_CHECK(motionPropertyIndex);
  DALI_TEST_CHECK(motionPropertyIndex2);
  DALI_TEST_CHECK(motionPropertyIndex2 == motionPropertyIndex);
  END_TEST;
}

int UtcDaliMotionPropertyIndexTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK(typeRegistry);

  TypeInfo typeInfo = typeRegistry.GetTypeInfo("MotionPropertyIndex");
  DALI_TEST_CHECK(typeInfo);

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK(handle);

  MotionPropertyIndex motionPropertyIndex = MotionPropertyIndex::DownCast(handle);
  DALI_TEST_CHECK(motionPropertyIndex);

  END_TEST;
}

int UtcDaliMotionPropertyIndexCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  MotionPropertyIndex motionPropertyIndex = MotionPropertyIndex::New();
  DALI_TEST_CHECK(motionPropertyIndex);

  MotionPropertyIndex copy(motionPropertyIndex);
  DALI_TEST_CHECK(motionPropertyIndex == copy);

  MotionPropertyIndex assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == motionPropertyIndex);

  END_TEST;
}

int UtcDaliMotionPropertyIndexMoveConstructor(void)
{
  ToolkitTestApplication application;

  MotionPropertyIndex motionPropertyIndex = MotionPropertyIndex::New();
  DALI_TEST_EQUALS(1, motionPropertyIndex.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  MotionPropertyIndex moved = std::move(motionPropertyIndex);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!motionPropertyIndex);

  END_TEST;
}

int UtcDaliMotionPropertyIndexMoveAssignment(void)
{
  ToolkitTestApplication application;

  MotionPropertyIndex motionPropertyIndex = MotionPropertyIndex::New();
  DALI_TEST_EQUALS(1, motionPropertyIndex.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  MotionPropertyIndex moved;
  moved = std::move(motionPropertyIndex);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

// Method test

int UtcDaliMotionPropertyIndexGetPropertyName(void)
{
  std::string     expectPropertyName  = "";
  Property::Index expectPropertyIndex = Property::INVALID_INDEX;

  MotionIndex index = MotionPropertyIndex::New();

  // Invalid motion index return empty string, invalid index
  DALI_TEST_EQUALS(expectPropertyName, index.GetPropertyName(), TEST_LOCATION);
  DALI_TEST_EQUALS(expectPropertyIndex, index.GetPropertyIndex(), TEST_LOCATION);

  expectPropertyIndex = 0u;
  index               = MotionPropertyIndex::New("", expectPropertyIndex);
  DALI_TEST_EQUALS(expectPropertyIndex, index.GetPropertyIndex(), TEST_LOCATION);

  expectPropertyIndex = 45u;
  index               = MotionPropertyIndex::New("", expectPropertyIndex);
  DALI_TEST_EQUALS(expectPropertyIndex, index.GetPropertyIndex(), TEST_LOCATION);

  expectPropertyName = "name";
  index              = MotionPropertyIndex::New("", expectPropertyName);
  DALI_TEST_EQUALS(expectPropertyName, index.GetPropertyName(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliMotionPropertyIndexSetGetPropertyId(void)
{
  Property::Index expectPropertyIndex = 0u;

  MotionPropertyIndex index = MotionPropertyIndex::New("", expectPropertyIndex);

  // Invalid motion index return empty string, invalid index
  DALI_TEST_CHECK(index.GetPropertyId().type == Property::Key::Type::INDEX);
  DALI_TEST_EQUALS(expectPropertyIndex, index.GetPropertyId().indexKey, TEST_LOCATION);

  std::string expectPropertyName = "hello";
  index.SetPropertyId(expectPropertyName);
  DALI_TEST_CHECK(index.GetPropertyId().type == Property::Key::Type::STRING);
  DALI_TEST_EQUALS(expectPropertyName, index.GetPropertyId().stringKey, TEST_LOCATION);

  expectPropertyIndex = 100u;
  index.SetPropertyId(expectPropertyIndex);
  DALI_TEST_CHECK(index.GetPropertyId().type == Property::Key::Type::INDEX);
  DALI_TEST_EQUALS(expectPropertyIndex, index.GetPropertyId().indexKey, TEST_LOCATION);

  END_TEST;
}