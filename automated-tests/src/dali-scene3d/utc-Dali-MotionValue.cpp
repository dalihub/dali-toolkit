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

#include <dali-scene3d/public-api/model-motion/motion-value.h>
#include <dali/devel-api/animation/key-frames-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Scene3D;

void model_motion_motion_value_startup(void)
{
  test_return_value = TET_UNDEF;
}

void model_motion_motion_value_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
} // namespace

// Positive test case for a method
int UtcDaliMotionValueNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliMotionValueNew");

  MotionValue motionValue = MotionValue::New();
  DALI_TEST_CHECK(motionValue);
  END_TEST;
}

// Positive test case for a method
int UtcDaliMotionValueDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliMotionValueDownCast");

  MotionValue motionValue = MotionValue::New();
  BaseHandle  handle(motionValue);

  MotionValue motionValue2 = MotionValue::DownCast(handle);
  DALI_TEST_CHECK(motionValue);
  DALI_TEST_CHECK(motionValue2);
  DALI_TEST_CHECK(motionValue2 == motionValue);
  END_TEST;
}

int UtcDaliMotionValueTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK(typeRegistry);

  TypeInfo typeInfo = typeRegistry.GetTypeInfo("MotionValue");
  DALI_TEST_CHECK(typeInfo);

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK(handle);

  MotionValue motionValue = MotionValue::DownCast(handle);
  DALI_TEST_CHECK(motionValue);

  END_TEST;
}

int UtcDaliMotionValueCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  MotionValue motionValue = MotionValue::New(1);
  DALI_TEST_CHECK(motionValue);
  DALI_TEST_CHECK(motionValue.GetValueType() == MotionValue::ValueType::PROPERTY_VALUE);

  MotionValue copy(motionValue);
  DALI_TEST_CHECK(motionValue == copy);
  DALI_TEST_CHECK(copy.GetValueType() == MotionValue::ValueType::PROPERTY_VALUE);

  MotionValue assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == motionValue);
  DALI_TEST_CHECK(assign.GetValueType() == MotionValue::ValueType::PROPERTY_VALUE);

  END_TEST;
}

int UtcDaliMotionValueMoveConstructor(void)
{
  ToolkitTestApplication application;

  MotionValue motionValue = MotionValue::New(3.0f);
  DALI_TEST_EQUALS(1, motionValue.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(motionValue.GetValueType() == MotionValue::ValueType::PROPERTY_VALUE);

  MotionValue moved = std::move(motionValue);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(moved.GetValueType() == MotionValue::ValueType::PROPERTY_VALUE);
  DALI_TEST_CHECK(!motionValue);

  END_TEST;
}

int UtcDaliMotionValueMoveAssignment(void)
{
  ToolkitTestApplication application;

  MotionValue motionValue = MotionValue::New(KeyFrames::New());
  DALI_TEST_EQUALS(1, motionValue.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(motionValue.GetValueType() == MotionValue::ValueType::KEY_FRAMES);

  MotionValue moved;
  moved = std::move(motionValue);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(moved.GetValueType() == MotionValue::ValueType::KEY_FRAMES);

  END_TEST;
}

// Method test

int UtcDaliMotionValueGetSetValue(void)
{
  ToolkitTestApplication application;

  MotionValue motionValue = MotionValue::New();
  DALI_TEST_CHECK(motionValue.GetValueType() == MotionValue::ValueType::INVALID);
  DALI_TEST_CHECK(motionValue.GetPropertyValue().GetType() == Property::Type::NONE);
  DALI_TEST_CHECK(!motionValue.GetKeyFrames());

  float expectValue = 3.0f;
  motionValue.SetValue(expectValue);
  DALI_TEST_CHECK(motionValue.GetValueType() == MotionValue::ValueType::PROPERTY_VALUE);
  DALI_TEST_CHECK(motionValue.GetPropertyValue().GetType() == Property::Type::FLOAT);
  DALI_TEST_CHECK(motionValue.GetPropertyValue().Get<float>() == expectValue);

  // Check the converted KeyFrames
  auto convertedKeyFrames = motionValue.GetKeyFrames();
  DALI_TEST_CHECK(convertedKeyFrames);
  auto convertedKeyFrameCount = DevelKeyFrames::GetKeyFrameCount(convertedKeyFrames);
  for(auto i = 0u; i < convertedKeyFrameCount; ++i)
  {
    float           progress;
    Property::Value value;
    DevelKeyFrames::GetKeyFrame(convertedKeyFrames, i, progress, value);

    // Check all value has same as expect property.
    DALI_TEST_EQUALS(value.Get<float>(), expectValue, TEST_LOCATION);
  }

  KeyFrames expectKeyFrames = KeyFrames::New();
  expectValue               = 2.0f;
  expectKeyFrames.Add(0.0f, expectValue - 1.0f);
  expectKeyFrames.Add(1.0f, expectValue);
  motionValue.SetValue(expectKeyFrames);

  DALI_TEST_CHECK(motionValue.GetValueType() == MotionValue::ValueType::KEY_FRAMES);
  DALI_TEST_CHECK(motionValue.GetKeyFrames() == expectKeyFrames);

  // Check property value return last frame value.
  DALI_TEST_CHECK(motionValue.GetPropertyValue().GetType() == Property::Type::FLOAT);
  DALI_TEST_EQUALS(motionValue.GetPropertyValue().Get<float>(), expectValue, TEST_LOCATION);

  // Check invalidate value.
  motionValue.Invalidate();
  DALI_TEST_CHECK(motionValue.GetValueType() == MotionValue::ValueType::INVALID);
  DALI_TEST_CHECK(motionValue.GetPropertyValue().GetType() == Property::Type::NONE);
  DALI_TEST_CHECK(!motionValue.GetKeyFrames());
  
  motionValue.SetValue(expectKeyFrames);
  DALI_TEST_CHECK(motionValue.GetValueType() == MotionValue::ValueType::KEY_FRAMES);
  DALI_TEST_CHECK(motionValue.GetKeyFrames() == expectKeyFrames);

  // Check invalidate value by Clear().
  motionValue.Clear();
  DALI_TEST_CHECK(motionValue.GetValueType() == MotionValue::ValueType::INVALID);
  DALI_TEST_CHECK(motionValue.GetPropertyValue().GetType() == Property::Type::NONE);
  DALI_TEST_CHECK(!motionValue.GetKeyFrames());

  END_TEST;
}