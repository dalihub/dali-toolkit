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

#include "dali-scene-loader/public-api/string-callback.h"
#include <dali-test-suite-utils.h>
#include <string_view>

using namespace Dali;
using namespace Dali::SceneLoader;

namespace
{

char sBuffer[64];

void TestLogFunction(Integration::Log::DebugPriority level, std::string& str)
{
  snprintf(sBuffer, sizeof(sBuffer), "%d: %s", level, str.c_str());
}

}

int UtcDaliUtilsDefaultStringCallback(void)
{
  InstallLogFunction(TestLogFunction);
  DefaultErrorCallback("Hello world!");
  DALI_TEST_EQUAL(std::string(sBuffer), "2: string-callback.cpp: DefaultErrorCallback(26) > Hello world!");
  END_TEST;
}
