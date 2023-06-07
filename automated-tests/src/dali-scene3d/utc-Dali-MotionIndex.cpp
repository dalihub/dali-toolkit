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

#include <dali-scene3d/public-api/model-motion/motion-index/blend-shape-index.h>
#include <dali-scene3d/public-api/model-motion/motion-index/motion-index.h>
#include <dali-scene3d/public-api/model-motion/motion-index/motion-property-index.h>
#include <dali-scene3d/public-api/model-motion/motion-index/motion-transform-index.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Scene3D;

void model_motion_motion_index_startup(void)
{
  test_return_value = TET_UNDEF;
}

void model_motion_motion_index_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
} // namespace

// Positive test case for a method
int UtcDaliMotionIndexDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliMotionIndexDownCast");

  MotionIndex blendShapeIndex = BlendShapeIndex::New();
  BaseHandle  handle(blendShapeIndex);

  MotionIndex blendShapeIndex2 = MotionIndex::DownCast(handle);
  DALI_TEST_CHECK(blendShapeIndex);
  DALI_TEST_CHECK(blendShapeIndex2);
  DALI_TEST_CHECK(blendShapeIndex2 == blendShapeIndex);
  END_TEST;
}

int UtcDaliMotionIndexDownCastNotMatchedType(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliMotionIndexDownCastNotMatchedType");

  {
    MotionIndex baseIndex = BlendShapeIndex::New();
    DALI_TEST_CHECK(baseIndex);

    BaseHandle handle(baseIndex);

    MotionIndex motionPropertyIndex = MotionPropertyIndex::DownCast(handle);
    DALI_TEST_CHECK(!motionPropertyIndex);

    MotionIndex motionTransformIndex = MotionTransformIndex::DownCast(handle);
    DALI_TEST_CHECK(!motionTransformIndex);

    MotionIndex blendShapeIndex = BlendShapeIndex::DownCast(handle);
    DALI_TEST_CHECK(blendShapeIndex);
    DALI_TEST_CHECK(blendShapeIndex == baseIndex);
  }
  END_TEST;
}

int UtcDaliMotionIndexCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  MotionIndex blendShapeIndex = BlendShapeIndex::New();
  DALI_TEST_CHECK(blendShapeIndex);

  MotionIndex copy(blendShapeIndex);
  DALI_TEST_CHECK(blendShapeIndex == copy);

  MotionIndex assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == blendShapeIndex);

  END_TEST;
}

int UtcDaliMotionIndexMoveConstructor(void)
{
  ToolkitTestApplication application;

  MotionIndex blendShapeIndex = MotionTransformIndex::New();
  DALI_TEST_EQUALS(1, blendShapeIndex.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  MotionIndex moved = std::move(blendShapeIndex);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!blendShapeIndex);

  END_TEST;
}

int UtcDaliMotionIndexMoveAssignment(void)
{
  ToolkitTestApplication application;

  MotionIndex blendShapeIndex = BlendShapeIndex::New();
  DALI_TEST_EQUALS(1, blendShapeIndex.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  MotionIndex moved;
  moved = std::move(blendShapeIndex);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

// Method test

int UtcDaliMotionIndexSetGetModelNodeId(void)
{
  std::string expectModelNodeStringId = "nodeId";
  MotionIndex index                   = MotionTransformIndex::New(expectModelNodeStringId, MotionTransformIndex::TransformType::POSITION);

  auto propertyKey = index.GetModelNodeId();
  DALI_TEST_CHECK(propertyKey.type == Property::Key::Type::STRING);
  DALI_TEST_EQUALS(expectModelNodeStringId, propertyKey.stringKey, TEST_LOCATION);

  expectModelNodeStringId = "anotherId";
  index.SetModelNodeId(expectModelNodeStringId);

  propertyKey = index.GetModelNodeId();
  DALI_TEST_CHECK(propertyKey.type == Property::Key::Type::STRING);
  DALI_TEST_EQUALS(expectModelNodeStringId, propertyKey.stringKey, TEST_LOCATION);

  Property::Index expectModelNodeIndexId = 193u;
  index.SetModelNodeId(expectModelNodeIndexId);

  propertyKey = index.GetModelNodeId();
  DALI_TEST_CHECK(propertyKey.type == Property::Key::Type::INDEX);
  DALI_TEST_EQUALS(expectModelNodeIndexId, propertyKey.indexKey, TEST_LOCATION);

  END_TEST;
}