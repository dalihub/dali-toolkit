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

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include <toolkit-event-thread-callback.h>
#include <dali-toolkit-test-suite-utils.h>
#include <string_view>
#include <dali-scene3d/internal/common/environment-map-load-task.h>

using namespace Dali;
using namespace Dali::Scene3D::Internal;

typedef IntrusivePtr<EnvironmentMapLoadTask> EnvironmentMapLoadTaskPtr;

bool called = false;
void LoadComplete()
{
  called = true;
}

int UtcDaliEnvironmentMapTaskSuccess01(void)
{
  ToolkitTestApplication    application;
  EnvironmentMapLoadTaskPtr environmentMapLoadTask;

  auto path              = TEST_RESOURCE_DIR "/forest_radiance.ktx";
  environmentMapLoadTask = new EnvironmentMapLoadTask(path, Dali::Scene3D::EnvironmentMapType::AUTO, Dali::MakeCallback(LoadComplete));
  Dali::AsyncTaskManager::Get().AddTask(environmentMapLoadTask);

  DALI_TEST_CHECK(!called);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  DALI_TEST_CHECK(called);
  DALI_TEST_CHECK(environmentMapLoadTask->HasSucceeded());
  DALI_TEST_CHECK(environmentMapLoadTask->GetLoadedTexture());
  DALI_TEST_EQUALS(Dali::Scene3D::EnvironmentMapType::CUBEMAP, environmentMapLoadTask->GetEnvironmentMapType(), TEST_LOCATION);

  environmentMapLoadTask.Reset();
  END_TEST;
}
