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

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Scene3D;

void model_motion_blend_shape_index_startup(void)
{
  test_return_value = TET_UNDEF;
}

void model_motion_blend_shape_index_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* const WEIGHTS_UNIFORM = "uBlendShapeWeight";

std::string GetPropertyNameFromIndex(int index)
{
  if(index >= 0)
  {
    char weightNameBuffer[32];

    // Get return value of snprintf to avoid SVACE.
    [[maybe_unused]] auto prefixSize = snprintf(weightNameBuffer, sizeof(weightNameBuffer), "%s[%d]", WEIGHTS_UNIFORM, index);

    return weightNameBuffer;
  }
  return "";
}
} // namespace

// Positive test case for a method
int UtcDaliBlendShapeIndexNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliBlendShapeIndexNew");

  BlendShapeIndex blendShapeIndex = BlendShapeIndex::New();
  DALI_TEST_CHECK(blendShapeIndex);
  DALI_TEST_EQUALS(blendShapeIndex.GetModelNodeId().stringKey, "", TEST_LOCATION);
  DALI_TEST_EQUALS(blendShapeIndex.GetBlendShapeId().indexKey, Property::INVALID_INDEX, TEST_LOCATION);

  blendShapeIndex = BlendShapeIndex::New("dummy", 3u);
  DALI_TEST_CHECK(blendShapeIndex);
  DALI_TEST_EQUALS(blendShapeIndex.GetModelNodeId().stringKey, "dummy", TEST_LOCATION);
  DALI_TEST_EQUALS(blendShapeIndex.GetBlendShapeId().indexKey, 3u, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliBlendShapeIndexDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliBlendShapeIndexDownCast");

  BlendShapeIndex blendShapeIndex = BlendShapeIndex::New();
  BaseHandle      handle(blendShapeIndex);

  BlendShapeIndex blendShapeIndex2 = BlendShapeIndex::DownCast(handle);
  DALI_TEST_CHECK(blendShapeIndex);
  DALI_TEST_CHECK(blendShapeIndex2);
  DALI_TEST_CHECK(blendShapeIndex2 == blendShapeIndex);
  END_TEST;
}

int UtcDaliBlendShapeIndexTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK(typeRegistry);

  TypeInfo typeInfo = typeRegistry.GetTypeInfo("BlendShapeIndex");
  DALI_TEST_CHECK(typeInfo);

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK(handle);

  BlendShapeIndex blendShapeIndex = BlendShapeIndex::DownCast(handle);
  DALI_TEST_CHECK(blendShapeIndex);

  END_TEST;
}

int UtcDaliBlendShapeIndexCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  BlendShapeIndex blendShapeIndex = BlendShapeIndex::New();
  DALI_TEST_CHECK(blendShapeIndex);

  BlendShapeIndex copy(blendShapeIndex);
  DALI_TEST_CHECK(blendShapeIndex == copy);

  BlendShapeIndex assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == blendShapeIndex);

  END_TEST;
}

int UtcDaliBlendShapeIndexMoveConstructor(void)
{
  ToolkitTestApplication application;

  BlendShapeIndex blendShapeIndex = BlendShapeIndex::New();
  DALI_TEST_EQUALS(1, blendShapeIndex.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  BlendShapeIndex moved = std::move(blendShapeIndex);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!blendShapeIndex);

  END_TEST;
}

int UtcDaliBlendShapeIndexMoveAssignment(void)
{
  ToolkitTestApplication application;

  BlendShapeIndex blendShapeIndex = BlendShapeIndex::New();
  DALI_TEST_EQUALS(1, blendShapeIndex.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  BlendShapeIndex moved;
  moved = std::move(blendShapeIndex);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

// Method test

int UtcDaliBlendShapeIndexGetPropertyName(void)
{
  std::string     expectPropertyName  = "";
  Property::Index expectPropertyIndex = Property::INVALID_INDEX;

  uint32_t blendShapeIndex;

  MotionIndex index = BlendShapeIndex::New();

  // Invalid motion index return empty string, invalid index
  DALI_TEST_EQUALS(expectPropertyName, index.GetPropertyName(), TEST_LOCATION);
  DALI_TEST_EQUALS(expectPropertyIndex, index.GetPropertyIndex(), TEST_LOCATION);

  blendShapeIndex    = 0u;
  index              = BlendShapeIndex::New("", blendShapeIndex);
  expectPropertyName = GetPropertyNameFromIndex(blendShapeIndex);
  DALI_TEST_EQUALS(expectPropertyName, index.GetPropertyName(), TEST_LOCATION);

  blendShapeIndex    = 45u;
  index              = BlendShapeIndex::New("", blendShapeIndex);
  expectPropertyName = GetPropertyNameFromIndex(blendShapeIndex);
  DALI_TEST_EQUALS(expectPropertyName, index.GetPropertyName(), TEST_LOCATION);

  blendShapeIndex    = 109u;
  index              = BlendShapeIndex::New("", blendShapeIndex);
  expectPropertyName = GetPropertyNameFromIndex(blendShapeIndex);
  DALI_TEST_EQUALS(expectPropertyName, index.GetPropertyName(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliBlendShapeIndexSetGetBlendShapeId(void)
{
  Property::Index expectPropertyIndex = 0u;

  BlendShapeIndex index = BlendShapeIndex::New("", expectPropertyIndex);

  // Invalid motion index return empty string, invalid index
  DALI_TEST_CHECK(index.GetBlendShapeId().type == Property::Key::Type::INDEX);
  DALI_TEST_EQUALS(expectPropertyIndex, index.GetBlendShapeId().indexKey, TEST_LOCATION);

  std::string expectPropertyName = "hello";
  index.SetBlendShapeId(expectPropertyName);
  DALI_TEST_CHECK(index.GetBlendShapeId().type == Property::Key::Type::STRING);
  DALI_TEST_EQUALS(expectPropertyName, index.GetBlendShapeId().stringKey, TEST_LOCATION);

  expectPropertyIndex = 100u;
  index.SetBlendShapeId(expectPropertyIndex);
  DALI_TEST_CHECK(index.GetBlendShapeId().type == Property::Key::Type::INDEX);
  DALI_TEST_EQUALS(expectPropertyIndex, index.GetBlendShapeId().indexKey, TEST_LOCATION);

  END_TEST;
}