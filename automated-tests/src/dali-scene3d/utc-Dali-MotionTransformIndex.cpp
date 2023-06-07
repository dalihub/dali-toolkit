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
#include <dali-scene3d/public-api/model-motion/motion-index/motion-transform-index.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Scene3D;

void model_motion_motion_transform_index_startup(void)
{
  test_return_value = TET_UNDEF;
}

void model_motion_motion_transform_index_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
struct PropertyStringIndex
{
  const MotionTransformIndex::TransformType type;

  const char* const     name;
  const Property::Index index;
};
const PropertyStringIndex PROPERTY_TABLE[] =
  {
    {MotionTransformIndex::TransformType::POSITION, "position", Actor::Property::POSITION},
    {MotionTransformIndex::TransformType::POSITION_X, "positionX", Actor::Property::POSITION_X},
    {MotionTransformIndex::TransformType::POSITION_Y, "positionY", Actor::Property::POSITION_Y},
    {MotionTransformIndex::TransformType::POSITION_Z, "positionZ", Actor::Property::POSITION_Z},
    {MotionTransformIndex::TransformType::ORIENTATION, "orientation", Actor::Property::ORIENTATION},
    {MotionTransformIndex::TransformType::SCALE, "scale", Actor::Property::SCALE},
    {MotionTransformIndex::TransformType::SCALE_X, "scaleX", Actor::Property::SCALE_X},
    {MotionTransformIndex::TransformType::SCALE_Y, "scaleY", Actor::Property::SCALE_Y},
    {MotionTransformIndex::TransformType::SCALE_Z, "scaleZ", Actor::Property::SCALE_Z},
};
} // namespace

// Positive test case for a method
int UtcDaliMotionTransformIndexNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliMotionTransformIndexNew");

  MotionTransformIndex motionTransformIndex = MotionTransformIndex::New();
  DALI_TEST_CHECK(motionTransformIndex);
  DALI_TEST_EQUALS(motionTransformIndex.GetModelNodeId().stringKey, "", TEST_LOCATION);
  DALI_TEST_EQUALS(motionTransformIndex.GetTransformType(), MotionTransformIndex::TransformType::INVALID, TEST_LOCATION);

  motionTransformIndex = MotionTransformIndex::New("dummy", MotionTransformIndex::TransformType::SCALE);
  DALI_TEST_CHECK(motionTransformIndex);
  DALI_TEST_EQUALS(motionTransformIndex.GetModelNodeId().stringKey, "dummy", TEST_LOCATION);
  DALI_TEST_EQUALS(motionTransformIndex.GetTransformType(), MotionTransformIndex::TransformType::SCALE, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliMotionTransformIndexDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliMotionTransformIndexDownCast");

  MotionTransformIndex motionTransformIndex = MotionTransformIndex::New();
  BaseHandle           handle(motionTransformIndex);

  MotionTransformIndex motionTransformIndex2 = MotionTransformIndex::DownCast(handle);
  DALI_TEST_CHECK(motionTransformIndex);
  DALI_TEST_CHECK(motionTransformIndex2);
  DALI_TEST_CHECK(motionTransformIndex2 == motionTransformIndex);
  END_TEST;
}

int UtcDaliMotionTransformIndexTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK(typeRegistry);

  TypeInfo typeInfo = typeRegistry.GetTypeInfo("MotionTransformIndex");
  DALI_TEST_CHECK(typeInfo);

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK(handle);

  MotionTransformIndex motionTransformIndex = MotionTransformIndex::DownCast(handle);
  DALI_TEST_CHECK(motionTransformIndex);

  END_TEST;
}

int UtcDaliMotionTransformIndexCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  MotionTransformIndex motionTransformIndex = MotionTransformIndex::New();
  DALI_TEST_CHECK(motionTransformIndex);

  MotionTransformIndex copy(motionTransformIndex);
  DALI_TEST_CHECK(motionTransformIndex == copy);

  MotionTransformIndex assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == motionTransformIndex);

  END_TEST;
}

int UtcDaliMotionTransformIndexMoveConstructor(void)
{
  ToolkitTestApplication application;

  MotionTransformIndex motionTransformIndex = MotionTransformIndex::New();
  DALI_TEST_EQUALS(1, motionTransformIndex.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  MotionTransformIndex moved = std::move(motionTransformIndex);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!motionTransformIndex);

  END_TEST;
}

int UtcDaliMotionTransformIndexMoveAssignment(void)
{
  ToolkitTestApplication application;

  MotionTransformIndex motionTransformIndex = MotionTransformIndex::New();
  DALI_TEST_EQUALS(1, motionTransformIndex.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  MotionTransformIndex moved;
  moved = std::move(motionTransformIndex);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

// Method test

int UtcDaliMotionTransformIndexGetPropertyNameAndIndex(void)
{
  auto transfromTypeCount = sizeof(PROPERTY_TABLE) / sizeof(PROPERTY_TABLE[0]);
  for(auto i = 0u; i < transfromTypeCount; i++)
  {
    auto type = PROPERTY_TABLE[i].type;

    std::string     expectPropertyName  = PROPERTY_TABLE[i].name;
    Property::Index expectPropertyIndex = PROPERTY_TABLE[i].index;

    MotionIndex index = MotionTransformIndex::New("", type);

    DALI_TEST_EQUALS(expectPropertyName, index.GetPropertyName(), TEST_LOCATION);
    DALI_TEST_EQUALS(expectPropertyIndex, index.GetPropertyIndex(), TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliMotionTransformIndexGetPropertyInvalid(void)
{
  std::string     expectPropertyName  = "";
  Property::Index expectPropertyIndex = Property::INVALID_INDEX;

  MotionIndex index = MotionTransformIndex::New();

  // Invalid motion index return empty string, invalid index
  DALI_TEST_EQUALS(expectPropertyName, index.GetPropertyName(), TEST_LOCATION);
  DALI_TEST_EQUALS(expectPropertyIndex, index.GetPropertyIndex(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliMotionTransformIndexSetGetPropertType(void)
{
  auto                 expectType = MotionTransformIndex::TransformType::POSITION;
  MotionTransformIndex index      = MotionTransformIndex::New("", expectType);

  // Invalid motion index return empty string, invalid index
  DALI_TEST_EQUALS(expectType, index.GetTransformType(), TEST_LOCATION);

  expectType = MotionTransformIndex::TransformType::ORIENTATION;
  index.SetTransformType(expectType);
  DALI_TEST_EQUALS(expectType, index.GetTransformType(), TEST_LOCATION);

  END_TEST;
}