/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

void utc_dali_shader_generator_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_shader_generator_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliShaderGenerator(void)
{
  tet_infoline("All tests run as part of cmake build");
  DALI_TEST_CHECK(true);
  END_TEST;
}
