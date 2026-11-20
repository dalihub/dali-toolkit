/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <type_traits>
#include <utility>

// INTERNAL INCLUDES
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-task.h>

using Task = Dali::Toolkit::Internal::VectorAnimationTask;

static_assert(std::is_nothrow_move_constructible_v<Task::AnimationData>);
static_assert(std::is_nothrow_move_assignable_v<Task::AnimationData>);

void utc_dali_toolkit_vector_animation_data_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_vector_animation_data_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliToolkitVectorAnimationDataCopyMove(void)
{
  Task::AnimationData source;
  source.resendFlag = Task::RESEND_SIZE;
  source.width      = 123u;
  source.dynamicProperties.push_back({1, "first", 2, nullptr});

  Task::AnimationData copy(source);
  DALI_TEST_EQUALS(copy.width, 123u, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.dynamicProperties.size(), 1u, TEST_LOCATION);

  Task::AnimationData copyAssigned;
  copyAssigned.resendFlag = Task::RESEND_PLAY_STATE;
  copyAssigned.dynamicProperties.push_back({0, "existing", 0, nullptr});
  copyAssigned = source;
  DALI_TEST_EQUALS(copyAssigned.resendFlag, Task::RESEND_SIZE, TEST_LOCATION);
  DALI_TEST_EQUALS(copyAssigned.dynamicProperties.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(copyAssigned.dynamicProperties[0].id, 1, TEST_LOCATION);

  Task::AnimationData moved(std::move(copy));
  DALI_TEST_EQUALS(moved.width, 123u, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.dynamicProperties.size(), 1u, TEST_LOCATION);

  Task::AnimationData moveAssigned;
  moveAssigned.resendFlag = Task::RESEND_PLAY_STATE;
  moveAssigned.dynamicProperties.push_back({0, "existing", 0, nullptr});
  DALI_TEST_CHECK(&(moveAssigned = std::move(moved)) == &moveAssigned);
  DALI_TEST_EQUALS(moveAssigned.resendFlag, Task::RESEND_SIZE, TEST_LOCATION);
  DALI_TEST_EQUALS(moveAssigned.dynamicProperties.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(moveAssigned.dynamicProperties[0].id, 1, TEST_LOCATION);
  END_TEST;
}
