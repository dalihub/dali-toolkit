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
#include <fstream>
#include <iostream>

#include <toolkit-event-thread-callback.h>

#include <dali-scene3d/public-api/model-motion/motion-data.h>
#include <dali-scene3d/public-api/model-motion/motion-value.h>

#include <dali-scene3d/public-api/model-motion/motion-index/blend-shape-index.h>
#include <dali-scene3d/public-api/model-motion/motion-index/motion-index.h>
#include <dali-scene3d/public-api/model-motion/motion-index/motion-transform-index.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Scene3D;

void model_motion_motion_data_startup(void)
{
  test_return_value = TET_UNDEF;
}

void model_motion_motion_data_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* TEST_BVH_FILE_NAME    = TEST_RESOURCE_DIR "/test.bvh";
const char* TEST_FACIAL_FILE_NAME = TEST_RESOURCE_DIR "/facial-blendshape-animation.json";

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

// For LoadCompleted
static bool gLoadCompleted = false;
void        OnLoadCompleted(MotionData data)
{
  gLoadCompleted = true;
}
} // namespace

// Positive test case for a method
int UtcDaliMotionDataNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliMotionDataNew");

  float      expectDuration = 0.0f;
  MotionData motionData     = MotionData::New();
  DALI_TEST_CHECK(motionData);
  DALI_TEST_EQUALS(motionData.GetDuration(), expectDuration, TEST_LOCATION);

  expectDuration = 10.0f;
  motionData     = MotionData::New(expectDuration);
  DALI_TEST_CHECK(motionData);
  DALI_TEST_EQUALS(motionData.GetDuration(), expectDuration, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMotionDataDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliMotionDataDownCast");

  MotionData motionData = MotionData::New();
  BaseHandle handle(motionData);

  MotionData motionData2 = MotionData::DownCast(handle);
  DALI_TEST_CHECK(motionData);
  DALI_TEST_CHECK(motionData2);
  DALI_TEST_CHECK(motionData2 == motionData);
  END_TEST;
}

int UtcDaliMotionDataTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK(typeRegistry);

  TypeInfo typeInfo = typeRegistry.GetTypeInfo("MotionData");
  DALI_TEST_CHECK(typeInfo);

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK(handle);

  MotionData motionData = MotionData::DownCast(handle);
  DALI_TEST_CHECK(motionData);

  END_TEST;
}

int UtcDaliMotionDataCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  MotionData motionData = MotionData::New();
  DALI_TEST_CHECK(motionData);

  MotionData copy(motionData);
  DALI_TEST_CHECK(motionData == copy);

  MotionData assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == motionData);

  END_TEST;
}

int UtcDaliMotionDataMoveConstructor(void)
{
  ToolkitTestApplication application;

  MotionData motionData = MotionData::New();
  DALI_TEST_EQUALS(1, motionData.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  MotionData moved = std::move(motionData);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!motionData);

  END_TEST;
}

int UtcDaliMotionDataMoveAssignment(void)
{
  ToolkitTestApplication application;

  MotionData motionData = MotionData::New();
  DALI_TEST_EQUALS(1, motionData.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  MotionData moved;
  moved = std::move(motionData);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

// Method test

int UtcDaliMotionDataAddMotion(void)
{
  ToolkitTestApplication application;

  MotionData motionData = MotionData::New(3.0f);

  const uint32_t countMax = 4;
  MotionIndex    index[countMax];
  MotionValue    value[countMax];
  for(uint32_t i = 0u; i < countMax; ++i)
  {
    // Generate index
    if(i & 1)
    {
      index[i] = BlendShapeIndex::New("node", 0);
    }
    else
    {
      index[i] = MotionTransformIndex::New("node", MotionTransformIndex::TransformType::POSITION_X);
    }

    // Generate value
    if(i & 2)
    {
      value[i] = MotionValue::New(3.0f);
    }
    else
    {
      KeyFrames keyFrames = KeyFrames::New();
      keyFrames.Add(0.0f, 9.0f);
      keyFrames.Add(1.0f, 5.0f);
      value[i] = MotionValue::New(keyFrames);
    }

    DALI_TEST_CHECK(index[i]);
    DALI_TEST_CHECK(value[i]);
    motionData.Add(index[i], value[i]);

    DALI_TEST_EQUALS(i + 1, motionData.GetMotionCount(), TEST_LOCATION);
  }

  for(uint32_t i = 0u; i < countMax; ++i)
  {
    DALI_TEST_EQUALS(index[i], motionData.GetIndex(i), TEST_LOCATION);
    DALI_TEST_EQUALS(value[i], motionData.GetValue(i), TEST_LOCATION);
  }

  DALI_TEST_CHECK(!motionData.GetIndex(countMax));
  DALI_TEST_CHECK(!motionData.GetValue(countMax));

  motionData.Clear();

  DALI_TEST_EQUALS(0u, motionData.GetMotionCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!motionData.GetIndex(0u));
  DALI_TEST_CHECK(!motionData.GetValue(0u));

  END_TEST;
}

int UtcDaliMotionDataSetGetDuration(void)
{
  ToolkitTestApplication application;

  float      expectDuration = 3.0f;
  MotionData motionData     = MotionData::New(expectDuration);
  DALI_TEST_EQUALS(motionData.GetDuration(), expectDuration, TEST_LOCATION);

  expectDuration = 7.0f;
  motionData.SetDuration(expectDuration);
  DALI_TEST_EQUALS(motionData.GetDuration(), expectDuration, TEST_LOCATION);

  expectDuration = 1.0f;
  motionData.SetDuration(expectDuration);
  DALI_TEST_EQUALS(motionData.GetDuration(), expectDuration, TEST_LOCATION);

  END_TEST;
}

int UtcDaliMotionDataLoadBvhAndFacialAsync(void)
{
  ToolkitTestApplication application;

  for(uint32_t tc = 0; tc < 4; ++tc)
  {
    MotionData motionData = MotionData::New();
    gLoadCompleted        = false;
    motionData.LoadCompletedSignal().Connect(&OnLoadCompleted);

    switch(tc)
    {
      case 0:
      default:
      {
        motionData.LoadBvh(TEST_BVH_FILE_NAME);
        break;
      }
      case 1:
      {
        motionData.LoadFacialAnimation(TEST_FACIAL_FILE_NAME);
        break;
      }
      case 2:
      {
        std::string rawString = ReadBufferFromFile(TEST_BVH_FILE_NAME);
        motionData.LoadBvhFromBuffer(reinterpret_cast<uint8_t*>(rawString.data()), static_cast<int>(rawString.length()));
        break;
      }
      case 3:
      {
        std::string rawString = ReadBufferFromFile(TEST_FACIAL_FILE_NAME);
        motionData.LoadFacialAnimationFromBuffer(reinterpret_cast<uint8_t*>(rawString.data()), static_cast<int>(rawString.length()));
        break;
      }
    }

    DALI_TEST_EQUALS(gLoadCompleted, false, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
    application.SendNotification();
    application.Render();

    // Check LoadCompletedSignal emitted.
    DALI_TEST_EQUALS(gLoadCompleted, true, TEST_LOCATION);

    // Check MotionData load successfully.
    DALI_TEST_GREATER(motionData.GetMotionCount(), 0u, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliMotionDataLoadBvhAndFacialSync(void)
{
  ToolkitTestApplication application;

  for(uint32_t tc = 0; tc < 4; ++tc)
  {
    MotionData motionData = MotionData::New();
    gLoadCompleted        = false;
    motionData.LoadCompletedSignal().Connect(&OnLoadCompleted);

    switch(tc)
    {
      case 0:
      default:
      {
        motionData.LoadBvh(TEST_BVH_FILE_NAME, Vector3::ONE, true);
        break;
      }
      case 1:
      {
        motionData.LoadFacialAnimation(TEST_FACIAL_FILE_NAME, true);
        break;
      }
      case 2:
      {
        std::string rawString = ReadBufferFromFile(TEST_BVH_FILE_NAME);
        motionData.LoadBvhFromBuffer(reinterpret_cast<uint8_t*>(rawString.data()), static_cast<int>(rawString.length()), Vector3::ONE, true);
        break;
      }
      case 3:
      {
        std::string rawString = ReadBufferFromFile(TEST_FACIAL_FILE_NAME);
        motionData.LoadFacialAnimationFromBuffer(reinterpret_cast<uint8_t*>(rawString.data()), static_cast<int>(rawString.length()), true);
        break;
      }
    }

    // Check LoadCompletedSignal emitted.
    DALI_TEST_EQUALS(gLoadCompleted, true, TEST_LOCATION);

    // Check MotionData load successfully.
    DALI_TEST_GREATER(motionData.GetMotionCount(), 0u, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliMotionDataLoadAsyncMultiple(void)
{
  ToolkitTestApplication application;

  MotionData motionData = MotionData::New();
  gLoadCompleted        = false;
  motionData.LoadCompletedSignal().Connect(&OnLoadCompleted);

  auto tryCount = 10u;
  for(auto i = 0u; i < tryCount; ++i)
  {
    motionData.LoadBvh(TEST_BVH_FILE_NAME);
  }

  DALI_TEST_EQUALS(gLoadCompleted, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gLoadCompleted, true, TEST_LOCATION);

  // Check MotionData load successfully.
  DALI_TEST_GREATER(motionData.GetMotionCount(), 0u, TEST_LOCATION);

  // Check if completed request comes only 1 time.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 1), false, TEST_LOCATION);

  END_TEST;
}